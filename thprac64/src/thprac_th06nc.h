#pragma once
#include <thprac_games.h>
#include <thprac_utils.h>
#include <d3d11.h>

namespace TH06NC {
    enum ADDRS {
        PATCHY_LAST_SPELLS_TABLE = 0x3de300,
        GAME_MANAGER_ADDR = 0x4f1e60,
        PLAYER_ADDR = 0x4ff3a0,
        STAGE_BACKGROUND_ADDR = 0x509b60,
        WINDOW_WIDTH = 0xc21e44,
        WINDOW_HEIGHT = 0xc21e44 + 0x4,
        HWND_PTR = 0x55e5a8,
        D3D_DEVICE_PTR = 0x9b1cf8,
        D3D_DEVICE_CONTEXT = 0x9b1d00,
        ANM_MANAGER_PTR_ADDR = 0xa6e9b0,
        ECL_MANAGER_ADDR = 0xa6eb78,
        ZUN_GUI_ADDR = 0xa6ebd0,
        INPUT_ADDR = 0xa6ec60,
        INPUT_PREV_ADDR = 0xa6ec64,
        IS_EIGTH_FRAME_OF_HELD_INPUT_ADDR = 0xa6ec48,
        ENEMY_MANAGER_ADDR = 0xaa1e90,
        BOSS_PTR_ADDR = 0xbadf78,
        MAIN_MENU_ADDR = 0xc07240,
    };

    enum FUNCS {
        LOAD_ANM_FILE = 0x20b0,
        ANM_VM_SET_SPRITE = 0x2980,
    };

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

    enum WARP_TYPE {
        NONE, CHAPTER,
        MIDBOSS, ENDBOSS, TLB,
        NONSPELL, SPELL,
        FRAME
    };

    constexpr WARP_TYPE fixType(int warpType) {
        return (WARP_TYPE)(warpType >= TLB ? warpType + 1 : warpType);
    }

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
        char __unknown4[0x5];      // 0x950
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
        char __unknown1[0x1ab7b]; // 0x0
        uint8_t selectedMode;     // 0x1ab7b
    };

    static_assert(offsetof(EnemyManager, timelineTime) == 0x10c0b8);
    static_assert(offsetof(Enemy, bossTimer) == 0x0);
    static_assert(offsetof(Enemy, eclTimer) == 0x40);
    static_assert(offsetof(Enemy, curHealth) == 0x234);
    static_assert(offsetof(Enemy, maxHealth) == 0x238);
    static_assert(offsetof(StageBackground, is_frozen) == 0x68);
    static_assert(offsetof(StageBackground, timelineTime) == 0xb0);
    static_assert(offsetof(MainMenu, selectedMode) == 0x1ab7b);

    struct Player {
        char __unknown1[0x7858]; // 0x0
        int32_t state_timer; // 0x7858
        char __unknown2[0x3c]; // 0x785c
        int8_t player_state; // 0x7898
        // size unknown
    };

    static_assert(offsetof(Player, state_timer) == 0x7858);
    static_assert(offsetof(Player, player_state) == 0x7898);

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

    int mChapterSetup[7][2]{
        { 4, 2 },
        { 2, 2 },
        { 4, 3 },
        { 4, 5 },
        { 3, 2 },
        { 2, 0 },
        { 4, 3 }
    };
}