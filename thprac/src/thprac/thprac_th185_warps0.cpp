#include "thprac_games.h"

using namespace THPrac;

void TH185Warps0(THPrac::stage_warps_t& out) noexcept {
    out.section_param = {
        {
            .label = S(TH_TUTORIAL)
        },
        {
            .label = S(TH185_WAVE_1),
            .jumps = {
                { "main", {
                    { .off = 0x188, .dest = 0x230 }
                } }
            }
        },
        {
            .label = S(TH185_WAVE_2),
            .jumps = {
                { "main", {
                    { .off = 0x188, .dest = 0x2fc }
                } }
            },
            .writes = {
                { "main", {
                    { .off = 0x184, .bytes = { 2 } }
                } }
            }
        },
        {
            .label = S(TH185_WAVE_3),
            .jumps = {
                { "main", {
                    { .off = 0x188, .dest = 0x3b8 }
                } }
            },
            .writes = {
                { "main", {
                    { .off = 0x184, .bytes = { 3 } }
                } }
            }
        },
        {
            .label = S(TH_BOSS),
            .jumps = {
                { "main", {
                    { .off = 0x188, .dest = 0x488 }
                } }
            },
            .writes = {
                { "main", {
                    { .off = 0x184, .bytes = { 4 } }
                } }
            }
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
                .writes {
                    { "Boss01tBoss", {
                        { .off = 0x220, .bytes = { 0x00, 0x00, 0x24, 0x00 } }
                    } }
                }
            },
            {
                .label = S(TH185_SPELL_0_1),
                .writes {
                    { "Boss01tBoss", {
                        { .off = 0x220, .bytes = { 0x00, 0x00, 0x24, 0x00 } },
                        { .off = 0x1b0, .bytes = { 0x84, 0x03, 0x00, 0x00 } }
                    } }
                }
            }
        }
    };
}
