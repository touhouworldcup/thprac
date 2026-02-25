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

void TH185Warps5(THPrac::stage_warps_t& out) noexcept {
    out.section_param = {
        {
            .label = S(TH185_WAVE_1)
        },
        {
            .label = S(TH185_WAVE_2),
            .jumps = {
                { "main", {
                    { .off = 0x28c, .dest = 0x3b4 }
                } }
            },
            .writes = {
                { "main", {
                    { .off = 0x28a, .bytes = { 0x02, 0x00, 0x00, 0x00 } }
                } }
            }
        },
        {
            .label = S(TH185_WAVE_3),
            .jumps = {
                { "main", {
                    { .off = 0x28c, .dest = 0x480 }
                } }
            },
            .writes = {
                { "main", {
                    { .off = 0x28a, .bytes = { 0x03, 0x00, 0x00, 0x00 } }
                } }
            }
        },
        {
            .label = S(TH185_WAVE_4),
            .jumps = {
                { "main", {
                    { .off = 0x28c, .dest = 0x54c }
                } }
            },
            .writes = {
                { "main", {
                    { .off = 0x28a, .bytes = { 0x04, 0x00, 0x00, 0x00 } }
                } }
            }
        },
        {
            .label = S(TH185_WAVE_5),
            .jumps = {
                { "main", {
                    { .off = 0x28c, .dest = 0x618 }
                } }
            },
            .writes = {
                { "main", {
                    { .off = 0x28a, .bytes = { 0x05, 0x00, 0x00, 0x00 } }
                } }
            }
        },
        {
            .label = S(TH185_WAVE_6),
            .jumps = {
                { "main", {
                    { .off = 0x28c, .dest = 0x6e4 }
                } }
            },
            .writes = {
                { "main", {
                    { .off = 0x28a, .bytes = { 0x06, 0x00, 0x00, 0x00 } }
                } }
            }
        },
        {
            .label = S(TH185_WAVE_7),
            .jumps = {
                { "main", {
                    { .off = 0x28c, .dest = 0x7b0 }
                } }
            },
            .writes = {
                { "main", {
                    { .off = 0x28a, .bytes = { 0x07, 0x00, 0x00, 0x00 } }
                } }
            }
        },
        {
            .label = S(TH_BOSS),
            .jumps = {
                { "main", {
                    { .off = 0x28c, .dest = 0x890 }
                } }
            },
        }
    };
    out.section_param[7].phases = {
        .label = S(TH_BOSS),
        .type = stage_warps_t::TYPE_COMBO,
        .section_param = {
            {
                .label = S(TH185_NONE_RANDOM)
            },
            {
                .label = S(TH185_KUDAMAKI_TSUKASA),
                .writes = {
                    { "WorldWaveB00", {
                        { .off = 0x34, .bytes = { FORCE_BOSS(0, 18) } }
                    } }
                }
            },
            {
                .label = S(TH185_IIZUNAMARU_MEGUMU),
                .writes = {
                    { "WorldWaveB00", {
                        { .off = 0x34, .bytes = { FORCE_BOSS(1, 18) } }
                    } }
                }
            },
            {
                .label = S(TH185_CLOWNPIECE),
                .writes = {
                    { "WorldWaveB00", {
                        { .off = 0x34, .bytes = { FORCE_BOSS(2, 18) } }
                    } }
                }
            },
            {
                .label = S(TH185_HINANAWI_TENSHI),
                .writes = {
                    { "WorldWaveB00", {
                        { .off = 0x34, .bytes = { FORCE_BOSS(3, 18) } }
                    } }
                }
            }
        }
    };
    out.section_param[7].phases->section_param[1].phases = {
        .label = S(TH_ATTACK),
        .type = stage_warps_t::TYPE_COMBO,
        .section_param = {
            {
                .label = S(TH_NONSPELL)
            },
            {
                .label = S(TH185_SPELL_17_1),
                .writes = {
                    { "Boss17Boss", {
                        { .off = 0x1b0, .bytes = { 0x18, 0x15, 0x00, 0x00 } }
                    } },
                    { "Boss17Boss1", {
                        { .off = 0x10, .bytes = {
                        0x00, 0x00, 0x00, 0x00, 0x0F, 0x02, 0x1C, 0x00, 0x00, 0x00, 0xFF, 0x03, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2F, 0x45, 0x80, 0xA0, 0xFF, 0xFF,
                        BOSS_SPELL_CARD_ASYNC(17, 1)
                    } } } }
                }
            },
            {
                .label = S(TH185_SPELL_17_2),
                .writes = {
                    { "Boss17Boss", {
                        { .off = 0x1b0, .bytes = { 0xf0, 0x0a, 0x00, 0x00 } }
                    } },
                    { "Boss17Boss1", {
                        { .off = 0x10, .bytes = { BOSS_SPELL_CARD_ASYNC(17, 2) } }
                    } }
                }
            }
        }
    };
    out.section_param[7].phases->section_param[2].phases = {
        .label = S(TH_ATTACK),
        .type = stage_warps_t::TYPE_COMBO,
        .section_param = {
            {
                .label = S(TH_NONSPELL)
            },
            {
                .label = S(TH185_SPELL_18_1),
                .writes = {
                    { "Boss18Boss", {
                        { .off = 0x1b0, .bytes = { 0x18, 0x15, 0x00, 0x00 } }
                    } },
                    { "Boss18Boss1", {
                        { .off = 0x10, .bytes = {
                        0x00, 0x00, 0x00, 0x00, 0x0F, 0x02, 0x1C, 0x00, 0x00, 0x00, 0xFF, 0x03, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2F, 0x45, 0x80, 0xA0, 0xFF, 0xFF,
                        BOSS_SPELL_CARD_ASYNC(18, 1)
                    } } } }
                }
            },
            {
                .label = S(TH185_SPELL_18_2),
                .writes = {
                    { "Boss18Boss", {
                        { .off = 0x1b0, .bytes = { 0xf0, 0x0a, 0x00, 0x00 } }
                    } },
                    { "Boss18Boss1", {
                        { .off = 0x10, .bytes = { BOSS_SPELL_CARD_ASYNC(18, 2) } }
                    } }
                }
            }
        }
    };
    out.section_param[7].phases->section_param[3].phases = {
        .label = S(TH_ATTACK),
        .type = stage_warps_t::TYPE_COMBO,
        .section_param = {
            {
                .label = S(TH_NONSPELL)
            },
            {
                .label = S(TH185_SPELL_19_1),
                .writes = {
                    { "Boss19Boss", {
                        { .off = 0x1b0, .bytes = { 0x18, 0x15, 0x00, 0x00 } }
                    } },
                    { "Boss19Boss1", {
                    { .off = 0x10, .bytes = {
                        0x00, 0x00, 0x00, 0x00, 0x0F, 0x02, 0x1C, 0x00, 0x00, 0x00, 0xFF, 0x03, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2F, 0x45, 0x80, 0xA0, 0xFF, 0xFF,
                        BOSS_SPELL_CARD_ASYNC(19, 1)
                    } } } }
                }
            },
            {
                .label = S(TH185_SPELL_19_2),
                .writes = {
                    { "Boss19Boss", {
                        { .off = 0x1b0, .bytes = { 0xf0, 0x0a, 0x00, 0x00 } }
                    } },
                    { "Boss19Boss1", {
                        { .off = 0x10, .bytes = { BOSS_SPELL_CARD_ASYNC(19, 2) } }
                    } }
                }
            }
        }
    };
    out.section_param[7].phases->section_param[4].phases = {
        .label = S(TH_ATTACK),
        .type = stage_warps_t::TYPE_COMBO,
        .section_param = {
            {
                .label = S(TH_NONSPELL)
            },
            {
                .label = S(TH185_SPELL_20_1),
                .writes = {
                    { "Boss20Boss", {
                        { .off = 0x1b0, .bytes = { 0x18, 0x15, 0x00, 0x00 } }
                    } },
                    { "Boss20Boss1", {
                    { .off = 0x10, .bytes = {
                        0x00, 0x00, 0x00, 0x00, 0x0F, 0x02, 0x1C, 0x00, 0x00, 0x00, 0xFF, 0x03, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2F, 0x45, 0x80, 0xA0, 0xFF, 0xFF,
                        BOSS_SPELL_CARD_ASYNC(20, 1)
                    } } } }
                }
            },
            {
                .label = S(TH185_SPELL_20_2),
                .writes = {
                    { "Boss20Boss", {
                        { .off = 0x1b0, .bytes = { 0xf0, 0x0a, 0x00, 0x00 } }
                    } },
                    { "Boss20Boss1", {
                        { .off = 0x10, .bytes = { BOSS_SPELL_CARD_ASYNC(20, 2) } }
                    } }
                }
            }
        }
    };
}
