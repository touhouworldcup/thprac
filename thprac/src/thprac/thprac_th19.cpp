#include "thprac_games.h"
#include "thprac_utils.h"

namespace THPrac {
namespace TH19 {
    HOOKSET_DEFINE(THMainHook)

    EHOOK_DY(th19_update, 0xC8B75) {
        GameGuiBegin(IMPL_WIN32_DX9);

        ImGui::TextUnformatted("Deez Nuts");

        GameGuiEnd();
    }

    EHOOK_DY(th19_render, 0xC8C8D) {
        GameGuiRender(IMPL_WIN32_DX9);
    }

    HOOKSET_ENDDEF()

    HOOKSET_DEFINE(THInitHook)
    static __declspec(noinline) void THGuiCreate()
    {
        // Init
        GameGuiInit(IMPL_WIN32_DX9, RVA(0x208388), RVA(0x209110), RVA(0xA9EE0),
            Gui::INGAGME_INPUT_GEN2, GetMemContent(RVA(0x1AE3A0)) + 0x30 + 0x2B0, GetMemContent(RVA(0x1AE3A0)) + 0x30 + 0x10, 0,
            -2, GetMemContent<float>(RVA(0x20B1D0)), 0.0f);

        //// Gui components creation
        //THOverlay::singleton();
        //THGuiPrac::singleton();
        //
        // Hooks
        THMainHook::singleton().EnableAllHooks();
    }
    static __declspec(noinline) void THInitHookDisable()
    {
        auto& s = THInitHook::singleton();
        s.th19_gui_init_1.Disable();
        s.th19_gui_init_2.Disable();
    }

    // in the part of MainMenu::on_tick responsible for the title screen menu itself
    EHOOK_DY(th19_gui_init_1, 0x1439A2)
    {
        THGuiCreate();
        THInitHookDisable();
    }

    // After InputManager is initialized
    EHOOK_DY(th19_gui_init_2, 0x2A8A)
    {
        THGuiCreate();
        THInitHookDisable();
    }
    HOOKSET_ENDDEF()
}


void TH19Init()
{    
    ingame_image_base = (uintptr_t)GetModuleHandleW(NULL);
    TH19::THInitHook::singleton().EnableAllHooks();
}
}