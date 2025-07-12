#include "thprac_games.h"
#include "thprac_utils.h"


namespace THPrac {
namespace TH095 {
    class THOverlay : public Gui::GameGuiWnd {
        THOverlay() noexcept
        {
            SetTitle("Mod Menu");
            SetFade(0.5f, 0.5f);
            SetPos(10.0f, 10.0f);
            SetSize(0.0f, 0.0f);
            SetWndFlag(
                ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav | 0);
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

            mMenu.SetTextOffsetRel(x_offset_1, x_offset_2);
            mMuteki.SetTextOffsetRel(x_offset_1, x_offset_2);
            mInfCharge.SetTextOffsetRel(x_offset_1, x_offset_2);
            mFocusLockOn.SetTextOffsetRel(x_offset_1, x_offset_2);
            mTimeLock.SetTextOffsetRel(x_offset_1, x_offset_2);
            mElBgm.SetTextOffsetRel(x_offset_1, x_offset_2);
        }
        virtual void OnContentUpdate() override
        {
            mMuteki();
            mInfCharge();
            mFocusLockOn();
            mTimeLock();
        }
        virtual void OnPreUpdate() override
        {
            if (mMenu(false) && !ImGui::IsAnyItemActive()) {
                if (*mMenu) {
                    Open();
                } else {
                    Close();
                }
            }
        }

        Gui::GuiHotKey mMenu { "ModMenuToggle", "BACKSPACE", VK_BACK };
        HOTKEY_DEFINE(mMuteki, TH_MUTEKI, "F1", VK_F1)
        PATCH_HK(0x4306DE, "01"),
        PATCH_HK(0x4307BB, "80"),
        PATCH_HK(0x43070d, "83c40c9090")
        HOTKEY_ENDDEF();

        HOTKEY_DEFINE(mInfCharge, TH_INFCHARGE, "F2", VK_F2)
        PATCH_HK(0x433EE2, "00")
        HOTKEY_ENDDEF();
    public:
        HOTKEY_DEFINE(mFocusLockOn, TH_COERCIVE, "F3", VK_F3)
        PATCH_HK(0x432ee4, "909090909090"),
        PATCH_HK(0x431cf2, "909090909090"),
        PATCH_HK(0x432f7e, "00")
        HOTKEY_ENDDEF();
    private:
        HOTKEY_DEFINE(mTimeLock, TH_TIMELOCK, "F4", VK_F4)
        PATCH_HK(0x418317, "2EE9")
        HOTKEY_ENDDEF();


    public:
        Gui::GuiHotKey mElBgm { TH_EL_BGM, "F7", VK_F7 };
    };

    class THAdvOptWnd : public Gui::GameGuiWnd {
        // Option Related Functions
    private:
        void FpsInit()
        {
            mOptCtx.vpatch_base = (int32_t)GetModuleHandleW(L"vpatch_th095.dll");
            if (mOptCtx.vpatch_base) {
                uint64_t hash[2];
                CalcFileHash(L"vpatch_th095.dll", hash);
                if (hash[0] != 11971022730696137433ll || hash[1] != 6069366661305861631ll)
                    mOptCtx.fps_status = -1;
                else if (*(int32_t*)(mOptCtx.vpatch_base + 0x1c024) == 0) {
                    mOptCtx.fps_status = 2;
                    mOptCtx.fps = *(int32_t*)(mOptCtx.vpatch_base + 0x1c034);
                }
            } else
                mOptCtx.fps_status = 0;
        }
        void FpsSet()
        {
            if (mOptCtx.fps_status == 1) {
                mOptCtx.fps_dbl = 1.0 / (double)mOptCtx.fps;
            } else if (mOptCtx.fps_status == 2) {
                *(int32_t*)(mOptCtx.vpatch_base + 0x19adc) = mOptCtx.fps;
                *(int32_t*)(mOptCtx.vpatch_base + 0x1c034) = mOptCtx.fps;
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
            SetTitle(S(TH_SPELL_PRAC));
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

        GameGuiEnd(UpdateAdvOptWindow());
        GameGuiRender(IMPL_WIN32_DX8);
    }

    HOOKSET_DEFINE(THMainHook)
    EHOOK_DY(th095_homing, 0x40498e, 3, {
        float* view_pos = (float*)pCtx->Edx;
        float* crosshair_pos = *(float**)(pCtx->Esp + 0x90);
        float* boss_pos = (float*)GetMemAddr(0x4bddc0, 0x76c4);
        if (*THOverlay::singleton().mFocusLockOn) {
            crosshair_pos[0] = view_pos[0] = boss_pos[0];
            crosshair_pos[1] = view_pos[1] = boss_pos[1];
            crosshair_pos[2] = view_pos[2] = boss_pos[2];
        }
    })
    EHOOK_DY(th095_render, 0x4208d6, 6, {
        THGuiUpdate();
    })
    HOOKSET_ENDDEF()


    static __declspec(noinline) void THGuiCreate()
    {
        if (ImGui::GetCurrentContext()) {
            return;
        }
        // Init
        GameGuiInit(IMPL_WIN32_DX8, 0x4c4678, 0x4c45e8,
            Gui::INGAGME_INPUT_GEN2, 0x4be21e, 0x4be21c, 0,
            -1);

        SetDpadHook(0x419725, 3);

        // Gui components creation
        THOverlay::singleton();

        // Hooks
        EnableAllHooks(THMainHook);
    }
    HOOKSET_DEFINE(THInitHook)
    EHOOK_DY(th095_gui_init_1, 0x4470a1, 7, {
        self->Disable();
        THGuiCreate();
    })
    EHOOK_DY(th095_gui_init_2, 0x42137e, 1, {
        self->Disable();
        THGuiCreate();
    })
    HOOKSET_ENDDEF()
}

void TH095Init()
{
    EnableAllHooks(TH095::THInitHook);
}
}
