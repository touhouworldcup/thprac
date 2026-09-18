#include "thprac_th06nc.h"
#include <wininternal.h>

// TODOs:
    // - Fix: Going back to prac after Extra makes Extra the selected gamemode (affects score extends in non-extra; doesn't go back until menuing back to practice mode)
    // - Fix: Rank bounds
    // - Add: STD timeline skip for Frame warping? Per-stage frame cap?
    // - All the warps
    // - Other TH6 hooks (check if needed)
    // - THOverlay, Replays, Advanced Menu, etc.
    // - Replace addresses that are members of static structs with static struct access (cf. GameManager)

using namespace TH06;
using std::pair;

namespace TH06NC {
    THPracParam thPracParam{};
    GameManager* GAME_MANAGER;
    Player* PLAYER;
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
            thPracParam.dlg = SectionHasDlg(thPracParam.section) ? *mDlg : false;

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


    // ECL Patching Tools
    void THPatch(ECLHelper&, int32_t, th_sections_t);
    void THStageWarp(int, int);

    __declspec(noinline) void THSectionPatch() {
        ECLHelper ecl;
        ecl.SetBaseAddr(ECL_MANAGER);
        int32_t section = thPracParam.section;

        if (section < 10000) THPatch(ecl, thPracParam.stage, (th_sections_t)section);
        else THStageWarp(thPracParam.stage, section - 10000);
    }

    void ECLWarp(int32_t time) {
        ENEMY_MANAGER->timelineTime.current = time;
    }

    void PostStartWarpAdjustments() {
        *(uint8_t*)(*(uintptr_t*)RVA(0xA6EC08) + 0xa24) = 2; // hide stage logo/title
        *(uint8_t*)(GetMemAddr<uintptr_t>(RVA(0x4ff2b8), 0x38, 0x480) + 0xef) = 255; // show stage HUD illustration

        // disable iframes
        PLAYER->player_state = 0;
        PLAYER->state_timer = 0;
    }

    __declspec(noinline) void THStageWarp(int32_t stage, int32_t portion) {
        constexpr int32_t d = 40;
        constexpr int32_t stageWarps[7][10] = {
            { 0, 594 - d, 1174 - d, 1554 - d, 2282 - d, 4372 - d }, // st1
            { 0, 894 - d, 3498 - d, 4533 - d }, // st2
            { 0, 910 - d, 1530 - d, 2622 - d, 3476 + 1, 3898 - d, 5054 - d }, // st3
            { 0, 1454, 2328, 3392, 4872, 5712, 7434, 8354, 9784 }, // st4
            { 0, 1352, 2292, 3814, 6774 }, // st5
            { 0, 1484 }, // st6
            { 0, 1300, 2600, 3680, 4803, 5933, 7733 }, // ex
        };

        int32_t warp = stageWarps[stage][portion - 1];
        if (warp) {
            ECLWarp(warp);
            PostStartWarpAdjustments();
        }
    }

    void ECLSetInsTime(ECLHelper& ecl, int offset, int32_t ecl_time = 0, bool timeline = false) {
        ecl << pair{ offset, timeline ? (int16_t)ecl_time : ecl_time };
    }

    template<typename... KVPairs>
    void ECLSetArgs(ECLHelper& ecl, int offset, KVPairs&&... args) {
        (ecl << ... << pair{ offset + 0xc + args.first, args.second });
    }

    template<typename... KVPairs>
    void ECLMakeIns(ECLHelper& ecl, int offset, int32_t ecl_time, ECL_OP op, KVPairs&&... args) {
        ecl.SetPos(offset);
        ecl << ecl_time << op.code << op.size << 0x00ffff00;
        if constexpr (sizeof...(args) > 0) ECLSetArgs(ecl, offset, args...);
    }

    void ECLDisable(ECLHelper& ecl, int offset, bool timeline = false) {
        ecl << pair{ offset + 0x4, (int16_t)(timeline ? 14 : 0) };
    }

    int32_t healthOverride;
    EHOOK_ST(th06nc_trigger_health_interrupt, 0x36248, 3, {
        uintptr_t enemyAddr = pCtx->Rbx;
        *(int32_t*)(enemyAddr + 0x234) = healthOverride;

        OG_INS(pCtx->Rax = enemyAddr);
        self->Disable();
    });

    void TriggerHealthInterrupt(int32_t threshold) {
        th06nc_trigger_health_interrupt.Enable();
        healthOverride = threshold - 1;
    }

    // ECL Patching
    __declspec(noinline) void THPatch(ECLHelper& ecl, int32_t stage, th_sections_t section) {
        PostStartWarpAdjustments();

        switch (stage) {
        case 0: { // Stage 1
            constexpr uint32_t st1MidbossTime = 1882;

            auto s1_boss_warp_skip_move = [&]() {
                constexpr uint32_t st1BossTime = 5093;
                constexpr uint32_t st1BossMoveInterp = 0x1744;

                ECLWarp(st1BossTime);
                ECLSetArgs(ecl, st1BossMoveInterp, pair{ 0, 0 });
            };

            switch (section) {
            case TH06_ST1_MID1: // Midboss
                ECLWarp(st1MidbossTime);
                break;

            case TH06_ST1_MID2: // Midspell (NHL)
                ECLWarp(st1MidbossTime);
                TriggerHealthInterrupt(500);
                break;

            case TH06_ST1_BOSS1: { // Non 1
                constexpr uint32_t st1BossDlgTime = 5092;
                thPracParam.dlg ? ECLWarp(st1BossDlgTime) : s1_boss_warp_skip_move();
                break;
            }

            case TH06_ST1_BOSS2: { // Spell 1 (NHL)
                constexpr uint32_t st1bsNon1TimeThreshold = 0x1870;

                s1_boss_warp_skip_move();
                ECLSetArgs(ecl, st1bsNon1TimeThreshold, pair{ 0, 0 });
                break;
            }

            case TH06_ST1_BOSS4: { // Spell 2
                constexpr uint32_t st1bsNon2TimeThreshold = 0x2b20;
                ECLSetArgs(ecl, st1bsNon2TimeThreshold, pair{ 0, 0 });
                [[fallthrough]];
            }
            case TH06_ST1_BOSS3: { // Non 2
                constexpr uint32_t st1bsNon1FirstDelayedIns = 0x18dc;
                constexpr uint32_t st1bsNon2ItemDrop = 0x2b50;

                s1_boss_warp_skip_move();
                ECLMakeIns(ecl, st1bsNon1FirstDelayedIns, 0, CALL, pair{ 0, 19 }); // call sub 19 (non2)
                ECLDisable(ecl, st1bsNon2ItemDrop);
                break;
            }

            default: break;
            }
            break;
        }

        case 1: { // Stage 2
            constexpr uint32_t st2MidbossTime = 2498;
            constexpr uint32_t st2bsNon2TimeThreshold = 0x20f4;

            auto s2_boss_warp_skip_fadein = [&]() {
                constexpr uint32_t st2BossTime = 5894;
                constexpr uint32_t st2BossFadeIn = 0x184c;

                ECLWarp(st2BossTime);
                ECLDisable(ecl, st2BossFadeIn);
            };

            auto s2_boss_non2_warp = [&]() {
                constexpr uint32_t st2bsNon1FirstDelayedIns = 0x192c;
                constexpr uint32_t st2bsNon2ItemDrop = 0x2124;

                s2_boss_warp_skip_fadein();
                ECLMakeIns(ecl, st2bsNon1FirstDelayedIns, 0, CALL, pair{ 0, 25 }); // call sub 25 (non2)
                ECLDisable(ecl, st2bsNon2ItemDrop);
            };

            switch (section) {
            case TH06::TH06_ST2_MID1: // Midboss
                ECLWarp(st2MidbossTime);
                break;

            case TH06::TH06_ST2_BOSS1: { // Non 1
                constexpr uint32_t st2BossDlgTime = 5893;
                thPracParam.dlg ? ECLWarp(st2BossDlgTime) : s2_boss_warp_skip_fadein();
                break;
            }

            case TH06::TH06_ST2_BOSS2: { // Spell 1
                constexpr uint32_t st2bsNon1TimeThreshold = 0x18ec;

                s2_boss_warp_skip_fadein();
                ECLSetArgs(ecl, st2bsNon1TimeThreshold, pair{ 0, 0 });
                break;
            }

            case TH06::TH06_ST2_BOSS3: // Non 2
                s2_boss_non2_warp();
                break;

            case TH06::TH06_ST2_BOSS4: // Spell 2
                s2_boss_non2_warp();
                ECLSetArgs(ecl, st2bsNon2TimeThreshold, pair{ 0, 0 });
                break;

            case TH06::TH06_ST2_BOSS5: { // Spell 3
                constexpr uint32_t st2bsNon2HealthThreshold = 0x20d4;
                constexpr uint32_t st2bsNon2TimeCallbackSub = 0x2104;
                constexpr uint32_t st2bsNon3DropItems = 0x377c;
                constexpr uint32_t st2bsNon3Particle = 0x3754;
                constexpr uint32_t st2bsNon3DelayedIns1 = 0x37b0;
                constexpr uint32_t st2bsNon3DelayedIns2 = 0x37c0;
                constexpr uint32_t st2bsNon3DelayedIns3 = 0x37d8;

                s2_boss_non2_warp();
                ECLSetArgs(ecl, st2bsNon2HealthThreshold, pair{ 0, 1400 });
                ECLSetArgs(ecl, st2bsNon2TimeCallbackSub, pair{ 0, 39 }); // call sub39
                ECLSetArgs(ecl, st2bsNon2TimeThreshold, pair{ 0, 0 });
                ECLDisable(ecl, st2bsNon3DropItems);
                ECLDisable(ecl, st2bsNon3Particle);
                ECLSetInsTime(ecl, st2bsNon3DelayedIns1, 0);
                ECLSetInsTime(ecl, st2bsNon3DelayedIns2, 0);
                ECLSetInsTime(ecl, st2bsNon3DelayedIns3, 0);
                break;
            }

            default: break;
            }
            break;
        }

        case 2: { // Stage 3
            constexpr uint32_t st3MidbossTime = 3474;
            constexpr uint32_t st3BossTime = 6254;

            constexpr uint32_t st3bsNon1FirstDelayedIns = 0x23f8;
            constexpr uint32_t st2bsNon3TimeThreshold = 0x3500;

            auto s3_boss_warp_skip_setup = [&]() {
                constexpr uint32_t st3DialogRead = 0x95e8;
                constexpr uint32_t st3DialogWait = 0x95f0;
                constexpr uint32_t st3BossInterupt = 0x95f8;

                ECLWarp(st3BossTime);
                ECLSetInsTime(ecl, st3DialogRead, st3BossTime, true);
                ECLDisable(ecl, st3DialogRead, true);
                ECLSetInsTime(ecl, st3DialogWait, st3BossTime, true);
                ECLDisable(ecl, st3DialogWait, true);
                ECLSetInsTime(ecl, st3BossInterupt, st3BossTime, true);

                constexpr uint32_t st3BossMoveInterp = 0x2258;
                constexpr uint32_t st3BossSub10Call = 0x2274;
                constexpr uint32_t st3BossBossSet = 0x22a8;

                ECLSetArgs(ecl, st3BossMoveInterp, pair{ 0, 0 });
                ECLDisable(ecl, st3BossSub10Call);
                ECLSetInsTime(ecl, st3BossBossSet, 0);
            };

            auto s3_boss_non3_warp = [&]() {
                constexpr uint32_t st3bsNon3ItemDrop = 0x3540;

                s3_boss_warp_skip_setup();
                ECLMakeIns(ecl, st3bsNon1FirstDelayedIns, 0, CALL, pair{ 0, 30 }); // call sub 30 (non3)
                ECLDisable(ecl, st3bsNon3ItemDrop);
            };

            switch (section) {
            case TH06::TH06_ST3_MID1: // Midnon
                ECLWarp(st3MidbossTime);
                break;

            case TH06::TH06_ST3_MID2: { // Midspell
                constexpr uint32_t st3mbsSub10Call = 0x1088;

                ECLWarp(st3MidbossTime);
                TriggerHealthInterrupt(1300);
                ECLDisable(ecl, st3mbsSub10Call); // makes boss intangible since we skip to the spell while it's executing
                break;
            }

            case TH06::TH06_ST3_BOSS1: // Non 1
                thPracParam.dlg ? ECLWarp(st3BossTime) : s3_boss_warp_skip_setup();
                break;

            case TH06::TH06_ST3_BOSS2: { // Spell 1
                constexpr uint32_t st3bsNon1TimeThreshold = 0x2360;

                s3_boss_warp_skip_setup();
                ECLSetArgs(ecl, st3bsNon1TimeThreshold, pair{ 0, 0 });
                break;
            }

            case TH06::TH06_ST3_BOSS4: { // Spell 2 (HL)
                constexpr uint32_t st3bsNon2TimeThreshold = 0x290c;
                ECLSetArgs(ecl, st3bsNon2TimeThreshold, pair{ 0, 0 });
                [[fallthrough]];
            }
            case TH06::TH06_ST3_BOSS3: { // Non 2
                constexpr uint32_t st3bsNon2ItemDrop = 0x294c;

                s3_boss_warp_skip_setup();
                ECLMakeIns(ecl, st3bsNon1FirstDelayedIns, 0, CALL, pair{ 0, 24 }); // call sub 24 (non2)
                ECLDisable(ecl, st3bsNon2ItemDrop);
                break;
            }

            case TH06::TH06_ST3_BOSS5: { // Non 3
                s3_boss_non3_warp();
                break;
            }

            case TH06::TH06_ST3_BOSS6: // Spell 3
                s3_boss_non3_warp();
                ECLSetArgs(ecl, st2bsNon3TimeThreshold, pair{ 0, 0 });
                break;

            case TH06::TH06_ST3_BOSS7: { // Spell 4 (NHL)
                constexpr uint32_t st3bsNon3HealthThreshold = 0x34d0;
                constexpr uint32_t st3bsNon3TimeCallbackSubN = 0x3510;
                constexpr uint32_t st3bsNon3TimeCallbackSubHL = 0x3520;
                constexpr uint32_t st3bsNon4DropItems = 0x52f4;
                constexpr uint32_t st3bsNon4TimeThreshold = 0x5304;
                constexpr uint32_t st3bsNon4Particle = 0x5364;
                constexpr uint32_t st3bsNon4DelayedIns1 = 0x538c;
                constexpr uint32_t st3bsNon4DelayedIns2 = 0x53a4;

                s3_boss_non3_warp();
                ECLSetArgs(ecl, st3bsNon3HealthThreshold, pair{ 0, 2000 });
                ECLSetArgs(ecl, st3bsNon3TimeCallbackSubN, pair{ 0, 45 }); // call sub45 (non4)
                ECLSetArgs(ecl, st3bsNon3TimeCallbackSubHL, pair{ 0, 45 }); // call sub45 (non4)
                ECLSetArgs(ecl, st2bsNon3TimeThreshold, pair{ 0, 0 });
                ECLDisable(ecl, st3bsNon4DropItems);
                ECLDisable(ecl, st3bsNon4Particle);
                ECLSetArgs(ecl, st3bsNon4TimeThreshold, pair{0, 2100}); // account for starting spell 60f sooner & time not resetting on cast
                ECLSetInsTime(ecl, st3bsNon4DelayedIns1, 0);
                ECLSetInsTime(ecl, st3bsNon4DelayedIns2, 0);
                break;
            }

            default: break;
            }
            break;
        }

        case 3: { // Stage 4
            switch (section) {
            default: break;
            }
            break;
        }

        case 4: { // Stage 5
            switch (section) {
            default: break;
            }
            break;
        }

        case 5: { // Stage 6
            switch (section) {
            default: break;
            }
            break;
        }

        case 6: { // Extra Stage
            switch (section) {
            default: break;
            }
            break;
        }

        default: break;
        }
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

        int32_t frame = thPracParam.frame;
        if (thPracParam.section) THSectionPatch();
        else if (frame) {
            if (frame > 60) PostStartWarpAdjustments();
            ECLWarp(frame);
        }
    })

    EHOOK_DY(th06nc_bg_fastforward, 0x3b4b5, 2, { // spell prac check for fast-forwarding stage background
        constexpr int32_t safeSpellNums[7] = { 2, 9, 24, 37, 84, 103, 121 }; // not fully sure how fast-forwarding works but giving it a spell# it expects makes it use the boss pseudo-interrupt
        int32_t section = thPracParam.section;

        if (thPracParam.mode && section) {
            int32_t stage = thPracParam.stage;

            if (section < 10000) { // Section
                if (th_sections_bgm[section]) // boss section -> boss bg
                    GAME_MANAGER->spellPracSpellNum = safeSpellNums[thPracParam.stage];
                else GAME_MANAGER->spellPracSpellNum = 0;

                return; // midboss section -> midboss bg

            } else if (section - 10000 > mChapterSetup[thPracParam.stage][0]) {
                GAME_MANAGER->spellPracSpellNum = 0;
                return; // post-mid chapter -> midboss bg
            }
        }

        OG_INS(pCtx->Rip = RVA(0x3b4bc)); // start bg
    })

    EHOOK_DY(th06nc_stage_bgm, 0x3b611, 3, { // stage start bgm pick (0x80 = boss, set by spell prac)
        int32_t section = thPracParam.section;

        if (thPracParam.mode && section && section < 10000
          && th_sections_bgm[section] && !thPracParam.dlg)
            pCtx->Rdx += 0x80;
        else OG_INS(pCtx->Rdx += pCtx->R15);
    })

    EHOOK_DY(th06nc_bgm_title, 0x3e5f4, 7, { // spell prac check when picking BGM name key to render
        OG_INS(pCtx->R12 = GetMemContent<uint64_t>(RVA(0x509c08)));
        int32_t section = thPracParam.section;

        if (thPracParam.mode && section && section < 10000
          && th_sections_bgm[section] && !thPracParam.dlg)
            pCtx->Rip = RVA(0x3e5fd);
    })
    HOOKSET_ENDDEF()

    static __declspec(noinline) void THGuiCreate() {
        if (ImGui::GetCurrentContext()) return;

        // Grab key globals
        GAME_MANAGER = (GameManager*)RVA(GAME_MANAGER_ADDR);
        PLAYER = (Player*)RVA(PLAYER_ADDR);
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
        th06nc_trigger_health_interrupt.Setup();

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