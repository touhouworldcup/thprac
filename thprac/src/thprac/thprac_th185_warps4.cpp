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

void TH185Warps4(THPrac::stage_warps_t& out) noexcept {
    out.section_param = {
        {
            .label = S(TH185_WAVE_1),
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
                    { .off = 0x258, .dest = 0x518 }
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
                    { .off = 0x258, .dest = 0x5e4 }
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
                    { .off = 0x258, .dest = 0x6c4 }
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
                .label = S(TH185_KOCHIYA_SANAE),
                .writes = {
                    { "WorldWaveB00", {
                        { .off = 0x34, .bytes = { FORCE_BOSS(0, 14) } }
                    } }
                }
            },
            {
                .label = S(TH185_IZAYOI_SAKUYA),
                .writes = {
                    { "WorldWaveB00", {
                        { .off = 0x34, .bytes = { FORCE_BOSS(1, 14) } }
                    } }
                }
            },
            {
                .label = S(TH185_KONPAKU_YOUMU),
                .writes = {
                    { "WorldWaveB00", {
                        { .off = 0x34, .bytes = { FORCE_BOSS(2, 14) } }
                    } }
                }
            },
            {
                .label = S(TH185_HAKUREI_REIMU),
                .writes = {
                    { "WorldWaveB00", {
                        { .off = 0x34, .bytes = { FORCE_BOSS(3, 14) } }
                    } }
                }
            },
            {
                .label = S(TH185_KAWASIRO_NITORI),
                .writes = {
                    { "WorldWaveB00", {
                        { .off = 0x34, .bytes = {
                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0xFF, 0x02, 0x00, 0x00, 0x00, 0x00, 14, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0xFF, 0x02, 0x00, 0x00, 0x00, 0x00, 15, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0xFF, 0x02, 0x00, 0x00, 0x00, 0x00, 16, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x14, 0x00, 0x01, 0x00, 0xFF, 0x01, 0x00, 0x00, 0x00, 0x00, 0x5E, 0xD9, 0xFF, 0xFF,
                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0xFF, 0x02, 0x00, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                        0x00, 0x00, 0x00, 0x00, 0xF3, 0x03, 0x18, 0x00, 0x00, 0x00, 0xFF, 0x02, 0x00, 0x00, 0x00, 0x00, 27, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00 } }
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
                .label = S(TH185_SPELL_13_1),
                .writes = {
                    { "Boss13Boss", {
                        { .off = 0x1b0, .bytes = { 0x98, 0x08, 0x00, 0x00 } }
                    } },
                    { "Boss13Boss1", {
                        { .off = 0x10, .bytes = { BOSS_SPELL_CARD_ASYNC(13, 1) } }
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
                .label = S(TH185_SPELL_14_1),
                .writes = {
                    { "Boss14Boss", {
                        { .off = 0x1b0, .bytes = { 0x98, 0x08, 0x00, 0x00 } }
                    } },
                    { "Boss14Boss1", {
                        { .off = 0x10, .bytes = { BOSS_SPELL_CARD_ASYNC(14, 1) } }
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
                .label = S(TH185_SPELL_15_1),
                .writes = {
                    { "Boss15Boss", {
                        { .off = 0x1b0, .bytes = { 0xf0, 0x0a, 0x00, 0x00 } }
                    } },
                    { "Boss15Boss1", {
                        { .off = 0x10, .bytes = { BOSS_SPELL_CARD_ASYNC(15, 1) } }
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
                .label = S(TH185_SPELL_16_1),
                .writes = {
                    { "Boss16Boss", {
                        { .off = 0x1b0, .bytes = { 0xf0, 0x0a, 0x00, 0x00 } }
                    } },
                    { "Boss16Boss1", {
                        { .off = 0x10, .bytes = { BOSS_SPELL_CARD_ASYNC(16, 1) } }
                    } }
                }
            }
        }
    };
    out.section_param[5].phases->section_param[5].phases = {
        .label = S(TH_ATTACK),
        .type = stage_warps_t::TYPE_COMBO,
        .section_param = {
            {
                .label = S(TH_DLG)
            },
            {
                .label = S(TH_NONSPELL),
                .writes = {
                    { "Boss26Boss", {
                        { .off = 0x208, .bytes = {  0x00, 0x00, 0x24, 0x00 } }
                    } },
                }
            },
            {
                .label = S(TH185_SPELL_26_1),
                .writes = {
                    { "Boss26Boss", {
                        { .off = 0x1b0, .bytes = { 0xa0, 0x0f, 0x00, 0x00 } },
                        { .off = 0x208, .bytes = { 0x00, 0x00, 0x24, 0x00 } }
                    } },
                    { "Boss26Boss1", {
                        { .off = 0x10, .bytes = {
                            0x00, 0x00, 0x00, 0x00, 0x0F, 0x02, 0x1C, 0x00, 0x00, 0x00, 0xFF, 0x03, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFA, 0x44, 0x80, 0xA0, 0xFF, 0xFF,
                            BOSS_SPELL_CARD_ASYNC(26, 1)
                        }
                    } } }
                }
            },
            {
                .label = S(TH185_SPELL_26_2),
                .writes = {
                    { "Boss26Boss", {
                        { .off = 0x1b0, .bytes = { 0xd0, 0x07, 0x00, 0x00 } },
                        { .off = 0x208, .bytes = { 0x00, 0x00, 0x24, 0x00 } }
                    } },
                    { "Boss26Boss1", {
                        { .off = 0x10, .bytes = { BOSS_SPELL_CARD_ASYNC(26, 2) } }
                    } }
                }
            }
        }
    };
}
