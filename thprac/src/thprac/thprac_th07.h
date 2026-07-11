#pragma once
#include "thprac_games.h"
#include "thprac_utils.h"

#include "../3rdParties/d3d8/include/d3d8.h"

namespace THPrac {
namespace TH07 {

// Struct definitions from https://github.com/exphp-share/th-re-data (modified)

__declspec(align(4)) struct Globals {
    int32_t displayed_score;
    int32_t true_score;
    int32_t __dword_8;
    int32_t displayed_high_score;
    uint8_t continues_used_in_high_score;
    int32_t _other_graze;
    int32_t graze;
    int32_t spell_bonus;
    uint8_t continues_used;
    int32_t point_stage;
    int32_t point_total;
    int32_t point_extends;
    int32_t next_extend_at;
    int32_t RNG_A[0x7];
    float miss_count;
    float RNG_C[0x2];
    float life_count;
    float RNG_D[0x2];
    float bomb_count;
    float bombs_used;
    float RNG_E[0x3];
    float power;
    float RNG_F[0x2];
    int32_t cherry_base;
    int32_t RNG_B[0x8];
    int32_t rng_temp;
    int32_t rng_total;
    int32_t RNG_G[0x5];
};

struct ScorefileChapterHeader {
    char magic[4];
    uint16_t size_1;
    uint16_t size_2;
    uint32_t version;
};
static_assert(sizeof(ScorefileChapterHeader) == 0xC);

struct ScorefileCatk {
    ScorefileChapterHeader header;
    uint32_t shottype_max_bonuses[0x6];
    uint32_t best_max_bonus;
    uint16_t spell_number;
    uint8_t name_hash;
    char spell_name[0x31];
    uint16_t shottype_attemps[0x6];
    uint16_t total_attempts;
    uint16_t shottype_captures[0x6];
    uint16_t total_captures;
};
static_assert(sizeof(ScorefileCatk) == 0x78);

struct ScorefileClrd {
    ScorefileChapterHeader header;
    uint8_t clear_amounts[0x6];
    uint8_t continue_amounts[0x6];
    uint8_t full_shottype;
};
static_assert(sizeof(ScorefileClrd) == 0x1c);

struct ScorefilePscr {
    ScorefileChapterHeader header;
    int32_t attempts;
    int32_t highscore;
    uint8_t full_shottype;
    uint8_t difficulty;
    uint8_t stage;
};
static_assert(sizeof(ScorefilePscr) == 0x18);

struct ScorefilePlstTime {
    uint32_t hours;
    uint32_t minutes;
    uint32_t seconds;
    uint32_t microseconds;
};
static_assert(sizeof(ScorefilePlstTime) == 0x10);

struct ScorefilePlstCounts {
    uint32_t total_attempts;
    uint32_t shottype_attempts[0x6];
    uint32_t retries;
    uint32_t clears;
    uint32_t continues;
    uint32_t practices;
};
static_assert(sizeof(ScorefilePlstCounts) == 0x2c);

struct ScorefilePlst {
    ScorefileChapterHeader header;
    ScorefilePlstTime total_time;
    ScorefilePlstTime game_time;
    ScorefilePlstCounts difficulty_stats[0x6];
    ScorefilePlstCounts total_stats;
};
static_assert(sizeof(ScorefilePlst) == 0x160);

struct Config {
    char _unk_0[0x14];
    uint32_t version;
    uint16_t deadzone_x;
    uint16_t deadzone_y;
    uint8_t extra_lives;
    uint8_t __byte_1d;
    uint8_t graphic_format;
    uint8_t bgm_format;
    uint8_t __byte_20;
    uint8_t last_selected_diff;
    uint8_t fullscreen;
    uint8_t __frameskip_setting;
    uint8_t _unk_1[0x10];
    uint32_t flags;
};
static_assert(sizeof(Config) == 0x38);

struct GameManager {
    void* _unk_0;
    Config* config;
    Globals* globals;
    char _unk_1[4];
    int32_t difficulty;
    int32_t diff_mask;
    ScorefileCatk card_data[0x8d];
    ScorefileCatk card_data2[0x8d];
    ScorefileClrd clear_data[0x6];
    ScorefilePscr practice_scores[0x6][0x6][0x4];
    ScorefilePlst play_stats;
    int32_t __dword_93D0;
    char __byte_93D4;
    uint8_t character;
    uint8_t character_shottype;
    uint8_t full_shottype;
    uint32_t flags_93D8;
    uint8_t __byte_93DC;
    uint8_t __byte_93DD;
    uint8_t __byte_93DE;
    uint8_t __byte_93DF;
    int32_t __dword_93E0;
    char _unk_2[0x200];
    int16_t _stage_starting_rng;
    int32_t _unk_3;
    int32_t stage;
    uint32_t _unk_5;
    Float2 stg_frame_pos;
    Float2 stg_frame_size;
    Float2 __float2_9604;
    Float2 __float2_960C;
    float float_rng_total;
    int32_t cherry_max;
    int32_t cherry;
    int32_t cherry_plus;
    uint32_t _unk_4;
    int32_t __int_9628;
    uint32_t __uint_962C;
    int32_t __dword_9630;
    int32_t rank;
    int32_t rank_max;
    int32_t rank_min;
    int32_t subrank;
};
static_assert(sizeof(GameManager) == 0x9644);
static_assert(offsetof(GameManager, config) == 0x4);
static_assert(offsetof(GameManager, globals) == 0x8);
static_assert(offsetof(GameManager, difficulty) == 0x10);
static_assert(offsetof(GameManager, diff_mask) == 0x14);
static_assert(offsetof(GameManager, card_data) == 0x18);
static_assert(offsetof(GameManager, card_data2) == 0x4230);
static_assert(offsetof(GameManager, clear_data) == 0x8448);
static_assert(offsetof(GameManager, practice_scores) == 0x84f0);
static_assert(offsetof(GameManager, play_stats) == 0x9270);
static_assert(offsetof(GameManager, __dword_93D0) == 0x93d0);
static_assert(offsetof(GameManager, __byte_93D4) == 0x93d4);
static_assert(offsetof(GameManager, character) == 0x93d5);
static_assert(offsetof(GameManager, character_shottype) == 0x93d6);
static_assert(offsetof(GameManager, full_shottype) == 0x93d7);
static_assert(offsetof(GameManager, flags_93D8) == 0x93d8);
static_assert(offsetof(GameManager, __byte_93DC) == 0x93dc);
static_assert(offsetof(GameManager, __byte_93DD) == 0x93dd);
static_assert(offsetof(GameManager, __byte_93DE) == 0x93de);
static_assert(offsetof(GameManager, __byte_93DF) == 0x93df);
static_assert(offsetof(GameManager, __dword_93E0) == 0x93e0);
static_assert(offsetof(GameManager, _stage_starting_rng) == 0x95e4);
static_assert(offsetof(GameManager, stage) == 0x95ec);
static_assert(offsetof(GameManager, stg_frame_pos) == 0x95f4);
static_assert(offsetof(GameManager, stg_frame_size) == 0x95fc);
static_assert(offsetof(GameManager, __float2_9604) == 0x9604);
static_assert(offsetof(GameManager, __float2_960C) == 0x960c);
static_assert(offsetof(GameManager, float_rng_total) == 0x9614);
static_assert(offsetof(GameManager, cherry_max) == 0x9618);
static_assert(offsetof(GameManager, cherry) == 0x961c);
static_assert(offsetof(GameManager, cherry_plus) == 0x9620);
static_assert(offsetof(GameManager, __int_9628) == 0x9628);
static_assert(offsetof(GameManager, __uint_962C) == 0x962c);
static_assert(offsetof(GameManager, __dword_9630) == 0x9630);
static_assert(offsetof(GameManager, rank) == 0x9634);
static_assert(offsetof(GameManager, rank_max) == 0x9638);
static_assert(offsetof(GameManager, rank_min) == 0x963c);
static_assert(offsetof(GameManager, subrank) == 0x9640);

enum FogState : uint8_t {
    FOG_UNSET = 0xFF,
    FOG_DISABLED = 0,
    FOG_ENABLED = 1
};

struct Supervisor {
    HINSTANCE h_instance;
    IDirect3D8* d3d;
    IDirect3DDevice8* d3d_device;
    uint32_t dinput_iface;  // real type: LPDIRECTINPUT8
    uint32_t keyboard;  // real type: LPDIRECTINPUTDEVICE8A
    uint32_t controller;  // real type: LPDIRECTINPUTDEVICE8A
    uint8_t controller_caps[0x2C];  // real type: DIDEVCAPS
    HWND hwnd_game_window;
    D3DMATRIX view_matrix;
    D3DMATRIX projection_matrix;
    D3DVIEWPORT8 viewport;
    char present_params[0x36]; 
    Config config;
    int32_t calc_count;
    uint32_t gamemode;
    uint32_t gamemode_next;
    uint32_t gamemode_prev;
    int32_t unk_160;
    int32_t gui_update_frames;
    int32_t is_in_ending;
    int32_t disable_vsync;
    int32_t could_set_refresh_rate;
    int32_t last_frame_time;
    float framerate_multiplier;
    struct MidiOutput* midi_output;
    float lag_percentage_numerator;
    float lag_percentage_denominator;
    int16_t replay_fps;
    uint8_t field29_0x18a;
    uint8_t field30_0x18b;
    uint32_t flags;
    uint32_t total_play_time;
    uint32_t system_time;
    D3DCAPS8 device_caps;
    char _unk_3[0x50];
    FogState current_fog_enabled;
    int32_t exe_checksum;
    int32_t exe_size;
    int32_t ver_file_size;
    char* ver_file;
};
static_assert(offsetof(Supervisor, d3d) == 0x4);
static_assert(offsetof(Supervisor, d3d_device) == 0x8);
static_assert(offsetof(Supervisor, view_matrix) == 0x48);
static_assert(offsetof(Supervisor, projection_matrix) == 0x88);
static_assert(offsetof(Supervisor, viewport) == 0xc8);
static_assert(offsetof(Supervisor, present_params) == 0xe0);
static_assert(offsetof(Supervisor, config) == 0x118);
static_assert(offsetof(Supervisor, gamemode) == 0x154);
static_assert(offsetof(Supervisor, gamemode_next) == 0x158);
static_assert(offsetof(Supervisor, gamemode_prev) == 0x15C);
static_assert(offsetof(Supervisor, device_caps) == 0x198);
static_assert(offsetof(Supervisor, current_fog_enabled) == 0x2bc);

// The following struct definitions are from the GensokyoClub's decompilation project

struct AnmLoadedSprite {
    int32_t sourceFileIndex;
    Float2 startPixelInclusive;
    Float2 endPixelExclusive;
    float textureHeight;
    float textureWidth;
    Float2 uvStart;
    Float2 uvEnd;
    float heightPx;
    float widthPx;
    Float2 uvScale;
    int32_t spriteID;
};

struct ZunTimer {
    int32_t previous;
    float subFrame;
    int32_t current;
};
static_assert(offsetof(ZunTimer, current) == 0x8);

struct ZunColor_struct {
    uint8_t b;
    uint8_t g;
    uint8_t r;
    uint8_t a;
};

union ZunColor {
    D3DCOLOR asD3DCOLOR;
    ZunColor_struct asStruct;
};

struct AnmVMPrefix {
    Float3 rotation;
    Float3 angleVel;
    Float2 scale;
    Float2 scaleGrowth;
    Float2 uvScrollPos;
    ZunTimer currentTimeInScript;
    ZunTimer waitTimer;
    ZunTimer interpCurrentTimes[5];
    ZunTimer interpEndTimes[5];
    uint8_t interpModes[5];
    uint8_t field10_0xc5;
    uint8_t field11_0xc6;
    uint8_t field12_0xc7;
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
    Float2 uvScrollVel;
    Matrix44 matrix1;
    Matrix44 matrix2;
    Matrix44 matrix3;
    ZunColor color1;
    ZunColor color2;
    uint32_t flags;
    int16_t type;
    int16_t pendingInterrupt;
};

struct AnmRawInstr {
    uint16_t op_code;
    uint16_t offset_to_next;
    uint16_t time;
    uint16_t var_mask;
    uint32_t args[0xa];  // is variable length
};

struct GuiFormattedText {
    Float3 pos;
    int32_t fmtArg;
    int32_t type;
    struct ZunTimer timer;
};

struct AnmVM {
    AnmVMPrefix prefix;
    Float3 pos;
    int16_t activeSpriteIndex;
    int16_t baseSpriteIndex;
    int16_t scriptIndex;
    uint8_t field5_0x1da;
    uint8_t field6_0x1db;
    AnmRawInstr* beginningOfScript;
    AnmRawInstr* currentInstruction;
    AnmLoadedSprite* loadedSprite;
    Float3 posInitial;
    Float3 posFinal;
    Float3 rotateInitial;
    Float3 rotateFinal;
    Float2 scaleInitial;
    Float2 scaleFinal;
    ZunColor color1Initial;
    ZunColor color1Final;
    Float3 pos2;
    int32_t timeOfLastSpriteSet;
    uint8_t fontWidth;
    uint8_t fontHeight;
    uint8_t field22_0x242;
    uint8_t field23_0x243;
    uint8_t field24_0x244;
    uint8_t field25_0x245;
    uint8_t field26_0x246;
    uint8_t field27_0x247;
    uint8_t field28_0x248;
    uint8_t field29_0x249;
    uint8_t field30_0x24a;
    uint8_t field31_0x24b;
};

struct GuiMsgVm {
    uint8_t* msgFile;
    void* currentInstr;
    int32_t currentMsgIdx;
    ZunTimer timer;
    int32_t framesElapsedDuringPause;
    AnmVM portraits[2];
    AnmVM dialogueLines[2];
    AnmVM introLines[2];
    COLORREF textColorsA[4];
    COLORREF textColorsB[4];
    uint32_t fontSize;
    uint32_t ignoreWaitCounter;
    BOOL dialogueSkippable;
};

struct GuiImpl {
    AnmVM vms[33];
    uint8_t bossLifeBarState;
    uint8_t field2_0x4bcd;
    uint8_t field3_0x4bce;
    uint8_t field4_0x4bcf;
    AnmVM stageTextSprites[5];
    AnmVM playerSpellPortrait;
    AnmVM enemySpellPortrait;
    AnmVM bombSpellCutInSpriteA;
    AnmVM enemySpellCutInSpriteA;
    AnmVM bombSpellCutInSpriteB;
    AnmVM enemySpellCutInSpriteB;
    AnmVM bombSpellName;
    AnmVM enemySpellName;
    AnmVM bombSpellNameBackground;
    AnmVM enemySpellNameBackground;
    AnmVM loadingScreenSprite;
    AnmVM nowLoadingSprite;
    AnmVM arcadeZoneSprite;
    AnmVM enemySpellStatsDigit;
    AnmVM enemySpellStats;
    AnmVM stageTransitionSprites[14][12];
    uint32_t stageTransitionActiveScriptCount;
    GuiMsgVm msg;
    uint32_t stageClearScreenCounter;
    int32_t clearBonusTotal;
    BOOL finishedStage;
    GuiFormattedText bonusScore;
    GuiFormattedText popupText;
    GuiFormattedText spellcardBonus;
    int32_t clearBonusPower;
    int32_t clearBonusPointItems;
    int32_t clearBonusCherryMax;
    int32_t clearBonusGraze;
};
static_assert(offsetof(GuiImpl, msg.dialogueSkippable) == 0x209b0);

// I have to rename this structure from Gui, to avoid shadowing the namespace thprac::Gui.
struct ZunGui {
    int32_t frameNumber;
    uint32_t flags;
    GuiImpl* impl;
    float bombSpellcardBarLength;  // leftover from EoSD 
    float blueSpellcardBarLength;  // leftover from EoSD
    uint32_t bossUIOpacity;
    int32_t eclSetLives;
    int32_t spellcardSecondsRemaining;
    int32_t previousSpellcardSecondsRemaining;
    BOOL bossPresent;
    uint8_t field10_0x25;
    uint8_t field11_0x26;
    uint8_t field12_0x27;
    float bossLifeBarMaxSize;
    float bossLifeBarSize;
    uint8_t field15_0x30;
    uint8_t field16_0x31;
    uint8_t field17_0x32;
    uint8_t field18_0x33;
    float bossLifeBarSegmentStop[8];
    float bossLifeBarSegmentStart[8];
    int32_t bossLifeBarSegmentColor[8];
};

enum ChainCallbackResult {
    CHAIN_CALLBACK_RESULT_CONTINUE_AND_REMOVE_JOB = 0,
    CHAIN_CALLBACK_RESULT_CONTINUE = 1,
    CHAIN_CALLBACK_RESULT_EXECUTE_AGAIN = 2,
    CHAIN_CALLBACK_RESULT_BREAK = 3,
    CHAIN_CALLBACK_RESULT_EXIT_GAME_SUCCESS = 4,
    CHAIN_CALLBACK_RESULT_EXIT_GAME_ERROR = 5,
    CHAIN_CALLBACK_RESULT_RESTART_FROM_FIRST_JOB = 6
};

enum ZunResult {
    ZUN_ERROR = -1,
    ZUN_SUCCESS = 0
};

struct ChainElem {
    int16_t priority;
    uint16_t isHeapAllocated;
    ChainCallbackResult (*callback)(void*);
    ZunResult (*addedCallback)(void*);
    ZunResult (*deletedCallback)(void*);
    ChainElem* prev;
    ChainElem* next;
    ChainElem* self;
    void* arg;
};
static_assert(offsetof(ChainElem, callback) == 0x4);
static_assert(offsetof(ChainElem, next) == 0x14);
static_assert(offsetof(ChainElem, arg) == 0x1c);

struct Chain {
    ChainElem calcChain;
    ChainElem drawChain;
};

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

struct ReplayDataHeader {
    uint32_t magic;
    uint16_t version;
    uint8_t field2_0x6;
    uint8_t field3_0x7;
    uint32_t checksum;
    uint8_t unk_c;
    uint8_t unk_d;
    uint8_t field7_0xe;
    uint8_t field8_0xf;
    uint32_t unk_10;
    uint32_t compressedSize;
    uint32_t decompressedSize;
    struct StageReplayData* stageInputData[7];
    struct StageReplayData* stageFpsData[7];
};

struct ReplayData {
    ReplayDataHeader header;
    uint8_t unk_54;
    char minorVersion;
    uint8_t shotType;
    uint8_t difficulty;
    char date[6];
    char playerName[8];
    uint8_t field7_0x66;
    uint8_t field8_0x67;
    uint8_t field9_0x68;
    uint8_t field10_0x69;
    uint16_t majorVersion;
    uint32_t score;
    Config gameConfiguration;
    uint8_t field14_0xa8_to_field45_0xc7[0x20];
    float slowdownRate2;
    float slowdownRate;
    float slowdownRate3;
    uint32_t unk_d4;
    int32_t exeSize;
    int32_t exeChecksum;
    char exeVersion[8];
};

enum TitleCurrentScreen {
    STATE_MAIN_MENU = 0,
    New_Name = 1,
    TitleCurrentScreen_Option = 2,
    TitleCurrentScreen_KeyConfig = 3,
    TitleCurrentScreen_DifficultySelect = 4,
    TitleCurrentScreen_CharacterSelect = 5,
    TitleCurrentScreen_ShotSelect = 6,
    TitleCurrentScreen_Replay = 7,
    TitleCurrentScreen_DifficultySelectPractice = 8,
    TitleCurrentScreen_CharacterSelectPractice = 9,
    TitleCurrentScreen_ShotSelectPractice = 10,
    TitleCurrentScreen_PracticeStageSelect = 11,
    TitleCurrentScreen_DifficultySelectExtra = 12,
    TitleCurrentScreen_CharacterSelectExtra = 13,
    TitleCurrentScreen_ShotSelectExtra = 14
};

struct TitleScreen {
    int32_t cursor;
    int32_t prevCursor;
    int32_t menuDepth;
    int32_t stateTimer;
    int8_t padding[84];
    TitleCurrentScreen previousScreen;
    BOOL advanceToNextMenu;
    char replayFilePaths[60][512];
    char replayNumbers[60][8];
    ReplayData replays[60];
    ReplayData* currentReplay;
    int32_t unk_b060;
    int32_t replayCount;
    int32_t selectedReplay;
    int32_t selectedReplayStage;
    int32_t optionIdleFrames;
    AnmVM* vms;
    AnmVM* currentHelpTextVM;
    AnmVM helperTextVMs[14];
    int32_t vmCount;
    TitleCurrentScreen currentScreen;
    int32_t framesSpentInMenu;
    int32_t demoPlayTimer;
    ChainElem* calcChain;
    ChainElem* drawChain;
    ControllerMapping controllerConfig;
    uint8_t field26_0xd11e;
    uint8_t field27_0xd11f;
    Config cfg;
};
static_assert(offsetof(TitleScreen, replayFilePaths) == 0x6c);
static_assert(sizeof(((TitleScreen*)(0))->replayFilePaths[0]) == 0x200);
static_assert(offsetof(TitleScreen, selectedReplay) == 0xb0b8);

using D3DXVECTOR3 = Float3;
using D3DXVECTOR2 = Float2;
using ZunVec2 = Float2;
using ZunVec3 = Float3;
using i8 = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using f32 = float;
using undefined = uint8_t;

struct EclRawInstr {
    i32 time;
    i16 opcode;
    i16 offsetToNext;
    u8 unk_8;
    u8 skipForDifficulty;
    u16 paramMask;
};

struct EnemyFloatInterp {
    undefined field0_0x0_to_field47_0x2f[0x30];
};

struct EclContextVars {
    i32 intVars[4];
    f32 floatVars[8];
    i32 counterVars[4];
    f32 floatVars2[2];
    i32 intParams[4];
    f32 floatParams[4];
};

struct EclContext {
    EclRawInstr* currentInstr;
    ZunTimer time;
    void* funcSetFunc;
    void* funcSetInstr;
    EclContextVars vars;
    ZunTimer waitTimer;
    EnemyFloatInterp floatInterpolators[8];
    i32 compareRegister;
    i32 asyncID;
    u16 subID;
    undefined field10_0x216;
    undefined field11_0x217;
};

struct BulletEx {
    f32 field0_0x0;
    f32 field1_0x4;
    i32 field2_0x8;
    i32 field3_0xc;
    i32 field4_0x10;
    i32 field5_0x14;
};

struct BulletTypeSprites {
    AnmVM spriteBullet;
    AnmVM spriteSpawnEffectFast;
    AnmVM spriteSpawnEffectNormal;
    AnmVM spriteSpawnEffectSlow;
    AnmVM spriteSpawnEffectDonut;
    D3DXVECTOR3 hitboxSize;
    u8 unk_b88;
    u8 bulletHeight;
    undefined field8_0xb8a;
    undefined field9_0xb8b;
};

struct EnemyBulletShooter {
    i16 sprite;
    i16 color;
    D3DXVECTOR3 position;
    f32 angle1;
    f32 angle2;
    f32 speed1;
    f32 speed2;
    BulletEx bulletEx[5];
    f32 laserStartOffset;
    f32 laserEndOffset;
    f32 laserLength;
    f32 laserWidth;
    i32 laserStartTime;
    i32 laserDuration;
    i32 laserStopTime;
    i32 laserStartHitboxTime;
    i32 laserStopHitboxTime;
    i16 count1;
    i16 count2;
    u16 aimMode;
    u16 unk_c2;
    u32 flags;
    i32 fireSfx;
    i32 exSfx;
    BulletTypeSprites* spriteTemplate;
};

enum LaserState : uint8_t {
    LASER_STATE_SPAWNING = 0,
    LASER_STATE_ACTIVE = 1,
    LASER_STATE_DESPAWNING = 2
};

struct Laser {
    AnmVM vm0;
    AnmVM vm1;
    D3DXVECTOR3 pos;
    float angle;
    float startOffset;
    float endOffset;
    float length;
    float width;
    float width2;
    float speed;
    i32 startTime;
    i32 startHitboxTime;
    i32 duration;
    i32 stopTime;
    i32 stopHitboxTime;
    BOOL inUse;
    ZunTimer time;
    u16 flags;
    i16 color;
    LaserState state;
    undefined field20_0x4e9;
    undefined field21_0x4ea;
    undefined field22_0x4eb;
};

struct EnemyFlags {
    u8 _1;
    u8 _2;
    u8 _3;
    u8 _4;
};

struct VertexTex1DiffuseXyzrwh {
    D3DXVECTOR3 position;
    f32 w;
    ZunColor diffuse;
    Float2 textureUV;
};

struct Effect {
    struct AnmVM vm;
    D3DXVECTOR3 pos;
    D3DXVECTOR3 arg;
    D3DXVECTOR3 velocity;
    D3DXVECTOR3 acceleration;
    D3DXVECTOR3 vecE;
    D3DXVECTOR3 posInterpInitial;
    D3DXVECTOR3 posInterpFinal;
    Float4 quaternion;
    f32 bossSquaresDistance;
    f32 bossSquaresAngle;
    ZunTimer timer;
    undefined field12_0x2c4;
    undefined field13_0x2c5;
    undefined field14_0x2c6;
    undefined field15_0x2c7;
    i32 (*updateCallback)(Effect* );
    i8 inUseFlag;
    i8 effectID;
    i8 isBossSquares;
    i8 bossSquaresFadeInCounter;
    u8 renderType;
    undefined field22_0x2d1;
    undefined field23_0x2d2;
    undefined field24_0x2d3;
    Effect* nextInList;
};

struct EnemyTrail {
    D3DXVECTOR3 position;
    D3DXVECTOR3 velocity;
    float movementAngle;
};

struct Enemy {
    AnmVM primaryVM;
    AnmVM vms[2];
    EclContext currentContext;
    EclContext savedContextStack[16];
    i32 stackDepth;
    i32 unk_2a80;
    i32 deathCallbackSub;
    i32 interrupts[32];
    i32 runInterrupt;
    D3DXVECTOR3 position;
    D3DXVECTOR3 velocity;
    D3DXVECTOR3 prevPosition;
    D3DXVECTOR3 movementDeltaLastFrame;
    D3DXVECTOR3 hitboxWidth;
    D3DXVECTOR3 lowDamageHitboxWidth;
    f32 angle;
    f32 angularVelocity;
    f32 orbitAngle;
    f32 orbitAngularVelocity;
    f32 speed;
    f32 acceleration;
    f32 orbitRadius;
    f32 orbitRadialVelocity;
    D3DXVECTOR3 shootOffset;
    D3DXVECTOR3 moveInterp;
    D3DXVECTOR3 moveInterpStartPos;
    ZunTimer moveInterpTimer;
    i32 moveInterpStartTime;
    float bulletRankSpeedLow;
    float bulletRankSpeedHigh;
    i16 bulletRankAmount1Low;
    i16 bulletRankAmount1High;
    i16 bulletRankAmount2Low;
    i16 bulletRankAmount2High;
    int life;
    int maxLife;
    i32 score;
    ZunTimer bossTimer;
    ZunColor color;
    EnemyBulletShooter bulletProps;
    int shootInterval;
    ZunTimer shootIntervalTimer;
    EnemyBulletShooter laserProps;
    Laser* lasers[32];
    i32 laserStore;
    i32 itemDrop;
    u8 deathAnm1;
    u8 deathAnm2;
    u8 deathAnm3;
    u8 bossID;
    u8 damageTimer;
    undefined field51_0x2e19;
    undefined field52_0x2e1a;
    undefined field53_0x2e1b;
    ZunTimer unused_2e1c;
    EnemyFlags flags;
    u16 unk_2e2c;
    u8 anmExFlags;
    u8 unk_2e2f;
    i16 anmExDefaults;
    i16 anmExFarLeft;
    i16 anmExFarRight;
    i16 anmExLeft;
    i16 anmExRight;
    undefined field64_0x2e3a;
    undefined field65_0x2e3b;
    D3DXVECTOR2 lowerMoveLimit;
    D3DXVECTOR2 upperMoveLimit;
    i32 damageTakenLastFrame;
    Effect* effectArray[24];
    Effect* spellcardEffect;
    i32 effectIdx;
    f32 effectDistance;
    i32 lifeCallbackThresholds[4];
    i32 lifeCallbackSubs[4];
    i32 timerCallbackThreshold;
    i32 timerCallbackSub;
    i32 callRepeatSub;
    EclContextVars callRepeatContext;
    ZunTimer callRepeatTimer;
    ZunTimer timer_2f5c;
    f32 unused_2f68;
    ZunTimer unused_2f6c;
    EnemyTrail trail[96];
    VertexTex1DiffuseXyzrwh trailVertices[194];
    u8 trailFlags;
    undefined field86_0x4f31;
    i16 trailDuration;
    i16 trailLength;
    i16 trailInterval;
    ZunTimer armorTimer;
    Enemy* nextInList;
};
static_assert(offsetof(Enemy, currentContext.time.current) == 0x6f0);

struct RunningSpellcardInfo {
    BOOL isCapturing;
    BOOL isActive;
    i32 captureScore;
    i32 grazeScoreBonus;
    i32 captureScoreDecrement;
    i32 idx;
    BOOL prebombProtectFlag;
    ZunTimer timer;
};

struct EclTimelineInstr {
    i16 time;
    i16 arg0;
    i16 opcode;
    i16 size;
};

struct EclTimeline {
    ZunTimer timer;
    EclTimelineInstr* instr;
};
static_assert(offsetof(EclTimeline, timer) == 0);
static_assert(sizeof(EclTimeline) == 0x10);

struct EnemyManager {
    char* stgEnmAnmFilename;
    char* stgEnm2AnmFilename;
    Enemy enemyTemplate;
    Enemy enemies[481];
    Enemy* bosses[8];
    uint16_t randomItemSpawnIndex;
    uint16_t randomItemTableIndex;
    int32_t enemyCount;
    int32_t unk_9545c0;
    RunningSpellcardInfo spellcardInfo;
    int32_t unk_9545ec;
    int32_t unk_9545f0;
    EclTimeline timelines[16];
    ZunTimer timer;
    Enemy* enemyListHeads[4];
};
static_assert(offsetof(EnemyManager, bosses) == 0x954598);
static_assert(offsetof(EnemyManager, timelines[0].timer.current) == 0x9545fc);

struct RenderVertexInfo {
    Float3 position;
    Float2 vertexUV;
};

struct AnmRawEntry {
    i32 numSprites;
    i32 numScripts;
    u32 textureIdx;
    i32 width;
    i32 height;
    u32 format;
    D3DCOLOR colorKey;
    u32 nameOffset;
    u32 spriteIdxOffset;
    u32 alphaNameOffset;
    u32 version;
    u32 memoryPriority;
    u32 textureOffset;
    u8 hasData;
    u8 isFirstEntry;
    undefined field15_0x36;
    undefined field16_0x37;
    u32 nextOffset;
    u32 unk2;
};

struct AnmEntry {
    AnmRawEntry* rawData;
    i32 spriteIdxOffset;
    i32 numEntries;
};

struct AnmManager {
    ZunColor color;
    BOOL useMixColor;
    i32 scriptsExecutedThisFrame;
    i32 scriptsStillRunningThisFrame;
    i32 renderStateChangesThisFrame;
    i32 flushesThisFrame;
    Float2 screenShakeOffset;
    Matrix44 worldMatrix;
    AnmLoadedSprite sprites[2560];
    AnmVM virtualMachine;
    LPDIRECT3DTEXTURE8 textures[264];
    void* imageDataArray[256];
    char* textureFileNames[264];
    i32 maybeLoadedSpriteCount;
    AnmRawInstr* scripts[2560];
    int spriteIndices[2560];
    AnmEntry anmFiles[50];
    LPDIRECT3DSURFACE8 surfaces[32];
    LPDIRECT3DSURFACE8 surfacesBis[32];
    uint8_t surfaceInfo[20][32];  // real type: D3DXIMAGE_INFO[32]
    D3DCOLOR currentTextureFactor;
    LPDIRECT3DTEXTURE8 currentTexture;
    u8 currentBlendMode;
    u8 currentColorOp;
    u8 currentVertexShader;
    u8 disableZWrite;
    u8 cameraMode;
    undefined field27_0x2e4d5;
    undefined field28_0x2e4d6;
    undefined field29_0x2e4d7;
    struct AnmLoadedSprite* currentSprite;
    LPDIRECT3DVERTEXBUFFER8 quadVertexBuffer;
    RenderVertexInfo untexturedQuad[4];
    i32 spritesToDraw;
    VertexTex1DiffuseXyzrwh vertexBuffer[49152];
    VertexTex1DiffuseXyzrwh* vertexBufferEndPtr;
    VertexTex1DiffuseXyzrwh* vertexBufferStartPtr;
    i32 captureAnmIdx;
    i32 textureCaptureSrcX;
    i32 textureCaptureSrcY;
    i32 textureCaptureSrcW;
    i32 textureCaptureSrcH;
    i32 textureCaptureDstX;
    i32 textureCaptureDstY;
    i32 textureCaptureDstW;
    i32 textureCaptureDstH;
};
static_assert(offsetof(AnmManager, anmFiles[5].rawData) == 0x2df2c);

struct StdCameraSky {
    f32 nearPlane;
    f32 farPlane;
    D3DCOLOR color;
};

struct StdCameraData {
    D3DXVECTOR3 position;
    D3DXVECTOR3 facing;
    D3DXVECTOR3 facingUp;
    D3DXVECTOR3 unk_24;
    D3DXVECTOR3 facingNormalized;
    f32 fov;
};

enum SpellcardState {
    NOT_RUNNING = 0,
    RUNNING = 1,
    RAN_FOR_60_FRAMES = 2
};

struct Background {
    AnmVM *quadVMs;
    AnmVM vm0;
    AnmVM vm1;
    struct StdRawHeader *stdData;
    i32 quadCount;
    i32 objectCount;
    struct StdRawObject **objects;
    struct StdRawObjectInstance *objectInstances;
    struct StdRawInstruction *beginningOfScript;
    ZunTimer scriptTime;
    i32 instructionIndex;
    i32 timer;
    u32 stage;
    D3DXVECTOR3 position;
    ZunColor clearColor;
    StdCameraSky skyFog;
    StdCameraSky skyFogInterpInitial;
    StdCameraSky skyFogInterpFinal;
    i32 skyFogInterpDuration;
    ZunTimer skyFogInterpTimer;
    i8 skyFogNeedsSetup;
    undefined field21_0x511;
    undefined field22_0x512;
    undefined field23_0x513;
    SpellcardState spellcardState;
    i32 ticksSinceSpellcardStarted;
    BOOL maybePendingFlush;
    i32 numSpellcardBackgroundVMs;
    i32 baseSpellcardBackgroundScriptIndex;
    AnmVM spellcardBackground[32];
    AnmVM unk_4ea8;
    i32 pendingInterrupt;
    StdCameraData cameraInterpFinal;
    StdCameraData cameraInterpInitial;
    StdCameraData cameraInterpFinalDeriv;
    StdCameraData cameraInterpInitialDeriv;
    StdCameraData camera;
    i32 cameraInterpDurations[4];
    ZunTimer cameraInterpTimers[4];
    i32 cameraInterpModes[4];
    D3DXVECTOR3 nextIns0Pos;
    i32 nextIns0Time;
    D3DXVECTOR3 prevIns0Pos;
    i32 prevIns0Time;
    bool jumpFlag;
    undefined field45_0x52a9;
    undefined field46_0x52aa;
    undefined field47_0x52ab;
    ZunColor tintColor;
    BOOL useTintColor;
};

struct BulletExState {
    ZunTimer timer;
    f32 float1;
    f32 float2;
    D3DXVECTOR3 vec3;
    i32 int1;
    i32 int2;
    i32 int3;
};

enum BulletState : uint16_t {
    BULLET_STATE_INACTIVE = 0,
    BULLET_STATE_ACTIVE = 1,
    BULLET_STATE_SPAWNING_SLOW = 2,
    BULLET_STATE_SPAWNING_NORMAL = 3,
    BULLET_STATE_SPAWNING_FAST = 4,
    BULLET_STATE_DESPAWNING = 5,
    __BULLET_STATE_LAST_SLOT = 6
} ;

struct Bullet {
    BulletTypeSprites sprites;
    D3DXVECTOR3 pos;
    D3DXVECTOR3 velocity;
    D3DXVECTOR3 ex4Acceleration; // leftover from EoSD
    f32 speed;
    f32 ex5Float0; // leftover from EoSD
    f32 dirChangeSpeed; // leftover from EoSD
    f32 angle;
    f32 ex5Float1; // leftover from EoSD
    f32 dirChangeRotation; // leftover from EoSD
    ZunTimer timeSinceBulletFired;
    ZunTimer timeActive;
    i32 ex5Int0; // leftover from EoSD
    i32 dirChangeInterval; // leftover from EoSD
    i32 dirChangeNumTimes; // leftover from EoSD
    i32 dirChangeMaxTimes; // leftover from EoSD
    i32 despawnProtectionFrames;
    u16 exFlags;
    u16 flags;
    i16 color;
    u16 unk_bfa;
    BulletState state;
    u16 despawnCounter;
    bool unk_c00; // seems to always be 1
    bool isGrazed;
    undefined field25_0xc02;
    undefined field26_0xc03;
    Bullet *nextInLayer;
    i32 youmuSpell2State;
    i32 transformSfx;
    i32 currentExIndex;
    BulletEx ex[5];
    BulletExState exState[5];
};

enum ItemType : uint8_t {
    ITEM_POWER_SMALL = 0,
    ITEM_POINT = 1,
    ITEM_POWER_BIG = 2,
    ITEM_BOMB = 3,
    ITEM_FULL_POWER = 4,
    ITEM_LIFE = 5,
    ITEM_STAR = 6,
    ITEM_CHERRY = 7,
    ITEM_CHERRY_PETAL = 8,
    ITEM_CHERRY_BULLET = 9
};

struct BulletManager {
    BulletTypeSprites bulletTypeTemplates[16];
    Bullet bullets[1025];
    Laser lasers[64];
    int bulletCount;
    int cancelFramesRemaining;
    ZunTimer timer;
    int unkCounter;
    char *bulletAnmFilename;
    Bullet *layerListHeads[6];
    Bullet *nextBulletSlot;
    i32 bonusItemType;
};
static_assert(offsetof(BulletManager, bonusItemType) == 0x37a160);

struct BombDamageRegion {
    Float3 pos;
    Float2 rectWidth;
    float radiusIncreasePerTick;
    int dmgPerTick;
    int totalDamageDealt;
};

struct BombCancelRegion {
    Float2 pos;
    Float2 rectWidth;
    float radius;
    float radiusIncreasePerTick;
    int duration;
    ItemType itemType;
    undefined field6_0x1d;
    undefined field7_0x1e;
    undefined field8_0x1f;
};

enum PlayerState : uint8_t {
    PLAYER_STATE_ALIVE = 0,
    PLAYER_STATE_RESPAWNING = 1,
    PLAYER_STATE_DEAD = 2,
    PLAYER_STATE_INVULNERABLE = 3,
    PLAYER_STATE_BORDER = 4
};

enum BorderState : uint8_t {
    BORDER_STATE_INACTIVE = 0,
    BORDER_STATE_ACTIVE = 1,
    BORDER_STATE_PENDING = 2
};

enum PlayerBulletType : uint16_t {
    PLAYER_BULLET_STANDARD = 0,
    PLAYER_BULLET_HOMING_UNFOCUSED = 1,
    PLAYER_BULLET_HOMING_FOCUSED = 2,
    PLAYER_BULLET_MISSILE = 3,
    PLAYER_BULLET_LASER_UNFOCUSED = 4,
    PLAYER_BULLET_LASER_FOCUSED = 5
};

struct PlayerBullet {
    AnmVM sprite;
    D3DXVECTOR3 pos;
    ZunVec3 trailData[16];
    D3DXVECTOR3 hitboxSize;
    ZunVec2 velocity;
    ZunVec2 spawnOffset;
    float speed;
    float angle;
    ZunTimer timer;
    i16 damage;
    i16 bulletState;
    PlayerBulletType bulletType;
    i16 initialFireDelay;
    i16 spawnPositionIdx;
    i16 fireRate;
    void *OnUpdate;
    void *OnDraw;
    void *OnHit;
    struct ShtPowerBulletData *shtData;
};

struct PlayerLaser {
    ZunTimer timer;
    PlayerBullet *laser;
};


struct PlayerBombArea {
    i32 state;
    i32 count;
    f32 speedOrAngle;
    f32 speed2;
    f32 angle2;
    D3DXVECTOR3 pos;
    D3DXVECTOR3 posArray[32];
    D3DXVECTOR3 velocity;
    D3DXVECTOR3 acceleration;
    struct AnmVM sprites[8];
    struct Effect *effect;
    struct ZunTimer timer;
};

struct PlayerBomb {
    int isActive;
    int isFocused;
    int duration;
    int cherryLossPerFrame;
    ZunTimer timer;
    void *calcUnfocused;
    void *drawUnfocused;
    void *calcFocused;
    void *drawFocused;
    PlayerBombArea bombAreas[128];
};

struct Player {
    AnmVM sprite;
    AnmVM orbsSprite[3];
    Float3 pos;
    D3DXVECTOR3 unk_93c;
    D3DXVECTOR3 hitboxTopLeft;
    D3DXVECTOR3 hitboxBottomRight;
    D3DXVECTOR3 grazeboxTopLeft;
    D3DXVECTOR3 grazeboxBottomRight;
    D3DXVECTOR3 grabItemTopLeft;
    D3DXVECTOR3 grabItemBottomRight;
    D3DXVECTOR3 hitboxHalfWidth;
    D3DXVECTOR3 grazeboxHalfWidth;
    D3DXVECTOR3 grabItemHalfWidth;
    D3DXVECTOR3 orbsPosition[2];
    D3DXVECTOR3 velocity;
    Effect *focusEffect;
    BombDamageRegion bombDamageRegions[112];
    BombCancelRegion bombCancelRegions[96];
    BOOL starItemScorePenalty;
    struct ShtPowerBulletData *laserPowerData[4];
    f32 horizonalMovementSpeedMultiplierDuringBomb;
    f32 verticalMovementSpeedMultiplierDuringBomb;
    i32 remainingDeathbombTicks;
    i32 postBorderInvincibilityTicks;
    i32 bulletGracePeriod;
    i32 bombCancelItem;
    PlayerState playerState;
    u8 unk_2409;
    i8 orbState;
    i8 isFocus;
    u8 damageParticleTimer;
    BorderState borderState;
    undefined field32_0x240e;
    undefined field33_0x240f;
    ZunTimer focusMovementTimer;
    i32 playerDirection;
    f32 previousHorizontalSpeed;
    f32 previousVerticalSpeed;
    D3DXVECTOR3 reimuATargetPos;
    D3DXVECTOR3 sakuyaATargetPos;
    BOOL isLockedOntoEnemy;
    PlayerBullet bullets[96];
    PlayerLaser lasers[3];
    ZunTimer fireBulletTimer;
    ZunTimer invulnerabilityTimer;
    ZunTimer borderTimer;
    void *fireBulletCallback; // leftover from EoSD
    void *fireBulletFocusCallback; // leftover from EoSD
    PlayerBomb bomb;
    D3DXVECTOR3 posForBomb;
    f32 sakuyaBAimDirection;
    ChainElem *calcChain;
    ChainElem *drawChainHighPrio;
    ChainElem *drawChainLowPrio;
    Effect *bombIFramesEffect;
    Effect *borderEffect;
    void *shtFile;
    void *bombShtFile;
};
static_assert(offsetof(Player, playerState) == 0x2408);


struct EclManagerUnkStruct {
    undefined field0_0x0_to_field255_0xff[0x100];
    D3DXVECTOR3 field256_0x100[8];
};

struct EclManager {
    uint32_t eclFile;
    EclRawInstr** subTable;
    EclManagerUnkStruct unk;
    i32 argA;
    i32 argB;
    i32 argC;
    i32 argD;
    f32 argR;
    f32 argS;
    f32 argM;
    f32 argN;
};

struct GameWindow {
    HWND window;
    BOOL windowIsClosing;
    BOOL windowIsActive;
    BOOL windowIsInactive;
    i8 framesSinceRedraw;
    undefined field5_0x11;
    undefined field6_0x12;
    undefined field7_0x13;
    uint64_t pcFrequency;
    bool usesRelativePath;
    undefined field10_0x1d;
    undefined field11_0x1e;
    undefined field12_0x1f;
    BOOL screenSaveActive;
    BOOL lowPowerActive;
    BOOL powerOffActive;
};
}
}