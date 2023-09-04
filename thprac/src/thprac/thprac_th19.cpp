#include "thprac_games.h"
#include "thprac_utils.h"
#include "thprac_th19.h"

namespace THPrac {
namespace TH19 {
    enum addrs {
        GLOBALS = 0x207910,
    };

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

    struct TH19Tools : public Gui::GameGuiWnd {
        TH19Tools()
        {
            SetWndFlag(ImGuiWindowFlags_NoCollapse);
            SetFade(0.8f, 0.8f);
            SetStyle(ImGuiStyleVar_WindowRounding, 0.0f);
            SetStyle(ImGuiStyleVar_WindowBorderSize, 0.0f);
            SetAutoSpacing(true);
            OnLocaleChange();
        }
        bool allow = false;

        virtual void OnContentUpdate() override
        {
            ImGui::TextUnformatted("Deez Nuts");
        }
        virtual void OnLocaleChange() override
        {
            SetTitle("Deez Nuts");
            SetSizeRel(0.5, 1);
            SetPosRel(0.5, 0);
        }
        SINGLETON(TH19Tools);
    };

    HOOKSET_DEFINE(TH19PracHook)

    // In the loader thread, right before the instruction that tells the main thread that loading has finished
    EHOOK_DY(th19_prac_init, 0x106878) {
        TH19Tools::singleton().Open();
        TH19Tools::singleton().allow = true;
    }
    
    // In the gamemode switching code where the game switches to main menu mode
    EHOOK_DY(th19_prac_uninit, 0x11FA07) {
        TH19Tools::singleton().Close();
        TH19Tools::singleton().allow = false;
    }

    HOOKSET_ENDDEF()

    struct THGuiPrac : public Gui::GameGuiWnd {
        THGuiPrac()
        {
            SetFade(0.8f, 0.1f);
            SetStyle(ImGuiStyleVar_WindowRounding, 0.0f);
            SetStyle(ImGuiStyleVar_WindowBorderSize, 0.0f);
            SetAutoSpacing(true);
            OnLocaleChange();
        }

    public:
        Gui::GuiCombo mMode { TH_MODE, TH_MODE_SELECT };
        SINGLETON(THGuiPrac);

    protected:
        virtual void OnLocaleChange() override
        {
            SetTitle(S(TH_MENU));
            SetSizeRel(0.3125f, 0.075f);
            SetPosRel(0.34375f, 0.15f);
            SetItemWidthRel(-0.052f);
        }
        virtual void OnContentUpdate() override { mMode(); }
    };

    PATCH_ST(th19_vs_mode_disable_movement, 0x142131, "\xeb", 1);

    HOOKSET_DEFINE(THMainHook)

    EHOOK_DY(th19_update_begin, 0xC89E0) {
        GameGuiBegin(IMPL_WIN32_DX9);
    }

    EHOOK_DY(th19_update_end, 0xC8B75) {
        auto& p = THGuiPrac::singleton();
        p.Update();

        auto& t = TH19Tools::singleton();
        if (t.allow) {
            if (Gui::KeyboardInputUpdate(VK_F11) == 1) {
                if (t.IsOpen()) {
                    t.Close();
                } else {
                    t.Open();
                }
            }
            t.Update();
        }
        
        GameGuiEnd(UpdateAdvOptWindow() || p.IsOpen() || t.IsOpen());
    }

    EHOOK_DY(th19_render, 0xC8C8D) {
        GameGuiRender(IMPL_WIN32_DX9);
    }

    EHOOK_DY(th19_vs_mode_enter, 0x14220F)
    {
        auto& p = THGuiPrac::singleton();

        if (p.IsClosed()) {
            p.Open();
            pCtx->Eip = RVA(0x14231C);
            th19_vs_mode_disable_movement.Enable();
            TH19PracHook::singleton().DisableAllHooks();
            return;
        }

        p.Close();
        if (*p.mMode) {
            TH19PracHook::singleton().EnableAllHooks();
        }
        th19_vs_mode_disable_movement.Disable();
    }
    EHOOK_DY(th19_vs_mode_exit, 0x1421CB) {
        TH19PracHook::singleton().DisableAllHooks();
        auto& p = THGuiPrac::singleton();
        if (p.IsClosed()) {
            return;
        }
        p.Close();
        th19_vs_mode_disable_movement.Disable();
        pCtx->Eip = RVA(0x142204);
    }

    HOOKSET_ENDDEF()

    HOOKSET_DEFINE(THInitHook)
    static __declspec(noinline) void THGuiCreate()
    {
        th19_vs_mode_disable_movement.Setup();

        // Init
        GameGuiInit(IMPL_WIN32_DX9, RVA(0x208388), RVA(0x209110), RVA(0xA9EE0),
            Gui::INGAGME_INPUT_GEN2, GetMemContent(RVA(0x1AE3A0)) + 0x30 + 0x2B0, GetMemContent(RVA(0x1AE3A0)) + 0x30 + 0x10, 0,
            -2, GetMemContent<float>(RVA(0x20B1D0)), 0.0f);

        //// Gui components creation
        //THOverlay::singleton();
        THGuiPrac::singleton();
        TH19Tools::singleton();
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