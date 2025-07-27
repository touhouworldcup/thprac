#include "thprac_games.h"
#include "thprac_utils.h"


namespace THPrac {
namespace TH125 {
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
            case LOCALE_ZH_CN:
                x_offset_1 = 0.1f;
                x_offset_2 = 0.14f;
                break;
            case LOCALE_EN_US:
                x_offset_1 = 0.1f;
                x_offset_2 = 0.14f;
                break;
            case LOCALE_JA_JP:
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

        Gui::GuiHotKeyChord mMenu { "ModMenuToggle", "BACKSPACE", Gui::GetBackspaceMenuChord() };
        HOTKEY_DEFINE(mMuteki, TH_MUTEKI, "F1", VK_F1)
        PATCH_HK(0x436C2C, "01"),
        PATCH_HK(0x436DF8, "eb19"),
        PATCH_HK(0x436c76, "83c4109090")
        HOTKEY_ENDDEF();

        HOTKEY_DEFINE(mInfCharge, TH_INFCHARGE, "F2", VK_F2)
        PATCH_HK(0x43A0EA, "00")
        HOTKEY_ENDDEF();
    public:
        HOTKEY_DEFINE(mFocusLockOn, TH_COERCIVE, "F3", VK_F3)
        PATCH_HK(0x438eb9, "909090909090"),
        PATCH_HK(0x438ec9, "909090909090"),
        PATCH_HK(0x4379c5, "909090909090"),
        PATCH_HK(0x438f66, "00")
        HOTKEY_ENDDEF();
    private:
        HOTKEY_DEFINE(mTimeLock, TH_TIMELOCK, "F4", VK_F4)
        PATCH_HK(0x41DEDA, "eb")
        HOTKEY_ENDDEF();
    public:
        Gui::GuiHotKey mElBgm { TH_EL_BGM, "F7", VK_F7 };
    };

    PATCH_ST(th125_hiscore_fix, 0x42ea14, "90909090909090");

    class THAdvOptWnd : public Gui::GameGuiWnd {
        // Option Related Functions
    private:
        void FpsInit()
        {
            if (mOptCtx.vpatch_base = (uintptr_t)GetModuleHandleW(L"openinputlagpatch.dll")) {
                OILPInit(mOptCtx);
            } else if (mOptCtx.vpatch_base = (uintptr_t)GetModuleHandleW(L"vpatch_th125.dll")) {
                uint64_t hash[2];
                CalcFileHash(L"vpatch_th125.dll", hash);
                if (hash[0] != 10688862875744720998ll || hash[1] != 16306494719441951827ll)
                    mOptCtx.fps_status = -1;
                else if (*(int32_t*)(mOptCtx.vpatch_base + 0x1b024) == 0) {
                    mOptCtx.fps_status = 2;
                    mOptCtx.fps = *(int32_t*)(mOptCtx.vpatch_base + 0x1b034);
                }
            } else if (*(uint8_t*)0x4d0ebb == 3) {
                mOptCtx.fps_status = 1;

                DWORD oldProtect;
                VirtualProtect((void*)0x44f149, 4, PAGE_EXECUTE_READWRITE, &oldProtect);
                *(double**)0x44f149 = &mOptCtx.fps_dbl;
                VirtualProtect((void*)0x44f149, 4, oldProtect, &oldProtect);
            } else
                mOptCtx.fps_status = 0;
        }
        void FpsSet()
        {
            if (mOptCtx.fps_status == 3) {
                mOptCtx.oilp_set_game_fps(mOptCtx.fps);
                mOptCtx.oilp_set_replay_skip_fps(mOptCtx.fps_replay_fast);
                mOptCtx.oilp_set_replay_slow_fps(mOptCtx.fps_replay_slow);
            } else if (mOptCtx.fps_status == 1) {
                mOptCtx.fps_dbl = 1.0 / (double)mOptCtx.fps;
            } else if (mOptCtx.fps_status == 2) {
                *(int32_t*)(mOptCtx.vpatch_base + 0x18abc) = mOptCtx.fps;
                *(int32_t*)(mOptCtx.vpatch_base + 0x1b034) = mOptCtx.fps;
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

            th125_hiscore_fix.Setup();

            FpsInit();
            GameplayInit();
        }

    protected:
        virtual void OnLocaleChange() override
        {
            SetTitle(S(TH_SPELL_PRAC));
            switch (Gui::LocaleGet()) {
            case LOCALE_ZH_CN:
                SetSizeRel(1.0f, 1.0f);
                SetPosRel(0.0f, 0.0f);
                SetItemWidthRel(-0.075f);
                SetAutoSpacing(true);
                break;
            case LOCALE_EN_US:
                SetSizeRel(1.0f, 1.0f);
                SetPosRel(0.0f, 0.0f);
                SetItemWidthRel(-0.075f);
                SetAutoSpacing(true);
                break;
            case LOCALE_JA_JP:
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

            if (BeginOptGroup<TH18_BUG_FIX>()) {
                if (ImGui::Checkbox(S(TH125_SPOILER_HISCORE_FIX), &mHiscoreFix)) {
                    th125_hiscore_fix.Toggle(mHiscoreFix);
                }

                EndOptGroup();
            }

            AboutOpt();
            ImGui::EndChild();
            ImGui::SetWindowFocus();
        }

        adv_opt_ctx mOptCtx;

        bool mHiscoreFix = false;
    };
    bool UpdateAdvOptWindow()
    {
        static THAdvOptWnd* advOptWnd = nullptr;
        if (!advOptWnd)
            advOptWnd = new THAdvOptWnd();
        if (Gui::GetChordPressed(Gui::GetAdvancedMenuChord())) {
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
        GameGuiBegin(IMPL_WIN32_DX9);

        // Gui components update
        THOverlay::singleton().Update();

        GameGuiEnd(UpdateAdvOptWindow());
        GameGuiRender(IMPL_WIN32_DX9);
    }

    HOOKSET_DEFINE(THMainHook)
    EHOOK_DY(th125_homing, 0x438714, 6, {
        float* crosshair_pos;
        float* boss_pos;

        if (*THOverlay::singleton().mFocusLockOn)
        {
            if (GetMemContent(0x4B678C, 0x54)) {

                crosshair_pos = (float*)GetMemAddr(0x4b68c8, 0x2484 + 0x13cc);
                boss_pos = (float*)GetMemAddr(0x4B678C, 0x54, 0x11c0 + 0x34);

                crosshair_pos[0] = boss_pos[0];
                crosshair_pos[1] = boss_pos[1];
                crosshair_pos[2] = boss_pos[2];
            }
        }
    })
    EHOOK_DY(th125_render_1, 0x44ee42, 5, {
        THGuiUpdate();
    })
    EHOOK_DY(th125_render_2, 0x44f255, 5, {
        THGuiUpdate();
    })
    EHOOK_DY(th125_render_3, 0x44f3fd, 5, {
        THGuiUpdate();
    })
    HOOKSET_ENDDEF()

    static __declspec(noinline) void THGuiCreate()
    {
        if (ImGui::GetCurrentContext()) {
            return;
        }
        // Init
        GameGuiInit(IMPL_WIN32_DX9, 0x4d0cd8, 0x4d17d8,
            Gui::INGAGME_INPUT_GEN2, 0x4d8dac, 0x4d8da8, 0,
            -1);

        SetDpadHook(0x46205F, 2);

        // Gui components creation
        THOverlay::singleton();

        // Hooks
        EnableAllHooks(THMainHook);

    }
    HOOKSET_DEFINE(THInitHook)
    PATCH_DY(th125_startup_1, 0x440657, "eb")
    PATCH_DY(th125_startup_2, 0x4418a6, "eb")
    EHOOK_DY(th125_gui_init_1, 0x441905, 3, {
        self->Disable();
        THGuiCreate();
    })
    EHOOK_DY(th125_gui_init_2, 0x44ff39, 1, {
        self->Disable();
        THGuiCreate();
    })
    HOOKSET_ENDDEF()
}

void TH125Init()
{
    EnableAllHooks(TH125::THInitHook);
}
}
