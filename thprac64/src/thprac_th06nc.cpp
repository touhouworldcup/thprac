#include "thprac_th06nc.h"
#include <wininternal.h>

// TODOs:
    // - Fix: Stage title popping up on any section except the first
    // - Fix: Alt tabbed game processing inputs when thprac is applied
    // - Fix: Going back to prac after Extra makes Extra the selected gamemode (affects score extends in non-extra; doesn't go back until menuing back to practice mode)
    // - Fix: Rank bounds
    // - All the warps
    // - Other TH6 hooks (check if needed)
    // - THOverlay, Replays, Advanced Menu, etc.
    // - Replace addresses that are members of static structs with static struct access (cf. GameManager)

using namespace TH06;
using std::pair;

namespace TH06NC {
    THPracParam thPracParam{};
    GameManager* GAME_MANAGER;
    EnemyManager* ENEMY_MANAGER;
    StageBackground* STAGE_BACKGROUND;
    void* ECL_MANAGER;

    class THGuiPrac : public Gui::GameGuiWnd {
        Gui::GuiCombo mMode{ TH_MODE, TH_MODE_SELECT };
        Gui::GuiCombo mStage{ TH_STAGE, TH_STAGE_SELECT };
        Gui::GuiCombo mWarp{ TH_WARP, TH_WARP_SELECT_FRAME };
        Gui::GuiCombo mSection{ TH_MODE };
        Gui::GuiCombo mPhase{ TH_PHASE };
        Gui::GuiCheckBox mDlg{ TH_DLG };

        Gui::GuiSlider<int32_t, ImGuiDataType_S32> mChapter{ TH_CHAPTER, 0, 0 };
        Gui::GuiDrag<int32_t, ImGuiDataType_S32> mFrame{ TH_FRAME, 0, INT_MAX };
        Gui::GuiSlider<int8_t, ImGuiDataType_S8> mLife{ TH_LIFE, 0, 8 };
        Gui::GuiSlider<int8_t, ImGuiDataType_S8> mBomb{ TH_BOMB, 0, 8 };
        Gui::GuiDrag<int64_t, ImGuiDataType_S64> mScore{ TH_SCORE, 0, 9999999990, 10, 100000000 };
        Gui::GuiSlider<int16_t, ImGuiDataType_S16> mPower{ TH_POWER, 0, 128 };
        Gui::GuiDrag<int32_t, ImGuiDataType_S32> mGraze{ TH_GRAZE, 0, 99999, 1, 10000 };
        Gui::GuiDrag<int32_t, ImGuiDataType_S32> mPoint{ TH_POINT, 0, 9999, 1, 1000 };

        Gui::GuiSlider<int32_t, ImGuiDataType_S32> mRank{ TH06_RANK, 0, 32, 1, 10, 10 };
        Gui::GuiCombo mFakeShot{ TH06_FS, TH06_TYPE_SELECT };

        Gui::GuiNavFocus mNavFocus{ TH_STAGE, TH_MODE, TH_WARP, TH_FRAME,
            TH_MID_STAGE, TH_END_STAGE, TH_NONSPELL, TH_SPELL, TH_PHASE, TH_CHAPTER,
            TH_LIFE, TH_BOMB, TH_SCORE, TH_POWER, TH_GRAZE, TH_POINT,
            TH06_RANK, TH06_FS };

        float mStep = 10.0;
        uint32_t mDiffculty = 0;
        uint8_t mShotType = 0;

        THGuiPrac() noexcept {
            *mLife = 8;
            *mBomb = 8;
            *mPower = 128;
            *mMode = 1;
            *mScore = 0;
            *mGraze = 0;
            *mRank = 32;

            SetFade(0.8f, 0.1f);
            SetStyle(ImGuiStyleVar_WindowRounding, 0.0f);
            SetStyle(ImGuiStyleVar_WindowBorderSize, 0.0f);
            OnLocaleChange();
        }
        SINGLETON(THGuiPrac);

        void SpellPhase() {
            auto section = CalcSection();

            if (section == TH06_ST7_END_S10)
                mPhase(TH_PHASE, TH_SPELL_PHASE1);
        }

        void SectionWidget() {
            static char chapterStr[256]{};
            auto& chapterCounts = mChapterSetup[*mStage];

            int st = 0;
            if (*mStage == 3) // Stage 4 Fake Shot
                st = (*mFakeShot ? *mFakeShot - 1 : mShotType) + 4;

            switch (*mWarp) {
            case 1: // Chapter
                mChapter.SetBound(1, chapterCounts[0] + chapterCounts[1]);

                if (chapterCounts[1] == 0)
                    sprintf_s(chapterStr, S(TH_STAGE_PORTION_N), *mChapter);
                else if (*mChapter <= chapterCounts[0])
                    sprintf_s(chapterStr, S(TH_STAGE_PORTION_1), *mChapter);
                else
                    sprintf_s(chapterStr, S(TH_STAGE_PORTION_2), *mChapter - chapterCounts[0]);

                mChapter(chapterStr);
                break;

            case 2: // Mid boss
            case 3: // End boss
                if (mSection(TH_WARP_SELECT_FRAME[*mWarp],
                    th_sections_cba[*mStage + st][*mWarp - 2],
                    th_sections_str[::Gui::LocaleGet()][mDiffculty]))
                    *mPhase = 0;
                if (SectionHasDlg(th_sections_cba[*mStage][*mWarp - 2][*mSection]))
                    mDlg();
                break;

            case 4: // Non-spell
            case 5: // Spellcard
                if (mSection(TH_WARP_SELECT_FRAME[*mWarp],
                    th_sections_cbt[*mStage + st][*mWarp - 4],
                    th_sections_str[::Gui::LocaleGet()][mDiffculty]))
                    *mPhase = 0;
                if (SectionHasDlg(th_sections_cbt[*mStage][*mWarp - 4][*mSection]))
                    mDlg();
                break;

            case 6: // Frame
                mFrame();
                break;
            }
        }

        void PracticeMenu(Gui::GuiNavFocus& nav_focus) {
            mMode();
            if (mStage()) *mSection = *mChapter = 0;

            if (*mMode == 1) {
                if (mWarp()) *mSection = *mChapter = *mPhase = *mFrame = 0;
                if (*mWarp) {
                    if (*mStage == 3) mFakeShot();

                    SectionWidget();
                    SpellPhase();
                }

                mLife();
                mBomb();
                mScore();
                mScore.RoundDown(10);
                mPower();
                mGraze();
                mPoint();
                mRank();
            }

            nav_focus();
        }

        virtual void OnLocaleChange() override {
            SetTitle(S(TH_MENU));

            switch (Gui::LocaleGet()) {
            case LOCALE_ZH_CN:
                SetSize(330.f, 390.f);
                SetPos(260.f, 65.f);
                SetItemWidth(-60.0f);
                break;

            case LOCALE_EN_US:
                SetSize(370.f, 375.f);
                SetPos(240.f, 75.f);
                SetItemWidth(-60.0f);
                break;

            case LOCALE_JA_JP:
                SetSize(330.f, 390.f);
                SetPos(260.f, 65.f);
                SetItemWidth(-65.0f);
                break;

            default:
                break;
            }
        }

        virtual void OnContentUpdate() override {
            ImGui::TextUnformatted(S(TH_MENU));
            ImGui::Separator();

            PracticeMenu(mNavFocus);
        }

        int CalcSection() {
            switch (*mWarp) {
            case 1: // Chapter
                return *mChapter + 10000;

            case 2: // Mid boss
            case 3: // End boss
                return th_sections_cba[*mStage][*mWarp - 2][*mSection];

            case 4: // Non-spell
            case 5: // Spellcard
                return th_sections_cbt[*mStage][*mWarp - 4][*mSection];

            default:
                return 0;
            }
        }

        bool SectionHasDlg(int32_t section) {
            switch (section) {
            case TH06_ST1_BOSS1:
            case TH06_ST2_BOSS1:
            case TH06_ST3_BOSS1:
            case TH06_ST4_BOSS1:
            case TH06_ST5_BOSS1:
            case TH06_ST5_MID1:
            case TH06_ST6_BOSS1:
            case TH06_ST6_MID1:
            case TH06_ST7_END_NS1:
            case TH06_ST7_MID1:
                return true;
            default:
                return false;
            }
        }

    public:
        __declspec(noinline) void OpenMenu() {
            SetFade(0.8f, 0.1f);
            Open();
            mDiffculty = GAME_MANAGER->difficulty;
            mShotType = GAME_MANAGER->character * 2 + GAME_MANAGER->subShot;
        }

        __declspec(noinline) void ConfirmMenu() {
            SetFade(0.8f, 0.8f);
            Close();
            *mNavFocus = 0;

            thPracParam.mode  = *mMode;
            thPracParam.stage = *mStage;
            thPracParam.section = CalcSection();
            thPracParam.phase = *mPhase;
            thPracParam.frame = *mFrame;
            if (SectionHasDlg(thPracParam.section))
                thPracParam.dlg = *mDlg;

            thPracParam.score = *mScore;
            thPracParam.life  = *mLife;
            thPracParam.bomb  = *mBomb;
            thPracParam.power = *mPower;
            thPracParam.graze = *mGraze;
            thPracParam.point = *mPoint;

            thPracParam.rank = *mRank;
            if (thPracParam.section >= TH06_ST4_BOSS1 && thPracParam.section <= TH06_ST4_BOSS7)
                thPracParam.fakeType = *mFakeShot;
        }

        __declspec(noinline) void CloseMenu() {
            Close();
            *mNavFocus = 0;
        }
    };


    // ECL Patching
    void THPatch(ECLHelper&, th_sections_t);
    void THStageWarp(int, int);

    __declspec(noinline) void THSectionPatch() {
        ECLHelper ecl;
        ecl.SetBaseAddr((void*)ECL_MANAGER_ADDR);
        auto section = thPracParam.section;

        if (section < 10000) THPatch(ecl, (th_sections_t)section);
        else THStageWarp(thPracParam.stage, section - 10000);
    }

    void ECLWarp(int32_t time) {
        ENEMY_MANAGER->timelineTime.current = time;
    }

    __declspec(noinline) void THStageWarp(int stage, int portion) {
        constexpr uint32_t stageWarps[7][10] = {
            { 68, 580, 1160, 1540, 2348, 4438 }, // st1
            { 270, 924, 3528, 4563 }, // st2
            { 340, 1050, 1670, 2762, 3807, 4118, 5274 }, // st3
            { 380, 1454, 2328, 3392, 4872, 5712, 7434, 8354, 9784 }, // st4
            { 350, 1352, 2292, 3814, 6774 }, // st5
            { 380, 1484 }, // st6
            { 380, 1300, 2600, 3680, 4803, 5933, 7733 }, // ex
        };

        uint32_t warp = stageWarps[stage - 1][portion - 1];
        if (warp) ECLWarp(warp);
    }

    __declspec(noinline) void THPatch(ECLHelper& ecl, th_sections_t section) {
        //todo
    }





    HOOKSET_DEFINE(THMainHook)

    // Prac Menu UX
    // TODO: change some R registers to E if that's possible (32th?)
    EHOOK_DY(th06nc_skip_prac_mode, 0x4bbd5, 6, { // subshot select confirm input, writing next state
        uintptr_t rsi = pCtx->Rsi;
        bool inPractice = GetMemContent<bool>(rsi + 0x168d1);

        if (inPractice) {
            *(uint32_t*)(rsi + 0x168b0) = 0x18; // start prompt
            pCtx->Rdx = -1; // prevent triggering unwanted interrupts (ty to zero318 for insight)
            THGuiPrac::singleton().OpenMenu();
        }
        else OG_INS(*(uint32_t*)(rsi + 0x168b0) = (uint32_t)pCtx->Rdx);
    })

    EHOOK_DY(th06nc_skip_prac_mode_2, 0x4bc46, 5, { // after setting -1 interrupts
        if (pCtx->Rdx == -1) pCtx->Rdx = 0x18;
    })

    EHOOK_DY(th06nc_cancel_prac, 0x4c2ba, 6, { // start prompt back input, writing next state
        uintptr_t rsi = pCtx->Rsi;
        bool inPractice = GetMemContent<bool>(rsi + 0x168d1);

        if (inPractice) {
            pCtx->Rip = RVA(0x4be03); // pretend we just exited mode selection
            THGuiPrac::singleton().CloseMenu();
        }
        else OG_INS(pCtx->Rax = *(uint32_t*)(rsi + 0x168cc));
    })

    EHOOK_DY(th06nc_confirm_prac, 0x4c474, 10, { // start prompt confirm input
        uintptr_t rsi = pCtx->Rsi;
        bool inPractice = GetMemContent<bool>(rsi + 0x168d1);

        if (inPractice) {
            THGuiPrac::singleton().ConfirmMenu();
            GAME_MANAGER->stage = thPracParam.stage;
        }
        OG_INS(*(uint32_t*)(rsi + 0x168b0) = 0x19);
    })

    // Core Hooks
    EHOOK_DY(th06nc_update, 0x3bf59, 1, { // end of run_all_on_tick
        GameGuiBegin(IMPL_WIN32_DX11);

        /* Gui components update */
        /* Gui::KeyboardInputUpdate(VK_ESCAPE); */
        THGuiPrac::singleton().Update();
        /* THGuiRep::singleton().Update();
        THOverlay::singleton().Update();

        if (tracker_open && (GAME_MANAGER->isInGame || GAME_MANAGER->isInGameMenu || GAME_MANAGER->isInRetryMenu))
            THTrackerUpdate();*/

        //GameGuiEnd(THAdvOptWnd::StaticUpdate() || THGuiPrac::singleton().IsOpen() || THPauseMenu::singleton().IsOpen());
        GameGuiEnd(THGuiPrac::singleton().IsOpen());
        OG_INS(pCtx->Rip = PopHelper(pCtx));
    })

    EHOOK_DY(th06nc_render, 0x3c257, 1, {  // end of run_all_on_draw
        GameGuiRender(IMPL_WIN32_DX11);
        OG_INS(pCtx->Rip = PopHelper(pCtx));
    })

    EHOOK_DY(th06nc_title_screen_transition, 0x4802e, 7, { // transition to title screen (main menu state 3)
        thPracParam.Reset();
        OG_INS(*(uint32_t*)(pCtx->Rsi + 0x168b0) = (uint32_t)pCtx->R10);
    })

    // On Prac (Re)Start
    EHOOK_DY(th06nc_patch_main, 0x3b69d, 1, { // end of GameManager::on_registration
        OG_INS(pCtx->Rip = PopHelper(pCtx));
        if (thPracParam.mode != 1) return;

        GAME_MANAGER->curPower = thPracParam.power;
        GAME_MANAGER->actualScore = GAME_MANAGER->visualScore = thPracParam.score;
        GAME_MANAGER->stageGraze = GAME_MANAGER->totalGraze = thPracParam.graze;
        GAME_MANAGER->stagePointItems = GAME_MANAGER->totalPointItems = thPracParam.point;
        GAME_MANAGER->livesRemaining = thPracParam.life;
        GAME_MANAGER->bombsRemaining = thPracParam.bomb;
        GAME_MANAGER->rank = thPracParam.rank;

        if (GAME_MANAGER->difficulty != 4) { // avoid triggering score extends
            if (thPracParam.score >= 60000000) GAME_MANAGER->scoreExtends = 4;
            else if (thPracParam.score >= 40000000) GAME_MANAGER->scoreExtends = 3;
            else if (thPracParam.score >= 20000000) GAME_MANAGER->scoreExtends = 2;
            else if (thPracParam.score >= 10000000) GAME_MANAGER->scoreExtends = 1;
        }

        if (thPracParam.frame) ECLWarp(thPracParam.frame);
        else if (thPracParam.section) THSectionPatch();
    })

    EHOOK_DY(th06nc_bg_fastforward, 0x3b4b5, 2, { // spell prac check for fast-forwarding stage background
        constexpr int32_t safeSpellNums[7] = { 2, 9, 24, 37, 84, 100, 121 }; // not fully sure how fast-forwarding works but giving it a spell# it expects makes it use the boss pseudo-interrupt

        if (thPracParam.mode) {
            int32_t section = thPracParam.section;
            int32_t stage = thPracParam.stage;

            if (section < 10000) { // Section
                if (th_sections_bgm[section]) // boss section -> boss bg
                    GAME_MANAGER->spellPracSpellNum = safeSpellNums[thPracParam.stage];

                return; // midboss section -> midboss bg

            } else if (section - 10000 > mChapterSetup[thPracParam.stage][0]) {
                return; // post-mid chapter -> midboss bg
            }
        }

        OG_INS(pCtx->Rip = RVA(0x3b4bc)); // start bg
    })
    EHOOK_DY(th06nc_stage_bgm, 0x3b611, 3, { // stage start bgm pick (0x80 = boss, set by spell prac)
        if (thPracParam.mode && th_sections_bgm[thPracParam.section] && !thPracParam.dlg)
            pCtx->Rdx += 0x80;
        else OG_INS(pCtx->Rdx += pCtx->R15);
    })
    HOOKSET_ENDDEF()

    static __declspec(noinline) void THGuiCreate() {
        if (ImGui::GetCurrentContext()) return;

        // Grab key globals
        GAME_MANAGER = (GameManager*)RVA(GAME_MANAGER_ADDR);
        STAGE_BACKGROUND = (StageBackground*)RVA(STAGE_BACKGROUND_ADDR);
        ENEMY_MANAGER = (EnemyManager*)RVA(ENEMY_MANAGER_ADDR);
        ECL_MANAGER = (void*)RVA(ECL_MANAGER_ADDR);

        // Init
        GameGuiInit(IMPL_WIN32_DX11, RVA(D3D_DEVICE_PTR), RVA(HWND_PTR),
                    Gui::INGAGME_INPUT_GEN1, RVA(INPUT_ADDR), RVA(INPUT_PREV_ADDR),
                    RVA(IS_EIGTH_FRAME_OF_HELD_INPUT_ADDR), 1.0f, RVA(D3D_DEVICE_CONTEXT));

        // weird...
        ImGui::GetIO().DisplaySize = ImVec2(1280, 720);

        //TODO
        //SetDpadHook(0x41D330, 3);

        // Gui components creation
        THGuiPrac::singleton();
        //THPauseMenu::singleton();
        //THGuiRep::singleton();
        //THOverlay::singleton();

        // Hooks
        EnableAllHooks(THMainHook);
        //th06_white_screen.Setup();
        //th06_result_screen_create.Setup();
        //th06_sfx_fix.Setup();
        //th06_sfx_fix.Disable();
        //th06_bomb_esc_r_prevent_desyncs.Setup();
        //th06_bomb_esc_r_prevent_desyncs.Disable();

        // Reset thPracParam
        thPracParam.Reset();
    }

    HOOKSET_DEFINE(THInitHook)
    //EHOOK_DY(th06nc_gui_init_1, 0x73D33, 4, { // main menu ontick (TODO: TEST THIS)
    //    THGuiCreate();
    //    self->Disable();
    //    OG_INS(pCtx->Rcx = *(uint64_t*)(pCtx->Rbp - 0x58));
    //})
    EHOOK_DY(th06nc_gui_init_2, 0x27070e, 8, { // initial loading (d3d creation)
        THGuiCreate();
        self->Disable();
        OG_INS(pCtx->R12 = *(uint64_t*)(pCtx->Rsp + 0x98));
    })
    HOOKSET_ENDDEF()
}

void TH06NCInitReal() {
    ingame_image_base = (uintptr_t)CurrentPeb()->ImageBaseAddress;
    EnableAllHooks(TH06NC::THInitHook);
}