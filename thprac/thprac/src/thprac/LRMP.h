#pragma once
#ifndef LRMP_HEADER
#define LRMP_HEADER

#if _WIN32 || _WIN64
#define ByteSwap16 _byteswap_ushort
#define ByteSwap32 _byteswap_ulong
#define ByteSwap64 _byteswap_uint64
#if _WIN64
#define ENV64
#else
#define ENV32
#endif
#elif __GNUC__
#define ByteSwap16 __builtin_bswap16
#define ByteSwap32 __builtin_bswap32
#define ByteSwap64 __builtin_bswap64
#if __x86_64__ || __ppc64__
#define ENV64
#else
#define ENV32
#endif
#else
#error Unsupported compiler
#endif

#include <nng/nng.h>
#include <nng/protocol/reqrep0/rep.h>
#include <nng/protocol/reqrep0/req.h>
#include <nng/transport/inproc/inproc.h>
#include <chrono>
#include <thread>
#include <cstring>

enum class ServerCmd : uint32_t {
    _RSV = 0,
    CLIENT_HELLO,
    CLIENT_REGISTER,
    CLIENT_AUTHORIZE,
    CLIENT_VERIFY,
    CLIENT_START_LIVE,
    CLIENT_PUSH,
    CLIENT_END_LIVE,
    CLIENT_PULL_PLAYERS,
    CLIENT_CHECK_PLAYER,
    CLIENT_PULL,
    CLIENT_ADMIN_CMD = 99,

    REP_OK = 100,
    REP_ERR_SERVER,
    REP_ERR_INVALID_MSG,
    REP_ERR_WRITE,
    REP_ERR_READ,
    REP_ERR_INVALID_CRED,
    REP_ERR_INVALID_LIVE,
    REP_ERR_LIVE_MISMATCH,
    REP_ERR_CURSOR_MISMATCH,

    INPROC_READY = 200,
};

static inline void LRMPMsgChopEx(nng_msg* msg)
{
}

template <class... Args, typename T>
static inline void LRMPMsgChopEx(nng_msg* msg, uint8_t* msgBody, std::pair<T, size_t> data, Args... rest)
{
    msgBody -= data.second;
    memcpy_s(data.first, data.second, msgBody, sizeof(T));
    //nng_msg_chop(msg, data.second);
    LRMPMsgChopEx(msg, rest...);
}

template <class... Args, typename T>
static inline void LRMPMsgChopEx(nng_msg* msg, uint8_t* msgBody, T* data, Args... rest)
{
    msgBody -= sizeof(T);
    memcpy(data, msgBody, sizeof(T));
    //nng_msg_chop(msg, sizeof(T));
    LRMPMsgChopEx(msg, rest...);
}

template <class... Args>
static inline void LRMPMsgChop(nng_msg* msg, Args... rest)
{
    LRMPMsgChopEx(msg, (uint8_t*)nng_msg_body(msg) + nng_msg_len(msg), rest...);
}

static inline void LRMPMsgDumpEx(uint8_t* msgBody)
{
}

template <class... Args, typename T>
static inline void LRMPMsgDumpEx(uint8_t* msgBody, std::pair<T, size_t> out, Args... rest)
{
    auto outSize = out.second;
    memcpy(out.first, msgBody, outSize);
    LRMPMsgDumpEx(msgBody + outSize, rest...);
}

template <class... Args, typename T>
static inline void LRMPMsgDumpEx(uint8_t* msgBody, T* out, Args... rest)
{
    *out = *(T*)msgBody;
    LRMPMsgDumpEx(msgBody + sizeof(T), rest...);
}

template <class... Args>
static inline void LRMPMsgDump(nng_msg* msg, size_t offset, Args... rest)
{
    LRMPMsgDumpEx((uint8_t*)nng_msg_body(msg) + 12 + offset, rest...);
}

static inline size_t LRMPMsgBodyLen(nng_msg* msg)
{
    return nng_msg_len(msg) - 12;
}

static inline void* LRMPMsgBodyPtr(nng_msg* msg, size_t offset = 0)
{
    return (void*)((uint8_t*)nng_msg_body(msg) + 12 + offset);
}

static inline uint32_t* LRMPMsgHeaderPtr(nng_msg* msg, size_t offset = 0)
{
    return (uint32_t*)(nng_msg_header(msg));
}

static inline void LRMPMsgFit(nng_msg* msg, size_t size)
{
    static char pad[256] {};
    auto len = nng_msg_len(msg) - 12;
    if (len < size) {
        nng_msg_append(msg, pad, size - len);
    } else if (len > size) {
        nng_msg_chop(msg, len - size);
    }
}

static inline void LRMPMsgPad(nng_msg* msg, size_t size)
{
    static char pad[256] {};
    nng_msg_append(msg, pad, size);
}

static inline void LRMPMsgAppend(nng_msg* msg)
{
}

template <class... Args, typename T>
static inline void LRMPMsgAppend(nng_msg* msg, std::pair<T, size_t> data, Args... rest)
{
    nng_msg_append(msg, data.first, data.second);
    LRMPMsgAppend(msg, rest...);
}

template <class... Args, typename T>
static inline void LRMPMsgAppend(nng_msg* msg, const T* data, Args... rest)
{
    nng_msg_append(msg, data, sizeof(T));
    LRMPMsgAppend(msg, rest...);
}

static inline void LRMPMsgSetCmd(nng_msg* msg, ServerCmd cmd)
{
    ((uint32_t*)nng_msg_body(msg))[2] = (uint32_t)cmd;
}

static inline ServerCmd LRMPMsgGetCmd(nng_msg* msg)
{
    return (ServerCmd)((uint32_t*)nng_msg_body(msg))[2];
}

static inline bool LRMPMsgCheck(nng_msg* msg)
{
    return (nng_msg_len(msg) >= 12 && !memcmp(nng_msg_body(msg), "LRMP/01", 8));
}

static inline bool LRMPMsgRouteToWorker(nng_msg* msg, nng_socket socket)
{
    if (nng_msg_header_len(msg) == 12) {
        uint32_t* header = (uint32_t*)nng_msg_header(msg);
        uint32_t tmp = header[0];
        header[0] = header[1];
        header[1] = tmp;
        nng_sendmsg(socket, msg, 0);
        return true;
    }
    return false;
}

static inline bool LRMPMsgRouteToClient(nng_msg* msg, nng_socket socket)
{
    if (nng_msg_header_len(msg) == 12) {
        nng_msg_header_trim(msg, 4);
        nng_sendmsg(socket, msg, 0);
        return true;
    }
    return false;
}

static inline void LRMPMsgPushRoute(nng_msg* msg, uint32_t routeId)
{
    nng_msg_header_insert_u32(msg, ByteSwap32(routeId));
}

static inline uint32_t LRMPMsgGetRoute(nng_msg* msg)
{
    return *(uint32_t*)nng_msg_header(msg);
}

template <class... Args>
static inline void LRMPMsgReset(nng_msg* msg, ServerCmd cmd, Args... rest)
{
    nng_msg_chop(msg, nng_msg_len(msg) - 12);
    uint32_t* msgBody = (uint32_t*)nng_msg_body(msg);
    msgBody[2] = (uint32_t)cmd;
    LRMPMsgAppend(msg, rest...);
}

template <class... Args>
static inline nng_msg* LRMPMsgCreateRaw(uint32_t msgId, ServerCmd cmd, Args... rest)
{
    nng_msg* msg;
    nng_msg_alloc(&msg, 12);
    nng_msg_header_insert_u32(msg, 0x80000000 | msgId);
    uint32_t* msgBody = (uint32_t*)nng_msg_body(msg);
    memcpy(msgBody, "LRMP/01", 8);
    msgBody[2] = (uint32_t)cmd;
    LRMPMsgAppend(msg, rest...);
    return msg;
}

template <class... Args>
static inline nng_msg* LRMPMsgCreate(ServerCmd cmd, Args... rest)
{
    nng_msg* msg;
    nng_msg_alloc(&msg, 12);
    uint32_t* msgBody = (uint32_t*)nng_msg_body(msg);
    memcpy(msgBody, "LRMP/01", 8);
    msgBody[2] = (uint32_t)cmd;
    LRMPMsgAppend(msg, rest...);
    return msg;
}

static inline int LRMPMsgSend(nng_socket socket, nng_msg* msg, int flag)
{
    return nng_sendmsg(socket, msg, flag);
}

static inline int LRMPMsgRecv(nng_socket socket, nng_msg** msg, int flag)
{
    int rc;
start:
    rc = nng_recvmsg(socket, msg, flag);
    if (!rc && !LRMPMsgCheck(*msg)) {
        nng_msg_free(*msg);
        goto start;
    }
    return rc;
}

static inline int LRMPMsgSendTimeout(nng_socket socket, nng_msg* msg, size_t timeoutMs, size_t timeToSleep = 5)
{
    int rc = 0;
    size_t timeSlept = 0;
    while (timeSlept < timeoutMs) {
        rc = LRMPMsgSend(socket, msg, NNG_FLAG_NONBLOCK);
        if (rc != NNG_EAGAIN) {
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(timeToSleep));
        timeSlept += timeToSleep;
    }
    return rc;
}

static inline int LRMPMsgRecvTimeout(nng_socket socket, nng_msg** msg, size_t timeoutMs, size_t timeToSleep = 5)
{
    int rc = 0;
    size_t timeSlept = 0;
    while (timeSlept < timeoutMs) {
        rc = LRMPMsgRecv(socket, msg, NNG_FLAG_NONBLOCK);
        if (rc != NNG_EAGAIN) {
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(timeToSleep));
        timeSlept += timeToSleep;
    }
    return rc;
}

static inline void LRMPMsgFree(nng_msg* msg)
{
    nng_msg_free(msg);
}

static inline int LRMPConnectRaw(nng_socket* socket, nng_dialer* dialer, const char* address)
{
    int rc;
    rc = nng_req0_open_raw(socket);
    rc = nng_dial(*socket, address, dialer, 0);
    return rc;
}

static inline int LRMPListenRaw(nng_socket* socket, nng_listener* listener, const char* address)
{
    int rc;
    rc = nng_rep0_open_raw(socket);
    rc = nng_setopt_int(*socket, NNG_OPT_RECVBUF, 128);
    rc = nng_setopt_int(*socket, NNG_OPT_SENDBUF, 128);
    rc = nng_listen(*socket, address, listener, 0);
    return rc;
}

static inline int LRMPConnect(nng_socket* socket, nng_dialer* dialer, const char* address)
{
    int rc;
    rc = nng_req0_open(socket);
    rc = nng_setopt_ms(*socket, NNG_OPT_RECVTIMEO, 10000);
    rc = nng_dial(*socket, address, dialer, 0);
    return rc;
}

static inline int LRMPListen(nng_socket* socket, nng_listener* listener, const char* address)
{
    int rc;
    rc = nng_rep0_open(socket);
    rc = nng_listen(*socket, address, listener, 0);
    return rc;
}

static inline void LRMPClose(nng_socket socket, nng_dialer dialer)
{
    nng_dialer_close(dialer);
    nng_close(socket);
}

static inline void LRMPClose(nng_socket socket, nng_listener listener)
{
    nng_listener_close(listener);
    nng_close(socket);
}

#endif