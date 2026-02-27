#include "thprac_games.h"


#define BOSS_SPELL_CARD_ASYNC(boss_id, spell_id) 0x00, 0x00, 0x00, 0x00, 0x0B, 0x00, 0x24, 0x00, 0x00, 0x00, 0xFF, 0x01, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x42, 0x6F, 0x73, 0x73, boss_id / 10 + 0x30, boss_id % 10 + 0x30, 0x42, 0x6F, 0x73, 0x73, 0x43, 0x61, 0x72, 0x64, 0x30 + spell_id, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0A, 0x00, 0x10, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00

using namespace THPrac;

void TH185Warps7(THPrac::stage_warps_t& out) noexcept {
    out.section_param = {
        {
            .label = S(TH185_WAVE_1)
        },
        {
            .label = S(TH185_WAVE_2),
            .jumps = {
                { "main", {
                    { .off = 0x29c, .dest = 0x3d4 }
                } }
            },
            .writes = {
                { "main", {
                    { .off = 0x288, .bytes = { 0x02, 0x00, 0x00, 0x00 } }
                } }
            }
        },
        {
            .label = S(TH185_WAVE_3),
            .jumps = {
                { "main", {
                    { .off = 0x29c, .dest = 0x4b0 }
                } }
            },
            .writes = {
                { "main", {
                    { .off = 0x288, .bytes = { 0x03, 0x00, 0x00, 0x00 } }
                } }
            }
        },
        {
            .label = S(TH185_WAVE_4),
            .jumps = {
                { "main", {
                    { .off = 0x29c, .dest = 0x58c }
                } }
            },
            .writes = {
                { "main", {
                    { .off = 0x288, .bytes = { 0x04, 0x00, 0x00, 0x00 } }
                } }
            }
        },
        {
            .label = S(TH185_TENKYU_CHIMATA),
            .jumps = {
                { "main", {
                    { .off = 0x29c, .dest = 0x67c }
                } }
            },
        }
    };
    out.section_param[4].phases = {
        .label = S(TH_ATTACK),
        .type = stage_warps_t::TYPE_COMBO,
        .section_param = {
            {
                .label = S(TH_DLG)
            },
            {
                .label = S(TH_NONSPELL),
                .writes = {
                    { "Boss25Boss", {
                        { .off = 0x208, .bytes = { 0, 0 } }
                    } }
                }
            },
            {
                .label = S(TH185_SPELL_25_1),
                .writes = {
                    { "Boss25Boss", {
                        { .off = 0x208, .bytes = { 0, 0 } },
                        { .off = 0x1b0, .bytes = { 0x58, 0x1b, 0x00, 0x00 } }
                    } },
                    { "Boss25Boss1", {
                        { .off = 0x10, .bytes = {
                            0x00, 0x00, 0x00, 0x00, 0x0F, 0x02, 0x1C, 0x00, 0x00, 0x00, 0xFF, 0x03, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7A, 0x45, 0x80, 0xA0, 0xFF, 0xFF,
                            BOSS_SPELL_CARD_ASYNC(25, 1)
                        } }
                    } }
                }
            },
            {
                .label = S(TH185_SPELL_25_2),
                .writes = {
                    { "Boss25Boss", {
                        { .off = 0x208, .bytes = { 0, 0 } },
                        { .off = 0x1b0, .bytes = { 0xa0, 0x0f, 0x00, 0x00 } }
                    } },
                    { "Boss25Boss1", {
                        { .off = 0x10, .bytes = {
                            BOSS_SPELL_CARD_ASYNC(25, 2)
                        } }
                    } }
                }
            }
        }
    };
}
