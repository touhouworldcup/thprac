#pragma once
#include <cstdint>
#include <string>

namespace THPrac {
enum MarketeerCmd : uint8_t {
    MARKETEER_ON_HOLD = 0,
    MARKETEER_LAUNCH = 1,
    MARKETEER_NEW_GAME = 2,
    MARKETEER_NEW_STAGE = 3,
    MARKETEER_CONTINUE = 4,
    MARKETEER_QUIT_TO_MENU = 5,
    MARKETEER_GAME_COMPLETE = 6,
    MARKETEER_PAUSE = 11,
    MARKETEER_RESUME = 12,
    MARKETEER_EXIT_GAME = 20,
    MARKETEER_TH18_CARD = 21,
    MARKETEER_TH18_BLANK = 22,
    MARKETEER_EXTRA = 0xFE,
    MARKETEER_HALT = 0xFF
};
enum MarketeerCaption {
    MKT_CAPTION_WAITING_SIGNAL,
    MKT_CAPTION_ACCEPTING,
    MKT_CAPTION_FETCHING,
    MKT_CAPTION_PLAYING,
};

struct StreamSignal {
    StreamSignal() = default;
    StreamSignal(uint64_t t, uint32_t p1, uint32_t p2, uint32_t p3, uint32_t p4, uint32_t p5, uint32_t p6, void* data = nullptr, size_t size = 0)
        : time(t)
        , param1(p1)
        , param2(p2)
        , param3(p3)
        , param4(p4)
        , param5(p5)
        , param6(p6)
    {
    }
    StreamSignal(uint64_t t, uint32_t p1, uint32_t p2, uint32_t p3, uint32_t p4, uint32_t p5, uint32_t p6, std::string* data)
        : time(t)
        , param1(p1)
        , param2(p2)
        , param3(p3)
        , param4(p4)
        , param5(p5)
        , param6(p6)
    {
        //time = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now()).time_since_epoch().count();
        if (data) {
            extra = *data;
        }
    }
    inline uint8_t GetCmd()
    {
        return *(uint8_t*)(&param1);
    }
    uint64_t time = 0;
    uint32_t param1 = 0;
    uint32_t param2 = 0;
    uint32_t param3 = 0;
    uint32_t param4 = 0;
    uint32_t param5 = 0;
    uint32_t param6 = 0;
    std::string extra {};

    uint32_t offset = 0;
};

int MarketeerGetStatus();
void MarketeerEnablePush(const char* url);
void MarketeerEnablePull(const char* url);
void MarketeerEndLive();
void MarketeerTerminate();

void MarketeerFreezeGame();
void MarketeerPush(void* buffer, size_t size);
void MarketeerPushSignal(unsigned int param1 = 0, unsigned int param2 = 0, unsigned int param3 = 0, unsigned int param4 = 0, unsigned int param5 = 0, unsigned int param6 = 0, std::string* data = nullptr);

void MarketeerOnStageStart(bool isGameStart, unsigned int stage, unsigned int rank, unsigned int player, unsigned int seed, unsigned int playerX, unsigned int playerY);
void MarketeerOnQuitToMenu();
void MarketeerOnGameComlete();

int MarketeerInit(const char* game, void* hwnd);
void MarketeerSetCaption(MarketeerCaption caption);
MarketeerCmd MarketeerInputHelper(void* buffer, size_t size);
StreamSignal* MarketeerGetCursor();
void MarketeerAdvCursor();
void MarketeerAlignStream(StreamSignal* sigPtr);
void MarketeerSetStreamLimit(StreamSignal* sigPtr);
int MarketeerReadStream(uint32_t unitOffset, uint32_t unitSize, void* out);
void MarketeerMutex();
}