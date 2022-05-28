#include "thprac_utils.h"

namespace THPrac {
namespace TH09 {
    class THOverlay : public Gui::GameGuiWnd {
        THOverlay() noexcept
        {
            SetTitle("Mod Menu");
            SetFade(0.5f, 0.5f);
            SetCursor(false);
            SetPos(10.0f, 10.0f);
            SetSize(0.0f, 0.0f);
            SetWndFlag(
                ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav | 0);
            OnLocaleChange();
        }
        SINGLETON(THOverlay);
    protected:
        virtual void OnLocaleChange() override
        {
            float x_offset_1 = 0.0f;
            float x_offset_2 = 0.0f;
            switch (Gui::LocaleGet()) {
            case Gui::LOCALE_ZH_CN:
                x_offset_1 = 0.1f;
                x_offset_2 = 0.14f;
                break;
            case Gui::LOCALE_EN_US:
                x_offset_1 = 0.1f;
                x_offset_2 = 0.14f;
                break;
            case Gui::LOCALE_JA_JP:
                x_offset_1 = 0.1f;
                x_offset_2 = 0.14f;
                break;
            default:
                break;
            }
        }
        virtual void OnContentUpdate() override
        {
            mMuteki();
            mInfHealth();
            mInfLives();
            mInfCharge();
        }
        virtual void OnPreUpdate() override
        {
            if (mMenu(false) && !ImGui::IsAnyItemActive()) {
                if (*mMenu) {
                    Open();
                } else {
                    Close();
                    //*((int32_t*)0x6c6eb0) = 2;
                }
            }
            //if (*((int32_t*)0x6c6ea4) == 2)
            //	SetPos(500.0f, 300.0f);
            //else
            //	SetPos(10.0f, 10.0f);
        }

        Gui::GuiHotKey mMenu { "ModMenuToggle", "BACKSPACE", VK_BACK };
        Gui::GuiHotKey mMuteki { TH_MUTEKI, "F1", VK_F1,
            (void*)0x41E8B3, "\x69\xF2\x06", 3,
            (void*)0x48DB20, "\x83\x3D\xA4\x7E\x4A\x00\x04\x74\x05\x83\xFB\x0B\x74\x02\x8B\x01\xC3", 17 };
        Gui::GuiHotKey mInfHealth { TH_INFHEALTH, "F2", VK_F2,
            (void*)0x41E46A, "\x2E\xE9\x70\xF6\x06", 5,
            (void*)0x48DAE0, "\x83\x3D\xA4\x7E\x4A\x00\x04\x74\x0A\x83\x7E\x08\x00\x0F\x84\x49\x0B\xF9\xFF\x8B\x86\xA8\x00\x00\x00\xE9\x72\x09\xF9\xFF", 30 };
        Gui::GuiHotKey mInfLives { TH_INFLIVES, "F3", VK_F3,
            (void*)0x417AAF, "\xEB\x08", 2 };
        Gui::GuiHotKey mInfCharge { TH_INFSPELLCHARGE, "F4", VK_F4,
            (void*)0x41BCEF, "\xE8\x0C\x1E\x07\x00", 5,
            (void*)0x48DB00, "\x83\x3D\xA4\x7E\x4A\x00\x04\x74\x06\x83\x7E\x08\x00\x74\x05\xDF\xE0\xF6\xC4\x01\xC3", 21 };

    public:
        Gui::GuiHotKey mElBgm {
            TH_EL_BGM,
            "F7",
            VK_F7,
        };
    };

    class THAdvOptWnd : public Gui::GameGuiWnd {
        // Option Related Functions
    private:
        void FpsInit()
        {
            mOptCtx.vpatch_base = (int32_t)GetModuleHandleA("vpatch_th09.dll");
            if (mOptCtx.vpatch_base) {
                uint64_t hash[2];
                CalcFileHash("vpatch_th09.dll", hash);
                if (hash[0] != 8777309807944811310ll || hash[1] != 16244273824227920047ll)
                    mOptCtx.fps_status = -1;
                else if (*(int32_t*)(mOptCtx.vpatch_base + 0x17024) == 0) {
                    mOptCtx.fps_status = 2;
                    mOptCtx.fps = *(int32_t*)(mOptCtx.vpatch_base + 0x17034);
                }
            } else
                mOptCtx.fps_status = 0;
        }
        void FpsSet()
        {
            if (mOptCtx.fps_status == 1) {
                mOptCtx.fps_dbl = 1.0 / (double)mOptCtx.fps;
            } else if (mOptCtx.fps_status == 2) {
                *(int32_t*)(mOptCtx.vpatch_base + 0x15a3c) = mOptCtx.fps;
                *(int32_t*)(mOptCtx.vpatch_base + 0x17034) = mOptCtx.fps;
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
            SetTitle(XSTR(TH_SPELL_PRAC));
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
            ImGui::Text(XSTR(TH_ADV_OPT));
            ImGui::Separator();
            ImGui::BeginChild("Adv. Options", ImVec2(0.0f, 0.0f));

            if (BeginOptGroup<TH_GAME_SPEED>()) {
                if (GameFPSOpt(mOptCtx))
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

    __declspec(noinline) void THGuiUpdate()
    {
        GameGuiBegin(IMPL_WIN32_DX8);

        // Gui components update
        THOverlay::singleton().Update();

        GameGuiEnd(IMPL_WIN32_DX8, UpdateAdvOptWindow());
    }

    HOOKSET_DEFINE(THMainHook)
    EHOOK_DY(th09_render, (void*)0x42dd51)
    {
        THGuiUpdate();
    }
    HOOKSET_ENDDEF()

    HOOKSET_DEFINE(THInitHook)
    static __declspec(noinline) void THGuiCreate()
    {
        // Init
        GameGuiInit(IMPL_WIN32_DX8, 0x4b3108, 0x4b30b0, 0x42d3d0,
            Gui::INGAGME_INPUT_GEN2, 0x4acf3a, 0x4acf38, 0,
            -1);

        // Gui components creation
        THOverlay::singleton();

        // Hooks
        THMainHook::singleton().EnableAllHooks();

        // Reset thPracParam
        //thPracParam.Reset();
    }
    static __declspec(noinline) void THInitHookDisable()
    {
        auto& s = THInitHook::singleton();
        s.th09_gui_init_1.Disable();
        s.th09_gui_init_2.Disable();
    }
    EHOOK_DY(th09_gui_init_1, (void*)0x42a0c4)
    {
        THGuiCreate();
        THInitHookDisable();
    }
    EHOOK_DY(th09_gui_init_2, (void*)0x42e3a8)
    {
        THGuiCreate();
        THInitHookDisable();
    }
    HOOKSET_ENDDEF()
}

void TH09Init()
{
    TH09::THInitHook::singleton().EnableAllHooks();
    TryKeepUpRefreshRate((void*)0x42de70);
}
}