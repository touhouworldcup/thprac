#include "thprac_games.h"
#include "thprac_utils.h"


namespace THPrac {
namespace TH095 {
    enum ADDRS {
        PHOTO_MANAGER_ADDR = 0x4C4E6C,
        ANMVM_SETUP_SPRITE_AND_MATRICES = 0x439E30
    };

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

        Gui::GuiHotKeyChord mMenu { "ModMenuToggle", "BACKSPACE", hotkeys.backspace_menu };
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

    HOOKSET_DEFINE(PhotoScoreDisplayFix)
    // Correctly compute the sixth and seventh least significant decimal digit, and set the AnmVM of it
    EHOOK_DY(th095_photo_score_display_fix1, 0x42C655, 4, {
        int photo_score = *(int*)(pCtx->Ebp + 0x8);
        BOOL *have_encountered_a_nonzero_digit = (BOOL*)(pCtx->Ebp - 0x34);
        uint8_t *this_ = *(uint8_t**)(pCtx->Ebp - 0x140);
        void* unknown_struct_ptr = GetMemContent<void*>(PHOTO_MANAGER_ADDR, 0x2571C);

        *have_encountered_a_nonzero_digit = (photo_score / 1'000'000) != 0;
        if (*have_encountered_a_nonzero_digit) {
            asm_call<0x404B80, Thiscall>(unknown_struct_ptr, (uint32_t*)(this_ + 0x245C), (int)(0x1E));
            asm_call<ANMVM_SETUP_SPRITE_AND_MATRICES, Thiscall>(unknown_struct_ptr, (int)(this_ + 0x245C), 
                                                                (int)(photo_score / 1'000'000 + 0xF));
        }

        *(float*)(this_ + 0x25B0) = *(float*)(pCtx->Ebp - 0x1C) - 9.0f;
        *(float*)(this_ + 0x25B4) = *(float*)(pCtx->Ebp - 0x18);
        *(float*)(this_ + 0x25B8) = *(float*)(pCtx->Ebp - 0x14);

        int sixth_digit = (photo_score / 100'000) % 10;
        if (sixth_digit != 0 || *have_encountered_a_nonzero_digit) {
            asm_call<0x404B80, Thiscall>(unknown_struct_ptr, (uint32_t*)(this_ + 0x10C8), (int)(0x1E));
            asm_call<ANMVM_SETUP_SPRITE_AND_MATRICES, Thiscall>(unknown_struct_ptr, (int)(this_ + 0x10C8), 
                                                                (int)(sixth_digit + 0xF));
            *have_encountered_a_nonzero_digit = true;
        }
        
        pCtx->Eip = 0x42C6A5;
    })
    // Display the AnmVM that is set above 
    PATCH_DY(th095_photo_score_display_fix2, 0x42C2E6, "837DE808")
    EHOOK_DY(th095_photo_score_display_fix3, 0x42C2F0, 3, {
        if (*(int*)(pCtx->Ebp - 0x18) == 6) {
            pCtx->Eip = 0x42C2DD;   
        }
    })
    // Hide the tens digit when the score is < 10 
    EHOOK_DY(th095_photo_score_display_fix4, 0x42C88D, 3, {
        int photo_score = *(int*)(pCtx->Ebp + 0x8);
        if (photo_score < 10) {
            pCtx->Eip = 0x42C8E6;
        }
    })
    HOOKSET_ENDDEF()

    class THAdvOptWnd : public Gui::GameGuiWnd {
        // Option Related Functions
    private:
        void FpsInit()
        {
            if (mOptCtx.vpatch_base = (uintptr_t)GetModuleHandleW(L"openinputlagpatch.dll")) {
                OILPInit(mOptCtx);
            } else if (mOptCtx.vpatch_base = (uintptr_t)GetModuleHandleW(L"vpatch_th095.dll")) {
                uint64_t hash[2];
                CalcFileHash(L"vpatch_th095.dll", hash);
                if (hash[0] != 11971022730696137433ull || hash[1] != 6069366661305861631ull)
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
            if (mOptCtx.fps_status == 3) {
                mOptCtx.oilp_set_game_fps(mOptCtx.fps);
                mOptCtx.oilp_set_replay_skip_fps(mOptCtx.fps_replay_fast);
                mOptCtx.oilp_set_replay_slow_fps(mOptCtx.fps_replay_slow);
            } else if (mOptCtx.fps_status == 1) {
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
            if (BeginOptGroup<TH_GAMEPLAY>()) {                
                if (ImGui::Checkbox(S(TH095_FIX_PHOTO_SCORE_DISPLAY), &mFixPhotoScoreDisplay)) {
                    if (mFixPhotoScoreDisplay) {
                        EnableAllHooks(PhotoScoreDisplayFix);
                    } else {
                        DisableAllHooks(PhotoScoreDisplayFix);
                    }
                }
                ImGui::SameLine();
                Gui::HelpMarker(S(TH095_FIX_PHOTO_SCORE_DISPLAY_DESC));

                EndOptGroup();
            }

            AboutOpt();
            ImGui::EndChild();
            ImGui::SetWindowFocus();
        }

        adv_opt_ctx mOptCtx;

    public:
        bool mFixPhotoScoreDisplay = false;
    };
    bool UpdateAdvOptWindow()
    {
        static THAdvOptWnd* advOptWnd = nullptr;
        if (!advOptWnd)
            advOptWnd = new THAdvOptWnd();
        if (Gui::GetChordPressed(hotkeys.advanced_menu)) {
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
            1.0f);

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
    DisableAllHooks(TH095::PhotoScoreDisplayFix);
}
}
