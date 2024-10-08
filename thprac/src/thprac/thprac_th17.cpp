#include "thprac_games.h"
#include "thprac_utils.h"
#include <numbers>
#define M_PI 3.14159265358979323846f

struct vec2f {
    float x;
    float y;
};

namespace THPrac {
namespace TH17 {
    __declspec(noinline) void SpawnToken(uint32_t goast, vec2f& pos, float ang) {
        uintptr_t goast_manager = GetMemContent(0x004B7684);
        __asm movss xmm2, ang;
        asm_call<0x00410380, Thiscall>(goast_manager, &pos, goast);
    }

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
        int32_t goast_1;
        int32_t goast_2;
        int32_t goast_3;
        int32_t goast_4;
        int32_t goast_5;
        int32_t power;
        int32_t value;
        int32_t graze;

        bool dlg;

        bool _playLock = false;
        void Reset()
        {
            memset(this, 0, sizeof(THPracParam));
        }
        bool ReadJson(std::string& json)
        {
            ParseJson();

            ForceJsonValue(game, "th17");
            GetJsonValue(mode);
            GetJsonValue(stage);
            GetJsonValue(section);
            GetJsonValue(phase);
            GetJsonValueEx(dlg, Bool);

            GetJsonValue(score);
            GetJsonValue(life);
            GetJsonValue(life_fragment);
            GetJsonValue(bomb);
            GetJsonValue(bomb_fragment);
            GetJsonValue(goast_1);
            GetJsonValue(goast_2);
            GetJsonValue(goast_3);
            GetJsonValue(goast_4);
            GetJsonValue(goast_5);
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
                AddJsonValueEx(game, "th17", jalloc);
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
                AddJsonValue(life_fragment);
                AddJsonValue(bomb);
                AddJsonValue(bomb_fragment);
                if (goast_1) {
                    AddJsonValue(goast_1);
                    if (goast_2) {
                        AddJsonValue(goast_2);
                        if (goast_3) {
                            AddJsonValue(goast_3);
                            if (goast_4) {
                                AddJsonValue(goast_4);
                                if (goast_5) {
                                    AddJsonValue(goast_5);
                                }
                            }
                        }
                    }
                }
                AddJsonValue(power);
                AddJsonValue(value);
                AddJsonValue(graze);

                ReturnJson();
            } else if (mode == 2) {
                CreateJson();

                AddJsonValueEx(version, GetVersionStr(), jalloc);
                AddJsonValueEx(game, "th17", jalloc);
                AddJsonValue(mode);

                if (phase)
                    AddJsonValue(phase);

                ReturnJson();
            }

            CreateJson();
            jalloc; // Dummy usage to silence C4189
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
                mDiffculty = *((int32_t*)0x4b2b28);
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
                thPracParam.life_fragment = *mLifeFragment;
                thPracParam.bomb = *mBomb;
                thPracParam.bomb_fragment = *mBombFragment;

                if (*mGoast1) {
                    thPracParam.goast_1 = *mGoast1;
                    if (*mGoast2) {
                        thPracParam.goast_2 = *mGoast2;
                        if (*mGoast3) {
                            thPracParam.goast_3 = *mGoast3;
                            if (*mGoast4) {
                                thPracParam.goast_4 = *mGoast4;
                                if (*mGoast5) {
                                    thPracParam.goast_5 = *mGoast5;
                                }
                            }
                        }
                    }
                }

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
            SetTitle(S(TH_MENU));
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
            ImGui::TextUnformatted(S(TH_MENU));
            ImGui::Separator();

            PracticeMenu();
        }
        const th_glossary_t* SpellPhase()
        {
            auto section = CalcSection();
            if (section == TH17_ST6_BOSS11) {
                return TH_SPELL_PHASE2;
            } else if (section == TH17_ST7_END_S10) {
                return TH17_SPELL_5PHASE;
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
                mGoast1();
                if (*mGoast1) {
                    mGoast2();
                    if (*mGoast2) {
                        mGoast3();
                        if (*mGoast3) {
                            mGoast4();
                            if (*mGoast4) {
                                mGoast5();
                            }
                        }
                    }
                }
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
            case TH17_ST1_BOSS1:
            case TH17_ST2_BOSS1:
            case TH17_ST3_BOSS1:
            case TH17_ST4_BOSS1:
            case TH17_ST5_BOSS1:
            case TH17_ST6_BOSS1:
            case TH17_ST7_END_NS1:
            case TH17_ST7_MID1:
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
        Gui::GuiSlider<int, ImGuiDataType_S32> mLifeFragment { TH_LIFE_FRAGMENT, 0, 2 };
        Gui::GuiSlider<int, ImGuiDataType_S32> mBomb { TH_BOMB, 0, 9 };
        Gui::GuiSlider<int, ImGuiDataType_S32> mBombFragment { TH_BOMB_FRAGMENT, 0, 2 };
        Gui::GuiCombo mGoast1 { TH17_GOAST_1, TH17_GOAST_SELECT };
        Gui::GuiCombo mGoast2 { TH17_GOAST_2, TH17_GOAST_SELECT };
        Gui::GuiCombo mGoast3 { TH17_GOAST_3, TH17_GOAST_SELECT };
        Gui::GuiCombo mGoast4 { TH17_GOAST_4, TH17_GOAST_SELECT };
        Gui::GuiCombo mGoast5 { TH17_GOAST_5, TH17_GOAST_SELECT };
        Gui::GuiSlider<int, ImGuiDataType_S32> mPower { TH_POWER, 100, 400 };
        Gui::GuiDrag<int, ImGuiDataType_S32> mValue { TH_VALUE, 0, 999990, 10, 100000 };
        Gui::GuiDrag<int, ImGuiDataType_S32> mGraze { TH_GRAZE, 0, 999999, 1, 100000 };

        Gui::GuiNavFocus mNavFocus { TH_STAGE, TH_MODE, TH_WARP, TH_DLG,
            TH_MID_STAGE, TH_END_STAGE, TH_NONSPELL, TH_SPELL, TH_PHASE, TH_CHAPTER,
            TH_SCORE, TH_LIFE, TH_LIFE_FRAGMENT, TH_BOMB, TH_BOMB_FRAGMENT,
            TH17_GOAST_1, TH17_GOAST_2, TH17_GOAST_3, TH17_GOAST_4, TH17_GOAST_5,
            TH_POWER, TH_VALUE, TH_GRAZE };

        int mChapterSetup[7][2] {
            { 2, 2 },
            { 2, 3 },
            { 3, 3 },
            { 4, 2 },
            { 4, 2 },
            { 2, 2 },
            { 4, 4 },
        };

        int mDiffculty = 0;
    };
    class THGuiRep {
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
            uint32_t index = GetMemContent(0x4b77f0, 0x5b54);
            char* repName = (char*)GetMemAddr(0x4b77f0, index * 4 + 0x5b5c, 0x220);
            std::wstring repDir(mAppdataPath);
            repDir.append(L"\\ShanghaiAlice\\th17\\replay\\");
            repDir.append(mb_to_utf16(repName, 932));

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
            mInGameInfo.SetTextOffsetRel(x_offset_1, x_offset_2);
        }
        virtual void OnContentUpdate() override
        {
            mMuteki();
            mInfLives();
            mInfBombs();
            mInfPower();
            mTimeLock();
            mAutoBomb();
            mInfRoaring();
            mNoGoast();
            mElBgm();
            mInGameInfo();
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
            new HookCtx(0x44956a, "\x01", 1) } };
        Gui::GuiHotKey mInfLives { TH_INFLIVES, "F2", VK_F2, {
            new HookCtx(0x44921a, "\x90", 1) } };
        Gui::GuiHotKey mInfBombs { TH_INFBOMBS, "F3", VK_F3, {
            new HookCtx(0x411c96, "\x90\x90\x90", 3) } };
        Gui::GuiHotKey mInfPower { TH_INFPOWER, "F4", VK_F4, {
            new HookCtx(0x447b84, "\x31\xf6\x90\x90\x90\x90", 6) } };
        Gui::GuiHotKey mTimeLock { TH_TIMELOCK, "F5", VK_F5, {
            new HookCtx(0x41a8cf, "\xeb", 1),
            new HookCtx(0x420a1e, "\x05\x8d", 2) } };
        Gui::GuiHotKey mAutoBomb { TH_AUTOBOMB, "F6", VK_F6, {
            new HookCtx(0x447c20, "\x90\x90\x90\x90\x90\x90", 6) } };

    public:
        Gui::GuiHotKey mInfRoaring { TH17_INF_ROARING, "F7", VK_F7, {
            new HookCtx(0x40ef6a, "\x00", 1) } };
        Gui::GuiHotKey mNoGoast { TH17_NO_GOAST, "F8", VK_F8, {
            new HookCtx(0x4347af, "\xe9\x03\x01\x00\x00", 5) } };
        Gui::GuiHotKey mElBgm { TH_EL_BGM, "F9", VK_F9 };
        Gui::GuiHotKey mInGameInfo { THPRAC_INGAMEINFO, "F10", VK_F10 };
    };

    
    class TH17InGameInfo : public Gui::GameGuiWnd {
        TH17InGameInfo() noexcept
        {
            SetTitle("igi");
            SetFade(0.9f, 0.9f);
            SetPos(-10000.0f, -10000.0f);
            SetSize(280.0f, 350.0f);
            SetWndFlag(
                ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav | 0);
            OnLocaleChange();
        }
        SINGLETON(TH17InGameInfo);

    public:
        int32_t mMissCount;
        int32_t mBombCount;
        int32_t mRoarBreakCount;
        int32_t mRoarCount;
        int32_t mSpecialGoastCount;
        int32_t mWolfCount;
        int32_t mOtterCount;
        int32_t mEagerCount;

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
        }

        virtual void OnContentUpdate() override
        {
            if (!*(DWORD*)(0x004B77D0)) {
                SetPos(-10000.0f, -10000.0f); // fly~
                return;
            }
            {
                SetPosRel(900.0f / 1280.0f, 500.0f / 960.0f);
                SetSizeRel(340.0f/1280.0f, 320.0f/960.0f);
                ImGui::Columns(2);
                ImGui::Text(S(THPRAC_INGAMEINFO_MISS_COUNT));
                ImGui::NextColumn();
                ImGui::Text("%8d", mMissCount);
                ImGui::NextColumn();
                ImGui::Text(S(THPRAC_INGAMEINFO_BOMB_COUNT));
                ImGui::NextColumn();
                ImGui::Text("%8d", mBombCount);
                ImGui::NextColumn();
                ImGui::Text(S(THPRAC_INGAMEINFO_17_SPECIAL_GOAST_COUNT));
                ImGui::NextColumn();
                ImGui::Text("%8d", mSpecialGoastCount);
                ImGui::NextColumn();
                ImGui::Text(S(THPRAC_INGAMEINFO_17_ROAR_COUNT));
                ImGui::NextColumn();
                ImGui::Text("%8d", mRoarCount);
                ImGui::NextColumn();
                ImGui::Text(S(THPRAC_INGAMEINFO_17_ROAR_BREAK_COUNT));
                ImGui::NextColumn();
                ImGui::Text("%8d", mRoarBreakCount);
                ImGui::NextColumn();
                ImGui::Text(S(THPRAC_INGAMEINFO_17_WOLF_COUNT));
                ImGui::NextColumn();
                ImGui::Text("%8d", mWolfCount);
                ImGui::NextColumn();
                ImGui::Text(S(THPRAC_INGAMEINFO_17_OTTER_COUNT));
                ImGui::NextColumn();
                ImGui::Text("%8d", mOtterCount);
                ImGui::NextColumn();
                ImGui::Text(S(THPRAC_INGAMEINFO_17_EAGLE_COUNT));
                ImGui::NextColumn();
                ImGui::Text("%8d", mEagerCount);
                ImGui::NextColumn();
            }
        }

        virtual void OnPreUpdate() override
        {
            if (*(THOverlay::singleton().mInGameInfo)) {
                Open();
            } else {
                Close();
            }
        }

    public:
    };


    class THGuiSP : public Gui::GameGuiWnd {
        THGuiSP() noexcept
        {
            *mBugFix = true;

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
                spell_id = GetMemContent(0x4b77f0, GetMemContent(0x4b77f0, 0x24) * 4 + 0x5ddc);
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
                    if (Gui::InGameInputGetConfirm()) {
                        SetFade(0.8f, 0.1f);
                        Close();

                        // Fill Param
                        thPracParam.mode = 2;
                        thPracParam.phase = *mPhase;

                        // Reset Frame Counter
                        *(int32_t*)(*((int32_t*)0x4b77f0) + 0x2b0) = 0;

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
            SetTitle(S(TH_SPELL_PRAC));
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
            ImGui::TextUnformatted(S(TH_SPELL_PRAC));
            ImGui::Separator();

            PracticeMenu();
        }
        const th_glossary_t* SpellPhase()
        {
            if (mSpellId >= 84 && mSpellId <= 87) {
                return TH_SPELL_PHASE2;
            } else if (mSpellId == 100) {
                return TH17_SPELL_5PHASE;
            }
            return nullptr;
        }
        void PracticeMenu()
        {
            mPhase(TH_PHASE, SpellPhase());
            mNavFocus();
        }

        unsigned int mSpellId = UINT_MAX;

        Gui::GuiCheckBox mBugFix { TH16_BUGFIX };
        Gui::GuiCombo mPhase { TH_PHASE };
        Gui::GuiNavFocus mNavFocus { TH_PHASE };
    };

    EHOOK_G1(th17_force_goast_angle, 0x4105C9) {
        th17_force_goast_angle::GetHook().Disable();
        pCtx->Eip = 0x41062E;
    }

    class THAdvOptWnd : public Gui::PPGuiWnd {
        EHOOK_ST(th17_all_clear_bonus_1, 0x43229f)
        {
            pCtx->Eip = 0x4322ac;
        }
        EHOOK_ST(th17_all_clear_bonus_2, 0x4323d5)
        {
            *(int32_t*)(GetMemAddr(0x4b76ac, 0x168)) = *(int32_t*)(0x4b59fc);
            if (GetMemContent(0x4b59c8) & 0x10) {
                typedef void (*PScoreFunc)();
                PScoreFunc a = (PScoreFunc)0x444980;
                a();
                pCtx->Eip = 0x4322a4;
            }
        }
        EHOOK_ST(th17_all_clear_bonus_3, 0x43252e)
        {
            *(int32_t*)(GetMemAddr(0x4b76ac, 0x168)) = *(int32_t*)(0x4b59fc);
            if (GetMemContent(0x4b59c8) & 0x10) {
                typedef void (*PScoreFunc)();
                PScoreFunc a = (PScoreFunc)0x444980;
                a();
                pCtx->Eip = 0x4322a4;
            }
        }
    private:
        bool mInGoastMenu = false;

        void FpsInit()
        {
            if (*(uint8_t*)0x4b5cd9 == 3) {
                mOptCtx.fps_status = 1;

                DWORD oldProtect;
                VirtualProtect((void*)0x462215, 4, PAGE_EXECUTE_READWRITE, &oldProtect);
                *(double**)0x462215 = &mOptCtx.fps_dbl;
                VirtualProtect((void*)0x462215, 4, oldProtect, &oldProtect);
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
            th17_all_clear_bonus_1.Setup();
            th17_all_clear_bonus_2.Setup();
            th17_all_clear_bonus_3.Setup();
        }
        void GameplaySet()
        {
            th17_all_clear_bonus_1.Toggle(mOptCtx.all_clear_bonus);
            th17_all_clear_bonus_2.Toggle(mOptCtx.all_clear_bonus);
            th17_all_clear_bonus_3.Toggle(mOptCtx.all_clear_bonus);
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

        void GoastMenu() {
            if (ImGui::Button(S(TH_BACK))) {
                mInGoastMenu = false;
                return;
            }

            Gui::ComboSelect(mSelectedGoast, (th_glossary_t*)TH17_GOAST_SELECT_TOKENS, elementsof(TH17_GOAST_SELECT_TOKENS) - 1, S(TH17_GOAST));

            if (!mSelectedGoast) {
                ImGui::BeginDisabled();
            }

            if (mGoastAngleRandom)
                ImGui::BeginDisabled();

            ImGui::SliderFloat("Angle", &mGoastAng, -M_PI, M_PI);
            ImGui::Checkbox("Force angle", &mForceGoastAngle);
            ImGui::SameLine();
            HelpMarker("Tokens will try to move away from eachother both when spawning and when bouncing from a wall. This will ensure that a token will always go in the direction you specify");

            if (mGoastAngleRandom)
                ImGui::EndDisabled();

            ImGui::Checkbox("Random angle", &mGoastAngleRandom);

            ImRotateStart();
            ImGui::NewLine();
            ImGui::NewLine();
            ImGui::NewLine();
            ImGui::Indent();
            ImGui::Indent();
            ImGui::TextUnformatted("--angle-preview-->");
            ImGui::Unindent();
            ImGui::Unindent();
            constexpr auto HALF_PI = static_cast<float>(std::numbers::pi / 2.0);
            ImRotateEnd(HALF_PI - mGoastAng);

            if (!mSelectedGoast) {
                ImGui::EndDisabled();
            }

        }

        void ContentUpdate()
        {
            if (mInGoastMenu) {
                ImGui::TextUnformatted("Spawn a Goast");
                ImGui::Separator();
                GoastMenu();
                return;
            }
            ImGui::TextUnformatted(S(TH_ADV_OPT));
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

                // Temp
                ImGui::Checkbox(S(TH17_GOAST_BUGFIX), &mGoastBugfix);
                ImGui::Checkbox(S(TH17_GOAST_REPFIX), &mGoastRepfix);
                ImGui::SameLine();
                HelpMarker(S(TH17_GOAST_REPFIX_DESC));

                if (ImGui::Button("Spawn a Goast")) {
                    mInGoastMenu = true;
                }

                EndOptGroup();
            }
            AboutOpt();
            ImGui::EndChild();
            ImGui::SetWindowFocus();
        }

        adv_opt_ctx mOptCtx;

    public:
        bool mGoastBugfix = true;
        bool mGoastRepfix = false;
        bool mForceGoastAngle = false;
        bool mGoastAngleRandom = false;

        size_t mSelectedGoast = 0;
        float mGoastAng = 0;
    };

    void ECLStdExec(ECLHelper& ecl, unsigned int start, int std_id, int ecl_time = 0)
    {
        if (start)
            ecl.SetPos(start);
        ecl << ecl_time << 0x00140276 << 0x01ff0000 << 0x00000000 << std_id;
    }
    void ECLJump(ECLHelper& ecl, unsigned int start, unsigned int dest, int at_frame, int ecl_time = 0)
    {
        if (start)
            ecl.SetPos(start);
        else
            start = ecl.GetPos();
        ecl << ecl_time << 0x0018000C << 0x02ff0000 << 0x00000000 << dest - start << at_frame;
    }
    __declspec(noinline) void THStageWarp(ECLHelper& ecl, int stage, int portion)
    {
        if (stage == 1) {
            switch (portion) {
            case 1:
                break;
            case 2:
                ECLJump(ecl, 0x78c4, 0x7c3c, 60, 90); // 0x7cd8
                ECLJump(ecl, 0x4678, 0x46e0, 0, 0);
                break;
            case 3:
                ECLJump(ecl, 0x78c4, 0x7cd8, 60, 90);
                break;
            case 4:
                ECLJump(ecl, 0x78c4, 0x7cd8, 60, 90);
                ECLJump(ecl, 0x47d0, 0x4814, 0, 0);
                break;
            default:
                break;
            }
        } else if (stage == 2) {
            switch (portion) {
            case 1:
                break;
            case 2:
                ECLJump(ecl, 0x99fc, 0x9dec, 60, 90); // 0x9e88
                ECLJump(ecl, 0x4654, 0x4688, 0, 0);
                break;
            case 3:
                ECLJump(ecl, 0x99fc, 0x9e88, 60, 90);
                break;
            case 4:
                ECLJump(ecl, 0x99fc, 0x9e88, 60, 90);
                ECLJump(ecl, 0x488c, 0x48c0, 0, 0);
                break;
            case 5:
                ECLJump(ecl, 0x99fc, 0x9e88, 60, 90);
                ECLJump(ecl, 0x488c, 0x4954, 0, 0);
                break;
            default:
                break;
            }
        } else if (stage == 3) {
            switch (portion) {
            case 1:
                break;
            case 2:
                ECLJump(ecl, 0x8cec, 0x9010, 60, 90); // 0x90c0
                ECLJump(ecl, 0x44b8, 0x44ec, 0, 0);
                break;
            case 3:
                ECLJump(ecl, 0x8cec, 0x9010, 60, 90); // 0x90c0
                ECLJump(ecl, 0x44b8, 0x45e8, 0, 0);
                break;
            case 4:
                ECLJump(ecl, 0x8cec, 0x90c0, 60, 90);
                ecl << pair{0x6b04, 0};
                break;
            case 5:
                ECLJump(ecl, 0x8cec, 0x90c0, 60, 90);
                ECLJump(ecl, 0x46f8, 0x4758, 0, 0);
                break;
            case 6:
                ECLJump(ecl, 0x8cec, 0x90c0, 60, 90);
                ECLJump(ecl, 0x46f8, 0x47c0, 0, 0);
                break;
            default:
                break;
            }
        } else if (stage == 4) {
            switch (portion) {
            case 1:
                break;
            case 2:
                ECLJump(ecl, 0xc374, 0xc698, 60, 90); // 0xc748
                ECLJump(ecl, 0x4d28, 0x4d88, 0, 0);
                break;
            case 3:
                ECLJump(ecl, 0xc374, 0xc698, 60, 90); // 0xc748
                ECLJump(ecl, 0x4d28, 0x4df0, 0, 0);
                break;
            case 4:
                ECLJump(ecl, 0xc374, 0xc698, 60, 90); // 0xc748
                ECLJump(ecl, 0x4d28, 0x4e58, 0, 0);
                break;
            case 5:
                ECLJump(ecl, 0xc374, 0xc748, 60, 90);
                break;
            case 6:
                ECLJump(ecl, 0xc374, 0xc748, 60, 90);
                ECLJump(ecl, 0x4f20, 0x4f94, 0, 0);
                break;
            default:
                break;
            }
        } else if (stage == 5) {
            switch (portion) {
            case 1:
                break;
            case 2:
                ECLJump(ecl, 0xae0c, 0xb154, 60, 90); // 0xb1f0
                ECLJump(ecl, 0x4ba4, 0x4bec, 0, 0);
                break;
            case 3:
                ECLJump(ecl, 0xae0c, 0xb154, 60, 90); // 0xb1f0
                ECLJump(ecl, 0x4ba4, 0x4c4c, 0, 0);
                break;
            case 4:
                ECLJump(ecl, 0xae0c, 0xb154, 60, 90); // 0xb1f0
                ECLJump(ecl, 0x4ba4, 0x4c80, 0, 0);
                break;
            case 5:
                ECLJump(ecl, 0xae0c, 0xb1f0, 60, 90);
                break;
            case 6:
                ECLJump(ecl, 0xae0c, 0xb1f0, 60, 90);
                ECLJump(ecl, 0x4d14, 0x4d88, 0, 0);
                ecl << pair{0x6a98, 0};
                break;
            default:
                break;
            }
        } else if (stage == 6) {
            switch (portion) {
            case 1:
                break;
            case 2:
                ECLJump(ecl, 0x71bc, 0x7504, 60, 90); // 0x75b4
                ECLJump(ecl, 0x35ac, 0x3620, 0, 0);
                break;
            case 3:
                ECLJump(ecl, 0x71bc, 0x75b4, 60, 90);
                ecl << pair{0x36c4, 0};
                break;
            case 4:
                ECLJump(ecl, 0x71bc, 0x75b4, 60, 90);
                ECLJump(ecl, 0x36b4, 0x3728, 0, 0);
                break;
            default:
                break;
            }
        } else if (stage == 7) {
            switch (portion) {
            case 1:
                break;
            case 2:
                ECLJump(ecl, 0xa208, 0xa6a8, 60, 90); // 0xa758
                ECLJump(ecl, 0x4960, 0x49a8, 0, 0);
                break;
            case 3:
                ECLJump(ecl, 0xa208, 0xa6a8, 60, 90); // 0xa758
                ECLJump(ecl, 0x4960, 0x49dc, 0, 0);
                break;
            case 4:
                ECLJump(ecl, 0xa208, 0xa6a8, 60, 90); // 0xa758
                ECLJump(ecl, 0x4960, 0x4a10, 0, 0);
                break;
            case 5:
                ECLJump(ecl, 0xa208, 0xa758, 60, 90);
                ecl << pair{0x4ac4, 0};
                break;
            case 6:
                ECLJump(ecl, 0xa208, 0xa758, 60, 90);
                ECLJump(ecl, 0x4ab4, 0x4b28, 0, 0);
                break;
            case 7:
                ECLJump(ecl, 0xa208, 0xa758, 60, 90);
                ECLJump(ecl, 0x4ab4, 0x4b88, 0, 0);
                break;
            case 8:
                ECLJump(ecl, 0xa208, 0xa758, 60, 90);
                ECLJump(ecl, 0x4ab4, 0x4bbc, 0, 0);
                break;
            default:
                break;
            }
        }
    }
    __declspec(noinline) void THPatch(ECLHelper& ecl, th_sections_t section)
    {
        switch (section) {
        case THPrac::TH17::TH17_ST1_MID1:
            ECLStdExec(ecl, 0x78c4, 1, 1);
            ECLJump(ecl, 0, 0x7c94, 60);
            break;
        case THPrac::TH17::TH17_ST1_BOSS1:
            ECLStdExec(ecl, 0x78c4, 1, 1);
            if (thPracParam.dlg)
                ECLJump(ecl, 0, 0x7d70, 60);
            else
                ECLJump(ecl, 0, 0x7d84, 60);
            break;
        case THPrac::TH17::TH17_ST1_BOSS2:
            ECLStdExec(ecl, 0x78c4, 1, 1);
            ECLJump(ecl, 0, 0x7d84, 60);
            ecl.SetFile(2);
            ECLJump(ecl, 0x39c, 0x484, 1); // Utilize Spell Practice Jump
            ecl << pair{0x494, 2400}; // Set Health
            ecl << pair{0x4b4, (int8_t)0x31}; // Set Spell Ordinal
            ecl << pair{0x1a44, (int16_t)0}; // Disable Item Drops
            break;
        case THPrac::TH17::TH17_ST1_BOSS3:
            ECLStdExec(ecl, 0x78c4, 1, 1);
            ECLJump(ecl, 0, 0x7d84, 60);
            ecl.SetFile(2);
            ecl << pair{0x5d4, (int8_t)0x32}; // Change Nonspell
            ecl << pair{0xf7c, (int16_t)0} << pair{0xfb4, (int16_t)0}; // Disable Item Drops
            ecl << pair{0x10e0, (int16_t)0}; // Disable Item Drops & SE
            ECLJump(ecl, 0x1234, 0x12b4, 0);
            ecl << pair{0xe9c, 0};
            break;
        case THPrac::TH17::TH17_ST1_BOSS4:
            ECLStdExec(ecl, 0x78c4, 1, 1);
            ECLJump(ecl, 0, 0x7d84, 60);
            ecl.SetFile(2);
            ECLJump(ecl, 0x39c, 0x484, 1); // Utilize Spell Practice Jump
            ecl << pair{0x494, 2400}; // Set Health
            ecl << pair{0x4b4, (int8_t)0x32}; // Set Spell Ordinal
            ecl << pair{0x2b2c, (int16_t)0}; // Disable Item Drops
            break;
        case THPrac::TH17::TH17_ST2_MID1:
            ECLStdExec(ecl, 0x99fc, 1, 1);
            ECLJump(ecl, 0, 0x9e44, 60);
            break;
        case THPrac::TH17::TH17_ST2_BOSS1:
            ECLStdExec(ecl, 0x99fc, 1, 1);
            if (thPracParam.dlg)
                ECLJump(ecl, 0, 0x9ee0, 60);
            else
                ECLJump(ecl, 0, 0x9ef4, 60);
            break;
        case THPrac::TH17::TH17_ST2_BOSS2:
            ECLStdExec(ecl, 0x99fc, 1, 1);
            ECLJump(ecl, 0, 0x9ef4, 60);
            ecl.SetFile(2);
            ECLJump(ecl, 0x3fc, 0x4e4, 1); // Utilize Spell Practice Jump
            ecl << pair{0x4f4, 2200}; // Set Health
            ecl << pair{0x514, (int8_t)0x31}; // Set Spell Ordinal
            ecl << pair{0x26e4, (int16_t)0} << pair{0x271c, (int16_t)0}; // Disable Item Drops
            break;
        case THPrac::TH17::TH17_ST2_BOSS3:
            ECLStdExec(ecl, 0x99fc, 1, 1);
            ECLJump(ecl, 0, 0x9ef4, 60);
            ecl.SetFile(2);
            ecl << pair{0x738, (int8_t)0x32}; // Change Nonspell
            ecl << pair{0x1360, (int16_t)0} << pair{0x1398, (int16_t)0}; // Disable Item Drops
            ecl << pair{0x14c4, (int16_t)0}; // Disable Item Drops & SE
            ecl << pair{0x1654, 60} << pair{0x1280, 90};
            break;
        case THPrac::TH17::TH17_ST2_BOSS4:
            ECLStdExec(ecl, 0x99fc, 1, 1);
            ECLJump(ecl, 0, 0x9ef4, 60);
            ecl.SetFile(2);
            ECLJump(ecl, 0x3fc, 0x4e4, 1); // Utilize Spell Practice Jump
            ecl << pair{0x4f4, 2200}; // Set Health
            ecl << pair{0x514, (int8_t)0x32}; // Set Spell Ordinal
            ecl << pair{0x3058, (int16_t)0}; // Disable Item Drops
            break;
        case THPrac::TH17::TH17_ST2_BOSS5:
            ECLStdExec(ecl, 0x99fc, 1, 1);
            ECLJump(ecl, 0, 0x9ef4, 60);
            ecl.SetFile(2);
            ECLJump(ecl, 0x3fc, 0x4e4, 1); // Utilize Spell Practice Jump
            ecl << pair{0x4f4, 3000}; // Set Health
            ecl << pair{0x514, (int8_t)0x33}; // Set Spell Ordinal
            ecl << pair{0x4b04, (int16_t)0}; // Disable Item Drops
            break;
        case THPrac::TH17::TH17_ST3_MID1:
            ECLStdExec(ecl, 0x8cec, 1, 1);
            ECLJump(ecl, 0, 0x907c, 60);
            break;
        case THPrac::TH17::TH17_ST3_BOSS1:
            ECLStdExec(ecl, 0x8cec, 1, 1);
            if (thPracParam.dlg)
                ECLJump(ecl, 0, 0x912c, 60);
            else
                ECLJump(ecl, 0, 0x9140, 60);
            break;
        case THPrac::TH17::TH17_ST3_BOSS2:
            ECLStdExec(ecl, 0x8cec, 1, 1);
            ECLJump(ecl, 0, 0x9140, 60);
            ecl.SetFile(2);
            ECLJump(ecl, 0x470, 0x558, 1); // Utilize Spell Practice Jump
            ecl << pair{0x568, 2700}; // Set Health
            ecl << pair{0x588, (int8_t)0x31}; // Set Spell Ordinal
            ecl << pair{0x2d70, (int16_t)0}; // Disable Item Drops
            break;
        case THPrac::TH17::TH17_ST3_BOSS3:
            ECLStdExec(ecl, 0x8cec, 1, 1);
            ECLJump(ecl, 0, 0x9140, 60);
            ecl.SetFile(2);
            ecl << pair{0x7cc, (int8_t)0x32}; // Change Nonspell
            ecl << pair{0x1330, (int16_t)0} << pair{0x1368, (int16_t)0}; // Disable Item Drops
            ecl << pair{0x1494, (int16_t)0}; // Disable Item Drops & SE
            ecl << pair{0x15f8, 0} << pair{0x1250, 30};
            break;
        case THPrac::TH17::TH17_ST3_BOSS4:
            ECLStdExec(ecl, 0x8cec, 1, 1);
            ECLJump(ecl, 0, 0x9140, 60);
            ecl.SetFile(2);
            ECLJump(ecl, 0x470, 0x558, 1); // Utilize Spell Practice Jump
            ecl << pair{0x568, 2700}; // Set Health
            ecl << pair{0x588, (int8_t)0x32}; // Set Spell Ordinal
            ecl << pair{0x3b2c, (int16_t)0} << pair{0x3b64, (int16_t)0}; // Disable Item Drops
            break;
        case THPrac::TH17::TH17_ST3_BOSS5:
            ECLStdExec(ecl, 0x8cec, 1, 1);
            ECLJump(ecl, 0, 0x9140, 60);
            ecl.SetFile(2);
            ecl << pair{0x7cc, (int8_t)0x33}; // Change Nonspell
            ecl << pair{0x2070, (int16_t)0} << pair{0x20a8, (int16_t)0}; // Disable Item Drops
            ecl << pair{0x21d4, (int16_t)0}; // Disable Item Drops & SE
            ecl << pair{0x2338, 0} << pair{0x1f90, 30};
            break;
        case THPrac::TH17::TH17_ST3_BOSS6:
            ECLStdExec(ecl, 0x8cec, 1, 1);
            ECLJump(ecl, 0, 0x9140, 60);
            ecl.SetFile(2);
            ECLJump(ecl, 0x470, 0x558, 1); // Utilize Spell Practice Jump
            ecl << pair{0x568, 2600}; // Set Health
            ecl << pair{0x588, (int8_t)0x33}; // Set Spell Ordinal
            ecl << pair{0x4ff0, (int16_t)0} << pair{0x5028, (int16_t)0}; // Disable Item Drops
            break;
        case THPrac::TH17::TH17_ST4_MID1:
            ECLStdExec(ecl, 0xc374, 1, 1);
            ECLJump(ecl, 0, 0xc704, 60);
            break;
        case THPrac::TH17::TH17_ST4_BOSS1:
            ECLStdExec(ecl, 0xc374, 1, 1);
            if (thPracParam.dlg)
                ECLJump(ecl, 0, 0xc7b4, 60);
            else
                ECLJump(ecl, 0, 0xc7c8, 60);
            break;
        case THPrac::TH17::TH17_ST4_BOSS2:
            ECLStdExec(ecl, 0xc374, 1, 1);
            ECLJump(ecl, 0, 0xc7c8, 60);
            ecl.SetFile(2);
            ECLJump(ecl, 0x3d4, 0x4bc, 1); // Utilize Spell Practice Jump
            ecl << pair{0x4cc, 2400}; // Set Health
            ecl << pair{0x4ec, (int8_t)0x31}; // Set Spell Ordinal
            ecl << pair{0x33dc, (int16_t)0}; // Disable Item Drops
            break;
        case THPrac::TH17::TH17_ST4_BOSS3:
            ECLStdExec(ecl, 0xc374, 1, 1);
            ECLJump(ecl, 0, 0xc7c8, 60);
            ecl.SetFile(2);
            ecl << pair{0x730, (int8_t)0x32}; // Change Nonspell
            ecl << pair{0x1454, (int16_t)0} << pair{0x148c, (int16_t)0}; // Disable Item Drops
            ecl << pair{0x15b8, (int16_t)0}; // Disable Item Drops & SE
            ecl << pair{0x171c, 0} << pair{0x1374, 30};
            break;
        case THPrac::TH17::TH17_ST4_BOSS4:
            ECLStdExec(ecl, 0xc374, 1, 1);
            ECLJump(ecl, 0, 0xc7c8, 60);
            ecl.SetFile(2);
            ECLJump(ecl, 0x3d4, 0x4bc, 1); // Utilize Spell Practice Jump
            ecl << pair{0x4cc, 2400}; // Set Health
            ecl << pair{0x4ec, (int8_t)0x32}; // Set Spell Ordinal
            ecl << pair{0x44f0, (int16_t)0} << pair{0x4528, (int16_t)0}; // Disable Item Drops
            break;
        case THPrac::TH17::TH17_ST4_BOSS5:
            ECLStdExec(ecl, 0xc374, 1, 1);
            ECLJump(ecl, 0, 0xc7c8, 60);
            ecl.SetFile(2);
            ecl << pair{0x730, (int8_t)0x33}; // Change Nonspell
            ecl << pair{0x24a0, (int16_t)0} << pair{0x24d8, (int16_t)0}; // Disable Item Drops
            ecl << pair{0x2604, (int16_t)0}; // Disable Item Drops & SE
            ecl << pair{0x2768, 0} << pair{0x23c0, 30};
            break;
        case THPrac::TH17::TH17_ST4_BOSS6:
            ECLStdExec(ecl, 0xc374, 1, 1);
            ECLJump(ecl, 0, 0xc7c8, 60);
            ecl.SetFile(2);
            ECLJump(ecl, 0x3d4, 0x4bc, 1); // Utilize Spell Practice Jump
            ecl << pair{0x4cc, 4000}; // Set Health
            ecl << pair{0x4ec, (int8_t)0x33}; // Set Spell Ordinal
            ecl << pair{0x5264, (int16_t)0} << pair{0x529c, (int16_t)0}; // Disable Item Drops
            break;
        case THPrac::TH17::TH17_ST5_MID1:
            ECLStdExec(ecl, 0xae0c, 1, 1);
            ECLJump(ecl, 0, 0xb1ac, 60);
            break;
        case THPrac::TH17::TH17_ST5_BOSS1:
            ECLStdExec(ecl, 0xae0c, 1, 1);
            if (thPracParam.dlg)
                ECLJump(ecl, 0, 0xb25c, 60);
            else
                ECLJump(ecl, 0, 0xb270, 60);
            break;
        case THPrac::TH17::TH17_ST5_BOSS2:
            ECLStdExec(ecl, 0xae0c, 1, 1);
            ECLJump(ecl, 0, 0xb270, 60);
            ecl.SetFile(2);
            ECLJump(ecl, 0x484, 0x56c, 1); // Utilize Spell Practice Jump
            ecl << pair{0x57c, 2500}; // Set Health
            ecl << pair{0x59c, (int8_t)0x31}; // Set Spell Ordinal
            ecl << pair{0x3854, (int16_t)0}; // Disable Item Drops
            break;
        case THPrac::TH17::TH17_ST5_BOSS3:
            ECLStdExec(ecl, 0xae0c, 1, 1);
            ECLJump(ecl, 0, 0xb270, 60);
            ecl.SetFile(2);
            ecl << pair{0x8e4, (int8_t)0x32}; // Change Nonspell
            ecl << pair{0x1508, (int16_t)0} << pair{0x1540, (int16_t)0}; // Disable Item Drops
            ecl << pair{0x166c, (int16_t)0}; // Disable Item Drops & SE
            ecl << pair{0x17d0, 0} << pair{0x1428, 30};
            break;
        case THPrac::TH17::TH17_ST5_BOSS4:
            ECLStdExec(ecl, 0xae0c, 1, 1);
            ECLJump(ecl, 0, 0xb270, 60);
            ecl.SetFile(2);
            ECLJump(ecl, 0x484, 0x56c, 1); // Utilize Spell Practice Jump
            ecl << pair{0x57c, 2300}; // Set Health
            ecl << pair{0x59c, (int8_t)0x32}; // Set Spell Ordinal
            ecl << pair{0x5260, (int16_t)0} << pair{0x5298, (int16_t)0}; // Disable Item Drops
            break;
        case THPrac::TH17::TH17_ST5_BOSS5:
            ECLStdExec(ecl, 0xae0c, 1, 1);
            ECLJump(ecl, 0, 0xb270, 60);
            ecl.SetFile(2);
            ecl << pair{0x8e4, (int8_t)0x33}; // Change Nonspell
            ecl << pair{0x2408, (int16_t)0} << pair{0x2440, (int16_t)0}; // Disable Item Drops
            ecl << pair{0x256c, (int16_t)0}; // Disable Item Drops & SE
            ecl << pair{0x26d0, 0} << pair{0x2328, 30};
            break;
        case THPrac::TH17::TH17_ST5_BOSS6:
            ECLStdExec(ecl, 0xae0c, 1, 1);
            ECLJump(ecl, 0, 0xb270, 60);
            ecl.SetFile(2);
            ECLJump(ecl, 0x484, 0x56c, 1); // Utilize Spell Practice Jump
            ecl << pair{0x57c, 2300}; // Set Health
            ecl << pair{0x59c, (int8_t)0x33}; // Set Spell Ordinal
            ecl << pair{0x70ac, (int16_t)0} << pair{0x70e4, (int16_t)0}; // Disable Item Drops
            break;
        case THPrac::TH17::TH17_ST5_BOSS7:
            ECLStdExec(ecl, 0xae0c, 1, 1);
            ECLJump(ecl, 0, 0xb270, 60);
            ecl.SetFile(2);
            ECLJump(ecl, 0x484, 0x56c, 1); // Utilize Spell Practice Jump
            ecl << pair{0x57c, 3000}; // Set Health
            ecl << pair{0x59c, (int8_t)0x34}; // Set Spell Ordinal
            break;
        case THPrac::TH17::TH17_ST6_MID1:
            ECLStdExec(ecl, 0x71bc, 1, 1);
            ECLJump(ecl, 0, 0x7570, 60);
            break;
        case THPrac::TH17::TH17_ST6_BOSS1:
            ECLStdExec(ecl, 0x71bc, 1, 1);
            ECLStdExec(ecl, 0, 2, 2);
            if (thPracParam.dlg)
                ECLJump(ecl, 0, 0x7620, 60);
            else
                ECLJump(ecl, 0, 0x7634, 60);
            ecl.SetFile(2);
            ecl << pair{0x454, (int16_t)0};
            break;
        case THPrac::TH17::TH17_ST6_BOSS2:
            ECLStdExec(ecl, 0x71bc, 1, 1);
            ECLStdExec(ecl, 0, 2, 2);
            ECLJump(ecl, 0, 0x7634, 60);
            ecl.SetFile(2);
            ecl << pair{0x454, (int16_t)0};
            ECLJump(ecl, 0x548, 0x630, 2); // Utilize Spell Practice Jump
            ecl << pair{0x640, 2600}; // Set Health
            ecl << pair{0x660, (int8_t)0x31}; // Set Spell Ordinal
            ecl << pair{0x66c4, (int16_t)0}; // Disable Item Drops
            break;
        case THPrac::TH17::TH17_ST6_BOSS3:
            ECLStdExec(ecl, 0x71bc, 3, 1);
            ECLJump(ecl, 0, 0x7634, 60);
            ecl.SetFile(2);
            ecl << pair{0x454, (int16_t)0};
            ecl << pair{0xda4, (int8_t)0x32}; // Change Nonspell
            ecl << pair{0x2038, (int16_t)0} << pair{0x2070, (int16_t)0}; // Disable Item Drops
            ecl << pair{0x219c, (int16_t)0}; // Disable Item Drops & SE
            ecl << pair{0x2300, 0} << pair{0x1f44, 30};
            ecl << pair{0x2004, (int16_t)0};
            break;
        case THPrac::TH17::TH17_ST6_BOSS4:
            ECLStdExec(ecl, 0x71bc, 3, 1);
            ECLJump(ecl, 0, 0x7634, 60);
            ecl.SetFile(2);
            ecl << pair{0x454, (int16_t)0};
            ECLJump(ecl, 0x548, 0x630, 2); // Utilize Spell Practice Jump
            ecl << pair{0x640, 3800}; // Set Health
            ecl << pair{0x660, (int8_t)0x32}; // Set Spell Ordinal
            ecl << pair{0x78d8, (int16_t)0} << pair{0x7910, (int16_t)0}; // Disable Item Drops
            break;
        case THPrac::TH17::TH17_ST6_BOSS5:
            ECLStdExec(ecl, 0x71bc, 5, 1);
            ECLJump(ecl, 0, 0x7634, 60);
            ecl.SetFile(2);
            ecl << pair{0x454, (int16_t)0};
            ecl << pair{0xda4, (int8_t)0x33}; // Change Nonspell
            ecl << pair{0x32c8, (int16_t)0} << pair{0x3300, (int16_t)0}; // Disable Item Drops
            ecl << pair{0x3444, (int16_t)0}; // Disable Item Drops & SE
            ecl << pair{0x31d4, 0};
            ECLJump(ecl, 0x3598, 0x35e8, 0);
            ecl << pair{0x3294, (int16_t)0};
            break;
        case THPrac::TH17::TH17_ST6_BOSS6:
            ECLStdExec(ecl, 0x71bc, 5, 1);
            ECLJump(ecl, 0, 0x7634, 60);
            ecl.SetFile(2);
            ecl << pair{0x454, (int16_t)0};
            ECLJump(ecl, 0x548, 0x630, 2); // Utilize Spell Practice Jump
            ecl << pair{0x640, 3000}; // Set Health
            ecl << pair{0x660, (int8_t)0x33}; // Set Spell Ordinal
            ecl << pair{0x9bc0, (int16_t)0} << pair{0x9bf8, (int16_t)0}; // Disable Item Drops
            break;
        case THPrac::TH17::TH17_ST6_BOSS7:
            ECLStdExec(ecl, 0x71bc, 5, 1);
            ECLJump(ecl, 0, 0x7634, 60);
            ecl.SetFile(2);
            ecl << pair{0x454, (int16_t)0};
            ecl << pair{0xda4, (int8_t)0x34}; // Change Nonspell
            ecl << pair{0x4888, (int16_t)0} << pair{0x48c0, (int16_t)0} << pair{0x48f8, (int16_t)0}; // Disable Item Drops
            ecl << pair{0x4a3c, (int16_t)0}; // Disable Item Drops & SE
            ecl << pair{0x4ba0, 0} << pair{0x4794, 30};
            ecl << pair{0x4854, (int16_t)0};
            break;
        case THPrac::TH17::TH17_ST6_BOSS8:
            ECLStdExec(ecl, 0x71bc, 5, 1);
            ECLJump(ecl, 0, 0x7634, 60);
            ecl.SetFile(2);
            ecl << pair{0x454, (int16_t)0};
            ECLJump(ecl, 0x548, 0x630, 2); // Utilize Spell Practice Jump
            ecl << pair{0x640, 2700}; // Set Health
            ecl << pair{0x660, (int8_t)0x34}; // Set Spell Ordinal
            ecl << pair{0xac30, (int16_t)0} << pair{0xac68, (int16_t)0}; // Disable Item Drops
            break;
        case THPrac::TH17::TH17_ST6_BOSS9:
            ECLStdExec(ecl, 0x71bc, 5, 1);
            ECLJump(ecl, 0, 0x7634, 60);
            ecl.SetFile(2);
            ecl << pair{0x454, (int16_t)0};
            ECLJump(ecl, 0x548, 0x630, 2); // Utilize Spell Practice Jump
            ecl << pair{0x640, 3500}; // Set Health
            ecl << pair{0x660, (int8_t)0x35}; // Set Spell Ordinal
            break;
        case THPrac::TH17::TH17_ST6_BOSS10:
            ECLStdExec(ecl, 0x71bc, 5, 1);
            ECLJump(ecl, 0, 0x7634, 60);
            ecl.SetFile(2);
            ecl << pair{0x454, (int16_t)0};
            ECLJump(ecl, 0x548, 0x630, 2); // Utilize Spell Practice Jump
            ecl << pair{0x640, 5000}; // Set Health
            ecl << pair{0x660, (int8_t)0x36}; // Set Spell Ordinal
            break;
        case THPrac::TH17::TH17_ST6_BOSS11:
            ECLStdExec(ecl, 0x71bc, 5, 1);
            ECLJump(ecl, 0, 0x7634, 60);
            ecl.SetFile(2);
            ecl << pair{0x454, (int16_t)0};
            ECLJump(ecl, 0x548, 0x630, 2); // Utilize Spell Practice Jump
            ecl << pair{0x640, 10000}; // Set Health
            ecl << pair{0x660, (int8_t)0x37}; // Set Spell Ordinal

            switch (thPracParam.phase) {
            case 1:
                ecl << pair{0x640, 7000};
                ECLJump(ecl, 0xf0c0, 0xf238, 120);
                break;
            case 2:
                ecl << pair{0x640, 4500};
                ECLJump(ecl, 0xf0c0, 0xf238, 120);
                ECLJump(ecl, 0xf2d0, 0xf448, 120);
                break;
            case 3:
                ecl << pair{0x640, 2500};
                ECLJump(ecl, 0xf0c0, 0xf238, 120);
                ECLJump(ecl, 0xf2d0, 0xf448, 120);
                ECLJump(ecl, 0xf508, 0xf680, 120);
                break;
            default:
                break;
            }
            break;
        case THPrac::TH17::TH17_ST7_MID1:
            ECLStdExec(ecl, 0xa208, 1, 1);
            if (thPracParam.dlg)
                ECLJump(ecl, 0, 0xa6ec, 60);
            else {
                ECLJump(ecl, 0, 0xa714, 60);
                ecl.SetFile(3);
                ecl.SetPos(0x410);
                ecl << 0 << 0x00140203 << 0x01ff0000 << 0 << 100;
                ECLJump(ecl, 0, 0x55c, 0);
                ecl << pair{0x5a4, 100};
                ECLJump(ecl, 0x5d8, 0x678, 0);
            }
            break;
        case THPrac::TH17::TH17_ST7_MID2:
            ECLStdExec(ecl, 0xa208, 1, 1);
            ECLJump(ecl, 0, 0xa714, 60);
            ecl.SetFile(3);
            ecl.SetPos(0x410);
            ecl << 0 << 0x00140203 << 0x01ff0000 << 0 << 100;
            ECLJump(ecl, 0, 0x55c, 0);
            ecl << pair{0x5a4, 100};
            ECLJump(ecl, 0x5d8, 0x714, 0);
            break;
        case THPrac::TH17::TH17_ST7_MID3:
            ECLStdExec(ecl, 0xa208, 1, 1);
            ECLJump(ecl, 0, 0xa714, 60);
            ecl.SetFile(3);
            ecl.SetPos(0x410);
            ecl << 0 << 0x00140203 << 0x01ff0000 << 0 << 100;
            ECLJump(ecl, 0, 0x55c, 0);
            ecl << pair{0x5a4, 100};
            ECLJump(ecl, 0x5d8, 0x7b0, 0);
            break;
        case THPrac::TH17::TH17_ST7_END_NS1:
            ECLStdExec(ecl, 0xa208, 1, 1);
            if (thPracParam.dlg)
                ECLJump(ecl, 0, 0xa7c4, 60);
            else
                ECLJump(ecl, 0, 0xa7d8, 60);
            break;
        case THPrac::TH17::TH17_ST7_END_S1:
            ECLStdExec(ecl, 0xa208, 1, 1);
            ECLJump(ecl, 0, 0xa7d8, 60);
            ecl.SetFile(2);
            ECLJump(ecl, 0x800, 0x8a0, 3); // Utilize Spell Practice Jump
            ecl << pair{0x6354, (int16_t)0}; // Disable Item Drops
            break;
        case THPrac::TH17::TH17_ST7_END_NS2:
            ECLStdExec(ecl, 0xa208, 1, 1);
            ECLJump(ecl, 0, 0xa7d8, 60);
            ecl.SetFile(2);
            ecl << pair{0x1180, (int8_t)0x32}; // Change Nonspell
            ecl << pair{0x1cf0, (int16_t)0} << pair{0x1d28, (int16_t)0} << pair{0x1d60, (int16_t)0}
                << pair{0x1d98, (int16_t)0} << pair{0x1dd0, (int16_t)0}; // Disable Item Drops
            ecl << pair{0x1efc, (int16_t)0}; // Disable Item Drops & SE
            break;
        case THPrac::TH17::TH17_ST7_END_S2:
            ECLStdExec(ecl, 0xa208, 1, 1);
            ECLJump(ecl, 0, 0xa7d8, 60);
            ecl.SetFile(2);
            ECLJump(ecl, 0x800, 0x93c, 3); // Utilize Spell Practice Jump
            break;
        case THPrac::TH17::TH17_ST7_END_NS3:
            ECLStdExec(ecl, 0xa208, 1, 1);
            ECLJump(ecl, 0, 0xa7d8, 60);
            ecl.SetFile(2);
            ecl << pair{0x1180, (int8_t)0x33}; // Change Nonspell
            ecl << pair{0x25b0, (int16_t)0} << pair{0x25e8, (int16_t)0} << pair{0x2620, (int16_t)0}
                << pair{0x2658, (int16_t)0} << pair{0x2690, (int16_t)0}; // Disable Item Drops
            ecl << pair{0x27bc, (int16_t)0}; // Disable Item Drops & SE
            ecl << pair{0x24d8, 0} << pair{0x2954, 0};
            break;
        case THPrac::TH17::TH17_ST7_END_S3:
            ECLStdExec(ecl, 0xa208, 1, 1);
            ECLJump(ecl, 0, 0xa7d8, 60);
            ecl.SetFile(2);
            ECLJump(ecl, 0x800, 0x9d8, 3); // Utilize Spell Practice Jump
            break;
        case THPrac::TH17::TH17_ST7_END_NS4:
            ECLStdExec(ecl, 0xa208, 1, 1);
            ECLJump(ecl, 0, 0xa7d8, 60);
            ecl.SetFile(2);
            ecl << pair{0x1180, (int8_t)0x34}; // Change Nonspell
            ecl << pair{0x2eac, (int16_t)0} << pair{0x2ee4, (int16_t)0} << pair{0x2f1c, (int16_t)0}
                << pair{0x2f54, (int16_t)0} << pair{0x2f8c, (int16_t)0}; // Disable Item Drops
            ecl << pair{0x30b8, (int16_t)0}; // Disable Item Drops & SE
            ecl << pair{0x2dd4, 0} << pair{0x3260, 0};
            break;
        case THPrac::TH17::TH17_ST7_END_S4:
            ECLStdExec(ecl, 0xa208, 1, 1);
            ECLJump(ecl, 0, 0xa7d8, 60);
            ecl.SetFile(2);
            ECLJump(ecl, 0x800, 0xa74, 3); // Utilize Spell Practice Jump
            break;
        case THPrac::TH17::TH17_ST7_END_NS5:
            ECLStdExec(ecl, 0xa208, 1, 1);
            ECLJump(ecl, 0, 0xa7d8, 60);
            ecl.SetFile(2);
            ecl << pair{0x1180, (int8_t)0x35}; // Change Nonspell
            ecl << pair{0x37e4, (int16_t)0} << pair{0x381c, (int16_t)0} << pair{0x3854, (int16_t)0}
                << pair{0x388c, (int16_t)0} << pair{0x38c4, (int16_t)0}; // Disable Item Drops
            ecl << pair{0x39f0, (int16_t)0}; // Disable Item Drops & SE
            ecl << pair{0x370c, 0} << pair{0x3b88, 0};
            break;
        case THPrac::TH17::TH17_ST7_END_S5:
            ECLStdExec(ecl, 0xa208, 1, 1);
            ECLJump(ecl, 0, 0xa7d8, 60);
            ecl.SetFile(2);
            ECLJump(ecl, 0x800, 0xb10, 3); // Utilize Spell Practice Jump
            break;
        case THPrac::TH17::TH17_ST7_END_NS6:
            ECLStdExec(ecl, 0xa208, 1, 1);
            ECLJump(ecl, 0, 0xa7d8, 60);
            ecl.SetFile(2);
            ecl << pair{0x1180, (int8_t)0x36}; // Change Nonspell
            ecl << pair{0x4110, (int16_t)0} << pair{0x4148, (int16_t)0} << pair{0x4180, (int16_t)0}
                << pair{0x41b8, (int16_t)0} << pair{0x41f0, (int16_t)0}; // Disable Item Drops
            ecl << pair{0x431c, (int16_t)0}; // Disable Item Drops & SE
            ecl << pair{0x4008, 0} << pair{0x44c4, 0};
            break;
        case THPrac::TH17::TH17_ST7_END_S6:
            ECLStdExec(ecl, 0xa208, 1, 1);
            ECLJump(ecl, 0, 0xa7d8, 60);
            ecl.SetFile(2);
            ECLJump(ecl, 0x800, 0xbac, 3); // Utilize Spell Practice Jump
            break;
        case THPrac::TH17::TH17_ST7_END_NS7:
            ECLStdExec(ecl, 0xa208, 1, 1);
            ECLJump(ecl, 0, 0xa7d8, 60);
            ecl.SetFile(2);
            ecl << pair{0x1180, (int8_t)0x37}; // Change Nonspell
            ecl << pair{0x4a48, (int16_t)0} << pair{0x4a80, (int16_t)0} << pair{0x4ab8, (int16_t)0}
                << pair{0x4af0, (int16_t)0} << pair{0x4b28, (int16_t)0}; // Disable Item Drops
            ecl << pair{0x4c54, (int16_t)0}; // Disable Item Drops & SE
            ecl << pair{0x4970, 0} << pair{0x4dfc, 0};
            break;
        case THPrac::TH17::TH17_ST7_END_S7:
            ECLStdExec(ecl, 0xa208, 1, 1);
            ECLJump(ecl, 0, 0xa7d8, 60);
            ecl.SetFile(2);
            ECLJump(ecl, 0x800, 0xc48, 3); // Utilize Spell Practice Jump
            break;
        case THPrac::TH17::TH17_ST7_END_NS8:
            ECLStdExec(ecl, 0xa208, 1, 1);
            ECLJump(ecl, 0, 0xa7d8, 60);
            ecl.SetFile(2);
            ecl << pair{0x1180, (int8_t)0x38}; // Change Nonspell
            ecl << pair{0x5380, (int16_t)0} << pair{0x53b8, (int16_t)0} << pair{0x53f0, (int16_t)0}
                << pair{0x5428, (int16_t)0} << pair{0x5460, (int16_t)0}; // Disable Item Drops
            ecl << pair{0x558c, (int16_t)0}; // Disable Item Drops & SE
            ecl << pair{0x52a8, 0} << pair{0x5734, 0};
            break;
        case THPrac::TH17::TH17_ST7_END_S8:
            ECLStdExec(ecl, 0xa208, 1, 1);
            ECLJump(ecl, 0, 0xa7d8, 60);
            ecl.SetFile(2);
            ECLJump(ecl, 0x800, 0xce4, 3); // Utilize Spell Practice Jump
            break;
        case THPrac::TH17::TH17_ST7_END_S9:
            ECLStdExec(ecl, 0xa208, 1, 1);
            ECLJump(ecl, 0, 0xa7d8, 60);
            ecl.SetFile(2);
            ECLJump(ecl, 0x800, 0xd80, 3); // Utilize Spell Practice Jump
            break;
        case THPrac::TH17::TH17_ST7_END_S10:
            ECLStdExec(ecl, 0xa208, 1, 1);
            ECLJump(ecl, 0, 0xa7d8, 60);
            ecl.SetFile(2);
            ECLJump(ecl, 0x800, 0xe1c, 3); // Utilize Spell Practice Jump

            switch (thPracParam.phase) {
            case 1:
                ecl << pair{0xe2c, 7000};
                ECLJump(ecl, 0x6d3c, 0x6d84, 90);
                break;
            case 2:
                ecl << pair{0xe2c, 4500};
                ECLJump(ecl, 0x6d3c, 0x6d84, 90);
                ECLJump(ecl, 0x6dcc, 0x6e14, 90);
                break;
            case 3:
                ecl << pair{0xe2c, 2000};
                ECLJump(ecl, 0x6d3c, 0x6d84, 90);
                ECLJump(ecl, 0x6dcc, 0x6e14, 90);
                ECLJump(ecl, 0x6e38, 0x6e80, 90);
                break;
            case 4:
                ECLJump(ecl, 0x6d3c, 0x6d84, 90);
                ECLJump(ecl, 0x6dcc, 0x6e14, 90);
                ECLJump(ecl, 0x6e38, 0x6e80, 90);
                ECLJump(ecl, 0x6ec8, 0x6f10, 90);
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
        uint32_t spell_id = *(int32_t*)(0x4b5a14);
        switch (spell_id) {
        case 84:
        case 85:
        case 86:
        case 87:
            ecl.SetFile(2);
            switch (thPracParam.phase) {
            case 1:
                ecl << pair{0xb98, 7000};
                ECLJump(ecl, 0xf0c0, 0xf238, 120);
                break;
            case 2:
                ecl << pair{0xb98, 4500};
                ECLJump(ecl, 0xf0c0, 0xf238, 120);
                ECLJump(ecl, 0xf2d0, 0xf448, 120);
                break;
            case 3:
                ecl << pair{0xb98, 2500};
                ECLJump(ecl, 0xf0c0, 0xf238, 120);
                ECLJump(ecl, 0xf2d0, 0xf448, 120);
                ECLJump(ecl, 0xf508, 0xf680, 120);
                break;
            default:
                break;
            }
            break;
        case 100:
            ecl.SetFile(2);
            switch (thPracParam.phase) {
            case 1:
                ecl << pair{0xe2c, 7000};
                ECLJump(ecl, 0x6d3c, 0x6d84, 90);
                break;
            case 2:
                ecl << pair{0xe2c, 4500};
                ECLJump(ecl, 0x6d3c, 0x6d84, 90);
                ECLJump(ecl, 0x6dcc, 0x6e14, 90);
                break;
            case 3:
                ecl << pair{0xe2c, 2000};
                ECLJump(ecl, 0x6d3c, 0x6d84, 90);
                ECLJump(ecl, 0x6dcc, 0x6e14, 90);
                ECLJump(ecl, 0x6e38, 0x6e80, 90);
                break;
            case 4:
                ECLJump(ecl, 0x6d3c, 0x6d84, 90);
                ECLJump(ecl, 0x6dcc, 0x6e14, 90);
                ECLJump(ecl, 0x6e38, 0x6e80, 90);
                ECLJump(ecl, 0x6ec8, 0x6f10, 90);
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
        ecl.SetBaseAddr((void*)GetMemAddr(0x4b76a0, 0x17c, 0xC));

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
        else if (thPracParam.dlg)
            return 0;
        else
            return th_sections_bgm[thPracParam.section];
    }
    void THSaveReplay(char* repName)
    {
        ReplaySaveParam(mb_to_utf16(repName, 932).c_str(), thPracParam.GetJson());
    }

#define TH17AddGoast(goast_id) asm_call<0x40f980, Thiscall>(GetMemContent(0x4b7684), goast_id)

    HOOKSET_DEFINE(THMainHook)
    EHOOK_DY(th17_window_mousedown, 0x46198b) {
        auto& adv_opt = THAdvOptWnd::singleton();

        if (GetMemContent(0x4b7684) && adv_opt.IsClosed() && adv_opt.mSelectedGoast) {
            LPARAM lParam = GetMemContent(pCtx->Ebp + 0x14);
            HWND hWnd = (HWND)pCtx->Edx;

            // Someone could have a non-standard window size
            RECT clientRect;
            GetClientRect(hWnd, &clientRect);

            float gameX = LOWORD(lParam) / (clientRect.right / 640.0f);
            float gameY = HIWORD(lParam) / (clientRect.bottom / 480.0f);

            if (adv_opt.mForceGoastAngle && !adv_opt.mGoastAngleRandom) {
                th17_force_goast_angle::GetHook().Enable();
            }
            vec2f stgFramePos { gameX - 224.0f, gameY - 16.0f };
            if (stgFramePos.y > 128 && stgFramePos.y < 448
                && abs(stgFramePos.x) < 192) {
                float ang = adv_opt.mGoastAng;
                if (adv_opt.mGoastAngleRandom) {
                    ang = asm_call<0x402880, Thiscall, float>(0x4B7668) * M_PI;
                }
                SpawnToken(adv_opt.mSelectedGoast, stgFramePos, ang);
            }
        }
    }
    EHOOK_DY(th17_goast_bugfix, 0x430080)
    {
        if (THAdvOptWnd::singleton().mGoastBugfix && *(uint32_t*)0x4b59dc == *(uint32_t*)0x4b59e0 && GetMemContent(0x4b76b0, 0xa8) == 0)
            *(uint32_t*)0x4b5ac4 &= 0xfffffffd;
    }
    EHOOK_DY(th17_goast_repfix, 0x430c97)
    {
        if (THAdvOptWnd::singleton().mGoastRepfix && *(uint32_t*)0x4b59dc == *(uint32_t*)0x4b59e0 && GetMemContent(0x4b76b0, 0xa8) == 1) {
            uint32_t repStart = 1;
            uint32_t repData = GetMemContent(0x4b77d8);

            for (; repStart < 8; repStart++) {
                if (*(uint32_t*)(repData + repStart * 0x28 + 0xd8))
                    break;
            }

            if (repStart < 8 && *(uint32_t*)0x4b59dc == repStart)
                *(uint32_t*)0x4b5ac4 &= 0xfffffffd;
        }
    }
    EHOOK_DY(th17_inf_roar, 0x40ef37)
    {
        int32_t roar_meter = pCtx->Eax;
        if (*(THOverlay::singleton().mInfRoaring) && roar_meter > 40) {
            pCtx->Eip = 0x40ef3f;
        }
    }
    EHOOK_DY(th17_everlasting_bgm, 0x4662e0)
    {
        int32_t retn_addr = ((int32_t*)pCtx->Esp)[0];
        int32_t bgm_cmd = ((int32_t*)pCtx->Esp)[1];
        int32_t bgm_id = ((int32_t*)pCtx->Esp)[2];
        // 4th stack item = i32 call_addr

        bool el_switch;
        bool is_practice;
        bool result;

        el_switch = *(THOverlay::singleton().mElBgm) && !THGuiRep::singleton().mRepStatus && (thPracParam.mode == 1) && thPracParam.section;
        is_practice = (*((int32_t*)0x4b59c8) & 0x1);
        result = ElBgmTestTemp<0x4420f3, 0x42eab2, 0x4311f0, 0x443bbd, 0x445f12, 0xffffffff>(
            el_switch, is_practice, retn_addr, bgm_cmd, bgm_id, 0xffffffff);

        if (result) {
            pCtx->Eip = 0x466376;
        }
    }
    EHOOK_DY(th17_param_reset, 0x451ed0)
    {
        thPracParam.Reset();
    }
    EHOOK_DY(th17_prac_menu_1, 0x456970)
    {
        THGuiPrac::singleton().State(1);
    }
    EHOOK_DY(th17_prac_menu_2, 0x456993)
    {
        THGuiPrac::singleton().State(2);
    }
    EHOOK_DY(th17_prac_menu_3, 0x456bc7)
    {
        THGuiPrac::singleton().State(3);
    }
    EHOOK_DY(th17_prac_menu_4, 0x456c76)
    {
        THGuiPrac::singleton().State(4);
    }
    PATCH_DY(th17_prac_menu_enter_1, 0x456a59, "\xeb", 1);
    EHOOK_DY(th17_prac_menu_enter_2, 0x456c26)
    {
        pCtx->Ecx = thPracParam.stage;
    }
    EHOOK_DY(th17_disable_prac_menu_1, 0x456de4)
    {
        pCtx->Eip = 0x456e15;
    }
    EHOOK_DY(th17_menu_rank_fix, 0x445ed1)
    {
        *((int32_t*)0x4b5a00) = *((int32_t*)0x4b2b28); // Restore In-game rank to menu rank
    }
    EHOOK_DY(th17_patch_main, 0x430cb3)
    {
        if (thPracParam.mode == 1) {
            *(int32_t*)(0x4b59fc) = (int32_t)(thPracParam.score / 10);
            *(int32_t*)(0x4b5a40) = thPracParam.life;
            *(int32_t*)(0x4b5a44) = thPracParam.life_fragment;
            *(int32_t*)(0x4b5a4c) = thPracParam.bomb;
            *(int32_t*)(0x4b5a50) = thPracParam.bomb_fragment;
            *(int32_t*)(0x4b5a30) = thPracParam.power;
            *(int32_t*)(0x4b5a24) = thPracParam.value * 100;
            *(int32_t*)(0x4b5a0c) = thPracParam.graze;

            if (thPracParam.goast_1) {
                TH17AddGoast(thPracParam.goast_1);
                if (thPracParam.goast_2) {
                    TH17AddGoast(thPracParam.goast_2);
                    if (thPracParam.goast_3) {
                        TH17AddGoast(thPracParam.goast_3);
                        if (thPracParam.goast_4) {
                            TH17AddGoast(thPracParam.goast_4);
                            if (thPracParam.goast_5) {
                                TH17AddGoast(thPracParam.goast_5);
                            }
                        }
                    }
                }
            }

            THSectionPatch();
        } else if (thPracParam.mode == 2) {
            ECLHelper ecl;
            ecl.SetBaseAddr((void*)GetMemAddr(0x4b76a0, 0x17c, 0xC));
            THPatchSP(ecl);
        }
        thPracParam._playLock = true;
    }
    EHOOK_DY(th17_bgm, 0x431d97)
    {
        if (THBGMTest()) {
            PushHelper32(pCtx, 1);
            pCtx->Eip = 0x431d99;
        }
    }
    EHOOK_DY(th17_restart, 0x4451dc)
    {
        auto s1 = pCtx->Esp + 0x8;
        auto s2 = pCtx->Edi + 0x1e4;
        auto s3 = *(DWORD*)(pCtx->Edi + 0x1e8);

        asm_call<0x465450, Stdcall>(0x7, pCtx->Ecx);

        uint32_t* ret = asm_call<0x4769f0, Thiscall, uint32_t*>(s2, s1, 0x87, pCtx->Ecx);

        asm_call<0x476510, Stdcall>(*ret, 0x6);

        // Restart New 1
        asm_call<0x476510, Stdcall>(s3, 0x1);

        // Restart Mod 1
        asm_call<0x41b630, Thiscall>(pCtx->Esi, 0x4);

        // Restart New 2;
        asm_call<0x443800, Thiscall>(pCtx->Edi, 0x10);

        pCtx->Edx = *(DWORD*)0x4b323c;
        pCtx->Eip = 0x445208;
    }
    EHOOK_DY(th17_rep_save, 0x44eeaa)
    {
        char* repName = (char*)(pCtx->Esp + 0x28);
        if (thPracParam.mode == 1)
            THSaveReplay(repName);
        else if (thPracParam.mode == 2 && thPracParam.phase)
            THSaveReplay(repName);
    }
    EHOOK_DY(th17_rep_menu_1, 0x4570f6)
    {
        THGuiRep::singleton().State(1);
    }
    EHOOK_DY(th17_rep_menu_2, 0x457216)
    {
        THGuiRep::singleton().State(2);
    }
    EHOOK_DY(th17_rep_menu_3, 0x4573da)
    {
        THGuiRep::singleton().State(3);
    }
    EHOOK_DY(th17_sp_menu_1, 0x45c554)
    {
        if (!THGuiSP::singleton().State()) {
            pCtx->Eip = 0x45c58b;
        }
    }
    EHOOK_DY(th17_sp_menu_2, 0x45c4c4)
    {
        if (THGuiSP::singleton().mState) {
            pCtx->Eip = 0x45c554;
        }
    }
    EHOOK_DY(th17_update, 0x4013b5)
    {
        static int x = 0;
        x++;
        if (x<5)
            return;
        GameGuiBegin(IMPL_WIN32_DX9, !THAdvOptWnd::singleton().IsOpen());
        
        // Gui components update
        THGuiPrac::singleton().Update();
        THOverlay::singleton().Update();
        THGuiSP::singleton().Update();
        TH17InGameInfo::singleton().Update();
        bool drawCursor = THAdvOptWnd::StaticUpdate() || THGuiPrac::singleton().IsOpen() || THGuiSP::singleton().IsOpen();
        GameGuiEnd(drawCursor);
    }
    EHOOK_DY(th17_render, 0x4014d0)
    {
        GameGuiRender(IMPL_WIN32_DX9);
    }
    HOOKSET_ENDDEF()

    HOOKSET_DEFINE(THInitHook)
    static __declspec(noinline) void THGuiCreate()
    {

        // Init
        GameGuiInit(IMPL_WIN32_DX9, 0x4b5ae8, 0x5226c0, 0x4617d0,
            Gui::INGAGME_INPUT_GEN2, 0x4b323c, 0x4b3238, 0,
            (*((int32_t*)0x524700) >> 2) & 0xf);

        // Gui components creation
        THGuiPrac::singleton();
        THGuiPrac::singleton();
        THGuiPrac::singleton();
        THGuiPrac::singleton();
        TH17InGameInfo::singleton();

        // Hooks
        THMainHook::singleton().EnableAllHooks();

        Gui::ImplDX9NewFrame();
        // Reset thPracParam
        thPracParam.Reset();
    }
    static __declspec(noinline) void THInitHookDisable()
    {
        auto& s = THInitHook::singleton();
        s.th17_gui_init_1.Disable();
        s.th17_gui_init_2.Disable();
    }
    PATCH_DY(th17_disable_demo, 0x45167c, "\xff\xff\xff\x7f", 4);
    EHOOK_DY(th17_disable_mutex, 0x46055c)
    {
        pCtx->Eip = 0x46083e;
    }
    PATCH_DY(th17_startup_1, 0x4511af, "\x90\x90", 2);
    PATCH_DY(th17_startup_2, 0x451f32, "\xeb", 1);
    EHOOK_DY(th17_gui_init_1, 0x452502)
    {
        THGuiCreate();
        THInitHookDisable();
    }
    EHOOK_DY(th17_gui_init_2, 0x462e17)
    {
        THGuiCreate();
        THInitHookDisable();
    }

    
#pragma region igi
    EHOOK_DY(th17_game_start, 0x4302E6) // gamestart-bomb set
    {
        TH17InGameInfo::singleton().mMissCount = 0;
        TH17InGameInfo::singleton().mBombCount = 0;
        TH17InGameInfo::singleton().mRoarBreakCount = 0;
        TH17InGameInfo::singleton().mRoarCount = 0;
        TH17InGameInfo::singleton().mSpecialGoastCount = 0;
        TH17InGameInfo::singleton().mWolfCount = 0;
        TH17InGameInfo::singleton().mOtterCount = 0;
        TH17InGameInfo::singleton().mEagerCount = 0;
    }
    EHOOK_DY(th17_roar_break, 0x40F880)
    {
        TH17InGameInfo::singleton().mRoarBreakCount++;
    }
    EHOOK_DY(th17_roar, 0x40FC8A)
    {
        int32_t cur_roar = *(DWORD*)0x004B5ABC;
        TH17InGameInfo::singleton().mRoarCount++;
        switch (cur_roar)
        {
        case 1:
            TH17InGameInfo::singleton().mWolfCount++;
            break;
        case 2:
            TH17InGameInfo::singleton().mOtterCount++;
            break;
        case 3:
            TH17InGameInfo::singleton().mEagerCount++;
            break;
        default:
            break;
        }
        for (int32_t i = 0; i < 5; i++){
            int32_t type = *(int32_t*)(0x004B5A64 + 4 * i);
            if (type >= 8 && type <= 0xE){
                TH17InGameInfo::singleton().mSpecialGoastCount++;
            }
        }
    }
    EHOOK_DY(th17_life_dec, 0x44921B)
    {
        TH17InGameInfo::singleton().mMissCount++;
    }
    EHOOK_DY(th17_bomb_dec, 0x411CAB)
    {
        TH17InGameInfo::singleton().mBombCount++;
    }
#pragma endregion

    HOOKSET_ENDDEF()
}

void TH17Init()
{
    TH17::THInitHook::singleton().EnableAllHooks();
    TryKeepUpRefreshRate((void*)0x462f7a, (void*)0x462d3d);
}
}
