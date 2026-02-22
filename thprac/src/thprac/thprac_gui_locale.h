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
    extern locale_t __glocale_current;
    __forceinline locale_t LocaleGet()
    {
        return __glocale_current;
    }
    __forceinline const char** LocaleGetCurrentGlossary() {
        return th_glossary_str[LocaleGet()];
    };
    __forceinline const char* LocaleGetStr(th_glossary_t name) {
        return LocaleGetCurrentGlossary()[name];
    };

    void LocaleRotate();
    bool LocaleInitFromCfg();

    bool LocaleCreateFont(float font_size);
    bool LocaleCreateMergeFont(locale_t locale, float font_size);
    bool LocaleRecreateMergeFont(locale_t locale, float font_size);


    void LocaleInit();

// TODO: These can't be refactored into functions as-is, because they depend on
// the current game's namespace being active where they're used. Fixing this
// will require an architectural change.
#define XCBA(stage, type) th_sections_cba[stage][type]
#define XSSS(rank) (th_sections_str[::THPrac::Gui::LocaleGet()][rank])

}

__forceinline const char* S(th_glossary_t name)
{
    return Gui::LocaleGetStr(name);
}
}
