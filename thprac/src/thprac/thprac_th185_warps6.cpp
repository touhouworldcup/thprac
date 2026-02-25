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

void TH185Warps6(THPrac::stage_warps_t& out) noexcept {
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
            .label = S(TH185_WAVE_6),
            .jumps = {
                { "main", {
                    { .off = 0x258, .dest = 0x6b0 }
                } }
            },
            .writes = {
                { "main", {
                    { .off = 0x254, .bytes = { 0x06, 0x00, 0x00, 0x00 } }
                } }
            }
        },
        {
            .label = S(TH185_WAVE_7),
            .jumps = {
                { "main", {
                    { .off = 0x258, .dest = 0x77c }
                } }
            },
            .writes = {
                { "main", {
                    { .off = 0x254, .bytes = { 0x07, 0x00, 0x00, 0x00 } }
                } }
            }
        },
        {
            .label = S(TH_BOSS),
            .jumps = {
                { "main", {
                    { .off = 0x258, .dest = 0x84c }
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
                .label = S(TH185_IBUKI_SUIKA),
                .writes = {
                    { "WorldWaveB00", {
                        { .off = 0x34, .bytes = { FORCE_BOSS(0, 22) } }
                    } }
                }
            },
            {
                .label = S(TH185_FUTATUIWA_MAMIZOU),
                .writes = {
                    { "WorldWaveB00", {
                        { .off = 0x34, .bytes = { FORCE_BOSS(1, 22) } }
                    } }
                }
            },
            {
                .label = S(TH185_KUROKOMA_SAKI),
                .writes = {
                    { "WorldWaveB00", {
                        { .off = 0x34, .bytes = { FORCE_BOSS(2, 22) } }
                    } }
                }
            },
            {
                .label = S(TH185_HIMEMUSHI_MOMOYO),
                .writes = {
                    { "WorldWaveB00", {
                        { .off = 0x34, .bytes = { FORCE_BOSS(3, 22) } }
                    } }
                }
            },
            {
                .label = S(TH185_YAMASHIRO_TAKANE),
                .writes = {
                    { "WorldWaveB00", {
                        { .off = 0x34, .bytes = { {
                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0xFF, 0x02, 0x00, 0x00, 0x00, 0x00, 0x16, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0xFF, 0x02, 0x00, 0x00, 0x00, 0x00, 0x17, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0xFF, 0x02, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x14, 0x00, 0x01, 0x00, 0xFF, 0x01, 0x00, 0x00, 0x00, 0x00, 0x5E, 0xD9, 0xFF, 0xFF,
                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0xFF, 0x02, 0x00, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                        0x00, 0x00, 0x00, 0x00, 0xF3, 0x03, 0x18, 0x00, 0x00, 0x00, 0xFF, 0x02, 0x00, 0x00, 0x00, 0x00,   28, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00 } }
                    } }
                } }
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
                .label = S(TH185_SPELL_21_1),
                .writes = {
                    { "Boss21Boss", {
                        { .off = 0x1b0, .bytes = { 0xa0, 0x0f, 0x00, 0x00 } }
                    } },
                    { "Boss21Boss1", {
                    { .off = 0x10, .bytes = {
                        0x00, 0x00, 0x00, 0x00, 0x0F, 0x02, 0x1C, 0x00, 0x00, 0x00, 0xFF, 0x03, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x96, 0x44, 0x80, 0xA0, 0xFF, 0xFF,
                        BOSS_SPELL_CARD_ASYNC(21, 1)
                    } } } }
                }
            },
            {
                .label = S(TH185_SPELL_21_2),
                .writes = {
                    { "Boss21Boss", {
                        { .off = 0x1b0, .bytes = { 0xb0, 0x04, 0x00, 0x00 } }
                    } },
                    { "Boss21Boss1", {
                    { .off = 0x10, .bytes = {
                        BOSS_SPELL_CARD_ASYNC(21, 2)
                    } } } }
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
                .label = S(TH185_SPELL_22_1),
                .writes = {
                    { "Boss22Boss", {
                        { .off = 0x1b0, .bytes = { 0xd4, 0x17, 0x00, 0x00 } }
                    } },
                    { "Boss22Boss1", {
                    { .off = 0x10, .bytes = {
                        0x00, 0x00, 0x00, 0x00, 0x0F, 0x02, 0x1C, 0x00, 0x00, 0x00, 0xFF, 0x03, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x40, 0x4E, 0x45, 0x80, 0xA0, 0xFF, 0xFF,
                        BOSS_SPELL_CARD_ASYNC(22, 1)
                    } } } }
                }
            },
            {
                .label = S(TH185_SPELL_22_2),
                .writes = {
                    { "Boss22Boss", {
                        { .off = 0x1b0, .bytes = { 0xe4, 0x0c, 0x00, 0x00 } }
                    } },
                    { "Boss22Boss1", {
                    { .off = 0x10, .bytes = {
                        BOSS_SPELL_CARD_ASYNC(22, 2)
                    } } } }
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
                .label = S(TH185_SPELL_23_1),
                .writes = {
                    { "Boss23Boss", {
                        { .off = 0x1b0, .bytes = { 0xd4, 0x17, 0x00, 0x00 } }
                    } },
                    { "Boss23Boss1", {
                    { .off = 0x10, .bytes = {
                        0x00, 0x00, 0x00, 0x00, 0x0F, 0x02, 0x1C, 0x00, 0x00, 0x00, 0xFF, 0x03, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x40, 0x4E, 0x45, 0x80, 0xA0, 0xFF, 0xFF,
                        BOSS_SPELL_CARD_ASYNC(23, 1)
                    } } } }
                }
            },
            {
                .label = S(TH185_SPELL_23_2),
                .writes = {
                    { "Boss23Boss", {
                        { .off = 0x1b0, .bytes = { 0xe4, 0x0c, 0x00, 0x00 } }
                    } },
                    { "Boss23Boss1", {
                    { .off = 0x10, .bytes = {
                        BOSS_SPELL_CARD_ASYNC(23, 2)
                    } } } }
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
                .label = S(TH185_SPELL_24_1),
                .writes = {
                    { "Boss24Boss", {
                        { .off = 0x1b0, .bytes = { 0xd4, 0x17, 0x00, 0x00 } }
                    } },
                    { "Boss24Boss1", {
                    { .off = 0x10, .bytes = {
                        0x00, 0x00, 0x00, 0x00, 0x0F, 0x02, 0x1C, 0x00, 0x00, 0x00, 0xFF, 0x03, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x40, 0x4E, 0x45, 0x80, 0xA0, 0xFF, 0xFF,
                        BOSS_SPELL_CARD_ASYNC(24, 1)
                    } } } }
                }
            },
            {
                .label = S(TH185_SPELL_24_2),
                .writes = {
                    { "Boss24Boss", {
                        { .off = 0x1b0, .bytes = { 0xe4, 0x0c, 0x00, 0x00 } }
                    } },
                    { "Boss24Boss1", {
                    { .off = 0x10, .bytes = {
                        BOSS_SPELL_CARD_ASYNC(24, 2)
                    } } } }
                }
            }
        }
    };
    out.section_param[7].phases->section_param[5].phases = {
        .label = S(TH_ATTACK),
        .type = stage_warps_t::TYPE_COMBO,
        .section_param = {
            {
                .label = S(TH_DLG)
            },
            {
                .label = S(TH_NONSPELL),
                .writes = {
                    { "Boss27Boss", {
                        { .off = 0x208, .bytes = { 0, 0 } }
                    } }
                }
            },
            {
                .label = S(TH185_SPELL_27_1),
                .writes = {
                    { "Boss27Boss", {
                        { .off = 0x208, .bytes = { 0, 0 } },
                        { .off = 0x1b0, .bytes = { 0x88, 0x2c, 0x00, 0x00 } }
                    } },
                    { "Boss27Boss1", {
                        { .off = 0x10, .bytes = {
                            0x00, 0x00, 0x00, 0x00, 0x0F, 0x02, 0x1C, 0x00, 0x00, 0x00, 0xFF, 0x03, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0xC0, 0xF3, 0x45, 0x80, 0xA0, 0xFF, 0xFF,
                            0x00, 0x00, 0x00, 0x00, 0x0F, 0x02, 0x1C, 0x00, 0x00, 0x00, 0xFF, 0x03, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7A, 0x45, 0x80, 0xA0, 0xFF, 0xFF,
                            BOSS_SPELL_CARD_ASYNC(27, 1)
                        } }
                    } }
                }
            },
            {
                .label = S(TH185_SPELL_27_2),
                .writes = {
                    { "Boss27Boss", {
                        { .off = 0x208, .bytes = { 0, 0 } },
                        { .off = 0x1b0, .bytes = { 0x78, 0x1e, 0x00, 0x00 } }
                    } },
                    { "Boss27Boss1", {
                        { .off = 0x10, .bytes = {
                            0x00, 0x00, 0x00, 0x00, 0x0F, 0x02, 0x1C, 0x00, 0x00, 0x00, 0xFF, 0x03, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7A, 0x45, 0x80, 0xA0, 0xFF, 0xFF,
                            BOSS_SPELL_CARD_ASYNC(27, 2)
                        } }
                    } }
                }
            },
            {
                .label = S(TH185_SPELL_27_3),
                .writes = {
                    { "Boss27Boss", {
                        { .off = 0x208, .bytes = { 0, 0 } },
                        { .off = 0x1b0, .bytes = { 0xa0, 0x0f, 0x00, 0x00 } }
                    } },
                    { "Boss27Boss1", {
                        { .off = 0x10, .bytes = {
                            BOSS_SPELL_CARD_ASYNC(27, 3)
                        } }
                    } }
                }
            }
        }
    };
}

