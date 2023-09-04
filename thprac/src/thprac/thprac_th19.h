#pragma once
#include <stdint.h>

// Struct definitions from a shared Ghidra project
// -----------------------------------------------

namespace THPrac { namespace TH19 {

struct GlobalsSide { /* Size should be 0xC0. DO NOT REMOVE FIELDS */
    uint64_t score;
    int32_t __int_8; /* idk, msg_related, when set to 2, unable to pause the game */
    int32_t shottype;
    int __dword_10;
    int __dword_14;
    int __dword_18;
    int __dword_1C;
    int extra_attack_gauge;
    int lv_1_spell_attack_threshold;
    int lv_2_spell_attack_threshold;
    int ex_attack_threshold;
    int boss_attack_threshold;
    int32_t c3_level; /* first level is 0 but shown as lvl1 in-game */
    int32_t c4_level; /* first level is 0 but shown as lvl1 in-game */
    int __dword_3C;
    int __dword_40; /* idk maybe AI/story mode related */
    int __dword_44;
    int __dword_48;
    int __dword_4C;
    int lives;
    int __dword_54;
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
    int __dword_9B0;
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

} }