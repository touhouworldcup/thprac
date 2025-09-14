#pragma once

#include <stdint.h>

#include "../3rdParties/d3d8/include/d3d8.h"
#include "../3rdParties/d3d8/include/dinput.h"
#include "../3rdParties/d3d8/include/d3dx8math.h"

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


/**
 * @brief The enum used in th06 for the difficulties of games, as used in 
 *        struct `GameManager`. 
 * @warning DON'T change the fields unless ZUN changed his corresponding 
 *          code in th06.
 * @details The code is from https://github.com/happyhavoc/th06/blob/master/src/GameManager.hpp .
 */
enum Difficulty : int32_t {
    DIFFICULTY_EASY,
    DIFFICULTY_NORMAL,
    DIFFICULTY_HARD,
    DIFFICULTY_LUNATIC,
    DIFFICULTY_EXTRA
};

/**
 * @brief The enum used in th06 for the characters of games, as used in 
 *        struct `GameManager`. 
 * @warning DON'T change the fields unless ZUN change his corresponding 
 *          code in th06.
 * @details The code is from https://github.com/happyhavoc/th06/blob/master/src/GameManager.hpp .
 *          Not sure which number is for Satsuki Rin, maybe 2.
 */
enum Character : uint8_t {
    CHARACTER_REIMU,
    CHARACTER_MARISA
};

/**
 * @brief The enum used in th06 for the shottypes of games, as used in 
 *        struct `GameManager`. 
 * @warning DON'T change the fields unless ZUN change his corresponding 
 *          code in th06.
 * @details The code is from https://github.com/happyhavoc/th06/blob/master/src/GameManager.hpp .
 *          We use (character * 2 + shottype) to combine the two
 *          enums into *real* shottypes such as ReimuA.
 */
enum ShotType : uint8_t {
    SHOTTYPE_A,
    SHOTTYPE_B
};

/**
 * @brief The struct used in th06 for managing the game.
 * @warning DON'T change the fields unless ZUN change his corresponding 
 *          code in th06.
 * @details The code is from https://github.com/happyhavoc/th06/blob/master/src/GameManager.hpp ,
 *          edited.
 *          We only use a few fields of it in our code.
 *          The code here is not byte-aligned by itself, and so the offset 
 *          (which stands for the byte-aligned struct) can be confusing.
 *          Some fields are still unknown, however one can refer to Happy
 *          Havoc's Reverse Engineering progress in 
 *          https://github.com/happyhavoc/th06/ .
 */
struct GameManager {
    uint32_t guiScore; // 0x0, score showed in gui
    uint32_t score; // 0x4, the real score
    uint32_t nextScoreIncrement; // 0x8
    uint32_t highScore; // 0xc, high score showed in gui is the real one
    Difficulty difficulty; // 0x10
    int32_t grazeInStage; // 0x14
    int32_t grazeInTotal; // 0x18
    uint32_t isInReplay; // 0x1c
    int32_t deaths; // 0x20
    int32_t bombsUsed; // 0x24
    int32_t spellcardsCaptured; // 0x28
    int8_t isTimeStopped; // 0x2c 
    Catk catk[CATK_NUM_CAPTURES]; // 0x30
    Clrd clrd[CLRD_NUM_CHARACTERS]; // 0x1030
    Pscr pscr[PSCR_NUM_CHARS_SHOTTYPES][PSCR_NUM_STAGES][PSCR_NUM_DIFFICULTIES];  // 0x1090
    uint16_t currentPower; // 0x1810
    int8_t unk_1812; // 0x1812
    int8_t unk_1813; // 0x1813
    uint16_t pointItemsCollectedInStage; // 0x1814
    uint16_t pointItemsCollected; // 0x1816
    uint8_t numRetries; // 0x1818
    int8_t powerItemCountForScore; // 0x1819
    int8_t livesRemaining; // 0x181a
    int8_t bombsRemaining; // 0x181b
    int8_t extraLives; // 0x181c, extend requirements passed
    Character character; // 0x181d
    ShotType shotType; // 0x181e
    uint8_t isInGameMenu; // 0x181f
    uint8_t isInRetryMenu; // 0x1820
    uint8_t isInMenu; // 0x1821
    uint8_t isGameCompleted; // 0x1822
    uint8_t isInPracticeMode; // 0x1823
    uint8_t demoMode; // 0x1824
    int8_t unk_1825; // 0x1825
    int8_t unk_1826; // 0x1826
    int8_t unk_1827; // 0x1827
    int32_t demoFrames; // 0x1828
    int8_t replayFile[256]; // 0x182c
    int8_t unk_192c[256]; // 0x192c
    uint16_t randomSeed; // 0x1a2c
    uint32_t gameFrames; // 0x1a2e
    int32_t currentStage; // 0x1a34, 7 for Stage Ex
    uint32_t menuCursorBackup; // 0x1a38
    D3DXVECTOR2 arcadeRegionTopLeftPos; // 0x1a3c
    D3DXVECTOR2 arcadeRegionSize; // 0x1a44
    D3DXVECTOR2 playerMovementAreaTopLeftPos; // 0x1a4c
    D3DXVECTOR2 playerMovementAreaSize; // 0x1a54
    float cameraDistance; // 0x1a5c
    D3DXVECTOR3 stageCameraFacingDir; // 0x1a60
    int32_t counat; // 0x1a6c
    int32_t rank; // 0x1a70
    int32_t maxRank; // 0x1a74
    int32_t minRank; // 0x1a78
    int32_t subRank; // 0x1a7c
    // 0x1a80
};
static_assert(sizeof(GameManager) == 0x1a80u);

/**
 * @brief The enum used in th06 for the states of Supervisors, as used in 
 *        struct `Supervisor`. 
 * @warning DON'T change the fields unless ZUN change his corresponding 
 *          code in th06.
 * @details The code is from https://github.com/happyhavoc/th06/blob/master/src/GameManager.hpp .
 */
enum SupervisorState : int32_t {
    SUPERVISOR_STATE_INIT,
    SUPERVISOR_STATE_MAINMENU,
    SUPERVISOR_STATE_GAMEMANAGER,
    SUPERVISOR_STATE_GAMEMANAGER_REINIT,
    SUPERVISOR_STATE_EXITSUCCESS,
    SUPERVISOR_STATE_EXITERROR,
    SUPERVISOR_STATE_RESULTSCREEN,
    SUPERVISOR_STATE_RESULTSCREEN_FROMGAME,
    SUPERVISOR_STATE_MAINMENU_REPLAY,
    SUPERVISOR_STATE_MUSICROOM,
    SUPERVISOR_STATE_ENDING,
};

/**
 * @brief The struct used in th06 for the configuration, as used in 
 *        struct `Supervisor`. 
 * @warning DON'T change the fields unless ZUN change his corresponding 
 *          code in th06.
 * @details The code is from https://github.com/happyhavoc/th06/blob/master/src/GameManager.hpp .
 */
struct GameConfiguration {
    int8_t controllerMapping[0x14]; // 0x0, of type ControllerMapping but unused
    int32_t version; // 0x14, always 0x102 for 1.02
    uint8_t lifeCount; // 0x18, starting lives
    uint8_t bombCount; // 0x19, starting bombs
    uint8_t colorMode16bit; // 0x1a
    uint8_t musicMode; // 0x1b
    uint8_t playSounds; // 0x1c
    uint8_t defaultDifficulty; // 0x1d
    uint8_t windowed; // 0x1e
    uint8_t frameskipConfig; // 0x1f, 0 = fullspeed, 1 = 1/2 speed, 2 = 1/4 speed
    int16_t padXAxis; // 0x20
    int16_t padYAxis; // 0x22
    int8_t unk[16]; // 0x24
    uint32_t opts; // 0x34, GameConfigOpts bitfield
};

/**
 * @brief The struct used in th06 for supervisoring.
 * @warning DON'T change the fields unless ZUN change his corresponding 
 *          code in th06.
 * @details The code is from https://github.com/happyhavoc/th06/blob/master/src/GameManager.hpp ,
 *          edited.
 */
struct Supervisor {
    typedef char Pbg3ArchiveName[32];

    HINSTANCE hInstance; // 0x0
    PDIRECT3D8 d3dIface; // 0x4
    PDIRECT3DDEVICE8 d3dDevice; // 0x8
    LPDIRECTINPUT8 dinputIface; // 0xc
    LPDIRECTINPUTDEVICE8A keyboard; // 0x10
    LPDIRECTINPUTDEVICE8A controller; // 0x14
    DIDEVCAPS controllerCaps; // 0x18
    HWND hwndGameWindow; // 0x44
    D3DXMATRIX viewMatrix; // 0x48
    D3DXMATRIX projectionMatrix; // 0x88
    D3DVIEWPORT8 viewport; // 0xc8
    D3DPRESENT_PARAMETERS presentParameters; // 0xe0
    GameConfiguration cfg; // 0x114
    GameConfiguration defaultConfig; // 0x14c
    int32_t calcCount; // 0x184
    SupervisorState wantedState; // 0x188
    SupervisorState curState; // 0x18c
    SupervisorState wantedState2; // 0x190

    int32_t unk194; // 0x194
    int32_t screenBackgroundRedrawFrames; // 0x198
    int32_t isInEnding; // 0x19c, is a bool

    int32_t vsyncEnabled; // 0x1a0
    int32_t lastFrameTime; // 0x1a4
    float effectiveFramerateMultiplier; // 0x1a8
    float framerateMultiplier; // 0x1ac

    void* midiOutput; // 0x1b0, should be MidiOutput* in happyhavoc/th06

    float unk1b4; // 0x1b4
    float unk1b8; // 0x1b8

    void* pbg3Archives[16]; // 0x1bc, should be Pbg3Archive* in happyhavoc/th06
    Pbg3ArchiveName pbg3ArchiveNames[16]; // 0x1fc

    uint8_t hasD3dHardwareVertexProcessing; // 0x3fc
    uint8_t lockableBackbuffer; // 0x3fd
    uint8_t colorMode16Bits; // 0x3fe
    int8_t offset_3ff; // 0x3ff

    uint32_t startupTimeBeforeMenuMusic; // 0x400
    D3DCAPS8 d3dCaps; // 0x404
    // 0x4d8

    __forceinline void RedrawBackground(void) {
        // The game set it to 3 in several places, so we will do the same here.
        screenBackgroundRedrawFrames = 3;
    }
};
static_assert(sizeof(Supervisor) == 0x4d8);

}
}