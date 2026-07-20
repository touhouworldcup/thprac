#pragma once
#include "thprac_games.h"
#include "thprac_utils.h"

#include "../3rdParties/d3d8/include/d3d8.h"

namespace THPrac {
namespace TH095 {

// Some of the structures are from https://github.com/exphp-share/th-re-data

// Anm IDs of data/bullet/photo2.png.
enum DATA_BULLET_PHOTO2_ANM_ID { 
    ANM_ID_DIGIT_0 = 15,
    ANM_ID_DIGIT_1 = 16,
    ANM_ID_DIGIT_2 = 17,
    ANM_ID_DIGIT_3 = 18,
    ANM_ID_DIGIT_4 = 19,
    ANM_ID_DIGIT_5 = 20,
    ANM_ID_DIGIT_6 = 21,
    ANM_ID_DIGIT_7 = 22,
    ANM_ID_DIGIT_8 = 23,
    ANM_ID_DIGIT_9 = 24,
    ANM_ID_PERCENT = 25,
    ANM_ID_DECIMAL_POINT = 26,
    ANM_ID_ENABLE = 27,
    ANM_ID_DISABLE = 28,
    ANM_ID_SUCCESS = 29,
    ANM_ID_FAILED = 30,
    ANM_ID_BOSS_SHOT = 31,
    ANM_ID_SELF_SHOT = 32,
    ANM_ID_TWO_SHOT = 33,
    ANM_ID_NICE_SHOT = 34,
    ANM_ID_RISK_SHOT = 35,
    ANM_ID_GEOMETRIC_UNUSED = 36,
    ANM_ID_RED_SHOT = 37,
    ANM_ID_PURPLE_SHOT = 38,
    ANM_ID_BLUE_SHOT = 39,
    ANM_ID_CYAN_SHOT = 40,
    ANM_ID_GREEN_SHOT = 41,
    ANM_ID_YELLOW_SHOT = 42,
    ANM_ID_ORANGE_SHOT = 43,
    ANM_ID_COLORFUL_SHOT = 44,
    ANM_ID_RAINBOW_SHOT = 45,
    ANM_ID_EMPTY_SHOT = 46,
    ANM_ID_SOLO_SHOT = 47,
    ANM_ID_LONELY_SHOT_UNUSED = 48,
    ANM_ID_MISS_SHOT_UNUSED = 49,
    ANM_ID_COUNTER_FLASH_UNUSED = 50
};

enum BonusBitfields {
    BONUS_BOSS_SHOT = 0x1,
    BONUS_SELF_SHOT = 0x2,
    BONUS_TWO_SHOT = 0x4,
    BONUS_NICE_SHOT = 0x8,
    BONUS_RISK_SHOT = 0x10,
    BONUS_GEOMETRIC_UNUSED = 0x20,
    BONUS_RED_SHOT = 0x40,
    BONUS_PURPLE_SHOT = 0x80,
    BONUS_BLUE_SHOT = 0x100,
    BONUS_CYAN_SHOT = 0x200,
    BONUS_GREEN_SHOT = 0x400,
    BONUS_YELLOW_SHOT = 0x800,
    BONUS_ORANGE_SHOT = 0x1000,
    BONUS_COLORFUL_SHOT = 0x2000,
    BONUS_RAINBOW_SHOT = 0x4000,
    BONUS_EMPTY_SHOT = 0x8000,
    BONUS_SOLO_SHOT = 0x10000,
    BONUS_LONELY_SHOT_UNUSED = 0x20000,
    BONUS_MISS_SHOT_UNUSED = 0x40000,
    BONUS_COUNTER_FLASH_UNUSED = 0x80000
};

struct ScoreResType {
    int final_score;
    int base_point;
    float bullet_shot_cnt;
    float graze_bullet_and_laser_cnt;
    float risk_shot_bonus_bugged;
    float boss_shot_multiplier;
    float nice_shot_multiplier;
    BonusBitfields bonuses;
};

struct AnmVmInterpModes {
    byte pos;
    byte rgb_1;
    byte alpha_1;
    byte rotate;
    byte scale;
    byte rgb_2;
    byte alpha_2;
    byte field7_0x7;
};

struct Timer {
    int32_t previous;
    float current_f;
    int32_t current;
};

struct AnmVmInterpTimers {
    Timer pos;
    Timer rgb_1;
    Timer alpha_1;
    Timer rotate;
    Timer scale;
    Timer rgb_2;
    Timer alpha_2;
};

struct AnmVmInterpData {
    Float3 pos_initial;
    Float3 pos_goal;
    Float3 rotate_initial;
    Float3 rotate_goal;
    Float2 scale_initial;
    Float2 scale_goal;
    D3DCOLOR color_1_initial;
    D3DCOLOR color_1_goal;
    D3DCOLOR color_2_initial;
    D3DCOLOR color_2_goal;
};

struct AnmVm {
    AnmVm* next_in_tick_list;
    AnmVm* next_in_layer_list;
    int32_t field2_0x8;
    int32_t layer;
    int32_t id;
    void* special_render_data;
    Float3 rotation;
    Float3 angular_velocity;
    Float2 scale;
    Float2 scale_growth;
    Float2 sprite_size;
    Float2 uv_scroll_pos;
    Timer time_in_script;
    Timer wait_timer;
    AnmVmInterpTimers interp_cur_times;
    AnmVmInterpTimers interp_end_times;
    AnmVmInterpModes interp_modes;
    int32_t int_vars[4];
    float float_vars[4];
    int32_t int_var_8;
    int32_t int_var_9;
    Float2 uv_scroll_vel;
    Float3 pos;
    Float3 entity_pos;
    Matrix44 matrix_1_unsure;
    Matrix44 matrix_2_unsure;
    Matrix44 matrix_3_unsure;
    D3DCOLOR color_1;
    D3DCOLOR color_2;
    uint32_t flags;
    int16_t unused_v3_ins_25_unsure;
    int16_t pending_interrupt;
    struct AnmLoaded* anm_files; 
    int16_t sprite_number;
    int16_t anm_file_index;
    int16_t field35_0x238;
    int16_t script_number;
    struct AnmRawInstr* beginning_of_script; 
    struct AnmRawInstr* current_instr; 
    struct AnmLoadedSprite* sprite;
    Timer interrupt_return_time;
    struct AnmRawInstr* interrupt_return_instr; 
    AnmVmInterpData interp_data;
    int32_t field43_0x2a8;
    int32_t field44_0x2ac;
    Float3 pos_2_unsure;
    int32_t time_of_last_sprite_set;
    uint8_t font_dims[2];
    uint8_t field48_0x2c2;
    uint8_t field49_0x2c3;
    uint8_t field50_0x2c4;
    uint8_t field51_0x2c5;
    uint8_t field52_0x2c6;
    uint8_t field53_0x2c7;
    uint8_t field54_0x2c8;
    uint8_t field55_0x2c9;
    uint8_t field56_0x2ca;
    uint8_t field57_0x2cb;
};  
static_assert(sizeof(AnmVm) == 0x2CC);

}  // namespace TH095
}  // namespace THPrac