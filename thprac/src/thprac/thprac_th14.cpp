#include "thprac_utils.h"
#include <list>

namespace THPrac {
namespace TH14 {
    using std::pair;
    struct THPracParam {
        int32_t mode;
        int32_t stage;
        int32_t section;
        int32_t phase;

        int64_t score;
        int32_t life;
        int32_t life_fragment;
        int32_t bomb;
        int32_t bomb_fragment;
        int32_t cycle;
        int32_t power;
        int32_t value;
        int32_t graze;

        bool _playLock = false;
        void Reset()
        {
            memset(this, 0, sizeof(THPracParam));
        }
        bool ReadJson(std::string& json)
        {
            ParseJson();

            ForceJsonValue(game, "th14");
            GetJsonValue(mode);
            GetJsonValue(stage);
            GetJsonValue(section);
            GetJsonValue(phase);

            GetJsonValue(score);
            GetJsonValue(life);
            GetJsonValue(life_fragment);
            GetJsonValue(bomb);
            GetJsonValue(bomb_fragment);
            GetJsonValue(cycle);
            GetJsonValue(power);
            GetJsonValue(value);
            GetJsonValue(graze);

            return true;
        }
        std::string GetJson()
        {
            if (mode == 1) {
                CreateJson();

                AddJsonValueEx(version, GetVersionStr(), jalloc);
                AddJsonValueEx(game, "th14", jalloc);
                AddJsonValue(mode);
                AddJsonValue(stage);
                if (section)
                    AddJsonValue(section);
                if (phase)
                    AddJsonValue(phase);

                AddJsonValue(score);
                AddJsonValue(life);
                AddJsonValue(life_fragment);
                AddJsonValue(bomb);
                AddJsonValue(bomb_fragment);
                AddJsonValue(cycle);
                AddJsonValue(power);
                AddJsonValue(value);
                AddJsonValue(graze);

                ReturnJson();
            } else if (mode == 2) {
                CreateJson();

                AddJsonValueEx(version, GetVersionStr(), jalloc);
                AddJsonValueEx(game, "th14", jalloc);
                AddJsonValue(mode);
                AddJsonValue(phase);

                ReturnJson();
            }

            CreateJson();
            ReturnJson();
        }
    };
    THPracParam thPracParam {};

    class THGuiPrac : public Gui::GameGuiWnd {
        THGuiPrac() noexcept
        {
            *mMode = 1;
            *mLife = 9;
            *mBomb = 9;
            *mPower = 400;
            *mValue = 10000;

            //SetWndFlag(ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
            SetFade(0.8f, 0.1f);
            SetStyle(ImGuiStyleVar_WindowRounding, 0.0f);
            SetStyle(ImGuiStyleVar_WindowBorderSize, 0.0f);
            OnLocaleChange();
        }
        SINGLETON(THGuiPrac);
    public:

        int GetStage()
        {
            return *mStage;
        }

        __declspec(noinline) void State(int state)
        {
            switch (state) {
            case 0:
                break;
            case 1:
                mDiffculty = *((int32_t*)0x4f5834);
                mShotType = *((int32_t*)0x4f582c);
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

                thPracParam.score = *mScore;
                thPracParam.life = *mLife;
                thPracParam.life_fragment = *mLifeFragment;
                thPracParam.bomb = *mBomb;
                thPracParam.bomb_fragment = *mBombFragment;
                thPracParam.cycle = *mCycle;
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
                SetPosRel(0.4f, 0.14f);
                SetItemWidthRel(-0.100f);
                SetAutoSpacing(true);
                break;
            case Gui::LOCALE_EN_US:
                SetSizeRel(0.6f, 0.75f);
                SetPosRel(0.35f, 0.165f);
                SetItemWidthRel(-0.100f);
                SetAutoSpacing(true);
                break;
            case Gui::LOCALE_JA_JP:
                SetSizeRel(0.56f, 0.81f);
                SetPosRel(0.37f, 0.14f);
                SetItemWidthRel(-0.105f);
                SetAutoSpacing(true);
                break;
            default:
                break;
            }
        }
        virtual void OnContentUpdate() override
        {
            ImGui::Text(XSTR(TH_MENU));
            ImGui::Separator();

            PracticeMenu();
        }
        th_glossary_t* SpellPhase()
        {
            auto section = CalcSection();
            if (section == TH14_ST6_BOSS9) {
                return TH_SPELL_PHASE1;
            } else if (section == TH14_ST6_BOSS10) {
                return TH14_SPELL_4PHASE;
            } else if (section == TH14_ST7_END_S10) {
                return TH14_SPELL_5PHASE;
            }
            return nullptr;
        }
        void PracticeMenu()
        {
            mMode();
            if (mStage())
                *mSection = *mChapter = 0;
            if (*mMode == 1) {
                if (mWarp())
                    *mSection = *mChapter = *mPhase = 0;
                if (*mWarp) {
                    SectionWidget();
                    mPhase(TH_PHASE, SpellPhase());
                }

                mLife();
                mLifeFragment();
                mBomb();
                mBombFragment();
                mCycle();
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
        void SectionWidget()
        {
            int st_offset = 0;
            if (*mStage == 3)
                st_offset = 4 + mShotType;
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
                        th_sections_cba[*mStage + st_offset][*mWarp - 2],
                        th_sections_str[::THPrac::Gui::LocaleGet()][mDiffculty]))
                    *mPhase = 0;
                break;
            case 4:
            case 5: // Non-spell & Spellcard
                if (mSection(TH_WARP_SELECT[*mWarp],
                        th_sections_cbt[*mStage + st_offset][*mWarp - 4],
                        th_sections_str[::THPrac::Gui::LocaleGet()][mDiffculty]))
                    *mPhase = 0;
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
        Gui::GuiCombo mCycle { TH14_CYCLE, TH14_CYCLE_LIST };

        Gui::GuiSlider<int, ImGuiDataType_S32> mChapter { TH_CHAPTER, 0, 0 };
        Gui::GuiDrag<int64_t, ImGuiDataType_S64> mScore { TH_SCORE, 0, 9999999990, 10, 100000000 };
        Gui::GuiSlider<int, ImGuiDataType_S32> mLife { TH_LIFE, 0, 9 };
        Gui::GuiSlider<int, ImGuiDataType_S32> mLifeFragment { TH_LIFE_FRAGMENT, 0, 2 };
        Gui::GuiSlider<int, ImGuiDataType_S32> mBomb { TH_BOMB, 0, 9 };
        Gui::GuiSlider<int, ImGuiDataType_S32> mBombFragment { TH_BOMB_FRAGMENT, 0, 7 };
        Gui::GuiSlider<int, ImGuiDataType_S32> mPower { TH_POWER, 100, 400 };
        Gui::GuiDrag<int, ImGuiDataType_S32> mValue { TH_VALUE, 0, 999990, 10, 100000 };
        Gui::GuiDrag<int, ImGuiDataType_S32> mGraze { TH_GRAZE, 0, 999999, 1, 100000 };

        Gui::GuiNavFocus mNavFocus { TH_STAGE, TH_MODE, TH_WARP,
            TH_MID_STAGE, TH_END_STAGE, TH_NONSPELL, TH_SPELL, TH_PHASE, TH_CHAPTER,
            TH_SCORE, TH_LIFE, TH_LIFE_FRAGMENT, TH_BOMB, TH_BOMB_FRAGMENT, TH14_CYCLE,
            TH_POWER, TH_VALUE, TH_GRAZE };

        int mChapterSetup[7][2] {
            { 3, 2 },
            { 3, 3 },
            { 3, 3 },
            { 4, 3 },
            { 4, 2 },
            { 4, 0 },
            { 4, 5 },
        };

        int mDiffculty = 0;
        int mShotType = 0;
    };
    class THGuiRep : public Gui::GameGuiWnd {
        THGuiRep() noexcept
        {
            wchar_t appdata[MAX_PATH];
            GetEnvironmentVariableW(L"APPDATA", appdata, MAX_PATH);
            mAppdataPath = appdata;
        }
        SINGLETON(THGuiRep);
    public:

        void CheckReplay()
        {
            uint32_t index = GetMemContent(0x4db6a4, 0x5aec);
            char* repName = (char*)GetMemAddr(0x4db6a4, index * 4 + 0x5af4, 0x220);
            std::wstring repDir(mAppdataPath);
            repDir.append(L"\\ShanghaiAlice\\th14\\replay\\");
            repDir.append(mb_to_utf16(repName));

            std::string param;
            if (ReplayLoadParam(repDir.c_str(), param) && mRepParam.ReadJson(param))
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
        std::wstring mAppdataPath;
        bool mParamStatus = false;
        THPracParam mRepParam;
    };
    class THOverlay : public Gui::GameGuiWnd {
        THOverlay() noexcept
        {
            SetTitle("Mod Menu");
            SetFade(0.5f, 0.5f);
            SetCursor(false);
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
                    //*((int32_t*)0x6c6eb0) = 2;
                }
            }
            //if (*((int32_t*)0x6c6ea4) == 2)
            //	SetPos(500.0f, 300.0f);
            //else
            //	SetPos(10.0f, 10.0f);
        }

        Gui::GuiHotKey mMenu { "ModMenuToggle", "BACKSPACE", VK_BACK };
        Gui::GuiHotKey mMuteki { TH_MUTEKI, "F1", VK_F1,
            (void*)0x44F877, "\x01", 1 };
        Gui::GuiHotKey mInfLives { TH_INFLIVES, "F2", VK_F2,
            (void*)0x44F617, "\x90", 1 };
        Gui::GuiHotKey mInfBombs { TH_INFBOMBS, "F3", VK_F3,
            (void*)0x412173, "\x90", 1 };
        Gui::GuiHotKey mInfPower { TH_INFPOWER, "F4", VK_F4,
            (void*)0x44DDA5, "\x58", 1 };
        Gui::GuiHotKey mTimeLock { TH_TIMELOCK, "F5", VK_F5,
            (void*)0x41C5DD, "\xeb", 1, (void*)0x424ACA, "\x90", 1 };
        Gui::GuiHotKey mAutoBomb { TH_AUTOBOMB, "F6", VK_F6,
            (void*)0x44DEC4, "\xE9\x07\x31\x06\x00\x90\x90", 8,
            (void*)0x4b0fd0, "\xA1\x2C\xB5\x4D\x00\x83\xC0\x40\x8B\x00\x85\xC0\x0F\x84\xEF\xCE\xF9\xFF\xE9\xEA\xCF\xF9\xFF\x90", 25 };

    public:
        Gui::GuiHotKey mElBgm {
            TH_EL_BGM,
            "F7",
            VK_F7,
        };
    };
    class THGuiSP : public Gui::GameGuiWnd {
        THGuiSP() noexcept
        {
            *mBugFix = true;

            //SetWndFlag(ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
            SetFade(0.8f, 0.1f);
            SetStyle(ImGuiStyleVar_WindowRounding, 0.0f);
            SetStyle(ImGuiStyleVar_WindowBorderSize, 0.0f);
            OnLocaleChange();
        }
        SINGLETON(THGuiSP);
    public:

        int mState = 0;
        __declspec(noinline) int State()
        {
            uint32_t spell_id;

            switch (mState) {
            case 0:
                spell_id = GetMemContent(0x4db6a4, GetMemContent(0x4db6a4, 0x28) * 4 + 0x5d70);
                if (mSpellId != spell_id) {
                    *mPhase = 0;
                    mSpellId = spell_id;
                }
                if (!SpellPhase())
                    return 1;
                SetFade(0.8f, 0.1f);
                Open();
                thPracParam.Reset();
                mState = 1;
                break;
            case 1:
                if (IsOpen()) {
                    if (Gui::InGameInputGet('Z')) {
                        SetFade(0.8f, 0.1f);
                        Close();

                        // Fill Param
                        thPracParam.mode = 2;
                        thPracParam.phase = *mPhase;

                        // Reset Frame Counter
                        *(int32_t*)(*((int32_t*)0x4db6a4) + 0x2b8) = 0;

                        mState = 0;

                        return 1;
                    } else if (Gui::InGameInputGet('X')) {
                        mState = 2;
                    }
                }
                break;
            case 2:
                SetFade(0.8f, 0.1f);
                Close();
                if (IsClosed())
                    mState = 0;
            default:
                break;
            }

            return 0;
        }

    protected:
        virtual void OnLocaleChange() override
        {
            SetTitle(XSTR(TH_SPELL_PRAC));
            switch (Gui::LocaleGet()) {
            case Gui::LOCALE_ZH_CN:
                SetSizeRel(0.38f, 0.12f);
                SetPosRel(0.35f, 0.45f);
                SetItemWidthRel(-0.075f);
                SetAutoSpacing(true);
                break;
            case Gui::LOCALE_EN_US:
                SetSizeRel(0.38f, 0.12f);
                SetPosRel(0.35f, 0.45f);
                SetItemWidthRel(-0.075f);
                SetAutoSpacing(true);
                break;
            case Gui::LOCALE_JA_JP:
                SetSizeRel(0.38f, 0.12f);
                SetPosRel(0.35f, 0.45f);
                SetItemWidthRel(-0.075f);
                SetAutoSpacing(true);
                break;
            default:
                break;
            }
        }
        virtual void OnContentUpdate() override
        {
            ImGui::Text(XSTR(TH_SPELL_PRAC));
            ImGui::Separator();

            PracticeMenu();
        }
        th_glossary_t* SpellPhase()
        {
            if (mSpellId >= 100 && mSpellId <= 103) {
                return TH_SPELL_PHASE1;
            } else if (mSpellId >= 104 && mSpellId <= 107) {
                return TH14_SPELL_4PHASE;
            } else if (mSpellId == 119) {
                return TH14_SPELL_5PHASE;
            }
            return nullptr;
        }
        void PracticeMenu()
        {
            mPhase(TH_PHASE, SpellPhase());

#if 0
				SIZE renderSize;
				renderSize.cx = 1280;
				renderSize.cy = 960;
				ImGui::Text("Pos: %f, %f", ImGui::GetWindowPos().x / (float)renderSize.cx,
					ImGui::GetWindowPos().y / (float)renderSize.cy);
				ImGui::Text("Size: %f, %f", ImGui::GetWindowSize().x / (float)renderSize.cx,
					ImGui::GetWindowSize().y / (float)renderSize.cy);
#endif

            mNavFocus();
        }

        unsigned int mSpellId = -1;

        Gui::GuiCheckBox mBugFix { TH16_BUGFIX };
        Gui::GuiCombo mPhase { TH_PHASE };
        Gui::GuiNavFocus mNavFocus { TH_PHASE };
    };

    class THMarisaLaser {
        friend class THAdvOptWnd;
        THMarisaLaser() = default;
        SINGLETON(THMarisaLaser);

    public:
        struct record_t {
            uint32_t stage;
            uint32_t frame;
            uint32_t ordinal;
            float value;
            int fix;
            //Gui::GuiCombo fix;

            record_t()
                : stage(0)
                , frame(0)
                , ordinal(0)
                , value(0.0f)
                , fix(0)
            {
            }
            record_t(float _value)
            {
                stage = *(uint32_t*)0x4f58a4;
                frame = *(uint32_t*)0x4f58b0;
                ordinal = 0;
                value = _value;
                fix = 0;
            }
            record_t(float _value, record_t& last)
            {
                stage = *(uint32_t*)0x4f58a4;
                frame = *(uint32_t*)0x4f58b0;
                if (stage == last.stage && frame == last.frame)
                    ordinal = last.ordinal + 1;
                else
                    ordinal = 0;
                value = _value;
                fix = 0;
            }
            void Clear()
            {
                stage = frame = ordinal = fix = 0;
                value = 0.0f;
            }
            bool operator==(const record_t& b) const
            {
                return stage == b.stage && frame == b.frame && ordinal == b.ordinal;
            }
            bool operator<(const record_t& b) const
            {
                if (stage < b.stage)
                    return true;
                else if (stage == b.stage) {
                    if (frame < b.frame)
                        return true;
                    else if (frame == b.frame) {
                        if (ordinal < b.ordinal)
                            return true;
                    }
                }
                return false;
            }
            record_t& operator=(record_t& b)
            {
                stage = b.stage;
                frame = b.frame;
                ordinal = b.ordinal;
                value = b.value;
                fix = b.fix;
                return *this;
            }
        };

    private:
        bool mOngoing = false;
        int mState = 0;
        std::list<record_t> mRecordsNormal;
        std::list<record_t> mRecordsPlayback;
        std::list<record_t> mRecordsRecover;
        record_t mPrevRecord {};

        void MergePbAndRec()
        {
            if (mRecordsPlayback.size()) {
                for (auto& record : mRecordsPlayback) {
                    auto it = mRecordsRecover.begin();
                    for (; true; ++it) {
                        if (it == mRecordsRecover.end() || record < *it) {
                            mRecordsRecover.insert(it, record);
                            break;
                        } else if (record == *it)
                            break;
                    }
                }
            }
        }
        float GetAltValue(record_t& record, float fallback_value)
        {
            uint32_t value = 0;
            float fvalue;
            int tempFix = record.fix & 0x0000ffff;
            int tempE = (record.fix & 0xffff0000) >> 16;
            switch (TH14_CORRECTION[tempFix]) {
            case TH14_CORRECTION_RECORDED:
                return record.value;
                break;
            case TH14_CORRECTION_ZERO:
                return 0.0f;
                break;
            case TH14_CORRECTION_E_POSITIVE:
                fvalue = 1.0f;
                for (int i = 0; i < tempE; ++i)
                    fvalue *= 10.0f;
                return fvalue;
                break;
            case TH14_CORRECTION_E_NEGATIVE:
                fvalue = -1.0f;
                for (int i = 0; i < tempE; ++i)
                    fvalue *= 10.0f;
                return fvalue;
                break;
            case TH14_CORRECTION_NAN:
                value = 0x7f800001;
                return *(float*)(&value);
                break;
            case TH14_CORRECTION_NAN_SIGNED:
                value = 0xff800001;
                return *(float*)(&value);
                break;
            case TH14_CORRECTION_INF:
                value = 0x7f800000;
                return *(float*)(&value);
                break;
            case TH14_CORRECTION_NINF:
                value = 0xff800000;
                return *(float*)(&value);
                break;
            case TH14_CORRECTION_SN:
                value = 0x00400000;
                return *(float*)(&value);
                break;
            case TH14_CORRECTION_SN_SIGNED:
                value = 0x80400000;
                return *(float*)(&value);
                break;
            default:
                return fallback_value;
            }

            return fallback_value;
        }
        float AccessNormal(record_t& current)
        {
            mRecordsNormal.push_back(current);
            return current.value;
        }
        float AccessPlayback(record_t& current)
        {
            for (auto it = mRecordsPlayback.begin();
                 it != mRecordsPlayback.end();
                 ++it) {
                if (*it == current)
                    return GetAltValue(*it, current.value);
            }
            return current.value;
        }
        float AccessRecover(record_t& current)
        {
            auto it = mRecordsRecover.begin();
            for (; true; ++it) {
                if (it == mRecordsRecover.end() || current < *it) {
                    mRecordsRecover.insert(it, current);
                    return current.value;
                } else if (current == *it) {
                    return GetAltValue(*it, current.value);
                }
            }
            return current.value;
        }

    public:
        __declspec(noinline) uint32_t Access(uint32_t value)
        {
            float _value = *(float*)(&value);
            record_t current(_value, mPrevRecord);
            mPrevRecord = current;

            switch (mState) {
            case 1:
                _value = AccessNormal(current);
                break;
            case 2:
                _value = AccessPlayback(current);
                break;
            case 3:
                _value = AccessRecover(current);
                break;
            default:
                break;
            }

            //char str[256];
            //sprintf_s(str, "%f", _value);
            //MB_INFO(str);

            return *(uint32_t*)(&_value);
        }
    };
    class THAdvOptWnd : public Gui::PPGuiWnd {
        EHOOK_ST(th14_marisa_laser, (void*)0x45286f)
        {
            *(uint32_t*)(pCtx->Esp + 0x2c) = THMarisaLaser::singleton().Access(*(uint32_t*)(pCtx->Esp + 0x2c));
        }
        EHOOK_ST(th14_all_clear_bonus_1, (void*)0x43708a)
        {
            pCtx->Eip = 0x437098;
        }
        EHOOK_ST(th14_all_clear_bonus_2, (void*)0x43712c)
        {
            *(int32_t*)(GetMemAddr(0x4db550, 0x140)) = *(int32_t*)(0x4f5830);
            if (GetMemContent(0x4f58b8) & 0x10) {
                typedef void (*PScoreFunc)();
                PScoreFunc a = (PScoreFunc)0x449f20;
                a();
                pCtx->Eip = 0x43708f;
            }
        }
        EHOOK_ST(th14_all_clear_bonus_3, (void*)0x43719f)
        {
            *(int32_t*)(GetMemAddr(0x4db550, 0x140)) = *(int32_t*)(0x4f5830);
            if (GetMemContent(0x4f58b8) & 0x10) {
                *(int32_t*)(GetMemAddr(0x4db550, 0x140)) = *(int32_t*)(0x4f5830);
                typedef void (*PScoreFunc)();
                PScoreFunc a = (PScoreFunc)0x449f20;
                a();
                pCtx->Eip = 0x43708f;
            }
        }
    private:
        // Option Related Functions
        void FpsInit()
        {
            // mOptCtx.vpatch_base = (int32_t)GetModuleHandleW("");
            if (false) {
                //if (*(int32_t*)(mOptCtx.vpatch_base + 0x1a024) == 0)
                //	mOptCtx.fps_status = 2;
            } else if (*(uint8_t*)0x4d9159 == 3) {
                mOptCtx.fps_status = 1;

                DWORD oldProtect;
                VirtualProtect((void*)0x46a792, 4, PAGE_EXECUTE_READWRITE, &oldProtect);
                *(double**)0x46a792 = &mOptCtx.fps_dbl;
                VirtualProtect((void*)0x46a792, 4, oldProtect, &oldProtect);
            } else
                mOptCtx.fps_status = 0;
        }
        void FpsSet()
        {
            if (mOptCtx.fps_status == 1) {
                mOptCtx.fps_dbl = 1.0 / (double)mOptCtx.fps;
            } else if (mOptCtx.fps_status == 2) {
                //*(int32_t*)(mOptCtx.vpatch_base + 0x16a8c) = mOptCtx.fps;
            }
        }
        void GameplayInit()
        {
            th14_all_clear_bonus_1.Setup();
            th14_all_clear_bonus_2.Setup();
            th14_all_clear_bonus_3.Setup();
        }
        void GameplaySet()
        {
            th14_all_clear_bonus_1.Toggle(mOptCtx.all_clear_bonus);
            th14_all_clear_bonus_2.Toggle(mOptCtx.all_clear_bonus);
            th14_all_clear_bonus_3.Toggle(mOptCtx.all_clear_bonus);
        }
        void DatRecInit()
        {
            mOptCtx.data_rec_func = [&](std::vector<RecordedValue>& values) {
                return DataRecFunc(values);
            };
            wchar_t appdata[MAX_PATH];
            GetEnvironmentVariableW(L"APPDATA", appdata, MAX_PATH);
            mOptCtx.data_rec_dir = appdata;
            mOptCtx.data_rec_dir += L"\\ShanghaiAlice\\th14\\replay\\";
        }
        void DataRecPreUpd()
        {
            DataRecOpt(mOptCtx, true, thPracParam._playLock);
        }
        void DataRecFunc(std::vector<RecordedValue>& values)
        {
            values.clear();
            values.emplace_back("Score", (int64_t) * (int32_t*)(0x4f5830) * 10ll);
            values.emplace_back("Graze", *(int32_t*)(0x4f5840));
            values.emplace_back("Value", ((float)*(int32_t*)(0x4f584c) / 100.0f), "%.2f");
        }
        void DataRecMenu()
        {
            if (DataRecOpt(mOptCtx)) {
                SetContentUpdFunc([&]() { ContentUpdate(); });
            }
        }

        void MarisaLaserInit()
        {
            THMarisaLaser::singleton();
            th14_marisa_laser.Setup();

            wchar_t appdata[MAX_PATH];
            GetEnvironmentVariableW(L"APPDATA", appdata, MAX_PATH);
            mRepDir = appdata;
            mRepDir += L"\\ShanghaiAlice\\th14\\replay\\";
        }
        bool MarisaLaserMenu()
        {
            auto thMarisaLaser = &THMarisaLaser::singleton();
            bool wndFocus = true;
            ImGui::PushTextWrapPos();
            ImGui::PushItemWidth(GetRelWidth(0.25f));
            if (mLock)
                ImGui::BeginDisabled();
            ImGui::ComboSectionsDefault(XSTR(TH14_MODE), &(thMarisaLaser->mState), TH14_MODE_COMBO, XITEMS, "");
            if (ImGui::IsPopupOpen(XSTR(TH14_MODE))) {
                wndFocus = false;
            }
            ImGui::PopItemWidth();
            if (mLock) {
                ImGui::EndDisabled();
                ImGui::SameLine();
                ImGui::Text(XSTR(TH14_LOCKED));
            }

            if (thMarisaLaser->mState == 0) {
                ImGui::Text(XSTR(TH14_MODE_NONE_DESC));
            } else if (thMarisaLaser->mState == 1) {
                ImGui::Text(XSTR(TH14_MODE_NORMAL_DESC));
            } else if (thMarisaLaser->mState == 2) {
                ImGui::Text(XSTR(TH14_MODE_PLAYBACK_DESC));
                if (mRepName[0]) {
                    ImGui::Text(XSTR(TH14_SELECTED_REPLAY), mRepName);
                    if (thMarisaLaser->mRecordsPlayback.size()) {
                        int i = 1;
                        for (auto& record : thMarisaLaser->mRecordsPlayback) {
                            int tempFix = record.fix & 0x0000ffff;
                            int tempE = (record.fix & 0xffff0000) >> 16;

                            if (TH14_CORRECTION[tempFix] == TH14_CORRECTION_E_POSITIVE || TH14_CORRECTION[tempFix] == TH14_CORRECTION_E_NEGATIVE) {
                                ImGui::Text(XSTR(TH14_RECORD_E), i++, record.stage, record.frame, *(uint32_t*)&(record.value),
                                    TH14_CORRECTION[tempFix] == TH14_CORRECTION_E_POSITIVE ? "" : "-", tempE);
                            } else {
                                ImGui::Text(XSTR(TH14_RECORD), i++, record.stage, record.frame, *(uint32_t*)&(record.value));
                                ImGui::SameLine(0.0f, 0.0f);
                                ImGui::Text(XSTR(TH14_CORRECTION[record.fix]));
                            }
                        }
                    } else {
                        ImGui::Text(XSTR(TH14_REPLAY_NO_RECORDS));
                    }
                } else {
                    ImGui::Text(XSTR(TH14_SELECTED_NONE));
                }
                // TODO: Replay info
            } else if (thMarisaLaser->mState == 3) {
                ImGui::Text(XSTR(TH14_MODE_RECOVER_DESC));

                if (ImGui::Button(XSTR(TH14_SAVE))) {
                    if (mRepName[0]) {
                        std::wstring rep;
                        rep = mRepDir;
                        rep += mRepName;
                        SaveReplayInternal(rep.c_str());
                    }
                }
                ImGui::SameLine();
                if (ImGui::Button(XSTR(TH14_LOAD)) && mRepName[0])
                    thMarisaLaser->MergePbAndRec();
                ImGui::SameLine();
                if (ImGui::Button(XSTR(TH14_CLEAR_RECORDS)))
                    thMarisaLaser->mRecordsRecover.clear();
                ImGui::SameLine();
                if (mRepName[0])
                    ImGui::Text(XSTR(TH14_SELECTED_REPLAY), mRepName);
                else
                    ImGui::Text(XSTR(TH14_SELECTED_NONE));

                ImGui::Separator();
                auto it = thMarisaLaser->mRecordsRecover.begin();
                if (it == thMarisaLaser->mRecordsRecover.end())
                    ImGui::Text(XSTR(TH14_NO_RECORDS));
                else {
                    int i = 1;
                    for (; it != thMarisaLaser->mRecordsRecover.end(); ++i) {
                        ImGui::Text(XSTR(TH14_RECORD), i, it->stage, it->frame, *(uint32_t*)&(it->value));

                        int tempFix = it->fix & 0x0000ffff;
                        int tempE = (it->fix & 0xffff0000) >> 16;

                        ImGui::SameLine();
                        auto needInputBox = TH14_CORRECTION[tempFix] == TH14_CORRECTION_E_POSITIVE || TH14_CORRECTION[tempFix] == TH14_CORRECTION_E_NEGATIVE;
                        if (needInputBox)
                            ImGui::PushItemWidth(GetRelWidth(0.13f));
                        else
                            ImGui::PushItemWidth(GetRelWidth(0.27f));
                        sprintf_s(mTempStr, "##%s_%d", XSTR(TH14_CORRECTION_VALUE), i);
                        ImGui::ComboSectionsDefault(mTempStr, &tempFix, TH14_CORRECTION, XITEMS, "");
                        if (ImGui::IsPopupOpen(mTempStr)) {
                            wndFocus = false;
                        }
                        if (needInputBox) {
                            ImGui::SameLine();
                            sprintf_s(mTempStr, "##e_%d", i);
                            ImGui::InputInt(mTempStr, &tempE, 1, 1, ImGuiInputTextFlags_CharsDecimal);
                            if (tempE > 38)
                                tempE = 38;
                            else if (tempE < 0)
                                tempE = 0;
                        }
                        it->fix = tempFix | (tempE << 16);
                        ImGui::PopItemWidth();

                        ImGui::SameLine();
                        sprintf_s(mTempStr, "%s##%d", XSTR(TH14_DELETE), i);
                        if (ImGui::Button(mTempStr))
                            it = thMarisaLaser->mRecordsRecover.erase(it);
                        else
                            ++it;
                    }
                }
            }
            ImGui::PopTextWrapPos();

            return wndFocus;
        }
        void MsgBox(UINT type, const wchar_t* title, const wchar_t* msg, const wchar_t* msg2 = nullptr)
        {
            std::wstring _msg = msg;
            if (msg2) {
                _msg += msg2;
            }
            MessageBoxW(NULL, _msg.c_str(), _title, type);
        }
        void MsgBox(UINT type, const char* title, const char* msg, const char* msg2 = nullptr)
        {
            wchar_t _title[256];
            wchar_t _msg[256];
            wchar_t _msg2[256];
            MultiByteToWideChar(CP_UTF8, 0, title, -1, _title, 256);
            MultiByteToWideChar(CP_UTF8, 0, msg, -1, _msg, 256);
            if (msg2) {
                MultiByteToWideChar(CP_UTF8, 0, msg2, -1, _msg2, 256);
            }
            MsgBox(type, _title, _msg, msg2 ? _msg2 : nullptr);
            
        }
        bool LoadReplayInternal(const wchar_t* rep_path)
        {
            auto thMarisaLaser = &THMarisaLaser::singleton();
            DWORD repMagic, bytesRead;
            auto repFile = CreateFileW(rep_path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
            if (repFile == INVALID_HANDLE_VALUE)
                return false;
            defer(CloseHandle(repFile));

            SetFilePointer(repFile, 0, NULL, FILE_BEGIN);

            if (ReadFile(repFile, &repMagic, 4, &bytesRead, NULL) && bytesRead == 4) {
                DWORD userPtr, userMagic, userLength, userNo;
                SetFilePointer(repFile, 12, NULL, FILE_BEGIN);
                if (ReadFile(repFile, &userPtr, 4, &bytesRead, NULL)  && bytesRead == 4) {
                    SetFilePointer(repFile, userPtr, NULL, FILE_BEGIN);
                    while (true) {
                        if (!ReadFile(repFile, &userMagic, 4, &bytesRead, NULL) || bytesRead != 4 || userMagic != 'RESU')
                            break;
                        if (!ReadFile(repFile, &userLength, 4, &bytesRead, NULL) || bytesRead != 4)
                            break;
                        if (!ReadFile(repFile, &userNo, 4, &bytesRead, NULL) || bytesRead != 4)
                            break;

                        if (userNo == 'RCER') {
                            if (userLength - 12 > 0 && (userLength - 12) % sizeof(THMarisaLaser::record_t) == 0) {
                                void* dataBuffer = malloc(userLength - 12);
                                if (!dataBuffer)
                                    return false;
                                defer(free(dataBuffer));
                                memset(dataBuffer, 0, userLength - 12);
                                if (ReadFile(repFile, dataBuffer, userLength - 12, &bytesRead, NULL) && (bytesRead == userLength - 12)) {
                                    THMarisaLaser::record_t* p_rec = (THMarisaLaser::record_t*)dataBuffer;
                                    size_t count = (userLength - 12) / sizeof(THMarisaLaser::record_t);
                                    thMarisaLaser->mRecordsPlayback.clear();
                                    for (size_t i = 0; i < count; ++i, ++p_rec)
                                        thMarisaLaser->mRecordsPlayback.push_back(*p_rec);
                                }
                                return bytesRead == userLength - 12;
                            }
                        } else {
                            SetFilePointer(repFile, userLength - 12, NULL, FILE_CURRENT);
                        }
                    }
                }
            }

            return false;
        }
        bool SaveReplayInternal(const wchar_t* rep_path)
        {
            auto thMarisaLaser = &THMarisaLaser::singleton();
            DWORD bytesProcessed;
            auto repFile = CreateFileW(rep_path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
            if (repFile == INVALID_HANDLE_VALUE) {
                MsgBox(MB_ICONERROR | MB_OK, XSTR(TH14_ERROR), XSTR(TH14_ERROR_SRC));
                return false;
            }
            auto repSize = GetFileSize(repFile, NULL);
            auto repBuffer = malloc(repSize);
            if (!repBuffer) {
                CloseHandle(repFile);
                return false;
            }
            if (!ReadFile(repFile, repBuffer, repSize, &bytesProcessed, NULL)) {
                CloseHandle(repFile);
                free(repBuffer);
                return false;
            }
            CloseHandle(repFile);

            std::list<THMarisaLaser::record_t>* records = (thMarisaLaser->mState == 3) ? &thMarisaLaser->mRecordsRecover : &thMarisaLaser->mRecordsNormal;
            auto dataSize = records->size() * sizeof(THMarisaLaser::record_t) + 12;
            auto dataBuffer = malloc(dataSize);
            if (!dataBuffer)
                return false;
            defer(free(dataBuffer));
            memset(dataBuffer, 0, dataSize);
            *(uint32_t*)((int)dataBuffer) = 'RESU';
            *(uint32_t*)((int)dataBuffer + 4) = dataSize;
            *(uint32_t*)((int)dataBuffer + 8) = 'RCER';
            THMarisaLaser::record_t* p_rec = (THMarisaLaser::record_t*)((int)dataBuffer + 12);
            auto i_rec = records->begin();
            for (unsigned int i = 0; i < records->size(); ++i, ++p_rec, ++i_rec)
                *p_rec = *i_rec;

            OPENFILENAMEW ofn;
            wchar_t szFile[MAX_PATH] = L"th14_ud----.rpy";
            ZeroMemory(&ofn, sizeof(ofn));
            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = NULL;
            ofn.lpstrFile = szFile;
            ofn.nMaxFile = MAX_PATH;
            ofn.lpstrFilter = L"Replay File\0*.rpy\0";
            ofn.nFilterIndex = 1;
            ofn.lpstrFileTitle = NULL;
            ofn.nMaxFileTitle = 0;
            ofn.lpstrInitialDir = mRepDir.c_str();
            ofn.lpstrDefExt = L".rpy";
            ofn.Flags = OFN_OVERWRITEPROMPT;
            if (GetSaveFileNameW(&ofn)) {
                auto outputFile = CreateFileW(szFile, GENERIC_READ | GENERIC_WRITE, NULL, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
                if (outputFile == INVALID_HANDLE_VALUE) {
                    MsgBox(MB_ICONERROR | MB_OK, XSTR(TH14_ERROR), XSTR(TH14_ERROR_DEST));
                    return false;
                }
                SetFilePointer(outputFile, 0, NULL, FILE_BEGIN);
                SetEndOfFile(outputFile);
                WriteFile(outputFile, repBuffer, repSize, &bytesProcessed, NULL);
                if (records->size())
                    WriteFile(outputFile, dataBuffer, dataSize, &bytesProcessed, NULL);
                CloseHandle(outputFile);

                MsgBox(MB_ICONINFORMATION | MB_OK, utf8_to_utf16(XSTR(TH14_SUCCESS)).c_str(), utf8_to_utf16(XSTR(TH14_SUCCESS_SAVED)).c_str(), szFile);
            }

            return true;
        }

        THAdvOptWnd() noexcept
        {
            SetWndFlag(ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove);
            SetFade(0.8f, 0.8f);
            SetStyle(ImGuiStyleVar_WindowRounding, 0.0f);
            SetStyle(ImGuiStyleVar_WindowBorderSize, 0.0f);

            InitUpdFunc([&]() { ContentUpdate(); },
                [&]() { LocaleUpdate(); },
                [&]() { PreUpdate(); },
                []() {});

            OnLocaleChange();
            FpsInit();
            GameplayInit();
            DatRecInit();
            MarisaLaserInit();
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

        __declspec(noinline) void ToggleLock(bool lock)
        {
            auto thMarisaLaser = &THMarisaLaser::singleton();
            mLock = lock;
            th14_marisa_laser.Toggle(lock && thMarisaLaser->mState);
            if (lock) {
                thMarisaLaser->mRecordsNormal.clear();
                thMarisaLaser->mPrevRecord.Clear();
            }
        }
        __declspec(noinline) void SelectReplay()
        {
            uint32_t index = GetMemContent(0x4db6a4, 0x5aec);
            std::wstring repName = mb_to_utf16((char*)GetMemAddr(0x4db6a4, index * 4 + 0x5af4, 0x220));
            wcscpy_s(mRepName, repName.c_str());

            std::wstring rep = mRepDir;
            rep += repName;
            LoadReplayInternal(rep.c_str());
        }
        __declspec(noinline) void DeselectReplay()
        {
            auto thMarisaLaser = &THMarisaLaser::singleton();
            mRepName[0] = 0;
            thMarisaLaser->mRecordsPlayback.clear();
        }
        __declspec(noinline) void SaveReplay(const wchar_t* rep_path)
        {
            auto thMarisaLaser = &THMarisaLaser::singleton();
            if (thMarisaLaser->mState == 1 && *(int32_t*)0x4f5828 == 1 && thMarisaLaser->mRecordsNormal.size())
                SaveReplayInternal(rep_path);
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
            bool wndFocus = true;

            ImGui::Text(XSTR(TH_ADV_OPT));
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
            if (BeginOptGroup<TH_DATANLY>()) {
                if (ImGui::Button(XSTR(TH_DATANLY_BUTTON))) {
                    SetContentUpdFunc([&]() { DataRecMenu(); });
                }
                EndOptGroup();
            }
            if (BeginOptGroup<TH14_MARISA_LASER>()) {
                wndFocus &= MarisaLaserMenu();
                EndOptGroup();
            }

            AboutOpt("Plus & InceRabbit (for making the video about Marisa's replay desync bug, and for sharing the replay)\n......and you!");
            ImGui::EndChild();
            if (wndFocus)
                ImGui::SetWindowFocus();
        }
        void PreUpdate()
        {
            DataRecPreUpd();
        }

        adv_opt_ctx mOptCtx;
        int mMode = 0;
        char mTempStr[128];
        bool mRecording = false;
        bool mLock = false;
        wchar_t mRepName[64];
        std::wstring mRepDir;
        wchar_t _title[256];
        wchar_t _msg[256];
        wchar_t _msg2[256];
    };

    void ECLJump(ECLHelper& ecl, unsigned int start, unsigned int dest, int at_frame, int ecl_time = 0)
    {
        ecl.SetPos(start);
        ecl << ecl_time << 0x0018000C << 0x02ff0000 << 0x00000000 << dest - start << at_frame;
    }
    __declspec(noinline) void THStageWarp(ECLHelper& ecl, int stage, int portion)
    {
        if (stage == 1) {
            switch (portion) {
            case 1:
                break;
            case 2:
                ECLJump(ecl, 0x689c, 0x6ae8, 60, 90); // 0x6b4c
                ECLJump(ecl, 0x40e4, 0x4118, 0, 0);
                break;
            case 3:
                ECLJump(ecl, 0x689c, 0x6ae8, 60, 90); // 0x6b4c
                ECLJump(ecl, 0x40e4, 0x414c, 0, 0);
                break;
            case 4:
                ECLJump(ecl, 0x689c, 0x6b4c, 60, 90);
                break;
            case 5:
                ECLJump(ecl, 0x689c, 0x6b4c, 60, 90);
                ECLJump(ecl, 0x41e8, 0x4218, 0, 0);
                break;
            default:
                break;
            }
        } else if (stage == 2) {
            switch (portion) {
            case 1:
                break;
            case 2:
                ECLJump(ecl, 0x4f30, 0x51c4, 60, 90); // 0x51e4, 0x5218
                ECLJump(ecl, 0x3004, 0x3038, 0, 0);
                break;
            case 3:
                ECLJump(ecl, 0x4f30, 0x51c4, 60, 90); // 0x51e4, 0x5218
                ECLJump(ecl, 0x3004, 0x306c, 0, 0);
                break;
            case 4:
                ECLJump(ecl, 0x4f30, 0x51e4, 60, 90); // 0x5218
                ECLJump(ecl, 0x31c8, 0x3220, 0, 0);
                break;
            case 5:
                ECLJump(ecl, 0x4f30, 0x5218, 60, 30);
                break;
            case 6:
                ECLJump(ecl, 0x4f30, 0x5218, 60, 90);
                ECLJump(ecl, 0x3118, 0x3160, 0, 0);
                break;
            default:
                break;
            }
        } else if (stage == 3) {
            switch (portion) {
            case 1:
                break;
            case 2:
                ECLJump(ecl, 0x648c, 0x6720, 60, 90); // 0x6740, 0x6774
                ECLJump(ecl, 0x21f4, 0x2228, 0, 0);
                break;
            case 3:
                ECLJump(ecl, 0x648c, 0x6720, 60, 30); // 0x6740, 0x6774
                ECLJump(ecl, 0x21f4, 0x2290, 0, 0);
                break;
            case 4:
                ECLJump(ecl, 0x648c, 0x6740, 60, 90); // 0x6774
                ECLJump(ecl, 0x23c4, 0x241c, 0, 0);
                break;
            case 5:
                ECLJump(ecl, 0x648c, 0x6774, 60, 30);
                break;
            case 6:
                ECLJump(ecl, 0x648c, 0x6774, 60, 90);
                ECLJump(ecl, 0x22f8, 0x233c, 0, 0);
                break;
            default:
                break;
            }
        } else if (stage == 4) {
            switch (portion) {
            case 1:
                break;
            case 2:
                ECLJump(ecl, 0x97f8, 0x9ab4, 60, 90); // 0x9ae4, 0x9b18
                ECLJump(ecl, 0x5090, 0x50c4, 0, 0);
                break;
            case 3:
                ECLJump(ecl, 0x97f8, 0x9ab4, 60, 90); // 0x9ae4, 0x9b18
                ECLJump(ecl, 0x5090, 0x50f8, 0, 0);
                break;
            case 4:
                ECLJump(ecl, 0x97f8, 0x9ab4, 60, 90); // 0x9ae4, 0x9b18
                ECLJump(ecl, 0x5090, 0x512c, 0, 0);
                break;
            case 5:
                ECLJump(ecl, 0x97f8, 0x9ae4, 60, 90); // 0x9b18
                ECLJump(ecl, 0x5240, 0x53c0, 0, 0);
                break;
            case 6:
                ECLJump(ecl, 0x97f8, 0x9b18, 60, 30);
                break;
            case 7:
                ECLJump(ecl, 0x97f8, 0x9b18, 60, 90);
                ECLJump(ecl, 0x5194, 0x51d8, 0, 0);
                break;
            default:
                break;
            }
        } else if (stage == 5) {
            switch (portion) {
            case 1:
                break;
            case 2:
                ECLJump(ecl, 0x8654, 0x8910, 60, 90); // 0x8940, 0x8988
                ECLJump(ecl, 0x2784, 0x27b8, 0, 0);
                break;
            case 3:
                ECLJump(ecl, 0x8654, 0x8910, 60, 90); // 0x8940, 0x8988
                ECLJump(ecl, 0x2784, 0x27ec, 0, 0);
                break;
            case 4:
                ECLJump(ecl, 0x8654, 0x8910, 60, 90); // 0x8940, 0x8988
                ECLJump(ecl, 0x2784, 0x2820, 0, 0);
                break;
            case 5:
                ECLJump(ecl, 0x8654, 0x8988, 60, 90);
                break;
            case 6:
                ECLJump(ecl, 0x8654, 0x8988, 60, 90);
                ECLJump(ecl, 0x2888, 0x28dc, 0, 0);
                break;
            default:
                break;
            }
        } else if (stage == 6) {
            switch (portion) {
            case 1:
                break;
            case 2:
                ECLJump(ecl, 0x79d0, 0x7bb8, 60, 90);
                ECLJump(ecl, 0x2bc0, 0x2bf4, 0, 0);
                break;
            case 3:
                ECLJump(ecl, 0x79d0, 0x7bb8, 60, 90);
                ECLJump(ecl, 0x2bc0, 0x2c28, 0, 0);
                break;
            case 4:
                ECLJump(ecl, 0x79d0, 0x7bb8, 60, 90);
                ECLJump(ecl, 0x2bc0, 0x2c5c, 0, 0);
                break;
            default:
                break;
            }
        } else if (stage == 7) {
            switch (portion) {
            case 1:
                break;
            case 2:
                ECLJump(ecl, 0x7778, 0x7a34, 60, 90); // 0x7aac, 0x7acc
                ECLJump(ecl, 0x36ac, 0x36e0, 0, 0);
                break;
            case 3:
                ECLJump(ecl, 0x7778, 0x7a34, 60, 50); // 0x7aac, 0x7acc
                ECLJump(ecl, 0x36ac, 0x3714, 0, 0);
                break;
            case 4:
                ECLJump(ecl, 0x7778, 0x7a34, 60, 90); // 0x7aac, 0x7acc
                ECLJump(ecl, 0x36ac, 0x3748, 0, 0);
                break;
            case 5:
                ECLJump(ecl, 0x7778, 0x7aac, 60, 90); // 0x7acc
                ecl << pair(0x4cf8, 0);
                break;
            case 6:
                ECLJump(ecl, 0x7778, 0x7acc, 60, 90);
                break;
            case 7:
                ECLJump(ecl, 0x7778, 0x7acc, 60, 10);
                ECLJump(ecl, 0x37b0, 0x37f4, 0, 0);
                break;
            case 8:
                ECLJump(ecl, 0x7778, 0x7acc, 60, 90);
                ECLJump(ecl, 0x37b0, 0x3828, 0, 0);
                break;
            case 9:
                ECLJump(ecl, 0x7778, 0x7acc, 60, 90);
                ECLJump(ecl, 0x37b0, 0x385c, 0, 0);
                break;
            default:
                break;
            }
        }
    }
    __declspec(noinline) void THPatch(ECLHelper& ecl, th_sections_t section)
    {
        auto st6_boss = [&](int std_id) {
            ECLJump(ecl, 0x79d0, 0x2aa0, 0); // Jump to 630
            ecl << pair(0x2ab0, std_id); // 630 Param
            ECLJump(ecl, 0x2ab4, 0x7c68, 60); // Jump to MainBoss
            ecl.SetFile(3);
            ecl << pair(0x4a0, (int16_t)0); // Disable 630
        };

        switch (section) {
        case THPrac::TH14::TH14_ST1_MID1:
            ECLJump(ecl, 0x689c, 0x6b08, 60);
            break;
        case THPrac::TH14::TH14_ST1_MID2:
            ECLJump(ecl, 0x689c, 0x6b08, 60);
            ecl.SetFile(3);
            ECLJump(ecl, 0x478, 0x560, 0);
            ecl << pair(0x38c, (int16_t)0);
            break;
        case THPrac::TH14::TH14_ST1_BOSS1:
            ECLJump(ecl, 0x689c, 0x6bbc, 60);
            break;
        case THPrac::TH14::TH14_ST1_BOSS2:
            ECLJump(ecl, 0x689c, 0x6bbc, 60);
            ecl.SetFile(2);
            ECLJump(ecl, 0x400, 0x4e8, 0); // Utilize Spell Practice Jump
            ecl << pair(0x4f8, 1500); // Set Health
            ecl << pair(0x518, (int8_t)0x31); // Set Spell Ordinal
            ecl << pair(0x1aa4, (int16_t)0); // Disable Item Drops
            break;
        case THPrac::TH14::TH14_ST1_BOSS3:
            ECLJump(ecl, 0x689c, 0x6bbc, 60);
            ecl.SetFile(2);
            ecl << pair(0x638, (int8_t)0x32); // Change Nonspell
            ecl << pair(0x1028, (int16_t)0) << pair(0x1158, (int16_t)0); // Disable Item Drops & SE
            ecl << pair(0x12e4, 0) << pair(0x12d0, 0); // Change Wait Time & Inv. Time
            break;
        case THPrac::TH14::TH14_ST1_BOSS4:
            ECLJump(ecl, 0x689c, 0x6bbc, 60);
            ecl.SetFile(2);
            ECLJump(ecl, 0x400, 0x4e8, 0); // Utilize Spell Practice Jump
            ecl << pair(0x4f8, 1900); // Set Health
            ecl << pair(0x518, (int8_t)0x32); // Set Spell Ordinal
            ecl << pair(0x2cbc, (int16_t)0); // Disable Item Drops
            break;
        case THPrac::TH14::TH14_ST2_MID1:
            ECLJump(ecl, 0x4f30, 0x51e4, 60);
            break;
        case THPrac::TH14::TH14_ST2_MID2:
            ECLJump(ecl, 0x4f30, 0x51e4, 60);
            ecl.SetFile(2);
            ECLJump(ecl, 0x3fc, 0x4e4, 0);
            ecl << pair(0xdf8, (int16_t)0);
            break;
        case THPrac::TH14::TH14_ST2_BOSS1:
            ECLJump(ecl, 0x4f30, 0x5274, 60);
            break;
        case THPrac::TH14::TH14_ST2_BOSS2:
            ECLJump(ecl, 0x4f30, 0x5274, 60);
            ecl.SetFile(3);
            ECLJump(ecl, 0x4e4, 0x5cc, 0); // Utilize Spell Practice Jump
            ecl << pair(0x5dc, 1400); // Set Health
            ecl << pair(0x5fc, (int8_t)0x31); // Set Spell Ordinal
            ecl << pair(0x2678, (int16_t)0); // Disable Item Drops
            break;
        case THPrac::TH14::TH14_ST2_BOSS3:
            ECLJump(ecl, 0x4f30, 0x5274, 60);
            ecl.SetFile(3);
            ecl << pair(0x800, (int8_t)0x32); // Change Nonspell
            ecl << pair(0x13d4, (int16_t)0) << pair(0x1518, (int16_t)0); // Disable Item Drops & SE
            //ecl << pair(0x12e4, 0) << pair(0x12d0, 0); // Change Wait Time & Inv. Time
            break;
        case THPrac::TH14::TH14_ST2_BOSS4:
            ECLJump(ecl, 0x4f30, 0x5274, 60);
            ecl.SetFile(3);
            ECLJump(ecl, 0x4e4, 0x5cc, 0); // Utilize Spell Practice Jump
            ecl << pair(0x5dc, 1900); // Set Health
            ecl << pair(0x5fc, (int8_t)0x32); // Set Spell Ordinal
            ecl << pair(0x3410, (int16_t)0); // Disable Item Drops
            break;
        case THPrac::TH14::TH14_ST2_BOSS5:
            ECLJump(ecl, 0x4f30, 0x5274, 60);
            ecl.SetFile(3);
            ECLJump(ecl, 0x4e4, 0x5cc, 0); // Utilize Spell Practice Jump
            ecl << pair(0x5dc, 2100); // Set Health
            ecl << pair(0x5fc, (int8_t)0x33); // Set Spell Ordinal
            //ecl << pair(0x1aa4, (int16_t)0); // Disable Item Drops
            break;
        case THPrac::TH14::TH14_ST3_MID1:
            ECLJump(ecl, 0x648c, 0x6740, 60);
            break;
        case THPrac::TH14::TH14_ST3_MID2_EN:
            ECLJump(ecl, 0x648c, 0x6740, 60);
            ecl.SetFile(2);
            ECLJump(ecl, 0x520, 0x614, 0);
            ECLJump(ecl, 0x664, 0xc88, 0);
            ecl << pair(0xcc8, (int16_t)0) << pair(0xe24, (int16_t)0); // Disable Item Drops & SE
            ecl << pair(0xf68, 0) << pair(0xf54, 0); // Change Wait Time & Inv. Time
            break;
        case THPrac::TH14::TH14_ST3_MID2_HL:
            ECLJump(ecl, 0x648c, 0x6740, 60);
            ecl.SetFile(2);
            ECLJump(ecl, 0x404, 0x4ec, 0);
            break;
        case THPrac::TH14::TH14_ST3_BOSS1:
            ECLJump(ecl, 0x648c, 0x67d0, 60);
            break;
        case THPrac::TH14::TH14_ST3_BOSS2:
            ECLJump(ecl, 0x648c, 0x67d0, 60);
            ecl.SetFile(3);
            ECLJump(ecl, 0x458, 0x540, 0); // Utilize Spell Practice Jump
            ecl << pair(0x550, 1600); // Set Health
            ecl << pair(0x570, (int8_t)0x31); // Set Spell Ordinal
            ecl << pair(0x20f0, (int16_t)0); // Disable Item Drops
            break;
        case THPrac::TH14::TH14_ST3_BOSS3:
            ECLJump(ecl, 0x648c, 0x67d0, 60);
            ecl.SetFile(3);
            ecl << pair(0x774, (int8_t)0x32); // Change Nonspell
            ecl << pair(0xe7c, (int16_t)0) << pair(0xfc0, (int16_t)0) << pair(0x1134, (int16_t)0); // Disable Item Drops & SE
            ecl << pair(0x10b0, 59) << pair(0x1154, 20) << pair(0x112c, 0); // Change Move Time, Wait Time & Inv. Time
            break;
        case THPrac::TH14::TH14_ST3_BOSS4:
            ECLJump(ecl, 0x648c, 0x67d0, 60);
            ecl.SetFile(3);
            ECLJump(ecl, 0x458, 0x540, 0); // Utilize Spell Practice Jump
            ecl << pair(0x550, 2100); // Set Health
            ecl << pair(0x570, (int8_t)0x32); // Set Spell Ordinal
            ecl << pair(0x38c4, (int16_t)0); // Disable Item Drops
            break;
        case THPrac::TH14::TH14_ST3_BOSS5:
            ECLJump(ecl, 0x648c, 0x67d0, 60);
            ecl.SetFile(3);
            ecl << pair(0x774, (int8_t)0x33); // Change Nonspell
            ecl << pair(0x1790, (int16_t)0) << pair(0x18d4, (int16_t)0) << pair(0x1a48, (int16_t)0); // Disable Item Drops & SE
            ecl << pair(0x19c4, 59) << pair(0x1a68, 20) << pair(0x1a40, 0); // Change Move Time, Wait Time & Inv. Time
            break;
        case THPrac::TH14::TH14_ST3_BOSS6:
            ECLJump(ecl, 0x648c, 0x67d0, 60);
            ecl.SetFile(3);
            ECLJump(ecl, 0x458, 0x540, 0); // Utilize Spell Practice Jump
            ecl << pair(0x550, 1800); // Set Health
            ecl << pair(0x570, (int8_t)0x33); // Set Spell Ordinal
            ecl << pair(0x47e4, (int16_t)0); // Disable Item Drops
            break;
        case THPrac::TH14::TH14_ST4_MID1:
            ECLJump(ecl, 0x97f8, 0x9ae4, 60);
            break;
        case THPrac::TH14::TH14_ST4_MID2:
            ECLJump(ecl, 0x97f8, 0x9ae4, 60);

            // BossA
            ecl.SetFile(2);
            ecl << pair(0x439, (int8_t)0x32);
            ecl << pair(0x143c, (int16_t)0) << pair(0x1598, (int16_t)0);
            ecl << pair(0x16c8, 0) << pair(0x16dc, (int16_t)0);

            // BossB
            ecl.SetFile(3);
            ecl << pair(0x542, (int8_t)0x32);
            ecl << pair(0xca8, (int16_t)0) << pair(0xe04, (int16_t)0);
            ecl << pair(0xf34, 0) << pair(0xf48, (int16_t)0);
            break;
        case THPrac::TH14::TH14_ST4_BOSS1:
            ECLJump(ecl, 0x97f8, 0x9b74, 60);

            // BossA
            ecl.SetFile(4);
            ECLJump(ecl, 0x9950, 0x9ab4, 0); // Skip Dummy Boss
            ecl << pair(0x3e8, 0); // Cancel Msg Invi.

            // BossB
            ecl.SetFile(5);
            ECLJump(ecl, 0x4dcc, 0x4f30, 0); // Skip Dummy Boss
            ecl << pair(0x358, 0); // Cancel Msg Invi.
            break;
        case THPrac::TH14::TH14_ST4_BOSS2:
            ECLJump(ecl, 0x97f8, 0x9b74, 60);

            // BossA
            ecl.SetFile(4);
            ECLJump(ecl, 0x9950, 0x9ab4, 0); // Skip Dummy Boss
            //ecl << pair(0x3e8, 0); // Cancel Msg Invi.
            ECLJump(ecl, 0x4e8, 0x5d0, 0); // Utilize Spell Practice Jump
            ecl << pair(0x5e0, 2300); // Set Health
            ecl << pair(0x600, (int8_t)0x31); // Set Spell Ordinal
            ecl << pair(0x6370, (int16_t)0); // Disable Item Drops

            // BossB
            ecl.SetFile(5);
            ECLJump(ecl, 0x4dcc, 0x4f30, 0); // Skip Dummy Boss
            //ecl << pair(0x358, 0); // Cancel Msg Invi.
            ECLJump(ecl, 0x458, 0x540, 0); // Utilize Spell Practice Jump
            ecl << pair(0x550, 2300); // Set Health
            ecl << pair(0x571, (int8_t)0x31); // Set Spell Ordinal
            ecl << pair(0x1e98, (int16_t)0); // Disable Item Drops
            break;
        case THPrac::TH14::TH14_ST4_BOSS3:
            ECLJump(ecl, 0x97f8, 0x9b74, 60);

            // BossA
            ecl.SetFile(4);
            ECLJump(ecl, 0x9950, 0x9ab4, 0); // Skip Dummy Boss
            ecl << pair(0x3e8, 0); // Cancel Msg Invi.
            ecl << pair(0x804, (int8_t)0x32); // Change Nonspell
            ecl << pair(0x24e4, (int16_t)0) << pair(0x2628, (int16_t)0); // Disable Item Drops & SE
            ecl << pair(0x2718, 59) << pair(0x2794, 0); // Change Move Time & Wait Time

            // BossB
            ecl.SetFile(5);
            ECLJump(ecl, 0x4dcc, 0x4f30, 0); // Skip Dummy Boss
            ecl << pair(0x358, 0); // Cancel Msg Invi.
            ecl << pair(0x775, (int8_t)0x32); // Change Nonspell
            ecl << pair(0xf0c, (int16_t)0) << pair(0x1050, (int16_t)0) << pair(0x11e4, (int16_t)0); // Disable Item Drops & SE
            ecl << pair(0x1140, 59) << pair(0x1204, 0) << pair(0x11dc, 0); // Change Move Time, Wait Time & Inv. Time
            break;
        case THPrac::TH14::TH14_ST4_BOSS4:
            ECLJump(ecl, 0x97f8, 0x9b74, 60);

            // BossA
            ecl.SetFile(4);
            ECLJump(ecl, 0x9950, 0x9ab4, 0); // Skip Dummy Boss
            //ecl << pair(0x3e8, 0); // Cancel Msg Invi.
            ECLJump(ecl, 0x4e8, 0x5d0, 0); // Utilize Spell Practice Jump
            ecl << pair(0x5e0, 2300); // Set Health
            ecl << pair(0x600, (int8_t)0x32); // Set Spell Ordinal
            ecl << pair(0x6c98, (int16_t)0); // Disable Item Drops

            // BossB
            ecl.SetFile(5);
            ECLJump(ecl, 0x4dcc, 0x4f30, 0); // Skip Dummy Boss
            //ecl << pair(0x358, 0); // Cancel Msg Invi.
            ECLJump(ecl, 0x458, 0x540, 0); // Utilize Spell Practice Jump
            ecl << pair(0x550, 2300); // Set Health
            ecl << pair(0x571, (int8_t)0x32); // Set Spell Ordinal
            ecl << pair(0x2ddc, (int16_t)0); // Disable Item Drops
            break;
        case THPrac::TH14::TH14_ST4_BOSS5:
            ECLJump(ecl, 0x97f8, 0x9b74, 60);

            // BossA
            ecl.SetFile(4);
            ECLJump(ecl, 0x9950, 0x9ab4, 0); // Skip Dummy Boss
            ecl << pair(0x3e8, 0); // Cancel Msg Invi.
            ecl << pair(0x804, (int8_t)0x33); // Change Nonspell
            ecl << pair(0x44dc, (int16_t)0) << pair(0x4634, (int16_t)0); // Disable Item Drops & SE
            ecl << pair(0x4724, 59) << pair(0x478c, 0); // Change Move Time & Wait Time

            // BossB
            ecl.SetFile(5);
            ECLJump(ecl, 0x4dcc, 0x4f30, 0); // Skip Dummy Boss
            ecl << pair(0x358, 0); // Cancel Msg Invi.
            ecl << pair(0x775, (int8_t)0x33); // Change Nonspell
            ecl << pair(0x1678, (int16_t)0) << pair(0x17dc, (int16_t)0) << pair(0x1950, (int16_t)0); // Disable Item Drops & SE
            ecl << pair(0x18cc, 59) << pair(0x1948, 0) << pair(0x1970, 0); // Change Move Time, Wait Time & Inv. Time
            break;
        case THPrac::TH14::TH14_ST4_BOSS6:
            ECLJump(ecl, 0x97f8, 0x9b74, 60);

            // BossA
            ecl.SetFile(4);
            ECLJump(ecl, 0x9950, 0x9ab4, 0); // Skip Dummy Boss
            //ecl << pair(0x3e8, 0); // Cancel Msg Invi.
            ECLJump(ecl, 0x4e8, 0x5d0, 0); // Utilize Spell Practice Jump
            ecl << pair(0x5e0, 2800); // Set Health
            ecl << pair(0x600, (int8_t)0x33); // Set Spell Ordinal
            ecl << pair(0x7db4, (int16_t)0); // Disable Item Drops

            // BossB
            ecl.SetFile(5);
            ECLJump(ecl, 0x4dcc, 0x4f30, 0); // Skip Dummy Boss
            //ecl << pair(0x358, 0); // Cancel Msg Invi.
            ECLJump(ecl, 0x458, 0x540, 0); // Utilize Spell Practice Jump
            ecl << pair(0x550, 2500); // Set Health
            ecl << pair(0x571, (int8_t)0x33); // Set Spell Ordinal
            ecl << pair(0x3598, (int16_t)0); // Disable Item Drops
            break;
        case THPrac::TH14::TH14_ST5_MID1:
            ECLJump(ecl, 0x8654, 0x8940, 60);
            break;
        case THPrac::TH14::TH14_ST5_MID2:
            ECLJump(ecl, 0x8654, 0x8940, 60);
            ecl.SetFile(2);
            ECLJump(ecl, 0x3c4, 0x4ac, 0);
            ecl << pair(0xd38, (int16_t)0);
            break;
        case THPrac::TH14::TH14_ST5_BOSS1:
            ECLJump(ecl, 0x8654, 0x89e4, 60);
            break;
        case THPrac::TH14::TH14_ST5_BOSS2:
            ECLJump(ecl, 0x8654, 0x89e4, 60);
            ecl.SetFile(3);
            ECLJump(ecl, 0x43c, 0x524, 0); // Utilize Spell Practice Jump
            ecl << pair(0x534, 2600); // Set Health
            ecl << pair(0x554, (int8_t)0x31); // Set Spell Ordinal
            ecl << pair(0x334c, (int16_t)0); // Disable Item Drops
            break;
        case THPrac::TH14::TH14_ST5_BOSS3:
            ECLJump(ecl, 0x8654, 0x89e4, 60);
            ecl.SetFile(3);
            ecl << pair(0x83c, (int8_t)0x32); // Change Nonspell
            ecl << pair(0x14b8, (int16_t)0) << pair(0x1638, (int16_t)0) << pair(0x17ac, (int16_t)0); // Disable Item Drops & SE
            ecl << pair(0x1728, 59) << pair(0x17cc, 20) << pair(0x17a4, 0); // Change Move Time, Wait Time & Inv. Time
            break;
        case THPrac::TH14::TH14_ST5_BOSS4:
            ECLJump(ecl, 0x8654, 0x89e4, 60);
            ecl.SetFile(3);
            ECLJump(ecl, 0x43c, 0x524, 0); // Utilize Spell Practice Jump
            ecl << pair(0x534, 2100); // Set Health
            ecl << pair(0x554, (int8_t)0x32); // Set Spell Ordinal
            ecl << pair(0x4350, (int16_t)0); // Disable Item Drops
            break;
        case THPrac::TH14::TH14_ST5_BOSS5:
            ECLJump(ecl, 0x8654, 0x89e4, 60);
            ecl.SetFile(3);
            ecl << pair(0x83c, (int8_t)0x33); // Change Nonspell
            ecl << pair(0x239c, (int16_t)0) << pair(0x2530, (int16_t)0) << pair(0x26c0, (int16_t)0); // Disable Item Drops & SE
            ecl << pair(0x2620, 59) << pair(0x26e0, 20) << pair(0x26b8, 0); // Change Move Time, Wait Time & Inv. Time
            break;
        case THPrac::TH14::TH14_ST5_BOSS6:
            ECLJump(ecl, 0x8654, 0x89e4, 60);
            ecl.SetFile(3);
            ECLJump(ecl, 0x43c, 0x524, 0); // Utilize Spell Practice Jump
            ecl << pair(0x534, 5700); // Set Health
            ecl << pair(0x554, (int8_t)0x33); // Set Spell Ordinal
            ecl << pair(0x59ec, (int16_t)0); // Disable Item Drops

            ECLJump(ecl, 0x538, 0x265c, 0);
            ECLJump(ecl, 0x2694, 0x59d4, 0);
            break;
        case THPrac::TH14::TH14_ST5_BOSS7:
            ECLJump(ecl, 0x8654, 0x89e4, 60);
            ecl.SetFile(3);
            ECLJump(ecl, 0x43c, 0x524, 0); // Utilize Spell Practice Jump
            ecl << pair(0x534, 2700); // Set Health
            ecl << pair(0x554, (int8_t)0x34); // Set Spell Ordinal

            ecl << pair(0x604c, (int16_t)0) << pair(0x5f3c, (int16_t)0) << pair(0x6168, (int16_t)0); // Disable Item Drops & SE
            ecl << pair(0x6094, 59) << pair(0x60c8, 0) << pair(0x60b4, 0); // Change Move Time, Wait Time & Inv. Time
            ecl << pair(0x3b0, (int16_t)0); // Disable 504
            break;
        case THPrac::TH14::TH14_ST6_MID1:
            ECLJump(ecl, 0x79d0, 0x7bd8, 60);
            break;
        case THPrac::TH14::TH14_ST6_MID2:
            ECLJump(ecl, 0x79d0, 0x7bd8, 60);
            ecl.SetFile(2);
            ecl << pair(0x3d5, (int8_t)0x32); // Change Nonspell
            ecl << pair(0xda0, (int16_t)0) << pair(0xee4, (int16_t)0) << pair(0x103c, (int16_t)0); // Disable Item Drops & SE
            ecl << pair(0xfd4, 59) << pair(0x105c, 20) << pair(0x1034, 0); // Change Move Time, Wait Time & Inv. Time
            break;
        case THPrac::TH14::TH14_ST6_BOSS1:
            st6_boss(2);
            break;
        case THPrac::TH14::TH14_ST6_BOSS2:
            st6_boss(2);
            ECLJump(ecl, 0x540, 0x628, 0); // Utilize Spell Practice Jump
            ecl << pair(0x638, 3000); // Set Health
            ecl << pair(0x658, (int8_t)0x31); // Set Spell Ordinal
            ecl << pair(0x4ac0, (int16_t)0); // Disable Item Drops
            break;
        case THPrac::TH14::TH14_ST6_BOSS3:
            st6_boss(3);
            ecl << pair(0xb3c, (int8_t)0x32); // Change Nonspell
            ecl << pair(0x166c, (int16_t)0) << pair(0x17e0, (int16_t)0) << pair(0x1940, (int16_t)0); // Disable Item Drops & SE
            ecl << pair(0x18d0, 59) << pair(0x1960, 0) << pair(0x1938, 60); // Change Move Time, Wait Time & Inv. Time
            ecl << pair(0x16c4, (int16_t)0); // Disable 630
            break;
        case THPrac::TH14::TH14_ST6_BOSS4:
            st6_boss(3);
            ECLJump(ecl, 0x540, 0x628, 0); // Utilize Spell Practice Jump
            ecl << pair(0x638, 2800); // Set Health
            ecl << pair(0x658, (int8_t)0x32); // Set Spell Ordinal
            ecl << pair(0x58bc, (int16_t)0); // Disable Item Drops
            break;
        case THPrac::TH14::TH14_ST6_BOSS5:
            st6_boss(4);
            ecl << pair(0xb3c, (int8_t)0x33); // Change Nonspell
            ecl << pair(0x27cc, (int16_t)0) << pair(0x2924, (int16_t)0) << pair(0x2a98, (int16_t)0); // Disable Item Drops & SE
            ecl << pair(0x2a14, 59) << pair(0x2ab8, 0) << pair(0x2a90, 60); // Change Move Time, Wait Time & Inv. Time
            ecl << pair(0x2808, (int16_t)0); // Disable 630
            break;
        case THPrac::TH14::TH14_ST6_BOSS6:
            st6_boss(4);
            ECLJump(ecl, 0x540, 0x7f0, 0); // Utilize Spell Practice Jump
            ecl << pair(0x6928, (int16_t)0); // Disable Item Drops
            break;
        case THPrac::TH14::TH14_ST6_BOSS7:
            st6_boss(4);
            ecl << pair(0xb3c, (int8_t)0x34); // Change Nonspell
            ecl << pair(0x37f8, (int16_t)0) << pair(0x3970, (int16_t)0) << pair(0x3ae4, (int16_t)0); // Disable Item Drops & SE
            ecl << pair(0x3a60, 59) << pair(0x3b04, 0) << pair(0x3adc, 60); // Change Move Time, Wait Time & Inv. Time
            ecl << pair(0x3834, (int16_t)0); // Disable 630
            ecl << pair(0x385c, (int16_t)0); // Disable BossCupEnd
            break;
        case THPrac::TH14::TH14_ST6_BOSS8:
            st6_boss(4);
            ECLJump(ecl, 0x540, 0x628, 0); // Utilize Spell Practice Jump
            ecl << pair(0x638, 1); // Set Health
            ecl << pair(0x658, (int8_t)0x34); // Set Spell Ordinal
            ecl << pair(0x75e0, (int16_t)0); // Disable Item Drops
            break;
        case THPrac::TH14::TH14_ST6_BOSS9:
            st6_boss(4);
            ECLJump(ecl, 0x540, 0x628, 0); // Utilize Spell Practice Jump
            ecl << pair(0x638, 3000); // Set Health
            ecl << pair(0x658, (int8_t)0x35); // Set Spell Ordinal
            if (thPracParam.phase == 1) {
                ecl.SetPos(0x8cdc);
                ecl << 30 << 30 << 30 << 30;
            }
            break;
        case THPrac::TH14::TH14_ST6_BOSS10:
            st6_boss(4);
            ECLJump(ecl, 0x540, 0x628, 0); // Utilize Spell Practice Jump
            ecl << pair(0x638, 8000); // Set Health
            ecl << pair(0x658, (int8_t)0x36); // Set Spell Ordinal
            ECLJump(ecl, 0x63c, 0x4790, 0);
            ECLJump(ecl, 0x47c8, 0x9c18, 0);
            switch (thPracParam.phase) {
            case 1:
                ecl << pair(0x638, 6500);
                ecl << pair(0x9fb0, 0);
                ECLJump(ecl, 0xa188, 0xa2cc, 120);
                break;
            case 2:
                ecl << pair(0x638, 3300);
                ecl << pair(0x9fb0, 0);
                ECLJump(ecl, 0xa188, 0xa2cc, 120);
                ECLJump(ecl, 0xa4a8, 0xa5ec, 120);
                break;
            case 3:
                ecl << pair(0x638, 3300);
                ecl << pair(0x9fb0, 0);
                ECLJump(ecl, 0xa188, 0xa2cc, 120);
                ECLJump(ecl, 0xa4a8, 0xa5ec, 120);
                ECLJump(ecl, 0xa7b4, 0xa828, 120);
                break;
            default:
                break;
            }
            break;
        case THPrac::TH14::TH14_ST7_MID1:
            ECLJump(ecl, 0x7778, 0x7a64, 60);
            ecl << pair(0x390c, (int16_t)0);
            break;
        case THPrac::TH14::TH14_ST7_MID2:
            ECLJump(ecl, 0x7778, 0x7a64, 60);
            ecl << pair(0x390c, (int16_t)0);
            ecl.SetFile(2);

            ECLJump(ecl, 0x528, 0x590, 0); // Disable Effect
            ecl << pair(0x3b4, -32.0f) << pair(0x3b8, 128.0f); // Change Pos
            ecl << pair(0x801, (int8_t)0x32); // Change Nonspell
            ecl << pair(0x1efc, (int16_t)0) << pair(0x2028, (int16_t)0); // Disable SE
            ecl << pair(0x2048, 0) << pair(0x2020, 0); // Change Wait Time & Inv. Time
            ecl << pair(0x1fe0, (int16_t)0); // Void 401

            ECLJump(ecl, 0x2d80, 0x2de8, 0); // Disable Effect
            ecl << pair(0x2c0c, 32.0f) << pair(0x2c10, 128.0f); // Change Pos
            ecl << pair(0x30b5, (int8_t)0x32); // Change Nonspell
            ecl << pair(0x2260, (int16_t)0); // Disable SE
            ecl << pair(0x2370, 0) << pair(0x2164, 0); // Change Wait Time & Inv. Time
            ecl << pair(0x2344, (int16_t)0); // Void 401
            break;
        case THPrac::TH14::TH14_ST7_MID3:
            ECLJump(ecl, 0x7778, 0x7a64, 60);
            ecl << pair(0x390c, (int16_t)0);
            ecl.SetFile(2);

            ECLJump(ecl, 0x528, 0x590, 0); // Disable Effect
            ecl << pair(0x3b4, -128.0f) << pair(0x3b8, 160.0f); // Change Pos
            ecl << pair(0x801, (int8_t)0x33); // Change Nonspell
            ecl << pair(0x2574, (int16_t)0) << pair(0x26a0, (int16_t)0); // Disable SE
            ecl << pair(0x26c0, 0) << pair(0x2698, 0); // Change Wait Time & Inv. Time
            ecl << pair(0x2658, (int16_t)0); // Void 401

            ECLJump(ecl, 0x2d80, 0x2de8, 0); // Disable Effect
            ecl << pair(0x2c0c, 128.0f) << pair(0x2c10, 160.0f); // Change Pos
            ecl << pair(0x30b5, (int8_t)0x33); // Change Nonspell
            ecl << pair(0x28d8, (int16_t)0); // Disable SE
            ecl << pair(0x29e8, 0) << pair(0x27dc, 0); // Change Wait Time & Inv. Time
            ecl << pair(0x29bc, (int16_t)0); // Void 401
            break;
        case THPrac::TH14::TH14_ST7_END_NS1:
            ECLJump(ecl, 0x7778, 0x7b38, 60);
            break;
        case THPrac::TH14::TH14_ST7_END_S1:
            ECLJump(ecl, 0x7778, 0x7b38, 60);
            ecl.SetFile(3);
            ECLJump(ecl, 0x790, 0x830, 0); // Utilize Spell Practice Jump
            ecl << pair(0x840, 2100); // Set Health
            ecl << pair(0x860, (int8_t)0x31); // Set Spell Ordinal
            ecl << pair(0x691c, (int16_t)0); // Disable Item Drops
            break;
        case THPrac::TH14::TH14_ST7_END_NS2:
            ECLJump(ecl, 0x7778, 0x7b38, 60);
            ecl.SetFile(3);
            ecl << pair(0xf08, (int8_t)0x32); // Change Nonspell
            ecl << pair(0x190c, (int16_t)0) << pair(0x1ab0, (int16_t)0) << pair(0x1c48, (int16_t)0); // Disable Item Drops & SE
            ecl << pair(0x1ba0, 59) << pair(0x1c68, 0) << pair(0x1c40, 60); // Change Move Time, Wait Time & Inv. Time
            break;
        case THPrac::TH14::TH14_ST7_END_S2:
            ECLJump(ecl, 0x7778, 0x7b38, 60);
            ecl.SetFile(3);
            ECLJump(ecl, 0x790, 0x830, 0); // Utilize Spell Practice Jump
            ecl << pair(0x840, 3000); // Set Health
            ecl << pair(0x860, (int8_t)0x32); // Set Spell Ordinal
            ecl << pair(0x9520, (int16_t)0); // Disable Item Drops
            break;
        case THPrac::TH14::TH14_ST7_END_NS3:
            ECLJump(ecl, 0x7778, 0x7b38, 60);
            ecl.SetFile(3);
            ecl << pair(0xf08, (int8_t)0x33); // Change Nonspell
            ecl << pair(0x22b4, (int16_t)0) << pair(0x2460, (int16_t)0) << pair(0x25f8, (int16_t)0); // Disable Item Drops & SE
            ecl << pair(0x2550, 59) << pair(0x2618, 0) << pair(0x25f0, 60); // Change Move Time, Wait Time & Inv. Time
            break;
        case THPrac::TH14::TH14_ST7_END_S3:
            ECLJump(ecl, 0x7778, 0x7b38, 60);
            ecl.SetFile(3);
            ECLJump(ecl, 0x790, 0x830, 0); // Utilize Spell Practice Jump
            ecl << pair(0x840, 2300); // Set Health
            ecl << pair(0x860, (int8_t)0x33); // Set Spell Ordinal
            ecl << pair(0xa670, (int16_t)0); // Disable Item Drops
            break;
        case THPrac::TH14::TH14_ST7_END_NS4:
            ECLJump(ecl, 0x7778, 0x7b38, 60);
            ecl.SetFile(3);
            ecl << pair(0xf08, (int8_t)0x34); // Change Nonspell
            ecl << pair(0x2c00, (int16_t)0) << pair(0x2da4, (int16_t)0) << pair(0x2f3c, (int16_t)0); // Disable Item Drops & SE
            ecl << pair(0x2e94, 59) << pair(0x2f5c, 0) << pair(0x2f34, 60); // Change Move Time, Wait Time & Inv. Time
            break;
        case THPrac::TH14::TH14_ST7_END_S4:
            ECLJump(ecl, 0x7778, 0x7b38, 60);
            ecl.SetFile(3);
            ECLJump(ecl, 0x790, 0x830, 0); // Utilize Spell Practice Jump
            ecl << pair(0x840, 3500); // Set Health
            ecl << pair(0x860, (int8_t)0x34); // Set Spell Ordinal
            ecl << pair(0xae74, (int16_t)0); // Disable Item Drops
            break;
        case THPrac::TH14::TH14_ST7_END_NS5:
            ECLJump(ecl, 0x7778, 0x7b38, 60);
            ecl.SetFile(3);
            ecl << pair(0xf08, (int8_t)0x35); // Change Nonspell
            ecl << pair(0x34ec, (int16_t)0) << pair(0x3690, (int16_t)0) << pair(0x3828, (int16_t)0); // Disable Item Drops & SE
            ecl << pair(0x3780, 59) << pair(0x3848, 0) << pair(0x3820, 60); // Change Move Time, Wait Time & Inv. Time
            break;
        case THPrac::TH14::TH14_ST7_END_S5:
            ECLJump(ecl, 0x7778, 0x7b38, 60);
            ecl.SetFile(3);
            ECLJump(ecl, 0x790, 0x830, 0); // Utilize Spell Practice Jump
            ecl << pair(0x840, 3300); // Set Health
            ecl << pair(0x860, (int8_t)0x35); // Set Spell Ordinal
            ecl << pair(0xb558, (int16_t)0); // Disable Item Drops
            break;
        case THPrac::TH14::TH14_ST7_END_NS6:
            ECLJump(ecl, 0x7778, 0x7b38, 60);
            ecl.SetFile(3);
            ecl << pair(0xf08, (int8_t)0x36); // Change Nonspell
            ecl << pair(0x4300, (int16_t)0) << pair(0x44ac, (int16_t)0) << pair(0x4644, (int16_t)0); // Disable Item Drops & SE
            ecl << pair(0x459c, 59) << pair(0x4664, 0) << pair(0x463c, 60); // Change Move Time, Wait Time & Inv. Time
            break;
        case THPrac::TH14::TH14_ST7_END_S6:
            ECLJump(ecl, 0x7778, 0x7b38, 60);
            ecl.SetFile(3);
            ECLJump(ecl, 0x790, 0x830, 0); // Utilize Spell Practice Jump
            ecl << pair(0x840, 3000); // Set Health
            ecl << pair(0x860, (int8_t)0x36); // Set Spell Ordinal
            ecl << pair(0xc5f0, (int16_t)0); // Disable Item Drops
            break;
        case THPrac::TH14::TH14_ST7_END_NS7:
            ECLJump(ecl, 0x7778, 0x7b38, 60);
            ecl.SetFile(3);
            ecl << pair(0xf08, (int8_t)0x37); // Change Nonspell
            ecl << pair(0x4c3c, (int16_t)0) << pair(0x4de0, (int16_t)0) << pair(0x4f78, (int16_t)0); // Disable Item Drops & SE
            ecl << pair(0x4ed0, 59) << pair(0x4f98, 0) << pair(0x4f70, 60); // Change Move Time, Wait Time & Inv. Time
            break;
        case THPrac::TH14::TH14_ST7_END_S7:
            ECLJump(ecl, 0x7778, 0x7b38, 60);
            ecl.SetFile(3);
            ECLJump(ecl, 0x790, 0x830, 0); // Utilize Spell Practice Jump
            ecl << pair(0x840, 3500); // Set Health
            ecl << pair(0x860, (int8_t)0x37); // Set Spell Ordinal
            ecl << pair(0xd538, (int16_t)0); // Disable Item Drops
            break;
        case THPrac::TH14::TH14_ST7_END_NS8:
            ECLJump(ecl, 0x7778, 0x7b38, 60);
            ecl.SetFile(3);
            ecl << pair(0xf08, (int8_t)0x38); // Change Nonspell
            ecl << pair(0x5938, (int16_t)0) << pair(0x5ae4, (int16_t)0) << pair(0x5c7c, (int16_t)0); // Disable Item Drops & SE
            ecl << pair(0x5bd4, 59) << pair(0x5c9c, 0) << pair(0x5c74, 60); // Change Move Time, Wait Time & Inv. Time
            break;
        case THPrac::TH14::TH14_ST7_END_S8:
            ECLJump(ecl, 0x7778, 0x7b38, 60);
            ecl.SetFile(3);
            ECLJump(ecl, 0x790, 0x830, 0); // Utilize Spell Practice Jump
            ecl << pair(0x840, 3500); // Set Health
            ecl << pair(0x860, (int8_t)0x38); // Set Spell Ordinal
            ecl << pair(0xe638, (int16_t)0); // Disable Item Drops
            break;
        case THPrac::TH14::TH14_ST7_END_S9:
            ECLJump(ecl, 0x7778, 0x7b38, 60);
            ecl.SetFile(3);
            ECLJump(ecl, 0x790, 0x830, 0); // Utilize Spell Practice Jump
            ecl << pair(0x840, 4000); // Set Health
            ecl << pair(0x860, (int8_t)0x39); // Set Spell Ordinal
            break;
        case THPrac::TH14::TH14_ST7_END_S10:
            ECLJump(ecl, 0x7778, 0x7b38, 60);
            ecl.SetFile(3);
            ECLJump(ecl, 0x790, 0x830, 0); // Utilize Spell Practice Jump
            ecl << pair(0x840, 8000); // Set Health
            ecl << pair(0x860, (int8_t)0x31) << pair(0x861, (int8_t)0x30); // Set Spell Ordinal
            ecl << pair(0x6d64, (int16_t)0); // Disable Item Drops

            switch (thPracParam.phase) {
            case 1:
                ecl << pair(0x840, 6200);
                ECLJump(ecl, 0x70e4, 0x7344, 60);
                break;
            case 2:
                ecl << pair(0x840, 4400);
                ECLJump(ecl, 0x70e4, 0x75b8, 60);
                break;
            case 3:
                ecl << pair(0x840, 2600);
                ECLJump(ecl, 0x70e4, 0x78a0, 60);
                break;
            case 4:
                ecl << pair(0x840, 800);
                ECLJump(ecl, 0x70e4, 0x78a0, 60);
                ECLJump(ecl, 0x78f0, 0x7ba0, 60);
                break;
            default:
                break;
            }
            break;
        /*
			case THPrac::TH14::TH14_ST4A_MID1:
				break;
			case THPrac::TH14::TH14_ST4A_BOSS1:
				break;
			case THPrac::TH14::TH14_ST4A_BOSS2:
				break;
			case THPrac::TH14::TH14_ST4A_BOSS3:
				break;
			case THPrac::TH14::TH14_ST4A_BOSS4:
				break;
			case THPrac::TH14::TH14_ST4A_BOSS5:
				break;
			case THPrac::TH14::TH14_ST4A_BOSS6:
				break;
			case THPrac::TH14::TH14_ST4B_MID1:
				break;
			case THPrac::TH14::TH14_ST4B_BOSS1:
				break;
			case THPrac::TH14::TH14_ST4B_BOSS2:
				break;
			case THPrac::TH14::TH14_ST4B_BOSS3:
				break;
			case THPrac::TH14::TH14_ST4B_BOSS4:
				break;
			case THPrac::TH14::TH14_ST4B_BOSS5:
				break;
			case THPrac::TH14::TH14_ST4B_BOSS6:
				break;
			*/
        default:
            break;
        }
    }
    __declspec(noinline) void THPatchSP(ECLHelper& ecl)
    {
        uint32_t spell_id = *(int32_t*)(0x4f5844);
        switch (spell_id) {
        case 100:
        case 101:
        case 102:
        case 103:
            ecl.SetFile(3);
            if (thPracParam.phase == 1) {
                ecl.SetPos(0x8cdc);
                ecl << 30 << 30 << 30 << 30;
            }
            break;
        case 104:
        case 105:
        case 106:
        case 107:
            ecl.SetFile(3);
            switch (thPracParam.phase) {
            case 1:
                ecl << pair(0xae0, 6500);
                ecl << pair(0x9fb0, 0);
                ECLJump(ecl, 0xa188, 0xa2cc, 120);
                break;
            case 2:
                ecl << pair(0xae0, 3300);
                ecl << pair(0x9fb0, 0);
                ECLJump(ecl, 0xa188, 0xa2cc, 120);
                ECLJump(ecl, 0xa4a8, 0xa5ec, 120);
                break;
            case 3:
                ecl << pair(0xae0, 3300);
                ecl << pair(0x9fb0, 0);
                ECLJump(ecl, 0xa188, 0xa2cc, 120);
                ECLJump(ecl, 0xa4a8, 0xa5ec, 120);
                ECLJump(ecl, 0xa7b4, 0xa828, 120);
                break;
            default:
                break;
            }
            break;
        case 119:
            ecl.SetFile(3);
            switch (thPracParam.phase) {
            case 1:
                ecl << pair(0xe94, 6200);
                ECLJump(ecl, 0x70e4, 0x7344, 60);
                break;
            case 2:
                ecl << pair(0xe94, 4400);
                ECLJump(ecl, 0x70e4, 0x75b8, 60);
                break;
            case 3:
                ecl << pair(0xe94, 2600);
                ECLJump(ecl, 0x70e4, 0x78a0, 60);
                break;
            case 4:
                ecl << pair(0xe94, 800);
                ECLJump(ecl, 0x70e4, 0x78a0, 60);
                ECLJump(ecl, 0x78f0, 0x7ba0, 60);
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
        ecl.SetBaseAddr((void*)GetMemAddr(0x4db544, 0xcc, 0xC));

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
    void THDataInit()
    {
        AnlyDataInit();

        DataRef<DATA_SCENE_ID>(U32_ARG(0x4d964c));
        DataRef<DATA_RND_SEED>(U16_ARG(0x4db510));
        DataRef<DATA_DIFFCULTY>(U8_ARG(0x4f5834));
        DataRef<DATA_SHOT_TYPE>(U8_ARG(0x4f5828));
        DataRef<DATA_SUB_SHOT_TYPE>(U8_ARG(0x4f582c));
        DataRef<DATA_STAGE>(U8_ARG(0x4f58a4));
        DataRef<DATA_STARTING_STAGE>(U8_ARG(0x4f58a8));
    }

    HOOKSET_DEFINE(THMainHook)
    EHOOK_DY(th14_everlasting_bgm, (void*)0x46ef90)
    {
        int32_t retn_addr = ((int32_t*)pCtx->Esp)[0];
        int32_t bgm_cmd = ((int32_t*)pCtx->Esp)[1];
        int32_t bgm_id = ((int32_t*)pCtx->Esp)[2];
        int32_t call_addr = ((int32_t*)pCtx->Esp)[3];

        bool el_switch;
        bool is_practice;
        bool result;

        el_switch = *(THOverlay::singleton().mElBgm) && !THGuiRep::singleton().mRepStatus && thPracParam.mode == 1 && thPracParam.section;
        is_practice = (*((int32_t*)0x4f58b8) & 0x1); // is restarting
        result = ElBgmTest<0x445743, 0x436526, 0x448e91, 0x44908a, 0xffffffff>(
            el_switch, is_practice, retn_addr, bgm_cmd, bgm_id, 0xffffffff);

        if (result) {
            pCtx->Eip = 0x46f022;
        } 
    }
    EHOOK_DY(th14_param_reset, (void*)0x45a111)
    {
        thPracParam.Reset();
        THAdvOptWnd::singleton().ToggleLock(false);
    }
    EHOOK_DY(th14_prac_menu_1, (void*)0x45e768)
    {
        THGuiPrac::singleton().State(1);
        THAdvOptWnd::singleton().ToggleLock(false);
    }
    EHOOK_DY(th14_prac_menu_2, (void*)0x45e787)
    {
        THGuiPrac::singleton().State(2);
    }
    EHOOK_DY(th14_prac_menu_3, (void*)0x45ea9f)
    {
        THGuiPrac::singleton().State(3);
        THAdvOptWnd::singleton().ToggleLock(true);
    }
    EHOOK_DY(th14_prac_menu_4, (void*)0x45eb40)
    {
        THGuiPrac::singleton().State(4);
    }
    PATCH_DY(th14_prac_menu_enter_1, (void*)0x45e847, "\xeb", 1);
    EHOOK_DY(th14_prac_menu_enter_2, (void*)0x45eafc)
    {
        pCtx->Ecx = thPracParam.stage;
    }
    EHOOK_DY(th14_disable_prac_menu_1, (void*)0x45eca7)
    {
        pCtx->Eip = 0x45ece4;
    }
    PATCH_DY(th14_disable_prac_menu_2, (void*)0x45e813, "\x00", 1);
    EHOOK_DY(th14_menu_rank_fix, (void*)0x449c3d)
    {
        *((int32_t*)0x4f5844) = -1;
        *((int32_t*)0x4f5834) = *((int32_t*)0x4d5984);
    }
    EHOOK_DY(th14_patch_main, (void*)0x4360ce)
    {
        if (thPracParam.mode == 1) {
            *(int32_t*)(0x4f5830) = (int32_t)(thPracParam.score / 10);
            *(int32_t*)(0x4f5864) = thPracParam.life;
            *(int32_t*)(0x4f5868) = thPracParam.life_fragment;
            *(int32_t*)(0x4f5870) = thPracParam.bomb;
            *(int32_t*)(0x4f5874) = thPracParam.bomb_fragment;
            *(int32_t*)(0x4f5894) = thPracParam.cycle;
            *(int32_t*)(0x4f5858) = thPracParam.power;
            *(int32_t*)(0x4f584c) = thPracParam.value * 100;
            *(int32_t*)(0x4f5840) = thPracParam.graze; // 0x4E7420: Chapter Graze

            THSectionPatch();
        } else if (thPracParam.mode == 2) {
            ECLHelper ecl;
            ecl.SetBaseAddr((void*)GetMemAddr(0x4db544, 0xcc, 0xC));
            THPatchSP(ecl);
        }
        thPracParam._playLock = true;
    }
    EHOOK_DY(th14_bgm, (void*)0x43699a)
    {
        if (THBGMTest()) {
            PushHelper32(pCtx, 1);
            pCtx->Eip = 0x43699c;
        } 
    }
    EHOOK_DY(th14_on_restart, (void*)0x447810)
    {
        THAdvOptWnd::singleton().ToggleLock(true);
    }
    EHOOK_DY(th14_normal_game_enter, (void*)0x45e63e)
    {
        THAdvOptWnd::singleton().ToggleLock(true);
    }
    EHOOK_DY(th14_rep_save, (void*)0x455bf2)
    {
        char* repName = (char*)(pCtx->Esp + 0x38);
        if (thPracParam.mode == 1)
            THSaveReplay(repName);
        else if (thPracParam.mode == 2 && thPracParam.phase)
            THSaveReplay(repName);
        THAdvOptWnd::singleton().SaveReplay(mb_to_utf16(repName).c_str());
    }
    EHOOK_DY(th14_rep_menu_1, (void*)0x45f10b)
    {
        THGuiRep::singleton().State(1);
        THAdvOptWnd::singleton().ToggleLock(false);
        THAdvOptWnd::singleton().DeselectReplay();
    }
    EHOOK_DY(th14_rep_menu_2, (void*)0x45f216)
    {
        THGuiRep::singleton().State(2);
        THAdvOptWnd::singleton().SelectReplay();
    }
    EHOOK_DY(th14_rep_menu_3, (void*)0x45f3ed)
    {
        THGuiRep::singleton().State(3);
        THAdvOptWnd::singleton().ToggleLock(true);
    }
    EHOOK_DY(th14_sp_menu_1, (void*)0x464068)
    {
        if (THGuiSP::singleton().State()) {
            THAdvOptWnd::singleton().ToggleLock(true);
        } else {
            pCtx->Eip = 0x46409f;
        }
    }
    EHOOK_DY(th14_sp_menu_2, (void*)0x463fd8)
    {
        THAdvOptWnd::singleton().ToggleLock(false);
        if (THGuiSP::singleton().mState) {
            pCtx->Eip = 0x464068;
        }
    }
    EHOOK_DY(th14_update, (void*)0x40138a)
    {
        GameGuiBegin(IMPL_WIN32_DX9, !THAdvOptWnd::singleton().IsOpen());

        // Gui components update
        THGuiPrac::singleton().Update();
        THGuiRep::singleton().Update();
        THOverlay::singleton().Update();
        THGuiSP::singleton().Update();
        bool drawCursor = THAdvOptWnd::StaticUpdate() || THGuiPrac::singleton().IsOpen() || THGuiSP::singleton().IsOpen();

        GameGuiEnd(drawCursor);
    }
    EHOOK_DY(th14_render, (void*)0x40149a)
    {
        GameGuiRender(IMPL_WIN32_DX9);
    }
    HOOKSET_ENDDEF()

    HOOKSET_DEFINE(THInitHook)
    static __declspec(noinline) void THGuiCreate()
    {
        // Init
        GameGuiInit(IMPL_WIN32_DX9, 0x4d8f68, 0x4f5a18, 0x469eb0,
            Gui::INGAGME_INPUT_GEN2, 0x4d6884, 0x4d6880, 0,
            (*((int32_t*)0x4f7a54) >> 2) & 0xf);

        // Gui components creation
        THGuiPrac::singleton();
        THGuiRep::singleton();
        THOverlay::singleton();
        THGuiSP::singleton();

        // Hooks
        THMainHook::singleton().EnableAllHooks();
        THDataInit();

        // Reset thPracParam
        thPracParam.Reset();
    }
    static __declspec(noinline) void THInitHookDisable()
    {
        auto& s = THInitHook::singleton();
        s.th14_gui_init_1.Disable();
        s.th14_gui_init_2.Disable();
    }
    PATCH_DY(th14_disable_demo, (void*)0x45996c, "\xff\xff\xff\x7f", 4);
    EHOOK_DY(th14_disable_mutex, (void*)0x469387)
    {
        pCtx->Eip = 0x469556;
    }
    PATCH_DY(th14_startup_1, (void*)0x4595a5, "\xeb", 1);
    PATCH_DY(th14_startup_2, (void*)0x45a173, "\xeb", 1);
    EHOOK_DY(th14_gui_init_1, (void*)0x45a591)
    {
        THGuiCreate();
        THInitHookDisable();
    }
    EHOOK_DY(th14_gui_init_2, (void*)0x46b37b)
    {
        THGuiCreate();
        THInitHookDisable();
    }
    HOOKSET_ENDDEF()
}

void TH14Init()
{
    TH14::THInitHook::singleton().EnableAllHooks();
    TryKeepUpRefreshRate((void*)0x46b4b3, (void*)0x46b2de);
}
}
