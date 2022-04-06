#define NOMINMAX

#include "LRMP.h"
#include "LRMP_SSL.h"
#include "ctrl_c_handler.h"
#include "robin_map.h"
#include <sstream>
#include <vector>
#include <atomic>
#include <deque>
#include <filesystem>
#include <functional>
#include <mutex>
#include <random>
#include <stdio.h>
#include <string>
#include <time.h>

typedef std::chrono::time_point<std::chrono::steady_clock> TimePoint;
typedef std::mersenne_twister_engine<std::uint_fast64_t, 64, 312, 156, 31,
    0xb5026f5aa96619e9, 29,
    0x5555555555555555, 17,
    0x71d67fffeda60000, 37,
    0xfff7eee000000000, 43, 6364136223846793005>
    RndGenType;
typedef RndGenType::result_type Cookie;

#if _WIN32 || _WIN64
template <>
struct std::hash<Cookie> {
    std::size_t operator()(Cookie const& s) const noexcept
    {
        return s;
    }
};
#endif

template <typename T>
static Cookie GenerateCookie(tsl::robin_map<Cookie, T>& map)
{
    static RndGenType* randGen = nullptr;
    if (!randGen) {
        randGen = new RndGenType((RndGenType::result_type)time(0));
    }
start:
    Cookie id = (*randGen)();
    uint8_t* testPtr = (uint8_t*)&id;
    for (unsigned int i = 0, j = 0; i < sizeof(Cookie); ++i) {
        if (!testPtr[i]) {
            j++;
        }
        if (j > 4) {
            goto start;
        }
    }
    if (map.find(id) != map.end()) {
        goto start;
    }
    return id;
}
static Cookie GenerateCookie()
{
    static RndGenType* randGen = nullptr;
    if (!randGen) {
        randGen = new RndGenType((RndGenType::result_type)time(0));
    }
    Cookie id = (*randGen)();
    return id;
}
static uint32_t GenerateLiveId()
{
    static uint32_t liveId = 0;
    if (!liveId) {
        liveId = 1;
    }
    return liveId++;
}

struct User {
    User() = default;

    std::string username;
    std::string fingerprint; // Debug only
    Cookie vrfyCookie[2] {};
    std::atomic<Cookie> cookie = 0;
    std::atomic<uint32_t> port = 0;
    std::atomic<uint32_t> routeId = 0;

    std::atomic<uint16_t> livePort = 0;
    std::atomic<uint16_t> liveIndex = 0;
    std::atomic<uint32_t> liveId = 0;

    inline bool CheckCookie(nng_msg* msg)
    {
        Cookie c;
        LRMPMsgDump(msg, 0, &c);
        return c == cookie;
    }
    inline bool CheckRoute(nng_msg* msg, uint32_t currentPort)
    {
        Cookie c;
        uint32_t r = LRMPMsgGetRoute(msg);
        LRMPMsgDump(msg, 0, &c);
        return (c == cookie) && (currentPort == port) && (r = routeId);
    }
};

struct LiveCtx {
#define STREAM_COUNT 8
    User* user;
    TimePoint refTimePoint;
    std::string dataDir;
    struct Signal {
        uint32_t size;
        uint32_t offset;
        double time;
    };
    struct BinaryStream {
        FILE* file = nullptr;
        char fileBuf[BUFSIZ] {};
        uint32_t fileSize = 0;
    };
    BinaryStream binaryStreams[STREAM_COUNT] {};

    LiveCtx(User* u, uint16_t port, uint16_t index, std::string rootDir = "streams")
        : user(u)
    {
        user->liveId = GenerateLiveId();
        user->livePort = port;
        user->liveIndex = index;
        refTimePoint = std::chrono::steady_clock::now();
        dataDir = rootDir;
        std::filesystem::create_directory(dataDir);

        time_t time;
        tm timeGm;
        char timeStr[128];
        std::time(&time);
#if _WIN32 || _WIN64
        gmtime_s(&timeGm, &time);
#elif __GNUC__
        gmtime_r(&time, &timeGm);
#else
#error Unsupported compiler
#endif
        strftime(timeStr, 128, "-%Y-%m-%d-%H-%M-%S", &timeGm);

        dataDir += '/';
        dataDir += user->username;
        dataDir += timeStr;
        std::filesystem::create_directory(dataDir);
    }
    ~LiveCtx()
    {
        user->livePort = 0;
        user->liveIndex = 0;
        for (auto& stream : binaryStreams) {
            if (stream.file) {
                fclose(stream.file);
            }
        }
    }

    bool InitStreamFile(BinaryStream& stream, int id)
    {
        if (!stream.file) {
            char fileNameBuf[128];
            sprintf(fileNameBuf, "%s/%d.stream", dataDir.c_str(), id);
            stream.file = fopen(fileNameBuf, "w+b");
            if (!stream.file) {
                return false;
            }
            setvbuf(stream.file, stream.fileBuf, _IOFBF, BUFSIZ);
        }
        return true;
    }
    bool InitStream(uint16_t streamId)
    {
        if (streamId >= STREAM_COUNT) {
            return false;
        }
        if (!InitStreamFile(binaryStreams[streamId], streamId)) {
            return false;
        }
        return true;
    }
    inline bool CheckCred(nng_msg* msg)
    {
        uint64_t cookie;
        uint32_t route = LRMPMsgGetRoute(msg);
        LRMPMsgDump(msg, 0, &cookie);
        return user->cookie == cookie && user->routeId == route;
    }
    void Push(nng_msg* msg, bool withTime = false)
    {
        if (!CheckCred(msg)) {
            LRMPMsgReset(msg, ServerCmd::REP_ERR_INVALID_CRED);
            return;
        }

        uint16_t streamId;
        LRMPMsgDump(msg, 14, &streamId);
        if (streamId >= STREAM_COUNT) {
            LRMPMsgReset(msg, ServerCmd::REP_ERR_WRITE);
            return;
        }

        auto& stream = binaryStreams[streamId];
        if (!stream.file) {
            if (!InitStream(streamId)) {
                LRMPMsgReset(msg, ServerCmd::REP_ERR_WRITE);
                return;
            }
        }

        uint32_t cursorConfirm;
        LRMPMsgDump(msg, 16, &cursorConfirm);
        if (cursorConfirm) {
            fseek(stream.file, 0, SEEK_END);
            uint32_t cursor = (uint32_t)ftell(stream.file);
            if (cursor != cursorConfirm) {
                LRMPMsgReset(msg, ServerCmd::REP_ERR_CURSOR_MISMATCH);
                return;
            }
        }

        auto size = (uint32_t)LRMPMsgBodyLen(msg) - 20u;
        auto data = LRMPMsgBodyPtr(msg, 20u);
        fseek(stream.file, 0, SEEK_END);
        if (withTime) {
            double time = std::chrono::duration<double>(std::chrono::steady_clock::now() - refTimePoint).count();
            fwrite(&time, 8u, 1u, stream.file);
        }
        if (size) {
            fwrite(data, size, 1u, stream.file);
        }
        stream.fileSize = (uint32_t)ftell(stream.file);

        LRMPMsgReset(msg, ServerCmd::REP_OK);
    }
    void Pull(nng_msg* msg, bool withSize = false)
    {
        uint16_t streamId;
        LRMPMsgDump(msg, 6, &streamId);
        if (streamId >= STREAM_COUNT) {
            LRMPMsgReset(msg, ServerCmd::REP_ERR_WRITE);
            return;
        }

        auto& stream = binaryStreams[streamId];
        if (!stream.file) {
            LRMPMsgReset(msg, ServerCmd::REP_ERR_READ);
            return;
        }

        uint32_t pos;
        LRMPMsgDump(msg, 8, &pos);
        if (stream.fileSize <= pos) {
            LRMPMsgReset(msg, ServerCmd::REP_ERR_READ);
            return;
        }
        uint32_t outSize = std::min(480u, stream.fileSize - pos);
        if (withSize) {
            uint32_t fragSize;
            LRMPMsgDump(msg, 12, &fragSize);
            outSize /= fragSize;
            if (!outSize) {
                LRMPMsgReset(msg, ServerCmd::REP_ERR_READ);
                return;
            }
            outSize *= fragSize;
        }

        LRMPMsgReset(msg, ServerCmd::REP_OK);
        LRMPMsgPad(msg, outSize + 4);
        uint32_t* outBuf = (uint32_t*)LRMPMsgBodyPtr(msg);
        fseek(stream.file, pos, SEEK_SET);
        auto sizeRead = fread(&(outBuf[1]), outSize, 1u, stream.file);
        if (sizeRead != 1) {
            LRMPMsgReset(msg, ServerCmd::REP_ERR_READ);
            return;
        }
        outBuf[0] = (uint32_t)ftell(stream.file);
    }
};

class LiveCtxPtr
{
public:
    std::shared_ptr<LiveCtx>& raw()
    {
        return m_ptr;
    }
    std::shared_ptr<LiveCtx> safe_get()
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_ptr;
    }
    template<typename T>
    void safe_assign(const T& ptr)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_ptr = ptr;
    }
    template <typename T>
    void safe_assign(T&& ptr)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_ptr = ptr;
    }

private:
    std::shared_ptr<LiveCtx> m_ptr;
    std::mutex m_mutex;

};

struct WorkerCtx {
    std::string workerFrontendURL;
    std::string workerBackendURL;
    size_t workerLoad = 0;
    size_t workerLoadMax = 64;
    std::unique_ptr<std::thread> workerThread;
    std::atomic<bool> workerTermination;
    LiveCtxPtr* liveVec;

    WorkerCtx() = delete;
    WorkerCtx(std::string frontendURL, std::string backendURL)
        : workerThread(std::make_unique<std::thread>(WorkerThreadFunc, this))
    {
        workerFrontendURL = frontendURL;
        workerBackendURL = backendURL;
        liveVec = new LiveCtxPtr[workerLoadMax];
        workerTermination = false;
    }
    ~WorkerCtx()
    {
        workerTermination = true;
        workerThread->join();
        delete[] liveVec;
    }

    inline std::shared_ptr<LiveCtx> GetLiveW(nng_msg* msg)
    {
        uint32_t liveId;
        uint16_t liveIndex;
        Cookie cookie;
        if (LRMPMsgBodyLen(msg) >= 14) {
            LRMPMsgDump(msg, 0, &cookie, &liveId, &liveIndex);
            if (liveIndex && liveIndex <= workerLoadMax) {
                auto live = liveVec[liveIndex - 1].safe_get();
                if (live && live->user->liveId == liveId) {
                    return live;
                }
            }
            LRMPMsgReset(msg, ServerCmd::REP_ERR_INVALID_LIVE);
        } else {
            LRMPMsgReset(msg, ServerCmd::REP_ERR_INVALID_MSG);
        }
        return nullptr;
    }
    inline std::shared_ptr<LiveCtx> GetLiveR(nng_msg* msg)
    {
        uint32_t liveId;
        uint16_t liveIndex;
        if (LRMPMsgBodyLen(msg) >= 6) {
            LRMPMsgDump(msg, 0, &liveId, &liveIndex);
            if (liveIndex && liveIndex <= workerLoadMax) {
                auto live = liveVec[liveIndex - 1].safe_get();
                if (live && live->user->liveId == liveId) {
                    return live;
                }
            }
            LRMPMsgReset(msg, ServerCmd::REP_ERR_INVALID_LIVE);
        } else {
            LRMPMsgReset(msg, ServerCmd::REP_ERR_INVALID_MSG);
        }
        return nullptr;
    }
    static void WorkerThreadFunc(WorkerCtx* pctx)
    {
        WorkerCtx& ctx = *pctx;

        nng_socket frontendSock, backendSock;
        nng_listener frontendListener;
        nng_dialer backendDialer;
        LRMPListenRaw(&frontendSock, &frontendListener, ctx.workerFrontendURL.c_str());
        LRMPConnectRaw(&backendSock, &backendDialer, ctx.workerBackendURL.c_str());

        nng_msg* msg;
        while (true) {
            bool isIdle = true;
            if (ctx.workerTermination) {
                break;
            }
            for (int i = 0; i < 10; ++i) {
                if (LRMPMsgRecv(frontendSock, &msg, NNG_FLAG_NONBLOCK)) {
                    break;
                }
                isIdle = false;

                ServerCmd msgCmd = LRMPMsgGetCmd(msg);
                switch (msgCmd) {
                case ServerCmd::CLIENT_PUSH: {
                    auto live = ctx.GetLiveW(msg);
                    if (live) {
                        live->Push(msg);
                    }
                    LRMPMsgSend(frontendSock, msg, 0);
                } break;
                case ServerCmd::CLIENT_PULL: {
                    auto live = ctx.GetLiveR(msg);
                    if (live) {
                        live->Pull(msg);
                    }
                    LRMPMsgSend(frontendSock, msg, 0);
                } break;
                case ServerCmd::CLIENT_HELLO:
                    LRMPMsgSend(frontendSock, msg, 0);
                    break;
                case ServerCmd::CLIENT_AUTHORIZE:
                case ServerCmd::CLIENT_VERIFY:
                case ServerCmd::CLIENT_START_LIVE:
                case ServerCmd::CLIENT_END_LIVE:
                case ServerCmd::CLIENT_ADMIN_CMD:
                case ServerCmd::CLIENT_PULL_PLAYERS:
                case ServerCmd::CLIENT_CHECK_PLAYER:
                    LRMPMsgSend(backendSock, msg, 0);
                    break;
                default:
                    LRMPMsgFree(msg);
                    break;
                }
            }
            for (int i = 0; i < 10; ++i) {
                if (LRMPMsgRecv(backendSock, &msg, NNG_FLAG_NONBLOCK)) {
                    break;
                }
                isIdle = false;

                ServerCmd msgCmd = LRMPMsgGetCmd(msg);
                switch (msgCmd) {
                case ServerCmd::CLIENT_HELLO:
                default:
                    LRMPMsgSend(frontendSock, msg, 0);
                    break;
                }
            }
            if (isIdle) {
                std::this_thread::sleep_for(std::chrono::milliseconds(5));
            }
        }

        LRMPClose(frontendSock, frontendListener);
        LRMPClose(backendSock, backendDialer);
    }
};

int main()
{
    CtrlCHandling();

    tsl::robin_map<std::string, std::shared_ptr<User>> userMap;
    tsl::robin_map<Cookie, User*> cookieMap;

    nng_socket backendSock;
    nng_listener backendListener;
    LRMPListenRaw(&backendSock, &backendListener, "inproc://lrmp_backend");

    std::vector<std::unique_ptr<WorkerCtx>> workers;
    workers.emplace_back(std::make_unique<WorkerCtx>("ipc://marketeer_test", "inproc://lrmp_backend")); // tcp://*:9961 || ipc://marketeer_test

    std::string playersDesc = "{\"players\":[]}";
    auto updPlayersDesc = [&]() {
        playersDesc = "{\"players\":[";
        int nbrPlayers = 0;
        for (auto& worker : workers) {
            for (unsigned int i = 0; i < worker->workerLoadMax; ++i) {
                auto live = worker->liveVec[i].safe_get();
                if (live) {
                    if (nbrPlayers) {
                        playersDesc += ",";
                    }
                    playersDesc += '{';

                    playersDesc += "\"username\":\"";
                    playersDesc += live->user->username;
                    playersDesc += "\",";
                    playersDesc += "\"fingerprint\":\"";
                    playersDesc += live->user->fingerprint;
                    playersDesc += '\"';

                    playersDesc += '}';
                    nbrPlayers++;
                }
            }
        }
        playersDesc += "]}";
    };

    std::string adminPassword { "8se.He;wN26J'T%\"" };
    char adminPasswordBuf[32];
    std::string adminCmd;
    char adminCmdBuf[256];
    std::string adminCmdArg;
    std::ostringstream adminCmdOutput;
    tsl::robin_map<std::string, std::function<void(std::string&, std::ostringstream&)>> cmdMap;
    cmdMap["list"] = [&](std::string& arg, std::ostringstream& out) {
        int nbrPlayers = 0;
        for (auto& worker : workers) {
            for (unsigned int i = 0; i < worker->workerLoadMax; ++i) {
                auto live = worker->liveVec[i].safe_get();
                if (live) {
                    out << live->user->username
                        << " ("
                        << live->user->fingerprint
                        << ")\n";
                    nbrPlayers++;
                }
            }
        }
        if (!nbrPlayers) {
            out << "No player is streaming.\n";
        }
    };
    cmdMap["kill"] = [&](std::string& arg, std::ostringstream& out) {
        auto userIt = userMap.find(arg);
        if (userIt != userMap.end()) {
            auto user = userIt.value();
            if (user->livePort) {
                // TODO: Load balacing
                auto& worker = *workers[0];
                worker.liveVec[user->liveIndex - 1].safe_assign(std::shared_ptr<LiveCtx>());
                worker.workerLoad--;
                updPlayersDesc();
                out << "Success.\n";
            } else {
                out << "User not streaming.\n";
            }
        } else {
            out << "User not found.\n";
        }
    };
    cmdMap["exit"] = [&](std::string& arg, std::ostringstream& out) {
        g_CtrlCInt = true;
        out << "Oyasumi.\n";
    };

    char usernameRaw[16] {};
    char fingerprintRaw[32] {};
    nng_msg* msg;
    while (true) {
        if (g_CtrlCInt) {
            goto end;
        }
        while (LRMPMsgRecv(backendSock, &msg, NNG_FLAG_NONBLOCK)) {
            if (g_CtrlCInt) {
                goto end;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
        }
        ServerCmd msgCmd = LRMPMsgGetCmd(msg);
        switch (msgCmd) {
        case ServerCmd::CLIENT_AUTHORIZE: {
            // TODO: Check public key
            if (LRMPMsgBodyLen(msg)) {
                Cookie vrfyCookie[2];
                vrfyCookie[0] = GenerateCookie();
                vrfyCookie[1] = GenerateCookie();
                std::string encData;
                std::string fingerprint = RSAVerficate(LRMPMsgBodyPtr(msg), LRMPMsgBodyLen(msg), vrfyCookie, sizeof(vrfyCookie), encData);
                if (fingerprint != "") {
                    auto& userPtr = userMap[fingerprint];
                    if (!userPtr) {
                        userPtr = std::make_shared<User>();
                        userPtr->fingerprint = fingerprint;
                    }
                    userPtr->vrfyCookie[0] = vrfyCookie[0];
                    userPtr->vrfyCookie[1] = vrfyCookie[1];
                    updPlayersDesc();
                    LRMPMsgReset(msg, ServerCmd::REP_OK, std::pair(encData.c_str(), encData.size()));
                } else {
                    LRMPMsgReset(msg, ServerCmd::REP_ERR_INVALID_CRED);
                }
            } else {
                LRMPMsgReset(msg, ServerCmd::REP_ERR_INVALID_MSG);
            }
            LRMPMsgSend(backendSock, msg, 0);
        } break;
        case ServerCmd::CLIENT_VERIFY: {
            if (LRMPMsgBodyLen(msg) == 48) {
                std::string fingerprint((char*)LRMPMsgBodyPtr(msg), 24);
                auto& userPtr = userMap[fingerprint];
                if (userPtr) {
                    Cookie vrfyCookie[2];
                    memcpy(vrfyCookie, LRMPMsgBodyPtr(msg, 24), sizeof(vrfyCookie));
                    if (userPtr->vrfyCookie[0] == vrfyCookie[0] && userPtr->vrfyCookie[1] == vrfyCookie[1]) {
                        memset(usernameRaw, 0, 16);
                        memcpy(usernameRaw, LRMPMsgBodyPtr(msg, 40), 8);
                        userPtr->username = usernameRaw;
                        // TODO: Check username

                        auto cookie = GenerateCookie(cookieMap);
                        if (userPtr->cookie) {
                            cookieMap.erase(cookie);
                        }
                        userPtr->cookie = cookie;
                        userPtr->routeId = LRMPMsgHeaderPtr(msg)[1];
                        cookieMap[cookie] = userPtr.get();

                        LRMPMsgReset(msg, ServerCmd::REP_OK, &cookie);
                    } else {
                        LRMPMsgReset(msg, ServerCmd::REP_ERR_INVALID_CRED);
                    }
                } else {
                    LRMPMsgReset(msg, ServerCmd::REP_ERR_INVALID_CRED);
                }
            } else {
                LRMPMsgReset(msg, ServerCmd::REP_ERR_INVALID_MSG);
            }
            LRMPMsgSend(backendSock, msg, 0);
        } break;
        case ServerCmd::CLIENT_START_LIVE: {
            Cookie cookie;
            LRMPMsgDump(msg, 0, &cookie);
            auto userIt = cookieMap.find(cookie);
            if (userIt != cookieMap.end() && userIt.value()->routeId == LRMPMsgHeaderPtr(msg)[1]) {
                auto user = userIt.value();
                if (!user->livePort) {
                    // TODO: Load balacing
                    LRMPMsgReset(msg, ServerCmd::REP_ERR_SERVER);
                    auto& worker = *workers[0];
                    for (unsigned int i = 0; i < worker.workerLoadMax; ++i) {
                        auto livePtr = worker.liveVec[i].safe_get();
                        if (!livePtr) {
                            uint16_t livePort = 9961;
                            uint16_t liveIndex = (uint16_t)(i + 1);
                            worker.liveVec[i].safe_assign(std::make_shared<LiveCtx>(user, livePort, liveIndex));
                            worker.workerLoad++;
                            updPlayersDesc();
                            uint32_t liveId = user->liveId;
                            LRMPMsgReset(msg, ServerCmd::REP_OK, &livePort, &liveId, &liveIndex);
                            break;
                        }
                    }
                } else {
                    uint16_t livePort = user->livePort;
                    uint16_t liveIndex = user->liveIndex;
                    LRMPMsgReset(msg, ServerCmd::REP_OK, &livePort, &liveIndex);
                }
            } else {
                LRMPMsgReset(msg, ServerCmd::REP_ERR_INVALID_CRED);
            }
            LRMPMsgSend(backendSock, msg, 0);
        } break;
        case ServerCmd::CLIENT_END_LIVE: {
            Cookie cookie;
            LRMPMsgDump(msg, 0, &cookie);
            auto userIt = cookieMap.find(cookie);
            if (userIt != cookieMap.end() && userIt.value()->routeId == LRMPMsgHeaderPtr(msg)[1]) {
                auto user = userIt.value();
                if (user->livePort) {
                    // TODO: Load balacing
                    auto& worker = *workers[0];
                    worker.liveVec[user->liveIndex - 1].safe_assign(std::shared_ptr<LiveCtx>());
                    worker.workerLoad--;
                    updPlayersDesc();
                    LRMPMsgReset(msg, ServerCmd::REP_OK);
                } else {
                    LRMPMsgReset(msg, ServerCmd::REP_ERR_INVALID_LIVE);
                }
            } else {
                LRMPMsgReset(msg, ServerCmd::REP_ERR_INVALID_CRED);
            }
            LRMPMsgSend(backendSock, msg, 0);
        } break;
        case ServerCmd::CLIENT_PULL_PLAYERS: {
            uint8_t padding = 0;
            LRMPMsgReset(msg, ServerCmd::REP_OK, std::pair(playersDesc.c_str(), playersDesc.size()), &padding);
            LRMPMsgSend(backendSock, msg, 0);
        } break;
        case ServerCmd::CLIENT_CHECK_PLAYER: {
            if (LRMPMsgBodyLen(msg) == 24) {
                memset(fingerprintRaw, 0, 32);
                LRMPMsgDump(msg, 0, std::pair(fingerprintRaw, (size_t)24));
                auto& userPtr = userMap[fingerprintRaw];
                if (userPtr && userPtr->livePort) {
                    uint16_t livePort = userPtr->livePort;
                    uint16_t liveIndex = userPtr->liveIndex;
                    uint32_t liveId = userPtr->liveId;
                    LRMPMsgReset(msg, ServerCmd::REP_OK, &livePort, &liveId, &liveIndex);
                } else {
                    LRMPMsgReset(msg, ServerCmd::REP_ERR_INVALID_LIVE);
                }
            } else {
                LRMPMsgReset(msg, ServerCmd::REP_ERR_INVALID_MSG);
            }
            LRMPMsgSend(backendSock, msg, 0);
        } break;
        case ServerCmd::CLIENT_ADMIN_CMD: {
            if (LRMPMsgBodyLen(msg) > 16 && LRMPMsgBodyLen(msg) < 256) {
                memset(adminPasswordBuf, 0, 32);
                LRMPMsgDump(msg, 0, std::pair(adminPasswordBuf, (size_t)16));
                if (adminPassword == adminPasswordBuf) {
                    memset(adminCmdBuf, 0, 256);
                    memcpy(adminCmdBuf, LRMPMsgBodyPtr(msg, 16), LRMPMsgBodyLen(msg) - 16);
                    adminCmd = adminCmdBuf;
                    adminCmdArg = ""; 
                    auto firstSpace = adminCmd.find(' ');
                    if (firstSpace != std::string::npos) {
                        adminCmdArg = adminCmd.substr(firstSpace + 1);
                        adminCmd = adminCmd.substr(0, firstSpace);
                    }
                    auto cmdIt = cmdMap.find(adminCmd);
                    if (cmdIt != cmdMap.end()) {
                        adminCmdOutput.str(std::string());
                        cmdIt.value()(adminCmdArg, adminCmdOutput);
                        auto output = adminCmdOutput.str();
                        LRMPMsgReset(msg, ServerCmd::REP_OK, std::pair(output.data(), (size_t)output.size() + 1));
                    } else {
                        LRMPMsgReset(msg, ServerCmd::REP_OK, std::pair("[error] Invalid command.", (size_t)25));
                    }
                    LRMPMsgSend(backendSock, msg, 0);
                } else {
                    LRMPMsgFree(msg);
                }
            } else {
                LRMPMsgFree(msg);
            }
        } break;
        case ServerCmd::CLIENT_HELLO:
            break;
        default:
            LRMPMsgFree(msg);
            break;
        }
    }

end:
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    for (auto& worker : workers) {
        worker.release();
    }
    LRMPClose(backendSock, backendListener);

    return 0;
}