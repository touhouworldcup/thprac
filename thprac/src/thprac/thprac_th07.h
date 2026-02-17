#pragma once
#include "thprac_games.h"
#include "thprac_utils.h"

#include "../3rdParties/d3d8/include/d3d8.h"

namespace THPrac {
namespace TH07 {

// Struct definitions from https://github.com/exphp-share/th-re-data


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

struct Supervisor {
    uint32_t _unk_0;
    IDirect3D8* d3d;
    IDirect3DDevice8* d3d_device;
    char _unk_1[0x3C];
    D3DMATRIX view_matrix;
    D3DMATRIX projection_matrix;
    D3DVIEWPORT8 viewport;
    char present_params[0x36]; 
    Config config;
    char _unk_4[4];
    uint32_t gamemode;
    uint32_t gamemode_next;
    uint32_t gamemode_prev;
    char _unk_2[0x38];
    D3DCAPS8 device_caps;
    char _unk_3[0x50];
    uint8_t current_fog_enabled;
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

}
}