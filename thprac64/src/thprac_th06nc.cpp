#include "thprac_th06nc.h"
#include <wininternal.h>

// TODOs:
    // - Hooking fix so that anything works (32th)
    // - Practice mode hooks to open/close/confirm thprac menu
    // - All the warps
    // - Other TH6 hooks (check if needed)
    // - THOverlay, Replays, Advanced Menu, etc.
    // - Replace addresses that are members of static structs with static struct access (cf. GameManager)

using namespace TH06;
namespace TH06NC {
    THPracParam thPracParam{};
    GameManager* GAME_MANAGER;
    EnemyManager* ENEMY_MANAGER;

    class THGuiPrac : public Gui::GameGuiWnd {
        Gui::GuiCombo mMode{ TH_MODE, TH_MODE_SELECT };
        Gui::GuiCombo mStage{ TH_STAGE, TH_STAGE_SELECT };
        Gui::GuiCombo mWarp{ TH_WARP, TH_WARP_SELECT_FRAME };
        Gui::GuiCombo mSection{ TH_MODE };
        Gui::GuiCombo mPhase{ TH_PHASE };
        Gui::GuiCheckBox mDlg{ TH_DLG };

        Gui::GuiSlider<int, ImGuiDataType_S32> mChapter{ TH_CHAPTER, 0, 0 };
        Gui::GuiDrag<int, ImGuiDataType_S32> mFrame{ TH_FRAME, 0, INT_MAX };
        Gui::GuiSlider<int8_t, ImGuiDataType_S32> mLife{ TH_LIFE, 0, 8 };
        Gui::GuiSlider<int8_t, ImGuiDataType_S32> mBomb{ TH_BOMB, 0, 8 };
        Gui::GuiDrag<int64_t, ImGuiDataType_S64> mScore{ TH_SCORE, 0, 9999999990, 10, 100000000 };
        Gui::GuiSlider<int16_t, ImGuiDataType_S32> mPower{ TH_POWER, 0, 128 };
        Gui::GuiDrag<int, ImGuiDataType_S32> mGraze{ TH_GRAZE, 0, 99999, 1, 10000 };
        Gui::GuiDrag<int, ImGuiDataType_S32> mPoint{ TH_POINT, 0, 9999, 1, 1000 };

        Gui::GuiSlider<int, ImGuiDataType_S32> mRank{ TH06_RANK, 0, 32, 1, 10, 10 };
        Gui::GuiCombo mFakeShot{ TH06_FS, TH06_TYPE_SELECT };

        Gui::GuiNavFocus mNavFocus{ TH_STAGE, TH_MODE, TH_WARP, TH_FRAME,
            TH_MID_STAGE, TH_END_STAGE, TH_NONSPELL, TH_SPELL, TH_PHASE, TH_CHAPTER,
            TH_LIFE, TH_BOMB, TH_SCORE, TH_POWER, TH_GRAZE, TH_POINT,
            TH06_RANK, TH06_FS };

        int mChapterSetup[7][2]{
            { 4, 2 },
            { 2, 2 },
            { 4, 3 },
            { 4, 5 },
            { 3, 2 },
            { 2, 0 },
            { 4, 3 }
        };

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
            if (*mStage == 3) { // Stage 4 Fake Shot
                st = (*mFakeShot ? *mFakeShot - 1 : mShotType) + 4;
            }

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






    // Hooks
    HOOKSET_DEFINE(THMainHook)
    EHOOK_DY(th06_prac_menu_1, 0x0, 0, {
        THGuiPrac::singleton().OpenMenu();
    })
    EHOOK_DY(th06_prac_menu_3, 0x0, 0, {
        THGuiPrac::singleton().ConfirmMenu();
    })
    EHOOK_DY(th06_prac_menu_4, 0x0, 0, {
        THGuiPrac::singleton().CloseMenu();
    })
    EHOOK_DY(th06_update, 0x0, 0, {
        GameGuiBegin(IMPL_WIN32_DX8);

        // Gui components update
        //Gui::KeyboardInputUpdate(VK_ESCAPE);
        THGuiPrac::singleton().Update();
        //THGuiRep::singleton().Update();
        //THOverlay::singleton().Update();

        //if (tracker_open && (GAME_MANAGER->isInGame || GAME_MANAGER->isInGameMenu || GAME_MANAGER->isInRetryMenu))
        //    THTrackerUpdate();

        //GameGuiEnd(THAdvOptWnd::StaticUpdate() || THGuiPrac::singleton().IsOpen() || THPauseMenu::singleton().IsOpen());
        GameGuiEnd(THGuiPrac::singleton().IsOpen());
    })
    EHOOK_DY(th06_patch_main, 0x0, 0, {
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
        else THSectionPatch();
    })
    HOOKSET_ENDDEF()


    static __declspec(noinline) void THGuiCreate() {
        if (ImGui::GetCurrentContext()) return;

        // Grab key globals
        GAME_MANAGER = GetMemContent<GameManager*>(RVA(GAME_MANAGER_ADDR));
        ENEMY_MANAGER = GetMemContent<EnemyManager*>(RVA(ENEMY_MANAGER_ADDR));

        // Init
        GameGuiInit(IMPL_WIN32_DX11,
                    GetMemContent<uintptr_t>(RVA(D3D_DEVICE_PTR)),
                    GetMemContent<uintptr_t>(RVA(HWND_PTR)),
                    Gui::INGAGME_INPUT_GEN1, RVA(INPUT_ADDR), RVA(INPUT_PREV_ADDR),
                    RVA(IS_EIGTH_FRAME_OF_HELD_INPUT_ADDR), 1.0f);

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
    EHOOK_DY(th06_gui_init_1, 0x73D33, 4, { // main menu ontick
        THGuiCreate();
        self->Disable();
    })
    EHOOK_DY(th06_gui_init_2, 0x27070e, 8, { // initial loading (d3d creation)
        THGuiCreate();
        self->Disable();
    })
    HOOKSET_ENDDEF()
}

void TH06NCInitReal() {
    ingame_image_base = (uintptr_t)CurrentPeb()->ImageBaseAddress;
    EnableAllHooks(TH06NC::THInitHook);
}