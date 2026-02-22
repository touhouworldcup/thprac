#pragma once

#include <stdint.h>

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
};

struct THGameInfo {
    uint32_t steamId;
};

struct THGameVersion {
    THGameID gameId;
    void (*initFunc)();
    uint32_t timeStamp;
    uint32_t textSize;
    uint32_t oepCode[10];
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

uint64_t GetExeInfo(uint8_t* mod);
uint64_t GetExeInfo(const wchar_t* path);
uint64_t GetRemoteExeInfo(HANDLE hProc, uintptr_t mod);

const THGameVersion* IdentifyExe(uint8_t* buf);
const THGameVersion* IdentifyExe(const wchar_t* path);

}
