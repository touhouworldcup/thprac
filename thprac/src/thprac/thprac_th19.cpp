#include "thprac_games.h"
#include "thprac_utils.h"

namespace THPrac {
namespace TH19 {
    class THAdvOptWnd : public Gui::GameGuiWnd {
        // Option Related Functions
    private:
        void FpsInit()
        {
            if (*(uint8_t*)RVA(0x208621) == 3) {
                mOptCtx.fps_status = 1;

                DWORD oldProtect;
                VirtualProtect((void*)RVA(0xA95B6), 4, PAGE_EXECUTE_READWRITE, &oldProtect);
                *(double**)RVA(0xA95B6) = &mOptCtx.fps_dbl;
                VirtualProtect((void*)RVA(0xA95B6), 4, oldProtect, &oldProtect);
            } else
                mOptCtx.fps_status = 0;
        }
        void FpsSet()
        {
            if (mOptCtx.fps_status == 1) {
                mOptCtx.fps_dbl = 1.0 / (double)mOptCtx.fps;
            }
        }
        void GameplayInit()
        {
        }
        void GameplaySet()
        {
        }

    public:
        THAdvOptWnd() noexcept
        {
            SetWndFlag(ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove);
            SetFade(0.8f, 0.8f);
            SetStyle(ImGuiStyleVar_WindowRounding, 0.0f);
            SetStyle(ImGuiStyleVar_WindowBorderSize, 0.0f);
            OnLocaleChange();

            FpsInit();
            GameplayInit();
        }

    protected:
        virtual void OnLocaleChange() override
        {
            SetTitle(S(TH_ADV_OPT));
            switch (Gui::LocaleGet()) {
            case Gui::LOCALE_ZH_CN:
                SetSizeRel(1.0f, 1.0f);
                SetPosRel(0.0f, 0.0f);
                SetItemWidthRel(-0.075f);
                SetAutoSpacing(true);
                break;
            case Gui::LOCALE_EN_US:
                SetSizeRel(1.0f, 1.0f);
                SetPosRel(0.0f, 0.0f);
                SetItemWidthRel(-0.075f);
                SetAutoSpacing(true);
                break;
            case Gui::LOCALE_JA_JP:
                SetSizeRel(1.0f, 1.0f);
                SetPosRel(0.0f, 0.0f);
                SetItemWidthRel(-0.075f);
                SetAutoSpacing(true);
                break;
            default:
                break;
            }
        }
        virtual void OnContentUpdate() override
        {
            ImGui::TextUnformatted(S(TH_ADV_OPT));
            ImGui::Separator();
            ImGui::BeginChild("Adv. Options", ImVec2(0.0f, 0.0f));

            if (BeginOptGroup<TH_GAME_SPEED>()) {
                if (GameFPSOpt(mOptCtx, false))
                    FpsSet();
                EndOptGroup();
            }

            AboutOpt();
            ImGui::EndChild();
            ImGui::SetWindowFocus();
        }

        adv_opt_ctx mOptCtx;
    };
    bool UpdateAdvOptWindow()
    {
        static THAdvOptWnd* advOptWnd = nullptr;
        if (!advOptWnd)
            advOptWnd = new THAdvOptWnd();
        if (Gui::KeyboardInputUpdate(VK_F12) == 1) {
            if (advOptWnd->IsOpen())
                advOptWnd->Close();
            else
                advOptWnd->Open();
        }
        advOptWnd->Update();

        return advOptWnd->IsOpen();
    }

    HOOKSET_DEFINE(THMainHook)

    EHOOK_DY(th19_update_begin, 0xC89E0) {
        GameGuiBegin(IMPL_WIN32_DX9);
    }

    EHOOK_DY(th19_update_end, 0xC8B75) {
        GameGuiEnd(UpdateAdvOptWindow());
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