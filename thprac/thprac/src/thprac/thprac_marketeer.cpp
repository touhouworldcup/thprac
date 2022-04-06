#define NOMINMAX
#include "thprac_marketeer.h"
#include "thprac_data_anly.h"
#include "thprac_launcher_utils.h"
#include "thprac_gui_impl_win32.h"
#include "thprac_load_exe.h"
#include "LRMP.h"
#include <Windows.h>
#include <algorithm>
#include <atomic>
#include <mutex>
#include <chrono>

namespace THPrac {

class MarketeerStream {
#define STREAM_BUFFER_SIZE (1024 * 1024 * 4)
public:
    int marketeerStatus = 0;
    char marketeerURL[256] {};

private:
    static DWORD __stdcall PullThreadFunc(LPVOID param)
    {
        MarketeerStream& stream = *(MarketeerStream*)param;
        nng_socket localSock;
        nng_dialer localDialer;
        nng_req0_open(&localSock);
        nng_setopt_ms(localSock, NNG_OPT_SENDTIMEO, 1000);
        nng_setopt_ms(localSock, NNG_OPT_RECVTIMEO, 1000);
        nng_dial(localSock, stream.marketeerURL, &localDialer, 0);
        nng_msg* clientReq;
        nng_msg* serverRep;


        {
            uint16_t cmd = 2, streamId = 0;
            uint32_t pad1 = 0, pad2 = 0, pad3 = 0, pad4 = 0;
            nng_msg_alloc(&clientReq, 0);
            LRMPMsgAppend(clientReq, &cmd, &streamId, &pad1, &pad2, &pad3, &pad4);
            // TODO: data
            if (nng_sendmsg(localSock, clientReq, 0)) {
                return 0;
            }
            if (nng_recvmsg(localSock, &serverRep, 0)) {
                return 0;
            }
            uint32_t* ptr = (uint32_t*)nng_msg_body(serverRep);
            stream.signalReadPtr = ptr[0];
            nng_msg_free(serverRep);
        }

        uint32_t reqSize = 32;
        while (!stream.haltFlag) {
            bool isIdle = true;

            uint32_t signalAdded = 0;
            uint32_t signalLimitPtr = stream.signalWritePtr;
            uint32_t signalPullMax = (STREAM_BUFFER_SIZE / 32 - 2);
            while (signalLimitPtr < signalPullMax) {
                uint16_t cmd = 1, streamId = 0;
                nng_msg_alloc(&clientReq, 0);
                LRMPMsgAppend(clientReq, &cmd, &streamId, &stream.signalReadPtr, &reqSize);
                if (nng_sendmsg(localSock, clientReq, 0)) {
                    if (stream.haltFlag) {
                        break;
                    }
                    nng_msg_free(clientReq);
                    continue;
                }
                if (nng_recvmsg(localSock, &serverRep, 0)) {
                    if (stream.haltFlag) {
                        break;
                    }
                    continue;
                }

                uint32_t fetchSize = 0;
                LRMPMsgDumpEx((uint8_t*)nng_msg_body(serverRep), &fetchSize);
                if (fetchSize) {
                    // TODO: data
                    isIdle = false;
                    stream.PushSignal((uint8_t*)nng_msg_body(serverRep) + 4, stream.signalReadPtr);
                    signalAdded++;
                    stream.signalReadPtr += fetchSize;
                    LRMPMsgFree(serverRep);
                    continue;
                } 
                LRMPMsgFree(serverRep);
                break;
            }
            stream.signalWritePtr += signalAdded;

            uint32_t streamLimitPtr = stream.streamWritePtr;
            uint32_t streamPullMax = (STREAM_BUFFER_SIZE - 1024);
            if (streamLimitPtr < streamPullMax && stream.streamPullToggle && stream.streamPullLock.try_lock()) {
                for (int connLimit = 0; connLimit < 5; ++connLimit) {
                    uint16_t cmd = 1, streamId = 1;
                    uint32_t tmpU32 = 0;
                    nng_msg_alloc(&clientReq, 0);
                    LRMPMsgAppend(clientReq, &cmd, &streamId, &stream.streamReadPtr, &tmpU32);
                    if (nng_sendmsg(localSock, clientReq, 0)) {
                        if (stream.haltFlag) {
                            break;
                        }
                        nng_msg_free(clientReq);
                        continue;
                    }
                    if (nng_recvmsg(localSock, &serverRep, 0)) {
                        if (stream.haltFlag) {
                            break;
                        }
                        continue;
                    }

                    uint32_t fetchSize = 0;
                    LRMPMsgDumpEx((uint8_t*)nng_msg_body(serverRep), &fetchSize);
                    if (fetchSize) {
                        isIdle = false;
                        stream.PushStream((uint8_t*)nng_msg_body(serverRep) + 4, nng_msg_len(serverRep) - 4);
                        stream.streamReadPtr += fetchSize;
                        LRMPMsgFree(serverRep);
                        continue;
                    }
                    LRMPMsgFree(serverRep);
                    break;
                }
                stream.streamPullLock.unlock();
            }

            if (isIdle) {
                for (int i = 0; i < 5; ++i) {
                    if (stream.streamWakeUpFlag) {
                        stream.streamWakeUpFlag = false;
                        break;
                    }
                    Sleep(100);
                }
            }
        }

        
        {
            if (stream.endingSignal) {
                uint16_t cmd = 3, streamId = 0;
                uint32_t pad1 = stream.endingSignal->offset, pad2 = 0, pad3 = 0, pad4 = 0;
                nng_msg_alloc(&clientReq, 0);
                LRMPMsgAppend(clientReq, &cmd, &streamId, &pad1, &pad2, &pad3, &pad4);
                // TODO: data
                if (nng_sendmsg(localSock, clientReq, 0)) {
                    return 0;
                }
                if (nng_recvmsg(localSock, &serverRep, 0)) {
                    return 0;
                }
                nng_msg_free(serverRep);
            }
        }

        return 0;
    }
    static DWORD __stdcall PushThreadFunc(LPVOID param)
    {
        MarketeerStream& stream = *(MarketeerStream*)param;
        nng_socket localSock;
        nng_dialer localDialer;
        nng_req0_open(&localSock);
        nng_setopt_ms(localSock, NNG_OPT_SENDTIMEO, 1000);
        nng_setopt_ms(localSock, NNG_OPT_RECVTIMEO, 1000);
        nng_dial(localSock, stream.marketeerURL, &localDialer, 0);
        nng_msg* clientReq;
        nng_msg* serverRep;

        {
            uint16_t cmd = 2, streamId = 0;
            uint32_t pad1 = 1;
            nng_msg_alloc(&clientReq, 0);
            LRMPMsgAppend(clientReq, &cmd, &streamId, &pad1);
            // TODO: data
            if (nng_sendmsg(localSock, clientReq, 0)) {
                return 0;
            }
            if (nng_recvmsg(localSock, &serverRep, 0)) {
                return 0;
            }
            uint32_t* ptr = (uint32_t*)nng_msg_body(serverRep);
            stream.signalRemotePtr = ptr[0];
            stream.streamRemotePtr = ptr[1];
            stream.streamSyncPtr = ptr[1];
            nng_msg_free(serverRep);
        }

        bool haltFlagBuffer = false;
        while (!haltFlagBuffer) {
            bool isIdle = true;

            if (stream.haltFlag) {
                haltFlagBuffer = true;
            }

            size_t signalWritePtr = stream.signalWritePtr;
            if (stream.signalReadPtr != signalWritePtr) {
                while (stream.signalReadPtr < signalWritePtr) {
                    isIdle = false;
                    auto& sig = stream.signalVec[stream.signalReadPtr];
                    uint32_t sigExtraSize = sig.extra.size();
                    uint16_t cmd = 1, streamId = 0;
                    double tmpDbl = 0.0;
                    nng_msg_alloc(&clientReq, 0);
                    LRMPMsgAppend(clientReq, &cmd, &streamId, &stream.signalRemotePtr,
                        &sig.time, &sig.param1, &sig.param2, &sig.param3, &sig.param4, &sig.param5, &sig.param6);
                    if (sigExtraSize && sigExtraSize % 32 == 0) {
                        LRMPMsgAppend(clientReq, std::pair(sig.extra.data(), sigExtraSize));
                    }
                    // TODO: data
                    if (nng_sendmsg(localSock, clientReq, 0)) {
                        if (stream.haltFlag) {
                            break;
                        }
                        nng_msg_free(clientReq);
                        continue;
                    }
                    if (nng_recvmsg(localSock, &serverRep, 0)) {
                        if (stream.haltFlag) {
                            break;
                        }
                        continue;
                    }
                    nng_msg_free(serverRep);
                    stream.signalRemotePtr += 32;
                    stream.signalReadPtr += 1;
                }
            }

            stream_push_start:
            size_t streamWritePtr = stream.streamWritePtr;
            if (stream.streamReadPtr != streamWritePtr) {
                size_t streamTargetPtr = stream.streamReadPtr < streamWritePtr ? streamWritePtr : stream.streamSize;
                while (stream.streamReadPtr < streamTargetPtr) {
                    auto streamDiff = std::min(480u, streamTargetPtr - stream.streamReadPtr);
                    nng_msg_alloc(&clientReq, 0);
                    uint16_t cmd = 1, streamId = 1;
                    LRMPMsgAppend(clientReq, &cmd, &streamId, &stream.streamRemotePtr,
                        std::pair((uint8_t*)(stream.streamBuffer + stream.streamReadPtr), (size_t)streamDiff));
                    if (nng_sendmsg(localSock, clientReq, 0)) {
                        if (stream.haltFlag) {
                            break;
                        }
                        nng_msg_free(clientReq);
                        continue;
                    }
                    if (nng_recvmsg(localSock, &serverRep, 0)) {
                        if (stream.haltFlag) {
                            break;
                        }
                        continue;
                    }
                    nng_msg_free(serverRep);
                    stream.streamReadPtr += streamDiff;
                    stream.streamRemotePtr += streamDiff;
                }
                if (stream.streamReadPtr == stream.streamSize) {
                    stream.streamReadPtr = 0;
                    goto stream_push_start;
                }
            } 

            if (isIdle) {
                Sleep(100);
            }
        }

        {
            uint16_t cmd = 3, streamId = 0;
            uint32_t pad1 = 0;
            nng_msg_alloc(&clientReq, 0);
            LRMPMsgAppend(clientReq, &cmd, &streamId, &pad1);
            // TODO: data
            if (nng_sendmsg(localSock, clientReq, 0)) {
                return 0;
            }
            if (nng_recvmsg(localSock, &serverRep, 0)) {
                return 0;
            }
            nng_msg_free(serverRep);
        }

        return 0;
    }

    std::atomic<bool> haltFlag = false;
    GuiThread pushThread { PushThreadFunc };
    GuiThread pullThread { PullThreadFunc };

public:
    void* hwnd;
    std::string currentGame;
    StreamSignal* endingSignal = nullptr;

    uint16_t signalId = 0;
    std::vector<StreamSignal> signalVec;
    std::atomic<uint32_t> signalWritePtr;
    uint32_t signalReadPtr = 0;
    uint32_t signalRemotePtr = 0;
    uint32_t signalPullCursor = 0;

    uint16_t streamId = 1;
    uint8_t* streamBuffer = nullptr;
    size_t streamSize = STREAM_BUFFER_SIZE;
    std::atomic<uint32_t> streamWritePtr;
    std::atomic<uint32_t> streamSyncPtr;
    uint32_t streamReadPtr = 0;
    uint32_t streamRemotePtr = 0;
    std::atomic<bool> streamWakeUpFlag;
    std::atomic<bool> streamPullToggle;
    std::mutex streamPullLock;
    uint32_t streamPullRefStart = 0;
    uint32_t streamPullRefStage = 0;
    uint32_t streamPullLimit = 0;

public:
    MarketeerStream()
    {
        streamWakeUpFlag = false;
        streamPullToggle = false;
        streamWritePtr = 0;
        streamBuffer = (uint8_t*)malloc(streamSize);
        signalVec.reserve(8192);
        signalWritePtr = 0;
    }
    void EnablePull()
    {
        if (!pullThread.IsActive()) {
            pullThread.Stop();
            pullThread.Start(this);
        }
    }
    void EnablePush()
    {
        if (!pushThread.IsActive()) {
            pushThread.Stop();
            pushThread.Start(this);
        }
    }
    void Halt()
    {
        haltFlag = true;
        pullThread.Wait();
        pushThread.Wait();
    }
    void PushStream(void* buffer, uint32_t size)
    {
        if (size) {
            uint8_t* u8Buffer = (uint8_t*)buffer;
            size_t wPtr = streamWritePtr;

            auto remainingSize = streamSize - wPtr;
            if (remainingSize < size) {
                memcpy(streamBuffer + wPtr, u8Buffer, size);
                memcpy(streamBuffer, u8Buffer + remainingSize, size - remainingSize);
                streamWritePtr = size - remainingSize;
            } else {
                memcpy(streamBuffer + wPtr, u8Buffer, size);
                if (wPtr + size == streamSize) {
                    streamWritePtr = 0;
                } else {
                    streamWritePtr = wPtr + size;
                }
            }
            streamSyncPtr = streamSyncPtr + size;
        }
    }
    void PushSignal(uint32_t param1 = 0, uint32_t param2 = 0, uint32_t param3 = 0, uint32_t param4 = 0, uint32_t param5 = 0, uint32_t param6 = 0, std::string* data = nullptr)
    {
        signalVec.emplace_back(std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now()).time_since_epoch().count(),
            param1, param2, param3, param4, param5, param6, data);
        signalWritePtr = signalWritePtr + 1;
    }
    void PushSignal(uint8_t* data, uint32_t offset)
    {
        signalVec.emplace_back(*(uint64_t*)(data),
            *(uint32_t*)(data + 8),
            *(uint32_t*)(data + 12),
            *(uint32_t*)(data + 16),
            *(uint32_t*)(data + 20),
            *(uint32_t*)(data + 24),
            *(uint32_t*)(data + 28));
        signalVec.back().offset = offset;
        signalWritePtr = signalWritePtr + 1;
    }
};
std::unique_ptr<MarketeerStream> g_mktStream;

void MarketeerEnablePush(const char* url)
{
    if (!g_mktStream) {
        g_mktStream = std::make_unique<MarketeerStream>();
    }
    strcpy(g_mktStream->marketeerURL, url);
    g_mktStream->EnablePush();
    g_mktStream->marketeerStatus = 2;
}

void MarketeerEnablePull(const char* url)
{
    if (!g_mktStream) {
        g_mktStream = std::make_unique<MarketeerStream>();
    }
    strcpy(g_mktStream->marketeerURL, url);
    g_mktStream->EnablePull();
    g_mktStream->marketeerStatus = 1;
}

int MarketeerGetStatus()
{
    if (!g_mktStream) {
        return 0;
    }
    return g_mktStream->marketeerStatus;
}

void MarketeerEndLive()
{
    g_mktStream->Halt();
}

void MarketeerTerminate()
{
    g_mktStream->Halt();
}

bool MarketeerCheckKey(int v_key)
{
    return (Gui::ImplWin32CheckForeground()) ? (GetAsyncKeyState(v_key) < 0) : false;
}

void MarketeerFreezeGame()
{
    HWND hWnd = NULL;
    MSG msg;
    while (::PeekMessage(&msg, hWnd, 0U, 0U, PM_REMOVE)) {
        ::TranslateMessage(&msg);
        ::DispatchMessage(&msg);
        if (msg.message == WM_QUIT) {
            exit(0);
        }
    }

    g_mktStream->streamWakeUpFlag = true;
    Sleep(20);
}

void MarketeerPush(void* buffer, size_t size)
{
    g_mktStream->PushStream(buffer, size);
}

void MarketeerPushSignal(unsigned int param1, unsigned int param2, unsigned int param3, unsigned int param4, unsigned int param5, unsigned int param6, std::string* data)
{
    g_mktStream->PushSignal(param1, param2, param3, param4, param5, param6, data);
}

void MarketeerOnStageStart(bool isGameStart, unsigned int stage, unsigned int rank, unsigned int player, unsigned int seed, unsigned int playerX, unsigned int playerY)
{
    uint32_t param1 = (isGameStart ? MARKETEER_NEW_GAME : MARKETEER_NEW_STAGE) | (stage << 8) | (rank << 16) | (player << 24);
    uint32_t param2 = g_mktStream->streamSyncPtr;
    uint32_t param3 = seed;
    uint32_t param4 = playerX;
    uint32_t param5 = playerY;
    g_mktStream->PushSignal(param1, param2, param3, param4, param5, 0);
}

void MarketeerOnQuitToMenu()
{
    uint32_t param2 = g_mktStream->streamSyncPtr;
    g_mktStream->PushSignal(MARKETEER_QUIT_TO_MENU, param2);
}

void MarketeerOnGameComlete()
{
    uint32_t param2 = g_mktStream->streamSyncPtr;
    g_mktStream->PushSignal(MARKETEER_GAME_COMPLETE, param2);
}

void MarketeerSetCurrentGame(const char* game)
{
    g_mktStream->currentGame = game;
}

StreamSignal* MarketeerGetCursor()
{
    if (g_mktStream && g_mktStream->signalVec.size() > g_mktStream->signalPullCursor) {
        auto& signal = g_mktStream->signalVec[g_mktStream->signalPullCursor];
        if (*(uint8_t*)(&signal.param1) == MARKETEER_LAUNCH) {
            if (strcmp(g_mktStream->currentGame.c_str(), (char*)(&signal.param2))) {
                g_mktStream->endingSignal = &signal;
                g_mktStream->Halt();
                return nullptr;
            }
        } else if (*(uint8_t*)(&signal.param1) == MARKETEER_EXIT_GAME) {
            g_mktStream->endingSignal = &signal;
            g_mktStream->Halt();
            return nullptr;
        }
        return &signal;
    }
    return nullptr;
}

StreamSignal* MarketeerPeekCursor()
{
    if (g_mktStream && g_mktStream->signalVec.size() > g_mktStream->signalPullCursor) {
        auto& signal = g_mktStream->signalVec[g_mktStream->signalPullCursor];
        return &signal;
    }
    return nullptr;
}

void MarketeerAdvCursor()
{
    if (g_mktStream) {
        g_mktStream->signalPullCursor++;
    }
}

void MarketeerAlignStream(StreamSignal* sigPtr)
{
    if (sigPtr) {
        auto& stream = *g_mktStream;
        auto sig = *sigPtr;
        MarketeerCmd cmd = (MarketeerCmd)sig.GetCmd();

        if (cmd == THPrac::MARKETEER_NEW_GAME || cmd == MARKETEER_NEW_STAGE) {
            std::lock_guard<std::mutex> lock(stream.streamPullLock);
            uint32_t remotePos = sig.param2;
            if (cmd == MARKETEER_NEW_GAME || remotePos >= stream.streamReadPtr) {
                stream.streamWritePtr = 0;
                stream.streamReadPtr = remotePos;
                stream.streamPullRefStart = remotePos;
            }
            stream.streamPullRefStage = remotePos - stream.streamPullRefStart;
            stream.streamPullToggle = true;
            stream.streamWakeUpFlag = true;
            stream.streamPullLimit = 0;
        }
    }
}

void MarketeerSetStreamLimit(StreamSignal* sigPtr)
{
    auto& stream = *g_mktStream;
    if (sigPtr) {
        auto sig = *sigPtr;
        MarketeerCmd cmd = (MarketeerCmd)sig.GetCmd();

        if (cmd == MARKETEER_NEW_GAME || cmd == MARKETEER_NEW_STAGE || cmd == MARKETEER_QUIT_TO_MENU) {
            stream.streamPullLimit = sig.param2 - stream.streamPullRefStart;
        }
    } else {
        stream.streamPullLimit = 1;
    }
}

int MarketeerReadStream(uint32_t unitOffset, uint32_t unitSize, void* out)
{
    auto& stream = *g_mktStream;
    uint32_t writePtr = stream.streamWritePtr;
    uint32_t offset = unitOffset * unitSize;
    offset += stream.streamPullRefStage;

    if (stream.streamPullLimit && stream.streamPullLimit < (offset + unitSize)) {
        return -1;
    } else if (writePtr < (offset + unitSize)) {
        return 0;
    } 

    memcpy(out, stream.streamBuffer + offset, unitSize);
    return unitSize;
}

int MarketeerInit(const char* game, void* hwnd)
{
    if (GetUserData() && *GetUserData()) {
        auto data = *GetUserData();
        if (*(uint32_t*)data == 'MRTS') {
            MarketeerEnablePush((char*)((uint8_t*)*GetUserData() + 4));
            g_mktStream->hwnd = hwnd;
            return 2;
        } else if (*(uint32_t*)data == 'WEIV') {
            MarketeerEnablePull((char*)((uint8_t*)*GetUserData() + 4));
            MarketeerSetCurrentGame(game);
            g_mktStream->hwnd = hwnd;
            //SetClassLongA(*(HWND*)hwnd, GCL_STYLE, GetClassLongA(*(HWND*)hwnd, GCL_STYLE) | CS_NOCLOSE);
            //SetWindowLongA(*(HWND*)hwnd, GWL_STYLE, GetWindowLongA(*(HWND*)hwnd, GWL_STYLE) & ~WS_SYSMENU); 
            Gui::ImplWin32SetNoClose(true);
            return 1;
        }
    }
    return 0;
}

MarketeerCmd MarketeerInputHelper(void* buffer, size_t size)
{
    MarketeerCmd result = MARKETEER_ON_HOLD;
    char outputBuf[128];
    int res = 0;

    do {
        auto sig = MarketeerPeekCursor();
        if (sig) {
            auto cmd = sig->GetCmd();
            if (cmd == MARKETEER_NEW_GAME || cmd == MARKETEER_NEW_STAGE || cmd == MARKETEER_QUIT_TO_MENU || cmd == MARKETEER_GAME_COMPLETE) {
                MarketeerSetStreamLimit(sig);
                DataRef<MARKETEER_LIMIT_LOCK>() = 1;
            } 
        }
        res = MarketeerReadStream(DataRef<MARKETEER_FRAME_REC_1>(), size, outputBuf);
        if (!res) {
            sig = MarketeerGetCursor();
            MarketeerSetCaption(MKT_CAPTION_FETCHING);
            MarketeerFreezeGame();
        } else {
            MarketeerSetCaption(MKT_CAPTION_PLAYING);
        }
    } while (!res);

    if (res == -1) {
        DataRef<MARKETEER_LIMIT_LOCK>() = 0;
        memset(outputBuf, 0, size);
        outputBuf[0] = outputBuf[1] = outputBuf[2] = 0;

        auto sig = MarketeerGetCursor();
        if (sig) {
            auto cmd = sig->GetCmd();
            auto stage = DataRef<DATA_STAGE>();
            if (cmd == MARKETEER_NEW_GAME) {
                result = MARKETEER_NEW_GAME;
            } else if (cmd == MARKETEER_QUIT_TO_MENU) {
                result = MARKETEER_QUIT_TO_MENU;
            }
        }
    }

    DataRef<MARKETEER_FRAME_REC_1>()++;
    memcpy(buffer, outputBuf, size);
    return result;
}

void MarketeerSetCaption(MarketeerCaption caption)
{
    switch (caption) {
    case THPrac::MKT_CAPTION_WAITING_SIGNAL:
        SetWindowTextA(*(HWND*)g_mktStream->hwnd, "[MARKETEER POC] Waiting for signal...");
        break;
    case THPrac::MKT_CAPTION_ACCEPTING:
        SetWindowTextA(*(HWND*)g_mktStream->hwnd, "[MARKETEER POC] Signal accepted, entering game...");
        break;
    case THPrac::MKT_CAPTION_FETCHING:
        SetWindowTextA(*(HWND*)g_mktStream->hwnd, "[MARKETEER POC] Fetching stream...");
        break;
    case THPrac::MKT_CAPTION_PLAYING:
        SetWindowTextA(*(HWND*)g_mktStream->hwnd, "[MARKETEER POC] Playing");
        break;
    default:
        break;
    }
}

void MarketeerMutex()
{
    if (GetUserData() && *GetUserData()) {
        CreateMutexA(0, 1, (const char*)*GetUserData());
    }
}

}