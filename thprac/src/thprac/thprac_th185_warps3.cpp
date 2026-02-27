#include "thprac_games.h"

#define FORCE_BOSS(boss_index, start_index)                                                                                                                                                                  \
    0x00, 0x00, 0x00, 0x00, 0xF3 * (boss_index == 0), 0x03 * (boss_index == 0), 0x18, 0x00, 0x00, 0x00, 0xFF, 0x02, 0x00, 0x00, 0x00, 0x00, (start_index), 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,         \
        0x00, 0x00, 0x00, 0x00, 0xF3 * (boss_index == 1), 0x03 * (boss_index == 1), 0x18, 0x00, 0x00, 0x00, 0xFF, 0x02, 0x00, 0x00, 0x00, 0x00, (start_index) + 1, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, \
        0x00, 0x00, 0x00, 0x00, 0xF3 * (boss_index == 2), 0x03 * (boss_index == 2), 0x18, 0x00, 0x00, 0x00, 0xFF, 0x02, 0x00, 0x00, 0x00, 0x00, (start_index) + 2, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, \
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x14, 0x00, 0x01, 0x00, 0xFF, 0x01, 0x00, 0x00, 0x00, 0x00, 0x5E, 0xD9, 0xFF, 0xFF,                                                                              \
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0xFF, 0x02, 0x00, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,                                                      \
        0x00, 0x00, 0x00, 0x00, 0xF3 * (boss_index == 3), 0x03 * (boss_index == 3), 0x18, 0x00, 0x00, 0x00, 0xFF, 0x02, 0x00, 0x00, 0x00, 0x00, (start_index) + 3, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00


#define BOSS_SPELL_CARD_ASYNC(boss_id, spell_id) 0x00, 0x00, 0x00, 0x00, 0x0B, 0x00, 0x24, 0x00, 0x00, 0x00, 0xFF, 0x01, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x42, 0x6F, 0x73, 0x73, boss_id / 10 + 0x30, boss_id % 10 + 0x30, 0x42, 0x6F, 0x73, 0x73, 0x43, 0x61, 0x72, 0x64, 0x30 + spell_id, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0A, 0x00, 0x10, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00

using namespace THPrac;

void TH185Warps3(THPrac::stage_warps_t& out) noexcept {
    out.section_param = {
        {
            .label = S(TH185_WAVE_1)
        },
        {
            .label = S(TH185_WAVE_2),
            .jumps = {
                { "main", {
                    { .off = 0x258, .dest = 0x380 }
                } }
            },
            .writes = {
                { "main", {
                    { .off = 0x254, .bytes = { 0x02, 0x00, 0x00, 0x00 } }
                } }
            }
        },
        {
            .label = S(TH185_WAVE_3),
            .jumps = {
                { "main", {
                    { .off = 0x258, .dest = 0x44c }
                } }
            },
            .writes = {
                { "main", {
                    { .off = 0x254, .bytes = { 0x03, 0x00, 0x00, 0x00 } }
                } }
            }
        },
        {
            .label = S(TH185_WAVE_4),
            .jumps = {
                { "main", {
                    { .off = 0x258, .dest = 0x51c }
                } }
            },
            .writes = {
                { "main", {
                    { .off = 0x254, .bytes = { 0x04, 0x00, 0x00, 0x00 } }
                } }
            }
        },
        {
            .label = S(TH185_WAVE_5),
            .jumps = {
                { "main", {
                    { .off = 0x258, .dest = 0x5e8 }
                } }
            },
            .writes = {
                { "main", {
                    { .off = 0x254, .bytes = { 0x05, 0x00, 0x00, 0x00 } }
                } }
            }
        },
        {
            .label = S(TH_BOSS),
            .jumps = {
                { "main", {
                    { .off = 0x258, .dest = 0x6cc }
                } }
            },
        }
    };
    out.section_param[5].phases = {
        .label = S(TH_BOSS),
        .type = stage_warps_t::TYPE_COMBO,
        .section_param = {
            {
                .label = S(TH185_NONE_RANDOM)
            },
            {
                .label = S(TH185_EBISU_EIKA),
                .writes = {
                    { "WorldWaveB00", {
                        { .off = 0x34, .bytes = { FORCE_BOSS(0, 10) } }
                    } }
                }
            },
            {
                .label = S(TH185_NIWATARI_KUTAKA),
                .writes = {
                    { "WorldWaveB00", {
                        { .off = 0x34, .bytes = { FORCE_BOSS(1, 10) } }
                    } }
                }
            },
            {
                .label = S(TH185_YATADERA_NARUMI),
                .writes = {
                    { "WorldWaveB00", {
                        { .off = 0x34, .bytes = { FORCE_BOSS(2, 10) } }
                    } }
                }
            },
            {
                .label = S(TH185_ONOZUKA_KOMACHI),
                .writes = {
                    { "WorldWaveB00", {
                        { .off = 0x34, .bytes = { FORCE_BOSS(3, 10) } }
                    } }
                }
            }
        }
    };
    out.section_param[5].phases->section_param[1].phases = {
        .label = S(TH_ATTACK),
        .type = stage_warps_t::TYPE_COMBO,
        .section_param = {
            {
                .label = S(TH_NONSPELL)
            },
            {
                .label = S(TH185_SPELL_9_1),
                .writes = {
                    { "Boss09Boss", {
                        { .off = 0x1b0, .bytes = { 0x08, 0x07, 0x00, 0x00 } }
                    } },
                    { "Boss09Boss1", {
                        { .off = 0x10, .bytes = { BOSS_SPELL_CARD_ASYNC(9, 1) } }
                    } }
                }
            }
        }
    };
    out.section_param[5].phases->section_param[2].phases = {
        .label = S(TH_ATTACK),
        .type = stage_warps_t::TYPE_COMBO,
        .section_param = {
            {
                .label = S(TH_NONSPELL)
            },
            {
                .label = S(TH185_SPELL_10_1),
                .writes = {
                    { "Boss10Boss", {
                        { .off = 0x1b0, .bytes = { 0x08, 0x07, 0x00, 0x00 } }
                    } },
                    { "Boss10Boss1", {
                        { .off = 0x10, .bytes = { BOSS_SPELL_CARD_ASYNC(10, 1) } }
                    } }
                }
            }
        }
    };
    out.section_param[5].phases->section_param[3].phases = {
        .label = S(TH_ATTACK),
        .type = stage_warps_t::TYPE_COMBO,
        .section_param = {
            {
                .label = S(TH_NONSPELL)
            },
            {
                .label = S(TH185_SPELL_11_1),
                .writes = {
                    { "Boss11Boss", {
                        { .off = 0x1b0, .bytes = { 0x08, 0x07, 0x00, 0x00 } }
                    } },
                    { "Boss11Boss1", {
                        { .off = 0x10, .bytes = { BOSS_SPELL_CARD_ASYNC(11, 1) } }
                    } }
                }
            }
        }
    };
    out.section_param[5].phases->section_param[4].phases = {
        .label = S(TH_ATTACK),
        .type = stage_warps_t::TYPE_COMBO,
        .section_param = {
            {
                .label = S(TH_NONSPELL)
            },
            {
                .label = S(TH185_SPELL_12_1),
                .writes = {
                    { "Boss12Boss", {
                        { .off = 0x1b0, .bytes = { 0x08, 0x07, 0x00, 0x00 } }
                    } },
                    { "Boss12Boss1", {
                        { .off = 0x10, .bytes = { BOSS_SPELL_CARD_ASYNC(12, 1) } }
                    } }
                }
            }
        }
    };

}
