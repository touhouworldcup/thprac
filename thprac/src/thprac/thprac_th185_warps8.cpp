#include "thprac_games.h"

#define FORCE_BOSS_C(boss_index, start_index)                                                                                                                                                                \
    0x00, 0x00, 0x00, 0x00, 0xF3 * (boss_index == 0), 0x03 * (boss_index == 0), 0x18, 0x00, 0x00, 0x00, 0xFF, 0x02, 0x00, 0x00, 0x00, 0x00, (start_index), 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,         \
        0x00, 0x00, 0x00, 0x00, 0xF3 * (boss_index == 1), 0x03 * (boss_index == 1), 0x18, 0x00, 0x00, 0x00, 0xFF, 0x02, 0x00, 0x00, 0x00, 0x00, (start_index) + 1, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, \
        0x00, 0x00, 0x00, 0x00, 0xF3 * (boss_index == 2), 0x03 * (boss_index == 2), 0x18, 0x00, 0x00, 0x00, 0xFF, 0x02, 0x00, 0x00, 0x00, 0x00, (start_index) + 2, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, \
        0x00, 0x00, 0x00, 0x00, 0xF3 * (boss_index == 3), 0x03 * (boss_index == 3), 0x18, 0x00, 0x00, 0x00, 0xFF, 0x02, 0x00, 0x00, 0x00, 0x00, (start_index) + 3, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00


#define BOSS_SPELL_CARD_ASYNC(boss_id, spell_id) 0x00, 0x00, 0x00, 0x00, 0x0B, 0x00, 0x24, 0x00, 0x00, 0x00, 0xFF, 0x01, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x42, 0x6F, 0x73, 0x73, boss_id / 10 + 0x30, boss_id % 10 + 0x30, 0x42, 0x6F, 0x73, 0x73, 0x43, 0x61, 0x72, 0x64, 0x30 + spell_id, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0A, 0x00, 0x10, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00

using namespace THPrac;

void TH185Warps8(THPrac::stage_warps_t& out) noexcept {
    out.section_param = {
        {
            .label = S(TH185_WAVE_1),
        },
        {
            .label = S(TH185_WAVE_2),
            .jumps = {
                { "main", {
                    { .off = 0x434, .dest = 0x52c }
                } }
            },
            .writes = {
                { "main", {
                    { .off = 0x3f0, .bytes = { 0x02, 0x00, 0x00, 0x00 } }
                } }
            }
        },
        {
            .label = S(TH185_WAVE_3),
            .jumps = {
                { "main", {
                    { .off = 0x434, .dest = 0x628 }
                } }
            },
            .writes = {
                { "main", {
                    { .off = 0x3f0, .bytes = { 0x03, 0x00, 0x00, 0x00 } }
                } }
            }
        },
        {
            .label = S(TH185_WAVE_4),
            .jumps = {
                { "main", {
                    { .off = 0x434, .dest = 0x6d8 }
                } }
            },
            .writes = {
                { "main", {
                    { .off = 0x3f0, .bytes = { 0x04, 0x00, 0x00, 0x00 } }
                } }
            }
        },
        {
            .label = S(TH185_WAVE_5),
            .jumps = {
                { "main", {
                    { .off = 0x434, .dest = 0x7d4 }
                } }
            },
            .writes = {
                { "main", {
                    { .off = 0x3f0, .bytes = { 0x05, 0x00, 0x00, 0x00 } }
                } }
            }
        },
        {
            .label = S(TH185_WAVE_6),
            .jumps = {
                { "main", {
                    { .off = 0x434, .dest = 0x884 }
                } }
            },
            .writes = {
                { "main", {
                    { .off = 0x3f0, .bytes = { 0x06, 0x00, 0x00, 0x00 } }
                } }
            }
        },
        {
            .label = S(TH185_WAVE_7),
            .jumps = {
                { "main", {
                    { .off = 0x434, .dest = 0x980 }
                } }
            },
            .writes = {
                { "main", {
                    { .off = 0x3f0, .bytes = { 0x07, 0x00, 0x00, 0x00 } }
                } }
            }
        },
        {
            .label = S(TH185_WAVE_8),
            .jumps = {
                { "main", {
                    { .off = 0x434, .dest = 0xa30 }
                } }
            },
            .writes = {
                { "main", {
                    { .off = 0x3f0, .bytes = { 0x08, 0x00, 0x00, 0x00 } }
                } }
            }
        },
        {
            .label = S(TH185_WAVE_9),
            .jumps = {
                { "main", {
                    { .off = 0x434, .dest = 0xb2c }
                } }
            },
            .writes = {
                { "main", {
                    { .off = 0x3f0, .bytes = { 0x09, 0x00, 0x00, 0x00 } }
                } }
            }
        },
        {
            .label = S(TH185_WAVE_10),
            .jumps = {
                { "main", {
                    { .off = 0x434, .dest = 0xbdc }
                } }
            },
            .writes = {
                { "main", {
                    { .off = 0x3f0, .bytes = { 0x0a, 0x00, 0x00, 0x00 } }
                } }
            }
        },
        {
            .label = S(TH185_WAVE_11),
            .jumps = {
                { "main", {
                    { .off = 0x434, .dest = 0xcd8 }
                } }
            },
            .writes = {
                { "main", {
                    { .off = 0x3f0, .bytes = { 0x0b, 0x00, 0x00, 0x00 } }
                } }
            }
        },
        {
            .label = S(TH185_WAVE_12),
            .jumps = {
                { "main", {
                    { .off = 0x434, .dest = 0xd88 }
                } }
            }
        },
    };
    out.section_param[1].phases = {
        .label = S(TH_BOSS),
        .type = stage_warps_t::TYPE_COMBO,
        .section_param = {
            {
                .label = S(TH185_NONE_RANDOM),
            },
            {
                .label = S(TH185_GOUTOKUZI_MIKE),
                .writes = {
                    { "WorldWaveB01", {
                        { .off = 0x34, .bytes = {
                            FORCE_BOSS_C(0, 2)
                        } }
                    } }
                }
            },
            {
                .label = S(TH185_AKI_MINORIKO),
                .writes = {
                    { "WorldWaveB01", {
                        { .off = 0x34, .bytes = {
                            FORCE_BOSS_C(1, 2)
                        } }
                    } }
                }
            },
            {
                .label = S(TH185_ETERNITY_LARVA),
                .writes = {
                    { "WorldWaveB01", {
                        { .off = 0x34, .bytes = {
                            FORCE_BOSS_C(2, 2)
                        } }
                    } }
                }
            },
            {
                .label = S(TH185_SAKATA_NEMUNO),
                .writes = {
                    { "WorldWaveB01", {
                        { .off = 0x34, .bytes = {
                            FORCE_BOSS_C(3, 2)
                        } }
                    } }
                }
            }
        }
    };
    out.section_param[3].phases = {
        .label = S(TH_BOSS),
        .type = stage_warps_t::TYPE_COMBO,
        .section_param = {
            {
                .label = S(TH185_NONE_RANDOM)
            },
            {
                .label = S(TH185_CIRNO),
                .writes = {
                    { "WorldWaveB02", {
                        { .off = 0x34, .bytes = {
                            FORCE_BOSS_C(0, 6)
                        } }
                    } }
                }
            },
            {
                .label = S(TH185_WAKASAGIHIME),
                .writes = {
                    { "WorldWaveB02", {
                        { .off = 0x34, .bytes = {
                            FORCE_BOSS_C(1, 6)
                        } }
                    } }
                }
            },
            {
                .label = S(TH185_SEKIBANKI),
                .writes = {
                    { "WorldWaveB02", {
                        { .off = 0x34, .bytes = {
                            FORCE_BOSS_C(2, 6)
                        } }
                    } }
                }
            },
            {
                .label = S(TH185_USHIZAKI_URUMI),
                .writes = {
                    { "WorldWaveB02", {
                        { .off = 0x34, .bytes = {
                            FORCE_BOSS_C(3, 6)
                        } }
                    } }
                }
            }
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
                    { "WorldWaveB03", {
                        { .off = 0x34, .bytes = {
                            FORCE_BOSS_C(0, 10)
                        } }
                    } }
                }
            },
            {
                .label = S(TH185_NIWATARI_KUTAKA),
                .writes = {
                    { "WorldWaveB03", {
                        { .off = 0x34, .bytes = {
                            FORCE_BOSS_C(1, 10)
                        } }
                    } }
                }
            },
            {
                .label = S(TH185_YATADERA_NARUMI),
                .writes = {
                    { "WorldWaveB03", {
                        { .off = 0x34, .bytes = {
                            FORCE_BOSS_C(2, 10)
                        } }
                    } }
                }
            },
            {
                .label = S(TH185_ONOZUKA_KOMACHI),
                .writes = {
                    { "WorldWaveB03", {
                        { .off = 0x34, .bytes = {
                            FORCE_BOSS_C(3, 10)
                        } }
                    } }
                }
            }
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
                .label = S(TH185_KOCHIYA_SANAE),
                .writes = {
                    { "WorldWaveB04", {
                        { .off = 0x34, .bytes = {
                            FORCE_BOSS_C(0, 14)
                        } }
                    } }
                }
            },
            {
                .label = S(TH185_IZAYOI_SAKUYA),
                .writes = {
                    { "WorldWaveB04", {
                        { .off = 0x34, .bytes = {
                            FORCE_BOSS_C(1, 14)
                        } }
                    } }
                }
            },
            {
                .label = S(TH185_KONPAKU_YOUMU),
                .writes = {
                    { "WorldWaveB04", {
                        { .off = 0x34, .bytes = {
                            FORCE_BOSS_C(2, 14)
                        } }
                    } }
                }
            },
            {
                .label = S(TH185_HAKUREI_REIMU),
                .writes = {
                    { "WorldWaveB04", {
                        { .off = 0x34, .bytes = {
                            FORCE_BOSS_C(3, 14)
                        } }
                    } }
                }
            }
        }
    };
    out.section_param[9].phases = {
        .label = S(TH_BOSS),
        .type = stage_warps_t::TYPE_COMBO,
        .section_param = {
            {
                .label = S(TH185_NONE_RANDOM)
            },
            {
                .label = S(TH185_KUDAMAKI_TSUKASA),
                .writes = {
                    { "WorldWaveB05", {
                        { .off = 0x34, .bytes = {
                            FORCE_BOSS_C(0, 18)
                        } }
                    } }
                }
            },
            {
                .label = S(TH185_IIZUNAMARU_MEGUMU),
                .writes = {
                    { "WorldWaveB05", {
                        { .off = 0x34, .bytes = {
                            FORCE_BOSS_C(1, 18)
                        } }
                    } }
                }
            },
            {
                .label = S(TH185_CLOWNPIECE),
                .writes = {
                    { "WorldWaveB05", {
                        { .off = 0x34, .bytes = {
                            FORCE_BOSS_C(2, 18)
                        } }
                    } }
                }
            },
            {
                .label = S(TH185_HINANAWI_TENSHI),
                .writes = {
                    { "WorldWaveB05", {
                        { .off = 0x34, .bytes = {
                            FORCE_BOSS_C(3, 18)
                        } }
                    } }
                }
            }
        }
    };
    out.section_param[11].phases = {
        .label = S(TH_BOSS),
        .type = stage_warps_t::TYPE_COMBO,
        .section_param = {
            {
                .label = S(TH185_NONE_RANDOM)
            },
            {
                .label = S(TH185_IBUKI_SUIKA),
                .writes = {
                    { "WorldWaveB06", {
                        { .off = 0x34, .bytes = {
                            FORCE_BOSS_C(0, 22)
                        } }
                    } }
                }
            },
            {
                .label = S(TH185_FUTATUIWA_MAMIZOU),
                .writes = {
                    { "WorldWaveB06", {
                        { .off = 0x34, .bytes = {
                            FORCE_BOSS_C(1, 22)
                        } }
                    } }
                }
            },
            {
                .label = S(TH185_KUROKOMA_SAKI),
                .writes = {
                    { "WorldWaveB06", {
                        { .off = 0x34, .bytes = {
                            FORCE_BOSS_C(2, 22)
                        } }
                    } }
                }
            },
            {
                .label = S(TH185_HIMEMUSHI_MOMOYO),
                .writes = {
                    { "WorldWaveB06", {
                        { .off = 0x34, .bytes = {
                            FORCE_BOSS_C(3, 22)
                        } }
                    } }
                }
            }
        }
    };
    out.section_param[1].phases->section_param[1].phases = {
        .label = S(TH_ATTACK),
        .type = stage_warps_t::TYPE_COMBO,
        .section_param = {
            {
                .label = S(TH_NONSPELL)
            },
            {
                .label = S(TH185_SPELL_1_1),
                .writes = {
                    { "Boss01Boss", {
                        { .off = 0x1b0, .bytes = { 0x14, 0x05, 0x00, 0x00 } }
                    } },
                    { "Boss01Boss1", {
                        { .off = 0x10, .bytes = { BOSS_SPELL_CARD_ASYNC(1, 1) } }
                    } }
                }
            }
        }
    };
    out.section_param[1].phases->section_param[2].phases = {
        .label = S(TH_ATTACK),
        .type = stage_warps_t::TYPE_COMBO,
        .section_param = {
            {
                .label = S(TH_NONSPELL)
            },
            {
                .label = S(TH185_SPELL_2_1),
                .writes = {
                    { "Boss02Boss", {
                        { .off = 0x1b0, .bytes = { 0x14, 0x05, 0x00, 0x00 } }
                    } },
                    { "Boss02Boss1", {
                        { .off = 0x10, .bytes = { BOSS_SPELL_CARD_ASYNC(2, 1) } }
                    } }
                }
            }
        }
    };
    out.section_param[1].phases->section_param[3].phases = {
        .label = S(TH_ATTACK),
        .type = stage_warps_t::TYPE_COMBO,
        .section_param = {
            {
                .label = S(TH_NONSPELL)
            },
            {
                .label = S(TH185_SPELL_3_1),
                .writes = {
                    { "Boss03Boss", {
                        { .off = 0x1b0, .bytes = { 0x14, 0x05, 0x00, 0x00 } }
                    } },
                    { "Boss03Boss1", {
                        { .off = 0x10, .bytes = { BOSS_SPELL_CARD_ASYNC(3, 1) } }
                    } }
                }
            }
        }
    };
    out.section_param[1].phases->section_param[4].phases = {
        .label = S(TH_ATTACK),
        .type = stage_warps_t::TYPE_COMBO,
        .section_param = {
            {
                .label = S(TH_NONSPELL)
            },
            {
                .label = S(TH185_SPELL_4_1),
                .writes = {
                    { "Boss04Boss", {
                        { .off = 0x1b0, .bytes = { 0x14, 0x05, 0x00, 0x00 } }
                    } },
                    { "Boss04Boss1", {
                        { .off = 0x10, .bytes = { BOSS_SPELL_CARD_ASYNC(4, 1) } }
                    } }
                }
            }
        }
    };
    out.section_param[3].phases->section_param[1].phases = {
        .label = S(TH_ATTACK),
        .type = stage_warps_t::TYPE_COMBO,
        .section_param = {
            {
                .label = S(TH_NONSPELL)
            },
            {
                .label = S(TH185_SPELL_5_1),
                .writes = {
                    { "Boss05Boss", {
                        { .off = 0x1b0, .bytes = { 0xDC, 0x05, 0x00, 0x00 } }
                    } },
                    { "Boss05Boss1", {
                        { .off = 0x10, .bytes = { BOSS_SPELL_CARD_ASYNC(5, 1) } }
                    } }
                }
            }
        }
    };
    out.section_param[3].phases->section_param[2].phases = {
        .label = S(TH_ATTACK),
        .type = stage_warps_t::TYPE_COMBO,
        .section_param = {
            {
                .label = S(TH_NONSPELL)
            },
            {
                .label = S(TH185_SPELL_6_1),
                .writes = {
                    { "Boss06Boss", {
                        { .off = 0x1b0, .bytes = { 0xDC, 0x05, 0x00, 0x00 } }
                    } },
                    { "Boss06Boss1", {
                        { .off = 0x10, .bytes = { BOSS_SPELL_CARD_ASYNC(6, 1) } }
                    } }
                }
            }
        }
    };
    out.section_param[3].phases->section_param[3].phases = {
        .label = S(TH_ATTACK),
        .type = stage_warps_t::TYPE_COMBO,
        .section_param = {
            {
                .label = S(TH_NONSPELL)
            },
            {
                .label = S(TH185_SPELL_7_1),
                .writes = {
                    { "Boss07Boss", {
                        { .off = 0x1b0, .bytes = { 0x14, 0x05, 0x00, 0x00 } }
                    } },
                    { "Boss07Boss1", {
                        { .off = 0x10, .bytes = { BOSS_SPELL_CARD_ASYNC(7, 1)} }
                    } }
                }
            }
        }
    };
    out.section_param[3].phases->section_param[4].phases = {
        .label = S(TH_ATTACK),
        .type = stage_warps_t::TYPE_COMBO,
        .section_param = {
            {
                .label = S(TH_NONSPELL)
            },
            {
                .label = S(TH185_SPELL_8_1),
                .writes = {
                    { "Boss08Boss", {
                        { .off = 0x1b0, .bytes = { 0xDC, 0x05, 0x00, 0x00 } }
                    } },
                    { "Boss08Boss1", {
                        { .off = 0x10, .bytes = { BOSS_SPELL_CARD_ASYNC(8, 1) } }
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
    out.section_param[7].phases->section_param[1].phases = {
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
    out.section_param[7].phases->section_param[2].phases = {
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
    out.section_param[7].phases->section_param[3].phases = {
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
    out.section_param[7].phases->section_param[4].phases = {
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
    out.section_param[9].phases->section_param[1].phases = {
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
    out.section_param[9].phases->section_param[2].phases = {
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
    out.section_param[9].phases->section_param[3].phases = {
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
    out.section_param[9].phases->section_param[4].phases = {
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
    out.section_param[11].phases->section_param[1].phases = {
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
    out.section_param[11].phases->section_param[2].phases = {
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
                        { .off = 0x1b0, .bytes = { 0x4a, 0x01, 0x00, 0x00 } }
                    } },
                    { "Boss22Boss1", {
                    { .off = 0x10, .bytes = {
                        BOSS_SPELL_CARD_ASYNC(22, 2)
                    } } } }
                }
            }
        }
    };
    out.section_param[11].phases->section_param[3].phases = {
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
    out.section_param[11].phases->section_param[4].phases = {
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
                        0x00, 0x00, 0x00, 0x00, 0x0F, 0x02, 0x1C, 0x00, 0x00, 0x00, 0xFF, 0x03, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x96, 0x44, 0x80, 0xA0, 0xFF, 0xFF,
                        BOSS_SPELL_CARD_ASYNC(24, 1)
                    } } } }
                }
            },
            {
                .label = S(TH185_SPELL_24_2),
                .writes = {
                    { "Boss24Boss", {
                        { .off = 0x1b0, .bytes = { 0x4a, 0x01, 0x00, 0x00 } }
                    } },
                    { "Boss24Boss1", {
                    { .off = 0x10, .bytes = {
                        BOSS_SPELL_CARD_ASYNC(24, 2)
                    } } } }
                }
            }
        }
    };
}
