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
constexpr unsigned HSCR_NUM_DIFFICULTIES = 5;
constexpr unsigned HSCR_NUM_CHARS_SHOTTYPES = 4;
constexpr unsigned HSCR_NUM_SCORES_SLOTS = 10;

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
    uint8_t isInGame;
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

struct Hscr {
    Th6k base;
    uint32_t score;
    uint8_t character;
    uint8_t difficulty;
    uint8_t stage;
    char name[9];
};
static_assert(sizeof(Hscr) == 0x1c);

struct ScoreListNode {
    ScoreListNode* prev;
    ScoreListNode* next;
    Hscr* data;
};
static_assert(sizeof(ScoreListNode) == 0xc);

struct ScoreDat {
    uint8_t xorseed[2];
    uint16_t csum;
    uint16_t unk_8;
    uint8_t unk[2];
    uint32_t dataOffset;
    ScoreListNode* scores;
    uint32_t fileLen;
};
static_assert(sizeof(ScoreDat) == 0x14);

struct ZunTimer {
    int32_t previous;
    float subFrame;
    int32_t current;
};
static_assert(sizeof(ZunTimer) == 0xc);

union AnmVmFlags {
    uint16_t flags;
    struct {
        uint32_t isVisible : 1;
        uint32_t flag1 : 1;
        uint32_t blendMode : 1;
        uint32_t colorOp : 1;
        uint32_t flag4 : 1;
        uint32_t usePosOffset : 1;
        uint32_t flip : 2;
        uint32_t anchor : 2;
        uint32_t posTime : 2;
        uint32_t zWriteDisable : 1;
        uint32_t isStopped : 1;
    };
};

struct AnmRawInstr {
    int16_t time;
    uint8_t opcode;
    uint8_t argsCount;
    uint32_t args[10];
};

struct AnmLoadedSprite {
    int32_t sourceFileIndex;
    Float2 startPixelInclusive;
    Float2 endPixelInclusive;
    float textureHeight;
    float textureWidth;
    Float2 uvStart;
    Float2 uvEnd;
    float heightPx;
    float widthPx;
    int32_t spriteId;
};
static_assert(sizeof(AnmLoadedSprite) == 0x38);

struct AnmVm {
    Float3 rotation;
    Float3 angleVel;
    float scaleY;
    float scaleX;
    float scaleInterpFinalY;
    float scaleInterpFinalX;
    Float2 uvScrollPos;
    ZunTimer currentTimeInScript;
    Matrix44 matrix;
    uint32_t color;
    AnmVmFlags flags;

    int16_t alphaInterpEndTime;
    int16_t scaleInterpEndTime;
    int16_t autoRotate;
    int16_t pendingInterrupt;
    int16_t posInterpEndTime;
    // Two padding bytes
    Float3 pos;
    float scaleInterpInitialY;
    float scaleInterpInitialX;
    ZunTimer scaleInterpTime;
    int16_t activeSpriteIndex;
    int16_t baseSpriteIndex;
    int16_t anmFileIndex;
    // Two padding bytes
    AnmRawInstr* beginingOfScript;
    AnmRawInstr* currentInstruction;
    AnmLoadedSprite* sprite;
    D3DCOLOR alphaInterpInitial;
    D3DCOLOR alphaInterpFinal;
    Float3 posInterpInitial;
    Float3 posInterpFinal;
    Float3 posOffset;
    ZunTimer posInterpTime;
    int32_t timeOfLastSpriteSet;
    ZunTimer alphaInterpTime;
    uint8_t fontWidth;
    uint8_t fontHeight;
    // Two final padding bytes
};
static_assert(sizeof(AnmVm) == 0x110);

enum ChainCallbackResult {
    CHAIN_CALLBACK_RESULT_CONTINUE_AND_REMOVE_JOB = (unsigned int)(0),
    CHAIN_CALLBACK_RESULT_CONTINUE = (unsigned int)(1),
    CHAIN_CALLBACK_RESULT_EXECUTE_AGAIN = (unsigned int)(2),
    CHAIN_CALLBACK_RESULT_BREAK = (unsigned int)(3),
    CHAIN_CALLBACK_RESULT_EXIT_GAME_SUCCESS = (unsigned int)(4),
    CHAIN_CALLBACK_RESULT_EXIT_GAME_ERROR = (unsigned int)(5),
    CHAIN_CALLBACK_RESULT_RESTART_FROM_FIRST_JOB = (unsigned int)(6),
};

enum ZunResult {
    ZUN_SUCCESS = 0,
    ZUN_ERROR = -1
};

using ChainCallback = ChainCallbackResult (*)(void*);
using ChainAddedCallback = ZunResult (*)(void*);
using ChainDeletedCallback = ZunResult (*)(void*);

struct ChainElem {
    short priority;
    uint16_t isHeapAllocated : 1;
    ChainCallback callback;
    ChainAddedCallback addedCallback;
    ChainDeletedCallback deletedCallback;
    ChainElem* prev;
    ChainElem* next;
    ChainElem* unkPtr;
    void* arg;
};
static_assert(sizeof(ChainElem) == 0x20);

struct ReplayDataInput {
    int32_t frameNum;
    uint16_t inputKey;
    uint16_t padding;
};

struct StageReplayData {
    int32_t score;
    int16_t randomSeed;
    int16_t pointItemsCollected;
    uint8_t power;
    int8_t livesRemaining;
    int8_t bombsRemaining;
    uint8_t rank;
    int8_t powerItemCountForScore;
    int8_t padding[3];
    ReplayDataInput replayInputs[53998];
};
static_assert(sizeof(StageReplayData) == 0x69780);

struct ReplayData {
    char magic[4];
    uint16_t version;
    uint8_t shottypeChara;
    uint8_t difficulty;
    int32_t checksum;
    uint8_t rngValue1;
    uint8_t rngValue2;
    int8_t key;
    int8_t rngValue3;
    char date[9];
    char name[8];
    int32_t score;
    float slowdownRate2;
    float slowdownRate;
    float slowdownRate3;
    StageReplayData* stageReplayData[7];
};
static_assert(sizeof(ReplayData) == 0x50);

enum ResultScreenState {
    RESULT_SCREEN_STATE_INIT = 0,
    RESULT_SCREEN_STATE_CHOOSING_DIFFICULTY,
    RESULT_SCREEN_STATE_EXITING,
    RESULT_SCREEN_STATE_BEST_SCORES_EASY,
    RESULT_SCREEN_STATE_BEST_SCORES_NORMAL,
    RESULT_SCREEN_STATE_BEST_SCORES_HARD,
    RESULT_SCREEN_STATE_BEST_SCORES_LUNATIC,
    RESULT_SCREEN_STATE_BEST_SCORES_EXTRA,
    RESULT_SCREEN_STATE_SPELLCARDS,
    RESULT_SCREEN_STATE_WRITING_HIGHSCORE_NAME,
    RESULT_SCREEN_STATE_SAVE_REPLAY_QUESTION,
    RESULT_SCREEN_STATE_CANT_SAVE_REPLAY,
    RESULT_SCREEN_STATE_CHOOSING_REPLAY_FILE,
    RESULT_SCREEN_STATE_WRITING_REPLAY_NAME,
    RESULT_SCREEN_STATE_OVERWRITE_REPLAY_FILE,
    RESULT_SCREEN_STATE_STATS_SCREEN,
    RESULT_SCREEN_STATE_STATS_TO_SAVE_TRANSITION,
    RESULT_SCREEN_STATE_EXIT,
};

struct ResultScreen {
    ScoreDat* scoreDat;
    int32_t frameTimer;
    int32_t resultScreenState;
    int32_t lastResultScreenState;
    int32_t cursor;
    int32_t lastBestScoresCursor;
    int32_t previousCursor;
    int32_t replayNumber;
    int32_t selectedCharacter;
    int32_t charUsed;
    int32_t lastSpellcardSelected;
    int32_t diffSelected;
    int32_t cheatCodeStep;
    char replayName[8];
    int32_t unk_3c;
    AnmVm unk_40[38];
    AnmVm unk_28a0[16];
    AnmVm unk_39a0;
    ScoreListNode scores[HSCR_NUM_DIFFICULTIES][HSCR_NUM_CHARS_SHOTTYPES];
    Hscr defaultScore[HSCR_NUM_DIFFICULTIES][HSCR_NUM_CHARS_SHOTTYPES][HSCR_NUM_SCORES_SLOTS];
    Hscr hscr;
    Th6k fileHeader;
    ChainElem* calcChain;
    ChainElem* drawChain;
    ReplayData replays[15];
    ReplayData defaultReplay;
};
static_assert(sizeof(ResultScreen) == 0x56b0);

struct ControllerMapping {
    int16_t shootButton;
    int16_t bombButton;
    int16_t focusButton;
    int16_t menuButton;
    int16_t upButton;
    int16_t downButton;
    int16_t leftButton;
    int16_t rightButton;
    int16_t skipButton;
};

struct GameConfiguration {
    ControllerMapping controllerMapping;
    // Always 0x102 for 1.02
    int32_t version;
    uint8_t lifeCount;
    uint8_t bombCount;
    uint8_t colorMode16bit;
    uint8_t musicMode;
    uint8_t playSounds;
    uint8_t defaultDifficulty;
    uint8_t windowed;
    // 0 = fullspeed, 1 = 1/2 speed, 2 = 1/4 speed.
    uint8_t frameskipConfig;
    int16_t padXAxis;
    int16_t padYAxis;
    int8_t unk[16];
    // GameConfigOpts bitfield.
    uint32_t opts;
};

struct MidiTrack {
    uint32_t trackPlaying;
    int32_t trackLengthOther;
    uint32_t trackLength;
    uint8_t opcode;
    uint8_t* trackData;
    uint8_t* curTrackDataCursor;
    uint8_t* startTrackDataMaybe;
    uint32_t unk1c;
};
static_assert(sizeof(MidiTrack) == 0x20);

struct MidiTimer
{
    virtual void DummyFunctionToGenerateVirtualTable() = 0;
    uint32_t timerId;
    TIMECAPS timeCaps;
};
static_assert(sizeof(MidiTimer) == 0x10);

struct MidiDevice {
    HMIDIOUT handle;
    uint32_t deviceId;
};
static_assert(sizeof(MidiDevice) == 0x8);

struct MidiChannel {
    uint8_t keyPressedFlags[16];
    uint8_t instrument;
    uint8_t instrumentBank;
    uint8_t pan;
    uint8_t effectOneDepth;
    uint8_t effectThreeDepth;
    uint8_t channelVolume;
    uint8_t modifiedVolume;
};

struct MidiOutput : MidiTimer {
    MIDIHDR* midiHeaders[32];
    int32_t midiHeadersCursor;
    uint8_t* midiFileData[32];
    int32_t numTracks;
    uint32_t format;
    int32_t divisions;
    int32_t tempo;
    uint32_t unk124;
    uint64_t volume;
    int64_t unk130;
    MidiTrack* tracks;
    MidiDevice midiOutDev;
    uint8_t unk144[16];
    MidiChannel channels[16];
    int8_t unk2c4;
    float fadeOutVolumeMultiplier;
    uint32_t fadeOutLastSetVolume;
    uint32_t unk2d0;
    uint32_t unk2d4;
    uint32_t unk2d8;
    uint32_t unk2dc;
    uint32_t fadeOutFlag;
    int32_t fadeOutInterval;
    int32_t fadeOutElapsedMS;
    uint32_t unk2ec;
    ULONGLONG unk2f0;
    ULONGLONG unk2f8;
};

struct IPbg3Parser {
    virtual void DummyFunctionToGenerateVirtualTable() = 0;
    uint32_t offsetInFile;
    uint32_t fileSize;
    uint32_t curByte;
    uint8_t bitIdxInCurByte;
    uint32_t crc;
};

struct IFileAbstraction {
};
struct FileAbstraction : public IFileAbstraction {
    virtual void DummyFunctionToGenerateVirtualTable() = 0;
    HANDLE handle;
    DWORD access;
};
static_assert(sizeof(FileAbstraction) == 0xc);

struct Pbg3Parser : public IPbg3Parser, public FileAbstraction {};
static_assert(sizeof(Pbg3Parser) == 0x24);

struct Pbg3Entry {
    uint32_t unk1;
    uint32_t unk2;
    uint32_t uncompressedSize;
    uint32_t dataOffset;
    uint32_t checksum;
    char filename[256];
};
static_assert(sizeof(Pbg3Entry) == 0x114);

using Pbg3ArchiveName = char[32];

struct Pbg3Archive {
    Pbg3Parser* parser;
    void* unk;
    uint32_t numOfEntries;
    uint32_t fileTableOffset;
    Pbg3Entry* entries;
};
static_assert(sizeof(Pbg3Archive) == 0x14);

enum SupervisorState {
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

struct Supervisor {
    HINSTANCE hInstance;
    PDIRECT3D8 d3dIface;
    PDIRECT3DDEVICE8 d3dDevice;
    uint32_t dinputIface;  // real type: LPDIRECTINPUT8
    uint32_t keyboard;  // real type: LPDIRECTINPUTDEVICE8A
    uint32_t controller;  // real type: LPDIRECTINPUTDEVICE8A
    int8_t controllerCaps[0x2c];  // real type: DIDEVCAPS
    HWND hwndGameWindow;
    Matrix44 viewMatrix;
    Matrix44 projectionMatrix;
    D3DVIEWPORT8 viewport;
    D3DPRESENT_PARAMETERS presentParameters;
    GameConfiguration cfg;
    GameConfiguration defaultConfig;
    int32_t calcCount;
    int32_t wantedState;
    int32_t curState;
    int32_t wantedState2;

    int32_t unk194;
    int32_t screenBorderForceRedrawFrames;
    uint32_t isInEnding;

    int32_t vsyncEnabled;
    int32_t lastFrameTime;
    float effectiveFramerateMultiplier;
    float framerateMultiplier;

    MidiOutput* midiOutput;

    float unk1b4;
    float unk1b8;

    Pbg3Archive* pbg3Archives[16];
    Pbg3ArchiveName pbg3ArchiveNames[16];

    uint8_t hasD3dHardwareVertexProcessing;
    uint8_t lockableBackbuffer;
    uint8_t colorMode16Bits;

    uint32_t startupTimeBeforeMenuMusic;
    D3DCAPS8 d3dCaps;
};
static_assert(sizeof(Supervisor) == 0x4d8);

struct MsgRawInstrArgPortraitAnmScript {
    int16_t portraitIdx;
    int16_t anmScriptIdx;
};
struct MsgRawInstrArgText {
    int16_t textColor;
    int16_t textLine;
    char text[1];
};
struct MsgRawInstrArgAnmInterrupt {
    int16_t unk1;
    uint8_t unk2;
};
union MsgRawInstrArgs {
    MsgRawInstrArgPortraitAnmScript portraitAnmScript;
    MsgRawInstrArgText text;
    int32_t dialogueSkippable;
    int32_t wait;
    MsgRawInstrArgAnmInterrupt anmInterrupt;
    int32_t music;
};
struct MsgRawInstr {
    uint16_t time;
    uint8_t opcode;
    uint8_t argSize;
    MsgRawInstrArgs args;
};
struct MsgRawHeader {
    int32_t numInstrs;
    MsgRawInstr* instrs[1];
};
static_assert(sizeof(MsgRawHeader) == 0x8);

struct GuiMsgVm {
    MsgRawHeader* msgFile;
    MsgRawInstr* currentInstr;
    int32_t currentMsgIdx;
    ZunTimer timer;
    int32_t framesElapsedDuringPause;
    AnmVm portraits[2];
    AnmVm dialogueLines[2];
    AnmVm introLines[2];
    D3DCOLOR textColorsA[4];
    D3DCOLOR textColorsB[4];
    uint32_t fontSize;
    uint32_t ignoreWaitCounter;
    uint8_t dialogueSkippable;
};
static_assert(sizeof(GuiMsgVm) == 0x6a8);

struct GuiFormattedText {
    Float3 pos;
    int32_t fmtArg;
    int32_t isShown;
    ZunTimer timer;
};
static_assert(sizeof(GuiFormattedText) == 0x20);

struct GuiImpl {
    AnmVm vms[26];
    uint8_t bossHealthBarState;
    AnmVm stageNameSprite;
    AnmVm songNameSprite;
    AnmVm playerSpellcardPortrait;
    AnmVm enemySpellcardPortrait;
    AnmVm bombSpellcardName;
    AnmVm enemySpellcardName;
    AnmVm bombSpellcardBackground;
    AnmVm enemySpellcardBackground;
    AnmVm loadingScreenSprite;
    GuiMsgVm msg;
    uint32_t finishedStage;
    uint32_t stageScore;
    GuiFormattedText bonusScore;
    GuiFormattedText fullPowerMode;
    GuiFormattedText spellCardBonus;
};
static_assert(sizeof(GuiImpl) == 0x2c44);

struct GuiFlags {
    uint32_t flag0 : 2;
    uint32_t flag1 : 2;
    uint32_t flag2 : 2;
    uint32_t flag3 : 2;
    uint32_t flag4 : 2;
};

// I have to rename this structure from Gui, to avoid shadowing the namespace thprac::Gui.
struct ZunGui {
    GuiFlags flags;
    GuiImpl* impl;
    float bombSpellcardBarLength;
    float blueSpellcardBarLength;
    uint32_t bossUIOpacity;
    int32_t eclSetLives;
    int32_t spellcardSecondsRemaining;
    int32_t lastSpellcardSecondsRemaining;
    bool bossPresent;
    float bossHealthBar1;
    float bossHealthBar2;
};
static_assert(sizeof(ZunGui) == 0x2c);

struct EclTimelineInstrArgs {
    uint32_t uintVar1;
    uint32_t uintVar2;
    uint32_t uintVar3;
    uint16_t ushortVar1;
    uint16_t ushortVar2;
    uint32_t uintVar4;
};
struct EclTimelineInstr {
    int16_t time;
    int16_t arg0;
    int16_t opCode;
    int16_t size;
    EclTimelineInstrArgs args;
};

enum EclVarId {
    ECL_VAR_I32_0 = -10001,
    ECL_VAR_I32_1 = -10002,
    ECL_VAR_I32_2 = -10003,
    ECL_VAR_I32_3 = -10004,
    ECL_VAR_F32_0 = -10005,
    ECL_VAR_F32_1 = -10006,
    ECL_VAR_F32_2 = -10007,
    ECL_VAR_F32_3 = -10008,
    ECL_VAR_I32_4 = -10009,
    ECL_VAR_I32_5 = -10010,
    ECL_VAR_I32_6 = -10011,
    ECL_VAR_I32_7 = -10012,
    ECL_VAR_DIFFICULTY = -10013,
    ECL_VAR_RANK = -10014,
    ECL_VAR_ENEMY_POS_X = -10015,
    ECL_VAR_ENEMY_POS_Y = -10016,
    ECL_VAR_ENEMY_POS_Z = -10017,
    ECL_VAR_PLAYER_POS_X = -10018,
    ECL_VAR_PLAYER_POS_Y = -10019,
    ECL_VAR_PLAYER_POS_Z = -10020,
    ECL_VAR_PLAYER_ANGLE = -10021,
    ECL_VAR_ENEMY_TIMER = -10022,
    ECL_VAR_PLAYER_DISTANCE = -10023,
    ECL_VAR_ENEMY_LIFE = -10024,
    ECL_VAR_PLAYER_SHOT = -10025,
};

union EclRawInstrArg {
    struct {
        int8_t a;
        int8_t b;
        int8_t c;
        int8_t d;
    } by;
    struct {
        int16_t lo;
        int16_t hi;
    } sh;
    float f32;
    int32_t int32_t;
    EclVarId id;
};
struct EclRawInstrAluArgs { 
    EclVarId res;
    EclRawInstrArg arg1;
    EclRawInstrArg arg2;
    EclRawInstrArg arg3;
    EclRawInstrArg arg4;
};
struct EclRawInstrJumpArgs {
    int32_t time;
    int32_t offset;
    EclVarId var;
};
struct EclRawInstrCallArgs {
    int32_t eclSub;
    int32_t var0;
    float float0;
    EclVarId cmpLhs;
    int32_t cmpRhs;
};
struct EclRawInstrCmpArgs {
    EclRawInstrArg lhs;
    EclRawInstrArg rhs;
};
struct EclRawInstrMoveArgs {
    Float3 pos;
};
struct EclRawInstrAnmSetMainArgs {
    int32_t scriptIdx;
};
struct EclRawInstrAnmSetSlotArgs {
    int32_t vmIdx;
    int32_t scriptIdx;
};
struct EclRawInstrAnmSetDeathArgs {
    int8_t deathAnm1;
    int8_t deathAnm2;
    int8_t deathAnm3;
};
struct EclRawInstrBulletArgs {
    int16_t sprite;
    int16_t color;
    EclVarId count1;
    EclVarId count2;
    float speed1;
    float speed2;
    float angle1;
    float angle2;
    int32_t flags;
};
struct EclRawInstrLaserArgs {
    int16_t sprite;
    int16_t color;
    float angle;
    float speed;
    float startOffset;
    float endOffset;
    float startLength;
    float width;
    int32_t startTime;
    int32_t duration;
    int32_t despawnDuration;
    int32_t hitboxStartTime;
    int32_t hitboxEndDelay;
    int32_t flags;
};
struct EclRawInstrLaserOpArgs {
    int32_t laserIdx;
    Float3 arg1;
};
struct EclRawInstrBulletEffectsArgs {
    EclVarId ivar1;
    EclVarId ivar2;
    EclVarId ivar3;
    EclVarId ivar4;
    float fvar1;
    float fvar2;
    float fvar3;
    float fvar4;
};
struct EclRawInstrSetInt {
    int32_t int32_t;
};
struct EclRawInstrSpellcardEffectArgs {
    int32_t effectColorId;
    Float3 pos;
    float effectDistance;
};
struct EclRawInstrMoveBoundSetArgs {
    Float2 lowerMoveLimit;
    Float2 upperMoveLimit;
};
struct EclRawInstrAnmSetPosesArgs {
    int16_t anmExDefault;
    int16_t anmExFarLeft;
    int16_t anmExFarRight;
    int16_t anmExLeft;
    int16_t anmExRight;
};
struct EclRawInstrSetInterruptArgs {
    int32_t interruptSub;
    int32_t interruptId;
};
struct EclRawInstrSpellcardStartArgs {
    int16_t spellcardSprite;
    int16_t spellcardId;
    char spellcardName[1];
};
struct EclRawInstrEffectParticleArgs {
    int32_t effectId;
    int32_t numParticles;
    uint32_t particleColor;
};
struct EclRawInstrTimeSetArgs {
    EclVarId timeToSet;
};
enum ItemType {  // This enum is 1 byte in size on Enemy
    ITEM_NO_ITEM = -2,
    ITEM_RANDOM_ITEM = -1,
    ITEM_POWER_SMALL,
    ITEM_POINT,
    ITEM_POWER_BIG,
    ITEM_BOMB,
    ITEM_FULL_POWER,
    ITEM_LIFE,
    ITEM_POINT_BULLET,
};
struct EclRawInstrDropItemArgs {
    ItemType itemId;
};
struct EclRawInstrEnemyCreateArgs {
    int32_t subId;
    Float3 pos;
    int16_t life;
    int16_t itemDrop;
    int32_t score;
};
static_assert(sizeof(EclRawInstrEnemyCreateArgs) == 0x18);
struct EclRawInstrAnmInterruptSlotArgs {
    int32_t vmId;
    int32_t interruptId;
};
enum SoundIdx {
    NO_SOUND = -1,
    SOUND_SHOOT = 0,
    SOUND_1 = 1,
    SOUND_2 = 2,
    SOUND_3 = 3,
    SOUND_PICHUN = 4,
    SOUND_5 = 5,
    SOUND_BOMB_REIMARI = 6,
    SOUND_7 = 7,
    SOUND_8 = 8,
    SOUND_SHOOT_BOSS = 9,
    SOUND_SELECT = 10,
    SOUND_BACK = 11,
    SOUND_MOVE_MENU = 12,
    SOUND_BOMB_REIMU_A = 13,
    SOUND_BOMB = 14,
    SOUND_F = 15,
    SOUND_BOSS_LASER = 16,
    SOUND_BOSS_LASER_2 = 17,
    SOUND_12 = 18,
    SOUND_BOMB_MARISA_B = 19,
    SOUND_TOTAL_BOSS_DEATH = 20,
    SOUND_15 = 21,
    SOUND_16 = 22,
    SOUND_17 = 23,
    SOUND_18 = 24,
    SOUND_WTF_IS_THAT_LMAO = 25,
    SOUND_1A = 26,
    SOUND_1B = 27,
    SOUND_1UP = 28,
    SOUND_1D = 29,
    SOUND_GRAZE = 30,
    SOUND_POWERUP = 31,
};
struct EclRawInstrBulletSoundArgs {
    SoundIdx bulletSfx;
};
struct EclRawInstrBulletRankInfluenceArgs {
    float bulletRankSpeedLow;
    float bulletRankSpeedHigh;
    int32_t bulletRankAmount1Low;
    int32_t bulletRankAmount1High;
    int32_t bulletRankAmount2Low;
    int32_t bulletRankAmount2High;
};
struct EclRawInstrExInstrArgs
{
    uint32_t exInstrIndex;
    union {
        int32_t i32Param;
        uint8_t u8Param;
    };
};

union EclRawInstrArgs {
    EclRawInstrAluArgs alu;
    EclRawInstrCmpArgs cmp;
    EclRawInstrJumpArgs jump;
    EclRawInstrCallArgs call;
    EclRawInstrAnmSetMainArgs anmSetMain;
    EclRawInstrAnmSetPosesArgs anmSetPoses;
    EclRawInstrAnmSetSlotArgs anmSetSlot;
    EclRawInstrAnmSetDeathArgs anmSetDeath;
    EclRawInstrMoveArgs move;
    EclRawInstrBulletArgs bullet;
    EclRawInstrLaserArgs laser;
    EclRawInstrLaserOpArgs laserOp;
    EclRawInstrBulletEffectsArgs bulletEffects;
    EclRawInstrSpellcardEffectArgs spellcardEffect;
    EclRawInstrMoveBoundSetArgs moveBoundSet;
    EclRawInstrSetInterruptArgs setInterrupt;
    EclRawInstrSpellcardStartArgs spellcardStart;
    EclRawInstrEffectParticleArgs effectParticle;
    EclRawInstrTimeSetArgs timeSet;
    EclRawInstrDropItemArgs dropItem;
    EclRawInstrEnemyCreateArgs enemyCreate;
    EclRawInstrAnmInterruptSlotArgs anmInterruptSlot;
    EclRawInstrBulletSoundArgs bulletSound;
    EclRawInstrBulletRankInfluenceArgs bulletRankInfluence;
    EclRawInstrExInstrArgs exInstr;
    int32_t setInt;
};

struct EclRawInstr {
    int32_t time;
    int16_t opCode;
    int16_t offsetToNext;
    uint8_t unk_8;
    // Bitfield where each bit tells us whether we should skip this instruction
    // on that difficulty (1) or run it (0).
    uint8_t skipForDifficulty;
    uint8_t unk_a;
    uint8_t unk_b;
    EclRawInstrArgs args;
};

struct Enemy;
struct EnemyEclContext {
    EclRawInstr* currentInstr;
    ZunTimer time;
    void (*funcSetFunc)(Enemy* , EclRawInstr* );
    int32_t var0;
    int32_t var1;
    int32_t var2;
    int32_t var3;
    float float0;
    float float1;
    float float2;
    float float3;
    int32_t var4;
    int32_t var5;
    int32_t var6;
    int32_t var7;
    int32_t compareRegister;
    uint16_t subId;
};
static_assert(sizeof(EnemyEclContext) == 0x4c);

struct EnemyBulletShooter {
    int16_t sprite;
    int16_t spriteOffset;
    Float3 position;
    float angle1;
    float angle2;
    float speed1;
    float speed2;
    float exFloats[4];
    int32_t exInts[4];
    int32_t unk_40;
    int16_t count1;
    int16_t count2;
    uint16_t aimMode;
    uint16_t unk_4a;
    uint32_t flags;
    SoundIdx sfx;
};
static_assert(sizeof(EnemyBulletShooter) == 0x54);

struct EnemyFlags {
    // First byte
    uint8_t movementMode : 2;
    uint8_t movementEaseType : 3;
    uint8_t shootingDisabled : 1;
    uint8_t invertX : 1;
    uint8_t isSlotOccupied : 1;

    // Second byte
    uint8_t isInteractable : 1;
    uint8_t isCollidable : 1;
    uint8_t hasBeenInBounds : 1;
    uint8_t isBoss : 1;
    uint8_t isDamageable : 1;
    uint8_t deathMode : 3;

    // Third byte
    bool shouldClampPos : 1;
    uint8_t rotateAnm : 1;
    uint8_t disableCallStack : 1;
    uint8_t isInvisible : 1;
    uint8_t isTimeoutSpell : 1;

    // Rest is padding.
};

struct EnemyLaserShooter {
    int16_t sprite;
    int16_t spriteOffset;
    Float3 position;
    float angle;
    uint32_t unk_14;
    float speed;
    uint32_t unk_1c;
    float startOffset;
    float endOffset;
    float startLength;
    float width;
    int32_t startTime;
    int32_t duration;
    int32_t despawnDuration;
    int32_t hitboxStartTime;
    int32_t hitboxEndDelay;
    uint32_t unk_44;
    uint16_t type;
    uint32_t flags;
    uint32_t unk_50;
};
static_assert(sizeof(EnemyLaserShooter) == 0x54);

struct Laser {
    AnmVm vm0;
    AnmVm vm1;
    Float3 pos;
    float angle;
    float startOffset;
    float endOffset;
    float startLength;
    float width;
    float speed;
    int32_t startTime;
    int32_t hitboxStartTime;
    int32_t duration;
    int32_t despawnDuration;
    int32_t hitboxEndDelay;
    int32_t inUse;
    ZunTimer timer;
    uint16_t flags;
    int16_t color;
    uint8_t state;
};
static_assert(sizeof(Laser) == 0x270);

enum EffectCallbackResult {
    EFFECT_CALLBACK_RESULT_STOP = 0,
    EFFECT_CALLBACK_RESULT_DONE = 1
};

struct Effect;
using EffectUpdateCallback = int32_t(*)(Effect*);
struct Effect {
    AnmVm vm;
    Float3 pos1;
    Float3 unk_11c;
    Float3 unk_128;
    Float3 position;
    Float3 pos2;
    Float4 quaternion;
    float unk_15c;
    float angleRelated;
    ZunTimer timer;
    int32_t unk_170;
    EffectUpdateCallback updateCallback;
    int8_t inUseFlag;
    int8_t effectId;
    int8_t unk_17a;
    int8_t unk_17b;
};
static_assert(sizeof(Effect) == 0x17c);

struct Enemy {
    AnmVm primaryVm;
    AnmVm vms[8];
    EnemyEclContext currentContext;
    EnemyEclContext savedContextStack[8];
    int32_t stackDepth;
    int32_t unk_c40;
    int32_t deathCallbackSub;
    int32_t interrupts[8];
    int32_t runInterrupt;
    Float3 position;
    Float3 hitboxDimensions;
    Float3 axisSpeed;
    float angle;
    float angularVelocity;
    float speed;
    float acceleration;
    Float3 shootOffset;
    Float3 moveInterp;
    Float3 moveInterpStartPos;
    ZunTimer moveInterpTimer;
    int32_t moveInterpStartTime;
    float bulletRankSpeedLow;
    float bulletRankSpeedHigh;
    int16_t bulletRankAmount1Low;
    int16_t bulletRankAmount1High;
    int16_t bulletRankAmount2Low;
    int16_t bulletRankAmount2High;
    int32_t life;
    int32_t maxLife;
    int32_t score;
    ZunTimer bossTimer;
    uint32_t color;
    EnemyBulletShooter bulletProps;
    int32_t shootInterval;
    ZunTimer shootIntervalTimer;
    EnemyLaserShooter laserProps;
    Laser* lasers[32]; // This looks like a structure
    int32_t laserStore;
    uint8_t deathAnm1;
    uint8_t deathAnm2;
    uint8_t deathAnm3;
    int8_t itemDrop;
    uint8_t bossId;
    uint8_t unk_e41;
    ZunTimer exInsFunc10Timer;
    EnemyFlags flags;
    uint8_t anmExFlags;
    int16_t anmExDefaults;
    int16_t anmExFarLeft;
    int16_t anmExFarRight;
    int16_t anmExLeft;
    int16_t anmExRight;
    Float2 lowerMoveLimit;
    Float2 upperMoveLimit;
    Effect* effectArray[12];
    int32_t effectIdx;
    float effectDistance;
    int32_t lifeCallbackThreshold;
    int32_t lifeCallbackSub;
    int32_t timerCallbackThreshold;
    int32_t timerCallbackSub;
    float exInsFunc6Angle;
    ZunTimer exInsFunc6Timer;
};
static_assert(sizeof(Enemy) == 0xec8);

struct RunningSpellcardInfo {
    uint32_t isCapturing;
    uint32_t isActive;
    int32_t captureScore;
    uint32_t idx;
    uint32_t usedBomb;
};
static_assert(sizeof(RunningSpellcardInfo) == 0x14);

struct EnemyManager {
    char* stgEnmAnmFilename;
    char* stgEnm2AnmFilename;
    Enemy enemyTemplate;
    Enemy enemies[257];
    Enemy* bosses[8];
    uint16_t randomItemSpawnIndex;
    uint16_t randomItemTableIndex;
    int32_t enemyCount;
    int8_t unk_ee5c0[4];
    RunningSpellcardInfo spellcardInfo;
    int32_t unk_ee5d8;
    EclTimelineInstr* timelineInstr;
    ZunTimer timelineTime;
};
static_assert(sizeof(EnemyManager) == 0xee5ec);

struct PatchouliShottypeVars {
    struct {
        int32_t var1;
        int32_t var2;
        int32_t var3;
    } shotVars[2];
};
static_assert(sizeof(PatchouliShottypeVars) == 0x18);

struct PlayerRect {
    float posX;
    float posY;
    float sizeX;
    float sizeY;
};
static_assert(sizeof(PlayerRect) == 0x10);

struct PlayerBullet {
    AnmVm sprite;
    Float3 position;
    Float3 size;
    Float2 velocity;
    float sidewaysMotion;
    Float3 unk_134;
    ZunTimer unk_140;
    int16_t damage;
    int16_t bulletState;
    int16_t bulletType;
    int16_t unk_152;
    int16_t spawnPositionIdx;
};
static_assert(sizeof(PlayerBullet) == 0x158);

enum FireBulletResult : int32_t {
    FBR_STOP_SPAWNING = -2,
    FBR_SPAWN_MORE = -1,
};
struct Player;
using FireBulletCallback = FireBulletResult (*)(Player*, PlayerBullet*, uint32_t, uint32_t);

struct CharacterData {
    float orthogonalMovementSpeed;
    float orthogonalMovementSpeedFocus;
    float diagonalMovementSpeed;
    float diagonalMovementSpeedFocus;
    FireBulletCallback fireBulletCallback;
    FireBulletCallback fireBulletFocusCallback;
};
static_assert(sizeof(CharacterData) == 0x18);

enum PlayerDirection {
    MOVEMENT_NONE,
    MOVEMENT_UP,
    MOVEMENT_DOWN,
    MOVEMENT_LEFT,
    MOVEMENT_RIGHT,
    MOVEMENT_UP_LEFT,
    MOVEMENT_UP_RIGHT,
    MOVEMENT_DOWN_LEFT,
    MOVEMENT_DOWN_RIGHT
};

struct PlayerBombInfo {
    uint32_t isInUse;
    int32_t duration;
    ZunTimer timer;
    void (*calc)(Player* p);
    void (*draw)(Player* p);
    int32_t reimuABombProjectilesState[8];
    float reimuABombProjectilesRelated[8];
    Float3 bombRegionPositions[8];
    Float3 bombRegionVelocities[8];
    AnmVm sprites[8][4];
};
static_assert(sizeof(PlayerBombInfo) == 0x231c);

struct Player {
    AnmVm playerSprite;
    AnmVm orbsSprite[3];
    Float3 positionCenter;
    Float3 unk_44c;
    Float3 hitboxTopLeft;
    Float3 hitboxBottomRight;
    Float3 grabItemTopLeft;
    Float3 grabItemBottomRight;
    Float3 hitboxSize;
    Float3 grabItemSize;
    Float3 orbsPosition[2];
    Float3 bombRegionPositions[32];
    Float3 bombRegionSizes[32];
    int32_t bombRegionDamages[32];
    int32_t unk_838[32];
    PlayerRect bombProjectiles[16];
    ZunTimer laserTimer[2];
    float horizontalMovementSpeedMultiplierDuringBomb;
    float verticalMovementSpeedMultiplierDuringBomb;
    int32_t respawnTimer;
    int32_t bulletGracePeriod;
    int8_t playerState;
    uint8_t unk_9e1;
    int8_t orbState;
    int8_t isFocus;
    uint8_t unk_9e4;
    ZunTimer focusMovementTimer;
    CharacterData characterData;
    PlayerDirection playerDirection;
    float previousHorizontalSpeed;
    float previousVerticalSpeed;
    int16_t previousFrameInput;
    Float3 positionOfLastEnemyHit;
    PlayerBullet bullets[80];
    ZunTimer fireBulletTimer;
    ZunTimer invulnerabilityTimer;
    FireBulletCallback fireBulletCallback;
    FireBulletCallback fireBulletFocusCallback;
    PlayerBombInfo bombInfo;
    ChainElem* chainCalc;
    ChainElem* chainDraw1;
    ChainElem* chainDraw2;
};
static_assert(sizeof(Player) == 0x98f0);

struct GameWindow {
    HWND window;
    int32_t isAppClosing;
    int32_t lastActiveAppValue;
    int32_t isAppActive;
    uint8_t curFrame;
    int32_t screenSaveActive;
    int32_t lowPowerActive;
    int32_t powerOffActive;
};

enum GameState {
    STATE_STARTUP,
    STATE_PRE_INPUT,
    STATE_MAIN_MENU,
    STATE_OPTIONS,
    STATE_QUIT,
    STATE_KEYCONFIG,
    STATE_DIFFICULTY_LOAD,
    STATE_DIFFICULTY_SELECT,
    STATE_CHARACTER_LOAD,
    STATE_CHARACTER_SELECT,
    STATE_SCORE,
    STATE_SHOT_SELECT,
    STATE_REPLAY_LOAD,
    STATE_REPLAY_ANIM,
    STATE_REPLAY_UNLOAD,
    STATE_REPLAY_SELECT,
    STATE_MUSIC_ROOM,
    STATE_PRACTICE_LVL_SELECT,
};

struct MainMenu {
    AnmVm vm[122];
    int32_t cursor;
    int8_t padding[0x40];
    uint32_t unk_81e4;
    int32_t chosenReplay;
    int32_t replayFilesNum;
    GameState gameState;
    int32_t stateTimer;
    int32_t idleFrames;
    D3DCOLOR minimumOpacity;
    D3DCOLOR menuTextColor;
    D3DCOLOR color2;
    D3DCOLOR color1;
    int32_t numFramesSinceActive;
    uint32_t framesActive;
    uint32_t framesInactive;
    int8_t padding2[4];
    int16_t controlMapping[9];
    int8_t padding3[2];
    uint8_t colorMode16bit;
    uint8_t windowed;
    uint8_t frameskipConfig;
    int8_t padding4;
    ChainElem* chainCalc;
    ChainElem* chainDraw;
    char replayFilePaths[60][512];
    char replayFileName[60][8];
    ReplayData replayFileData[60];
    ReplayData* currentReplay;
    int32_t timeRelatedArrSize;
    float timeRelatedArr[16];
    uint32_t unk_10f24;
    uint32_t unk_10f28;
    int32_t frameCountForRefreshRateCalc;
    uint32_t lastFrameTime;
};
static_assert(sizeof(MainMenu) == 0x10f34);

struct EclRawHeader {
    int16_t subCount;
    int16_t mainCount;
    EclTimelineInstr* timelineOffsets[3];
    EclRawInstr* subOffsets[0];
};
static_assert(sizeof(EclRawHeader) == 0x10);

struct EclManager {
    EclRawHeader* eclFile;
    EclRawInstr** subTable;
    EclTimelineInstr* timeline;
};
static_assert(sizeof(EclManager) == 0xc);

struct AnmRawScript {
    uint32_t id;
    AnmRawInstr* firstInstruction;
};

struct AnmRawEntry {
    int32_t numSprites;
    int32_t numScripts;
    uint32_t textureIdx;
    int32_t width;
    int32_t height;
    uint32_t format;
    uint32_t colorKey;
    uint32_t nameOffset;
    uint32_t spriteIdxOffset;
    uint32_t mipmapNameOffset;
    uint32_t version;
    uint32_t unk1;
    uint32_t textureOffset;
    uint32_t hasData;
    uint32_t nextOffset;
    uint32_t unk2;
    uint32_t spriteOffsets[10];
    AnmRawScript scripts[10];
};
static_assert(sizeof(AnmRawEntry) == 0xb8);

struct RenderVertexInfo {
    Float3 position;
    Float2 textureUV;
};
static_assert(sizeof(RenderVertexInfo) == 0x14);

struct AnmManager {
    AnmLoadedSprite sprites[2048];
    AnmVm virtualMachine;
    IDirect3DTexture8* textures[264];
    void* imageDataArray[256];
    int32_t maybeLoadedSpriteCount;
    AnmRawInstr* scripts[2048];
    int32_t spriteIndices[2048];
    AnmRawEntry* anmFiles[128];
    uint32_t anmFilesSpriteIndexOffsets[128];
    IDirect3DSurface8* surfaces[32];
    IDirect3DSurface8* surfacesBis[32];
    uint8_t surfaceSourceInfo[32][0x14];  // real type: D3DXIMAGE_INFO[32]
    D3DCOLOR currentTextureFactor;
    IDirect3DTexture8* currentTexture;
    uint8_t currentBlendMode;
    uint8_t currentColorOp;
    uint8_t currentVertexShader;
    uint8_t currentZWriteDisable;
    AnmLoadedSprite* currentSprite;
    IDirect3DVertexBuffer8* vertexBuffer;
    RenderVertexInfo vertexBufferContents[4];
    int32_t screenshotTextureId;
    int32_t screenshotLeft;
    int32_t screenshotTop;
    int32_t screenshotWidth;
    int32_t screenshotHeight;
};
static_assert(sizeof(AnmManager) == 0x2112c);
}
}