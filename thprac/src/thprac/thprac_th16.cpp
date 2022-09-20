#include "thprac_utils.h"

namespace THPrac {
namespace TH16 {
    using std::pair;
    struct THPracParam {
        int32_t mode;
        int32_t stage;
        int32_t section;
        int32_t phase;

        int64_t score;
        int32_t life;
        int32_t bomb;
        int32_t bomb_fragment;
        int32_t season_gauge;
        int32_t power;
        int32_t value;
        int32_t graze;
        bool dlg;
        bool bug_fix;

        bool _playLock = false;
        void Reset()
        {
            memset(this, 0, sizeof(THPracParam));
        }
        bool ReadJson(std::string& json)
        {
            ParseJson();

            ForceJsonValue(game, "th16");
            GetJsonValue(mode);
            GetJsonValue(stage);
            GetJsonValue(section);
            GetJsonValue(phase);
            GetJsonValueEx(dlg, Bool);

            GetJsonValue(score);
            GetJsonValue(life);
            GetJsonValue(bomb);
            GetJsonValue(bomb_fragment);
            GetJsonValue(season_gauge);
            GetJsonValue(power);
            GetJsonValue(value);
            GetJsonValue(graze);
            GetJsonValueEx(bug_fix, Bool);

            return true;
        }
        std::string GetJson()
        {
            if (mode == 1) {
                CreateJson();

                AddJsonValueEx(version, GetVersionStr(), jalloc);
                AddJsonValueEx(game, "th16", jalloc);
                AddJsonValue(mode);
                AddJsonValue(stage);
                if (section)
                    AddJsonValue(section);
                if (phase)
                    AddJsonValue(phase);
                if (dlg)
                    AddJsonValue(dlg);

                AddJsonValue(score);
                AddJsonValue(life);
                AddJsonValue(bomb);
                AddJsonValue(bomb_fragment);
                AddJsonValue(season_gauge);
                AddJsonValue(power);
                AddJsonValue(value);
                AddJsonValue(graze);

                ReturnJson();
            } else if (mode == 2) {
                CreateJson();

                AddJsonValueEx(version, GetVersionStr(), jalloc);
                AddJsonValueEx(game, "th16", jalloc);
                AddJsonValue(mode);

                AddJsonValue(season_gauge);
                if (phase)
                    AddJsonValue(phase);
                if (bug_fix)
                    AddJsonValue(bug_fix);

                ReturnJson();
            }

            CreateJson();
            ReturnJson();
        }
    };
    THPracParam thPracParam {};
    int32_t thSubSeasonB = -1;

    class THGuiPrac : public Gui::GameGuiWnd {
        THGuiPrac() noexcept
        {
            *mMode = 1;
            *mLife = 9;
            *mBomb = 9;
            *mSeasonGauge = 6;
            *mPower = 400;
            *mValue = 10000;

            SetFade(0.8f, 0.1f);
            SetStyle(ImGuiStyleVar_WindowRounding, 0.0f);
            SetStyle(ImGuiStyleVar_WindowBorderSize, 0.0f);
            OnLocaleChange();
        }
        SINGLETON(THGuiPrac);
    public:

        __declspec(noinline) void State(int state)
        {
            switch (state) {
            case 0:
                break;
            case 1:
                mDiffculty = *((int32_t*)0x49f274);
                SetFade(0.8f, 0.1f);
                Open();
                thPracParam.Reset();
            case 2:
                break;
            case 3:
                SetFade(0.8f, 0.1f);
                Close();

                // Fill Param
                thPracParam.mode = *mMode;
                thPracParam.stage = *mStage;
                thPracParam.section = CalcSection();
                thPracParam.phase = SpellPhase() ? *mPhase : 0;
                if (SectionHasDlg(thPracParam.section))
                    thPracParam.dlg = *mDlg;

                thPracParam.score = *mScore;
                thPracParam.life = *mLife;
                thPracParam.bomb = *mBomb;
                thPracParam.bomb_fragment = *mBombFragment;
                thPracParam.season_gauge = *mSeasonGauge;
                thPracParam.power = *mPower;
                thPracParam.value = *mValue;
                thPracParam.graze = *mGraze;
                break;
            case 4:
                Close();
                *mNavFocus = 0;
                break;
            default:
                break;
            }
        }

    protected:
        virtual void OnLocaleChange() override
        {
            SetTitle(XSTR(TH_MENU));
            switch (Gui::LocaleGet()) {
            case Gui::LOCALE_ZH_CN:
                SetSizeRel(0.5f, 0.81f);
                SetPosRel(0.27f, 0.18f);
                SetItemWidthRel(-0.100f);
                SetAutoSpacing(true);
                break;
            case Gui::LOCALE_EN_US:
                SetSizeRel(0.6f, 0.79f);
                SetPosRel(0.215f, 0.18f);
                SetItemWidthRel(-0.100f);
                SetAutoSpacing(true);
                break;
            case Gui::LOCALE_JA_JP:
                SetSizeRel(0.56f, 0.81f);
                SetPosRel(0.230f, 0.18f);
                SetItemWidthRel(-0.105f);
                SetAutoSpacing(true);
                break;
            default:
                break;
            }
        }
        virtual void OnContentUpdate() override
        {
            ImGui::TextUnformatted(XSTR(TH_MENU));
            ImGui::Separator();

            PracticeMenu();
        }
        const th_glossary_t* SpellPhase()
        {
            auto section = CalcSection();
            if (section == TH16_ST6_SPRING_FINAL || section == TH16_ST6_SUMMER_FINAL || section == TH16_ST6_AUTUMN_FINAL || section == TH16_ST6_WINTER_FINAL || section == TH16_ST7_END_S10) {
                return TH_SPELL_PHASE2;
            }
            return nullptr;
        }
        void PracticeMenu()
        {
            mMode();
            if (mStage())
                *mSection = *mChapter = 0;
            if (*mMode == 1) {
                int mbs = -1;
                if (*mStage == 5) { // Counting from 0
                    mbs = 2;
                    if (*mWarp == 2)
                        *mWarp = 0;
                }
                if (mWarp(mbs))
                    *mSection = *mChapter = *mPhase = 0;
                if (*mWarp) {
                    SectionWidget();
                    mPhase(TH_PHASE, SpellPhase());
                }

                mLife();
                mBomb();
                mBombFragment();
                mSeasonGauge();
                auto power_str = std::to_string((float)(*mPower) / 100.0f).substr(0, 4);
                mPower(power_str.c_str());
                mValue();
                mValue.RoundDown(10);
                mGraze();
                mScore();
                mScore.RoundDown(10);
            }

            mNavFocus();
        }
        int CalcSection()
        {
            int chapterId = 0;
            switch (*mWarp) {
            case 1: // Chapter
                // Chapter Id = 10000 + Stage * 100 + Section
                chapterId += (*mStage + 1) * 100;
                chapterId += *mChapter;
                chapterId += 10000; // Base of chapter ID is 1000.
                return chapterId;
                break;
            case 2:
            case 3: // Mid boss & End boss
                return th_sections_cba[*mStage][*mWarp - 2][*mSection];
                break;
            case 4:
            case 5: // Non-spell & Spellcard
                return th_sections_cbt[*mStage][*mWarp - 4][*mSection];
                break;
            default:
                return 0;
                break;
            }
        }
        bool SectionHasDlg(int32_t section)
        {
            switch (section) {
            case TH16_ST1_BOSS1:
            case TH16_ST2_BOSS1:
            case TH16_ST3_BOSS1:
            case TH16_ST4_BOSS1:
            case TH16_ST5_BOSS1:
            case TH16_ST6_BOSS1:
            case TH16_ST7_END_NS1:
            case TH16_ST7_MID1:
                return true;
            default:
                return false;
            }
        }
        void SectionWidget()
        {
            static char chapterStr[256] {};
            auto& chapterCounts = mChapterSetup[*mStage];

            switch (*mWarp) {
            case 1: // Chapter
                mChapter.SetBound(1, chapterCounts[0] + chapterCounts[1]);

                if (chapterCounts[1] == 0 && chapterCounts[2] != 0) {
                    sprintf_s(chapterStr, XSTR(TH_STAGE_PORTION_N), *mChapter);
                } else if (*mChapter <= chapterCounts[0]) {
                    sprintf_s(chapterStr, XSTR(TH_STAGE_PORTION_1), *mChapter);
                } else {
                    sprintf_s(chapterStr, XSTR(TH_STAGE_PORTION_2), *mChapter - chapterCounts[0]);
                };

                mChapter(chapterStr);
                break;
            case 2:
            case 3: // Mid boss & End boss
                if (mSection(TH_WARP_SELECT[*mWarp],
                        th_sections_cba[*mStage][*mWarp - 2],
                        th_sections_str[::THPrac::Gui::LocaleGet()][mDiffculty]))
                    *mPhase = 0;
                if (SectionHasDlg(th_sections_cba[*mStage][*mWarp - 2][*mSection]))
                    mDlg();
                break;
            case 4:
            case 5: // Non-spell & Spellcard
                if (mSection(TH_WARP_SELECT[*mWarp],
                        th_sections_cbt[*mStage][*mWarp - 4],
                        th_sections_str[::THPrac::Gui::LocaleGet()][mDiffculty]))
                    *mPhase = 0;
                if (SectionHasDlg(th_sections_cbt[*mStage][*mWarp - 4][*mSection]))
                    mDlg();
                break;
            default:
                break;
            }
        }

        Gui::GuiCombo mMode { TH_MODE, TH_MODE_SELECT };
        Gui::GuiCombo mStage { TH_STAGE, TH_STAGE_SELECT };
        Gui::GuiCombo mWarp { TH_WARP, TH_WARP_SELECT };
        Gui::GuiCombo mSection { TH_MODE };
        Gui::GuiCombo mPhase { TH_PHASE };
        Gui::GuiCheckBox mDlg { TH_DLG };

        Gui::GuiSlider<int, ImGuiDataType_S32> mChapter { TH_CHAPTER, 0, 0 };
        Gui::GuiDrag<int64_t, ImGuiDataType_S64> mScore { TH_SCORE, 0, 9999999990, 10, 100000000 };
        Gui::GuiSlider<int, ImGuiDataType_S32> mLife { TH_LIFE, 0, 9 };
        Gui::GuiSlider<int, ImGuiDataType_S32> mBomb { TH_BOMB, 0, 9 };
        Gui::GuiSlider<int, ImGuiDataType_S32> mBombFragment { TH_BOMB_FRAGMENT, 0, 4 };
        Gui::GuiSlider<int, ImGuiDataType_S32> mSeasonGauge { TH16_SEASON_GAUGE, 0, 6 };
        Gui::GuiSlider<int, ImGuiDataType_S32> mPower { TH_POWER, 100, 400 };
        Gui::GuiDrag<int, ImGuiDataType_S32> mValue { TH_VALUE, 0, 999990, 10, 100000 };
        Gui::GuiDrag<int, ImGuiDataType_S32> mGraze { TH_GRAZE, 0, 999999, 1, 100000 };

        Gui::GuiNavFocus mNavFocus { TH_STAGE, TH_MODE, TH_WARP, TH_DLG,
            TH_MID_STAGE, TH_END_STAGE, TH_NONSPELL, TH_SPELL, TH_PHASE, TH_CHAPTER,
            TH_SCORE, TH_LIFE, TH_BOMB, TH_BOMB_FRAGMENT, TH16_SEASON_GAUGE,
            TH_POWER, TH_VALUE, TH_GRAZE };

        int mChapterSetup[7][2] {
            { 2, 2 },
            { 4, 0 },
            { 4, 2 },
            { 3, 3 },
            { 5, 3 },
            { 3, 0 },
            { 4, 4 },
        };


        int mDiffculty = 0;
    };
    class THOverlay : public Gui::GameGuiWnd {
        THOverlay() noexcept
        {
            SetTitle("Mod Menu");
            SetFade(0.5f, 0.5f);
            SetPos(10.0f, 10.0f);
            SetSize(0.0f, 0.0f);
            SetWndFlag(
                ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav | 0);
            OnLocaleChange();
        }
        SINGLETON(THOverlay);
    public:

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
            mInfLives.SetTextOffsetRel(x_offset_1, x_offset_2);
            mInfBombs.SetTextOffsetRel(x_offset_1, x_offset_2);
            mInfPower.SetTextOffsetRel(x_offset_1, x_offset_2);
            mTimeLock.SetTextOffsetRel(x_offset_1, x_offset_2);
            mAutoBomb.SetTextOffsetRel(x_offset_1, x_offset_2);
            mElBgm.SetTextOffsetRel(x_offset_1, x_offset_2);
        }
        virtual void OnContentUpdate() override
        {
            mMuteki();
            mInfLives();
            mInfBombs();
            mInfPower();
            mTimeLock();
            mAutoBomb();
            mElBgm();
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
        Gui::GuiHotKey mMuteki { TH_MUTEKI, "F1", VK_F1, {
            new HookCtx(0x443fe1, "\x01", 1) } };
        Gui::GuiHotKey mInfLives { TH_INFLIVES, "F2", VK_F2, {
            new HookCtx(0x443d39, "\x90", 1) } };
        Gui::GuiHotKey mInfBombs { TH_INFBOMBS, "F3", VK_F3, {
            new HookCtx(0x40db83, "\x90\x90\x90", 3) } };
        Gui::GuiHotKey mInfPower { TH_INFPOWER, "F4", VK_F4, {
            new HookCtx(0x442749, "\x90\x90\x90\x90\x90\x90", 6) } };
        Gui::GuiHotKey mTimeLock { TH_TIMELOCK, "F5", VK_F5, {
            new HookCtx(0x417965, "\xeb", 1),
            new HookCtx(0x41d4ef, "\x05\x8d", 2) } };
        Gui::GuiHotKey mAutoBomb { TH_AUTOBOMB, "F6", VK_F6, {
            new HookCtx(0x4427a1, "\xc6", 1) } };

    public:
        Gui::GuiHotKey mElBgm { TH_EL_BGM, "F7", VK_F7 };
    };
    class THGuiSP : public Gui::GameGuiWnd {
        THGuiSP() noexcept
        {
            *mSeasonGauge = 3;
            *mBugFix = true;

            SetFade(0.8f, 0.1f);
            SetStyle(ImGuiStyleVar_WindowRounding, 0.0f);
            SetStyle(ImGuiStyleVar_WindowBorderSize, 0.0f);
            OnLocaleChange();
        }
        SINGLETON(THGuiSP);
    public:

        __declspec(noinline) void State(int state)
        {
            uint32_t spell_id;
            switch (state) {
            case 0:
                break;
            case 1:
                spell_id = GetMemContent(0x4a6f20, GetMemContent(0x4a6f20, 0x5dcc) * 4 + 0x5dd0);
                if (mSpellId != spell_id) {
                    if ((mSpellId >= 106 && spell_id < 106) || (spell_id >= 106 && mSpellId < 106))
                        *mSubSeason = 0;
                    *mPhase = 0;
                    mSpellId = spell_id;
                }

                SetFade(0.8f, 0.1f);
                Open();
                thPracParam.Reset();
            case 2:
                break;
            case 3:
                SetFade(0.8f, 0.1f);
                Close();
                *mNavFocus = 0;

                // Fill Param
                thPracParam.mode = 2;
                thPracParam.season_gauge = *mSeasonGauge;
                thPracParam.phase = *mPhase;

                if (mSpellId >= 46 && mSpellId <= 69)
                    thPracParam.bug_fix = *mBugFix;
                else
                    thPracParam.bug_fix = false;

                break;
            case 4:
                Close();
                *mNavFocus = 0;
                break;
            case 5:
                if (mSpellId < 106)
                    *((int32_t*)0x4a57ac) = *mSubSeason;
                else
                    *((int32_t*)0x4a57ac) = 4;
                break;
            default:
                break;
            }
        }

    protected:
        virtual void OnLocaleChange() override
        {
            SetTitle(XSTR(TH_SPELL_PRAC));
            switch (Gui::LocaleGet()) {
            case Gui::LOCALE_ZH_CN:
                SetSizeRel(0.4f, 0.235f);
                SetPosRel(0.36f, 0.4f);
                SetItemWidthRel(-0.075f);
                SetAutoSpacing(true);
                break;
            case Gui::LOCALE_EN_US:
                SetSizeRel(0.4f, 0.22f);
                SetPosRel(0.36f, 0.4f);
                SetItemWidthRel(-0.125f);
                SetAutoSpacing(true);
                break;
            case Gui::LOCALE_JA_JP:
                SetSizeRel(0.4f, 0.235f);
                SetPosRel(0.36f, 0.4f);
                SetItemWidthRel(-0.110f);
                SetAutoSpacing(true);
                break;
            default:
                break;
            }
        }
        virtual void OnContentUpdate() override
        {
            ImGui::TextUnformatted(XSTR(TH_SPELL_PRAC));
            ImGui::Separator();

            PracticeMenu();
        }
        void PracticeMenu()
        {
            if (mSpellId < 106)
                mSubSeason(TH16_SUBSEASON, TH16_SUBSEASON_1);
            else
                mSubSeason(TH16_SUBSEASON, TH16_SUBSEASON_2);

            mSeasonGauge();

            if ((mSpellId >= 90 && mSpellId <= 105) || mSpellId == 118)
                mPhase(TH_PHASE, TH_SPELL_PHASE2);

            if (mSpellId >= 46 && mSpellId <= 69)
                mBugFix();

            mNavFocus();
        }

        unsigned int mSpellId = -1;

        Gui::GuiCombo mSubSeason { TH16_SUBSEASON };
        Gui::GuiSlider<int, ImGuiDataType_S32> mSeasonGauge { TH16_SEASON_GAUGE_ALT, 0, 6 };
        Gui::GuiCheckBox mBugFix { TH16_BUGFIX };
        Gui::GuiCombo mPhase { TH_PHASE };

        Gui::GuiNavFocus mNavFocus { TH16_SUBSEASON, TH16_SEASON_GAUGE_ALT, TH16_BUGFIX, TH_PHASE };
    };

    class THAdvOptWnd : public Gui::PPGuiWnd {
        EHOOK_ST(th16_all_clear_bonus_1, 0x42e240)
        {
            pCtx->Eip = 0x42e24d;
        }
        EHOOK_ST(th16_all_clear_bonus_2, 0x42e2c0)
        {
            *(int32_t*)(GetMemAddr(0x4a6dcc, 0x170)) = *(int32_t*)(0x4a57b0);
            if (GetMemContent(0x4a5bec) & 0x10) {
                typedef void (*PScoreFunc)();
                PScoreFunc a = (PScoreFunc)0x43f500;
                a();
                pCtx->Eip = 0x42e245;
            }
        }
        EHOOK_ST(th16_all_clear_bonus_3, 0x42e39b)
        {
            *(int32_t*)(GetMemAddr(0x4a6dcc, 0x170)) = *(int32_t*)(0x4a57b0);
            if (GetMemContent(0x4a5bec) & 0x10) {
                typedef void (*PScoreFunc)();
                PScoreFunc a = (PScoreFunc)0x43f500;
                a();
                pCtx->Eip = 0x42e245;
            }
        }
    private:
        void FpsInit()
        {
            if (*(uint8_t*)0x4c12c9 == 3) {
                mOptCtx.fps_status = 1;

                DWORD oldProtect;
                VirtualProtect((void*)0x45acc1, 4, PAGE_EXECUTE_READWRITE, &oldProtect);
                *(double**)0x45acc1 = &mOptCtx.fps_dbl;
                VirtualProtect((void*)0x45acc1, 4, oldProtect, &oldProtect);
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
            th16_all_clear_bonus_1.Setup();
            th16_all_clear_bonus_2.Setup();
            th16_all_clear_bonus_3.Setup();
        }
        void GameplaySet()
        {
            th16_all_clear_bonus_1.Toggle(mOptCtx.all_clear_bonus);
            th16_all_clear_bonus_2.Toggle(mOptCtx.all_clear_bonus);
            th16_all_clear_bonus_3.Toggle(mOptCtx.all_clear_bonus);
        }

        THAdvOptWnd() noexcept
        {
            SetWndFlag(ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove);
            SetFade(0.8f, 0.8f);
            SetStyle(ImGuiStyleVar_WindowRounding, 0.0f);
            SetStyle(ImGuiStyleVar_WindowBorderSize, 0.0f);

            InitUpdFunc([&]() { ContentUpdate(); },
                [&]() { LocaleUpdate(); },
                [&]() {},
                []() {});

            OnLocaleChange();
            FpsInit();
            GameplayInit();
        }
        SINGLETON(THAdvOptWnd);

    public:
        __declspec(noinline) static bool StaticUpdate()
        {
            auto& advOptWnd = THAdvOptWnd::singleton();

            if (Gui::KeyboardInputUpdate(VK_F12) == 1) {
                if (advOptWnd.IsOpen())
                    advOptWnd.Close();
                else
                    advOptWnd.Open();
            }
            advOptWnd.Update();

            return advOptWnd.IsOpen();
        }

    protected:
        void LocaleUpdate()
        {
            SetTitle("AdvOptMenu");
            switch (Gui::LocaleGet()) {
            case Gui::LOCALE_ZH_CN:
                SetSizeRel(1.0f, 1.0f);
                SetPosRel(0.0f, 0.0f);
                SetItemWidthRel(-0.0f);
                SetAutoSpacing(true);
                break;
            case Gui::LOCALE_EN_US:
                SetSizeRel(1.0f, 1.0f);
                SetPosRel(0.0f, 0.0f);
                SetItemWidthRel(-0.0f);
                SetAutoSpacing(true);
                break;
            case Gui::LOCALE_JA_JP:
                SetSizeRel(1.0f, 1.0f);
                SetPosRel(0.0f, 0.0f);
                SetItemWidthRel(-0.0f);
                SetAutoSpacing(true);
                break;
            default:
                break;
            }
        }
        void ContentUpdate()
        {
            ImGui::TextUnformatted(XSTR(TH_ADV_OPT));
            ImGui::Separator();
            ImGui::BeginChild("Adv. Options", ImVec2(0.0f, 0.0f));

            if (BeginOptGroup<TH_GAME_SPEED>()) {
                if (GameFPSOpt(mOptCtx))
                    FpsSet();
                EndOptGroup();
            }
            if (BeginOptGroup<TH_GAMEPLAY>()) {
                if (GameplayOpt(mOptCtx))
                    GameplaySet();
                EndOptGroup();
            }

            AboutOpt();
            ImGui::EndChild();
            ImGui::SetWindowFocus();
        }

        adv_opt_ctx mOptCtx;
    };

    void ECLJump(ECLHelper& ecl, unsigned int start, unsigned int dest, int at_frame, int ecl_time = 0)
    {
        ecl.SetPos(start);
        ecl << ecl_time << 0x0018000C << 0x02ff0000 << 0x00000000 << dest - start << at_frame;
    }
    void ECLST6Background(ECLHelper& ecl, int ordinal)
    {
        ecl.SetFile(2);

        ECLJump(ecl, 0x4b4, 0x768, 1);

        ecl.SetPos(0x768);
        switch (ordinal) {
        case 2:
        case 3:
            ecl << 0 << 0x00140154 << 0x01ff0001 << 0 << -9924;

            ecl << 0 << 0x0014002a << 0x01ff0000 << 0 << 0;
            ecl << 0 << 0x0014002b << 0x01ff0001 << 0 << -9924;

            ecl << 0 << 0x00140154 << 0x01ff0001 << 0 << -9923;

            ecl << 0 << 0x00340141 << 0x06ff0000 << 0
                << 12 << "MapleEnemy3" << '\0'
                << 0 << 0 << 100 << 1000 << 0;

            ecl << 0 << 0x0014002a << 0x01ff0000 << 0 << 0;
            ecl << 0 << 0x0014002b << 0x01ff0001 << 0 << -9923;

            ecl << 0 << 0x00140276 << 0x01ff0000 << 0 << ordinal - 1;
            break;
        case 4:
        case 5:
            ecl << 0 << 0x00140154 << 0x01ff0001 << 0 << -9924;

            ecl << 0 << 0x0014002a << 0x01ff0000 << 0 << 0;
            ecl << 0 << 0x0014002b << 0x01ff0001 << 0 << -9924;

            ecl << 0 << 0x00140154 << 0x01ff0001 << 0 << -9923;

            ecl << 0 << 0x00340141 << 0x06ff0000 << 0
                << 12 << "MapleEnemy3" << '\0'
                << 0 << 0 << 100 << 1000 << 0;

            ecl << 0 << 0x0014002a << 0x01ff0000 << 0 << 0;
            ecl << 0 << 0x0014002b << 0x01ff0001 << 0 << -9923;

            ecl << 0 << 0x00140276 << 0x01ff0000 << 0 << ((ordinal == 4) ? 4 : 3);

            ecl << 0 << 0x00340141 << 0x06ff0000 << 0
                << 12 << "MapleEnemy" << ((ordinal == 4) ? '5' : '4') << '\0'
                << 0 << 0 << 100 << 1000 << 0;

            ecl << 0 << 0x0014002a << 0x01ff0001 << 0 << -9931;
            ecl << 0 << 0x0014002b << 0x01ff0001 << 0 << -9923;
            break;
        case 1:
        case 6:
            ecl << 0 << 0x00140154 << 0x01ff0001 << 0 << -9924;

            ecl << 0 << 0x0014002a << 0x01ff0000 << 0 << 0;
            ecl << 0 << 0x0014002b << 0x01ff0001 << 0 << -9924;

            ecl << 0 << 0x00140154 << 0x01ff0001 << 0 << -9923;

            ecl << 0 << 0x00340141 << 0x06ff0000 << 0
                << 12 << "MapleEnemy6" << '\0'
                << 0 << 0 << 100 << 1000 << 0;
            ecl << 0 << 0x00340141 << 0x06ff0000 << 0
                << 12 << "MapleEnemy7" << '\0'
                << 0 << 0 << 100 << 1000 << 0;

            ecl << 0 << 0x0014002a << 0x01ff0001 << 0 << -9931;
            ecl << 0 << 0x0014002b << 0x01ff0001 << 0 << -9923;

            ecl << 0 << 0x00140276 << 0x01ff0000 << 0 << 5;
            break;
        default:
            break;
        }

        ecl << 0 << 0x00140017 << 0x01ff0000 << 0 << 101
            << 0 << 0x0014012e << 0x01ff0000 << 0 << 3;
        ECLJump(ecl, ecl.GetPos(), 0x4dc, 0);
    }
    __declspec(noinline) void THStageWarp(ECLHelper& ecl, int stage, int portion)
    {
        if (stage == 1) {
            switch (portion) {
            case 1:
                break;
            case 2:
                ECLJump(ecl, 0x7c10, 0x7df8, 60, 90); // 0x7f14
                ECLJump(ecl, 0x5070, 0x50a4, 0, 0);
                break;
            case 3:
                ECLJump(ecl, 0x7c10, 0x7e94, 60, 90);
                ecl << pair{0x60d4, 0};
                break;
            case 4:
                ECLJump(ecl, 0x7c10, 0x7e94, 60, 90);
                ECLJump(ecl, 0x5160, 0x51d0, 0, 0);
                break;
            default:
                break;
            }
        } else if (stage == 2) {
            switch (portion) {
            case 1:
                break;
            case 2:
                ECLJump(ecl, 0x77c8, 0x79b0, 60, 90);
                ECLJump(ecl, 0x4070, 0x40f8, 0, 0);
                break;
            case 3:
                ECLJump(ecl, 0x77c8, 0x79b0, 60, 90);
                ECLJump(ecl, 0x4070, 0x412c, 0, 0);
                break;
            case 4:
                ECLJump(ecl, 0x77c8, 0x79b0, 60, 90);
                ECLJump(ecl, 0x4070, 0x4194, 0, 0);
                break;
            default:
                break;
            }
        } else if (stage == 3) {
            switch (portion) {
            case 1:
                break;
            case 2:
                ECLJump(ecl, 0x8648, 0x88bc, 60, 90); // 0x8960
                ECLJump(ecl, 0x5248, 0x527c, 0, 0);
                break;
            case 3:
                ECLJump(ecl, 0x8648, 0x88bc, 60, 90); // 0x8960
                ECLJump(ecl, 0x5248, 0x52b0, 0, 0);
                break;
            case 4:
                ECLJump(ecl, 0x8648, 0x88bc, 60, 90); // 0x8960
                ECLJump(ecl, 0x5248, 0x5318, 0, 0);
                break;
            case 5:
                ECLJump(ecl, 0x8648, 0x8960, 60, 90);
                break;
            case 6:
                ECLJump(ecl, 0x8648, 0x8960, 60, 90);
                ECLJump(ecl, 0x536c, 0x539c, 0, 0);
                break;
            default:
                break;
            }
        } else if (stage == 4) {
            switch (portion) {
            case 1:
                break;
            case 2:
                ECLJump(ecl, 0x9bb0, 0x9e24, 60, 90); // 0x9ec0
                ECLJump(ecl, 0x6244, 0x6278, 0, 0);
                break;
            case 3:
                ECLJump(ecl, 0x9bb0, 0x9e24, 60, 90); // 0x9ec0
                ECLJump(ecl, 0x6244, 0x62ac, 0, 0);
                break;
            case 4:
                ECLJump(ecl, 0x9bb0, 0x9ec0, 60, 90);
                break;
            case 5:
                ECLJump(ecl, 0x9bb0, 0x9ec0, 60, 90);
                ECLJump(ecl, 0x6314, 0x6348, 0, 0);
                break;
            case 6:
                ECLJump(ecl, 0x9bb0, 0x9ec0, 60, 90);
                ECLJump(ecl, 0x6314, 0x63a8, 0, 0);
                break;
            default:
                break;
            }
        } else if (stage == 5) {
            switch (portion) {
            case 1:
                break;
            case 2:
                ECLJump(ecl, 0x9070, 0x925c, 60, 90); // 0x92f8
                ECLJump(ecl, 0x4a94, 0x4ac8, 0, 0);
                break;
            case 3:
                ECLJump(ecl, 0x9070, 0x925c, 60, 90); // 0x92f8
                ECLJump(ecl, 0x4a94, 0x4afc, 0, 0);
                break;
            case 4:
                ECLJump(ecl, 0x9070, 0x925c, 60, 90); // 0x92f8
                ECLJump(ecl, 0x4a94, 0x4b30, 0, 0);
                break;
            case 5:
                ECLJump(ecl, 0x9070, 0x925c, 60, 90); // 0x92f8
                ECLJump(ecl, 0x4a94, 0x4b64, 0, 0);
                break;
            case 6:
                ECLJump(ecl, 0x9070, 0x92f8, 60, 90);
                ecl << pair{0x7418, 0};
                break;
            case 7:
                ECLJump(ecl, 0x9070, 0x92f8, 60, 90);
                ECLJump(ecl, 0x4bcc, 0x4c00, 0, 0);
                break;
            case 8:
                ECLJump(ecl, 0x9070, 0x92f8, 60, 90);
                ECLJump(ecl, 0x4bcc, 0x4c60, 0, 0);
                break;
            default:
                break;
            }
        } else if (stage == 6) {
            switch (portion) {
            case 1:
                break;
            case 2:
                ECLJump(ecl, 0x5698, 0x58ac, 60, 90);
                ECLJump(ecl, 0x34c0, 0x34f4, 0, 0);
                ecl << pair{0x3aac, 0};
                break;
            case 3:
                ECLJump(ecl, 0x5698, 0x58ac, 60, 90);
                ECLJump(ecl, 0x34c0, 0x3538, 0, 0);
                break;
            default:
                break;
            }
        } else if (stage == 7) {
            switch (portion) {
            case 1:
                break;
            case 2:
                ECLJump(ecl, 0x988c, 0x9b18, 60, 90); // 0x9c1c
                ECLJump(ecl, 0x614c, 0x6180, 0, 0);
                break;
            case 3:
                ECLJump(ecl, 0x988c, 0x9b18, 60, 90); // 0x9c1c
                ECLJump(ecl, 0x614c, 0x61b4, 0, 0);
                break;
            case 4:
                ECLJump(ecl, 0x988c, 0x9b18, 60, 90); // 0x9c1c
                ECLJump(ecl, 0x614c, 0x61e8, 0, 0);
                break;
            case 5:
                ECLJump(ecl, 0x988c, 0x9c1c, 60, 90);
                break;
            case 6:
                ECLJump(ecl, 0x988c, 0x9c1c, 60, 10);
                ECLJump(ecl, 0x623c, 0x6284, 0, 0);
                break;
            case 7:
                ECLJump(ecl, 0x988c, 0x9c1c, 60, 90);
                ECLJump(ecl, 0x623c, 0x62e0, 0, 0);
                break;
            case 8:
                ECLJump(ecl, 0x988c, 0x9c1c, 60, 90);
                ECLJump(ecl, 0x623c, 0x633c, 0, 0);
                break;
            default:
                break;
            }
        }
    }
    __declspec(noinline) void THPatch(ECLHelper& ecl, th_sections_t section)
    {
        switch (section) {
        case THPrac::TH16::TH16_ST1_MID1:
            ECLJump(ecl, 0x7be8, 0x7e50, 60);
            break;
        case THPrac::TH16::TH16_ST1_BOSS1:
            if (thPracParam.dlg)
                ECLJump(ecl, 0x7be8, 0x7f00, 60);
            else
                ECLJump(ecl, 0x7be8, 0x7f14, 60);
            break;
        case THPrac::TH16::TH16_ST1_BOSS2:
            ECLJump(ecl, 0x7be8, 0x7f14, 60);
            ecl.SetFile(2);
            ECLJump(ecl, 0x3cc, 0x4b4, 1); // Utilize Spell Practice Jump
            ecl << pair{0x4c4, 1700}; // Set Health
            ecl << pair{0x4e4, (int8_t)0x31}; // Set Spell Ordinal
            ecl << pair{0x1ba8, (int16_t)0}; // Disable Item Drops
            break;
        case THPrac::TH16::TH16_ST1_BOSS3:
            ECLJump(ecl, 0x7be8, 0x7f14, 60);
            ecl.SetFile(2);
            ecl << pair{0x604, (int8_t)0x32}; // Change Nonspell
            ecl << pair{0x1044, (int16_t)0} << pair{0x1174, (int16_t)0}; // Disable Item Drops & SE
            break;
        case THPrac::TH16::TH16_ST1_BOSS4:
            ECLJump(ecl, 0x7be8, 0x7f14, 60);
            ecl.SetFile(2);
            ECLJump(ecl, 0x3cc, 0x4b4, 1); // Utilize Spell Practice Jump
            ecl << pair{0x4c4, 1700}; // Set Health
            ecl << pair{0x4e4, (int8_t)0x32}; // Set Spell Ordinal
            ecl << pair{0x2778, (int16_t)0}; // Disable Item Drops
            break;
        case THPrac::TH16::TH16_ST2_MID1:
            ECLJump(ecl, 0x77c8, 0x7a34, 60);
            break;
        case THPrac::TH16::TH16_ST2_BOSS1:
            if (thPracParam.dlg)
                ECLJump(ecl, 0x77c8, 0x7a74, 60);
            else {
                ECLJump(ecl, 0x77c8, 0x7a88, 60);
                ecl.SetFile(2);
                ECLJump(ecl, 0x144, 0x4a88, 0);
                ECLJump(ecl, 0x4d50, 0x360, 1);
            }
            break;
        case THPrac::TH16::TH16_ST2_BOSS2:
            ECLJump(ecl, 0x77c8, 0x7a88, 60);
            ecl.SetFile(2);
            ECLJump(ecl, 0x144, 0x4a88, 0);
            ECLJump(ecl, 0x4d50, 0x360, 1);

            ECLJump(ecl, 0x360, 0x448, 1); // Utilize Spell Practice Jump
            ecl << pair{0x458, 2100}; // Set Health
            ecl << pair{0x478, (int8_t)0x31}; // Set Spell Ordinal
            ecl << pair{0x1bc4, (int16_t)0}; // Disable Item Drops
            break;
        case THPrac::TH16::TH16_ST2_BOSS3:
            ECLJump(ecl, 0x77c8, 0x7a88, 60);
            ecl.SetFile(2);
            ECLJump(ecl, 0x144, 0x4a88, 0);
            ECLJump(ecl, 0x4d50, 0x360, 1);

            ecl << pair{0x67c, (int8_t)0x32}; // Change Nonspell
            ecl << pair{0xe28, (int16_t)0} << pair{0xf74, (int16_t)0}; // Disable Item Drops & SE
            ecl << pair{0x10ec, 59} << pair{0x1138, 0} << pair{0xd5c, 60}; // Change Move Time, Wait Time & Inv. Time
            break;
        case THPrac::TH16::TH16_ST2_BOSS4:
            ECLJump(ecl, 0x77c8, 0x7a88, 60);
            ecl.SetFile(2);
            ECLJump(ecl, 0x144, 0x4a88, 0);
            ECLJump(ecl, 0x4d50, 0x360, 1);

            ECLJump(ecl, 0x360, 0x448, 1); // Utilize Spell Practice Jump
            ecl << pair{0x458, 2200}; // Set Health
            ecl << pair{0x478, (int8_t)0x32}; // Set Spell Ordinal
            ecl << pair{0x2b70, (int16_t)0}; // Disable Item Drops
            break;
        case THPrac::TH16::TH16_ST2_BOSS5:
            ECLJump(ecl, 0x77c8, 0x7a88, 60);
            ecl.SetFile(2);
            ECLJump(ecl, 0x144, 0x4a88, 0);
            ECLJump(ecl, 0x4d50, 0x360, 1);

            ECLJump(ecl, 0x360, 0x448, 1); // Utilize Spell Practice Jump
            ecl << pair{0x458, 3000}; // Set Health
            ecl << pair{0x478, (int8_t)0x33}; // Set Spell Ordinal
            ecl << pair{0x3b50, (int16_t)0}; // Disable Item Drops
            break;
        case THPrac::TH16::TH16_ST3_MID1:
            ECLJump(ecl, 0x8648, 0x891c, 60);
            break;
        case THPrac::TH16::TH16_ST3_MID2:
            ECLJump(ecl, 0x8648, 0x891c, 60);
            ecl.SetFile(3);
            ECLJump(ecl, 0x268, 0x2b8, 0);
            ECLJump(ecl, 0x4b8, 0x508, 0);
            ECLJump(ecl, 0x51c, 0x5b4, 0);
            ecl << pair{0xdfc, (int16_t)0}; // Disable Item Drops
            break;
        case THPrac::TH16::TH16_ST3_BOSS1:
            if (thPracParam.dlg)
                ECLJump(ecl, 0x8648, 0x89cc, 60);
            else
                ECLJump(ecl, 0x8648, 0x89e0, 60);
            break;
        case THPrac::TH16::TH16_ST3_BOSS2:
            ECLJump(ecl, 0x8648, 0x89e0, 60);
            ecl.SetFile(2);
            ECLJump(ecl, 0x448, 0x530, 1); // Utilize Spell Practice Jump
            ecl << pair{0x540, 2000}; // Set Health
            ecl << pair{0x560, (int8_t)0x31}; // Set Spell Ordinal
            ecl << pair{0x2638, (int16_t)0}; // Disable Item Drops
            break;
        case THPrac::TH16::TH16_ST3_BOSS3:
            ECLJump(ecl, 0x8648, 0x89e0, 60);
            ecl.SetFile(2);
            ecl << pair{0x764, (int8_t)0x32}; // Change Nonspell
            ecl << pair{0x10a8, (int16_t)0} << pair{0x11d8, (int16_t)0}; // Disable Item Drops & SE
            ecl << pair{0x1398, 0} << pair{0xfdc, 60}; // Change Wait Time & Inv. Time
            break;
        case THPrac::TH16::TH16_ST3_BOSS4:
            ECLJump(ecl, 0x8648, 0x89e0, 60);
            ecl.SetFile(2);
            ECLJump(ecl, 0x448, 0x530, 1); // Utilize Spell Practice Jump
            ecl << pair{0x540, 2100}; // Set Health
            ecl << pair{0x560, (int8_t)0x32}; // Set Spell Ordinal
            ecl << pair{0x3138, (int16_t)0}; // Disable Item Drops
            break;
        case THPrac::TH16::TH16_ST3_BOSS5:
            ECLJump(ecl, 0x8648, 0x89e0, 60);
            ecl.SetFile(2);
            ecl << pair{0x764, (int8_t)0x33}; // Change Nonspell
            ecl << pair{0x1c14, (int16_t)0} << pair{0x1d44, (int16_t)0}; // Disable Item Drops & SE
            ecl << pair{0x1f18, 0} << pair{0x1ebc, 60}; // Change Wait Time & Inv. Time
            break;
        case THPrac::TH16::TH16_ST3_BOSS6:
            ECLJump(ecl, 0x8648, 0x89e0, 60);
            ecl.SetFile(2);
            ECLJump(ecl, 0x448, 0x530, 1); // Utilize Spell Practice Jump
            ecl << pair{0x540, 1700}; // Set Health
            ecl << pair{0x560, (int8_t)0x33}; // Set Spell Ordinal
            ecl << pair{0x44a4, (int16_t)0}; // Disable Item Drops
            break;
        case THPrac::TH16::TH16_ST4_MID1:
            ECLJump(ecl, 0x9bb0, 0x9e7c, 60);
            break;
        case THPrac::TH16::TH16_ST4_MID2:
            ECLJump(ecl, 0x9bb0, 0x9ec0, 60);
            ECLJump(ecl, 0x6314, 0x6368, 0);
            ecl << pair{0x6378, 60};
            break;
        case THPrac::TH16::TH16_ST4_BOSS1:
            if (thPracParam.dlg)
                ECLJump(ecl, 0x9bb0, 0x9f70, 60);
            else
                ECLJump(ecl, 0x9bb0, 0x9f84, 60);
            break;
        case THPrac::TH16::TH16_ST4_BOSS2:
            ECLJump(ecl, 0x9bb0, 0x9f84, 60);
            ecl.SetFile(2);
            ECLJump(ecl, 0x454, 0x53c, 1); // Utilize Spell Practice Jump
            ecl << pair{0x54c, 2300}; // Set Health
            ecl << pair{0x56c, (int8_t)0x31}; // Set Spell Ordinal
            ecl << pair{0x252c, (int16_t)0}; // Disable Item Drops
            break;
        case THPrac::TH16::TH16_ST4_BOSS3:
            ECLJump(ecl, 0x9bb0, 0x9f84, 60);
            ecl.SetFile(2);
            ecl << pair{0x770, (int8_t)0x32}; // Change Nonspell
            ecl << pair{0x1014, (int16_t)0} << pair{0x1144, (int16_t)0}; // Disable Item Drops & SE
            ecl << pair{0x1304, 0} << pair{0xf48, 60}; // Change Wait Time & Inv. Time
            break;
        case THPrac::TH16::TH16_ST4_BOSS4:
            ECLJump(ecl, 0x9bb0, 0x9f84, 60);
            ecl.SetFile(2);
            ECLJump(ecl, 0x454, 0x53c, 1); // Utilize Spell Practice Jump
            ecl << pair{0x54c, 2200}; // Set Health
            ecl << pair{0x56c, (int8_t)0x32}; // Set Spell Ordinal
            ecl << pair{0x314c, (int16_t)0}; // Disable Item Drops
            break;
        case THPrac::TH16::TH16_ST4_BOSS5:
            ECLJump(ecl, 0x9bb0, 0x9f84, 60);
            ecl.SetFile(2);
            ecl << pair{0x770, (int8_t)0x33}; // Change Nonspell
            ecl << pair{0x19a8, (int16_t)0} << pair{0x1ad8, (int16_t)0}; // Disable Item Drops & SE
            ecl << pair{0x1cac, 0} << pair{0x1c50, 60}; // Change Wait Time & Inv. Time
            break;
        case THPrac::TH16::TH16_ST4_BOSS6:
            ECLJump(ecl, 0x9bb0, 0x9f84, 60);
            ecl.SetFile(2);
            ECLJump(ecl, 0x454, 0x53c, 1); // Utilize Spell Practice Jump
            ecl << pair{0x54c, 2300}; // Set Health
            ecl << pair{0x56c, (int8_t)0x33}; // Set Spell Ordinal
            ecl << pair{0x4698, (int16_t)0}; // Disable Item Drops
            break;
        case THPrac::TH16::TH16_ST5_MID1:
            ECLJump(ecl, 0x9070, 0x92b4, 60);
            break;
        case THPrac::TH16::TH16_ST5_BOSS1:
            if (thPracParam.dlg)
                ECLJump(ecl, 0x9070, 0x9364, 60);
            else {
                ECLJump(ecl, 0x9070, 0x9378, 60);
                ecl << pair{0x93ac, (int16_t)0};
                ecl.SetFile(3);
                ecl << pair{0x4bc, 64.0f}; // BossA Pos
                ecl << pair{0x648, (int16_t)0}; // BossA Move
            }
            break;
        case THPrac::TH16::TH16_ST5_BOSS2A:
            ECLJump(ecl, 0x9070, 0x9378, 60);
            ecl << pair{0x93ac, (int16_t)0};
            ecl.SetFile(3);
            ecl << pair{0x4bc, 64.0f}; // BossA Pos
            ecl << pair{0x648, (int16_t)0}; // BossA Move

            ECLJump(ecl, 0x700, 0x7e8, 2); // Utilize Spell Practice Jump
            ecl << pair{0x7f8, 2000}; // Set Health
            ecl << pair{0x818, (int8_t)0x31}; // Set Spell Ordinal
            ECLJump(ecl, 0x6d38, 0x70f8, 1); // Utilize Wait Loop
            ecl << pair{0x7230, (int16_t)0} << pair{0x8d18, (int16_t)0}; // Disable Item Drops
            break;
        case THPrac::TH16::TH16_ST5_BOSS2B:
            ECLJump(ecl, 0x9070, 0x9378, 60);
            ecl << pair{0x93ac, (int16_t)0};
            ecl.SetFile(3);
            ecl << pair{0x4bc, 64.0f}; // BossA Pos
            ecl << pair{0x648, (int16_t)0}; // BossA Move

            ECLJump(ecl, 0x6d38, 0x6e20, 1); // Utilize Spell Practice Jump
            ecl << pair{0x6e30, 2000}; // Set Health
            ecl << pair{0x6e50, (int8_t)0x31}; // Set Spell Ordinal
            ECLJump(ecl, 0x700, 0xae0, 2); // Utilize Wait Loop
            ecl << pair{0x7b04, (int16_t)0} << pair{0x8d18, (int16_t)0}; // Disable Item Drops
            break;
        case THPrac::TH16::TH16_ST5_BOSS3:
            ECLJump(ecl, 0x9070, 0x9378, 60);
            ecl << pair{0x93ac, (int16_t)0};
            ecl.SetFile(3);
            ecl << pair{0x4bc, 64.0f}; // BossA Pos
            ecl << pair{0x648, (int16_t)0}; // BossA Move

            ecl << pair{0xb58, (int8_t)0x32}; // Change Nonspell
            ECLJump(ecl, 0x6d38, 0x70f8, 1); // Utilize Wait Loop

            // Boss A
            ecl << pair(0x4bc, -64.0f) << pair{0x4c0, 128.0f}; // Spawn Pos
            ecl << pair{0x22f0, (int16_t)0} << pair{0x3dfc, (int16_t)0}; // Disable Item Drops & SE
            ecl << pair{0x3fc4, (int16_t)0} << pair{0x4000, 0} << pair{0x3cd8, 0}; // Void Move, Change Wait Time & Inv. Time

            // Boss B
            ecl << pair{0x6b24, 64.0f} << pair{0x6b28, 128.0f}; // Spawn Pos
            ecl << pair{0x2eb0, (int16_t)0}; // Disable SE
            ecl << pair{0x3078, (int16_t)0} << pair{0x30b4, 0} << pair{0x2d8c, 0}; // Void Move, Change Wait Time & Inv. Time
            break;
        case THPrac::TH16::TH16_ST5_BOSS4A:
            ECLJump(ecl, 0x9070, 0x9378, 60);
            ecl << pair{0x93ac, (int16_t)0};
            ecl.SetFile(3);
            ecl << pair{0x4bc, 64.0f}; // BossA Pos
            ecl << pair{0x648, (int16_t)0}; // BossA Move

            ECLJump(ecl, 0x700, 0x7e8, 2); // Utilize Spell Practice Jump
            ecl << pair{0x7f8, 2000}; // Set Health
            ecl << pair{0x818, (int8_t)0x32}; // Set Spell Ordinal
            ECLJump(ecl, 0x6d38, 0x70f8, 1); // Utilize Wait Loop
            ecl << pair{0x9a94, (int16_t)0} << pair{0x8d18, (int16_t)0}; // Disable Item Drops
            break;
        case THPrac::TH16::TH16_ST5_BOSS4B:
            ECLJump(ecl, 0x9070, 0x9378, 60);
            ecl << pair{0x93ac, (int16_t)0};
            ecl.SetFile(3);
            ecl << pair{0x4bc, 64.0f}; // BossA Pos
            ecl << pair{0x648, (int16_t)0}; // BossA Move

            ECLJump(ecl, 0x6d38, 0x6e20, 1); // Utilize Spell Practice Jump
            ecl << pair{0x6e30, 2000}; // Set Health
            ecl << pair{0x6e50, (int8_t)0x32}; // Set Spell Ordinal
            ECLJump(ecl, 0x700, 0xae0, 2); // Utilize Wait Loop
            ecl << pair{0xa0b4, (int16_t)0} << pair{0x8d18, (int16_t)0}; // Disable Item Drops
            break;
        case THPrac::TH16::TH16_ST5_BOSS5:
            ECLJump(ecl, 0x9070, 0x9378, 60);
            ecl << pair{0x93ac, (int16_t)0};
            ecl.SetFile(3);
            ecl << pair{0x4bc, 64.0f}; // BossA Pos
            ecl << pair{0x648, (int16_t)0}; // BossA Move

            ecl << pair{0xb58, (int8_t)0x33}; // Change Nonspell
            ECLJump(ecl, 0x6d38, 0x70f8, 1); // Utilize Wait Loop

            // Boss A
            ecl << pair{0x668, (int16_t)0}; // Disable 504
            ecl << pair{0x4bc, 0.0f} << pair{0x4c0, 144.0f}; // Spawn Pos
            ecl << pair{0x42d0, (int16_t)0} << pair{0x5d9c, (int16_t)0}; // Disable Item Drops & SE
            ecl << pair{0x5f64, (int16_t)0} << pair{0x5fa0, 0} << pair{0x5c78, 0}; // Void Move, Change Wait Time & Inv. Time

            // Boss B
            ecl << pair{0x6cb0, (int16_t)0}; // Disable 504
            ecl << pair{0x6b24, 0.0f} << pair{0x6b28, 64.0f}; // Spawn Pos
            ecl << pair{0x4e40, (int16_t)0}; // Disable SE
            ecl << pair{0x5008, (int16_t)0} << pair{0x5044, 0} << pair{0x4d1c, 0}; // Void Move, Change Wait Time & Inv. Time
            break;
        case THPrac::TH16::TH16_ST5_BOSS6:
            ECLJump(ecl, 0x9070, 0x9378, 60);
            ecl << pair{0x93ac, (int16_t)0};
            ecl.SetFile(3);
            ecl << pair{0x4bc, 64.0f}; // BossA Pos
            ecl << pair{0x648, (int16_t)0}; // BossA Move

            ECLJump(ecl, 0x700, 0x7e8, 2); // Utilize Spell Practice Jump
            ecl << pair{0x7f8, 2300}; // Set Health
            ecl << pair{0x818, (int8_t)0x33}; // Set Spell Ordinal
            ecl << pair{0xbb38, (int16_t)0}; // Disable Item Drops

            ECLJump(ecl, 0x6d38, 0x6e20, 1); // Utilize Spell Practice Jump
            ecl << pair{0x6e30, 2300}; // Set Health
            ECLJump(ecl, 0x6e34, 0x70f8, 1); // Utilize Wait Loop
            ecl << pair{0xbd00, (int16_t)0}; // Disable Item Drops
            break;
        case THPrac::TH16::TH16_ST5_BOSS7:
            ECLJump(ecl, 0x9070, 0x9378, 60);
            ecl << pair{0x93ac, (int16_t)0};
            ecl.SetFile(3);
            ecl << pair{0x4bc, 64.0f}; // BossA Pos
            ecl << pair{0x648, (int16_t)0}; // BossA Move

            ECLJump(ecl, 0x700, 0x7e8, 2); // Utilize Spell Practice Jump
            ecl << pair{0x7f8, 3000}; // Set Health
            ecl << pair{0x818, (int8_t)0x34}; // Set Spell Ordinal
            ecl << pair{0x6230, (int16_t)0}; // Disable Item Drops

            ECLJump(ecl, 0x6d38, 0x6e20, 1); // Utilize Spell Practice Jump
            ecl << pair{0x6e30, 3000}; // Set Health
            ecl << pair{0x6e50, (int8_t)0x34}; // Set Spell Ordinal
            ecl << pair{0x636c, (int16_t)0}; // Disable Item Drops
            break;
        case THPrac::TH16::TH16_ST6_BOSS1:
            if (thPracParam.dlg)
                ECLJump(ecl, 0x56c0, 0x592c, 60);
            else
                ECLJump(ecl, 0x56c0, 0x5940, 60);
            ecl.SetFile(2);
            ECLST6Background(ecl, 1);
            break;
        case THPrac::TH16::TH16_ST6_BOSS2:
            ECLJump(ecl, 0x56c0, 0x5940, 60);
            ecl.SetFile(2);
            ECLST6Background(ecl, 1);
            ECLJump(ecl, 0x680, 0xe88, 1); // Utilize Spell Practice Jump
            ecl << pair{0xe98, 2400}; // Set Health
            ecl << pair{0xeb8, (int8_t)0x31}; // Set Spell Ordinal
            ecl << pair{0x4bd8, (int16_t)0}; // Disable Item Drops
            break;
        case THPrac::TH16::TH16_ST6_BOSS3:
            ECLJump(ecl, 0x56c0, 0x5940, 60);
            ecl.SetFile(2);
            ECLST6Background(ecl, 2);
            ecl << pair{0xef4, 0x32}; // Nonspell Ordinal
            ecl << pair{0x17b8, 0}; // Invincible time
            ECLJump(ecl, 0x1860, 0x1920, 0); // Skip Background change
            ecl << pair{0x1964, (int16_t)0} << pair{0x1a94, (int16_t)0}; // Void Item Drop & SE
            ECLJump(ecl, 0x1c08, 0x1c58, 0); // Skip Wait & SE
            break;
        case THPrac::TH16::TH16_ST6_BOSS4:
            ECLJump(ecl, 0x56c0, 0x5940, 60);
            ecl.SetFile(2);
            ECLST6Background(ecl, 2);
            ECLJump(ecl, 0x680, 0xe88, 1); // Utilize Spell Practice Jump
            ecl << pair{0xe98, 2400}; // Set Health
            ecl << pair{0xeb8, (int8_t)0x32}; // Set Spell Ordinal
            ecl << pair{0x59b0, (int16_t)0}; // Disable Item Drops
            break;
        case THPrac::TH16::TH16_ST6_BOSS5:
            ECLJump(ecl, 0x56c0, 0x5940, 60);
            ecl.SetFile(2);
            ECLST6Background(ecl, 3);
            ecl << pair{0xef4, 0x33}; // Nonspell Ordinal
            ecl << pair{0x2140, 0}; // Invincible time
            ECLJump(ecl, 0x21e8, 0x22c0, 0); // Skip Background change
            ecl << pair{0x22ec, (int16_t)0} << pair{0x241c, (int16_t)0}; // Void Item Drop & SE
            ECLJump(ecl, 0x2590, 0x25e0, 0); // Skip Wait & SE
            break;
        case THPrac::TH16::TH16_ST6_BOSS6:
            ECLJump(ecl, 0x56c0, 0x5940, 60);
            ecl.SetFile(2);
            ECLST6Background(ecl, 3);
            ECLJump(ecl, 0x680, 0xe88, 1); // Utilize Spell Practice Jump
            ecl << pair{0xe98, 2500}; // Set Health
            ecl << pair{0xeb8, (int8_t)0x33}; // Set Spell Ordinal
            ecl << pair{0x6d1c, (int16_t)0}; // Disable Item Drops
            break;
        case THPrac::TH16::TH16_ST6_BOSS7:
            ECLJump(ecl, 0x56c0, 0x5940, 60);
            ecl.SetFile(2);
            ECLST6Background(ecl, 4);
            ecl << pair{0xef4, 0x34}; // Nonspell Ordinal
            ecl << pair{0x2b4c, 0}; // Invincible time
            ECLJump(ecl, 0x2bf4, 0x2d10, 0); // Skip Background change
            ecl << pair{0x2d54, (int16_t)0} << pair{0x2e84, (int16_t)0}; // Void Item Drop & SE
            ECLJump(ecl, 0x2ff8, 0x3048, 0); // Skip Wait & SE
            break;
        case THPrac::TH16::TH16_ST6_BOSS8:
            ECLJump(ecl, 0x56c0, 0x5940, 60);
            ecl.SetFile(2);
            ECLST6Background(ecl, 4);
            ECLJump(ecl, 0x680, 0xe88, 1); // Utilize Spell Practice Jump
            ecl << pair{0xe98, 3000}; // Set Health
            ecl << pair{0xeb8, (int8_t)0x34}; // Set Spell Ordinal
            ecl << pair{0x7dbc, (int16_t)0}; // Disable Item Drops
            break;
        case THPrac::TH16::TH16_ST6_BOSS9:
            ECLJump(ecl, 0x56c0, 0x5940, 60);
            ecl.SetFile(2);
            ECLST6Background(ecl, 5);
            ecl << pair{0xef4, 0x35}; // Nonspell Ordinal
            ecl << pair{0x3608, 0}; // Invincible time
            ECLJump(ecl, 0x36b0, 0x37cc, 0); // Skip Background change
            ecl << pair{0x3810, (int16_t)0} << pair{0x3940, (int16_t)0}; // Void Item Drop & SE
            ECLJump(ecl, 0x3ab4, 0x3b04, 0); // Skip Wait & SE
            break;
        case THPrac::TH16::TH16_ST6_BOSS10:
            ECLJump(ecl, 0x56c0, 0x5940, 60);
            ecl.SetFile(2);
            ECLST6Background(ecl, 5);
            ECLJump(ecl, 0x680, 0xe88, 1); // Utilize Spell Practice Jump
            ecl << pair{0xe98, 4200}; // Set Health
            ecl << pair{0xeb8, (int8_t)0x35}; // Set Spell Ordinal
            ecl << pair{0x8a68, (int16_t)0}; // Disable Item Drops
            break;
        case THPrac::TH16::TH16_ST6_SPRING_FINAL:
            ECLJump(ecl, 0x56c0, 0x5940, 60);
            ecl.SetFile(2);
            ECLST6Background(ecl, 6);
            ECLJump(ecl, 0x680, 0xe88, 1); // Utilize Spell Practice Jump
            ecl << pair{0xe98, 5000}; // Set Health
            ecl << pair{0xeb8, (int8_t)0x36}; // Set Spell Ordinal
            switch (thPracParam.phase) {
            case 1:
                ecl << pair{0xe98, 3500}; // Set Health
                ecl << pair{0xa6ec, (int16_t)0}; // Speed Up
                ecl << pair{0xa208, (int16_t)0};
                ecl << pair{0xa00c, (int16_t)0}; // Disable Anm
                break;
            case 2:
                ecl << pair{0xe98, 2500}; // Set Health
                ecl << pair{0xa6ec, (int16_t)0}; // Speed Up
                ecl << pair{0xa208, (int16_t)0};
                ecl << pair{0xa340, (int16_t)0};
                ecl << pair{0xa00c, (int16_t)0}; // Disable Anm
                break;
            case 3:
                ecl << pair{0xe98, 1500}; // Set Health
                ecl << pair{0xa6ec, (int16_t)0}; // Speed Up
                ecl << pair{0xa208, (int16_t)0};
                ecl << pair{0xa340, (int16_t)0};
                ecl << pair{0xa4b4, (int16_t)0};
                ecl << pair{0xa00c, (int16_t)0}; // Disable Anm
                break;
            default:
                break;
            }
            break;
        case THPrac::TH16::TH16_ST6_SUMMER_FINAL:
            ECLJump(ecl, 0x56c0, 0x5940, 60);
            ecl.SetFile(2);
            ECLST6Background(ecl, 6);
            ECLJump(ecl, 0x680, 0xe88, 1); // Utilize Spell Practice Jump
            ecl << pair{0xe98, 5000}; // Set Health
            ecl << pair{0xeb8, (int8_t)0x37}; // Set Spell Ordinal
            switch (thPracParam.phase) {
            case 1:
                ecl << pair{0xe98, 3500}; // Set Health
                ecl << pair{0xc99c, (int16_t)0}; // Speed Up 1
                ecl << pair{0xd774, (int16_t)0}; // Speed Up 2
                ecl << pair{0xc4e0, (int16_t)0};
                ecl << pair{0xc2e4, (int16_t)0}; // Disable Anm
                break;
            case 2:
                ecl << pair{0xe98, 2500}; // Set Health
                ecl << pair{0xc99c, (int16_t)0}; // Speed Up 1
                ecl << pair{0xd774, (int16_t)0}; // Speed Up 2
                ecl << pair{0xc4e0, (int16_t)0};
                ecl << pair{0xc618, (int16_t)0};
                ecl << pair{0xc2e4, (int16_t)0}; // Disable Anm
                break;
            case 3:
                ecl << pair{0xe98, 1500}; // Set Health
                ecl << pair{0xc99c, (int16_t)0}; // Speed Up 1
                ecl << pair{0xd774, (int16_t)0}; // Speed Up 2
                ecl << pair{0xc4e0, (int16_t)0};
                ecl << pair{0xc618, (int16_t)0};
                ecl << pair{0xc78c, (int16_t)0};
                ecl << pair{0xc2e4, (int16_t)0}; // Disable Anm
                break;
            default:
                break;
            }
            break;
        case THPrac::TH16::TH16_ST6_AUTUMN_FINAL:
            ECLJump(ecl, 0x56c0, 0x5940, 60);
            ecl.SetFile(2);
            ECLST6Background(ecl, 6);
            ECLJump(ecl, 0x680, 0xe88, 1); // Utilize Spell Practice Jump
            ecl << pair{0xe98, 3200}; // Set Health
            ecl << pair{0xeb8, (int8_t)0x38}; // Set Spell Ordinal
            switch (thPracParam.phase) {
            case 1:
                ecl << pair{0xdc48, 2400}; // Set Health
                ecl << pair{0xe6ac, (int16_t)0}; // Speed Up 1
                ecl << pair{0xf4ec, (int16_t)0}; // Speed Up 2
                ecl << pair{0xe6fc, (int16_t)0}; // Speed Up 3
                ecl << pair{0xe1f0, (int16_t)0};
                ecl << pair{0xdff4, (int16_t)0}; // Disable Anm
                break;
            case 2:
                ecl << pair{0xdc48, 1600}; // Set Health
                ecl << pair{0xe6ac, (int16_t)0}; // Speed Up 1
                ecl << pair{0xf4ec, (int16_t)0}; // Speed Up 2
                ecl << pair{0xe6fc, (int16_t)0}; // Speed Up 3
                ecl << pair{0xe1f0, (int16_t)0};
                ecl << pair{0xe328, (int16_t)0};
                ecl << pair{0xdff4, (int16_t)0}; // Disable Anm
                break;
            case 3:
                ecl << pair{0xdc48, 800}; // Set Health
                ecl << pair{0xe6ac, (int16_t)0}; // Speed Up 1
                ecl << pair{0xf4ec, (int16_t)0}; // Speed Up 2
                ecl << pair{0xe6fc, (int16_t)0}; // Speed Up 3
                ecl << pair{0xe1f0, (int16_t)0};
                ecl << pair{0xe328, (int16_t)0};
                ecl << pair{0xe49c, (int16_t)0};
                ecl << pair{0xdff4, (int16_t)0}; // Disable Anm
                break;
            default:
                break;
            }
            break;
        case THPrac::TH16::TH16_ST6_WINTER_FINAL:
            ECLJump(ecl, 0x56c0, 0x5940, 60);
            ecl.SetFile(2);
            ECLST6Background(ecl, 6);
            ECLJump(ecl, 0x680, 0xe88, 1); // Utilize Spell Practice Jump
            ecl << pair{0xe98, 5300}; // Set Health
            ecl << pair{0xeb8, (int8_t)0x39}; // Set Spell Ordinal
            switch (thPracParam.phase) {
            case 1:
                ecl << pair{0xfa40, 3700}; // Set Health
                ecl << pair{0x104a4, (int16_t)0}; // Speed Up 1
                ecl << pair{0x11764, (int16_t)0}; // Speed Up 2
                ecl << pair{0xffe8, (int16_t)0};
                ecl << pair{0xfdec, (int16_t)0}; // Disable Anm
                break;
            case 2:
                ecl << pair{0xfa40, 2600}; // Set Health
                ecl << pair{0x104a4, (int16_t)0}; // Speed Up 1
                ecl << pair{0x11764, (int16_t)0}; // Speed Up 2
                ecl << pair{0xffe8, (int16_t)0};
                ecl << pair{0x10120, (int16_t)0};
                ecl << pair{0xfdec, (int16_t)0}; // Disable Anm
                break;
            case 3:
                ecl << pair{0xfa40, 1500}; // Set Health
                ecl << pair{0x104a4, (int16_t)0}; // Speed Up 1
                ecl << pair{0x11764, (int16_t)0}; // Speed Up 2
                ecl << pair{0xffe8, (int16_t)0};
                ecl << pair{0x10120, (int16_t)0};
                ecl << pair{0x10294, (int16_t)0};
                ecl << pair{0xfdec, (int16_t)0}; // Disable Anm
                break;
            default:
                break;
            }
            break;
        case THPrac::TH16::TH16_ST7_MID1:
            if (thPracParam.dlg)
                ECLJump(ecl, 0x988c, 0x9b4c, 60);
            else {
                ECLJump(ecl, 0x988c, 0x9b84, 60);

                // Boss A
                ecl.SetFile(2);
                ECLJump(ecl, 0x1f0, 0x720, 0);
                ecl.SetPos(0x720);
                ecl << 0 << 0x00140203 << 0x01ff0000 << 0 << 101;
                ecl << 0 << 0x0014002a << 0x01ff0000 << 0 << 0
                    << 0 << 0x0014002b << 0x01ff0001 << 0 << -9949;
                ECLJump(ecl, ecl.GetPos(), 0x218, 0);
                ECLJump(ecl, 0x458, 0x4a8, 0);
                ECLJump(ecl, 0x4bc, 0x50c, 0);

                // Boss B
                ecl.SetFile(3);
                ECLJump(ecl, 0x7e0, 0xc40, 0);
                ecl.SetPos(0xc40);
                ecl << 0 << 0x00140203 << 0x01ff0000 << 0 << 101;
                ecl << 0 << 0x0014002a << 0x01ff0001 << 0 << -9914
                    << 0 << 0x0014002b << 0x01ff0001 << 0 << -9925;
                ECLJump(ecl, ecl.GetPos(), 0x808, 0);
                ECLJump(ecl, 0x978, 0x9c8, 0);
                ECLJump(ecl, 0x9dc, 0xa2c, 0);
            }
            break;
        case THPrac::TH16::TH16_ST7_MID2:
            ECLJump(ecl, 0x988c, 0x9b84, 60);

            // Boss A
            ecl.SetFile(2);
            ECLJump(ecl, 0x1f0, 0x720, 0);
            ecl.SetPos(0x720);
            ecl << 0 << 0x00140203 << 0x01ff0000 << 0 << 101;
            ecl << 0 << 0x0014002a << 0x01ff0000 << 0 << 0
                << 0 << 0x0014002b << 0x01ff0001 << 0 << -9949;
            ECLJump(ecl, ecl.GetPos(), 0x218, 0);
            ECLJump(ecl, 0x458, 0x4a8, 0);
            ECLJump(ecl, 0x4bc, 0x5a8, 0);

            // Boss B
            ecl.SetFile(3);
            ECLJump(ecl, 0x7e0, 0xc40, 0);
            ecl.SetPos(0xc40);
            ecl << 0 << 0x00140203 << 0x01ff0000 << 0 << 101;
            ecl << 0 << 0x0014002a << 0x01ff0001 << 0 << -9914
                << 0 << 0x0014002b << 0x01ff0001 << 0 << -9925;
            ECLJump(ecl, ecl.GetPos(), 0x808, 0);
            ECLJump(ecl, 0x978, 0x9c8, 0);
            ECLJump(ecl, 0x9dc, 0xac8, 0);
            break;
        case THPrac::TH16::TH16_ST7_MID3:
            ECLJump(ecl, 0x988c, 0x9b84, 60);

            // Boss A
            ecl.SetFile(2);
            ECLJump(ecl, 0x1f0, 0x720, 0);
            ecl.SetPos(0x720);
            ecl << 0 << 0x00140203 << 0x01ff0000 << 0 << 101;
            ecl << 0 << 0x0014002a << 0x01ff0000 << 0 << 0
                << 0 << 0x0014002b << 0x01ff0001 << 0 << -9949;
            ECLJump(ecl, ecl.GetPos(), 0x218, 0);
            ECLJump(ecl, 0x458, 0x4a8, 0);
            ECLJump(ecl, 0x4bc, 0x644, 0);

            // Boss B
            ecl.SetFile(3);
            ECLJump(ecl, 0x7e0, 0xc40, 0);
            ecl.SetPos(0xc40);
            ecl << 0 << 0x00140203 << 0x01ff0000 << 0 << 101;
            ecl << 0 << 0x0014002a << 0x01ff0001 << 0 << -9914
                << 0 << 0x0014002b << 0x01ff0001 << 0 << -9925;
            ECLJump(ecl, ecl.GetPos(), 0x808, 0);
            ECLJump(ecl, 0x978, 0x9c8, 0);
            ECLJump(ecl, 0x9dc, 0xb64, 0);
            break;
        case THPrac::TH16::TH16_ST7_END_NS1:
            if (thPracParam.dlg)
                ECLJump(ecl, 0x988c, 0x9c88, 60);
            else
                ECLJump(ecl, 0x988c, 0x9c9c, 60);
            break;
        case THPrac::TH16::TH16_ST7_END_S1:
            ECLJump(ecl, 0x988c, 0x9c9c, 60);
            ecl.SetFile(4);

            ECLJump(ecl, 0x8d0, 0x970, 3); // Utilize Spell Practice Jump
            ecl << pair{0x980, 3000}; // Set Health
            ecl << pair{0x9a0, (int8_t)0x31}; // Set Spell Ordinal
            ecl << pair{0x5958, (int16_t)0}; // Disable Item Drops
            break;
        case THPrac::TH16::TH16_ST7_END_NS2:
            ECLJump(ecl, 0x988c, 0x9c9c, 60);
            ecl.SetFile(4);
            ecl << pair{0xf58, (int8_t)0x32}; // Change Nonspell
            ecl << pair{0x19bc, (int16_t)0} << pair{0x1aec, (int16_t)0}; // Disable Item Drops & SE
            ecl << pair{0x1c80, 59} << pair{0x1ca0, 0} << pair{0x18e4, 0}; // Change Move Time, Wait Time & Inv. Time
            break;
        case THPrac::TH16::TH16_ST7_END_S2:
            ECLJump(ecl, 0x988c, 0x9c9c, 60);
            ecl.SetFile(4);

            ECLJump(ecl, 0x8d0, 0x970, 3); // Utilize Spell Practice Jump
            ecl << pair{0x980, 2000}; // Set Health
            ecl << pair{0x9a0, (int8_t)0x32}; // Set Spell Ordinal
            ecl << pair{0x78b8, (int16_t)0}; // Disable Item Drops
            break;
        case THPrac::TH16::TH16_ST7_END_NS3:
            ECLJump(ecl, 0x988c, 0x9c9c, 60);
            ecl.SetFile(4);
            ecl << pair{0xf58, (int8_t)0x33}; // Change Nonspell
            ecl << pair{0x21cc, (int16_t)0} << pair{0x22fc, (int16_t)0}; // Disable Item Drops & SE
            ecl << pair{0x2490, 59} << pair{0x24b0, 0} << pair{0x20f4, 0}; // Change Move Time, Wait Time & Inv. Time
            break;
        case THPrac::TH16::TH16_ST7_END_S3:
            ECLJump(ecl, 0x988c, 0x9c9c, 60);
            ecl.SetFile(4);

            ECLJump(ecl, 0x8d0, 0x970, 3); // Utilize Spell Practice Jump
            ecl << pair{0x980, 3500}; // Set Health
            ecl << pair{0x9a0, (int8_t)0x33}; // Set Spell Ordinal
            ecl << pair{0x88e4, (int16_t)0}; // Disable Item Drops
            break;
        case THPrac::TH16::TH16_ST7_END_NS4:
            ECLJump(ecl, 0x988c, 0x9c9c, 60);
            ecl.SetFile(4);
            ecl << pair{0xf58, (int8_t)0x34}; // Change Nonspell
            ecl << pair{0x29dc, (int16_t)0} << pair{0x2b0c, (int16_t)0}; // Disable Item Drops & SE
            ecl << pair{0x2ca0, 59} << pair{0x2cd0, 0} << pair{0x2904, 0}; // Change Move Time, Wait Time & Inv. Time
            break;
        case THPrac::TH16::TH16_ST7_END_S4:
            ECLJump(ecl, 0x988c, 0x9c9c, 60);
            ecl.SetFile(4);

            ECLJump(ecl, 0x8d0, 0x970, 3); // Utilize Spell Practice Jump
            ecl << pair{0x980, 2700}; // Set Health
            ecl << pair{0x9a0, (int8_t)0x34}; // Set Spell Ordinal
            ecl << pair{0x9450, (int16_t)0}; // Disable Item Drops
            break;
        case THPrac::TH16::TH16_ST7_END_NS5:
            ECLJump(ecl, 0x988c, 0x9c9c, 60);
            ecl.SetFile(4);
            ecl << pair{0xf58, (int8_t)0x35}; // Change Nonspell
            ecl << pair{0x31fc, (int16_t)0} << pair{0x332c, (int16_t)0}; // Disable Item Drops & SE
            ecl << pair{0x34e8, 0} << pair{0x3124, 0}; // Change Wait Time & Inv. Time
            ecl << pair{0x49c, 0.0f} << pair{0x4a0, 356.0f}; // Boss Pos
            break;
        case THPrac::TH16::TH16_ST7_END_S5:
            ECLJump(ecl, 0x988c, 0x9c9c, 60);
            ecl.SetFile(4);

            ECLJump(ecl, 0x8d0, 0x970, 3); // Utilize Spell Practice Jump
            ecl << pair{0x980, 2000}; // Set Health
            ecl << pair{0x9a0, (int8_t)0x35}; // Set Spell Ordinal
            ecl << pair{0x9ecc, (int16_t)0}; // Disable Item Drops
            break;
        case THPrac::TH16::TH16_ST7_END_NS6:
            ECLJump(ecl, 0x988c, 0x9c9c, 60);
            ecl.SetFile(4);
            ecl << pair{0xf58, (int8_t)0x36}; // Change Nonspell
            ecl << pair{0x3d94, (int16_t)0} << pair{0x3ec4, (int16_t)0}; // Disable Item Drops & SE
            ecl << pair{0x4058, 59} << pair{0x4088, 0} << pair{0x3cbc, 0}; // Change Move Time, Wait Time & Inv. Time
            break;
        case THPrac::TH16::TH16_ST7_END_S6:
            ECLJump(ecl, 0x988c, 0x9c9c, 60);
            ecl.SetFile(4);

            ECLJump(ecl, 0x8d0, 0x970, 3); // Utilize Spell Practice Jump
            ecl << pair{0x980, 3000}; // Set Health
            ecl << pair{0x9a0, (int8_t)0x36}; // Set Spell Ordinal
            ecl << pair{0xaaec, (int16_t)0}; // Disable Item Drops
            break;
        case THPrac::TH16::TH16_ST7_END_NS7:
            ECLJump(ecl, 0x988c, 0x9c9c, 60);
            ecl.SetFile(4);
            ecl << pair{0xf58, (int8_t)0x37}; // Change Nonspell
            ecl << pair{0x45b4, (int16_t)0} << pair{0x46e4, (int16_t)0}; // Disable Item Drops & SE
            ecl << pair{0x4878, 59} << pair{0x48a8, 0} << pair{0x44dc, 0}; // Change Move Time, Wait Time & Inv. Time
            break;
        case THPrac::TH16::TH16_ST7_END_S7:
            ECLJump(ecl, 0x988c, 0x9c9c, 60);
            ecl.SetFile(4);

            ECLJump(ecl, 0x8d0, 0x970, 3); // Utilize Spell Practice Jump
            ecl << pair{0x980, 3000}; // Set Health
            ecl << pair{0x9a0, (int8_t)0x37}; // Set Spell Ordinal
            ecl << pair{0xb384, (int16_t)0}; // Disable Item Drops
            break;
        case THPrac::TH16::TH16_ST7_END_NS8:
            ECLJump(ecl, 0x988c, 0x9c9c, 60);
            ecl.SetFile(4);
            ecl << pair{0xf58, (int8_t)0x38}; // Change Nonspell
            ecl << pair{0x4dd4, (int16_t)0} << pair{0x4f04, (int16_t)0}; // Disable Item Drops & SE
            ecl << pair{0x5098, 59} << pair{0x50c8, 0} << pair{0x4cfc, 0}; // Change Move Time, Wait Time & Inv. Time
            break;
        case THPrac::TH16::TH16_ST7_END_S8:
            ECLJump(ecl, 0x988c, 0x9c9c, 60);
            ecl.SetFile(4);

            ECLJump(ecl, 0x8d0, 0x970, 3); // Utilize Spell Practice Jump
            ecl << pair{0x980, 2400}; // Set Health
            ecl << pair{0x9a0, (int8_t)0x38}; // Set Spell Ordinal
            ecl << pair{0xbdb8, (int16_t)0}; // Disable Item Drops
            break;
        case THPrac::TH16::TH16_ST7_END_S9:
            ECLJump(ecl, 0x988c, 0x9c9c, 60);
            ecl.SetFile(4);

            ECLJump(ecl, 0x8d0, 0x970, 3); // Utilize Spell Practice Jump
            ecl << pair{0x980, 3000}; // Set Health
            ecl << pair{0x9a0, (int8_t)0x39}; // Set Spell Ordinal
            ecl << pair{0x55f4, (int16_t)0}; // Disable Item Drops
            ecl << pair{0x49c, 0.0f} << pair{0x4a0, 416.0f}; // Boss Pos
            ECLJump(ecl, 0x62c, 0x6d8, 0); // Skip 503(3)
            ECLJump(ecl, 0x794, 0x84c, 3); // Skip 504
            break;
        case THPrac::TH16::TH16_ST7_END_S10:
            ECLJump(ecl, 0x988c, 0x9c9c, 60);
            ecl.SetFile(4);

            ECLJump(ecl, 0x8d0, 0x970, 3); // Utilize Spell Practice Jump
            ecl << pair{0x980, 5500}; // Set Health
            ecl << pair{0x9a0, (int8_t)0x31};
            ecl << pair{0x9a1, (int8_t)0x30}; // Set Spell Ordinal
            ecl << pair{0x49c, 0.0f} << pair{0x4a0, 416.0f}; // Boss Pos
            ECLJump(ecl, 0x794, 0x84c, 3); // Skip 504

            switch (thPracParam.phase) {
            case 1:
                ecl << pair{0x980, 4000};
                ECLJump(ecl, 0x6270, 0x62cc, 90);
                break;
            case 2:
                ecl << pair{0x980, 2500};
                ECLJump(ecl, 0x6270, 0x62cc, 90);
                ECLJump(ecl, 0x62f0, 0x634c, 90);
                break;
            case 3:
                ecl << pair{0x980, 1000};
                ECLJump(ecl, 0x6270, 0x62cc, 90);
                ECLJump(ecl, 0x62f0, 0x634c, 90);
                ECLJump(ecl, 0x6370, 0x63cc, 90);
                break;
            default:
                break;
            }
            break;
        default:
            break;
        }
    }
    __declspec(noinline) void THPatchSP(ECLHelper& ecl)
    {
        uint32_t spell_id = *(int32_t*)(0x4a57c8);
        switch (spell_id) {
        case 90:
        case 91:
        case 92:
        case 93:
            ecl.SetFile(2);
            switch (thPracParam.phase) {
            case 1:
                ecl << pair{0xbec, 3500}; // Set Health
                ecl << pair{0xa6ec, (int16_t)0}; // Speed Up
                ecl << pair{0xa208, (int16_t)0};
                ecl << pair{0xa00c, (int16_t)0}; // Disable Anm
                break;
            case 2:
                ecl << pair{0xbec, 2500}; // Set Health
                ecl << pair{0xa6ec, (int16_t)0}; // Speed Up
                ecl << pair{0xa208, (int16_t)0};
                ecl << pair{0xa340, (int16_t)0};
                ecl << pair{0xa00c, (int16_t)0}; // Disable Anm
                break;
            case 3:
                ecl << pair{0xbec, 1500}; // Set Health
                ecl << pair{0xa6ec, (int16_t)0}; // Speed Up
                ecl << pair{0xa208, (int16_t)0};
                ecl << pair{0xa340, (int16_t)0};
                ecl << pair{0xa4b4, (int16_t)0};
                ecl << pair{0xa00c, (int16_t)0}; // Disable Anm
                break;
            default:
                break;
            }
            break;
        case 94:
        case 95:
        case 96:
        case 97:
            ecl.SetFile(2);
            switch (thPracParam.phase) {
            case 1:
                ecl << pair{0xcd0, 3500}; // Set Health
                ecl << pair{0xc99c, (int16_t)0}; // Speed Up 1
                ecl << pair{0xd774, (int16_t)0}; // Speed Up 2
                ecl << pair{0xc4e0, (int16_t)0};
                ecl << pair{0xc2e4, (int16_t)0}; // Disable Anm
                break;
            case 2:
                ecl << pair{0xcd0, 2500}; // Set Health
                ecl << pair{0xc99c, (int16_t)0}; // Speed Up 1
                ecl << pair{0xd774, (int16_t)0}; // Speed Up 2
                ecl << pair{0xc4e0, (int16_t)0};
                ecl << pair{0xc618, (int16_t)0};
                ecl << pair{0xc2e4, (int16_t)0}; // Disable Anm
                break;
            case 3:
                ecl << pair{0xcd0, 1500}; // Set Health
                ecl << pair{0xc99c, (int16_t)0}; // Speed Up 1
                ecl << pair{0xd774, (int16_t)0}; // Speed Up 2
                ecl << pair{0xc4e0, (int16_t)0};
                ecl << pair{0xc618, (int16_t)0};
                ecl << pair{0xc78c, (int16_t)0};
                ecl << pair{0xc2e4, (int16_t)0}; // Disable Anm
                break;
            default:
                break;
            }
            break;
        case 98:
        case 99:
        case 100:
        case 101:
            ecl.SetFile(2);
            switch (thPracParam.phase) {
            case 1:
                ecl << pair{0xdc48, 2400}; // Set Health
                ecl << pair{0xe6ac, (int16_t)0}; // Speed Up 1
                ecl << pair{0xf4ec, (int16_t)0}; // Speed Up 2
                ecl << pair{0xe6fc, (int16_t)0}; // Speed Up 3
                ecl << pair{0xe1f0, (int16_t)0};
                ecl << pair{0xdff4, (int16_t)0}; // Disable Anm
                break;
            case 2:
                ecl << pair{0xdc48, 1600}; // Set Health
                ecl << pair{0xe6ac, (int16_t)0}; // Speed Up 1
                ecl << pair{0xf4ec, (int16_t)0}; // Speed Up 2
                ecl << pair{0xe6fc, (int16_t)0}; // Speed Up 3
                ecl << pair{0xe1f0, (int16_t)0};
                ecl << pair{0xe328, (int16_t)0};
                ecl << pair{0xdff4, (int16_t)0}; // Disable Anm
                break;
            case 3:
                ecl << pair{0xdc48, 800}; // Set Health
                ecl << pair{0xe6ac, (int16_t)0}; // Speed Up 1
                ecl << pair{0xf4ec, (int16_t)0}; // Speed Up 2
                ecl << pair{0xe6fc, (int16_t)0}; // Speed Up 3
                ecl << pair{0xe1f0, (int16_t)0};
                ecl << pair{0xe328, (int16_t)0};
                ecl << pair{0xe49c, (int16_t)0};
                ecl << pair{0xdff4, (int16_t)0}; // Disable Anm
                break;
            default:
                break;
            }
            break;
        case 102:
        case 103:
        case 104:
        case 105:
            ecl.SetFile(2);
            switch (thPracParam.phase) {
            case 1:
                ecl << pair{0xfa40, 3700}; // Set Health
                ecl << pair{0x104a4, (int16_t)0}; // Speed Up 1
                ecl << pair{0x11764, (int16_t)0}; // Speed Up 2
                ecl << pair{0xffe8, (int16_t)0};
                ecl << pair{0xfdec, (int16_t)0}; // Disable Anm
                break;
            case 2:
                ecl << pair{0xfa40, 2600}; // Set Health
                ecl << pair{0x104a4, (int16_t)0}; // Speed Up 1
                ecl << pair{0x11764, (int16_t)0}; // Speed Up 2
                ecl << pair{0xffe8, (int16_t)0};
                ecl << pair{0x10120, (int16_t)0};
                ecl << pair{0xfdec, (int16_t)0}; // Disable Anm
                break;
            case 3:
                ecl << pair{0xfa40, 1500}; // Set Health
                ecl << pair{0x104a4, (int16_t)0}; // Speed Up 1
                ecl << pair{0x11764, (int16_t)0}; // Speed Up 2
                ecl << pair{0xffe8, (int16_t)0};
                ecl << pair{0x10120, (int16_t)0};
                ecl << pair{0x10294, (int16_t)0};
                ecl << pair{0xfdec, (int16_t)0}; // Disable Anm
                break;
            default:
                break;
            }
            break;
        case 118:
            ecl.SetFile(4);
            switch (thPracParam.phase) {
            case 1:
                ecl << pair{0xefc, 4000};
                ECLJump(ecl, 0x6270, 0x62cc, 90);
                break;
            case 2:
                ecl << pair{0xefc, 2500};
                ECLJump(ecl, 0x6270, 0x62cc, 90);
                ECLJump(ecl, 0x62f0, 0x634c, 90);
                break;
            case 3:
                ecl << pair{0xefc, 1000};
                ECLJump(ecl, 0x6270, 0x62cc, 90);
                ECLJump(ecl, 0x62f0, 0x634c, 90);
                ECLJump(ecl, 0x6370, 0x63cc, 90);
                break;
            default:
                break;
            }
            break;
        default:
            break;
        }
    }
    __declspec(noinline) void THSectionPatch()
    {
        ECLHelper ecl;
        ecl.SetBaseAddr((void*)GetMemAddr(0x4a6dc0, 0x17c, 0xC));

        auto section = thPracParam.section;
        if (section >= 10000 && section < 20000) {
            int stage = (section - 10000) / 100;
            int portionId = (section - 10000) % 100;
            THStageWarp(ecl, stage, portionId);
        } else {
            THPatch(ecl, (th_sections_t)section);
        }
    }

    int THBGMTest()
    {
        if (!thPracParam.mode)
            return 0;
        else if (thPracParam.section >= 10000)
            return 0;
        else
            return th_sections_bgm[thPracParam.section];
    }
    void THSaveReplay(char* repName)
    {
        ReplaySaveParam(mb_to_utf16(repName).c_str(), thPracParam.GetJson());
    }

    unsigned char th16_spbugfix_ecl1[448] = {
        0x00, 0x00, 0x00, 0x00, 0x2A, 0x00, 0x14, 0x00, 0x01, 0x00, 0xFF, 0x01,
        0x00, 0x00, 0x00, 0x00, 0x3D, 0xD9, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00,
        0x2A, 0x00, 0x14, 0x00, 0x00, 0x00, 0xFF, 0x01, 0x00, 0x00, 0x00, 0x00,
        0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3D, 0x00, 0x10, 0x00,
        0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x0D, 0x00, 0x18, 0x00, 0x00, 0x00, 0xFF, 0x02, 0x00, 0x00, 0x00, 0x00,
        0x68, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x2A, 0x00, 0x14, 0x00, 0x01, 0x00, 0xFF, 0x01, 0x00, 0x00, 0x00, 0x00,
        0x46, 0xD9, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x2A, 0x00, 0x14, 0x00,
        0x01, 0x00, 0xFF, 0x01, 0x00, 0x00, 0x00, 0x00, 0x3B, 0xD9, 0xFF, 0xFF,
        0x00, 0x00, 0x00, 0x00, 0x3B, 0x00, 0x10, 0x00, 0x00, 0x00, 0xFF, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0D, 0x00, 0x18, 0x00,
        0x00, 0x00, 0xFF, 0x02, 0x00, 0x00, 0x00, 0x00, 0x7C, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2A, 0x00, 0x14, 0x00,
        0x00, 0x00, 0xFF, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x2B, 0x00, 0x14, 0x00, 0x01, 0x00, 0xFF, 0x01,
        0x00, 0x00, 0x00, 0x00, 0x3D, 0xD9, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00,
        0x20, 0x03, 0x24, 0x00, 0x01, 0x00, 0xFF, 0x02, 0x00, 0x00, 0x00, 0x00,
        0x3A, 0xD9, 0xFF, 0xFF, 0x0C, 0x00, 0x00, 0x00, 0x42, 0x6F, 0x73, 0x73,
        0x45, 0x73, 0x63, 0x61, 0x70, 0x65, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x0C, 0x00, 0x18, 0x00, 0x00, 0x00, 0xFF, 0x02, 0x00, 0x00, 0x00, 0x00,
        0xB4, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x2A, 0x00, 0x14, 0x00, 0x01, 0x00, 0xFF, 0x01, 0x00, 0x00, 0x00, 0x00,
        0x46, 0xD9, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x2A, 0x00, 0x14, 0x00,
        0x01, 0x00, 0xFF, 0x01, 0x00, 0x00, 0x00, 0x00, 0x3A, 0xD9, 0xFF, 0xFF,
        0x00, 0x00, 0x00, 0x00, 0x3B, 0x00, 0x10, 0x00, 0x00, 0x00, 0xFF, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0D, 0x00, 0x18, 0x00,
        0x00, 0x00, 0xFF, 0x02, 0x00, 0x00, 0x00, 0x00, 0x64, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2A, 0x00, 0x14, 0x00,
        0x00, 0x00, 0xFF, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x2B, 0x00, 0x14, 0x00, 0x01, 0x00, 0xFF, 0x01,
        0x00, 0x00, 0x00, 0x00, 0x3D, 0xD9, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00,
        0x20, 0x03, 0x24, 0x00, 0x01, 0x00, 0xFF, 0x02, 0x00, 0x00, 0x00, 0x00,
        0x3B, 0xD9, 0xFF, 0xFF, 0x0C, 0x00, 0x00, 0x00, 0x42, 0x6F, 0x73, 0x73,
        0x45, 0x73, 0x63, 0x61, 0x70, 0x65, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x0B, 0x02, 0x10, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0xF9, 0x01, 0x10, 0x00, 0x00, 0x00, 0xFF, 0x00,
        0x00, 0x00, 0x00, 0x00
    };
    static char th16_spbugfix_str1[] { "BossDeadB" };
    __declspec(noinline) int TH16SPEclPatch(char* sub_str)
    {
        int signal = 0;
        void* ecl_addr;
        if (!strcmp(sub_str, "BossCard1Hide")) {
            signal = 1;
            return signal;
        } else if (!strcmp(sub_str, "Boss2")) {
            signal = 2;
            ecl_addr = sub_str - 0x74c0;
        } else if (!strcmp(sub_str, "Boss2B")) {
            signal = 3;
            ecl_addr = sub_str - 0x7d94;
        } else if (!strcmp(sub_str, "Boss3")) {
            signal = 2;
            ecl_addr = sub_str - 0x9d24;
        } else if (!strcmp(sub_str, "Boss3B")) {
            signal = 3;
            ecl_addr = sub_str - 0xa344;
        } else if (!strcmp(sub_str, "Boss4B")) {
            signal = 3;
            return signal;
        } else if (!strcmp(sub_str, "BossDeadB")) {
            signal = 3;
            return signal;
        } else {
            return signal;
        }

        VFile ecl;
        ecl.SetFile(ecl_addr, 0x10f4c);

        ecl.SetPos(0x1068c);
        ecl << 0 << 0x0018000C << 0x02ff0000 << 0x00000000 << 0x40c - 0x1068c << 0;

        ecl.SetPos(0x40c);
        ecl.Write(th16_spbugfix_ecl1, 448);
        ecl << 0 << 0x0018000C << 0x02ff0000 << 0x00000000 << 0x106bc - (0x40c + 448) << 0;

        return signal;
    }

    HOOKSET_DEFINE(THMainHook)
    EHOOK_DY(th16_spbugfix, 0x4214fa)
    {
        char* sub_str;
        int signal;
        sub_str = (char*)pCtx->Esi;
        sub_str += 0x20;

        signal = 0;
        if (thPracParam.bug_fix && (*((int32_t*)0x4a5bec) & 0x30) == 32 && *((int32_t*)0x4a5790) == 5) {
            signal = TH16SPEclPatch(sub_str);
            if (signal == 1) {
                *(DWORD*)(pCtx->Ebp - 0x420) = pCtx->Eax;
                pCtx->Eip = 0x422a9b;
            } else if (signal == 2) {
                *(*((int32_t**)0x4a6dc0) + 6) = 0;
            } else if (signal == 3) {
                *(*((int32_t**)0x4a6dc0) + 6) = 0;

                pCtx->Esi = asm_call<0x4251d0, Thiscall, DWORD>(pCtx->Edi, 2);
                pCtx->Eax = asm_call<0x4251d0, Thiscall, DWORD>(pCtx->Edi, 0);

                PushHelper32(pCtx, (DWORD)th16_spbugfix_str1);
                pCtx->Eip = 0x42159b;
            }
        }
    }
    EHOOK_DY(th16_everlasting_bgm, 0x45ed00)
    {
        auto isInReplay = []() -> bool {
            if (*(uintptr_t*)0x4e9bc4)
                return GetMemContent(0x4e9bc4, 0x10);
            return false;
        };

        int32_t retn_addr = ((int32_t*)pCtx->Esp)[0];
        int32_t bgm_cmd = ((int32_t*)pCtx->Esp)[1];
        int32_t bgm_id = ((int32_t*)pCtx->Esp)[2];
        int32_t call_addr = ((int32_t*)pCtx->Esp)[3];

        bool el_switch;
        bool is_practice;
        bool result;

        el_switch = *(THOverlay::singleton().mElBgm) && !isInReplay() && (thPracParam.mode == 1) && thPracParam.section;
        is_practice = (*((int32_t*)0x4a5bec) & 0x1);
        result = ElBgmTest<0x43c423, 0x42d6b9, 0x43f199, 0x4409c0, 0xffffffff>(
            el_switch, is_practice, retn_addr, bgm_cmd, bgm_id, 0xffffffff);

        if (result) {
            pCtx->Eip = 0x45ed93;
        }
    }
    EHOOK_DY(th16_param_reset, 0x44b610)
    {
        thPracParam.Reset();
        thSubSeasonB = -1;
    }
    EHOOK_DY(th16_prac_menu_1, 0x450f60)
    {
        THGuiPrac::singleton().State(1);
    }
    EHOOK_DY(th16_prac_menu_2, 0x450f83)
    {
        THGuiPrac::singleton().State(2);
    }
    EHOOK_DY(th16_prac_menu_3, 0x4512cc)
    {
        THGuiPrac::singleton().State(3);
    }
    EHOOK_DY(th16_prac_menu_4, 0x45136d)
    {
        THGuiPrac::singleton().State(4);
    }
    PATCH_DY(th16_prac_menu_enter_1, 0x451044, "\xeb", 1);
    EHOOK_DY(th16_prac_menu_enter_2, 0x451327)
    {
        // Change sub-season to dog days if playing extra
        if (thPracParam.stage == 6) {
            thSubSeasonB = *((int32_t*)0x4a57ac);
            *((int32_t*)0x4a57ac) = 4;
        }

        pCtx->Ecx = thPracParam.stage;
    }
    EHOOK_DY(th16_disable_prac_menu_1, 0x4514d1)
    {
        pCtx->Eip = 0x45150e;
    }
    PATCH_DY(th16_disable_prac_menu_2, 0x451013, "\x00", 1);
    EHOOK_DY(th16_menu_rank_fix, 0x44097f)
    {
        *((int32_t*)0x4a57c8) = -1; // Reset spell practice ID
        *((int32_t*)0x4a57b4) = *((int32_t*)0x49f274); // Restore In-game rank to menu rank
        if (thSubSeasonB != -1) // Restore sub-season
        {
            *((int32_t*)0x4a57ac) = thSubSeasonB;
            thSubSeasonB = -1;
        }
    }
    EHOOK_DY(th16_patch_main, 0x42d1ec)
    {
        if (thPracParam.mode == 1) {
            *(int32_t*)(0x4a57b0) = (int32_t)(thPracParam.score / 10);
            *(int32_t*)(0x4a57f4) = thPracParam.life;
            *(int32_t*)(0x4a5800) = thPracParam.bomb;
            *(int32_t*)(0x4a5804) = thPracParam.bomb_fragment;
            *(int32_t*)(0x4a57e4) = thPracParam.power;
            *(int32_t*)(0x4a57d8) = thPracParam.value * 100;
            *(int32_t*)(0x4a57c0) = thPracParam.graze;
            switch (thPracParam.season_gauge) {
            case 1:
                *(int32_t*)(0x4a5808) = 100;
                break;
            case 2:
                *(int32_t*)(0x4a5808) = 230;
                break;
            case 3:
                *(int32_t*)(0x4a5808) = 390;
                break;
            case 4:
                *(int32_t*)(0x4a5808) = 590;
                break;
            case 5:
                *(int32_t*)(0x4a5808) = 840;
                break;
            case 6:
                *(int32_t*)(0x4a5808) = 1140;
                break;
            default:
                *(int32_t*)(0x4a5808) = 0;
                break;
            }

            if (*(int32_t*)(0x4a57b4) == 4) {

                if (thPracParam.score >= 100000000)
                    *(int32_t*)(0x4a57fc) = 6;
                else if (thPracParam.score >= 80000000)
                    *(int32_t*)(0x4a57fc) = 5;
                else if (thPracParam.score >= 60000000)
                    *(int32_t*)(0x4a57fc) = 4;
                else if (thPracParam.score >= 40000000)
                    *(int32_t*)(0x4a57fc) = 3;
                else if (thPracParam.score >= 20000000)
                    *(int32_t*)(0x4a57fc) = 2;
                else if (thPracParam.score >= 10000000)
                    *(int32_t*)(0x4a57fc) = 1;
            } else {
                if (thPracParam.score >= 10000000000)
                    *(int32_t*)(0x4a57fc) = 11;
                else if (thPracParam.score >= 1000000000)
                    *(int32_t*)(0x4a57fc) = 10;
                else if (thPracParam.score >= 500000000)
                    *(int32_t*)(0x4a57fc) = 9;
                else if (thPracParam.score >= 250000000)
                    *(int32_t*)(0x4a57fc) = 8;
                else if (thPracParam.score >= 150000000)
                    *(int32_t*)(0x4a57fc) = 7;
                else if (thPracParam.score >= 100000000)
                    *(int32_t*)(0x4a57fc) = 6;
                else if (thPracParam.score >= 70000000)
                    *(int32_t*)(0x4a57fc) = 5;
                else if (thPracParam.score >= 40000000)
                    *(int32_t*)(0x4a57fc) = 4;
                else if (thPracParam.score >= 20000000)
                    *(int32_t*)(0x4a57fc) = 3;
                else if (thPracParam.score >= 10000000)
                    *(int32_t*)(0x4a57fc) = 2;
                else if (thPracParam.score >= 5000000)
                    *(int32_t*)(0x4a57fc) = 1;
            }

            THSectionPatch();
        } else if (thPracParam.mode == 2) {
            switch (thPracParam.season_gauge) {
            case 1:
                *(int32_t*)(0x4a5808) = 100;
                break;
            case 2:
                *(int32_t*)(0x4a5808) = 230;
                break;
            case 3:
                break;
            case 4:
                *(int32_t*)(0x4a5808) = 580;
                break;
            case 5:
                *(int32_t*)(0x4a5808) = 840;
                break;
            case 6:
                *(int32_t*)(0x4a5808) = 1140;
                break;
            default:
                *(int32_t*)(0x4a5808) = 0;
                break;
            }

            ECLHelper ecl;
            ecl.SetBaseAddr((void*)GetMemAddr(0x4a6dc0, 0x17c, 0xC));
            THPatchSP(ecl);
        }
        thPracParam._playLock = true;
    }
    EHOOK_DY(th16_bgm, 0x42de8c)
    {
        if (THBGMTest()) {
            PushHelper32(pCtx, 1);
            pCtx->Eip = 0x42de8e;
        }
    }
    EHOOK_DY(th16_rep_save, 0x448be4)
    {
        char* repName = (char*)(pCtx->Esp + 0x38);
        if (thPracParam.mode == 1)
        {
            THSaveReplay(repName);
        }
        else if (thPracParam.mode == 2)
        {
            if (thPracParam.season_gauge != 3 || thPracParam.phase || thPracParam.bug_fix)
                THSaveReplay(repName);
        }
    }
    EHOOK_DY(th16_rep_load, 0x447ae7)
    {
        thPracParam = {};
        std::string param;
        std::wstring path = mb_to_utf16((char*)0x4d7d0d) + L"replay\\" + mb_to_utf16((char*)0x4A6DE0);
        if (ReplayLoadParam(path.c_str(), param))
            thPracParam.ReadJson(param);
    }
    EHOOK_DY(th16_spmenu_alt_1, 0x455da6)
    {
        pCtx->Esp += 0x10;
        pCtx->Eip = 0x455dab;
    }
    PATCH_DY(th16_spmenu_alt_2, 0x455e75, "\x90\x90\x90\x90\x90", 5);
    EHOOK_DY(th16_spmenu_alt_3, 0x456b8b)
    {
        pCtx->Eip = 0x456d2a;
    }
    EHOOK_DY(th16_spmenu_alt_4, 0x456bb8)
    {
        pCtx->Eip = 0x456cba;
    }
    EHOOK_DY(th16_sp_menu_1, 0x455dbc)
    {
        THGuiSP::singleton().State(1);
    }
    EHOOK_DY(th16_sp_menu_2, 0x455dff)
    {
        THGuiSP::singleton().State(2);
    }
    EHOOK_DY(th16_sp_menu_3, 0x455eeb)
    {
        THGuiSP::singleton().State(3);
    }
    EHOOK_DY(th16_sp_menu_4, 0x455fee)
    {
        THGuiSP::singleton().State(4);
    }
    EHOOK_DY(th16_sp_menu_5, 0x455fe3)
    {
        THGuiSP::singleton().State(5);
    }
    EHOOK_DY(th16_update, 0x40156f)
    {
        GameGuiBegin(IMPL_WIN32_DX9, !THAdvOptWnd::singleton().IsOpen());

        // Gui components update
        THGuiPrac::singleton().Update();
        THOverlay::singleton().Update();
        THGuiSP::singleton().Update();
        bool drawCursor = THAdvOptWnd::StaticUpdate() || THGuiPrac::singleton().IsOpen() || THGuiSP::singleton().IsOpen();
        GameGuiEnd(drawCursor);
    }
    EHOOK_DY(th16_render, 0x40168a)
    {
        GameGuiRender(IMPL_WIN32_DX9);
    }
    HOOKSET_ENDDEF()

    HOOKSET_DEFINE(THInitHook)
    static __declspec(noinline) void THGuiCreate()
    {
        // Init
        GameGuiInit(IMPL_WIN32_DX9, 0x4c10d8, 0x4d7ce0, 0x45a450,
            Gui::INGAGME_INPUT_GEN2, 0x4a50bc, 0x4a50b8, 0,
            (*((int32_t*)0x4d9d1c) >> 2) & 0xf);

        // Gui components creation
        THGuiPrac::singleton();
        THOverlay::singleton();
        THGuiSP::singleton();

        // Hooks
        THMainHook::singleton().EnableAllHooks();

        // Reset thPracParam
        thPracParam.Reset();
    }
    static __declspec(noinline) void THInitHookDisable()
    {
        auto& s = THInitHook::singleton();
        s.th16_gui_init_1.Disable();
        s.th16_gui_init_2.Disable();
    }
    PATCH_DY(th16_disable_demo, 0x44afb0, "\xff\xff\xff\x7f", 4);
    EHOOK_DY(th16_disable_mutex, 0x4598db)
    {
        pCtx->Eip = 0x459aa1;
    }
    PATCH_DY(th16_startup_1, 0x44ac1f, "\x90\x90", 2);
    PATCH_DY(th16_startup_2, 0x44b672, "\xeb", 1);
    EHOOK_DY(th16_gui_init_1, 0x44bdc1)
    {
        THGuiCreate();
        THInitHookDisable();
    }
    EHOOK_DY(th16_gui_init_2, 0x45b788)
    {
        THGuiCreate();
        THInitHookDisable();
    }
    HOOKSET_ENDDEF()
}

void TH16Init()
{
    TH16::THInitHook::singleton().EnableAllHooks();
    TryKeepUpRefreshRate((void*)0x45b8da, (void*)0x45b6ad);
}
}
