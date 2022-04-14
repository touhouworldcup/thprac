#include "thprac_marketeer.h"
#include "thprac_launcher_mkt.h"
#include "thprac_launcher_games.h"
#include "thprac_launcher_main.h"
#include "thprac_launcher_utils.h"
#include "thprac_launcher_cfg.h"
#include "thprac_main.h"
#include "thprac_config_db.h"
#include "thprac_ssl.h"
#include "LRMP.h"
#include <stdio.h>
#include <string>
#include <list>
#include <atomic>
#include <mutex>
#include <chrono>
#include <ctime>

namespace THPrac {

namespace Marketeer {
    class MarketeerStream {
    public:
        enum Type : int {
            MKT_IDLE = 0,
            MKT_STREAMER = 1,
            MKT_VIEWER = 2,
        };
        enum Status {
            STATUS_ERROR_LAUNCH_FAILED = -5,
            STATUS_ERROR_LIVE = -4,
            STATUS_ERROR_SERVER = -3,
            STATUS_ERROR_AUTH = -2,
            STATUS_FINISHED = -1,
            STATUS_ERROR_CONNECTION = 0,
            STATUS_CONNECTING = 1,
            STATUS_STREAMING = 2,
            STATUS_FINISHING = 3,
        };
        std::atomic<int> type = MKT_IDLE;

#define STREAM_COUNT (8)
#define STREAM_BUFFER_SIZE (1024 * 1024 * 4)
    private:
        std::atomic<int> marketeerStatus { 0 };
        std::string marketeerPrivateKey;
        std::string marketeerPublicKey;
        std::string marketeerFingerprint;
        nng_socket marketeerSock {};
        nng_dialer marketeerDialer {};
        uint64_t marketeerCookie = 0;
        uint16_t marketeerPort = 0;
        uint16_t marketeerLiveIndex = 0;
        uint32_t marketeerLiveId = 0;
        char marketeerURL[256] {};
        char marketeerUsername[16] {};
        
        static DWORD __stdcall StaticConnect(LPVOID param)
        {
            MarketeerStream& mkt = *(MarketeerStream*)param;
            mkt.marketeerStatus = STATUS_CONNECTING;
            nng_req0_open(&mkt.marketeerSock);
            nng_setopt_ms(mkt.marketeerSock, NNG_OPT_SENDTIMEO, 10000);
            nng_setopt_ms(mkt.marketeerSock, NNG_OPT_RECVTIMEO, 10000);

            do {
                if (!nng_dial(mkt.marketeerSock, mkt.marketeerURL, &mkt.marketeerDialer, 0)) {
                    break;
                }
                mkt.marketeerStatus = STATUS_ERROR_CONNECTION;
                Sleep(1000);
            } while (!mkt.haltFlag);
            if (mkt.haltFlag) {
                mkt.marketeerStatus = STATUS_FINISHED;
                return 0;
            }

            // TODO: Hello
            return 1;
        }
        static DWORD __stdcall StaticAuth(LPVOID param)
        {
            MarketeerStream& mkt = *(MarketeerStream*)param;
            mkt.marketeerStatus = STATUS_CONNECTING;
            nng_msg* clientReq = nullptr;
            nng_msg* serverRep = nullptr;

            do {
                clientReq = LRMPMsgCreate(ServerCmd::CLIENT_AUTHORIZE,
                    std::pair(mkt.marketeerPublicKey.c_str(), (size_t)mkt.marketeerPublicKey.size()));
                if (!LRMPMsgSend(mkt.marketeerSock, clientReq, 0)) {
                    if (!LRMPMsgRecv(mkt.marketeerSock, &serverRep, 0)) {
                        break;
                    }
                } else {
                    LRMPMsgFree(clientReq);
                }
                mkt.marketeerStatus = STATUS_ERROR_CONNECTION;
                Sleep(1000);
            } while (!mkt.haltFlag);
            if (mkt.haltFlag) {
                mkt.marketeerStatus = STATUS_FINISHED;
                return 0;
            }

            if (LRMPMsgGetCmd(serverRep) != ServerCmd::REP_OK) {
                mkt.marketeerStatus = STATUS_ERROR_AUTH;
                return 0;
            } 

            std::string dec = SSLDecrypt(mkt.marketeerPrivateKey, LRMPMsgBodyPtr(serverRep), LRMPMsgBodyLen(serverRep));
            if (dec.size()) {
                do {
                    clientReq = LRMPMsgCreate(ServerCmd::CLIENT_VERIFY,
                        std::pair(mkt.marketeerFingerprint.c_str(), (size_t)mkt.marketeerFingerprint.size()),
                        std::pair(dec.c_str(), (size_t)dec.size()),
                        std::pair(mkt.marketeerUsername, (size_t)8));
                    if (!LRMPMsgSend(mkt.marketeerSock, clientReq, 0)) {
                        if (!LRMPMsgRecv(mkt.marketeerSock, &serverRep, 0)) {
                            break;
                        }
                    } else {
                        LRMPMsgFree(clientReq);
                    }
                    mkt.marketeerStatus = STATUS_ERROR_CONNECTION;
                    Sleep(1000);
                } while (!mkt.haltFlag);
                if (mkt.haltFlag) {
                    return 0;
                }
            }

            if (LRMPMsgGetCmd(serverRep) == ServerCmd::REP_OK) {
                LRMPMsgDump(serverRep, 0, &mkt.marketeerCookie);
            } else {
                mkt.marketeerStatus = STATUS_ERROR_AUTH;
                return 0;
            }

            return 1;
        }
        static DWORD __stdcall StaticStartStreamer(LPVOID param)
        {
            // TODO:
            int ret = 0;
            MarketeerStream& mkt = *(MarketeerStream*)param;
            mkt.marketeerStatus = STATUS_CONNECTING;
            nng_msg* clientReq = nullptr;
            nng_msg* serverRep = nullptr;

        start:
            do {
                clientReq = LRMPMsgCreate(ServerCmd::CLIENT_START_LIVE, &mkt.marketeerCookie);
                if (!LRMPMsgSend(mkt.marketeerSock, clientReq, 0)) {
                    if (!LRMPMsgRecv(mkt.marketeerSock, &serverRep, 0)) {
                        break;
                    }
                } else {
                    LRMPMsgFree(clientReq);
                }
                mkt.marketeerStatus = STATUS_ERROR_CONNECTION;
                Sleep(1000);
            } while (!mkt.haltFlag);
            if (mkt.haltFlag) {
                mkt.marketeerStatus = STATUS_FINISHED;
                return ret;
            }

            auto cmd = LRMPMsgGetCmd(serverRep);
            if (cmd == ServerCmd::REP_OK) {
                LRMPMsgDump(serverRep, 0, &mkt.marketeerPort, &mkt.marketeerLiveId, &mkt.marketeerLiveIndex);
                mkt.marketeerStatus = STATUS_STREAMING;
                ret = 1;
            } else if (cmd == ServerCmd::REP_ERR_INVALID_CRED) {
                if (!StaticAuth(param)) {
                    return ret;
                }
                goto start;
            } else {
                mkt.marketeerStatus = STATUS_ERROR_SERVER;
                return 0;
            }

            LRMPMsgFree(serverRep);
            return ret;
        }
        static DWORD __stdcall StaticEndStreamer(LPVOID param)
        {
            // TODO:
            int ret = 0;
            MarketeerStream& mkt = *(MarketeerStream*)param;
            mkt.marketeerStatus = STATUS_FINISHING;
            nng_msg* clientReq = nullptr;
            nng_msg* serverRep = nullptr;

            start:
            do {
                clientReq = LRMPMsgCreate(ServerCmd::CLIENT_END_LIVE, &mkt.marketeerCookie);
                if (!LRMPMsgSend(mkt.marketeerSock, clientReq, 0)) {
                    if (!LRMPMsgRecv(mkt.marketeerSock, &serverRep, 0)) {
                        break;
                    }
                } else {
                    LRMPMsgFree(clientReq);
                }
                mkt.marketeerStatus = STATUS_ERROR_CONNECTION;
                Sleep(1000);
            } while (!mkt.haltFlag);
            if (mkt.haltFlag) {
                mkt.marketeerStatus = STATUS_FINISHED;
                return 0;
            }

            auto cmd = LRMPMsgGetCmd(serverRep);
            if (cmd == ServerCmd::REP_OK || cmd == ServerCmd::REP_ERR_INVALID_LIVE) {
                mkt.marketeerStatus = STATUS_FINISHED;
                ret = 1;
            } else if (cmd == ServerCmd::REP_ERR_INVALID_CRED) {
                if (!StaticAuth(param)) {
                    return ret;
                }
                goto start;
            } else {
                mkt.marketeerStatus = STATUS_ERROR_SERVER;
                return 0;
            }

            LRMPMsgFree(serverRep);
            return ret;
        }
        static DWORD __stdcall StaticStartViewer(LPVOID param)
        {
            // TODO:
            MarketeerStream& mkt = *(MarketeerStream*)param;
            mkt.marketeerStatus = STATUS_CONNECTING;
            nng_msg* clientReq = nullptr;
            nng_msg* serverRep = nullptr;

            do {
                clientReq = LRMPMsgCreate(ServerCmd::CLIENT_CHECK_PLAYER, std::pair(mkt.marketeerFingerprint.c_str(), (size_t)24));
                if (!LRMPMsgSend(mkt.marketeerSock, clientReq, 0)) {
                    if (!LRMPMsgRecv(mkt.marketeerSock, &serverRep, 0)) {
                        break;
                    }
                } else {
                    LRMPMsgFree(clientReq);
                }
                mkt.marketeerStatus = STATUS_ERROR_CONNECTION;
                Sleep(1000);
            } while (!mkt.haltFlag);
            if (mkt.haltFlag) {
                return 0;
            }

            auto cmd = LRMPMsgGetCmd(serverRep);
            if (cmd == ServerCmd::REP_OK) {
                LRMPMsgDump(serverRep, 0, &mkt.marketeerPort, &mkt.marketeerLiveId, &mkt.marketeerLiveIndex);
                LRMPMsgFree(serverRep);
                mkt.marketeerStatus = STATUS_STREAMING;
            } else {
                mkt.marketeerStatus = STATUS_ERROR_SERVER;
                return 0;
            }

            return 1;
        }

        static DWORD __stdcall PullLocalFunc(LPVOID param)
        {
            MarketeerStream& mkt = *(MarketeerStream*)param;
            nng_socket localSock;
            nng_listener localListener;
            nng_msg* msg = nullptr;

            std::string localUrl = "ipc://";
            localUrl += mkt.idString;
            nng_rep0_open(&localSock);
            nng_setopt_ms(localSock, NNG_OPT_SENDTIMEO, 1000);
            nng_setopt_ms(localSock, NNG_OPT_RECVTIMEO, 1000);
            nng_listen(localSock, localUrl.c_str(), &localListener, 0);

            char localBuf[512];
            while (!mkt.haltFlag) {
                if (nng_recvmsg(localSock, &msg, NNG_FLAG_NONBLOCK)) {
                    Sleep(10);
                    continue;
                }
                uint16_t cmd, streamId;
                uint32_t readPtr, readSize, result = 0;
                LRMPMsgDumpEx((uint8_t*)nng_msg_body(msg), &cmd, &streamId, &readPtr, &readSize);
                nng_msg_chop(msg, nng_msg_len(msg));
                switch (cmd) {
                case 1: {
                    result = mkt.streams[streamId].PullStream(readPtr, localBuf, readSize);
                    LRMPMsgAppend(msg, &result);
                    if (result) {
                        LRMPMsgAppend(msg, std::pair((uint8_t*)localBuf, result));
                    }
                } break;
                case 2: {
                    uint32_t runCurosr = mkt.cursorForControl;
                    mkt.singalsMutex.lock();
                    result = mkt.singalsForControl[runCurosr].cursorPos;
                    mkt.singalsMutex.unlock();
                    LRMPMsgAppend(msg, &result);
                } break;
                case 3: {
                    mkt.singalsMutex.lock();
                    uint32_t finalPos = readPtr + 32;
                    uint32_t i = mkt.cursorForControl;
                    for (; i < mkt.singalsForControl.size(); ++i) {
                        if (mkt.singalsForControl[i].cursorPos >= finalPos) {
                            break;
                        }
                    }
                    mkt.singalsMutex.unlock();
                    TerminateProcess(mkt.handleForControl, 0);
                    CloseHandle(mkt.handleForControl);
                    mkt.handleForControl = 0;
                    mkt.cursorForControl = i;
                } break;
                default:
                    LRMPMsgAppend(msg, &result);
                    break;
                }
                nng_sendmsg(localSock, msg, 0);
            }

            nng_listener_close(localListener);
            nng_close(localSock);

            return 0;
        }
        static DWORD __stdcall PullThreadFunc(LPVOID param)
        {
            MarketeerStream& mkt = *(MarketeerStream*)param;
            nng_msg* clientReq;
            nng_msg* serverRep;

            if (!StaticConnect(param)) {
                return 0;
            }
            if (!StaticStartViewer(param)) {
                return 0;
            }

            GuiThread pullThread { PullLocalFunc };
            pullThread.Start(param);

            uint32_t reqSize = 32;
            while (!mkt.haltFlag) {
                bool isIdle = true;

                for (int i = 0; i < STREAM_COUNT; ++i) {
                    auto& stream = mkt.streams[i];
                    for (int connLimit = 0; connLimit < 5 && !mkt.haltFlag; ++connLimit) {
                        clientReq = LRMPMsgCreate(ServerCmd::CLIENT_PULL, &mkt.marketeerLiveId, &mkt.marketeerLiveIndex, &stream.streamId, &stream.streamReadPtr);
                        if (LRMPMsgSend(mkt.marketeerSock, clientReq, 0)) {
                            mkt.marketeerStatus = STATUS_ERROR_CONNECTION;
                            LRMPMsgFree(clientReq);
                            break;
                        }
                        if (LRMPMsgRecv(mkt.marketeerSock, &serverRep, 0)) {
                            mkt.marketeerStatus = STATUS_ERROR_CONNECTION;
                            break;
                        }

                        auto cmd = LRMPMsgGetCmd(serverRep);
                        if (cmd == ServerCmd::REP_OK) {
                            isIdle = false;
                            LRMPMsgDump(serverRep, 0, &stream.streamReadPtr);
                            stream.PushStream(LRMPMsgBodyPtr(serverRep, 4), LRMPMsgBodyLen(serverRep) - 4);
                            LRMPMsgFree(serverRep);
                            continue;
                        } else if (cmd == ServerCmd::REP_ERR_INVALID_LIVE) {
                            mkt.marketeerStatus = STATUS_ERROR_LIVE;
                            LRMPMsgFree(serverRep);
                            goto end;
                        } else  {
                            LRMPMsgFree(serverRep);
                        }
                        break;
                    }
                }

                if (mkt.haltFlag) {
                    break;
                }

                if (isIdle) {
                    Sleep(500);
                }
            }

            mkt.marketeerStatus = STATUS_FINISHING;
            end:
            mkt.haltFlag = true;
            pullThread.Wait();
            if (mkt.marketeerStatus == STATUS_FINISHING) {
                mkt.marketeerStatus = STATUS_FINISHED;
            }
            return 0;
        }
        static DWORD __stdcall PushLocalFunc(LPVOID param)
        {
            MarketeerStream& mkt = *(MarketeerStream*)param;
            nng_socket localSock;
            nng_listener localListener;
            nng_msg* msg = nullptr;

            std::string localUrl = "ipc://";
            localUrl += mkt.idString;
            nng_rep0_open(&localSock);
            nng_setopt_ms(localSock, NNG_OPT_SENDTIMEO, 1000);
            nng_setopt_ms(localSock, NNG_OPT_RECVTIMEO, 1000);
            nng_listen(localSock, localUrl.c_str(), &localListener, 0);

            while (!mkt.haltFlag) {
                if (nng_recvmsg(localSock, &msg, NNG_FLAG_NONBLOCK)) {
                    Sleep(10);
                    continue;
                }
                uint16_t cmd, streamId;
                uint32_t writePtr, result = 0;
                LRMPMsgDumpEx((uint8_t*)nng_msg_body(msg), &cmd, &streamId, &writePtr);
                switch (cmd) {
                case 0:
                    nng_msg_chop(msg, nng_msg_len(msg));
                    LRMPMsgAppend(msg, &result);
                    break;
                case 1: {
                    uint32_t currentPtr = mkt.streams[streamId].streamWritePtr;
                    if (currentPtr == writePtr) {
                        mkt.streams[streamId].PushStream((uint8_t*)nng_msg_body(msg) + 8, nng_msg_len(msg) - 8);
                    }
                    nng_msg_chop(msg, nng_msg_len(msg));
                    LRMPMsgAppend(msg, &result);
                } break;
                case 2: {
                    uint32_t currentPtr1 = mkt.streams[streamId].streamWritePtr;
                    uint32_t currentPtr2 = mkt.streams[writePtr].streamWritePtr;
                    nng_msg_chop(msg, nng_msg_len(msg));
                    LRMPMsgAppend(msg, &currentPtr1, &currentPtr2);
                } break;
                case 3: {
                    char tmpBuf[32] {};
                    *(uint64_t*)(&tmpBuf[0]) = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now()).time_since_epoch().count();
                    tmpBuf[8] = MARKETEER_EXIT_GAME;
                    mkt.streams[streamId].PushStream(tmpBuf, 32);
                    nng_msg_chop(msg, nng_msg_len(msg));
                } break;
                default:
                    result = -1;
                    nng_msg_chop(msg, nng_msg_len(msg));
                    LRMPMsgAppend(msg, &result);
                    break;
                }
                nng_sendmsg(localSock, msg, 0);
            }

            nng_listener_close(localListener);
            nng_close(localSock);

            return 0;
        }
        static DWORD __stdcall PushThreadFunc(LPVOID param)
        {
            MarketeerStream& mkt = *(MarketeerStream*)param;
            nng_msg* clientReq = nullptr;
            nng_msg* serverRep = nullptr;

            if (!StaticConnect(param)) {
                return 0;
            }
            if (!StaticAuth(param)) {
                return 0;
            }
            if (!StaticStartStreamer(param)) {
                return 0;
            }

            GuiThread pushThread { PushLocalFunc };
            pushThread.Start(param);

            char localBuffer[512];
            while (!mkt.haltFlag) {
                bool isIdle = true;

                for (int i = 0; i < STREAM_COUNT; ++i) {
                    auto& stream = mkt.streams[i];
                    size_t streamWritePtr = stream.streamWritePtr;
                    if (stream.streamReadPtr != streamWritePtr) {
                        uint32_t streamTargetPtr = streamWritePtr;
                        while (stream.streamReadPtr < streamTargetPtr) {
                            isIdle = false;
                            auto streamDiff = std::min(480u, streamTargetPtr - stream.streamReadPtr);
                            stream.PullStream(stream.streamReadPtr, localBuffer, streamDiff);

                            clientReq = LRMPMsgCreate(ServerCmd::CLIENT_PUSH,
                                &mkt.marketeerCookie, &mkt.marketeerLiveId, &mkt.marketeerLiveIndex, &stream.streamId, &stream.streamRemotePtr,
                                std::pair((uint8_t*)(localBuffer), (size_t)streamDiff));
                            if (LRMPMsgSend(mkt.marketeerSock, clientReq, 0)) {
                                mkt.marketeerStatus = STATUS_ERROR_CONNECTION;
                                LRMPMsgFree(clientReq);
                                if (mkt.haltFlag) {
                                    break;
                                }
                                continue;
                            }
                            if (LRMPMsgRecv(mkt.marketeerSock, &serverRep, 0)) {
                                mkt.marketeerStatus = STATUS_ERROR_CONNECTION;
                                if (mkt.haltFlag) {
                                    break;
                                }
                                continue;
                            }

                            auto cmd = LRMPMsgGetCmd(serverRep);
                            if (cmd == ServerCmd::REP_ERR_INVALID_CRED) {
                                if (!StaticAuth(param)) {
                                    goto end;
                                }
                            } else if (cmd == ServerCmd::REP_ERR_INVALID_LIVE) {
                                goto end;
                            }

                            LRMPMsgFree(serverRep);
                            stream.streamReadPtr += streamDiff;
                            stream.streamRemotePtr += streamDiff;
                        }
                    }

                    if (mkt.haltFlag) {
                        break;
                    }
                }

                if (isIdle) {
                    Sleep(1000);
                }
            }

            end:
            mkt.haltFlag = true;
            pushThread.Wait();
            
            if (!StaticEndStreamer(param)) {
                return 0;
            }

            return 1;
        }

        
    public:
        struct ViewSignal {
            ViewSignal(MarketeerCmd _cmd, std::string& _game, uint32_t cursor, uint32_t _info, uint64_t _time)
                : cmd(_cmd)
                , game(_game)
                , cursorPos(cursor)
                , info(_info)
                , time(_time)
            {
            }
            MarketeerCmd cmd;
            std::string game;
            uint32_t cursorPos;
            uint32_t info;
            uint64_t time;
        };
    private:
        bool autoPlay { false };
        uint32_t cursorExpected { 0 };
        std::atomic<uint32_t> cursorForControl { 0 };
        std::atomic<uint32_t> sizeForGui { 0 };
        std::vector<ViewSignal> singalsForControl;
        std::mutex singalsMutex;
        std::string currentGameForControl;
        std::atomic<void*> handleForControl { 0 };
        uint32_t posForControl { 0 };
        std::mutex userMutex;
        uint32_t userSignal { 0 };
        uint32_t userParam { 0 };
        static DWORD __stdcall StartPlayback(LPVOID param)
        {
            MarketeerStream& mkt = *(MarketeerStream*)param;
            std::string ctx = "VIEW";
            ctx += "ipc://";
            ctx += mkt.GetIdStr();
            mkt.handleForControl = LauncherGamesExternalLaunch(mkt.currentGameForControl.c_str(), ctx);
            if (!mkt.handleForControl) {
                //mkt.marketeerStatus = STATUS_ERROR_LAUNCH_FAILED;
                return 0;
            }
            return 1;
        }
        static DWORD __stdcall StopPlayback(LPVOID param)
        {
            MarketeerStream& mkt = *(MarketeerStream*)param;
            void* handle = mkt.handleForControl;
            if (handle) {
                mkt.handleForControl = 0;
                TerminateProcess((HANDLE)handle, 0);
            }
            return 0;
        }
        static DWORD __stdcall LocalViewerFunc(LPVOID param)
        {
            MarketeerStream& mkt = *(MarketeerStream*)param;
            PullLocalFunc(param);
            mkt.marketeerStatus = STATUS_FINISHED;
            return 0;
        }
        static DWORD __stdcall ViewerCtrlThreadFunc(LPVOID param)
        {
            MarketeerStream& mkt = *(MarketeerStream*)param;
            char bufferforControl[32] {};
            std::string tmpGame;
            while (!mkt.haltFlag) {
                auto size = mkt.streams[0].PullStream(mkt.posForControl, bufferforControl, 32);
                if (size == 32) {
                    auto cmd = bufferforControl[8]; 
                    if (cmd == MarketeerCmd::MARKETEER_LAUNCH) {
                        tmpGame = &(bufferforControl[12]);
                    } 
                    if (cmd == MarketeerCmd::MARKETEER_LAUNCH || cmd == MarketeerCmd::MARKETEER_NEW_GAME || cmd == MarketeerCmd::MARKETEER_EXIT_GAME) {
                        mkt.singalsMutex.lock();
                        mkt.singalsForControl.emplace_back((MarketeerCmd)cmd, tmpGame, mkt.posForControl,
                            *(uint32_t*)&(bufferforControl[8]), *(uint64_t*)&(bufferforControl[0]));
                        mkt.singalsMutex.unlock();
                    }
                    mkt.posForControl += 32;
                    continue;
                }
                mkt.sizeForGui = mkt.singalsForControl.size();
                
                uint32_t userSignal = 0, userParam = 0;
                if (mkt.userMutex.try_lock()) {
                    userSignal = mkt.userSignal;
                    userParam = mkt.userParam;
                    mkt.userSignal = mkt.userParam = 0;
                    mkt.userMutex.unlock();
                }
                switch (userSignal) {
                case 1:
                    if (!mkt.autoPlay && mkt.singalsForControl.size() > userParam) {
                        StopPlayback(param);
                        mkt.cursorExpected = userParam + 1;
                        mkt.cursorForControl = userParam;
                        mkt.currentGameForControl = mkt.singalsForControl[userParam].game;
                        StartPlayback(param);
                    }
                    break;
                case 2:
                    StopPlayback(param);
                    break;
                case 3:
                    mkt.autoPlay = true;
                    break;
                case 4:
                    mkt.autoPlay = true;
                    break;
                default:
                    break;
                }
                if (mkt.autoPlay) {
                    if (userSignal == 4) {
                        mkt.autoPlay = false;
                    } else {
                        uint32_t cursor = mkt.cursorForControl;
                        void* handle = mkt.handleForControl;
                        if (mkt.cursorExpected <= cursor && mkt.singalsForControl.size() > cursor && !handle) {
                            mkt.cursorExpected = cursor + 1;
                            if (mkt.singalsForControl[cursor].cmd != MarketeerCmd::MARKETEER_EXIT_GAME) {
                                mkt.currentGameForControl = mkt.singalsForControl[cursor].game;
                                StopPlayback(param);
                                StartPlayback(param);
                            }
                        }
                    }
                }
                

                Sleep(200);
            }

            StopPlayback(param);
            return 1;
        }


        bool tabSwitch = false;
        std::string idString;
        std::atomic<bool> haltFlag = false;
        GuiThread pushThread { PushThreadFunc };
        GuiThread pullThread { PullThreadFunc };
        GuiThread localViewerThread { LocalViewerFunc };
        GuiThread viewerCtrlThread { ViewerCtrlThreadFunc };
        std::atomic<bool> pullWakeUpFlag { false };

        struct SubStream {
            uint16_t streamId = 0;

            FILE* streamFile = nullptr;
            char streamFileBuf[BUFSIZ] {};
            std::mutex streamFileMutex;

            std::atomic<uint32_t> streamWritePtr { 0 };
            uint32_t streamReadPtr = 0;
            uint32_t streamRemotePtr = 0;

            void PushStream(void* buffer, uint32_t size)
            {
                if (size) {
                    std::lock_guard<std::mutex> lock(streamFileMutex);
                    fseek(streamFile, 0, SEEK_END);
                    fwrite(buffer, size, 1, streamFile);
                    streamWritePtr += size;
                }
            }
            uint32_t PullStream(uint32_t pos, void* buffer, uint32_t size)
            {
                uint32_t currentPtr = streamWritePtr;
                if (!size || pos + size <= currentPtr) {
                    if (!size) {
                        size = std::min(480u, currentPtr - pos);
                    }
                    std::lock_guard<std::mutex> lock(streamFileMutex);
                    fseek(streamFile, pos, SEEK_SET);
                    auto objReads = fread(buffer, size, 1u, streamFile);
                    return objReads == 1 ? size : 0;
                }
                return 0;
            }
        };
        SubStream _streams[STREAM_COUNT];
        SubStream* streams = nullptr;

        void InternalInit()
        {
            if (!streams) {
                auto dataDir = LauncherGetDataDir();
                dataDir += L"\\marketeer";
                CreateDirectoryW(dataDir.c_str(), NULL);
                dataDir += L"\\ongoing";
                CreateDirectoryW(dataDir.c_str(), NULL);
                dataDir += L'\\';
                dataDir += utf8_to_utf16(idString);
                CreateDirectoryW(dataDir.c_str(), NULL);

                streams = _streams;
                for (uint16_t i = 0; i < STREAM_COUNT; ++i) {
                    auto& stream = streams[i];
                    stream.streamId = i;
                    char fileNameBuf[128];
                    sprintf_s(fileNameBuf, "%s\\%d.stream", utf16_to_utf8(dataDir).c_str(), stream.streamId);
                    stream.streamFile = fopen(fileNameBuf, "w+b");
                    setvbuf(stream.streamFile, stream.streamFileBuf, _IOFBF, BUFSIZ);
                }
            }
        }

    public:
        MarketeerStream() = default;
        ~MarketeerStream()
        {
            haltFlag = true;
            pullThread.Wait();
            pushThread.Wait();
            localViewerThread.Wait();
            viewerCtrlThread.Wait();
            for (int i = 0; i < STREAM_COUNT; ++i) {
                if (_streams[i].streamFile) {
                    fclose(_streams[i].streamFile);
                }
            }
            auto dataDir = LauncherGetDataDir();
            dataDir += L"\\marketeer";
            CreateDirectoryW(dataDir.c_str(), NULL);
            dataDir += L"\\ongoing";
            CreateDirectoryW(dataDir.c_str(), NULL);
            dataDir += L'\\';
            dataDir += utf8_to_utf16(idString);
            //DeleteFolder(dataDir);
        }

        void SetIdStr(std::string id)
        {
            idString = id;
        }
        void SetURL(const char* url)
        {
            memset(marketeerURL, 0, 256);
            memcpy_s(marketeerURL, 256, url, strlen(url));
        }
        void SetUsername(const char* username)
        {
            memset(marketeerUsername, 0, 16);
            memcpy_s(marketeerUsername, 16, username, std::max(8u, strlen(username)));
        }
        void SetKey(std::string& key)
        {
            marketeerPrivateKey = key;
            SSLGetInfoFromKey(key, marketeerPublicKey, marketeerFingerprint);
        }
        void SetFingerprint(std::string& fingerprint)
        {
            marketeerPrivateKey = marketeerPublicKey = "";
            marketeerFingerprint = fingerprint;
        }

        bool& TabSwitch()
        {
            return tabSwitch;
        }
        std::string GetIdStr()
        {
            return idString;
        }
        std::string GetURL()
        {
            return marketeerURL;
        }
        std::string GetUsername()
        {
            return marketeerUsername;
        }
        std::string GetKey()
        {
            return marketeerPrivateKey;
        }
        std::string GetFingerprint()
        {
            return marketeerFingerprint;
        }
        Status GetStatus()
        {
            int status = marketeerStatus;
            return (Status)status;
        }

        void ViewerControl(uint32_t signal, uint32_t param)
        {
            std::lock_guard<std::mutex> lock(userMutex);
            userParam = param;
            userSignal = signal;
        }
        std::vector<ViewSignal>& ViewerLockSingals()
        {
            singalsMutex.lock();
            return singalsForControl;
        }
        void ViewerUnlockSingals()
        {
            singalsMutex.unlock();
        }
        bool ViewerAutoplayStatus()
        {
            return autoPlay;
        }

        void LaunchCallback(const char* game)
        {
            char tmpBuffer[32] {};
            *(uint64_t*)(&tmpBuffer[0]) = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now()).time_since_epoch().count();
            tmpBuffer[8] = MarketeerCmd::MARKETEER_LAUNCH;
            strcpy(&(tmpBuffer[12]), game);
            *(uint32_t*)(&tmpBuffer[28]) = streams[1].streamWritePtr;
            streams[0].PushStream(tmpBuffer, 32);
        }

        void StartLocalViewer(std::wstring path)
        {
            if (!localViewerThread.IsActive()) {
                marketeerStatus = STATUS_STREAMING;
                wchar_t tmpStr[256];
                streams = _streams;
                for (uint16_t i = 0; i < STREAM_COUNT; ++i) {
                    auto& stream = streams[i];
                    stream.streamId = i;
                    swprintf_s(tmpStr, L"%s\\%d.stream", path.c_str(), i);
                    stream.streamFile = fopen(utf16_to_utf8(std::wstring(tmpStr)).c_str(), "rb");
                    setvbuf(stream.streamFile, stream.streamFileBuf, _IOFBF, BUFSIZ);
                    fseek(stream.streamFile, 0, SEEK_END);
                    stream.streamWritePtr = ftell(stream.streamFile);
                }
                localViewerThread.Stop();
                localViewerThread.Start(this);
                viewerCtrlThread.Stop();
                viewerCtrlThread.Start(this);
                type = MKT_VIEWER;
            }
        }
        void StartViewer()
        {
            if (!pullThread.IsActive()) {
                marketeerStatus = STATUS_CONNECTING;
                InternalInit();
                pullThread.Stop();
                pullThread.Start(this);
                viewerCtrlThread.Stop();
                viewerCtrlThread.Start(this);
                type = MKT_VIEWER;
            }
        }
        void StartStreamer()
        {
            if (!pushThread.IsActive()) {
                marketeerStatus = STATUS_CONNECTING;
                InternalInit();
                pushThread.Stop();
                pushThread.Start(this);
                type = MKT_STREAMER;
            }
        }
        void Halt()
        {
            haltFlag = true;
            //pullThread.Wait();
            //pushThread.Wait();
        }
    };

    std::string GenStreamName(std::string& username)
    {
        std::string name = username;
        char timeStr[128];

        SYSTEMTIME time;
        GetLocalTime(&time);
        sprintf_s(timeStr, "-%d-%d-%d-%d-%d-%d-%d", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond, time.wMilliseconds);
        name += timeStr;

        return name;
    }

    struct Server {
        Server()
            : name(), address(), desc()
        {
        
        }

        Server(Server&& source) {
            name = std::move(source.name);
            address = std::move(source.address);
            desc = std::move(source.desc);
            isHidden = source.isHidden;
            isEditing = source.isEditing;
        }

        Server(std::string _name, std::string _address, std ::string _desc)
            : name(_name) , address(_address), desc(_desc)
        {
        
        }
        bool isHidden = false;
        bool isEditing = false;
        bool toRemove = false;
        std::string name;
        std::string address;
        std::string desc;
        std::atomic<int> status { 0 };
    };
    struct Player {
        Player() = default;
        Player(std::string _username, std::string _fingerprint, std::string _server, std::string _serverURL, std::string _nowPlaying = "", std::string _tag = "")
            : username(_username)
            , fingerprint(_fingerprint)
            , server(_server)
            , serverURL(_serverURL)
            , nowPlaying(_nowPlaying)
            , tag(_tag)
        {
        }
        bool isSelected = false;
        bool isHidden = true;
        std::string username;
        std::string fingerprint;
        std::string server;
        std::string serverURL;
        std::string nowPlaying;
        std::string tag;
    };
    struct ConnView {
        ConnView() = default;
        enum class Type {
            _RSV = 0,
            STREAMER = 1,
            VIEWER = 2,
        };
        Type type = Type::_RSV;
        std::string server;
        std::string username;
        std::string fingerprint;
        std::string key;
        std::atomic<int> status;
        std::atomic<bool> termination;
        MarketeerStream stream;
    };
}

struct MarketeerAdminConsole {
    char InputBuf[256];
    ImVector<char*> Items;
    ImVector<char*> History;
    int HistoryPos; // -1: new line, 0..History.Size-1 browsing history.
    bool AutoScroll;
    bool ScrollToBottom;

    MarketeerAdminConsole()
    {
        ClearLog();
        memset(InputBuf, 0, sizeof(InputBuf));
        HistoryPos = -1;

        AutoScroll = true;
        ScrollToBottom = false;

        //strcpy(marketeerURL, "tcp://v8f6s0.com:9961");
        //strcpy(marketeerAdminPassword, "8se.He;wN26J'T%\"");
    }
    ~MarketeerAdminConsole()
    {
        ClearLog();
        for (int i = 0; i < History.Size; i++)
            free(History[i]);
    }

private:
    std::string marketeerCommand;
    std::string marketeerRelayBuf;
    std::atomic<int> marketeerStatus { 0 };
    char marketeerURL[256] {};
    char marketeerAdminPassword[32] {};
    static DWORD __stdcall ExecCmdFunc(LPVOID param)
    {
        MarketeerAdminConsole& mkt = *(MarketeerAdminConsole*)param;
        nng_socket marketeerSock {};
        nng_dialer marketeerDialer {};
        nng_msg* clientReq = nullptr;
        nng_msg* serverRep = nullptr;

        if (!mkt.marketeerURL[0] || !mkt.marketeerAdminPassword[0] || !mkt.marketeerCommand.size()) {
            mkt.marketeerStatus = -3;
            return 0;
        }

        nng_req0_open(&marketeerSock);
        nng_setopt_ms(marketeerSock, NNG_OPT_SENDTIMEO, 10000);
        nng_setopt_ms(marketeerSock, NNG_OPT_RECVTIMEO, 10000);

        if (!nng_dial(marketeerSock, mkt.marketeerURL, &marketeerDialer, 0)) {
            clientReq = LRMPMsgCreate(ServerCmd::CLIENT_ADMIN_CMD,
                std::pair(mkt.marketeerAdminPassword, (size_t)16),
                std::pair(mkt.marketeerCommand.data(), (size_t)mkt.marketeerCommand.size() + 1));
            if (!LRMPMsgSend(marketeerSock, clientReq, 0)) {
                if (!LRMPMsgRecv(marketeerSock, &serverRep, 0)) {
                    auto cmd = LRMPMsgGetCmd(serverRep);
                    if (cmd == ServerCmd::REP_OK) {
                        mkt.marketeerRelayBuf = (char*)LRMPMsgBodyPtr(serverRep);
                        mkt.marketeerStatus = 1;
                    } else {
                        mkt.marketeerStatus = -2;
                    }
                    LRMPMsgFree(serverRep);
                } else {
                    mkt.marketeerStatus = -1;
                }
            } else {
                LRMPMsgFree(clientReq);
                mkt.marketeerStatus = -1;
            }
            LRMPClose(marketeerSock, marketeerDialer);
        } else {
            mkt.marketeerStatus = -1;
        }

        return 0;
    }
    GuiThread mExecCmdThread { ExecCmdFunc };

public:
    // Portable helpers
    static int Stricmp(const char* s1, const char* s2)
    {
        int d;
        while ((d = toupper(*s2) - toupper(*s1)) == 0 && *s1) {
            s1++;
            s2++;
        }
        return d;
    }
    static int Strnicmp(const char* s1, const char* s2, int n)
    {
        int d = 0;
        while (n > 0 && (d = toupper(*s2) - toupper(*s1)) == 0 && *s1) {
            s1++;
            s2++;
            n--;
        }
        return d;
    }
    static char* Strdup(const char* s)
    {
        IM_ASSERT(s);
        size_t len = strlen(s) + 1;
        void* buf = malloc(len);
        IM_ASSERT(buf);
        return (char*)memcpy(buf, (const void*)s, len);
    }
    static void Strtrim(char* s)
    {
        char* str_end = s + strlen(s);
        while (str_end > s && str_end[-1] == ' ')
            str_end--;
        *str_end = 0;
    }

    void ClearLog()
    {
        for (int i = 0; i < Items.Size; i++)
            free(Items[i]);
        Items.clear();
    }

    void AddLog(const char* fmt, ...) IM_FMTARGS(2)
    {
        // FIXME-OPT
        char buf[1024];
        va_list args;
        va_start(args, fmt);
        vsnprintf(buf, IM_ARRAYSIZE(buf), fmt, args);
        buf[IM_ARRAYSIZE(buf) - 1] = 0;
        va_end(args);
        Items.push_back(Strdup(buf));
    }

    void Draw(const char* title)
    {
        // As a specific feature guaranteed by the library, after calling Begin() the last Item represent the title bar.
        // So e.g. IsItemHovered() will return true when hovering the title bar.
        // Here we create a context menu only available from the title bar.
        if (ImGui::BeginPopupContextItem()) {
            //if (ImGui::MenuItem("Close Console"))
            //    *p_open = false;
            ImGui::EndPopup();
        }

        bool copy_to_clipboard = false;
        ImGui::PushItemWidth(300.0f);
        ImGui::InputText("Server", marketeerURL, 256);
        ImGui::PopItemWidth();
        ImGui::SameLine();
        ImGui::PushItemWidth(200.0f);
        ImGui::InputText("Admin password", marketeerAdminPassword, 17, ImGuiInputTextFlags_Password);
        ImGui::PopItemWidth();
        ImGui::SameLine();
        if (ImGui::Button("Clear")) {
            ClearLog();
        }
        ImGui::SameLine();
        ImGui::Checkbox("Auto-scroll", &AutoScroll);
        ImGui::Separator();

        // Reserve enough left-over height for 1 separator + 1 input text
        const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
        ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footer_height_to_reserve), false, ImGuiWindowFlags_HorizontalScrollbar);
        if (ImGui::BeginPopupContextWindow()) {
            if (ImGui::Selectable("Clear"))
                ClearLog();
            ImGui::EndPopup();
        }

        // Display every line as a separate entry so we can change their color or add custom widgets.
        // If you only want raw text you can use ImGui::TextUnformatted(log.begin(), log.end());
        // NB- if you have thousands of entries this approach may be too inefficient and may require user-side clipping
        // to only process visible items. The clipper will automatically measure the height of your first item and then
        // "seek" to display only items in the visible area.
        // To use the clipper we can replace your standard loop:
        //      for (int i = 0; i < Items.Size; i++)
        //   With:
        //      ImGuiListClipper clipper;
        //      clipper.Begin(Items.Size);
        //      while (clipper.Step())
        //         for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
        // - That your items are evenly spaced (same height)
        // - That you have cheap random access to your elements (you can access them given their index,
        //   without processing all the ones before)
        // You cannot this code as-is if a filter is active because it breaks the 'cheap random-access' property.
        // We would need random-access on the post-filtered list.
        // A typical application wanting coarse clipping and filtering may want to pre-compute an array of indices
        // or offsets of items that passed the filtering test, recomputing this array when user changes the filter,
        // and appending newly elements as they are inserted. This is left as a task to the user until we can manage
        // to improve this example code!
        // If your items are of variable height:
        // - Split them into same height items would be simpler and facilitate random-seeking into your list.
        // - Consider using manual call to IsRectVisible() and skipping extraneous decoration from your items.
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1)); // Tighten spacing
        if (copy_to_clipboard)
            ImGui::LogToClipboard();
        for (int i = 0; i < Items.Size; i++) {
            const char* item = Items[i];
            //if (!Filter.PassFilter(item))
            //    continue;

            // Normally you would store more information in your item than just a string.
            // (e.g. make Items[] an array of structure, store color/type etc.)
            ImVec4 color;
            bool has_color = false;
            if (strstr(item, "[error]")) {
                color = ImVec4(1.0f, 0.4f, 0.4f, 1.0f);
                has_color = true;
            } else if (strncmp(item, "# ", 2) == 0) {
                color = ImVec4(1.0f, 0.8f, 0.6f, 1.0f);
                has_color = true;
            }
            if (has_color)
                ImGui::PushStyleColor(ImGuiCol_Text, color);
            ImGui::TextUnformatted(item);
            if (has_color)
                ImGui::PopStyleColor();
        }
        if (copy_to_clipboard)
            ImGui::LogFinish();

        if (ScrollToBottom || (AutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()))
            ImGui::SetScrollHereY(1.0f);
        ScrollToBottom = false;

        ImGui::PopStyleVar();
        ImGui::EndChild();
        ImGui::Separator();

        bool disabled = mExecCmdThread.IsActive();
        if (!disabled && marketeerStatus) {
            switch (marketeerStatus) {
            case -3:
                AddLog("[error] Invalid server/password settings.");
                break;
            case -2:
                AddLog("[error] Server error.");
                break;
            case -1:
                AddLog("[error] Error while excuting command on target server.");
                break;
            case 1:
                AddLog(marketeerRelayBuf.c_str());
                break;
            default:
                break;
            }
            marketeerStatus = 0;
        }

        // Command-line
        bool reclaim_focus = false;
        ImGuiInputTextFlags input_text_flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackHistory;
        if (disabled) {
            ImGui::BeginDisabled();
        }
        if (ImGui::InputText("Input", InputBuf, IM_ARRAYSIZE(InputBuf), input_text_flags, &TextEditCallbackStub, (void*)this)) {
            char* s = InputBuf;
            Strtrim(s);
            if (s[0]) {
                marketeerCommand = s;
                ExecCommand(s);
            }
            strcpy(s, "");
            reclaim_focus = true;
        }
        if (disabled) {
            ImGui::EndDisabled();
        }

        // Auto-focus on window apparition
        ImGui::SetItemDefaultFocus();
        if (reclaim_focus)
            ImGui::SetKeyboardFocusHere(-1); // Auto focus previous widget
    }

    void ExecCommand(const char* command_line)
    {
        AddLog("# %s\n", command_line);

        // Insert into history. First find match and delete it so it can be pushed to the back.
        // This isn't trying to be smart or optimal.
        HistoryPos = -1;
        for (int i = History.Size - 1; i >= 0; i--)
            if (Stricmp(History[i], command_line) == 0) {
                free(History[i]);
                History.erase(History.begin() + i);
                break;
            }
        History.push_back(Strdup(command_line));

        // Process command
        mExecCmdThread.Stop();
        mExecCmdThread.Start(this);

        // On command input, we scroll to bottom even if AutoScroll==false
        ScrollToBottom = true;
    }

    // In C++11 you'd be better off using lambdas for this sort of forwarding callbacks
    static int TextEditCallbackStub(ImGuiInputTextCallbackData* data)
    {
        MarketeerAdminConsole* console = (MarketeerAdminConsole*)data->UserData;
        return console->TextEditCallback(data);
    }

    int TextEditCallback(ImGuiInputTextCallbackData* data)
    {
        // AddLog("cursor: %d, selection: %d-%d", data->CursorPos, data->SelectionStart, data->SelectionEnd);
        switch (data->EventFlag) {
        case ImGuiInputTextFlags_CallbackHistory: {
            // Example of HISTORY
            const int prev_history_pos = HistoryPos;
            if (data->EventKey == ImGuiKey_UpArrow) {
                if (HistoryPos == -1)
                    HistoryPos = History.Size - 1;
                else if (HistoryPos > 0)
                    HistoryPos--;
            } else if (data->EventKey == ImGuiKey_DownArrow) {
                if (HistoryPos != -1)
                    if (++HistoryPos >= History.Size)
                        HistoryPos = -1;
            }

            // A better implementation would preserve the data on the current input line along with cursor position.
            if (prev_history_pos != HistoryPos) {
                const char* history_str = (HistoryPos >= 0) ? History[HistoryPos] : "";
                data->DeleteChars(0, data->BufTextLen);
                data->InsertChars(0, history_str);
            }
        } break;
        default:
            break;
        }
        return 0;
    }
};

class THMarketeerGui {
private:
    THMarketeerGui()
    {
        SettingsInit();
        mGuiUpdFunc = [&]() { return GuiContent(); };
        //mServerList.emplace_back("TWC Server", "deafbeef.aaa:9961", "Something blah blah blah");
        //mServerList.emplace_back("Custom Server", "12.34.56.78:9961", "blah blah blah");
        mServerList.emplace_back("Debug Server", "ipc://marketeer_test", "Cool"); //ipc://marketeer_test
        mSelectedServer = mServerList.end();
        mServerListFilteredSize = 1;
        mSelectedPlayer = mPlayerList.end();
        StartRefreshThread();

        auto dataDir = LauncherGetDataDir();
        dataDir += L"\\streams";
        DeleteFolder(dataDir);
    }
    SINGLETON(THMarketeerGui);

public:
    bool IsStreaming()
    {
        if (mStreams.size()) {
            for (auto& stream : mStreams) {
                if (stream.GetStatus() >= 0) {
                    return true;
                }
            }
        }
        return false;
    }
    void GuiUpdate()
    {
        GuiMain();
    }
    std::string& GetCtx()
    {
        return mStreamCtx;
    }
    void LaunchCallback(const char* game)
    {
        if (mStreams.size()) {
            mStreams.front().LaunchCallback(game);
        }
    }
    void TerminateAll()
    {
        for (auto& stream : mStreams) {
            stream.Halt();
        }
        mStreams.clear();
        for (auto& view : mViews) {
            view.Halt();
        }
        mViews.clear();
    }

private:
    static void __stdcall ParseListJson(Marketeer::Server& server, const char* jsonStr, size_t size)
    {
        auto& mkt = THMarketeerGui::singleton();
        rapidjson::Document json;
        if (json.Parse(jsonStr, size).HasParseError()) {
            return;
        }
        if (json.HasMember("players") && json["players"].IsArray()) {
            auto& players = json["players"];
            for (auto playerIt = players.Begin(); playerIt != players.End(); ++playerIt) {
                auto& player = *playerIt;
                if (player.IsObject() && player.HasMember("username") && player["username"].IsString()
                    && player.HasMember("fingerprint") && player["fingerprint"].IsString()) {
                    std::string username = player["username"].GetString();
                    std::string fingerprint = player["fingerprint"].GetString();
                    mkt.mPlayerListMutex.lock();
                    mkt.mPlayerList.emplace_back(username, fingerprint, server.name, server.address);
                    mkt.mPlayerListMutex.unlock();
                }
            }
            mkt.mPlayerListMutex.lock();
            mkt.PlayerFilter();
            mkt.mPlayerListMutex.unlock();
        }
    }
    static DWORD __stdcall RefreshServer(LPVOID param)
    {
        auto& mkt = THMarketeerGui::singleton();
        auto& server = *(Marketeer::Server*)param;

        server.status = 1;
        nng_socket marketeerSock;
        nng_dialer marketeerDialer;
        nng_req0_open(&marketeerSock);
        nng_setopt_ms(marketeerSock, NNG_OPT_SENDTIMEO, 5000);
        nng_setopt_ms(marketeerSock, NNG_OPT_RECVTIMEO, 5000);
        if (!nng_dial(marketeerSock, server.address.c_str(), &marketeerDialer, 0)) {
            nng_msg* clientReq = nullptr;
            nng_msg* serverRep = nullptr;

            clientReq = LRMPMsgCreate(ServerCmd::CLIENT_PULL_PLAYERS);
            if (!LRMPMsgSend(marketeerSock, clientReq, 0)) {
                if (!LRMPMsgRecv(marketeerSock, &serverRep, 0)) {
                    ParseListJson(server, (char*)LRMPMsgBodyPtr(serverRep), LRMPMsgBodyLen(serverRep));
                    LRMPMsgFree(serverRep);
                    server.status = 2;
                } else {
                    server.status = -2;
                }
            } else {
                LRMPMsgFree(clientReq);
                server.status = -1;
            }

            nng_dialer_close(marketeerDialer);
            nng_close(marketeerSock);
        } else {
            server.status = -1;
        }

        return 0;
    }
    static DWORD __stdcall RefreshThread(LPVOID param)
    {
        auto& mkt = THMarketeerGui::singleton();

        mkt.mPlayerListMutex.lock();
        mkt.mPlayerListFilteredSize = 0;
        mkt.mPlayerList.clear();
        mkt.mSelectedPlayer = mkt.mPlayerList.end();
        mkt.mPlayerListMutex.unlock();

        if (mkt.mServerList.size()) {
            std::vector<std::unique_ptr<GuiThread>> updateThreads;
            for (auto& server : mkt.mServerList) {
                updateThreads.emplace_back(std::make_unique<GuiThread>(RefreshServer));
                updateThreads.back()->Start(&server);
            }
            for (auto& t : updateThreads) {
                t->Wait();
            }
        }

        return 0;
    }
    void StartRefreshThread()
    {
        if (!refershThread.IsActive()) {
            refershThread.Stop();
            refershThread.Start();
        }
    }

    void ServerFilter(std::string filter)
    {
        mServerListFilteredSize = 0;
        auto contains = [](std::string& a, std::string& b) {
            return std::search(a.begin(), a.end(),
                       b.begin(), b.end(), [](char x, char y) { return std::tolower(x) == std::tolower(y); })
                != a.end();
        };
        bool isFilterEmpty = filter == "";
        for (auto& server : mServerList) {
            if (isFilterEmpty) {
                goto search_hit;
            }
            if (contains(server.name, filter) || 
                contains(server.address, filter) ||
                contains(server.desc, filter)) {
search_hit:
                server.isHidden = false;
                mServerListFilteredSize++;
                continue;
            }
            server.isHidden = true;
        }
    }
    void ServerView()
    {
        bool tableCtxMenu = false;
        auto tableFlag = ImGuiTableFlags_Resizable | ImGuiTableFlags_Sortable | ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersV | ImGuiTableFlags_BordersOuterV | ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersOuterH | ImGuiTableFlags_BordersInnerH | ImGuiTableFlags_ScrollX | ImGuiTableFlags_ScrollY;
        static char filterInput[256];

        static char serverNameInput[64];
        static char serverAddressInput[64];
        static char serverDescInput[64];

        if (ImGui::InputText("Filter", filterInput, 256)) {
            ServerFilter(filterInput);
        } else {
            mServerListFilteredSize = mServerList.size();
        }
        if (ImGui::BeginTable("mkt_server_table", 4, tableFlag)) {
            ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed, 128.0f, 0);
            ImGui::TableSetupColumn("Address", ImGuiTableColumnFlags_WidthFixed, 256.0f, 1);
            ImGui::TableSetupColumn("Description", ImGuiTableColumnFlags_WidthStretch, 0.0f, 2);
            ImGui::TableSetupColumn("Status", ImGuiTableColumnFlags_WidthFixed, 96.0f, 3);
            ImGui::TableSetupScrollFreeze(1, 1);
            ImGui::TableHeadersRow();

            ImGuiTableSortSpecs* sorts_specs = ImGui::TableGetSortSpecs();
            static bool items_need_sort = false;
            if (sorts_specs && sorts_specs->SpecsDirty)
                items_need_sort = true;
            if (sorts_specs && items_need_sort && mServerList.size() > 1) {
                const ImGuiTableColumnSortSpecs* colSort = sorts_specs->Specs;
                mServerList.sort([&](const Marketeer::Server& a, const Marketeer::Server& b) -> bool {
                    auto isAscending = colSort->SortDirection == ImGuiSortDirection_Ascending;
                    switch (colSort->ColumnUserID) {
                    case 0:
                        return isAscending ? a.name < b.name : a.name > b.name;
                    case 1:
                        return isAscending ? a.address < b.address : a.address > b.address;
                    case 2:
                        return isAscending ? a.desc < b.desc : a.desc > b.desc;
                    case 3:
                        return isAscending ? a.status < b.status : a.status > b.status;
                    default:
                        return false;
                    }
                });
                sorts_specs->SpecsDirty = false;
            }
            items_need_sort = false;

            ImGuiListClipper clipper;
            if (mServerListFilteredSize) {
                auto serverIt = mServerList.begin();
                clipper.Begin(mServerListFilteredSize);
                while (clipper.Step()) {
                    for (int row_n = clipper.DisplayStart; row_n < clipper.DisplayEnd; row_n++, serverIt++) {
                        while (serverIt->isHidden) {
                            serverIt++;
                        }

                        ImGui::TableNextRow(ImGuiTableRowFlags_None);
                        ImGui::TableSetColumnIndex(0);

                        bool currentSelected = serverIt == mSelectedServer;
                        ImGui::Selectable(serverIt->name.c_str(), &currentSelected, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowItemOverlap);
                        if (currentSelected) {
                            mSelectedServer = serverIt;
                        }

                        if (!tableCtxMenu) {
                            if (currentSelected && ImGui::BeginPopupContextWindow()) {
                                if (!mStreams.size()) {
                                    if (ImGui::Selectable("Stream")) {
                                        auto& stream = mStreams.emplace_back();

                                        auto streamName = Marketeer::GenStreamName(mUsername);
                                        stream.TabSwitch() = true;
                                        stream.SetIdStr(streamName);
                                        stream.SetURL(serverIt->address.c_str());
                                        stream.SetUsername(mUsername.c_str());
                                        stream.SetKey(mPrivateKey);
                                        stream.StartStreamer();

                                        mStreamCtx = "STRM";
                                        mStreamCtx += "ipc://";
                                        mStreamCtx += streamName;
                                        mStreamCtx += '\0';
                                    }
                                    ImGui::Separator();
                                }
                                if (ImGui::Selectable("Edit")) {
                                    serverIt->isEditing = true;
                                }
                                if (ImGui::Selectable("Remove")) {
                                    serverIt->toRemove = true;
                                }
                                ImGui::Separator();
                                if (ImGui::Selectable("Add server")) {
                                    Marketeer::Server newServer;
                                    newServer.isEditing = true;
                                    mServerList.push_back(std::move(newServer));
                                }
                                ImGui::EndPopup();
                                tableCtxMenu = true;
                                mSelectedServer = serverIt;
                            }                             
                            
                            if (serverIt->isEditing) {
                                ImGui::OpenPopup("Edit");
                                if (GuiModal("Edit")) {
                                    ImGui::InputText("Name", serverNameInput, 64);
                                    ImGui::InputText("Address", serverAddressInput, 64);
                                    ImGui::InputText("Description", serverDescInput, 64);
                                    if (ImGui::Button("Apply")) {
                                        serverIt->name = serverNameInput;
                                        serverIt->address = serverAddressInput;
                                        serverIt->desc = serverDescInput;
                                        serverIt->isEditing = false;
                                    }
                                    ImGui::SameLine();
                                    if (ImGui::Button("Cancel")) {
                                        serverIt->isEditing = false;
                                    }
                                }
                                ImGui::EndPopup();
                            }
                        }

                        if (ImGui::TableSetColumnIndex(1)) {
                            ImGui::TextUnformatted("---"); // serverIt->address.c_str()
                        }

                        if (ImGui::TableSetColumnIndex(2)) {
                            ImGui::TextUnformatted(serverIt->desc.c_str());
                        }

                        if (ImGui::TableSetColumnIndex(3)) {
                            switch (serverIt->status) {
                            case -2:
                                ImGui::TextUnformatted("Error");
                                break;
                            case -1:
                                ImGui::TextUnformatted("Failed");
                                break;
                            case 1:
                                ImGui::TextUnformatted("Refreshing");
                                break;
                            case 2:
                                ImGui::TextUnformatted("Alive");
                                break;
                            default:
                                ImGui::TextUnformatted("");
                                break;
                            }
                        }
                    }
                }
            }

            if (!tableCtxMenu) {
                if (ImGui::BeginPopupContextWindow()) {
                    if (ImGui::Selectable("Add Server")) {
                        Marketeer::Server newServer;
                        newServer.isEditing = true;
                        mServerList.push_back(std::move(newServer));
                    }
                    ImGui::EndPopup();
                    tableCtxMenu = true;
                }
            }
            ImGui::EndTable();
        }
        bool removedAServer = false;
        mServerList.remove_if([&removedAServer](Marketeer::Server& n) {
            if (n.toRemove) {
                removedAServer = true;
                return true;
            } else {
                return false;
            }
        });
        if (removedAServer) {
            mSelectedServer = mServerList.end();
        }
    }

    bool PlayerCtxMenu(int type, int* result = nullptr)
    {
        return false;
        if (type == 0) {
            if (!ImGui::BeginPopupContextWindow()) {
                return false;
            }
        } else {
            if (!ImGui::BeginPopupContextItem()) {
                return false;
            }
        }

        if (type == 1) {
            if (ImGui::Selectable("Watch")) {
            }
        }

        if (ImGui::Selectable("...")) {
        }
        ImGui::EndPopup();

        return true;
    }
    void PlayerFilter()
    {
        std::string filter = mPlayerFilter;
        mPlayerListFilteredSize = 0;
        auto contains = [](std::string& a, std::string& b) {
            return std::search(a.begin(), a.end(),
                       b.begin(), b.end(), [](char x, char y) { return std::tolower(x) == std::tolower(y); })
                != a.end();
        };
        auto radio = mPlayerFilterRadio;
        bool isFilterEmpty = filter == "";
        for (auto& player : mPlayerList) {
            if (isFilterEmpty) {
                goto search_hit;
            }
            bool hit = false;
            if (!hit) {
                hit |= (!radio || radio == 1) ? contains(player.username, filter) : false;
            }
            if (!hit) {
                hit |= (!radio || radio == 2) ? contains(player.nowPlaying, filter) : false;
            }
            if (!hit) {
                hit |= (!radio || radio == 3) ? contains(player.tag, filter) : false;
            }
            if (!hit) {
                hit |= (!radio || radio == 4) ? contains(player.server, filter) : false;
            }
            if (!hit) {
                hit |= (!radio || radio == 5) ? contains(player.fingerprint, filter) : false;
            }

            if (hit) {
search_hit:
                player.isHidden = false;
                mPlayerListFilteredSize++;
                continue;
            }

            player.isHidden = true;
        }
    }
    void PlayerView()
    {
        std::lock_guard<std::mutex> lock(mPlayerListMutex);
        bool tableCtxMenu = false;
        auto tableFlag = ImGuiTableFlags_Resizable | ImGuiTableFlags_Sortable | ImGuiTableFlags_RowBg | 
            ImGuiTableFlags_BordersV | ImGuiTableFlags_BordersOuterV | ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersOuterH |
            ImGuiTableFlags_BordersInnerH | ImGuiTableFlags_ScrollX | ImGuiTableFlags_ScrollY;

        auto isRefreshing = refershThread.IsActive();
        bool needFilterUpdate = false;
        if (isRefreshing) {
            ImGui::BeginDisabled();
        }
        needFilterUpdate |= ImGui::InputText("Filter", mPlayerFilter, 256);
        needFilterUpdate |= ImGui::RadioButton("All", &mPlayerFilterRadio, 0);
        ImGui::SameLine();
        needFilterUpdate |= ImGui::RadioButton("Username", &mPlayerFilterRadio, 1);
        ImGui::SameLine();
        needFilterUpdate |= ImGui::RadioButton("Now playing", &mPlayerFilterRadio, 2);
        ImGui::SameLine();
        needFilterUpdate |= ImGui::RadioButton("Tag", &mPlayerFilterRadio, 3);
        ImGui::SameLine();
        needFilterUpdate |= ImGui::RadioButton("Server", &mPlayerFilterRadio, 4);
        ImGui::SameLine();
        needFilterUpdate |= ImGui::RadioButton("Fingerprint", &mPlayerFilterRadio, 5);
        if (needFilterUpdate) {
            PlayerFilter();
        }
        if (isRefreshing) {
            ImGui::EndDisabled();
        }

        if (ImGui::BeginTable("mkt_player_table", 5, tableFlag)) {
            ImGui::TableSetupColumn("Username", ImGuiTableColumnFlags_WidthFixed, 128.0f, 0);
            ImGui::TableSetupColumn("Now playing", ImGuiTableColumnFlags_WidthFixed, 128.0f, 1);
            ImGui::TableSetupColumn("Tag", ImGuiTableColumnFlags_WidthFixed, 128.0f, 2);
            ImGui::TableSetupColumn("Server", ImGuiTableColumnFlags_WidthFixed, 192.0f, 3);
            ImGui::TableSetupColumn("Fingerprint", ImGuiTableColumnFlags_WidthStretch, 0.0f, 4);
            ImGui::TableSetupScrollFreeze(1, 1);
            ImGui::TableHeadersRow();

            ImGuiTableSortSpecs* sorts_specs = ImGui::TableGetSortSpecs();
            static bool items_need_sort = false;
            if (sorts_specs && sorts_specs->SpecsDirty)
                items_need_sort = true;
            if (sorts_specs && items_need_sort && mPlayerList.size() > 1) {
                const ImGuiTableColumnSortSpecs* colSort = sorts_specs->Specs;
                mPlayerList.sort([&](const Marketeer::Player& a, const Marketeer::Player& b) -> bool {
                    auto isAscending = colSort->SortDirection == ImGuiSortDirection_Ascending;
                    switch (colSort->ColumnUserID) {
                    case 0:
                        return isAscending ? a.username < b.username : a.username > b.username;
                    case 1:
                        return isAscending ? a.nowPlaying < b.nowPlaying : a.nowPlaying > b.nowPlaying;
                    case 2:
                        return isAscending ? a.tag < b.tag : a.tag > b.tag;
                    case 3:
                        return isAscending ? a.server < b.server : a.server > b.server;
                    case 4:
                        return isAscending ? a.fingerprint < b.fingerprint : a.fingerprint > b.fingerprint;
                    default:
                        return false;
                    }
                });
                sorts_specs->SpecsDirty = false;
            }
            items_need_sort = false;

            ImGuiListClipper clipper;
            if (mPlayerListFilteredSize) {
                auto playerIt = mPlayerList.begin();
                clipper.Begin(mPlayerListFilteredSize);
                while (clipper.Step()) {
                    for (int row_n = clipper.DisplayStart; row_n < clipper.DisplayEnd; row_n++, playerIt++) {
                        while (playerIt->isHidden) {
                            playerIt++;
                        }

                        ImGui::TableNextRow(ImGuiTableRowFlags_None);

                        ImGui::TableSetColumnIndex(0);
                        std::string selectableName = playerIt->username;
                        selectableName += "##";
                        selectableName += playerIt->fingerprint;
                        if (ImGui::Selectable(selectableName.c_str(), playerIt == mSelectedPlayer, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowItemOverlap)) {
                            mSelectedPlayer = playerIt;
                        }
                        if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
                            auto& stream = mViews.emplace_back();
                            auto streamName = Marketeer::GenStreamName(playerIt->username);
                            stream.TabSwitch() = true;
                            stream.SetIdStr(streamName);
                            stream.SetURL(playerIt->serverURL.c_str());
                            stream.SetUsername(playerIt->username.c_str());
                            stream.SetFingerprint(playerIt->fingerprint);
                            stream.StartViewer();
                        }
                        if (!tableCtxMenu) {
                            tableCtxMenu = PlayerCtxMenu(1);
                            if (tableCtxMenu) {
                                mSelectedPlayer = playerIt;
                            }
                        }

                        if (ImGui::TableSetColumnIndex(1)) {
                            ImGui::TextUnformatted(playerIt->nowPlaying.c_str());
                        }
                        if (ImGui::TableSetColumnIndex(2)) {
                            ImGui::TextUnformatted(playerIt->tag.c_str());
                        }
                        if (ImGui::TableSetColumnIndex(3)) {
                            ImGui::TextUnformatted(playerIt->server.c_str());
                        }
                        if (ImGui::TableSetColumnIndex(4)) {
                            ImGui::TextUnformatted(playerIt->fingerprint.c_str());
                        }
                    }
                }
            }

            if (!tableCtxMenu) {
                tableCtxMenu = PlayerCtxMenu(0);
            }
            ImGui::EndTable();
        }
    }

    bool StreamerTab(Marketeer::MarketeerStream& stream)
    {
        bool result = false;

        int flag = (stream.TabSwitch() ? ImGuiTabItemFlags_SetSelected : 0);
        if (flag) {
            stream.TabSwitch() = false;
        }

        char tabTitle[256];
        sprintf_s(tabTitle, "Streaming: %s##%s", stream.GetUsername().c_str(), stream.GetIdStr().c_str());
        if (ImGui::BeginTabItem(tabTitle, nullptr, flag)) {
            char tempTxt[256];
            sprintf_s(tempTxt, "Streaming to %s with identity: %s [%s]", stream.GetURL().c_str(), stream.GetUsername().c_str(), stream.GetFingerprint().c_str());
            GuiSetPosYRel(0.4f);
            GuiCenteredText(tempTxt);
            GuiSetPosYRel(0.5f);
            switch (stream.GetStatus()) {
            case Marketeer::MarketeerStream::STATUS_ERROR_SERVER:
                GuiCenteredText("Server error.");
                GuiSetPosYRel(0.6f);
                if (GuiButtonTxtCentered("Close")) {
                    result = true;
                }
                break;
            case Marketeer::MarketeerStream::STATUS_ERROR_AUTH:
                GuiCenteredText("Authentication error.");
                GuiSetPosYRel(0.6f);
                if (GuiButtonTxtCentered("Close")) {
                    result = true;
                }
                break;
            case Marketeer::MarketeerStream::STATUS_ERROR_CONNECTION:
                GuiCenteredText("Connection error, retrying...");
                GuiSetPosYRel(0.6f);
                if (GuiButtonTxtCentered("End stream")) {
                    mStreamCtx = "";
                    stream.Halt();
                }
                break;
            case Marketeer::MarketeerStream::STATUS_CONNECTING:
                GuiCenteredText("Connecting to server...");
                GuiSetPosYRel(0.6f);
                if (GuiButtonTxtCentered("End stream")) {
                    mStreamCtx = "";
                    stream.Halt();
                }
                break;
            case Marketeer::MarketeerStream::STATUS_STREAMING:
                GuiCenteredText("Stream ongoing.");
                GuiSetPosYRel(0.6f);
                if (GuiButtonTxtCentered("End stream")) {
                    mStreamCtx = "";
                    stream.Halt();
                }
                break;
            case Marketeer::MarketeerStream::STATUS_FINISHING:
                GuiCenteredText("Terminating stream...");
                break;
            case Marketeer::MarketeerStream::STATUS_FINISHED:
                GuiCenteredText("Stream finished.");
                GuiSetPosYRel(0.6f);
                if (GuiButtonTxtCentered("Close")) {
                    result = true;
                }
                break;
            default:
                break;
            }
            ImGui::EndTabItem();
        }

        return result;
    }
    bool ViewerTab(Marketeer::MarketeerStream& stream)
    {
        bool result = false;

        int flag = (stream.TabSwitch() ? ImGuiTabItemFlags_SetSelected : 0);
        if (flag) {
            stream.TabSwitch() = false;
        }

        auto streamStatus = stream.GetStatus();
        bool showTable = false;
        char tmpStr[256];
        int buttonSignal = 0;
        sprintf_s(tmpStr, "Watching: %s##%s", stream.GetUsername().c_str(), stream.GetIdStr().c_str());
        if (ImGui::BeginTabItem(tmpStr, nullptr, flag)) {
            char tempTxt[256];

            switch (streamStatus) {
            case Marketeer::MarketeerStream::STATUS_ERROR_LIVE:
            case Marketeer::MarketeerStream::STATUS_ERROR_SERVER:
            case Marketeer::MarketeerStream::STATUS_ERROR_AUTH:
            case Marketeer::MarketeerStream::STATUS_FINISHED:
                if (ImGui::Button("Close")) {
                    result = true;
                }
                break;
            case Marketeer::MarketeerStream::STATUS_ERROR_CONNECTION:
            case Marketeer::MarketeerStream::STATUS_CONNECTING:
            case Marketeer::MarketeerStream::STATUS_STREAMING:
                if (ImGui::Button("End")) {
                    stream.Halt();
                }
                break;
            default:
                break;
            }
            ImGui::SameLine();
            sprintf_s(tempTxt, "Watching \"%s\"'s stream (%s)", stream.GetUsername().c_str(), stream.GetFingerprint().c_str());
            GuiCenteredText(tempTxt);
            ImGui::Separator();

            switch (stream.GetStatus()) {
            case Marketeer::MarketeerStream::STATUS_ERROR_LIVE:
                ImGui::Text("Stream has ended or is invalid.");
                break;
            case Marketeer::MarketeerStream::STATUS_ERROR_SERVER:
                ImGui::Text("Server error.");
                break;
            case Marketeer::MarketeerStream::STATUS_ERROR_AUTH:
                ImGui::Text("Authentication error.");
                break;
            case Marketeer::MarketeerStream::STATUS_ERROR_CONNECTION:
                showTable = true;
                ImGui::Text("Connection error, retrying...");
                break;
            case Marketeer::MarketeerStream::STATUS_CONNECTING:
                showTable = true;
                ImGui::Text("Connecting to server...");
                break;
            case Marketeer::MarketeerStream::STATUS_STREAMING:
                showTable = true;
                ImGui::Text("Connected to stream.");
                break;
            case Marketeer::MarketeerStream::STATUS_FINISHING:
                ImGui::Text("Terminating...");
                break;
            case Marketeer::MarketeerStream::STATUS_FINISHED:
                ImGui::Text("Successfully ended.");
                break;
            default:
                break;
            }

            
        if (showTable) {

                if (ImGui::Button("Halt current playback")) {
                    stream.ViewerControl(2, 0);
                }
                ImGui::SameLine();
                if (stream.ViewerAutoplayStatus()) {
                    if (ImGui::Button("Disengage auto play")) {
                        stream.ViewerControl(4, 0);
                    }
                } else {
                    if (ImGui::Button("Engage auto play")) {
                        stream.ViewerControl(3, 0);
                    }
                }

                auto tableFlag = ImGuiTableFlags_Resizable | ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersV | ImGuiTableFlags_BordersOuterV | ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersOuterH | ImGuiTableFlags_BordersInnerH | ImGuiTableFlags_ScrollX | ImGuiTableFlags_ScrollY;

                char timeStr[256];
                sprintf_s(tmpStr, "mkt_viewer_table##%s", stream.GetIdStr().c_str());
                if (ImGui::BeginTable(tmpStr, 3, tableFlag)) {
                    ImGui::TableSetupColumn("Time", ImGuiTableColumnFlags_WidthFixed, 160.0f, 0);
                    ImGui::TableSetupColumn("Game", ImGuiTableColumnFlags_WidthFixed, 128.0f, 1);
                    ImGui::TableSetupColumn("Info", ImGuiTableColumnFlags_WidthStretch, 0.0f, 2);
                    ImGui::TableSetupScrollFreeze(1, 1);
                    ImGui::TableHeadersRow();

                    ImGuiListClipper clipper;
                    auto& signalsList = stream.ViewerLockSingals();
                    if (signalsList.size()) {
                        // auto signalIt = signalsList.begin();
                        uint32_t signalIdx = 0;
                        clipper.Begin(signalsList.size());
                        while (clipper.Step()) {
                            for (int row_n = clipper.DisplayStart; row_n < clipper.DisplayEnd; row_n++, signalIdx++) {
                                ImGui::TableNextRow(ImGuiTableRowFlags_None);
                                ImGui::TableSetColumnIndex(0);

                                std::chrono::milliseconds dur(signalsList[signalIdx].time);
                                std::chrono::time_point<std::chrono::system_clock> dt(dur);
                                time_t tt = std::chrono::system_clock::to_time_t(dt);
                                tm timeGm;
                                localtime_s(&timeGm, &tt);
                                strftime(timeStr, 256, "%D %T", &timeGm);
                                sprintf_s(tmpStr, "%s##%d", timeStr, row_n);
                                ImGui::Selectable(tmpStr, false, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowItemOverlap);

                                if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
                                    stream.ViewerControl(1, signalIdx);
                                }
                                if (ImGui::TableSetColumnIndex(1)) {
                                    ImGui::TextUnformatted(signalsList[signalIdx].game.c_str());
                                }
                                if (ImGui::TableSetColumnIndex(2)) {
                                    if (signalsList[signalIdx].cmd == MARKETEER_LAUNCH) {
                                        ImGui::TextUnformatted("Launch");
                                    } else if (signalsList[signalIdx].cmd == MARKETEER_EXIT_GAME) {
                                        ImGui::TextUnformatted("Exit");
                                    } else if (signalsList[signalIdx].cmd == MARKETEER_NEW_GAME) {
                                        ImGui::TextUnformatted("New game");
                                    }
                                }
                            }
                        }
                    }
                    stream.ViewerUnlockSingals();

                    ImGui::EndTable();
                }
            }

            ImGui::EndTabItem();
        }


        return result;
    }

    static int UsernameInputCallback(ImGuiInputTextCallbackData* data)
    {
        if (data->EventFlag == ImGuiInputTextFlags_CallbackCharFilter) {
            // Toggle casing of first character
            if (data->EventChar == '!') {
                data->EventChar = 0;
            }
        }
        return 0;
    }
    void MainTab()
    {
        static int radio = 0;
        ImGui::SetNextItemWidth(ImGui::CalcTextSize("View:").x);
        ImGui::LabelText("", "View:");
        ImGui::SameLine();
        ImGui::RadioButton("Server", &radio, 0);
        ImGui::SameLine();
        ImGui::RadioButton("Player", &radio, 1);
        ImGui::SameLine();

        auto canRefresh = refershThread.IsActive();
        if (canRefresh) {
            ImGui::BeginDisabled();
        }
        if (ImGui::Button("Refresh")) {
            StartRefreshThread();
        }
        if (canRefresh) {
            ImGui::EndDisabled();
        }

        ImGui::BeginChild("##marketeer_radio", ImVec2(0, -16.0f), false);
        if (!radio) {
            ServerView();
        } else {
            PlayerView();
        }
        ImGui::EndChild();
    }
    void DebugTab()
    {
        
        if (ImGui::Button("Debug")) {
            auto& stream = mViews.emplace_back();
            auto streamName = Marketeer::GenStreamName(std::string("debug"));
            stream.TabSwitch() = true;
            stream.SetIdStr(streamName);
            stream.SetURL("");
            stream.SetUsername("");
            stream.SetFingerprint(std::string());
            //stream.StartLocalViewer(std::wstring(L"C:\\Users\\Ack\\AppData\\Roaming\\thprac\\marketeer\\ongoing\\ack-2022-4-2-18-9-52-36"));
        }
    }
    void SettingsInit()
    {
        ConfigDBInit();
        CheckCfgTable("marketeer");
        if (GetStrFromDB("marketeer", "username", mUsername) == S_OK) {
            strcpy_s(mUsernameBuffer, mUsername.c_str());
        }

        if (GetStrFromDB("marketeer", "private_key", mPrivateKey) != S_OK) {
            mPrivateKey = "";
        } else {
            mPrivateKey = SSLLoadPEM(mPrivateKey);
            mFingerprint = SSLGetFingerprint(mPrivateKey);
            if (mFingerprint == "") {
                mPrivateKey = "";
                SetStrToDB("marketeer", "private_key", std::string());
            }
        }
    }
    void SettingsTab()
    {
        ImGui::Text("Identity");
        ImGui::Separator();
        ImGui::SetNextItemWidth(ImGui::CalcTextSize("Username:").x);
        ImGui::LabelText("", "Username:");
        ImGui::SameLine();
        ImGui::PushItemWidth(180.0f - ImGui::GetWindowSize().x);
        if (ImGui::InputText("##mkt_username", mUsernameBuffer, 9, ImGuiInputTextFlags_CallbackCharFilter, UsernameInputCallback)) {
            mUsername = mUsernameBuffer;
            SetStrToDB("marketeer", "username", mUsername);
        }
        ImGui::PopItemWidth();
        if (!mPrivateKey.size()) {
            ImGui::Text("No key is loaded.");
            if (ImGui::Button("Generate key")) {
                if (SSLGenKey(mPrivateKey)) {
                    mFingerprint = SSLGetFingerprint(mPrivateKey);
                    SetStrToDB("marketeer", "private_key", SSLGetPEM(mPrivateKey));
                }
            }
            //ImGui::SameLine();
            //ImGui::Button("Load key");
        } else {
            ImGui::Text("Key loaded. Fingerprint: [%s]", mFingerprint.c_str());
            //ImGui::Button("Export key");
            //ImGui::SameLine();
            if (ImGui::Button("Clear key")) {
                mPrivateKey = "";
                mFingerprint = "";
                SetStrToDB("marketeer", "private_key", std::string());
            }
        }
    }
    void AdminTab()
    {
        static MarketeerAdminConsole console;
        console.Draw("Example: Console");
    }
    bool GuiContent()
    {

        if (ImGui::BeginTabBar("MarketeerTabBar")) {
            if (ImGui::BeginTabItem("Main##mkt")) {
                MainTab();
                ImGui::EndTabItem();
            }

            bool clear = false;
            for (auto& stream : mStreams) {
                clear |= StreamerTab(stream);
            }
            if (clear) {
                mStreams.clear();
            }
            for (auto it = mViews.begin(); it != mViews.end();) {
                if (ViewerTab(*it)) {
                    it = mViews.erase(it);
                } else {
                    ++it;
                }
            }

            if (ImGui::BeginTabItem("Settings##mkt")) {
                SettingsTab();
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Admin##mkt")) {
                AdminTab();
                ImGui::EndTabItem();
            }
            if (false && ImGui::BeginTabItem("Debug##mkt")) {
                DebugTab();
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }

        return true;
    }
    void GuiMain()
    {
        if (!mGuiUpdFunc()) {
            mGuiUpdFunc = [&]() { return GuiContent(); };
        }
    }

    std::function<bool(void)> mGuiUpdFunc = []() { return true; };

    char mUsernameBuffer[64] {};
    std::string mUsername;
    std::string mPrivateKey;
    std::string mFingerprint;
    std::string mStreamCtx;

    std::list<Marketeer::MarketeerStream> mStreams;
    std::list<Marketeer::MarketeerStream> mViews;

    std::list<Marketeer::Player> mPlayerList;
    char mPlayerFilter[256];
    int mPlayerFilterRadio = 0;
    std::mutex mPlayerListMutex;
    size_t mPlayerListFilteredSize;
    std::list<Marketeer::Player>::iterator mSelectedPlayer;

    std::list<Marketeer::Server> mServerList;
    char mServerFilter[256];
    std::mutex mServerListMutex;
    size_t mServerListFilteredSize;
    std::list<Marketeer::Server>::iterator mSelectedServer;

    GuiThread refershThread { RefreshThread };
};

bool LauncherMktGuiUpd()
{
    THMarketeerGui::singleton().GuiUpdate();
    return true;
}

bool LauncherMktIsStreaming()
{
    return THMarketeerGui::singleton().IsStreaming();
}

std::string& LauncherMktGetCtx()
{
    return THMarketeerGui::singleton().GetCtx();
}

void LauncherMktLaunchCallback(const char* game)
{
    return THMarketeerGui::singleton().LaunchCallback(game);
}

void LauncherMktTerminateAll()
{
    return THMarketeerGui::singleton().TerminateAll();
}
}