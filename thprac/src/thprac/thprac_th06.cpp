#include "thprac_utils.h"
#include "../3rdParties/d3d8/include/d3d8.h"


namespace THPrac {

namespace TH06 {
    bool THBGMTest();
    using std::pair;
    struct THPracParam {
        int32_t mode;

        int32_t stage;
        int32_t section;
        int32_t phase;
        int32_t frame;

        int64_t score;
        float life;
        float bomb;
        float power;
        int32_t graze;
        int32_t point;

        int32_t rank;
        bool rankLock;
        int32_t fakeType;

        bool dlg;

        bool _playLock;
        void Reset()
        {
            mode = 0;
            stage = 0;
            section = 0;
            phase = 0;
            score = 0ll;
            life = 0.0f;
            bomb = 0.0f;
            power = 0.0f;
            graze = 0;
            point = 0;
            rank = 0;
            rankLock = false;
            fakeType = 0;
        }
        bool ReadJson(std::string& json)
        {
            ParseJson();

            ForceJsonValue(game, "th06");
            GetJsonValue(mode);
            GetJsonValue(stage);
            GetJsonValue(section);
            GetJsonValue(phase);
            GetJsonValueEx(dlg, Bool);
            GetJsonValue(frame);
            GetJsonValue(score);
            GetJsonValue(life);
            GetJsonValue(bomb);
            GetJsonValue(power);
            GetJsonValue(graze);
            GetJsonValue(point);
            GetJsonValue(rank);
            GetJsonValueEx(rankLock, Bool);
            GetJsonValue(fakeType);

            return true;
        }
        std::string GetJson()
        {
            CreateJson();

            AddJsonValueEx(version, GetVersionStr(), jalloc);
            AddJsonValueEx(game, "th06", jalloc);
            AddJsonValue(mode);
            AddJsonValue(stage);
            if (section)
                AddJsonValue(section);
            if (phase)
                AddJsonValue(phase);
            if (frame)
                AddJsonValue(frame);
            if (dlg)
                AddJsonValue(dlg);

            AddJsonValue(score);
            AddJsonValueEx(life, (int)life);
            AddJsonValueEx(bomb, (int)bomb);
            AddJsonValueEx(power, (int)power);
            AddJsonValue(graze);
            AddJsonValue(point);
            AddJsonValue(rank);
            AddJsonValue(rankLock);
            AddJsonValue(fakeType);

            ReturnJson();
        }
    };
    bool thRestartFlag = false;
    THPracParam thPracParam {};

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
    public:

    protected:
        virtual void OnLocaleChange() override
        {
            float x_offset_1 = 0.0f;
            float x_offset_2 = 0.0f;
            switch (Gui::LocaleGet()) {
            case Gui::LOCALE_ZH_CN:
                x_offset_1 = 0.12f;
                x_offset_2 = 0.172f;
                break;
            case Gui::LOCALE_EN_US:
                x_offset_1 = 0.12f;
                x_offset_2 = 0.16f;
                break;
            case Gui::LOCALE_JA_JP:
                x_offset_1 = 0.18f;
                x_offset_2 = 0.235f;
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
                    *((int32_t*)0x6c6eb0) = 3;
                }
            }
        }

        Gui::GuiHotKey mMenu { "ModMenuToggle", "BACKSPACE", VK_BACK };
        Gui::GuiHotKey mMuteki { TH_MUTEKI, "F1", VK_F1, {
            new HookCtx(0x4277c2, "\x03", 1),
            new HookCtx(0x42779a, "\x83\xc4\x10\x90\x90", 5) } };
        Gui::GuiHotKey mInfLives { TH_INFLIVES, "F2", VK_F2, {
            new HookCtx(0x428DDC, "\x15", 1) } };
        Gui::GuiHotKey mInfBombs { TH_INFBOMBS, "F3", VK_F3, {
            new HookCtx(0x4289e3, "\x00", 1) } };
        Gui::GuiHotKey mInfPower { TH_INFPOWER, "F4", VK_F4, {
            new HookCtx(0x41BBE2, "\x3E\xC6\x80", 3),
            new HookCtx(0x41BBE9, "\x80", 1),
            new HookCtx(0x428B7D, "\x00", 1) } };
        Gui::GuiHotKey mTimeLock { TH_TIMELOCK, "F5", VK_F5, {
            new HookCtx(0x412DD1, "\xeb", 1) } };
        Gui::GuiHotKey mAutoBomb { TH_AUTOBOMB, "F6", VK_F6, {
            new HookCtx(0x428989, "\xEB\x1D", 2),
            new HookCtx(0x4289B4, "\x85\xD2", 2),
            new HookCtx(0x428A94, "\xFF\x89", 2),
            new HookCtx(0x428A9D, "\x66\xC7\x05\x04\xD9\x69\x00\x02", 8) } };

    public:
        Gui::GuiHotKey mElBgm { TH_EL_BGM, "F7", VK_F7 };
    };
    class THGuiPrac : public Gui::GameGuiWnd {
        THGuiPrac() noexcept
        {
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
    public:

        __declspec(noinline) void State(int state)
        {
            switch (state) {
            case 0:
                break;
            case 1:
                SetFade(0.8f, 0.1f);
                Open();
                mDiffculty = (int)(*((int8_t*)0x69bcb0));
                mShotType = (int)(*((int8_t*)0x69d4bd) * 2) + *((int8_t*)0x69d4be);
                break;
            case 2:
                break;
            case 3:
                SetFade(0.8f, 0.8f);
                Close();
                *mNavFocus = 0;

                // Fill Param
                thPracParam.mode = *mMode;

                thPracParam.stage = *mStage;
                thPracParam.section = CalcSection();
                thPracParam.phase = *mPhase;
                thPracParam.frame = *mFrame;
                if (SectionHasDlg(thPracParam.section))
                    thPracParam.dlg = *mDlg;

                thPracParam.score = *mScore;
                thPracParam.life = (float)*mLife;
                thPracParam.bomb = (float)*mBomb;
                thPracParam.power = (float)*mPower;
                thPracParam.graze = *mGraze;
                thPracParam.point = *mPoint;

                thPracParam.rank = *mRank;
                thPracParam.rankLock = *mRankLock;
                if (thPracParam.section >= TH06_ST4_BOSS1 && thPracParam.section <= TH06_ST4_BOSS7)
                    thPracParam.fakeType = *mFakeShot;
                break;
            case 4:
                Close();
                *mNavFocus = 0;
                break;
            case 5:
                // Fill Param
                thPracParam.mode = *mMode;

                thPracParam.stage = *mStage;
                thPracParam.section = CalcSection();
                thPracParam.phase = *mPhase;
                thPracParam.frame = *mFrame;
                if (SectionHasDlg(thPracParam.section))
                    thPracParam.dlg = *mDlg;

                thPracParam.score = *mScore;
                thPracParam.life = (float)*mLife;
                thPracParam.bomb = (float)*mBomb;
                thPracParam.power = (float)*mPower;
                thPracParam.graze = *mGraze;
                thPracParam.point = *mPoint;

                thPracParam.rank = *mRank;
                thPracParam.rankLock = *mRankLock;
                if (thPracParam.section >= TH06_ST4_BOSS1 && thPracParam.section <= TH06_ST4_BOSS7)
                    thPracParam.fakeType = *mFakeShot;
                break;
            default:
                break;
            }
        }

        void SpellPhase()
        {
            auto section = CalcSection();
            if (section == TH06_ST7_END_S10) {
                mPhase(TH_PHASE, TH_SPELL_PHASE1);
            }
        }
        void PracticeMenu(Gui::GuiNavFocus& nav_focus)
        {
            mMode();
            if (mStage())
                *mSection = *mChapter = 0;
            if (*mMode == 1) {
                if (mWarp())
                    *mSection = *mChapter = *mPhase = *mFrame = 0;
                if (*mWarp) {
                    int st = 0;
                    if (*mStage == 3) {
                        mFakeShot();
                        st = (*mFakeShot ? *mFakeShot - 1 : mShotType) + 4;
                    }

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
                if (mRankLock()) {
                    if (*mRankLock)
                        mRank.SetBound(0, 99);
                    else
                        mRank.SetBound(0, 32);
                }
            }

            nav_focus();
        }

    protected:
        virtual void OnLocaleChange() override
        {
            SetTitle(S(TH_MENU));
            switch (Gui::LocaleGet()) {
            case Gui::LOCALE_ZH_CN:
                SetSize(330.f, 390.f);
                SetPos(260.f, 65.f);
                SetItemWidth(-60.0f);
                break;
            case Gui::LOCALE_EN_US:
                SetSize(370.f, 375.f);
                SetPos(240.f, 75.f);
                SetItemWidth(-60.0f);
                break;
            case Gui::LOCALE_JA_JP:
                SetSize(330.f, 390.f);
                SetPos(260.f, 65.f);
                SetItemWidth(-65.0f);
                break;
            default:
                break;
            }
        }
        virtual void OnContentUpdate() override
        {
            ImGui::TextUnformatted(S(TH_MENU));
            ImGui::Separator();

            PracticeMenu(mNavFocus);
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
        void SectionWidget()
        {
            static char chapterStr[256] {};
            auto& chapterCounts = mChapterSetup[*mStage];

            int st = 0;
            if (*mStage == 3) {
                st = (*mFakeShot ? *mFakeShot - 1 : mShotType) + 4;
            }

            switch (*mWarp) {
            case 1: // Chapter
                mChapter.SetBound(1, chapterCounts[0] + chapterCounts[1]);

                if (chapterCounts[1] == 0) {
                    sprintf_s(chapterStr, S(TH_STAGE_PORTION_N), *mChapter);
                } else if (*mChapter <= chapterCounts[0]) {
                    sprintf_s(chapterStr, S(TH_STAGE_PORTION_1), *mChapter);
                } else {
                    sprintf_s(chapterStr, S(TH_STAGE_PORTION_2), *mChapter - chapterCounts[0]);
                };

                mChapter(chapterStr);
                break;
            case 2:
            case 3: // Mid boss & End boss
                if (mSection(TH_WARP_SELECT_FRAME[*mWarp],
                    th_sections_cba[*mStage + st][*mWarp - 2],
                    th_sections_str[::THPrac::Gui::LocaleGet()][mDiffculty]))
                    *mPhase = 0;
                if (SectionHasDlg(th_sections_cba[*mStage][*mWarp - 2][*mSection]))
                    mDlg();
                break;
            case 4:
            case 5: // Non-spell & Spellcard
                if (mSection(TH_WARP_SELECT_FRAME[*mWarp],
                    th_sections_cbt[*mStage + st][*mWarp - 4],
                    th_sections_str[::THPrac::Gui::LocaleGet()][mDiffculty]))
                    *mPhase = 0;
                if (SectionHasDlg(th_sections_cbt[*mStage][*mWarp - 4][*mSection]))
                    mDlg();
                break;
            case 6:
                mFrame();
                break;
            }
        }


        // Data
        Gui::GuiCombo mMode { TH_MODE, TH_MODE_SELECT };
        Gui::GuiCombo mStage { TH_STAGE, TH_STAGE_SELECT };
        Gui::GuiCombo mWarp { TH_WARP, TH_WARP_SELECT_FRAME };
        Gui::GuiCombo mSection { TH_MODE };
        Gui::GuiCombo mPhase { TH_PHASE };
        Gui::GuiCheckBox mDlg { TH_DLG };

        Gui::GuiSlider<int, ImGuiDataType_S32> mChapter { TH_CHAPTER, 0, 0 };
        Gui::GuiDrag<int, ImGuiDataType_S32> mFrame { TH_FRAME, 0, INT_MAX };
        Gui::GuiSlider<int, ImGuiDataType_S32> mLife { TH_LIFE, 0, 8 };
        Gui::GuiSlider<int, ImGuiDataType_S32> mBomb { TH_BOMB, 0, 8 };
        Gui::GuiDrag<int64_t, ImGuiDataType_S64> mScore { TH_SCORE, 0, 9999999990, 10, 100000000 };
        Gui::GuiSlider<int, ImGuiDataType_S32> mPower { TH_POWER, 0, 128 };
        Gui::GuiDrag<int, ImGuiDataType_S32> mGraze { TH_GRAZE, 0, 99999, 1, 10000 };
        Gui::GuiDrag<int, ImGuiDataType_S32> mPoint { TH_POINT, 0, 9999, 1, 1000 };

        Gui::GuiSlider<int, ImGuiDataType_S32> mRank { TH06_RANK, 0, 32, 1, 10, 10 };
        Gui::GuiCheckBox mRankLock { TH06_RANKLOCK };
        Gui::GuiCombo mFakeShot { TH06_FS, TH06_TYPE_SELECT };

        Gui::GuiNavFocus mNavFocus { TH_STAGE, TH_MODE, TH_WARP, TH_FRAME,
            TH_MID_STAGE, TH_END_STAGE, TH_NONSPELL, TH_SPELL, TH_PHASE, TH_CHAPTER,
            TH_LIFE, TH_BOMB, TH_SCORE, TH_POWER, TH_GRAZE, TH_POINT,
            TH06_RANK, TH06_RANKLOCK, TH06_FS };

        int mChapterSetup[7][2] {
            { 4, 2 },
            { 2, 2 },
            { 4, 3 },
            { 4, 5 },
            { 3, 2 },
            { 2, 0 },
            { 4, 3 }
        };

        float mStep = 10.0;
        int mDiffculty = 0;
        int mShotType = 0;
    };
    class THPauseMenu : public Gui::GameGuiWnd {
        THPauseMenu() noexcept
        {
            SetTitle("Pause Menu");
            SetFade(0.8f, 0.1f);
            SetSize(384.f, 448.f);
            SetPos(32.f, 16.f);
            SetItemWidth(-60);
            SetStyle(ImGuiStyleVar_WindowRounding, 0.0f);
            SetStyle(ImGuiStyleVar_WindowBorderSize, 0.0f);
        }
        SINGLETON(THPauseMenu);
    public:

        bool el_bgm_signal { false };
        bool el_bgm_changed { false };

        enum state {
            STATE_CLOSE = 0,
            STATE_RESUME = 1,
            STATE_EXIT = 2,
            STATE_RESTART = 3,
            STATE_OPEN = 4,
        };
        enum signal {
            SIGNAL_NONE = 0,
            SIGNAL_RESUME = 1,
            SIGNAL_EXIT = 2,
            SIGNAL_RESTART = 3,
        };
        signal PMState()
        {
            switch (mState) {
            case THPrac::TH06::THPauseMenu::STATE_CLOSE:
                return StateClose();
            case THPrac::TH06::THPauseMenu::STATE_RESUME:
                return StateResume();
            case THPrac::TH06::THPauseMenu::STATE_EXIT:
                return StateExit();
            case THPrac::TH06::THPauseMenu::STATE_RESTART:
                return StateRestart();
            case THPrac::TH06::THPauseMenu::STATE_OPEN:
                return StateOpen();
            default:
                return SIGNAL_NONE;
            }
        }

    protected:
        signal StateRestart()
        {
            if (mState != STATE_RESTART) {
                mState = STATE_RESTART;
                mFrameCounter = 0;
            }

            if (mFrameCounter == 1) {
                *mNavFocus = 0;
                inSettings = false;

                auto oldMode = thPracParam.mode;
                auto oldStage = thPracParam.stage;
                auto oldBgmFlag = THBGMTest();
                thRestartFlag = true;
                THGuiPrac::singleton().State(5);
                if (*(THOverlay::singleton().mElBgm) && !el_bgm_changed && oldMode == thPracParam.mode && oldStage == thPracParam.stage && oldBgmFlag == THBGMTest()) {
                    el_bgm_signal = true;
                }

                Close();
            } else if (mFrameCounter == 10) {
                StateClose();
                return SIGNAL_RESTART;
            }

            return SIGNAL_NONE;
        }
        signal StateExit()
        {
            if (mState != STATE_EXIT) {
                mState = STATE_EXIT;
                mFrameCounter = 0;
            }

            if (mFrameCounter == 1) {
                *mNavFocus = 0;
                inSettings = false;
                Close();
            } else if (mFrameCounter == 10) {
                StateClose();
                return SIGNAL_EXIT;
            }

            return SIGNAL_NONE;
        }
        signal StateResume()
        {
            if (mState != STATE_RESUME) {
                mState = STATE_RESUME;
                mFrameCounter = 0;
            }

            if (mFrameCounter == 1) {
                *mNavFocus = 0;
                inSettings = false;
                Close();
            } else if (mFrameCounter == 10) {
                StateClose();
                return SIGNAL_RESUME;
            }

            return SIGNAL_NONE;
        }
        signal StateClose()
        {
            if (mState != STATE_CLOSE) {
                mState = STATE_CLOSE;
                mFrameCounter = 0;
            }

            if (mFrameCounter > 5) {
                return StateOpen();
            }

            return SIGNAL_NONE;
        }
        signal StateOpen()
        {
            if (mState != STATE_OPEN) {
                mState = STATE_OPEN;
                mFrameCounter = 0;
            }

            if (mFrameCounter == 1) {
                Open();
            }
            if (mFrameCounter > 10) {
                if (Gui::KeyboardInputGetSingle(VK_ESCAPE))
                    StateResume();
                else if (Gui::KeyboardInputGetRaw(0x51))
                    StateExit();
                else if (Gui::KeyboardInputGetRaw(0x52))
                    StateRestart();
            }

            return SIGNAL_NONE;
        }

        virtual void OnPreUpdate() override
        {
            if (mFrameCounter < UINT_MAX)
                mFrameCounter++;
        }
        virtual void OnLocaleChange() override
        {
            switch (Gui::LocaleGet()) {
            case Gui::LOCALE_ZH_CN:
                SetItemWidth(-60.0f);
                break;
            case Gui::LOCALE_EN_US:
                SetItemWidth(-60.0f);
                break;
            case Gui::LOCALE_JA_JP:
                SetItemWidth(-65.0f);
                break;
            default:
                break;
            }
        }
        virtual void OnContentUpdate() override
        {
            if (!inSettings) {
                ImGui::Dummy(ImVec2(10.0f, 140.0f));
                ImGui::Indent(119.0f);
                if (mResume())
                    StateResume();
                ImGui::Spacing();
                if (mExit())
                    StateExit();
                ImGui::Spacing();
                if (mRestart())
                    StateRestart();
                ImGui::Spacing();
                if (mSettings())
                    inSettings = !inSettings;
                ImGui::Spacing();
                ImGui::Unindent();
                mNavFocus();
            } else {
                ImGui::Dummy(ImVec2(10.0f, 10.0f));
                ImGui::Indent(119.0f);
                if (mResume())
                    StateResume();
                ImGui::Spacing();
                if (mExit())
                    StateExit();
                ImGui::Spacing();
                if (mRestart())
                    StateRestart();
                ImGui::Spacing();
                if (mSettings())
                    inSettings = !inSettings;
                ImGui::Spacing();
                ImGui::Unindent(67.0f);
                THGuiPrac::singleton().PracticeMenu(mNavFocus);
            }
        }

        // Var
        state mState = STATE_CLOSE;
        unsigned int mFrameCounter = 0;
        bool inSettings = false;

        Gui::GuiButton mResume { TH_RESUME, 130.0f, 25.0f };
        Gui::GuiButton mExit { TH_EXIT, 130.0f, 25.0f };
        Gui::GuiButton mRestart { TH_RESTART, 130.0f, 25.0f };
        Gui::GuiButton mSettings { TH_TWEAK, 130.0f, 25.0f };

        Gui::GuiNavFocus mNavFocus { TH_RESUME, TH_EXIT, TH_RESTART, TH_TWEAK,
            TH_STAGE, TH_MODE, TH_WARP,
            TH_MID_STAGE, TH_END_STAGE, TH_NONSPELL, TH_SPELL, TH_PHASE,
            TH_LIFE, TH_BOMB, TH_SCORE, TH_POWER, TH_GRAZE, TH_POINT,
            TH06_RANK, TH06_RANKLOCK, TH06_FS };
    };
    class THGuiRep : public Gui::GameGuiWnd {
        THGuiRep() noexcept
        {
        }
        SINGLETON(THGuiRep);
    public:

        void CheckReplay()
        {
            uint32_t index = GetMemContent(0x6d46c0 + 0x81e8);
            char* raw = (char*)(0x6d46c0 + index * 512 + 0x823c);

            std::string param;
            if (ReplayLoadParam(mb_to_utf16(raw, 932).c_str(), param) && mRepParam.ReadJson(param))
                mParamStatus = true;
            else
                mRepParam.Reset();
        }

        bool mRepStatus = false;
        void State(int state)
        {
            switch (state) {
            case 1:
                mRepStatus = false;
                mParamStatus = false;
                thPracParam.Reset();
                break;
            case 2:
                CheckReplay();
                break;
            case 3:
                mRepStatus = true;
                if (mParamStatus)
                    memcpy(&thPracParam, &mRepParam, sizeof(THPracParam));
                break;
            default:
                break;
            }
        }

    protected:
        bool mParamStatus = false;
        THPracParam mRepParam;
    };

    class THAdvOptWnd : public Gui::PPGuiWnd {
        // Option Related Functions
    private:
        void FpsInit()
        {
            mOptCtx.vpatch_base = (int32_t)GetModuleHandleW(L"vpatch_th06.dll");
            if (mOptCtx.vpatch_base) {
                uint64_t hash[2];
                CalcFileHash(L"vpatch_th06.dll", hash);
                if (hash[0] != 3665784961181135876ll || hash[1] != 9283021252209177490ll)
                    mOptCtx.fps_status = -1;
                else if (*(int32_t*)(mOptCtx.vpatch_base + 0x17024) == 0) {
                    mOptCtx.fps_status = 2;
                    mOptCtx.fps = *(int32_t*)(mOptCtx.vpatch_base + 0x17034);
                }
            } else {
                mOptCtx.vpatch_base = (int32_t)GetModuleHandleW(L"vpatch_th06_unicode.dll");
                if (mOptCtx.vpatch_base) {
                    uint64_t hash[2];
                    CalcFileHash(L"vpatch_th06_unicode.dll", hash);
                    if (hash[0] != 5021620919341617817ll || hash[1] != 10919509441391235291ll)
                        mOptCtx.fps_status = -1;
                    else if (*(int32_t*)(mOptCtx.vpatch_base + 0x17024) == 0) {
                        mOptCtx.fps_status = 2;
                        mOptCtx.fps = *(int32_t*)(mOptCtx.vpatch_base + 0x17034);
                    }
                } else
                    mOptCtx.fps_status = 0;
            }
        }
        void FpsSet()
        {
            if (mOptCtx.fps_status == 1) {
                mOptCtx.fps_dbl = 1.0 / (double)mOptCtx.fps;
            } else if (mOptCtx.fps_status == 2) {
                *(int32_t*)(mOptCtx.vpatch_base + 0x15a4c) = mOptCtx.fps;
                *(int32_t*)(mOptCtx.vpatch_base + 0x17034) = mOptCtx.fps;
            }
        }
        void GameplayInit()
        {
        }
        void GameplaySet()
        {
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
            SetTitle(S(TH_SPELL_PRAC));
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
            *((int32_t*)0x6c6eb0) = 3;
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

    // ECL Patch Helper
    void ECLWarp(int32_t time)
    {
        *((int32_t*)(0x5a5fb0)) = time;
    }
    void ECLSetHealth(ECLHelper& ecl, int offset, int32_t ecl_time, int32_t time)
    {
        ecl.SetPos(offset);
        ecl << ecl_time << 0x0010006f << 0x00ffff00 << time;
    }
    void ECLSetTime(ECLHelper& ecl, int offset, int32_t ecl_time, int32_t health)
    {
        ecl.SetPos(offset);
        ecl << ecl_time << 0x00100073 << 0x00ffff00 << health;
    }
    void ECLStall(ECLHelper& ecl, int offset)
    {
        ecl.SetPos(offset);
        ecl << 0x99999 << 0x000c0000 << 0x0000ff00;
    }
    void ECLNameFix()
    {
        void* thisPtr = *((void**)0x6d4588);
        if (thPracParam.stage == 5) {
            asm_call<0x431dc0, Thiscall>(thisPtr, 11, "data/eff06.anm", 691);
        } else if (thPracParam.stage == 6) {
            asm_call<0x431dc0, Thiscall>(thisPtr, 11, "data/eff07.anm", 691);
            asm_call<0x431dc0, Thiscall>(thisPtr, 18, "data/face12c.anm", 1192);
        }
    }
    __declspec(noinline) void THPatch(ECLHelper& ecl, th_sections_t section)
    {
        int shot;
        auto s2b_nd = [&]() {
            ECLWarp(0x1760);
            ecl << pair{0x18fc, 0x0};
            ecl << pair{0x191c, 0x0};
            ecl << pair{0x192c, 0x0};
            ecl << pair{0x194c, 0x0};
            ecl << pair{0x196c, 0x0};
            ecl << pair{0x198c, 0x0};
            ecl << pair{0x19a0, 0x0};
        };
        auto s3b_n1 = [&]() {
            ecl << pair{0x1274, (int16_t)0x0};
            ecl << pair{0x12f0, (int16_t)0x0};
            ECLWarp(0x16d4);
            ecl << pair{0x80d6, (int16_t)0x16d4};
            ecl << pair{0x1f70, 0x1};
            ecl << pair{0x1f90, 0x0};
            ecl << pair{0x80dc, (int16_t)0x24};
            ecl << pair{0x20ec, 0x0};
            ecl << pair{0x210c, 0x0};
            ecl << pair{0x212c, 0x0};
            ecl << pair{0x214c, 0x1e};
            ecl << pair{0x2160, 0x1e};
            ecl << pair{0x2194, 0x1e};
            ecl << pair{0x2188, 150};
        };
        auto s4b_time = [&]() {
            ecl << pair{0x2790, 0x0};
            ecl << pair{0x27b0, 0x0};
            ecl << pair{0x27d0, 0x0};
            ecl << pair{0x27f0, 0x0};
            ecl << pair{0x2810, 0x0};
            ecl << pair{0x2824, 0x0};
            ecl << pair{0x283c, 0x0};
            ecl << pair{0x2850, 0x0};
        };
        auto s7b_call = [&]() {
            ecl << pair{0x344e, 0x0};
            ecl << pair{0x3452, 0x00180023};
            ecl << pair{0x3456, 0x00ffff00};
            ecl << pair{0x345a, 0x0};
            ecl << pair{0x345e, 0x0};
            ecl << pair{0x3462, 0x0};
            ECLStall(ecl, 0x3466);
        };
        auto s7b_n1 = [&]() {
            ECLWarp(0x2192);
            ecl << pair{0x339e, 0x0};
            ecl << pair{0x33ae, 0x0};
            ecl << pair{0x33ce, 0x0};
            ecl << pair{0x33ee, 0x0};
            ecl << pair{0x340e, 0x0};
            ecl << pair{0x342e, 0x0};
        };

        switch (section) {
        case THPrac::TH06::TH06_ST1_MID1:
            ECLWarp(0x7d8);
            ecl << pair{0x0ab0, 0x3c} << pair{0x0ad0, 0x3c};
            break;
        case THPrac::TH06::TH06_ST1_MID2:
            ECLWarp(0x7d8);
            ecl << pair{0x0ab0, 0x3c} << pair{0x0ad0, 0x3c};
            ECLSetHealth(ecl, 0x0af0, 0x3c, 0x1f3);
            break;
        case THPrac::TH06::TH06_ST1_BOSS1:
            if (thPracParam.dlg)
                ECLWarp(0x149e);
            else {
                ECLWarp(0x149f);
                ecl << pair{0x16a6, 0} << pair{0x16c6, 0} << pair{0x16e6, 0x50};
            }
            break;
        case THPrac::TH06::TH06_ST1_BOSS2:
            ECLWarp(0x149f);
            ecl << pair{0x16a6, 0} << pair{0x16c6, 0} << pair{0x16e6, 0x50};
            ECLSetTime(ecl, 0x16e6, 0, 0);
            ECLStall(ecl, 0x16f6);
            break;
        case THPrac::TH06::TH06_ST1_BOSS3:
            ECLWarp(0x149f);
            ecl << pair{0x16a6, 0} << pair{0x16c6, 0} << pair{0x16e6, 0x50}
                << pair{0x16f2, 0x10} << pair{0x293a, 0} << pair{0x294a, 0}
                << pair{0x291e, (int16_t)0};
            break;
        case THPrac::TH06::TH06_ST1_BOSS4:
            ECLWarp(0x149f);
            ecl << pair{0x16a6, 0} << pair{0x16c6, 0} << pair{0x16e6, 0x50}
                << pair{0x16f2, 0x10} << pair{0x293a, 0} << pair{0x294a, 0}
                << pair{0x291e, (int16_t)0};
            ECLSetTime(ecl, 0x294a, 0, 0);
            ECLStall(ecl, 0x295a);
            break;
        case THPrac::TH06::TH06_ST2_MID1:
            ECLWarp(0xa1c);
            break;
        case THPrac::TH06::TH06_ST2_BOSS1:
            if (thPracParam.dlg)
                ECLWarp(0x175f);
            else
                ECLWarp(0x1760);
            break;
        case THPrac::TH06::TH06_ST2_BOSS2:
            s2b_nd();
            ECLSetTime(ecl, 0x19a0, 0x0, 0x0);
            ECLStall(ecl, 0x19b0);
            break;
        case THPrac::TH06::TH06_ST2_BOSS3:
            s2b_nd();
            ecl << pair{0x19ac, 0x19};
            ecl << pair{0x2138, 0x0};
            ecl << pair{0x2148, 0x60};
            ecl << pair{0x2110, (int16_t)0x0};
            break;
        case THPrac::TH06::TH06_ST2_BOSS4:
            s2b_nd();
            ecl << pair{0x19ac, 0x19};
            ecl << pair{0x2138, 0x0};
            ecl << pair{0x2148, 0x60};
            ecl << pair{0x2110, (int16_t)0x0};
            ECLSetTime(ecl, 0x2148, 0x30, 0x0);
            ECLStall(ecl, 0x2158);
            break;
        case THPrac::TH06::TH06_ST2_BOSS5:
            s2b_nd();
            ecl << pair{0x19ac, 0x19};
            ecl << pair{0x2138, 0x0};
            ecl << pair{0x2148, 0x60};
            ecl << pair{0x2110, (int16_t)0x0};
            ecl << pair{0x2148, 0x0};
            ecl << pair{0x2154, 0x20};
            ecl << pair{0x33a2, (int16_t)0x0};
            ecl << pair{0x337a, (int16_t)0x0};
            ecl << pair{0x3392, (int16_t)0x0};
            ecl << pair{0x2090, 0x578};
            ecl << pair{0x20b0, 0xffffffff};
            ecl << pair{0x20c0, 0xffffffff};
            ecl << pair{0x20f0, 0x1c};
            break;
        case THPrac::TH06::TH06_ST3_MID1:
            ECLWarp(0x0edc);
            break;
        case THPrac::TH06::TH06_ST3_MID2:
            ECLWarp(0x0edc);
            ecl << pair{0x1274, (int16_t)0x0};
            ecl << pair{0x12f0, (int16_t)0x0};
            ecl << pair{0x1018, 0x0};
            ECLSetHealth(ecl, 0x10dc, 0x1e, 0x513);
            ECLStall(ecl, 0x10ec);
            break;
        case THPrac::TH06::TH06_ST3_BOSS1:
            if (thPracParam.dlg)
                ECLWarp(0x16d4);
            else
                s3b_n1();
            break;
        case THPrac::TH06::TH06_ST3_BOSS2:
            s3b_n1();
            ecl << pair{0x214c, 0x0};
            ECLSetTime(ecl, 0x2160, 0x0, 0x0);
            ECLStall(ecl, 0x2170);
            break;
        case THPrac::TH06::TH06_ST3_BOSS3:
            s3b_n1();
            ecl << pair{0x214c, 0x0};
            ecl << pair{0x2160, 0x0};
            ecl << pair{0x216c, 0x14};
            ecl << pair{0x25f4, (int16_t)0x0};
            break;
        case THPrac::TH06::TH06_ST3_BOSS4:
            s3b_n1();
            ecl << pair{0x214c, 0x0};
            ecl << pair{0x2160, 0x0};
            ecl << pair{0x216c, 0x14};
            ecl << pair{0x25f4, (int16_t)0x0};
            ECLSetTime(ecl, 0x267c, 0x0, 0x0);
            ECLStall(ecl, 0x268c);
            break;
        case THPrac::TH06::TH06_ST3_BOSS5:
            s3b_n1();
            ecl << pair{0x214c, 0x0};
            ecl << pair{0x2160, 0x0};
            ecl << pair{0x216c, 0x1a};
            ecl << pair{0x31d0, (int16_t)0x0};
            break;
        case THPrac::TH06::TH06_ST3_BOSS6:
            s3b_n1();
            ecl << pair{0x214c, 0x0};
            ecl << pair{0x2160, 0x0};
            ecl << pair{0x216c, 0x1a};
            ecl << pair{0x31d0, (int16_t)0x0};
            ECLSetTime(ecl, 0x3254, 0x0, 0x0);
            ECLStall(ecl, 0x3264);
            break;
        case THPrac::TH06::TH06_ST3_BOSS7:
            s3b_n1();
            ecl << pair{0x214c, 0x0};
            ecl << pair{0x2160, 0x0};
            ecl << pair{0x216c, 0x1a};
            ecl << pair{0x31d0, (int16_t)0x0};
            ECLSetTime(ecl, 0x3254, 0x0, 0x0);
            ECLStall(ecl, 0x3264);
            ecl << pair{0x3168, 0x7d0};
            ecl << pair{0x31a8, 0x21};
            ecl << pair{0x31b8, 0x21};
            ecl << pair{0x4b64, (int16_t)0x0};
            ecl << pair{0x4bec, (int16_t)0x0};
            break;
        case THPrac::TH06::TH06_ST4_BOOKS:
            ECLWarp(0x0d40);
            break;
        case THPrac::TH06::TH06_ST4_MID1:
            ECLWarp(0x1024);
            break;
        case THPrac::TH06::TH06_ST4_BOSS1:
            if (thPracParam.dlg)
                ECLWarp(0x29c6);
            else
                ECLWarp(0x29c7);
            break;
        case THPrac::TH06::TH06_ST4_BOSS2:
            ECLWarp(0x29c7);
            s4b_time();
            ECLSetTime(ecl, 0x2810, 0x0, 0x0);
            ECLStall(ecl, 0x2820);
            break;
        case THPrac::TH06::TH06_ST4_BOSS3:
            ECLWarp(0x29c7);
            s4b_time();
            ecl << pair{0x2854, (int16_t)0x23};
            ecl << pair{0x285c, 0x25};
            ecl << pair{0x6da0, (int16_t)0x0};
            ECLSetTime(ecl, 0x7440, 0x0, 0x0);
            ECLStall(ecl, 0x7450);
            break;
        case THPrac::TH06::TH06_ST4_BOSS4:
            ECLWarp(0x29c7);
            s4b_time();
            ecl << pair{0x2854, (int16_t)0x23};
            ecl << pair{0x285c, 0x25};
            ecl << pair{0x6da0, (int16_t)0x0};
            break;
        case THPrac::TH06::TH06_ST4_BOSS5:
            ECLWarp(0x29c7);
            s4b_time();
            ecl << pair{0x2854, (int16_t)0x23};
            ecl << pair{0x285c, 0x27};
            ecl << pair{0x7950, (int16_t)0x0};
            ecl << pair{0x7afc, 0x0};
            ecl << pair{0x7b0c, 0x0};
            ecl << pair{0x7b2c, 0x0};
            ecl << pair{0x7b4c, 0x0};
            ecl << pair{0x7b6c, 0x0};
            ecl << pair{0x7b8c, 0x0};
            break;
        case THPrac::TH06::TH06_ST4_BOSS6:
            ECLWarp(0x29c7);
            s4b_time();
            ecl << pair{0x2854, (int16_t)0x23};
            ecl << pair{0x285c, 0x27};
            ecl << pair{0x7950, (int16_t)0x0};

            ECLSetHealth(ecl, 0x7afc, 0, 1699);
            ECLSetHealth(ecl, 0x7b0c, 0, 3399);
            ECLStall(ecl, 0x7b1c);
            ecl << pair{0x7b04, (int16_t)0x0200} << pair{0x7b14, (int16_t)0x0c00};
            ecl << pair{0x7bc8, (int16_t)0}
                << pair{0x7cf4, (int16_t)0} << pair{0x7d0c, (int16_t)0};
            ecl << pair{0x7d18, 0x0} << pair{0x7d28, 0x0}
                << pair{0x7d48, 0x0} << pair{0x7d68, 0x0}
                << pair{0x7d88, 0x0} << pair{0x7da8, 0x0};
            break;
        case THPrac::TH06::TH06_ST4_BOSS7:
            ECLWarp(0x29c7);
            s4b_time();
            ecl << pair{0x2854, (int16_t)0x23};
            ecl << pair{0x285c, 0x27};
            ecl << pair{0x7950, (int16_t)0x0};

            ECLSetHealth(ecl, 0x7afc, 0, 1699);
            ECLStall(ecl, 0x7b0c);
            ecl << pair{0x7a74, (int16_t)41} << pair{0x7a94, (int16_t)41};
            ecl << pair{0x7a54, (int16_t)1700} << pair{0x7a64, (int16_t)1700};
            ecl << pair{0x7de4, (int16_t)0}
                << pair{0x7ed0, (int16_t)0} << pair{0x7ee8, (int16_t)0};
            ecl << pair{0x7ef4, 0x0} << pair{0x7f04, 0x0}
                << pair{0x7f24, 0x0} << pair{0x7f44, 0x0}
                << pair{0x7f64, 0x0} << pair{0x7f84, 0x0};
            break;
        case THPrac::TH06::TH06_ST5_MID1:
            ECLWarp(0x0d2c);
            if (!thPracParam.dlg)
                ecl << pair{0x64a8, (uint16_t)13};
            break;
        case THPrac::TH06::TH06_ST5_MID2:
            ECLWarp(0x0d2c);
            ecl << pair{0x64a4, (int16_t)0x0};
            ECLSetHealth(ecl, 0x14d8, 0x1e, 0x2c5);
            ECLStall(ecl, 0x14e8);
            break;
        case THPrac::TH06::TH06_ST5_BOSS1:
            ECLWarp(0x1e18);
            if (!thPracParam.dlg) {
                ecl << pair{0x767c, (int16_t)0x0};
                ecl << pair{0x22c8, 0x0};
                ecl << pair{0x22e8, 0x0};
                ecl << pair{0x2308, 0x0};
                ecl << pair{0x2328, 0x0};
                ecl << pair{0x2348, 0x0};
                ecl << pair{0x2218, (int16_t)0x0};
            }
            break;
        case THPrac::TH06::TH06_ST5_BOSS2:
            ECLWarp(0x1e18);
            ecl << pair{0x767c, (int16_t)0x0};
            ecl << pair{0x22c8, 0x0};
            ecl << pair{0x22e8, 0x0};
            ecl << pair{0x2308, 0x0};
            ecl << pair{0x2328, 0x0};
            ecl << pair{0x2348, 0x0};
            ecl << pair{0x2218, (int16_t)0x0};
            ECLSetTime(ecl, 0x2348, 0x0, 0x0);
            ECLStall(ecl, 0x2358);
            break;
        case THPrac::TH06::TH06_ST5_BOSS3:
            ECLWarp(0x1e18);
            ecl << pair{0x767c, (int16_t)0x0};
            ecl << pair{0x22c8, 0x0};
            ecl << pair{0x22e8, 0x0};
            ecl << pair{0x2308, 0x0};
            ecl << pair{0x2328, 0x0};
            ecl << pair{0x2348, 0x0};
            ecl << pair{0x2218, (int16_t)0x0};
            ecl << pair{0x235c, 0x0};
            ecl << pair{0x2368, 0x22};
            ecl << pair{0x3778, (int16_t)0x0};
            ecl << pair{0x3828, (int16_t)0x0};
            break;
        case THPrac::TH06::TH06_ST5_BOSS4:
            ECLWarp(0x1e18);
            ecl << pair{0x767c, (int16_t)0x0};
            ecl << pair{0x22c8, 0x0};
            ecl << pair{0x22e8, 0x0};
            ecl << pair{0x2308, 0x0};
            ecl << pair{0x2328, 0x0};
            ecl << pair{0x2348, 0x0};
            ecl << pair{0x2218, (int16_t)0x0};
            ecl << pair{0x235c, 0x0};
            ecl << pair{0x2368, 0x22};
            ecl << pair{0x3778, (int16_t)0x0};
            ecl << pair{0x3828, (int16_t)0x0};
            ECLSetTime(ecl, 0x38b8, 0x0, 0x0);
            ECLStall(ecl, 0x38c8);
            break;
        case THPrac::TH06::TH06_ST5_BOSS5:
            ECLWarp(0x1e18);
            ecl << pair{0x767c, (int16_t)0x0};
            ecl << pair{0x22c8, 0x0};
            ecl << pair{0x22e8, 0x0};
            ecl << pair{0x2308, 0x0};
            ecl << pair{0x2328, 0x0};
            ecl << pair{0x2348, 0x0};
            ecl << pair{0x2218, (int16_t)0x0};
            ecl << pair{0x235c, 0x1e};
            ecl << pair{0x2368, 0x29};
            ecl << pair{0x4638, (int16_t)0x0};
            ecl << pair{0x46e8, (int16_t)0x0};
            break;
        case THPrac::TH06::TH06_ST5_BOSS6:
            ECLWarp(0x1e18);
            ecl << pair{0x767c, (int16_t)0x0};
            ecl << pair{0x22c8, 0x0};
            ecl << pair{0x22e8, 0x0};
            ecl << pair{0x2308, 0x0};
            ecl << pair{0x2328, 0x0};
            ecl << pair{0x2348, 0x0};
            ecl << pair{0x2218, (int16_t)0x0};
            ecl << pair{0x235c, 0x1e};
            ecl << pair{0x2368, 0x29};
            ecl << pair{0x4638, (int16_t)0x0};
            ecl << pair{0x46e8, (int16_t)0x0};
            ecl << pair{0x235c, 0x0};
            ECLSetTime(ecl, 0x4758, 0x0, 0x0);
            ECLStall(ecl, 0x4768);
            break;
        case THPrac::TH06::TH06_ST6_MID1:
            ECLWarp(0x0a04);
            if (!thPracParam.dlg) {
                ecl << pair{0x77f2, (int16_t)0x0};
                ecl << pair{0x9e8, 0x1};
            }
            break;
        case THPrac::TH06::TH06_ST6_MID2:
            shot = (int)(*((int8_t*)0x69d4bd) * 2) + *((int8_t*)0x69d4be);
            if (shot > 1)
                shot = 1099;
            else if (!shot)
                shot = 749;
            else
                shot = 999;
            ECLWarp(0x0a04);
            ecl << pair{0x77f2, (int16_t)0x0};
            ecl << pair{0x0d2c, 0x0};
            ECLSetHealth(ecl, 0x0d3c, 0x0, shot);
            ECLStall(ecl, 0x0d4c);
            break;
        case THPrac::TH06::TH06_ST6_BOSS1:
            if (thPracParam.dlg)
                ECLWarp(0x0c5f);
            else {
                ECLWarp(0x0c61);
                ECLNameFix();
                ecl << pair{0x1686, 0x0};
                ecl << pair{0x16a6, 0x0};
                ecl << pair{0x16c6, 0x0};
                ecl << pair{0x16e6, 0x0};
                ecl << pair{0x15d6, (int16_t)0x0};
            }
            break;
        case THPrac::TH06::TH06_ST6_BOSS2:
            ECLNameFix();
            ECLWarp(0x0c61);
            ecl << pair{0x1686, 0x0};
            ecl << pair{0x16a6, 0x0};
            ecl << pair{0x16c6, 0x0};
            ecl << pair{0x16e6, 0x0};
            ecl << pair{0x15d6, (int16_t)0x0};
            ECLSetTime(ecl, 0x1706, 0x0, 0x0);
            ECLStall(ecl, 0x1716);
            break;
        case THPrac::TH06::TH06_ST6_BOSS3:
            ECLNameFix();
            ECLWarp(0x0c61);
            ecl << pair{0x1686, 0x0};
            ecl << pair{0x16a6, 0x0};
            ecl << pair{0x16c6, 0x0};
            ecl << pair{0x16e6, 0x0};
            ecl << pair{0x15d6, (int16_t)0x0};
            ecl << pair{0x1706, 0x0};
            ecl << pair{0x171a, 0x0};
            ecl << pair{0x1726, 0x13};
            ecl << pair{0x1b8e, (int16_t)0x0};
            ecl << pair{0x1c3e, (int16_t)0x0};
            break;
        case THPrac::TH06::TH06_ST6_BOSS4:
            ECLNameFix();
            ECLWarp(0x0c61);
            ecl << pair{0x1686, 0x0};
            ecl << pair{0x16a6, 0x0};
            ecl << pair{0x16c6, 0x0};
            ecl << pair{0x16e6, 0x0};
            ecl << pair{0x15d6, (int16_t)0x0};
            ecl << pair{0x1706, 0x0};
            ecl << pair{0x171a, 0x0};
            ecl << pair{0x1726, 0x13};
            ecl << pair{0x1b8e, (int16_t)0x0};
            ecl << pair{0x1c3e, (int16_t)0x0};
            ECLSetTime(ecl, 0x1cf2, 0x0, 0x0);
            ECLStall(ecl, 0x1d02);
            break;
        case THPrac::TH06::TH06_ST6_BOSS5:
            ECLNameFix();
            ECLWarp(0x0c61);
            ecl << pair{0x1686, 0x0};
            ecl << pair{0x16a6, 0x0};
            ecl << pair{0x16c6, 0x0};
            ecl << pair{0x16e6, 0x0};
            ecl << pair{0x15d6, (int16_t)0x0};
            ecl << pair{0x1706, 0x0};
            ecl << pair{0x171a, 0x1e};
            ecl << pair{0x1726, 0x17};
            ecl << pair{0x28e2, (int16_t)0x0};
            ecl << pair{0x2992, (int16_t)0x0};
            break;
        case THPrac::TH06::TH06_ST6_BOSS6:
            ECLNameFix();
            ECLWarp(0x0c61);
            ecl << pair{0x1686, 0x0};
            ecl << pair{0x16a6, 0x0};
            ecl << pair{0x16c6, 0x0};
            ecl << pair{0x16e6, 0x0};
            ecl << pair{0x15d6, (int16_t)0x0};
            ecl << pair{0x1706, 0x0};
            ecl << pair{0x171a, 0x1e};
            ecl << pair{0x1726, 0x17};
            ecl << pair{0x28e2, (int16_t)0x0};
            ecl << pair{0x2992, (int16_t)0x0};
            ecl << pair{0x171a, 0x0};
            ECLSetTime(ecl, 0x2a22, 0x0, 0x0);
            ECLStall(ecl, 0x2a32);
            break;
        case THPrac::TH06::TH06_ST6_BOSS7:
            ECLNameFix();
            ECLWarp(0x0c61);
            ecl << pair{0x1686, 0x0};
            ecl << pair{0x16a6, 0x0};
            ecl << pair{0x16c6, 0x0};
            ecl << pair{0x16e6, 0x0};
            ecl << pair{0x15d6, (int16_t)0x0};
            ecl << pair{0x1706, 0x0};
            ecl << pair{0x171a, 0x0};
            ecl << pair{0x1726, 0x1a};
            ecl << pair{0x2d8e, (int16_t)0x0};
            ecl << pair{0x2e3e, (int16_t)0x0};
            break;
        case THPrac::TH06::TH06_ST6_BOSS8:
            ECLNameFix();
            ECLWarp(0x0c61);
            ecl << pair{0x1686, 0x0};
            ecl << pair{0x16a6, 0x0};
            ecl << pair{0x16c6, 0x0};
            ecl << pair{0x16e6, 0x0};
            ecl << pair{0x15d6, (int16_t)0x0};
            ecl << pair{0x1706, 0x0};
            ecl << pair{0x171a, 0x0};
            ecl << pair{0x1726, 0x1a};
            ecl << pair{0x2d8e, (int16_t)0x0};
            ecl << pair{0x2e3e, (int16_t)0x0};
            ECLSetTime(ecl, 0x2ee6, 0x0, 0x0);
            ECLStall(ecl, 0x2ef6);
            break;
        case THPrac::TH06::TH06_ST6_BOSS9:
            ECLNameFix();
            ECLWarp(0x0c61);
            ecl << pair{0x1686, 0x0};
            ecl << pair{0x16a6, 0x0};
            ecl << pair{0x16c6, 0x0};
            ecl << pair{0x16e6, 0x0};
            ecl << pair{0x15d6, (int16_t)0x0};
            ecl << pair{0x1706, 0x0};
            ecl << pair{0x171a, 0x0};
            ecl << pair{0x1732, 0x0};
            ecl << pair{0x1726, 0x2b};
            ecl << pair{0x1722, (int16_t)0x0300};
            ecl << pair{0x1736, (int16_t)0x23};
            ecl << pair{0x173a, (int16_t)0x0c00};
            ecl << pair{0x173e, 0x2c};
            ecl << pair{0x5c8e, (int16_t)0x0};
            ecl << pair{0x6290, (int16_t)0x0};
            ecl << pair{0x1622, 0xffffffff};
            break;
        case THPrac::TH06::TH06_ST7_MID1:
            ECLWarp(0x1284);
            if (!thPracParam.dlg)
                ecl << pair{0x0d2e2, (int16_t)0x0};
            break;
        case THPrac::TH06::TH06_ST7_MID2:
            ECLWarp(0x1284);
            ecl << pair{0x0d2e2, (int16_t)0x0};
            ecl << pair{0x1b14, 0x12};
            ecl << pair{0x1c2c, (int16_t)0x0};
            break;
        case THPrac::TH06::TH06_ST7_MID3:
            ECLWarp(0x1284);
            ecl << pair{0x0d2e2, (int16_t)0x0};
            ecl << pair{0x1b14, 0x13};
            ecl << pair{0x1d7c, (int16_t)0x0};
            break;
        case THPrac::TH06::TH06_ST7_END_NS1:
            if (thPracParam.dlg)
                ECLWarp(0x2191);
            else {
                ECLNameFix();
                s7b_n1();
            }
            break;
        case THPrac::TH06::TH06_ST7_END_S1:
            ECLNameFix();
            s7b_n1();
            ECLSetTime(ecl, 0x344e, 0x0, 0x0);
            break;
        case THPrac::TH06::TH06_ST7_END_NS2:
            ECLNameFix();
            s7b_n1();
            s7b_call();
            ecl << pair{0x345a, 0x23};
            ecl << pair{0x4210, (int16_t)0x0};
            break;
        case THPrac::TH06::TH06_ST7_END_S2:
            ECLNameFix();
            s7b_n1();
            s7b_call();
            ecl << pair{0x345a, 0x23};
            ecl << pair{0x4210, (int16_t)0x0};
            ecl << pair{0x41fc, 0x0};
            ecl << pair{0x420c, 0x0};
            ECLSetTime(ecl, 0x421c, 0x0, 0x0);
            ECLStall(ecl, 0x422c);
            break;
        case THPrac::TH06::TH06_ST7_END_NS3:
            ECLNameFix();
            s7b_n1();
            s7b_call();
            ecl << pair{0x345a, 0x26};
            ecl << pair{0x4c62, (int16_t)0x0};
            break;
        case THPrac::TH06::TH06_ST7_END_S3:
            ECLNameFix();
            s7b_n1();
            s7b_call();
            ecl << pair{0x345a, 0x26};
            ecl << pair{0x4c62, (int16_t)0x0};
            ecl << pair{0x4c4e, 0x0};
            ecl << pair{0x4c5e, 0x0};
            ECLSetTime(ecl, 0x4c6e, 0x0, 0x0);
            ECLStall(ecl, 0x4c7e);
            break;
        case THPrac::TH06::TH06_ST7_END_NS4:
            ECLNameFix();
            s7b_n1();
            s7b_call();
            ecl << pair{0x345a, 0x2b};
            ecl << pair{0x59cc, (int16_t)0x0};
            break;
        case THPrac::TH06::TH06_ST7_END_S4:
            ECLNameFix();
            s7b_n1();
            s7b_call();
            ecl << pair{0x345a, 0x2b};
            ecl << pair{0x59cc, (int16_t)0x0};
            ecl << pair{0x59b8, 0x0};
            ecl << pair{0x59c8, 0x0};
            ECLSetTime(ecl, 0x59d8, 0x0, 0x0);
            ECLStall(ecl, 0x59e8);
            break;
        case THPrac::TH06::TH06_ST7_END_NS5:
            ECLNameFix();
            s7b_n1();
            s7b_call();
            ecl << pair{0x345a, 0x2f};
            ecl << pair{0x63a2, (int16_t)0x0};
            break;
        case THPrac::TH06::TH06_ST7_END_S5:
            ECLNameFix();
            s7b_n1();
            s7b_call();
            ecl << pair{0x345a, 0x2f};
            ecl << pair{0x63a2, (int16_t)0x0};
            ecl << pair{0x638e, 0x0};
            ecl << pair{0x639e, 0x0};
            ECLSetTime(ecl, 0x63ae, 0x0, 0x0);
            ECLStall(ecl, 0x63be);
            break;
        case THPrac::TH06::TH06_ST7_END_NS6:
            ECLNameFix();
            s7b_n1();
            s7b_call();
            ecl << pair{0x345a, 0x31};
            ecl << pair{0x6b1c, (int16_t)0x0};
            break;
        case THPrac::TH06::TH06_ST7_END_S6:
            ECLNameFix();
            s7b_n1();
            s7b_call();
            ecl << pair{0x345a, 0x31};
            ecl << pair{0x6b1c, (int16_t)0x0};
            ecl << pair{0x6b08, 0x0};
            ecl << pair{0x6b18, 0x0};
            ECLSetTime(ecl, 0x6b28, 0x0, 0x0);
            ECLStall(ecl, 0x6b38);
            break;
        case THPrac::TH06::TH06_ST7_END_NS7:
            ECLNameFix();
            s7b_n1();
            s7b_call();
            ecl << pair{0x345a, 0x35};
            ecl << pair{0x78aa, (int16_t)0x0};
            break;
        case THPrac::TH06::TH06_ST7_END_S7:
            ECLNameFix();
            s7b_n1();
            s7b_call();
            ecl << pair{0x345a, 0x35};
            ecl << pair{0x78aa, (int16_t)0x0};
            ecl << pair{0x7896, 0x0};
            ecl << pair{0x78a6, 0x0};
            ECLSetTime(ecl, 0x78b6, 0x0, 0x0);
            ECLStall(ecl, 0x78c6);
            break;
        case THPrac::TH06::TH06_ST7_END_NS8:
            ECLNameFix();
            s7b_n1();
            s7b_call();
            ecl << pair{0x345a, 0x38};
            ecl << pair{0x8508, (int16_t)0x0};
            break;
        case THPrac::TH06::TH06_ST7_END_S8:
            ECLNameFix();
            s7b_n1();
            s7b_call();
            ecl << pair{0x345a, 0x38};
            ecl << pair{0x8508, (int16_t)0x0};
            ecl << pair{0x84f4, 0x0};
            ecl << pair{0x8504, 0x0};
            ECLSetTime(ecl, 0x8514, 0x0, 0x0);
            ECLStall(ecl, 0x8524);
            break;
        case THPrac::TH06::TH06_ST7_END_S9:
            ECLNameFix();
            s7b_n1();
            s7b_call();
            ecl << pair{0x345a, 0x3b};
            ecl << pair{0x940a, (int16_t)0x0};
            ecl << pair{0x93f6, 0x0};
            ecl << pair{0x9406, 0x0};
            ecl << pair{0x9416, 0x0};
            ecl << pair{0x9422, 0x0};
            ecl << pair{0x943a, 0x0};
            ecl << pair{0x9466, 0x0};
            ecl << pair{0x9472, 0x0};
            ecl << pair{0x9482, 0x0};
            break;
        case THPrac::TH06::TH06_ST7_END_S10:
            ECLNameFix();
            s7b_n1();
            s7b_call();
            ecl << pair{0x345a, 0x43};
            ecl << pair{0x0bea4, (int16_t)0x0};
            ecl << pair{0x0be90, 0x0};
            ecl << pair{0x0bea0, 0x0};
            ecl << pair{0x0beb0, 0x0};
            ecl << pair{0x0bed0, 0x0};
            ecl << pair{0x0bef0, 0x0};
            ecl << pair{0x0bf10, 0x0};
            ecl << pair{0x0bf30, 0x0};
            ecl << pair{0x0bf50, 0x0};
            ecl << pair{0x0bf5c, 0x0};
            ecl << pair{0x0bf74, 0x0};
            ecl << pair{0x0bfa0, 0x0};
            ecl << pair{0x0bfac, 0x0};
            ecl << pair{0x0bfbc, 0x0};
            break;
        default:
            break;
        }
    }
    __declspec(noinline) void THStageWarp(ECLHelper& ecl, int stage, int portion)
    {
        if (stage == 1) {
            switch (portion) {
            case 1:
                ECLWarp(68);
                break;
            case 2:
                ECLWarp(580);
                break;
            case 3:
                ECLWarp(1160);
                break;
            case 4:
                ECLWarp(1540);
                break;
            case 5:
                ECLWarp(2348);
                break;
            case 6:
                ECLWarp(4438);
                break;
            default:
                break;
            }
        } else if (stage == 2) {
            switch (portion) {
            case 1:
                ECLWarp(270);
                break;
            case 2:
                ECLWarp(924);
                break;
            case 3:
                ECLWarp(3528);
                break;
            case 4:
                ECLWarp(4563);
                break;
            default:
                break;
            }
        } else if (stage == 3) {
            switch (portion) {
            case 1:
                ECLWarp(340);
                break;
            case 2:
                ECLWarp(1050);
                break;
            case 3:
                ECLWarp(1670);
                break;
            case 4:
                ECLWarp(2762);
                break;
            case 5:
                ECLWarp(3807);
                break;
            case 6:
                ECLWarp(4118);
                break;
            case 7:
                ECLWarp(5274);
                break;
            default:
                break;
            }
        } else if (stage == 4) {
            switch (portion) {
            case 1:
                ECLWarp(380);
                break;
            case 2:
                ECLWarp(1454);
                break;
            case 3:
                ECLWarp(2328);
                break;
            case 4:
                ECLWarp(0x0d40);
                break;
            case 5:
                ECLWarp(4872);
                break;
            case 6:
                ECLWarp(5712);
                break;
            case 7:
                ECLWarp(7434);
                break;
            case 8:
                ECLWarp(8354);
                break;
            case 9:
                ECLWarp(9784);
                break;
            default:
                break;
            }
        } else if (stage == 5) {
            switch (portion) {
            case 1:
                ECLWarp(350);
                break;
            case 2:
                ECLWarp(1352);
                break;
            case 3:
                ECLWarp(2292);
                break;
            case 4:
                ECLWarp(3814);
                break;
            case 5:
                ECLWarp(6774);
                break;
            default:
                break;
            }
        } else if (stage == 6) {
            switch (portion) {
            case 1:
                ECLWarp(380);
                break;
            case 2:
                ECLWarp(1484);
                break;
            default:
                break;
            }
        } else if (stage == 7) {
            switch (portion) {
            case 1:
                ECLWarp(380);
                break;
            case 2:
                ECLWarp(1300);
                break;
            case 3:
                ECLWarp(2600);
                break;
            case 4:
                ECLWarp(3680);
                break;
            case 5:
                ECLWarp(4803);
                break;
            case 6:
                ECLWarp(5933);
                break;
            case 7:
                ECLWarp(7733);
                break;
            default:
                break;
            }
        }
    }
    __declspec(noinline) void THSectionPatch()
    {
        ECLHelper ecl;
        ecl.SetBaseAddr((void*)0x487e50);

        auto section = thPracParam.section;
        if (section >= 10000 && section < 20000) {
            int stage = (section - 10000) / 100;
            int portionId = (section - 10000) % 100;
            THStageWarp(ecl, stage, portionId);
        } else {
            THPatch(ecl, (th_sections_t)section);
        }
    }

    // Hook Helper
    bool THBGMTest()
    {
        if (!thPracParam.mode)
            return 0;
        else if (thPracParam.section >= 10000)
            return 0;
        else
            return th_sections_bgm[thPracParam.section];
    }
    void THSaveReplay(char* rep_name)
    {
        ReplaySaveParam(mb_to_utf16(rep_name, 932).c_str(), thPracParam.GetJson());
    }

    EHOOK_G1(th06_result_screen_create, 0x42d812)
    {
        th06_result_screen_create::GetHook().Disable();
        *(uint32_t*)(*(uint32_t*)(pCtx->Ebp - 0x10) + 0x8) = 0xA;
        pCtx->Eip = 0x42d839;
    }
    HOOKSET_DEFINE(THMainHook)
    PATCH_DY(th06_reacquire_input, 0x41dc58, "\x00\x00\x00\x00\x74", 5);
    EHOOK_DY(th06_bgm_play, 0x424b5d)
    {
        int32_t retn_addr = ((int32_t*)pCtx->Esp)[0];

        if (THPauseMenu::singleton().el_bgm_signal) {
            pCtx->Eip = 0x424d35;
        }
        if (retn_addr == 0x418db4) {
            THPauseMenu::singleton().el_bgm_changed = true;
        }
    }
    EHOOK_DY(th06_bgm_stop, 0x430f80)
    {
        if (THPauseMenu::singleton().el_bgm_signal) {
            pCtx->Eip = 0x43107b;
        }
    }
    EHOOK_DY(th06_prac_menu_1, 0x437179)
    {
        THGuiPrac::singleton().State(1);
    }
    EHOOK_DY(th06_prac_menu_3, 0x43738c)
    {
        THGuiPrac::singleton().State(3);
    }
    EHOOK_DY(th06_prac_menu_4, 0x43723f)
    {
        THGuiPrac::singleton().State(4);
    }
    EHOOK_DY(th06_prac_menu_enter, 0x4373a3)
    {
        *(int32_t*)(0x69d6d4) = *(int32_t*)(0x69d6d8) = thPracParam.stage;
        if (thPracParam.stage == 6)
            *(int8_t*)(0x69bcb0) = 4;
        else
            *(int8_t*)(0x69bcb0) = *(int8_t*)(0x6c6e49);
    }
    EHOOK_DY(th06_pause_menu, 0x401b8f)
    {
        if (thPracParam.mode && (*((int32_t*)0x69bcbc) == 0)) {
            auto sig = THPauseMenu::singleton().PMState();
            if (sig == THPauseMenu::SIGNAL_RESUME) {
                pCtx->Eip = 0x40223d;
            } else if (sig == THPauseMenu::SIGNAL_EXIT) {
                *(uint32_t*)0x6c6ea4 = 7; // Set gamemode to result screen
                *(uint16_t*)0x69d4bf = 0; // Close pause menu
                th06_result_screen_create::GetHook().Enable();
            } else if (sig == THPauseMenu::SIGNAL_RESTART) {
                pCtx->Eip = 0x40263c;
            } else {
                pCtx->Eip = 0x4026a6;
            }
        }
    }
    EHOOK_DY(th06_patch_main, 0x41c17a)
    {
        THPauseMenu::singleton().el_bgm_changed = false;
        if (thPracParam.mode == 1) {
            // TODO: Probably remove this ASM comment?
            /*
					mov eax,dword ptr [@MENU_RANK]
					mov dword ptr [69d710],eax
					cmp dword ptr [@MENU_RANKLOCK],@MENU_ON_STR
					jnz @f
					mov dword ptr [69d714],eax
					mov dword ptr [69d718],eax
				*/
            *(int8_t*)(0x69d4ba) = (int8_t)thPracParam.life;
            *(int8_t*)(0x69d4bb) = (int8_t)thPracParam.bomb;
            *(int16_t*)(0x69d4b0) = (int16_t)thPracParam.power;
            *(int32_t*)(0x69bca0) = *(int32_t*)(0x69bca4) = (int32_t)thPracParam.score;
            *(int32_t*)(0x69bcb4) = *(int32_t*)(0x69bcb8) = (int32_t)thPracParam.graze;
            *(int16_t*)(0x69d4b4) = *(int16_t*)(0x69d4b6) = (int16_t)thPracParam.point;
            *(uint32_t*)0x5a5fb0 = thPracParam.frame;

            if (*(int8_t*)(0x69bcb0) != 4) {
                if (thPracParam.score >= 60000000)
                    *(int8_t*)(0x69d4bc) = 4;
                else if (thPracParam.score >= 40000000)
                    *(int8_t*)(0x69d4bc) = 3;
                else if (thPracParam.score >= 20000000)
                    *(int8_t*)(0x69d4bc) = 2;
                else if (thPracParam.score >= 10000000)
                    *(int8_t*)(0x69d4bc) = 1;
            }

            *(int32_t*)(0x69d710) = *(int32_t*)(0x69d710) = (int32_t)thPracParam.rank;
            if (thPracParam.rankLock) {
                *(int32_t*)(0x69d714) = *(int32_t*)(0x69d714) = (int32_t)thPracParam.rank;
                *(int32_t*)(0x69d718) = *(int32_t*)(0x69d718) = (int32_t)thPracParam.rank;
            }

            THSectionPatch();
        }
        thPracParam._playLock = true;

        if (THPauseMenu::singleton().el_bgm_signal) {
            THPauseMenu::singleton().el_bgm_signal = false;
            pCtx->Eip = 0x41c18a;
        } else if (THBGMTest()) {
            pCtx->Eax += 0x310;
            pCtx->Eip = 0x41c17f;
        }
    }
    PATCH_S1(th06_white_screen, 0x42fee0, "\xc3", 1);
    EHOOK_DY(th06_restart, 0x435901)
    {
        if (thRestartFlag) {
            th06_white_screen::GetPatch().Enable();
            thRestartFlag = false;
            pCtx->Eip = 0x43738c;
        } else {
            thPracParam.Reset();
            th06_white_screen::GetPatch().Disable();
        }
    }
    EHOOK_DY(th06_title, 0x41ae2c)
    {
        if (thPracParam.mode != 0 && thPracParam.section) {
            pCtx->Eip = 0x41af35;
        }
    }
    PATCH_DY(th06_preplay_1, 0x42d835, "\x09", 1);
    EHOOK_DY(th06_preplay_2, 0x418ef9)
    {
        if (thPracParam.mode && !THGuiRep::singleton().mRepStatus) {
            *(uint32_t*)0x69bca0 = *(uint32_t*)0x69bca4;
            pCtx->Eip = 0x418f0e;
        }
    }
    EHOOK_DY(th06_save_replay, 0x42b03b)
    {
        char* rep_name = *(char**)(pCtx->Ebp + 0x8);
        if (thPracParam.mode)
            THSaveReplay(rep_name);
    }
    EHOOK_DY(th06_rep_menu_1, 0x438262)
    {
        THGuiRep::singleton().State(1);
    }
    EHOOK_DY(th06_rep_menu_2, 0x4385d5)
    {
        THGuiRep::singleton().State(2);
    }
    EHOOK_DY(th06_rep_menu_3, 0x438974)
    {
        THGuiRep::singleton().State(3);
    }
    EHOOK_DY(th06_fake_shot_type, 0x40b2f9)
    {
        if (thPracParam.fakeType) {
            *((int32_t*)0x487e44) = thPracParam.fakeType - 1;
            pCtx->Eip = 0x40b2ff;
        }
    }
    EHOOK_DY(th06_patchouli, 0x40c100)
    {
        int32_t* var = *(int32_t**)(pCtx->Esp + 4);
        if (thPracParam.fakeType) {
            var[618] = ((int32_t*)0x476264)[3 * (thPracParam.fakeType - 1)];
            var[619] = ((int32_t*)0x476268)[3 * (thPracParam.fakeType - 1)];
            var[620] = ((int32_t*)0x47626c)[3 * (thPracParam.fakeType - 1)];
            pCtx->Eip = 0x40c174;
        }
    }
    EHOOK_DY(th06_cancel_muteki, 0x429ec4)
    {
        if (thPracParam.mode) {
            *(uint8_t*)(pCtx->Eax + 0x9e0) = 0;
            pCtx->Eip = 0x429ecb;
        }
    }
    EHOOK_DY(th06_hamon_rage, 0x40e1c7)
    {
        if (thPracParam.mode && thPracParam.stage == 6 && thPracParam.section == TH06_ST7_END_S10 && thPracParam.phase == 1) {
            pCtx->Eip = 0x40e1d8;
        }
    }
    PATCH_DY(th06_disable_menu, 0x439ab2, "\x90\x90\x90\x90\x90", 5);
    EHOOK_DY(th06_update, 0x41caac)
    {
        GameGuiBegin(IMPL_WIN32_DX8, !THAdvOptWnd::singleton().IsOpen());

        // Gui components update
        Gui::KeyboardInputUpdate(VK_ESCAPE);
        THPauseMenu::singleton().Update();
        THGuiPrac::singleton().Update();
        THGuiRep::singleton().Update();
        THOverlay::singleton().Update();

        GameGuiEnd(THAdvOptWnd::StaticUpdate() || THGuiPrac::singleton().IsOpen() || THPauseMenu::singleton().IsOpen());
    }
    EHOOK_DY(th06_render, 0x41cb6d)
    {
        GameGuiRender(IMPL_WIN32_DX8);
        if (Gui::KeyboardInputUpdate(VK_HOME) == 1)
            THSnapshot::Snapshot(*(IDirect3DDevice8**)0x6c6d20);
    }
    HOOKSET_ENDDEF()

    HOOKSET_DEFINE(THInitHook)
    static __declspec(noinline) void THGuiCreate()
    {
        // Init
        GameGuiInit(IMPL_WIN32_DX8, 0x6c6d20, 0x6c6bd4, 0x420d40,
            Gui::INGAGME_INPUT_GEN1, 0x69d904, 0x69d908, 0x69d90c,
            -1);

        // Gui components creation
        THGuiPrac::singleton();
        THPauseMenu::singleton();
        THGuiRep::singleton();
        THOverlay::singleton();

        // Hooks
        THMainHook::singleton().EnableAllHooks();

        // Reset thPracParam
        thPracParam.Reset();
    }
    static __declspec(noinline) void THInitHookDisable()
    {
        auto& s = THInitHook::singleton();
        s.th06_gui_init_1.Disable();
        s.th06_gui_init_2.Disable();
    }
    EHOOK_DY(th06_gui_init_1, 0x43596f)
    {
        THGuiCreate();
        THInitHookDisable();
    }
    EHOOK_DY(th06_gui_init_2, 0x42140c)
    {
        THGuiCreate();
        THInitHookDisable();
    }
    HOOKSET_ENDDEF()
}

void TH06Init()
{
    TH06::THInitHook::singleton().EnableAllHooks();
}

}
