#pragma once

#include <stdint.h>
#include "utils/utils.h"

namespace THPrac {

enum THGameType {
    TYPE_ERROR = 0,
    TYPE_ORIGINAL = 1,
    TYPE_MODDED = 2,
    TYPE_THCRAP = 3,
    TYPE_CHINESE = 4,
    TYPE_SCHINESE = 5,
    TYPE_TCHINESE = 6,
    TYPE_NYASAMA = 7,
    TYPE_STEAM = 8,
    TYPE_UNCERTAIN = 9,
    TYPE_MALICIOUS = 10,
    TYPE_UNKNOWN = 11,
};

enum THGameID {
    ID_UNKNOWN = 0,
    ID_ALCOSTG,
    ID_TH06,
    ID_TH07,
    ID_TH075,
    ID_TH08,
    ID_TH09,
    ID_TH095,
    ID_TH10,
    ID_TH105,
    ID_TH11,
    ID_TH12,
    ID_TH123,
    ID_TH125,
    ID_TH128,
    ID_TH13,
    ID_TH135,
    ID_TH14,
    ID_TH143,
    ID_TH145,
    ID_TH15,
    ID_TH155,
    ID_TH16,
    ID_TH165,
    ID_TH17,
    ID_TH175,
    ID_TH18,
    ID_TH185,
    ID_TH19,
    ID_TH20,
    ID_TH_MAX,
};

struct ExeInfo {
    uint32_t timeStamp;
    uint32_t textSize;

    operator bool() {
        return timeStamp && textSize;
    }

    bool operator==(const ExeInfo& rhs) {
        return timeStamp == rhs.timeStamp && textSize == rhs.textSize;
    }
};

struct THGameInfo {
    uint32_t steamId;
};

struct THGameVersion {
    THGameID gameId;
    void (*initFunc)();
    ExeInfo exeInfo;
    uint16_t oepCode[10];
};

struct THKnownGame {
    const THGameVersion* ver;
    THGameType type;
    uint32_t metroHash[4];
};

extern const THGameVersion gGameVersions[];
extern const THKnownGame gKnownGames[];
extern const char* gThGameStrs[];

extern const unsigned int gGameVersionsCount;
extern const unsigned int gKnownGamesCount;

enum ThVersionArrOffset {
    VER_ALCOSTG,
    VER_TH06,
    VER_TH07,
    VER_TH075,
    VER_TH08,
    VER_TH09,
    VER_TH095,
    VER_TH10,
    VER_TH105,
    VER_TH11,
    VER_TH12,
    VER_TH123,
    VER_TH125,
    VER_TH128,
    VER_TH13,
    VER_TH135,
    VER_TH14,
    VER_TH143,
    VER_TH145,
    VER_TH15,
    VER_TH155,
    VER_TH16,
    VER_TH165,
    VER_TH17,
    VER_TH175,
    VER_TH18,
    VER_TH185,
    VER_TH19_V1_00A,
    VER_TH19_V1_10C,
    VER_TH20,
    VER_MAX,
};

ExeInfo GetExeInfo(const uint8_t* mod, size_t len);
ExeInfo GetRemoteExeInfo(void* hProc, uintptr_t mod);

const THGameVersion* IdentifyExe(const uint8_t* buf, size_t len);
const THGameVersion* IdentifyExe(const wchar_t* path);

bool IdentifyKnownGame(THKnownGame& out, uint16_t (&outOepCode)[10], const uint8_t* buf, size_t size);
bool IdentifyKnownGame(THKnownGame& out, uint16_t (&outOepCode)[10], const wchar_t* fn);

}
