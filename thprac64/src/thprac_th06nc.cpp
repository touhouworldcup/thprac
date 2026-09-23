#include "thprac_th06nc.h"
#include <wininternal.h>

// TODOs:
    // - Fix: Rank bounds
    // - Add: STD timeline skip for Frame warping? Per-stage frame cap?
    // - All the warps
    // - Other TH6 hooks (check if needed)
    // - THOverlay, Replays, Advanced Menu, etc.
    // - Replace addresses that are members of static structs with static struct access (cf. GameManager)
    // - Correct spell translations to match NC (official TLs)

using namespace TH06;
using std::pair;

namespace TH06NC {
    THPracParam thPracParam{};
    GameManager* GAME_MANAGER;
    Player* PLAYER;
    EnemyManager* ENEMY_MANAGER;
    StageBackground* STAGE_BACKGROUND;
    Enemy** BOSS_PTR;
    void* ECL_MANAGER;
    void* ANM_MANAGER_PTR;
    void* ZUN_GUI;

    class THGuiPrac : public Gui::GameGuiWnd {
        Gui::GuiCombo mMode{ TH_MODE, TH_MODE_SELECT };
        Gui::GuiCombo mStage{ TH_STAGE, TH_STAGE_SELECT };
        Gui::GuiCombo mWarp{ TH_WARP };
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
        Gui::GuiCheckBox mGuaranteeTLB { TH06NC_TLB_LOCK };

        Gui::GuiNavFocus mNavFocus{ TH_STAGE, TH_MODE, TH_WARP, TH_FRAME,
            TH_MID_STAGE, TH_END_STAGE, TH_NONSPELL, TH_SPELL, TH_PHASE, TH_CHAPTER,
            TH_LIFE, TH_BOMB, TH_SCORE, TH_POWER, TH_GRAZE, TH_POINT,
            TH06_RANK, TH06_FS };

        float mStep = 10.0;
        int32_t mDiffculty = -1;
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

        const th_glossary_t* SpellPhase() {
            int section = CalcSection();

            switch (section) {
            case TH06_ST7_END_S9:
                return TH06_SPELL_PHASE_TIMEOUT;

            case TH06_ST7_END_S10:
                return TH06_SPELL_PHASE_QED;

            case TH06NC_TLB1:
            case TH06NC_TLB2:
                return TH_SPELL_PHASE_FULL;

            case TH06NC_TLB3:
                return TH_SPELL_PHASE1;

            default: return nullptr;
            }
        }

        int SectionWidget(int warpType) {
            static char chapterStr[256]{};
            int stage = *mStage;
            auto& chapterCounts = mChapterSetup[stage];

            if (stage != 6)
                warpType = fixType(warpType);

            int stgOffset = 0;
            if (stage == 3) // Stage 4 Fake Shot
                stgOffset = (*mFakeShot ? *mFakeShot - 1 : mShotType) + 4;

            switch (warpType) {
            case CHAPTER:
                mChapter.SetBound(1, chapterCounts[0] + chapterCounts[1]);

                if (chapterCounts[1] == 0)
                    sprintf_s(chapterStr, S(TH_STAGE_PORTION_N), *mChapter);
                else if (*mChapter <= chapterCounts[0])
                    sprintf_s(chapterStr, S(TH_STAGE_PORTION_1), *mChapter);
                else
                    sprintf_s(chapterStr, S(TH_STAGE_PORTION_2), *mChapter - chapterCounts[0]);

                mChapter(chapterStr);
                break;

            case MIDBOSS:
            case ENDBOSS:
                if (mSection(TH_WARP_SELECT_FRAME[warpType],
                    th_sections_cba[stage + stgOffset][warpType - 2],
                    th_sections_str[::Gui::LocaleGet()][mDiffculty]))
                    *mPhase = 0;

                return th_sections_cba[stage][warpType - 2][*mSection];

            case TLB:
                if (mSection(TH_SPELL, th_sections_cba[6][2], th_sections_str[Gui::LocaleGet()][0]))
                    *mPhase = 0;

                return th_sections_cba[6][2][*mSection];

            case NONSPELL:
            case SPELL:
                if (mSection(TH_WARP_SELECT_FRAME[warpType - 1],
                    th_sections_cbt[stage + stgOffset][warpType - 5],
                    th_sections_str[::Gui::LocaleGet()][mDiffculty]))
                    *mPhase = 0;

                return th_sections_cbt[stage][warpType - 5][*mSection];

            case FRAME:
                mFrame();
                break;
            }

            return A0000ERROR;
        }

        int prevStage = *mStage; //s1
        void AdjustWarpSelection() {
            int stage = *mStage;
            int warpType = *mWarp;

            if (stage == 6) { // change to Ex (warp has 1 more option)
                if (warpType >= TLB) *mWarp = warpType + 1;
            } else if (prevStage == 6) { // change from Ex (warp has 1 less option)
                if (warpType >= TLB) *mWarp = warpType - 1; // if TLB was selected, it changes to End Boss
            }

            prevStage = stage;
        }

        void PracticeMenu(Gui::GuiNavFocus& nav_focus) {
            mMode();
            if (mStage()) {
                *mSection = *mChapter = 0;
                AdjustWarpSelection();
            }

            if (*mMode == 1) {
                int warpType = *mWarp;
                int stage = *mStage;
                int section;

                if (mWarp(TH_WARP, stage == 6 ? TH06NC_WARP_SELECT_EX : TH_WARP_SELECT_FRAME))
                    *mSection = *mChapter = *mPhase = *mFrame = 0;

                if (warpType) {
                    if (stage == 3 && warpType > CHAPTER && fixType(warpType) != FRAME)
                        mFakeShot();

                    section = SectionWidget(warpType);
                    mPhase(TH_PHASE, SpellPhase());
                    if (SectionHasDlg(section))
                        mDlg();
                }

                mLife();
                mBomb();
                mScore();
                mScore.RoundDown(10);
                mPower();
                mGraze();
                mPoint();
                mRank();

                if (stage == 6 && warpType > MIDBOSS && warpType != FRAME && section < TH06NC_TLB1)
                    mGuaranteeTLB();
            }

            nav_focus();
        }

        virtual void OnLocaleChange() override {
            constexpr float asnWidth = 0.26f;
            constexpr float asnHeight = 0.58f;
            constexpr float enWidth = asnWidth * 1.2f;
            constexpr float enHeight = asnHeight;

            constexpr float targetX = 0.81f;
            constexpr float targetY = 0.4f;
            constexpr float asnX = targetX - asnWidth / 2.f;
            constexpr float asnY = targetY - asnHeight / 2.f;
            constexpr float enX = targetX - enWidth / 2.f;
            constexpr float enY = targetY - enHeight / 2.f;

            SetTitle(S(TH_MENU));

            switch (Gui::LocaleGet()) {
            case LOCALE_ZH_CN:
            case LOCALE_JA_JP:
                SetSizeRel(asnWidth, asnHeight);
                SetPosRel(asnX, asnY);
                //SetItemWidth(-60.f); // -65.f for JP, butwhy
                break;

            case LOCALE_EN_US:
                SetSizeRel(enWidth, enHeight);
                SetPosRel(enX, enY);
                //SetItemWidth(-60.f);
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
            int stage = *mStage;
            int warpType = *mWarp;
            if (stage != 6) warpType = fixType(warpType);

            switch (warpType) {
            case CHAPTER:
                return *mChapter + 10000;

            case MIDBOSS:
            case ENDBOSS:
                return th_sections_cba[stage][warpType - 2][*mSection];

            case TLB:
                return th_sections_cba[6][2][*mSection];

            case NONSPELL:
            case SPELL:
                return th_sections_cbt[stage][warpType - 5][*mSection];

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
            case TH06NC_TLB1:
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
            else thPracParam.fakeType = 0;
            if (thPracParam.section >= TH06_ST7_END_NS1 && thPracParam.section <= TH06_ST7_END_S10)
                thPracParam.guaranteeTLB = *mGuaranteeTLB;
            else thPracParam.guaranteeTLB = false;
        }

        __declspec(noinline) void CloseMenu() {
            Close();
            *mNavFocus = 0;
            mDiffculty = -1;
        }

        __declspec(noinline) void RestoreDifficulty() {
            // ensure the game menu's difficulty is restored to what it was
            // when practice started (extra stage sets it to 4)
            if (mDiffculty > -1) GAME_MANAGER->difficulty = mDiffculty;
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
        constexpr int32_t d = 45;
        constexpr int32_t stageWarps[7][9] = {
            { 0, 594  - d, 1174 - d, 1554 - d, 2282 - d, 4372 - d }, // st1
            { 0, 894  - d, 3498 - d, 4533 - d }, // st2
            { 0, 910  - d, 1530 - d, 2622 - d, 3476 + 1, 3898 - d, 5054 - d }, // st3
            { 0, 1430 - d, 2304 - d, 3378 - d, 4858 - d, 5698 - d, 7380 - d, 8300 - d, 9730 - d }, // st4
            { 0, 1342 - d, 2252 - d, 3774 - d, 6734 - d }, // st5
            { 0, 1459 - d }, // st6
            { 0, 1260 - d, 2560 - d, 3640 - d, 4763 - d, 5893 - d, 7693 - d }, // ex
        };

        int32_t warp = stageWarps[stage][portion - 1];
        if (warp) {
            ECLWarp(warp);
            PostStartWarpAdjustments();
        }
    }

    void ECLSetInsTime(ECLHelper& ecl, int offset, int32_t ecl_time, bool timeline = false) {
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
    int32_t triggerFrame;

    EHOOK_ST(th06nc_trigger_health_interrupt, 0x36f12, 1, { // end of timeline ECL ontick
        Enemy* boss = *BOSS_PTR;

        if (boss && boss->bossTimer.current >= triggerFrame) {
            boss->curHealth = healthOverride;
            self->Disable();
        }

        OG_INS(pCtx->Rip = PopHelper(pCtx));
    });

    void TriggerHealthInterrupt(int32_t threshold, int32_t atBossFrame = 0) {
        th06nc_trigger_health_interrupt.Enable();
        healthOverride = threshold - 1;
        triggerFrame = atBossFrame;
    }

    void LoadANMFile(const char* filename, int32_t anmID, int32_t spriteIndexOffset) {
        asm_call_rel<LOAD_ANM_FILE, Thiscall>(ANM_MANAGER_PTR, anmID, filename, spriteIndexOffset);
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

            case TH06_ST1_MID2: { // Midspell (NHL)
                constexpr uint32_t st1mbsSpellMoveInterp = 0x1448;

                ECLWarp(st1MidbossTime);
                TriggerHealthInterrupt(500);
                ECLSetArgs(ecl, st1mbsSpellMoveInterp, pair{ 0, 0 });
                break;
            }

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
            case TH06_ST2_MID1: // Midboss
                ECLWarp(st2MidbossTime);
                break;

            case TH06_ST2_BOSS1: { // Non 1
                constexpr uint32_t st2BossDlgTime = 5893;
                thPracParam.dlg ? ECLWarp(st2BossDlgTime) : s2_boss_warp_skip_fadein();
                break;
            }

            case TH06_ST2_BOSS2: { // Spell 1
                constexpr uint32_t st2bsNon1TimeThreshold = 0x18ec;

                s2_boss_warp_skip_fadein();
                ECLSetArgs(ecl, st2bsNon1TimeThreshold, pair{ 0, 0 });
                break;
            }

            case TH06_ST2_BOSS3: // Non 2
                s2_boss_non2_warp();
                break;

            case TH06_ST2_BOSS4: // Spell 2
                s2_boss_non2_warp();
                ECLSetArgs(ecl, st2bsNon2TimeThreshold, pair{ 0, 0 });
                break;

            case TH06_ST2_BOSS5: { // Spell 3
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
            case TH06_ST3_MID1: // Midnon
                ECLWarp(st3MidbossTime);
                break;

            case TH06_ST3_MID2: { // Midspell
                constexpr uint32_t st3mbsSub10Call = 0x1088;
                constexpr uint32_t st3mbsSpellMoveInterp = 0x1c90;

                ECLWarp(st3MidbossTime);
                TriggerHealthInterrupt(1300);
                ECLDisable(ecl, st3mbsSub10Call); // makes boss intangible since we skip to the spell while it's executing
                ECLSetArgs(ecl, st3mbsSpellMoveInterp, pair{ 0, 0 });
                break;
            }

            case TH06_ST3_BOSS1: // Non 1
                thPracParam.dlg ? ECLWarp(st3BossTime) : s3_boss_warp_skip_setup();
                break;

            case TH06_ST3_BOSS2: { // Spell 1
                constexpr uint32_t st3bsNon1TimeThreshold = 0x2360;

                s3_boss_warp_skip_setup();
                ECLSetArgs(ecl, st3bsNon1TimeThreshold, pair{ 0, 0 });
                break;
            }

            case TH06_ST3_BOSS4: { // Spell 2 (HL)
                constexpr uint32_t st3bsNon2TimeThreshold = 0x290c;
                ECLSetArgs(ecl, st3bsNon2TimeThreshold, pair{ 0, 0 });
                [[fallthrough]];
            }
            case TH06_ST3_BOSS3: { // Non 2
                constexpr uint32_t st3bsNon2ItemDrop = 0x294c;

                s3_boss_warp_skip_setup();
                ECLMakeIns(ecl, st3bsNon1FirstDelayedIns, 0, CALL, pair{ 0, 24 }); // call sub 24 (non2)
                ECLDisable(ecl, st3bsNon2ItemDrop);
                break;
            }

            case TH06_ST3_BOSS5: { // Non 3
                s3_boss_non3_warp();
                break;
            }

            case TH06_ST3_BOSS6: // Spell 3
                s3_boss_non3_warp();
                ECLSetArgs(ecl, st2bsNon3TimeThreshold, pair{ 0, 0 });
                break;

            case TH06_ST3_BOSS7: { // Spell 4 (NHL)
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
            constexpr uint32_t st4bsNon1FirstDelayedIns = 0x2890;
            constexpr uint32_t st4bsNon3FirstDelayedIns = 0x7c4c;

            auto s4_boss_warp_skip_move = [&]() {
                constexpr uint32_t st4BossTime = 10511;
                constexpr uint32_t st4BossMoveInterp = 0x2310;

                ECLWarp(st4BossTime);
                ECLSetArgs(ecl, st4BossMoveInterp, pair{ 0, 0 });
            };

            auto s4_boss_non3_warp = [&]() {
                constexpr uint32_t st4bsNon3ItemDrop = 0x7a9c;

                s4_boss_warp_skip_move();
                ECLMakeIns(ecl, st4bsNon1FirstDelayedIns, 0, CALL, pair{ 0, 39 }); // call sub 39 (non3)
                ECLDisable(ecl, st4bsNon3ItemDrop);
            };

            auto s4_boss_post_non3_warp = [&](int subNum) {
                s4_boss_non3_warp();

                // trigger non 4/5 via timer threshold to set health to expected sp2/3 amount
                ECLMakeIns(ecl, st4bsNon3FirstDelayedIns, 0, TIMER_CALLBACK, pair{ 0, subNum });
                ECLMakeIns(ecl, st4bsNon3FirstDelayedIns + TIMER_CALLBACK.size, 0, TIMER_THRESHOLD, pair{ 0, 0 });
                ECLMakeIns(ecl, st4bsNon3FirstDelayedIns + TIMER_CALLBACK.size + TIMER_THRESHOLD.size, 1, NOP);
            };

            switch (section) {
            case TH06_ST4_BOOKS: { // Books
                constexpr uint32_t st4BooksTime = 3378 - 40;
                ECLWarp(st4BooksTime);
                break;
            }

            case TH06_ST4_MID1: { // Midboss
                constexpr uint32_t st4MidbossTime = 4058;
                ECLWarp(st4MidbossTime);
                ecl << pair{ 0x24c0 + 0xc, 6942069 };
                break;
            }

            case TH06_ST4_BOSS1: { // Non 1
                constexpr uint32_t st4BossDlgTime = 10510;
                thPracParam.dlg ? ECLWarp(st4BossDlgTime) : s4_boss_warp_skip_move();
                break;
            }

            case TH06_ST4_BOSS2: // Spell 1
                s4_boss_warp_skip_move();
                ECLMakeIns(ecl, st4bsNon1FirstDelayedIns, 0, TIMER_THRESHOLD, pair{ 0, 0 });
                break;

            case TH06_ST4_BOSS3: { // Spell 2
                constexpr uint32_t st4bsNon2FirstDelayedIns = 0x7568;

                ECLMakeIns(ecl, st4bsNon2FirstDelayedIns, 0, TIMER_THRESHOLD, pair{ 0, 0 });
                ECLMakeIns(ecl, st4bsNon2FirstDelayedIns + TIMER_THRESHOLD.size, 1, NOP);
                [[fallthrough]];
            }
            case TH06_ST4_BOSS4: { // Non 2
                constexpr uint32_t st4bsNon2ItemDrop = 0x6ec4;

                s4_boss_warp_skip_move();
                ECLMakeIns(ecl, st4bsNon1FirstDelayedIns, 0, CALL, pair{ 0, 37 }); // call sub 37 (non2)
                ECLDisable(ecl, st4bsNon2ItemDrop);
                break;
            }

            case TH06_ST4_BOSS5: { // Spell 3
                constexpr uint32_t st4bsNon3DelayedIns2 = 0x7c5c;
                constexpr uint32_t st4bsNon3DelayedIns3 = 0x7c7c;
                constexpr uint32_t st4bsNon3DelayedIns4 = 0x7c9c;
                constexpr uint32_t st4bsNon3DelayedIns5 = 0x7cbc;
                constexpr uint32_t st4bsNon3DelayedIns6 = 0x7cdc;

                s4_boss_non3_warp();
                ECLSetInsTime(ecl, st4bsNon3FirstDelayedIns, 0);
                ECLSetInsTime(ecl, st4bsNon3DelayedIns2, 0);
                ECLSetInsTime(ecl, st4bsNon3DelayedIns3, 0);
                ECLSetInsTime(ecl, st4bsNon3DelayedIns4, 0);
                ECLSetInsTime(ecl, st4bsNon3DelayedIns5, 0);
                ECLSetInsTime(ecl, st4bsNon3DelayedIns6, 0);
                break;
            }

            case TH06_ST4_BOSS6: { // Spell 4
                constexpr uint32_t st4bsNon4ItemDrop = 0x7d2c;
                constexpr uint32_t st4bsNon4Particle = 0x7e58;
                constexpr uint32_t st4bsNon4DelayedIns1 = 0x7e80;
                constexpr uint32_t st4bsNon4DelayedIns2 = 0x7e90;
                constexpr uint32_t st4bsNon4DelayedIns3 = 0x7eb0;
                constexpr uint32_t st4bsNon4DelayedIns4 = 0x7ed0;
                constexpr uint32_t st4bsNon4DelayedIns5 = 0x7ef0;
                constexpr uint32_t st4bsNon4DelayedIns6 = 0x7f10;

                s4_boss_post_non3_warp(40); // non 4
                ECLDisable(ecl, st4bsNon4ItemDrop);
                ECLDisable(ecl, st4bsNon4Particle);
                ECLSetInsTime(ecl, st4bsNon4DelayedIns1, 0);
                ECLSetInsTime(ecl, st4bsNon4DelayedIns2, 0);
                ECLSetInsTime(ecl, st4bsNon4DelayedIns3, 0);
                ECLSetInsTime(ecl, st4bsNon4DelayedIns4, 0);
                ECLSetInsTime(ecl, st4bsNon4DelayedIns5, 0);
                ECLSetInsTime(ecl, st4bsNon4DelayedIns6, 0);
                break;
            }

            case TH06_ST4_BOSS7: { // Spell 5
                constexpr uint32_t st4bsNon3HealthThresholdHL = 0x7ba8;
                constexpr uint32_t st4bsNon5ItemDrop = 0x7f60;
                constexpr uint32_t st4bsNon5Particle = 0x804c;
                constexpr uint32_t st4bsNon5DelayedIns1 = 0x8074;
                constexpr uint32_t st4bsNon5DelayedIns2 = 0x8084;
                constexpr uint32_t st4bsNon5DelayedIns3 = 0x80a4;
                constexpr uint32_t st4bsNon5DelayedIns4 = 0x80c4;
                constexpr uint32_t st4bsNon5DelayedIns5 = 0x80e4;
                constexpr uint32_t st4bsNon5DelayedIns6 = 0x8104;

                ECLSetArgs(ecl, st4bsNon3HealthThresholdHL, pair{ 0, 1700 });
                s4_boss_post_non3_warp(41); // non 5

                ECLDisable(ecl, st4bsNon5ItemDrop);
                ECLDisable(ecl, st4bsNon5Particle);
                ECLSetInsTime(ecl, st4bsNon5DelayedIns1, 0);
                ECLSetInsTime(ecl, st4bsNon5DelayedIns2, 0);
                ECLSetInsTime(ecl, st4bsNon5DelayedIns3, 0);
                ECLSetInsTime(ecl, st4bsNon5DelayedIns4, 0);
                ECLSetInsTime(ecl, st4bsNon5DelayedIns5, 0);
                ECLSetInsTime(ecl, st4bsNon5DelayedIns6, 0);
                break;
            }

            default: break;
            }
            break;
        }

        case 4: { // Stage 5
            constexpr uint32_t st5MidbossTime = 3272;
            constexpr uint32_t st5BossTime = 7604;
            constexpr uint32_t st5bsNon1FirstDelayedIns = 0x24a4;

            auto s5_midboss_warp_skip_move = [&]() {
                constexpr uint32_t st5MidbossDialogRead = 0x7944;
                constexpr uint32_t st5MidbossMoveInterp = 0x1360;

                ECLWarp(st5MidbossTime);
                ECLDisable(ecl, st5MidbossDialogRead, true);
                ECLSetArgs(ecl, st5MidbossMoveInterp, pair{ 0, 0 });
            };

            auto s5_boss_warp_skip_move = [&]() {
                constexpr uint32_t st5BossDialogRead = 0x8b1c;
                constexpr uint32_t st5BossMoveInterp = 0x22dc;
                constexpr uint32_t st5bsNon1Particle = 0x23f0;

                ECLWarp(st5BossTime);
                ECLDisable(ecl, st5BossDialogRead, true);
                ECLSetArgs(ecl, st5BossMoveInterp, pair{ 0, 0 });
                ECLDisable(ecl, st5bsNon1Particle);
            };

            switch (section) {
            case TH06_ST5_MID1: // Midboss
                thPracParam.dlg ? ECLWarp(st5MidbossTime) : s5_midboss_warp_skip_move();
                break;

            case TH06_ST5_MID2: { // Midspell
                constexpr uint32_t st5mbsInteractable = 0x1524;

                s5_midboss_warp_skip_move();
                ECLSetInsTime(ecl, st5mbsInteractable, 0);
                TriggerHealthInterrupt(710);
                break;
            }

            case TH06_ST5_BOSS1: // Non 1
                thPracParam.dlg ? ECLWarp(st5BossTime) : s5_boss_warp_skip_move();
                break;

            case TH06_ST5_BOSS2: { // Spell 1
                constexpr uint32_t st5bsNon1TimeThreshold = 0x2404;

                s5_boss_warp_skip_move();
                ECLSetArgs(ecl, st5bsNon1TimeThreshold, pair{ 0, 0 });
                break;
            }

            case TH06_ST5_BOSS4: { // Spell 2
                constexpr uint32_t st5bsNon2TimeThreshold = 0x3a44;
                ECLSetArgs(ecl, st5bsNon2TimeThreshold, pair{ 0, 0 });
                [[fallthrough]];
            }
            case TH06_ST5_BOSS3: { // Non 2
                constexpr uint32_t st5bsNon2ItemDrop = 0x3980;
                constexpr uint32_t st5bsNon2Particle = 0x3a30;

                s5_boss_warp_skip_move();
                ECLMakeIns(ecl, st5bsNon1FirstDelayedIns, 0, CALL, pair{ 0, 36 }); // call sub 36 (non2)
                ECLDisable(ecl, st5bsNon2ItemDrop);
                ECLDisable(ecl, st5bsNon2Particle);
                break;
            }

            case TH06_ST5_BOSS6: { // Spell 3
                constexpr uint32_t st5bsNon3TimeThreshold = 0x49cc;
                ECLSetArgs(ecl, st5bsNon3TimeThreshold, pair{ 0, 0 });
                [[fallthrough]];
            }
            case TH06_ST5_BOSS5: { // Non 3
                constexpr uint32_t st5bsNon3ItemDrop = 0x4908;
                constexpr uint32_t st5bsNon3Particle = 0x49b8;

                s5_boss_warp_skip_move();
                ECLMakeIns(ecl, st5bsNon1FirstDelayedIns, 0, CALL, pair{ 0, 43 }); // call sub 43 (non3)
                ECLDisable(ecl, st5bsNon3ItemDrop);
                ECLDisable(ecl, st5bsNon3Particle);
                break;
            }

            default: break;
            }
            break;
        }

        case 5: { // Stage 6
            constexpr uint32_t st6MidbossTime = 2493;
            constexpr uint32_t st6bsNon1FirstDelayedIns = 0x1834;

            auto s6_midboss_warp_skip_move = [&]() {
                constexpr uint32_t st6MidbossDialogRead = 0x9584;
                constexpr uint32_t st6MidbossMoveInterp = 0xa80;

                ECLWarp(st6MidbossTime);
                ECLDisable(ecl, st6MidbossDialogRead, true);
                ECLSetArgs(ecl, st6MidbossMoveInterp, pair{ 0, 0 });
            };

            auto s6_boss_warp_skip_move = [&]() {
                constexpr uint32_t st6BossTime = 3098;
                constexpr uint32_t st6BossMoveInterp = 0x1618;
                constexpr uint32_t st6bsNon1Particle = 0x1780;

                ECLWarp(st6BossTime);
                ECLSetArgs(ecl, st6BossMoveInterp, pair{ 0, 0 });
                ECLDisable(ecl, st6bsNon1Particle);
                LoadANMFile("data/eff06.anm", BACKGROUND, 0x2d3);
            };

            switch (section) {
            case TH06_ST6_MID1: // Midboss
                thPracParam.dlg ? ECLWarp(st6MidbossTime) : s6_midboss_warp_skip_move();
                break;

            case TH06_ST6_MID2: { // Midspell
                constexpr uint32_t st6mbsPreInteractable = 0xdd0;
                constexpr uint32_t st6mbsInteractable = 0xde0;

                s6_midboss_warp_skip_move();
                ECLSetInsTime(ecl, st6mbsPreInteractable, 0);
                ECLSetInsTime(ecl, st6mbsInteractable, 0);
                TriggerHealthInterrupt(750); // lowest (interrupt corrects health)
                break;
            }

            case TH06_ST6_BOSS1: { // Non 1
                constexpr uint32_t st6BossDlgTime = 3096;
                thPracParam.dlg ? ECLWarp(st6BossDlgTime) : s6_boss_warp_skip_move();
                break;
            }

            case TH06_ST6_BOSS2: { // Spell 1
                constexpr uint32_t st6bsNon1TimeThreshold = 0x1794;

                s6_boss_warp_skip_move();
                ECLSetArgs(ecl, st6bsNon1TimeThreshold, pair{ 0, 0 });
                break;
            }

            case TH06_ST6_BOSS4: { // Spell 2
                constexpr uint32_t st6bsNon2TimeThreshold = 0x1e14;
                ECLSetArgs(ecl, st6bsNon2TimeThreshold, pair{ 0, 0 });
                [[fallthrough]];
            }
            case TH06_ST6_BOSS3: { // Non 2
                constexpr uint32_t st6bsNon2ItemDrop = 0x1d38;
                constexpr uint32_t st6bsNon2Particle = 0x1e00;

                s6_boss_warp_skip_move();
                ECLMakeIns(ecl, st6bsNon1FirstDelayedIns, 0, CALL, pair{ 0, 21 }); // call sub 21 (non2)
                ECLDisable(ecl, st6bsNon2ItemDrop);
                ECLDisable(ecl, st6bsNon2Particle);
                break;
            }

            case TH06_ST6_BOSS6: { // Spell 3
                constexpr uint32_t st6bsNon3TimeThreshold = 0x2bf8;
                ECLSetArgs(ecl, st6bsNon3TimeThreshold, pair{ 0, 0 });
                [[fallthrough]];
            }
            case TH06_ST6_BOSS5: { // Non 3
                constexpr uint32_t st6bsNon3ItemDrop = 0x2b1c;
                constexpr uint32_t st6bsNon3Particle = 0x2be4;

                s6_boss_warp_skip_move();
                ECLMakeIns(ecl, st6bsNon1FirstDelayedIns, 0, CALL, pair{ 0, 25 }); // call sub 25 (non3)
                ECLDisable(ecl, st6bsNon3ItemDrop);
                ECLDisable(ecl, st6bsNon3Particle);
                break;
            }

            case TH06_ST6_BOSS8: { // Spell 4
                constexpr uint32_t st6bsNon4TimeThreshold = 0x30bc;
                ECLSetArgs(ecl, st6bsNon4TimeThreshold, pair{ 0, 0 });
                [[fallthrough]];
            }
            case TH06_ST6_BOSS7: { // Non 4
                constexpr uint32_t st6bsNon4ItemDrop = 0x2fe0;
                constexpr uint32_t st6bsNon4Particle = 0x30a8;

                s6_boss_warp_skip_move();
                ECLMakeIns(ecl, st6bsNon1FirstDelayedIns, 0, CALL, pair{ 0, 28 }); // call sub 28 (non4)
                ECLDisable(ecl, st6bsNon4ItemDrop);
                ECLDisable(ecl, st6bsNon4Particle);
                break;
            }

            case TH06_ST6_BOSS9: { // Spell 5
                constexpr uint32_t st6bsNon1HealthThreshold = 0x17c4;
                constexpr uint32_t st6bsNNon5ItemDrop = 0x6720;
                constexpr uint32_t st6bsNNon5DelayedIns1 = 0x6800;
                constexpr uint32_t st6bsNNon5DelayedIns2 = 0x6810;
                constexpr uint32_t st6bsNNon5DelayedIns3 = 0x6820;
                constexpr uint32_t st6bsNNon5DelayedIns4 = 0x6834;
                constexpr uint32_t st6bsNNon5DelayedIns5 = 0x684c;

                constexpr uint32_t st6bsHLNon5ItemDrop = 0x6e24;
                constexpr uint32_t st6bsHLNon5DelayedIns1 = 0x6f04;
                constexpr uint32_t st6bsHLNon5DelayedIns2 = 0x6f14;
                constexpr uint32_t st6bsHLNon5DelayedIns3 = 0x6f24;
                constexpr uint32_t st6bsHLNon5DelayedIns4 = 0x6f38;
                constexpr uint32_t st6bsHLNon5DelayedIns5 = 0x6f50;

                s6_boss_warp_skip_move();
                ECLSetArgs(ecl, st6bsNon1HealthThreshold, pair{0, -1});

                if (GAME_MANAGER->difficulty <= 1) {
                    ECLMakeIns(ecl, st6bsNon1FirstDelayedIns, 0, CALL, pair{ 0, 61 }); // call sub 61 (non5 for N)

                    ECLDisable(ecl, st6bsNNon5ItemDrop);
                    ECLSetInsTime(ecl, st6bsNNon5DelayedIns1, 0);
                    ECLSetInsTime(ecl, st6bsNNon5DelayedIns2, 0);
                    ECLSetInsTime(ecl, st6bsNNon5DelayedIns3, 0);
                    ECLSetInsTime(ecl, st6bsNNon5DelayedIns4, 0);
                    ECLSetInsTime(ecl, st6bsNNon5DelayedIns5, 0);

                } else {
                    ECLMakeIns(ecl, st6bsNon1FirstDelayedIns, 0, CALL, pair{ 0, 64 }); // call sub 64 (non5 for HL)

                    ECLDisable(ecl, st6bsHLNon5ItemDrop);
                    ECLSetInsTime(ecl, st6bsHLNon5DelayedIns1, 0);
                    ECLSetInsTime(ecl, st6bsHLNon5DelayedIns2, 0);
                    ECLSetInsTime(ecl, st6bsHLNon5DelayedIns3, 0);
                    ECLSetInsTime(ecl, st6bsHLNon5DelayedIns4, 0);
                    ECLSetInsTime(ecl, st6bsHLNon5DelayedIns5, 0);
                }
                break;
            }

            default: break;
            }
            break;
        }

        case 6: { // Extra Stage
            constexpr uint32_t st7MidbossTime = 4640;
            constexpr uint32_t st7MidbossFirstSub = 0x1ad4;
            constexpr uint32_t st7bsNon1FirstDelayedIns = 0x360e;
            constexpr uint32_t st7TLBFirstSubCall = 0xd2f2;

            auto ex_midboss_warp_skip_move = [&]() {
                constexpr uint32_t st7MidbossDialogRead = 0x11800;
                constexpr uint32_t st7MidbossMoveInterp = 0x1ab8;

                ECLWarp(st7MidbossTime);
                ECLDisable(ecl, st7MidbossDialogRead, true);
                ECLSetArgs(ecl, st7MidbossMoveInterp, pair{ 0, 0 });
            };

            auto ex_boss_warp_skip_move = [&]() {
                constexpr uint32_t st7BossTime = 8494;
                constexpr uint32_t st7BossMoveInterp = 0x341a;

                ECLWarp(st7BossTime);
                ECLSetArgs(ecl, st7BossMoveInterp, pair{ 0, 0 });
                LoadANMFile("data/eff07.anm", BACKGROUND, 0x2d3);
            };

            auto ex_tlb_warp_skip_anim = [&]() {
                constexpr uint32_t st7TLBTime = 8500;
                constexpr uint32_t st7TLBInstantEnmCreate = 0xcd0a;

                ECLWarp(st7TLBTime);
                LoadANMFile("data/eff07.anm", BACKGROUND, 0x2d3);
                LoadANMFile("data/frame_stage7_1.anm", STAGE_ILLUST_MAIN, 0x782);
                ECLDisable(ecl, st7TLBInstantEnmCreate);

                uintptr_t difficultyVM = GetMemAddr((uintptr_t)ZUN_GUI + 0x38, 0x28e0);
                asm_call_rel<ANM_VM_SET_SPRITE, Thiscall>(*(uintptr_t*)ANM_MANAGER_PTR, difficultyVM, 0x647);
            };

            auto ex_adjust_times = [&](uint32_t start, uint32_t end, int32_t skipTo, int32_t startDelay = 0) {
                uint32_t curIns = start;
                int32_t timeAcc = startDelay;
                int32_t prevInsTime = 0;
                int32_t curInsTime;
                int16_t curInsSize;

                while (curIns <= end) {
                    ecl.SetPos(curIns);
                    ecl >> curInsTime;
                    ecl.SetPos(curIns + 0x6);
                    ecl >> curInsSize;

                    if (curInsTime < skipTo) {
                        ECLSetInsTime(ecl, curIns, 0);
                        ECLDisable(ecl, curIns);

                    } else {
                        if (prevInsTime)
                            timeAcc += (curInsTime - prevInsTime);

                        ECLSetInsTime(ecl, curIns, timeAcc);
                        prevInsTime = curInsTime;
                    }

                    curIns = curIns + curInsSize;
                }
            };

            switch (section) {
            case TH06_ST7_MID1: // Midspell 1
                thPracParam.dlg ? ECLWarp(st7MidbossTime) : ex_midboss_warp_skip_move();
                break;

            case TH06_ST7_MID2: { // Midspell 2
                constexpr uint32_t st7mbsNon2ItemDrop = 0x1d54;

                ex_midboss_warp_skip_move();
                ECLSetArgs(ecl, st7MidbossFirstSub, pair{ 0, 18 }); // sub 18
                ECLDisable(ecl, st7mbsNon2ItemDrop);
                break;
            }

            case TH06_ST7_MID3: { // Midspell 3
                constexpr uint32_t st7mbsNon3ItemDrop = 0x1ea4;

                ex_midboss_warp_skip_move();
                ECLSetArgs(ecl, st7MidbossFirstSub, pair{ 0, 19 }); // sub 19
                ECLDisable(ecl, st7mbsNon3ItemDrop);
                break;
            }

            case TH06_ST7_END_NS1: { // Non 1
                constexpr uint32_t st7BossDlgTime = 8493;
                thPracParam.dlg ? ECLWarp(st7BossDlgTime) : ex_boss_warp_skip_move();
                break;
            }

            case TH06_ST7_END_S1: { // Spell 1
                constexpr uint32_t st7bsNon1TimeThreshold = 0x3572;

                ex_boss_warp_skip_move();
                ECLSetArgs(ecl, st7bsNon1TimeThreshold, pair{ 0, 0 });
                break;
            }

            case TH06_ST7_END_S2: { // Spell 2
                constexpr uint32_t st7bsNon2TimeThreshold = 0x443c;
                ECLSetArgs(ecl, st7bsNon2TimeThreshold, pair{ 0, 0 });
                [[fallthrough]];
            }
            case TH06_ST7_END_NS2: { // Non 2
                constexpr uint32_t st7bsNon2ItemDrop = 0x44e8;

                ex_boss_warp_skip_move();
                ECLMakeIns(ecl, st7bsNon1FirstDelayedIns, 0, CALL, pair{ 0, 43 }); // call sub 43 (non2)
                ECLDisable(ecl, st7bsNon2ItemDrop);
                break;
            }

            case TH06_ST7_END_S3: { // Spell 3
                constexpr uint32_t st7bsNon3TimeThreshold = 0x4fba;
                ECLSetArgs(ecl, st7bsNon3TimeThreshold, pair{ 0, 0 });
                [[fallthrough]];
            }
            case TH06_ST7_END_NS3: { // Non 3
                constexpr uint32_t st7bsNon3ItemDrop = 0x5066;

                ex_boss_warp_skip_move();
                ECLMakeIns(ecl, st7bsNon1FirstDelayedIns, 0, CALL, pair{ 0, 48 }); // call sub 48 (non3)
                ECLDisable(ecl, st7bsNon3ItemDrop);
                break;
            }

            case TH06_ST7_END_S4: { // Spell 4
                constexpr uint32_t st7bsNon4TimeThreshold = 0x5d90;
                ECLSetArgs(ecl, st7bsNon4TimeThreshold, pair{ 0, 0 });
                [[fallthrough]];
            }
            case TH06_ST7_END_NS4: { // Non 4
                constexpr uint32_t st7bsNon4ItemDrop = 0x5e3c;

                ex_boss_warp_skip_move();
                ECLMakeIns(ecl, st7bsNon1FirstDelayedIns, 0, CALL, pair{ 0, 55 }); // call sub 55 (non4)
                ECLDisable(ecl, st7bsNon4ItemDrop);
                break;
            }

            case TH06_ST7_END_S5: { // Spell 5
                constexpr uint32_t st7bsNon5TimeThreshold = 0x67d2;
                ECLSetArgs(ecl, st7bsNon5TimeThreshold, pair{ 0, 0 });
                [[fallthrough]];
            }
            case TH06_ST7_END_NS5: { // Non 5
                constexpr uint32_t st7bsNon5ItemDrop = 0x687e;

                ex_boss_warp_skip_move();
                ECLMakeIns(ecl, st7bsNon1FirstDelayedIns, 0, CALL, pair{ 0, 61 }); // call sub 61 (non5)
                ECLDisable(ecl, st7bsNon5ItemDrop);
                break;
            }

            case TH06_ST7_END_S6: { // Spell 6
                constexpr uint32_t st7bsNon6TimeThreshold = 0x6fb8;
                ECLSetArgs(ecl, st7bsNon6TimeThreshold, pair{ 0, 0 });
                [[fallthrough]];
            }
            case TH06_ST7_END_NS6: { // Non 6
                constexpr uint32_t st7bsNon6ItemDrop = 0x7064;

                ex_boss_warp_skip_move();
                ECLMakeIns(ecl, st7bsNon1FirstDelayedIns, 0, CALL, pair{ 0, 65 }); // call sub 65 (non6)
                ECLDisable(ecl, st7bsNon6ItemDrop);
                break;
            }

            case TH06_ST7_END_S7: { // Spell 7
                constexpr uint32_t st7bsNon7TimeThreshold = 0x7db2;
                ECLSetArgs(ecl, st7bsNon7TimeThreshold, pair{ 0, 0 });
                [[fallthrough]];
            }
            case TH06_ST7_END_NS7: { // Non 7
                constexpr uint32_t st7bsNon7ItemDrop = 0x7e5e;

                ex_boss_warp_skip_move();
                ECLMakeIns(ecl, st7bsNon1FirstDelayedIns, 0, CALL, pair{ 0, 71 }); // call sub 71 (non7)
                ECLDisable(ecl, st7bsNon7ItemDrop);
                break;
            }

            case TH06_ST7_END_S8: { // Spell 8
                constexpr uint32_t st7bsNon8TimeThreshold = 0x8a7c;
                ECLSetArgs(ecl, st7bsNon8TimeThreshold, pair{ 0, 0 });
                [[fallthrough]];
            }
            case TH06_ST7_END_NS8: { // Non 8
                constexpr uint32_t st7bsNon8ItemDrop = 0x8b28;

                ex_boss_warp_skip_move();
                ECLMakeIns(ecl, st7bsNon1FirstDelayedIns, 0, CALL, pair{ 0, 76 }); // call sub 76 (non8)
                ECLDisable(ecl, st7bsNon8ItemDrop);
                break;
            }

            case TH06_ST7_END_S9: { // Spell 9 (Timeout)
                constexpr uint32_t st7bsNon9ItemDrop = 0x9a96;
                constexpr uint32_t st7bsNon9TimeThreshold = 0x99ea;
                constexpr uint32_t st7bsTimeoutTimeThreshold = 0x9c64;
                constexpr uint32_t st7bsTimeoutStart = 0x9d50;
                constexpr uint32_t st7bsTimeoutEnd = 0x9f78;

                ex_boss_warp_skip_move();
                ECLMakeIns(ecl, st7bsNon1FirstDelayedIns, 0, CALL, pair{ 0, 81 }); // call sub 81 (non9)
                ECLDisable(ecl, st7bsNon9ItemDrop);
                ECLSetArgs(ecl, st7bsNon9TimeThreshold, pair{ 0, 0 });

                switch (thPracParam.phase) {
                case 1:
                    ex_adjust_times(st7bsTimeoutStart, st7bsTimeoutEnd, 1856, 45);
                    ECLSetArgs(ecl, st7bsTimeoutTimeThreshold, pair{ 0, 5160 - 1856 + 45 });
                    break;

                case 2:
                    ex_adjust_times(st7bsTimeoutStart, st7bsTimeoutEnd, 4416, 45);
                    ECLSetArgs(ecl, st7bsTimeoutTimeThreshold, pair{ 0, 5160 - 4416 + 45 });
                    break;
                }
                break;
            }

            case TH06_ST7_END_S10: { // Spell 10 (QED)
                constexpr uint32_t st7bsNon10ItemDrop = 0xc58c;
                constexpr uint32_t st7bsNon10TimeThreshold = 0xc4f0;

                ex_boss_warp_skip_move();
                ECLMakeIns(ecl, st7bsNon1FirstDelayedIns, 0, CALL, pair{ 0, 91 }); // call sub 91 (non10)
                ECLDisable(ecl, st7bsNon10ItemDrop);
                ECLSetArgs(ecl, st7bsNon10TimeThreshold, pair{ 0, 0 });

                // for rage phase warp, cf. th06nc_qed_phase_warp
                // (yes, QED is mostly hardcoded lol)

                if (thPracParam.phase == 1) {
                    constexpr uint32_t st07bsSpell10ParticleLoop = 0xc8aa;
                    ECLDisable(ecl, st07bsSpell10ParticleLoop);
                    TriggerHealthInterrupt(1800, 3); // (6000f * 30%)
                }
                break;
            }

            case TH06NC_TLB1: {
                constexpr uint32_t st7TLBDlgTime = 8497;
                constexpr uint32_t st7TLB1SetWingLength = 0xd54c;

                if (thPracParam.dlg) {
                    ECLWarp(st7TLBDlgTime);
                    LoadANMFile("data/eff07.anm", BACKGROUND, 0x2d3);
                }
                else ex_tlb_warp_skip_anim();

                if (thPracParam.phase)
                    ECLSetArgs(ecl, st7TLB1SetWingLength, pair{ 0x4, 140 });
                break;
            }

            case TH06NC_TLB2: {
                constexpr uint32_t st7TLB2NonFirstDelayedIns = 0xde1c;
                constexpr uint32_t st7TLB2SetAimedBubbleDelay = 0xe062;

                ex_tlb_warp_skip_anim();
                ECLSetArgs(ecl, st7TLBFirstSubCall, pair{ 0, 106 }); // sub 106 (TLB2 fake non)
                ECLMakeIns(ecl, st7TLB2NonFirstDelayedIns, 0, CALL, pair{ 0, 107 }); // sub 107 (TLB2 start)

                if (thPracParam.phase)
                    ECLSetArgs(ecl, st7TLB2SetAimedBubbleDelay, pair{ 0x4, 100 });
                break;
            }

            case TH06NC_TLB3: {
                constexpr uint32_t st7TLB3NonFirstDelayedIns = 0xe8f2;

                ex_tlb_warp_skip_anim();
                ECLSetArgs(ecl, st7TLBFirstSubCall, pair{ 0, 112 }); // sub 112 (TLB3 fake non)
                ECLMakeIns(ecl, st7TLB3NonFirstDelayedIns, 0, CALL, pair{ 0, 113 }); // sub 113 (TLB3 start)

                if (thPracParam.phase) {
                    // like QED, some of this spell is hardcoded (cf. th06nc_danmaku_heart_phase_warp)
                    // the very first wave behaves differently and has higher post-wave delay, so we skip it
                    constexpr uint32_t st7TLB3FirstWave1 = 0xeb40;
                    constexpr uint32_t st7TLB3FirstWave2 = 0xeb64;
                    constexpr uint32_t st7TLB3LoopStart = 0xeb88;
                    constexpr uint32_t st7TLB3LoopParticles = 0xebc4;
                    constexpr uint32_t st7TLB3LoopDelay = 0xed08;
                    constexpr uint32_t st7TLB3LoopEnd = 0xed20;

                    ex_adjust_times(st7TLB3LoopStart, st7TLB3LoopEnd, 600, 2);
                    ECLDisable(ecl, st7TLB3FirstWave1);
                    ECLDisable(ecl, st7TLB3FirstWave2);
                    ECLSetArgs(ecl, st7TLB3LoopParticles, pair{ 0, 2 });
                    ECLSetArgs(ecl, st7TLB3LoopDelay, pair{ 0, 4 });
                    ECLSetArgs(ecl, st7TLB3LoopEnd, pair{ 0, 2 });
                }
                break;
            }

            default: break;
            }
            break;
        }

        default: break;
        }
    }

    ImVec2 GetWindowSize() {
        float windowWidth = (float)GetMemContent<int32_t>(RVA(WINDOW_WIDTH));
        float windowHeight = (float)GetMemContent<int32_t>(RVA(WINDOW_HEIGHT));
        return { windowWidth, windowHeight };
    }

    float GetWindowScale() {
        return 1.15f * GetWindowSize().x / 1280.f;
    }





    HOOKSET_DEFINE(THMainHook)

    // Prac Menu UX
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

    EHOOK_DY(th06nc_setup_prac_screen, 0x4b062, 5, { // first check of difficulty upon returning to prac menu
        bool inPractice = GetMemContent<bool>(pCtx->Rsi + 0x168d1);
        if (inPractice) THGuiPrac::singleton().RestoreDifficulty();

        OG_INS(pCtx->Rcx = 0xad);
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

    EHOOK_DY(th06nc_resolution_change, 0x7c848, 1, { // window dimensions have changed
        ImVec2 size = GetWindowSize();
        float scale = GetWindowScale();

        ImGui::GetStyle() = ImGuiStyle();
        ImGui::GetStyle().ScaleAllSizes(scale);
        ImGui::GetStyle().MouseCursorScale = 1.0f;
        //Gui::LocaleCreateFont(16.f * scale);  // <- seems needed, but makes the prac window disappear...
        ImGui::GetIO().DisplaySize = size;
        THGuiPrac::singleton().RefreshLocale();

        OG_INS(pCtx->Rip = PopHelper(pCtx));
    })

    // On Prac (Re)Start
    EHOOK_DY(th06nc_patch_main, 0x3b69d, 1, { // end of GameManager::on_registration
        OG_INS(pCtx->Rip = PopHelper(pCtx));
        if (thPracParam.mode != 1) return;
        int32_t section = thPracParam.section;

        GAME_MANAGER->curPower = thPracParam.power;
        GAME_MANAGER->actualScore = GAME_MANAGER->visualScore = thPracParam.score;
        GAME_MANAGER->stageGraze = GAME_MANAGER->totalGraze = thPracParam.graze;
        GAME_MANAGER->stagePointItems = GAME_MANAGER->totalPointItems = thPracParam.point;
        GAME_MANAGER->livesRemaining = thPracParam.life;
        GAME_MANAGER->bombsRemaining = thPracParam.bomb;
        GAME_MANAGER->rank = thPracParam.rank;

        if (thPracParam.guaranteeTLB || (section >= TH06NC_TLB1 && section <= TH06NC_TLB3))
            GAME_MANAGER->spellCapsForTLB = 6;

        if (GAME_MANAGER->difficulty != 4) { // avoid triggering score extends
            if (thPracParam.score >= 60000000) GAME_MANAGER->scoreExtends = 4;
            else if (thPracParam.score >= 40000000) GAME_MANAGER->scoreExtends = 3;
            else if (thPracParam.score >= 20000000) GAME_MANAGER->scoreExtends = 2;
            else if (thPracParam.score >= 10000000) GAME_MANAGER->scoreExtends = 1;
        }

        if (section) THSectionPatch();
        else if (int32_t frame = thPracParam.frame) {
            if (frame > 60) PostStartWarpAdjustments();
            ECLWarp(frame);
        }
    })

    EHOOK_DY(th06nc_patchouli_fakeshot, 0x23a1d, 6, { // retrieving shottype ID in ECL
        if (thPracParam.mode && thPracParam.fakeType)
            *(int32_t*)RVA(0xa6ebbc) = thPracParam.fakeType - 1;
        else
            OG_INS(*(int32_t*)RVA(0xa6ebbc) = (int32_t)pCtx->Rcx);
    })

    EHOOK_DY(th06nc_patchouli_fakeshot_2, 0x333d0, 4, {  // retrieving patchy's last 3 spells
        if (thPracParam.mode && thPracParam.fakeType) {
            uintptr_t patchyLastSpellsEntry = RVA(PATCHY_LAST_SPELLS_TABLE) + (thPracParam.fakeType - 1) * 0xc;

            *(int32_t*)(pCtx->R8 + 0x4c) = GetMemContent<int32_t>(patchyLastSpellsEntry);
            *(int32_t*)(pCtx->R8 + 0x50) = GetMemContent<int32_t>(patchyLastSpellsEntry + 0x4);
            *(int32_t*)(pCtx->R8 + 0x54) = GetMemContent<int32_t>(patchyLastSpellsEntry + 0x8);
        }
        else OG_INS(*(int32_t*)(pCtx->R8 + 0x54) = (int32_t)pCtx->Rax);
    })

    EHOOK_DY(th06nc_qed_phase_warp, 0x35864, 7, { // QED retrieving boss health (or 0 if timer is under 2min)
        const Enemy* flandre = (Enemy*)pCtx->Rcx;

        if (thPracParam.mode && thPracParam.phase == 2)
            pCtx->Rax = 0;
        else
            OG_INS(pCtx->Rax = flandre->bossTimer.current < 7200 ? flandre->curHealth : 0);
    })

    EHOOK_DY(th06nc_danmaku_heart_phase_warp, 0x35a4a, 8, { // DH retrieving boss health (or 0 if timer is under 2min) (yes these 2 spells work the exact same way)
        const Enemy* flandre = (Enemy*)pCtx->Rcx;

        if (thPracParam.mode && thPracParam.phase)
            pCtx->Rax = 0;
        else
            OG_INS(pCtx->R8 = flandre->bossTimer.current < 7200 ? flandre->curHealth : 0);
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

        if (thPracParam.mode && section && section < 10000 && !thPracParam.dlg) {
            if (section >= TH06NC_TLB1) {
                pCtx->Rdx += 0x100; // flan TLB theme
                return;

            } else if (th_sections_bgm[section]) {
                pCtx->Rdx += 0x80;
                return;
            }
        }

        OG_INS(pCtx->Rdx += pCtx->R15);
    })

    EHOOK_DY(th06nc_bgm_title, 0x3e655, 7, { // after setting BGM name translation key
        OG_INS(pCtx->R13 = *(int64_t*)RVA(0xa6ec28));
        int32_t section = thPracParam.section;

        if (thPracParam.mode && section && section < 10000 && !thPracParam.dlg) {
            if (section >= TH06NC_TLB1)
                pCtx->Rax = 0x100;
            else if (th_sections_bgm[section])
                pCtx->Rax = 0x80;
        }
    })

    EHOOK_DY(th06nc_bgm_title_2, 0x3e74a, 8, { // before call to load BGM name ascii
        if (thPracParam.section == TH06NC_TLB1 && thPracParam.dlg)
            pCtx->Rip = RVA(0x3e757); // dont load it for TLB dialogue

        OG_INS(*(uint32_t*)(pCtx->Rsp + 0x20) = 0x10101);
    })

    // fix extra prac using wrong/invalid registers
    // note: [0x9a8, 0x9e0] inclusive should be unused spell prac registers (for Spell #1 on unreachable difficulties)
    EHOOK_DY(th06nc_practice_high_score_read, 0x3ae66, 4, { // initializing high score for this practice mode stage/diff/shot/char
        if (GAME_MANAGER->stage == 6) // game later adds 1 to this for reasons unknown
            pCtx->Rax = 0x12e + (GAME_MANAGER->character * 2 + GAME_MANAGER->subShot) * 2 + GAME_MANAGER->mode; // [0x12e, 0x135] -> [0x9a8, 0x9e0]
        else
            OG_INS(pCtx->Rax = pCtx->Rcx * 3);
    })
    EHOOK_DY(th06nc_practice_high_score_write, 0x74440, 4, { // retrieving practice high score register on run end
        if (GAME_MANAGER->stage == 7)
            pCtx->Rax = 0x12e + (GAME_MANAGER->character * 2 + GAME_MANAGER->subShot) * 2 + GAME_MANAGER->mode; // [0x12e, 0x135] -> [0x9a8, 0x9e0]
        else
            OG_INS(pCtx->Rax = pCtx->Rcx * 3);
    })
    HOOKSET_ENDDEF()

    static __declspec(noinline) void THGuiCreate() {
        if (ImGui::GetCurrentContext()) return;

        // Grab key globals
        GAME_MANAGER = (GameManager*)RVA(GAME_MANAGER_ADDR);
        PLAYER = (Player*)RVA(PLAYER_ADDR);
        STAGE_BACKGROUND = (StageBackground*)RVA(STAGE_BACKGROUND_ADDR);
        ANM_MANAGER_PTR = (void*)RVA(ANM_MANAGER_PTR_ADDR);
        ENEMY_MANAGER = (EnemyManager*)RVA(ENEMY_MANAGER_ADDR);
        ECL_MANAGER = (void*)RVA(ECL_MANAGER_ADDR);
        ZUN_GUI = (void*)RVA(ZUN_GUI_ADDR);
        BOSS_PTR = (Enemy**)RVA(BOSS_PTR_ADDR);

        // Init
        GameGuiInit(IMPL_WIN32_DX11, RVA(D3D_DEVICE_PTR), RVA(HWND_PTR),
                    Gui::INGAGME_INPUT_GEN1, RVA(INPUT_ADDR), RVA(INPUT_PREV_ADDR),
                    RVA(IS_EIGTH_FRAME_OF_HELD_INPUT_ADDR), GetWindowScale(), RVA(D3D_DEVICE_CONTEXT));
        ImGui::GetIO().DisplaySize = GetWindowSize();

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