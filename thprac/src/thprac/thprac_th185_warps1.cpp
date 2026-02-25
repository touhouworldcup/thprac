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

void TH185Warps1(THPrac::stage_warps_t& out) noexcept {
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
                    { .off = 0x254, .bytes = { 0x03, 0x00, 0x00, 0x00 }}
                } }
            }
        },
        {
            .label = S(TH_BOSS),
            .jumps = {
                { "main", {
                    { .off = 0x258, .dest = 0x530 }
                } }
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
                .label = S(TH185_GOUTOKUZI_MIKE),
                .writes = {
                    { "WorldWaveB00", {
                        { .off = 0x34, .bytes = { FORCE_BOSS(0, 2) } }
                    } }
                }
            },
            { 
                .label = S(TH185_AKI_MINORIKO),
                .writes = {
                    { "WorldWaveB00", {
                        { .off = 0x34, .bytes = { FORCE_BOSS(1, 2) } }
                    } }
                }
            },
            {
                .label = S(TH185_ETERNITY_LARVA),
                .writes = {
                    { "WorldWaveB00", {
                        { .off = 0x34, .bytes = { FORCE_BOSS(2, 2) } }
                    } }
                }
            },
            { 
                .label = S(TH185_SAKATA_NEMUNO),
                .writes = {
                    { "WorldWaveB00", {
                        { .off = 0x34, .bytes = { FORCE_BOSS(3, 2) } }
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
    out.section_param[3].phases->section_param[2].phases = {
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
    out.section_param[3].phases->section_param[3].phases = {
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
    out.section_param[3].phases->section_param[4].phases = {
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
}
