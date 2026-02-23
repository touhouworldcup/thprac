#pragma once
#include "thprac_locale_def.h"
#include "thprac_cfg.h"

namespace THPrac {
namespace Gui {

    void LocaleSet(Locale locale);
    void LocaleSetFromSysLang();
    __forceinline Locale LocaleGet() {
        return gSettingsGlobal.language;
    }
    __forceinline const char** LocaleGetCurrentGlossary() {
        return th_glossary_str[LocaleGet()];
    };
    __forceinline const char* LocaleGetStr(th_glossary_t name) {
        return LocaleGetCurrentGlossary()[name];
    };

    bool LocaleCreateFont(float font_size);

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
