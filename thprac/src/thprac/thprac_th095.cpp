#include "thprac_th095.h"

#include <cmath>
#include "thprac_games.h"
#include "thprac_utils.h"


namespace THPrac {
namespace TH095 {
    enum ADDRS {
        PHOTO_MANAGER_ADDR = 0x4C4E6C,
        PLAYER_PTR = 0x4C4E70,
        ANMVM_SETUP_SPRITE_AND_MATRICES = 0x439E30,
        ANMVM_RUN = 0x43A600
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

    AnmVm photoScoreSeventhDigitVms[10];

    HOOKSET_DEFINE(PhotoScoreDisplayFix)
    // Clean the VMs each time the constructor of PhotoManager is called
    EHOOK_DY(th095_photo_score_display_fix0, 0x42A931, 3, {
        memset(photoScoreSeventhDigitVms, 0x00, sizeof(photoScoreSeventhDigitVms));
    })
    // Correctly compute the sixth and seventh least significant decimal digit, and set the AnmVM of it
    EHOOK_DY(th095_photo_score_display_fix1, 0x42C655, 4, {
        int photo_score = *(int*)(pCtx->Ebp + 0x8);
        BOOL* have_encountered_a_nonzero_digit = (BOOL*)(pCtx->Ebp - 0x34);
        struct PhotoManagerArrayItem* this_ = *(struct PhotoManagerArrayItem**)(pCtx->Ebp - 0x140);
        void* unknown_struct_ptr = GetMemContent<void*>(PHOTO_MANAGER_ADDR, 0x2571C);

        struct BonusManager* bonus_manager_ptr = (struct BonusManager*)(*(intptr_t*)(PLAYER_PTR) + 0x1E3C);
        int* existing_photo_cnt = (int*)((intptr_t)(bonus_manager_ptr) + 0xBA8);
        AnmVm* score_digit_vms = (AnmVm*)((intptr_t)(this_) + 0x10C8);
        AnmVm* seventh_digit_vm = &photoScoreSeventhDigitVms[*existing_photo_cnt];
        Float3* cur_digit_pos = (Float3*)(pCtx->Ebp - 0x1C);

        *have_encountered_a_nonzero_digit = (photo_score / 1'000'000) != 0;
        if (*have_encountered_a_nonzero_digit) {
            asm_call<0x404B80, Thiscall>(unknown_struct_ptr, seventh_digit_vm, (int)(0x1E));
            asm_call<ANMVM_SETUP_SPRITE_AND_MATRICES, Thiscall>(unknown_struct_ptr, seventh_digit_vm, 
                                                                photo_score / 1'000'000 + ANM_ID_DIGIT_0);
        }
        seventh_digit_vm->entity_pos.x = cur_digit_pos->x - 9.0f;
        seventh_digit_vm->entity_pos.y = cur_digit_pos->y;
        seventh_digit_vm->entity_pos.z = cur_digit_pos->z;
        
        int sixth_digit = (photo_score / 100'000) % 10;
        if (sixth_digit != 0 || *have_encountered_a_nonzero_digit) {
            asm_call<0x404B80, Thiscall>(unknown_struct_ptr, &score_digit_vms[0], (int)(0x1E));
            asm_call<ANMVM_SETUP_SPRITE_AND_MATRICES, Thiscall>(unknown_struct_ptr, &score_digit_vms[0], 
                                                                sixth_digit + ANM_ID_DIGIT_0);
            *have_encountered_a_nonzero_digit = true;
        }
        
        pCtx->Eip = 0x42C6A5;
    })
    // Clean photoScoreSeventhDigitVms in the constructor of PhotoManager
    EHOOK_DY(th095_photo_score_display_fix2, 0x42A921, 6, {
        memset(photoScoreSeventhDigitVms, 0x00, sizeof(photoScoreSeventhDigitVms));
    })
    // Display the AnmVM that is set above
    EHOOK_DY(th095_photo_score_display_fix3, 0x42C3C8, 5, {
        struct PhotoManager* this_ = *(struct PhotoManager**)(pCtx->Ebp - 0x1C);
        struct PhotoManagerArrayItem* array = (struct PhotoManagerArrayItem*)((intptr_t)(this_) + 0x44);

        for (int i = 0; i < 10; ++i) {
            AnmVm* ones_digit_vm = (AnmVm*)((intptr_t)(array) + i * 0x2214 + 0x10C8) + 5;
            AnmVm* seventh_digit_vm = &photoScoreSeventhDigitVms[i];
            seventh_digit_vm->color_1 = ones_digit_vm->color_1;
            asm_call<0x4452D0, Fastcall>(seventh_digit_vm);
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

    AnmVm extraBonusVms[50];
    int extraBonusVmsSize = 0;
    struct BonusInfo {
        int basePoint, resultScore;
        float baseFactor;
    } bonusInfo;

    void SetExtraBonusVmColor(uint8_t blue_goal) {
        for (int i = 0; i < extraBonusVmsSize; ++i) {
            if (extraBonusVms[i].int_var_9 != 0) {
                extraBonusVms[i].interp_cur_times.alpha_1.current = 0;
                extraBonusVms[i].interp_cur_times.alpha_1.current_f = 0.0f;
                extraBonusVms[i].interp_cur_times.alpha_1.previous = -999'999;
                extraBonusVms[i].interp_end_times.alpha_1.current = 16;
                extraBonusVms[i].interp_end_times.alpha_1.current_f = 16.0f;
                extraBonusVms[i].interp_end_times.alpha_1.previous = -999'999;
                extraBonusVms[i].interp_modes.alpha_1 = 0;
                *((uint8_t*)(&extraBonusVms[i].interp_data.color_1_initial) + 3) = 
                    *((uint8_t*)(&extraBonusVms[i].color_1) + 3);
                *((uint8_t*)(&extraBonusVms[i].interp_data.color_1_goal) + 3) = blue_goal;
            }
        }
    }
    void SetIntegerVm(int value, Float3* cur_drawing_pos, int target_int_vars_zero) {
        bool has_started_printing = false;
        for (int divisor = 1'000'000; divisor != 0; divisor /= 10) {
            int cur_digit = (value % (divisor * 10)) / divisor;
            if (has_started_printing || (cur_digit != 0) || (divisor == 1)) {
                has_started_printing = true;
                asm_call<0x42E730, Fastcall>(&extraBonusVms[extraBonusVmsSize++], cur_drawing_pos,
                                             cur_digit + ANM_ID_DIGIT_0, target_int_vars_zero);
            }
            cur_drawing_pos->x += 9.0f;
        }
    }
    void AddBonusLastDecimal(PCONTEXT const& pCtx, int digit) {
        Float3* cur_drawing_pos = (Float3*)(pCtx->Ebp - 0x10);
        int* cur_vm_idx = (int*)(pCtx->Ebp - 0x4);
        int* target_int_vars_zero = (int*)(pCtx->Ebp - 0x2C);
        AnmVm* bonus_vms = (AnmVm*)(*(intptr_t*)(PHOTO_MANAGER_ADDR) + 0x1774C);
        cur_drawing_pos->x += 9.0f;
        asm_call<0x42E730, Fastcall>(&bonus_vms[*cur_vm_idx], cur_drawing_pos, digit + ANM_ID_DIGIT_0,
                                     *target_int_vars_zero);
        (*cur_vm_idx)++;
    }

    HOOKSET_DEFINE(DsStyledBonusDisplay)
    // Clean our VM when the constructor of PhotoManager is called
    EHOOK_DY(th095_ds_styled_bonus_display0, 0x42A934, 7, {
        memset(extraBonusVms, 0x00, sizeof(extraBonusVms));
        extraBonusVmsSize = 0;
    })
    // Set the extra info to be displayed
    EHOOK_DY(th095_ds_styled_bonus_display1, 0x4339E3, 2, {
        ScoreResType* score_res = *(ScoreResType**)(pCtx->Ebp + 0xC);
        bonusInfo.baseFactor = *(float*)(*(intptr_t*)(PHOTO_MANAGER_ADDR) + 0x25718);
        bonusInfo.basePoint = score_res->base_point;
        bonusInfo.resultScore = score_res->final_score;
    })
    // Set the display of "Risk Shot" always being 0
    PATCH_DY(th095_ds_styled_bonus_display2_1, 0x42E045, "EB50")
    PATCH_DY(th095_ds_styled_bonus_display2_2, 0x42E0AA, "EB51")
    PATCH_DY(th095_ds_styled_bonus_display2_3, 0x42E110, "EB51")
    // Clear extraBonusVms and set the VM of Base Point
    EHOOK_DY(th095_ds_styled_bonus_display3, 0x42C9A2, 7, {
        Float3* cur_drawing_pos = (Float3*)(pCtx->Ebp - 0x10);
        float* start_drawing_x = (float*)(pCtx->Ebp - 0x28);
        int* target_int_vars_zero = (int*)(pCtx->Ebp - 0x2C);

        cur_drawing_pos->x += 63.0f;
        SetIntegerVm(bonusInfo.basePoint, cur_drawing_pos, *target_int_vars_zero);
        cur_drawing_pos->y += 12.0f;
        cur_drawing_pos->x = *start_drawing_x;
    })
    // Set the VM of Base Factor and Result Score
    EHOOK_DY(th095_ds_styled_bonus_display4, 0x42E70B, 6, {
        Float3* cur_drawing_pos = (Float3*)(pCtx->Ebp - 0x10);
        float* start_drawing_x = (float*)(pCtx->Ebp - 0x28);
        int* target_int_vars_zero = (int*)(pCtx->Ebp - 0x2C);

        // Set the VM of Base Factor
        if (std::abs(bonusInfo.baseFactor - 1.0f) > 1e-6) {
            cur_drawing_pos->x += 36.0f;
            SetIntegerVm((int)(bonusInfo.baseFactor), cur_drawing_pos, *target_int_vars_zero);
            asm_call<0x42E730, Fastcall>(&extraBonusVms[extraBonusVmsSize++], cur_drawing_pos, ANM_ID_DECIMAL_POINT,
                                         *target_int_vars_zero);
            cur_drawing_pos->x += 9.0f;
            asm_call<0x42E730, Fastcall>(&extraBonusVms[extraBonusVmsSize++], cur_drawing_pos, 
                                         ((int)(bonusInfo.baseFactor * 10) % 10) + ANM_ID_DIGIT_0,
                                         *target_int_vars_zero);
            cur_drawing_pos->x += 9.0f;
            asm_call<0x42E730, Fastcall>(&extraBonusVms[extraBonusVmsSize++], cur_drawing_pos, 
                                         ((int)(bonusInfo.baseFactor * 100) % 10) + ANM_ID_DIGIT_0,
                                         *target_int_vars_zero);
            cur_drawing_pos->y += 12.0f;
            cur_drawing_pos->x = *start_drawing_x;
        }

        // Set the VM of Result Score
        cur_drawing_pos->x += 63.0f;
        SetIntegerVm(bonusInfo.resultScore, cur_drawing_pos, *target_int_vars_zero);
        cur_drawing_pos->y += 12.0f;
        cur_drawing_pos->x = *start_drawing_x;
    })
    // Modify the procedure which sets the VMs of Boss Shot
    EHOOK_DY(th095_ds_styled_bonus_display5_1, 0x42E227, 3, {
        Float3* cur_drawing_pos = (Float3*)(pCtx->Ebp - 0x10);
        cur_drawing_pos->x -= 9.0f;
    })
    EHOOK_DY(th095_ds_styled_bonus_display5_2, 0x42E31F, 3, {
        ScoreResType* score_res = *(ScoreResType**)(pCtx->Ebp + 0x14);
        AddBonusLastDecimal(pCtx, (int)(score_res->boss_shot_multiplier * 100) % 10);
    })
    // Modify the procedure which sets the VMs of Self Shot
    EHOOK_DY(th095_ds_styled_bonus_display6_1, 0x42E390, 3, {
        Float3* cur_drawing_pos = (Float3*)(pCtx->Ebp - 0x10);
        cur_drawing_pos->x -= 9.0f;
    })
    EHOOK_DY(th095_ds_styled_bonus_display6_2, 0x42E452, 3, {
        AddBonusLastDecimal(pCtx, 0);
    })
    // Modify the procedure which sets the VMs of Two Shot
    EHOOK_DY(th095_ds_styled_bonus_display7_1, 0x42E4C4, 3, {
        Float3* cur_drawing_pos = (Float3*)(pCtx->Ebp - 0x10);
        cur_drawing_pos->x -= 9.0f;
    })
    EHOOK_DY(th095_ds_styled_bonus_display7_2, 0x42E586, 3, {
        AddBonusLastDecimal(pCtx, 0);
    })
    // Modify the procedure which sets the VMs of Nice Shot
    EHOOK_DY(th095_ds_styled_bonus_display8_1, 0x42E5F8, 3, {
        Float3* cur_drawing_pos = (Float3*)(pCtx->Ebp - 0x10);
        cur_drawing_pos->x -= 9.0f;
    })
    EHOOK_DY(th095_ds_styled_bonus_display8_2, 0x42E6FC, 3, {
        ScoreResType* score_res = *(ScoreResType**)(pCtx->Ebp + 0x14);
        AddBonusLastDecimal(pCtx, (int)(score_res->nice_shot_multiplier * 100) % 10);
    })
    // Run the extra VMs
    EHOOK_DY(th095_ds_styled_bonus_display9, 0x42C212, 5, {
        struct PhotoManager* this_ = *(struct PhotoManager**)(pCtx->Ebp - 0xFC);
        for (int i = 0; i < extraBonusVmsSize; ++i) {
            pCtx->Edx = asm_call<ANMVM_RUN, Fastcall, uint64_t>(&extraBonusVms[i], this_) >> 32;
        }
    })
    // Set the colors of the VMs
    EHOOK_DY(th095_ds_styled_bonus_display10_1, 0x42BF4E, 7, {
        SetExtraBonusVmColor(0x20);
    })
    EHOOK_DY(th095_ds_styled_bonus_display10_2, 0x42C0F5, 7, {
        SetExtraBonusVmColor(0xFF);
    })
    // Draw the VMs
    EHOOK_DY(th095_ds_styled_bonus_display11, 0x42C2B8, 7, {
        for (int i = 0; i < extraBonusVmsSize; ++i) {
            asm_call<0x4452D0, Fastcall>(&extraBonusVms[i]);
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

                if (ImGui::Checkbox(S(TH095_DS_STYLE_BONUS_DISPLAY), &mDsStyleBonusDisplay)) {
                    if (mDsStyleBonusDisplay) {
                        EnableAllHooks(DsStyledBonusDisplay);
                    } else {
                        DisableAllHooks(DsStyledBonusDisplay);
                    }
                }
                ImGui::SameLine();
                Gui::HelpMarker(S(TH095_DS_STYLE_BONUS_DISPLAY_DESC));

                EndOptGroup();
            }

            AboutOpt();
            ImGui::EndChild();
            ImGui::SetWindowFocus();
        }

        adv_opt_ctx mOptCtx;

    public:
        bool mFixPhotoScoreDisplay = false;
        bool mDsStyleBonusDisplay = false;
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
    // TH095::th095_photo_score_display_fix_toggle_on.Setup();
    // TH095::th095_photo_score_display_fix_toggle_off.Setup();
    DisableAllHooks(TH095::DsStyledBonusDisplay);
}
}
