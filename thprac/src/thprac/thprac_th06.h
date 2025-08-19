#pragma once

#include <stdint.h>
#include "thprac_games.h"

namespace THPrac {
namespace TH06 {

// Struct definition and constants from https://github.com/happyhavoc/th06

constexpr unsigned PSCR_NUM_CHARS_SHOTTYPES = 4;
constexpr unsigned PSCR_NUM_STAGES = 6;
constexpr unsigned PSCR_NUM_DIFFICULTIES = 4;
constexpr unsigned CLRD_NUM_CHARACTERS = 4;
constexpr unsigned CATK_NUM_CAPTURES = 64;

struct Th6k {
    uint32_t magic;
    uint16_t th6kLen;
    uint16_t unkLen;
    uint8_t version;
    uint8_t unk_9;
};

struct Catk {
    Th6k base;
    int32_t captureScore;
    uint16_t idx;
    uint8_t nameCsum;
    uint8_t characterShotType;
    uint32_t unk_14;
    char name[32];
    uint32_t unk_38;
    uint16_t numAttempts;
    uint16_t numSuccess;
};

struct Clrd {
    Th6k base;
    uint8_t difficultyClearedWithRetries[5];
    uint8_t difficultyClearedWithoutRetries[5];
    uint8_t characterShotType;
};

struct Pscr {
    Th6k base;
    int32_t score;
    uint8_t character;
    uint8_t difficulty;
    uint8_t stage;
};

struct GameManager {
    uint32_t guiScore;
    uint32_t score;
    uint32_t nextScoreIncrement;
    uint32_t highScore;
    uint32_t difficulty;
    int32_t grazeInStage;
    int32_t grazeInTotal;
    uint32_t isInReplay;
    int32_t deaths;
    int32_t bombsUsed;
    int32_t spellcardsCaptured;
    int8_t isTimeStopped;
    Catk catk[CATK_NUM_CAPTURES];
    Clrd clrd[CLRD_NUM_CHARACTERS];
    Pscr pscr[PSCR_NUM_CHARS_SHOTTYPES][PSCR_NUM_STAGES][PSCR_NUM_DIFFICULTIES];
    uint16_t currentPower;
    int8_t unk_1812;
    int8_t unk_1813;
    uint16_t pointItemsCollectedInStage;
    uint16_t pointItemsCollected;
    uint8_t numRetries;
    int8_t powerItemCountForScore;
    int8_t livesRemaining;
    int8_t bombsRemaining;
    int8_t extraLives;
    uint8_t character;
    uint8_t shotType;
    uint8_t isInGameMenu;
    uint8_t isInRetryMenu;
    uint8_t isInMenu;
    uint8_t isGameCompleted;
    uint8_t isInPracticeMode;
    uint8_t demoMode;
    int8_t unk_1825;
    int8_t unk_1826;
    int8_t unk_1827;
    int32_t demoFrames;
    int8_t replayFile[256];
    int8_t unk_192c[256];
    uint16_t randomSeed;
    uint32_t gameFrames;
    int32_t currentStage;
    uint32_t menuCursorBackup;
    Float2 arcadeRegionTopLeftPos;
    Float2 arcadeRegionSize;
    Float2 playerMovementAreaTopLeftPos;
    Float2 playerMovementAreaSize;
    float cameraDistance;
    Float3 stageCameraFacingDir;
    int32_t counat;
    int32_t rank;
    int32_t maxRank;
    int32_t minRank;
    int32_t subRank;
};
}
}