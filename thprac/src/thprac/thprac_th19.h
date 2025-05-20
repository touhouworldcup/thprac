#pragma once
#include <stdint.h>
#include "thprac_games.h"

// Struct definitions from a shared Ghidra project
// -----------------------------------------------

namespace THPrac { namespace TH19 {

namespace V1_00a {
    struct GlobalsSide { /* Size should be 0xC0. DO NOT REMOVE FIELDS */
        uint64_t score;
        int32_t __int_8; /* idk, msg_related, when set to 2, unable to pause the game */
        int32_t shottype;
        int __dword_10;
        int __dword_14;
        int __dword_18;
        int __dword_1C;
        int gauge;
        int c1_threshold;
        int c2_threshold;
        int c3_threshold;
        int c4_threshold;
        int32_t c3_level; /* first level is 0 but shown as lvl1 in-game */
        int32_t c4_level; /* first level is 0 but shown as lvl1 in-game */
        int __dword_3C;
        int __dword_40; /* idk maybe AI/story mode related */
        int __dword_44;
        int __dword_48;
        int __dword_4C;
        int lives;
        int max_lives;
        int __dword_58;
        int __dword_5C;
        int __dword_60;
        int bombs;
        int __dword_68;
        int __dword_6C;
        int __dword_70;
        int __dword_74;
        int __dword_78;
        int __dword_7C;
        int __dword_80;
        int __dword_84;
        int __dword_88;
        int __dword_8C;
        int __dword_90;
        int __meter_rate;
        int __dword_98;
        int __dword_9C;
        int __dword_A0;
        int __dword_A4;
        int __dword_A8;
        int __dword_AC;
        int __dword_B0;
        int __dword_B4;
        int __dword_B8;
        int __dword_BC;
    };

    struct Globals {
        GlobalsSide side[2];
        int32_t difficulty;
        int32_t __counter_184;
        int __dword_188;
        int __dword_18C;
        int __rank_related_190;
        int __dword_194;
        int __rank_related_198;
        int __dword_19C;
        int __dword_1A0;
        int32_t __int_array_1A4[256];
        int32_t __int_array_5A4[256];
        int __dword_9A4;
        int __dword_9A8;
        int __dword_9AC;
        int story_stage;
        int __dword_9B4;
        int __dword_9B8;
        int32_t chapter;
        int __dword_9C0;
        int32_t __int_9C4;
        int32_t __int_9C8;
        int __dword_9CC;
        int32_t __ecl_var_9907;
        int32_t miss_count_in_game;
        int __dword_9D8;
        int __dword_9DC;
        int __dword_9E0;
        int __dword_9E4;
        int __dword_9E8;
        int32_t __int_array_9EC[20];
        int __dword_A3C;
        int __dword_A40;
        int __dword_array_A44[2];
        int __dword_A4C;
        int __dword_A50;
        int __dword_A54;
    };

};

namespace V1_10c {
    /* 424 */
    struct GlobalsSide {
        uint64_t score;
        int field_8;
        int shottype;
        int field_10;
        int field_14;
        int field_18;
        int field_1C;
        int field_20;
        int gauge;
        int c1_threshold;
        int c2_threshold;
        int c3_threshold;
        int c4_threshold;
        int c3_level;
        int c4_level;
        int field_40;
        int field_44;
        int field_48;
        int field_4C;
        int field_50;
        int lives;
        int max_lives;
        int field_5C;
        int field_60;
        int field_64;
        int bombs;
        int field_6C;
        int field_70;
        int field_74;
        int field_78;
        int field_7C;
        int field_80;
        int field_84;
        int field_88;
        int field_8C;
        int field_90;
        int field_94;
        int field_98;
        int field_9C;
        int field_A0;
        int field_A4;
        int field_A8;
        int field_AC;
        int field_B0;
        int field_B4;
        int field_B8;
        int field_BC;
        int field_C0;
        int field_C4;
    };

    /* 425 */
    struct Globals {
        GlobalsSide side[2];
        int difficulty;
        int field_194;
        int field_198;
        int field_19C;
        int field_1A0;
        int field_1A4;
        int field_1A8;
        int field_1AC;
        int field_1B0;
        int field_1B4[256];
        int field_5B4[256];
        int field_9B4;
        int field_9B8;
        int field_9BC;
        int story_stage;
        int field_9C4;
        int field_9C8;
        int field_9CC;
        int field_9D0;
        int field_9D4;
        int field_9D8;
        int field_9DC;
        int field_9E0;
        int field_9E4;
        int field_9E8;
        int field_9EC;
        int field_9F0;
        int field_9F4;
        int field_9F8;
        int field_9FC[21];
        uint32_t rules;
        uint32_t player_rules[2];
        int pvp_timer_start;
        int pvp_timer;
        int field_A64;
    };
};

enum PlayerBarrierState : int {
    BARRIER_ACTIVE = 1,
    BARRIER_INACTIVE = 2,
    BARRIER_DISABLED = 3,
};

struct PlayerBarrier { /* size uncertain */
    PlayerBarrierState state;
    Timer19 no_hit_timer; /* hitstun seems to last 40f (timer does not dictate stun status) */
    uint8_t unk0[4];
    int anm_id;
};

#include <Windows.h>

void drawEnemyHP(PCONTEXT pCtx, float SCALE);

} }