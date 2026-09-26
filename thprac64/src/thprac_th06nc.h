#pragma once
#include <thprac_games.h>
#include <thprac_utils.h>
#include <d3d11.h>

namespace TH06NC {
    enum VERSION {
        VER_1_03A,
        VER_1_03B,
        __VER_CNT,
    };
    VERSION gameVersion;


//-------------------------------------------------------------------------
// Hook RVAs --------------------------------------------------------------

#define HOOK_LIST(X)                          \
    /*                           V1_03A      V1_03B */ \
    X(ECL_RETRIEVE_SHOT_ID,      0x23a1d,    0x238cd)  \
    X(FUNCSET_PATCHY_GET_LAST3,  0x333d0,    0x34cf0)  \
    X(FUNCSET_QED_GET_HEALTH,    0x35864,    0x37184)  \
    X(FUNCSET_DH_GET_HEALTH,     0x35a4a,    0x3736a)  \
    X(ECL_TIMELINE_RET,          0x36f12,    0x38832)  \
    X(ENEMY_MGR_TICK_BOSS_TIME,  0x3816f,    0x39a8f)  \
    X(ENEMY_MGR_TICK_TIMELINE,   0x381f9,    0x39b19)  \
    X(PAUSE_MENU_BGM_PAUSE,      0x3a313,    0x3bba3)  \
    X(PRAC_HIGH_SCORE_READ,      0x3ae66,    0x3c6e0)  \
    X(GAME_MGR_REG_BG_FF_CHECK,  0x3b4b5,    0x3cd35)  \
    X(GAME_MGR_REG_BGM_PICK,     0x3b611,    0x3ce91)  \
    X(GAME_MGR_REG_PLAY_BGM,     0x3b61b,    0x3ce9b)  \
    X(GAME_MANAGER_REGISTERED,   0x3b69d,    0x3cf1d)  \
    X(GAME_MGR_END_PAUSE_BGM,    0x3b6cb,    0x3cf4b)  \
    X(POST_ON_TICK,              0x3bf59,    0x3d7d9)  \
    X(POST_ON_DRAW,              0x3c257,    0x1f28)   \
    X(BGM_NAME_GET_TRANSLATIONS, 0x3e655,    0x3ff35)  \
    X(BGM_NAME_LOAD,             0x3e74a,    0x4002a)  \
    X(MENU_SET_TITLE_STATE,      0x4802e,    0x4978e)  \
    X(MENU_SET_PRAC_STATE,       0x4bbd5,    0x4d335)  \
    X(MENU_SET_PRAC_STATE_2,     0x4bc46,    0x4d3a6)  \
    X(MENU_CANCEL_CONFIRM_STATE, 0x4c2ba,    0x4da1a)  \
    X(MENU_SET_CONFIRMED_STATE,  0x4c474,    0x4dbd4)  \
    X(MENU_LOAD_DIFF_CHECK,      0x4b062,    0x4c7c2)  \
    X(GUI_INIT_MAIN,             0x4d0e5,    0x4e845)  \
    X(BOMB_INPUT_CHECK,          0x689fa,    0x6a10a)  \
    X(BOMB_CNT_DECREASE,         0x68a7b,    0x6a18b)  \
    X(POWER_DECREASE,            0x68baf,    0x6a2bf)  \
    X(POWER_MIN_CHALLENGE_MODE,  0x68bbe,    0x6a2ce)  \
    X(LIFE_CNT_DECREASE,         0x68e88,    0x6a59f)  \
    X(BULLET_HIT_BIG_PARTICLE,   0x6aacf,    0x6c1df)  \
    X(BULLET_HIT_SET_PLY_STATE,  0x6aaf7,    0x6c207)  \
    X(LASER_HIT_BIG_PARTICLE,    0x6ad18,    0x6c428)  \
    X(LASER_HIT_SET_PLY_STATE,   0x6ad44,    0x6c454)  \
    X(PRAC_HIGH_SCORE_WRITE,     0x74440,    0x75b0f)  \
    X(POST_DIMENSION_CHANGE,     0x7c848,    0x7e498)  \
    X(GUI_INIT_LAUNCH,           0x270725,   0x2724d5) \

#define HK_ENUM(name, ...) name,
    enum HOOKS { HOOK_LIST(HK_ENUM) };
#undef HK_ENUM
#define HK_DATA(name, ...) { __VA_ARGS__ },
    static const uintptr_t hook_data[][__VER_CNT] = { HOOK_LIST(HK_DATA) };
#undef HK_DATA

    uintptr_t GetHookAddr(uintptr_t key) {
        return hook_data[key][gameVersion];
    }

    void SetupHook(HookCtx& hook) {
        hook.addr = GetHookAddr(hook.addr);
        hook.Setup();
    }

    void EnableAllHooksVersion(HookCtx* hooks, size_t num) {
        for (size_t i = 0; i < num; i++) {
            HookCtx& hook = hooks[i];
            SetupHook(hook);
            hook.Enable();
        }
    }

#define EnableAllHooksVersion(hooks) \
    EnableAllHooksVersion(TH06NC::hooks, elementsof(TH06NC::hooks))
#undef HOOK_LIST

//-------------------------------------------------------------------------
// Instruction Offsets (used by hooks) ------------------------------------

#define INST_LIST(X)                                      \
    /*                               V1_03A     V1_03B */ \
    X(MENU_SET_SUBSHOT_SEL_STATE,    0x4be03,   0x4d563)  \
    X(GAME_MGR_REG_BG_FF_CHECK_FAIL, 0x3b4bc,   0x3cd3c)  \
    X(BOMB_INPUT_CHECK_PASS,         0x68a11,   0x6a121)  \
    X(BOMB_INPUT_CHECK_FAIL,         0x68ad2,   0x6a1e2)  \

#define IS_VAR(name, ...) uintptr_t name;
    INST_LIST(IS_VAR)
#define IS_ENUM(name, ...) name##_ID,
        enum INSTS { INST_LIST(IS_ENUM) };
#undef IS_ENUM
#define IS_DATA(name, ...) { __VA_ARGS__ },
        static const uintptr_t inst_data[][__VER_CNT] = { INST_LIST(IS_DATA) };
#undef IS_DATA

    void InitInstructions(VERSION ver) {
    #define IS_INIT(name, ...) name = RVA(inst_data[name##_ID][ver]);
        INST_LIST(IS_INIT)
    #undef IS_INIT
    }
#undef INST_LIST


//-------------------------------------------------------------------------
// Global RVAs ------------------------------------------------------------

#define ADDR_LIST(X)                                    \
    /*                             V1_03A     V1_03B */ \
    X(PATCHY_LAST_SPELLS_TABLE,    0x3de300,  0x428f50) \
    X(GAME_MANAGER_ADDR,           0x4f1e60,  0x53cab0) \
    X(PLAYER_ADDR,                 0x4ff3a0,  0x549ff0) \
    X(STAGE_BACKGROUND_ADDR,       0x509b60,  0x5547b0) \
    X(WINDOW_WIDTH,                0xc21e44,  0xc6e064) \
    X(WINDOW_HEIGHT,               0xc21e48,  0xc6e068) \
    X(BGM_ADDR,                    0x50966c,  0x5542bc) \
    X(HWND_PTR,                    0x55e5a8,  0x5a9548) \
    X(D3D_DEVICE_PTR,              0x9b1cf8,  0x9fcc98) \
    X(D3D_DEVICE_CONTEXT,          0x9b1d00,  0x9fcca0) \
    X(ANM_MANAGER_PTR_ADDR,        0xa6e9b0,  0xabac30) \
    X(ECL_MANAGER_ADDR,            0xa6eb78,  0xabad98) \
    X(ECL_PLAYER_SHOT,             0xa6ebbc,  0xabadc0) \
    X(ZUN_GUI_ADDR,                0xa6ebd0,  0xabadf0) \
    X(EIGTH_FRAME_INPUT_HELD_ADDR, 0xa6ec48,  0xabae68) \
    X(INPUT_ADDR,                  0xa6ec60,  0xabae80) \
    X(INPUT_PREV_ADDR,             0xa6ec64,  0xabae84) \
    X(ENEMY_MANAGER_ADDR,          0xaa1e90,  0xaee0b0) \
    X(BOSS_PTR_ADDR,               0xbadf78,  0xbfa198) \
    X(MAIN_MENU_ADDR,              0xc07240,  0xc53460) \
    X(SUPERVISOR_ADDR,             0xc21970,  0xc6db90) \

#define AD_VAR(name, ...) uintptr_t name;
    ADDR_LIST(AD_VAR)
#undef AD_VAR
#define AD_ENUM(name, ...) name##_ID,
        enum ADDRS { ADDR_LIST(AD_ENUM) };
#undef AD_ENUM
#define AD_DATA(name, ...) { __VA_ARGS__ },
    static const uintptr_t addr_data[][__VER_CNT] = { ADDR_LIST(AD_DATA) };
#undef AD_DATA

    void InitGlobals(VERSION ver) {
    #define AD_INIT(name, ...) name = RVA(addr_data[name##_ID][ver]);
        ADDR_LIST(AD_INIT)
    #undef AD_INIT
    }
#undef ADDR_LIST


//-------------------------------------------------------------------------
// Game Functions ---------------------------------------------------------

#define FUNC_LIST(X)                          \
    /*                   V1_03A     V1_03B */ \
    X(LOAD_ANM_FILE,     0x20b0,    0x2440)   \
    X(ANM_VM_SET_SPRITE, 0x2980,    0x2d60)   \
    X(BGM_PLAY,          0x7bc80,   0x7d8d0)  \
    X(BGM_STOP,          0x7f9c0,   0x81740)  \
    X(BGM_RESUME,        0xc8110,   0xc9df0)  \
    X(BGM_PAUSE,         0xcce50,   0xceb30)  \

    struct Function {
        uintptr_t addr;

        template <typename R = void, typename... Args>
        R operator()(Args... args) const {
            auto* func = (R(*)(Args...))addr;
            return func(args...);
        }
    };

#define FN_VAR(name, ...) Function name;
    FUNC_LIST(FN_VAR)
#undef FN_VAR
#define FN_ENUM(name, ...) name##_ID,
        enum FUNCS { FUNC_LIST(FN_ENUM) };
#undef FN_ENUM
#define FN_DATA(name, ...) { __VA_ARGS__ },
    static const uintptr_t func_data[][__VER_CNT] = { FUNC_LIST(FN_DATA) };
#undef FN_DATA

    void InitFuncs(VERSION ver) {
    #define FN_INIT(name, ...) name.addr = RVA(func_data[name##_ID][ver]);
        FUNC_LIST(FN_INIT)
    #undef FN_INIT
    }
#undef FUNC_LIST


//-------------------------------------------------------------------------
// Initialization ---------------------------------------------------------

    void SetupGameVersion(VERSION ver) {
        gameVersion = ver;
        InitInstructions(ver);
        InitGlobals(ver);
        InitFuncs(ver);
    }


//-------------------------------------------------------------------------
// Utils ------------------------------------------------------------------

    struct ECL_OP {
        int16_t code;
        int16_t size;
    };

    constexpr ECL_OP NOP = { 0, 0xc };
    constexpr ECL_OP CALL = { 35, 0x18 };
    constexpr ECL_OP TIMER_THRESHOLD = { 115, 0x10 };
    constexpr ECL_OP TIMER_CALLBACK = { 116, 0x10 };

    enum ANM_IDS {
        BACKGROUND = 11,
        STAGE_ILLUST_MAIN = 62,
        STAGE_ILLUST_TLB = 63,
    };

    enum RANK_VALS {
        EASY_RANK = 20,
        NHL_RANK = 32,
        EX_RANK = 18,
    };

    enum DIFFICULTY {
        EASY, NORMAL,
        HARD, LUNATIC,
        EXTRA
    };

    enum WARP_TYPE {
        NONE, CHAPTER,
        MIDBOSS, ENDBOSS, TLB,
        NONSPELL, SPELL,
        FRAME
    };

    constexpr WARP_TYPE fixType(int warpType) {
        return (WARP_TYPE)(warpType >= TLB ? warpType + 1 : warpType);
    }

    // used for ECL patching & midboss timelock
    constexpr uint32_t st1MidbossTime = 1882;
    constexpr uint32_t st2MidbossTime = 2498;
    constexpr uint32_t st4MidbossTime = 4058;
    constexpr uint32_t st5MidbossTime = 3272;

    int mChapterSetup[7][2]{
        { 4, 2 },
        { 2, 2 },
        { 4, 3 },
        { 4, 5 },
        { 3, 2 },
        { 2, 0 },
        { 4, 3 }
    };


//-------------------------------------------------------------------------
// Structs ----------------------------------------------------------------

    struct GameManager {
        char __unknown1[0x8];      // 0x0
        uint32_t spellCaps;        // 0x8
        uint32_t spellCapsForTLB;  // 0xc
        char __unknown2[0x10];     // 0x10
        uint8_t character;         // 0x20
        uint8_t subShot;           // 0x21
        uint32_t stage;            // 0x24
        uint16_t curPower;         // 0x28
        char __unknown3[0x906];    // 0x2a
        int64_t visualScore;       // 0x930
        int64_t actualScore;       // 0x938
        int64_t __unkScore;        // 0x940
        int64_t highScore;         // 0x948
        char __unknown4[0x4];      // 0x950
        int8_t inPracticeMode;     // 0x954
        int8_t inSpellPrac;        // 0x955
        char __unknown5[0x2];      // 0x956
        int8_t spellPracSpellNum;  // 0x958
        uint16_t stagePointItems;  // 0x95a
        uint16_t totalPointItems;  // 0x95c
        uint32_t difficulty;       // 0x960
        uint8_t mode;              // 0x964
        char __unknown6[0xc907];   // 0x965
        int32_t stageGraze;        // 0xd26c
        int32_t totalGraze;        // 0xd270
        char __unknown7[0x1c];     // 0xd274
        int8_t livesRemaining;     // 0xd290
        int8_t bombsRemaining;     // 0xd291
        uint16_t scoreExtends;     // 0xd292
        char __unknown8[0x188];    // 0xd294
        int32_t rank;              // 0xd41c
        // size unknown
    };

    static_assert(offsetof(GameManager, spellCaps) == 0x8);
    static_assert(offsetof(GameManager, spellCapsForTLB) == 0xc);
    static_assert(offsetof(GameManager, character) == 0x20);
    static_assert(offsetof(GameManager, subShot) == 0x21);
    static_assert(offsetof(GameManager, stage) == 0x24);
    static_assert(offsetof(GameManager, curPower) == 0x28);
    static_assert(offsetof(GameManager, visualScore) == 0x930);
    static_assert(offsetof(GameManager, actualScore) == 0x938);
    static_assert(offsetof(GameManager, highScore) == 0x948);
    static_assert(offsetof(GameManager, inPracticeMode) == 0x954);
    static_assert(offsetof(GameManager, inSpellPrac) == 0x955);
    static_assert(offsetof(GameManager, spellPracSpellNum) == 0x958);
    static_assert(offsetof(GameManager, stagePointItems) == 0x95a);
    static_assert(offsetof(GameManager, totalPointItems) == 0x95c);
    static_assert(offsetof(GameManager, difficulty) == 0x960);
    static_assert(offsetof(GameManager, mode) == 0x964);
    static_assert(offsetof(GameManager, stageGraze) == 0xd26c);
    static_assert(offsetof(GameManager, totalGraze) == 0xd270);
    static_assert(offsetof(GameManager, livesRemaining) == 0xd290);
    static_assert(offsetof(GameManager, bombsRemaining) == 0xd291);
    static_assert(offsetof(GameManager, scoreExtends) == 0xd292);
    static_assert(offsetof(GameManager, rank) == 0xd41c);

    struct Timer {
        int32_t prev;    // 0x0
        int32_t current; // 0x4
        // size unknown
    };

    struct EnemyManager {
        char __unknown1[0x10c0b8]; // 0x0
        Timer timelineTime;        // 0x10c0b8
        // size unknown
    };

    struct Enemy {
        Timer bossTimer;        // 0x0
        char __unknown1[0x38];  // 0x8
        Timer eclTimer;         // 0x40
        char __unknown2[0x1ec]; // 0x48
        int32_t curHealth;      // 0x234
        int32_t maxHealth;      // 0x238
        // size unknown
    };

    struct StageBackground {
        char __unknown1[0x68]; // 0x0
        uint8_t is_frozen;     // 0x68
        char __unknown2[0x47]; // 0x69
        Timer timelineTime;    // 0xb0
        // size unknown
    };

    struct MainMenu {
        char __unknown1[0x168b0]; // 0x0
        uint32_t curState; // 0x168b0
        char __unknown2[0x1d]; // 0x168b4
        uint8_t inPractice; // 0x168d1
    };

    struct ZUNGui {
        char __unknown1[0x38]; // 0x0
        uintptr_t stageLogo;   // 0x38
        char __unknown2[0x4];  // 0x40
        uint8_t isBossPresent; // 0x44
    };

    struct Supervisor {
        char __unknown1[0x42c]; // 0x0
        uint32_t curState;      // 0x42c
        char __unknown2[0x1b];  // 0x430
        uint8_t curMode;        // 0x44b
    };

    enum SUPERVISOR_STATES {
        RESTART_START = 0x0,
        MAIN_MENU_EXIT = 0x1,
        RUN_START = 0x2,
        STAGE_TRANSITION = 0x3,
        RUN_END_NO_ENDING = 0x7,
        REPLAY_MENU_EXIT = 0x8,
        RUN_END_ENDING = 0xa,
        RESTART_END = 0xc,
    };

    static_assert(offsetof(EnemyManager, timelineTime) == 0x10c0b8);
    static_assert(offsetof(Enemy, bossTimer) == 0x0);
    static_assert(offsetof(Enemy, eclTimer) == 0x40);
    static_assert(offsetof(Enemy, curHealth) == 0x234);
    static_assert(offsetof(Enemy, maxHealth) == 0x238);
    static_assert(offsetof(StageBackground, is_frozen) == 0x68);
    static_assert(offsetof(StageBackground, timelineTime) == 0xb0);
    static_assert(offsetof(MainMenu, curState) == 0x168b0);
    static_assert(offsetof(MainMenu, inPractice) == 0x168d1);
    static_assert(offsetof(ZUNGui, stageLogo) == 0x38);
    static_assert(offsetof(ZUNGui, isBossPresent) == 0x44);
    static_assert(offsetof(Supervisor, curState) == 0x42c);
    static_assert(offsetof(Supervisor, curMode) == 0x44b);

    struct Player {
        char __unknown1[0x7858]; // 0x0
        int32_t state_timer; // 0x7858
        char __unknown2[0x3c]; // 0x785c
        int8_t player_state; // 0x7898
        // size unknown
    };

    static_assert(offsetof(Player, state_timer) == 0x7858);
    static_assert(offsetof(Player, player_state) == 0x7898);


//-------------------------------------------------------------------------
// THPracParam ------------------------------------------------------------
    struct THPracParam {
        int32_t mode;
        int32_t gameMode;

        int32_t stage;
        int32_t section;
        int32_t phase;
        int32_t frame;

        int16_t power;
        int64_t score;
        int32_t graze;
        int32_t point;
        int8_t life;
        int8_t bomb;

        int32_t rank;
        int32_t fakeType;
        bool guaranteeTLB;
        bool dlg;

        void Reset()
        {
            mode = 0;
            stage = 0;
            section = 0;
            phase = 0;
            frame = 0;
            score = 0;
            life = 0;
            bomb = 0;
            power = 0;
            graze = 0;
            point = 0;
            rank = 0;
            fakeType = 0;
            guaranteeTLB = false;
            dlg = false;
        }

        bool ReadJson(std::string& json)
        {
            ParseJson();

            ForceJsonValue(game, "th06");
            GetJsonValue(mode);
            GetJsonValue(stage);
            GetJsonValue(section);
            GetJsonValue(phase);
            GetJsonValue(frame);
            GetJsonValue(score);
            GetJsonValue(life);
            GetJsonValue(bomb);
            GetJsonValue(power);
            GetJsonValue(graze);
            GetJsonValue(point);
            GetJsonValue(rank);
            GetJsonValue(fakeType);
            GetJsonValue(guaranteeTLB);
            GetJsonValue(dlg);

            return true;
        }

        std::string GetJson()
        {
            CreateJson();

            AddJsonVersion();
            AddJsonValueEx(game, "th06nc");
            AddJsonValue(mode);
            AddJsonValue(stage);
            if (section)
                AddJsonValue(section);
            if (phase)
                AddJsonValue(phase);
            if (frame)
                AddJsonValue(frame);
            if (guaranteeTLB)
                AddJsonValue(guaranteeTLB);
            if (dlg)
                AddJsonValue(dlg);

            AddJsonValue(score);
            AddJsonValue(life);
            AddJsonValue(bomb);
            AddJsonValue(power);
            AddJsonValue(graze);
            AddJsonValue(point);
            AddJsonValue(rank);
            AddJsonValue(fakeType);

            ReturnJson();
        }
    };
}