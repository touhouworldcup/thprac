#pragma once
#include "thprac_locale_def.h"

namespace THPrac {
namespace Gui {
    enum locale_t {
        LOCALE_NONE = -1,
        LOCALE_ZH_CN = 0,
        LOCALE_EN_US = 1,
        LOCALE_JA_JP = 2,
    };

    void LocaleSet(locale_t locale);
    void LocaleAutoSet();
    locale_t LocaleGet();
    void LocaleRotate();
    bool LocaleInitFromCfg();

    bool LocaleCreateFont(float font_size);
    bool LocaleCreateMergeFont(locale_t locale, float font_size);
    bool LocaleRecreateMergeFont(locale_t locale, float font_size);

    const char** LocaleGetCurrentGlossary();
    const char* LocaleGetStr(th_glossary_t name);

// TODO: These can't be refactored into functions as-is, because they depend on
// the current game's namespace being active where they're used. Fixing this
// will require an architectural change.
#define XCBA(stage, type) th_sections_cba[stage][type]
#define XSSS(rank) (th_sections_str[::THPrac::Gui::LocaleGet()][rank])

}
}
