#include "thprac_games.h"
#include "thprac_utils.h"
#include <format>

#include "utils/wininternal.h"

// WTF Microsoft
#undef hyper

namespace THPrac {
namespace TH20 {
    using std::pair;
    using namespace TH20;
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

        float hyper;
        float stone;
        int32_t stoneMax;
        int32_t levelR;
        int32_t priorityR;
        int32_t levelB;
        int32_t priorityB;
        int32_t levelY;
        int32_t priorityY;
        int32_t levelG;
        int32_t priorityG;

        int32_t reimuR2Timer[7];
        int32_t passiveMeterTimer[7];

        bool dlg;

        bool _playLock = false;
        void Reset()
        {
            memset(this, 0, sizeof(THPracParam));
        }
        bool ReadJson(std::string& json)
        {
            ParseJson();

            ForceJsonValue(game, "th20");
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
            GetJsonValue(cycle);
            GetJsonValue(power);
            GetJsonValue(value);

            GetJsonValue(hyper);
            GetJsonValue(stone);
            GetJsonValue(stoneMax);
            GetJsonValue(levelR);
            GetJsonValue(priorityR);
            GetJsonValue(levelB);
            GetJsonValue(priorityB);
            GetJsonValue(levelY);
            GetJsonValue(priorityY);
            GetJsonValue(levelG);
            GetJsonValue(priorityG);

            GetJsonArray(reimuR2Timer, elementsof(reimuR2Timer));
            GetJsonArray(passiveMeterTimer, elementsof(passiveMeterTimer));

            return true;
        }
        std::string GetJson()
        {
            if (mode == 0) {
                CreateJson();

                AddJsonValueEx(version, GetVersionStr(), jalloc);
                AddJsonValueEx(game, "th20", jalloc);
                AddJsonValue(mode);

                AddJsonArray(reimuR2Timer, elementsof(reimuR2Timer));
                AddJsonArray(passiveMeterTimer, elementsof(passiveMeterTimer));

                ReturnJson();
            } else if (mode == 1) {
                CreateJson();

                AddJsonValueEx(version, GetVersionStr(), jalloc);
                AddJsonValueEx(game, "th20", jalloc);
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
                AddJsonValue(cycle);
                AddJsonValue(power);
                AddJsonValue(value);

                AddJsonValue(hyper);
                AddJsonValue(stone);
                AddJsonValue(stoneMax);
                AddJsonValue(levelR);
                AddJsonValue(priorityR);
                AddJsonValue(levelB);
                AddJsonValue(priorityB);
                AddJsonValue(levelY);
                AddJsonValue(priorityY);
                AddJsonValue(levelG);
                AddJsonValue(priorityG);

                ReturnJson();
            }

            CreateJson();
            jalloc; // Dummy usage to silence C4189
            ReturnJson();
        }
    };
    THPracParam thPracParam {};
    uint32_t replayStones[4] {};

    class THGuiPrac : public Gui::GameGuiWnd {
        THGuiPrac() noexcept
        {
            *mMode = 1;
            *mLife = 7;
            *mBomb = 7;
            *mPower = 400;
            *mValue = 0;
            *mLevelR = 0;
            *mLevelB = 0;
            *mLevelY = 0;
            *mLevelG = 0;
            *mPriorityR = 0;
            *mPriorityB = 0;
            *mPriorityY = 0;
            *mPriorityG = 0;

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
                mDiffculty = *((int32_t*)RVA(0x1B0A60));
                SetFade(0.8f, 0.1f);
                Open();
                thPracParam.Reset();
            case 2:
                break;
            case 3: {
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
                thPracParam.cycle = *mCycle;
                thPracParam.power = *mPower;
                thPracParam.value = *mValue;

                int stoneMaxStageDefault = 1100;
                if (*mStage == 1) stoneMaxStageDefault = 1200;
                if (*mStage == 2) stoneMaxStageDefault = 1300;
                if (*mStage >= 3) stoneMaxStageDefault = 1400;
                thPracParam.hyper = *mHyper / 10000.0f;
                thPracParam.stone = *mStone / 10000.0f;
                thPracParam.stoneMax = stoneMaxStageDefault + *mStoneSummoned * 150;
                thPracParam.levelR = *mLevelR;
                thPracParam.priorityR = *mPriorityR;
                thPracParam.levelB = *mLevelB;
                thPracParam.priorityB = *mPriorityB;
                thPracParam.levelG = *mLevelG;
                thPracParam.priorityG = *mPriorityG;
                thPracParam.levelY = *mLevelY;
                thPracParam.priorityY = *mPriorityY;
                break;
            }
            case 4:
                Close();
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
                SetSizeRel(0.65f, 0.81f);
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
            if (section == TH20_ST7_BOSS17)
                return TH20_SPELL_PHASE_TIMEOUT;
            if (section == TH20_ST6_BOSS12 || section == TH20_ST7_BOSS18)
                return TH_SPELL_PHASE2;
            return nullptr;
        }
        void PracticeMenu()
        {
            mMode();
            if (mStage())
                *mSection = *mChapter = 0;
            if (*mMode == 1) {
                int mbs = -1;
                if (*mStage == 2 || *mStage == 5) { // Counting from 0
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
                mLifeFragment();
                mBomb();
                mBombFragment();
                auto power_str = std::to_string((float)(*mPower) / 100.0f).substr(0, 4);
                mPower(power_str.c_str());
                auto value_str = std::format("{:.2f}", (float)(*mValue) / 5000.0f);
                mValue(value_str.c_str());
                mHyper(std::format("{:.2f} %%", (float)(*mHyper) / 100.0f).c_str());
                mStone(std::format("{:.2f} %%", (float)(*mStone) / 100.0f).c_str());
                mStoneSummoned();
                mCycle();

                ImGui::Columns(2);
                auto& style = ImGui::GetStyle();
                auto old_col = style.Colors[ImGuiCol_SliderGrab];
                auto old_col_active = style.Colors[ImGuiCol_SliderGrabActive];

                style.Colors[ImGuiCol_SliderGrab] = ImVec4(1.0f, 0.25f, 0.25f, 0.40f);
                style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(1.0f, 0.25f, 0.25f, 1.0f);
                mLevelR();
                ImGui::NextColumn();
                mPriorityR();

                style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.25f, 0.25f, 1.0f, 0.40f);
                style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.25f, 0.25f, 1.0f, 1.0f);
                ImGui::NextColumn();
                mLevelB();
                ImGui::NextColumn();
                mPriorityB();

                style.Colors[ImGuiCol_SliderGrab] = ImVec4(1.0f, 1.0f, 0.25f, 0.40f);
                style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(1.0f, 1.0f, 0.25f, 1.0f);
                ImGui::NextColumn();
                mLevelY();
                ImGui::NextColumn();
                mPriorityY();

                style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.25f, 1.0f, 0.25f, 0.40f);
                style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.0f, 1.0f, 0.25f, 1.0f);
                ImGui::NextColumn();
                mLevelG();
                ImGui::NextColumn();
                mPriorityG();

                style.Colors[ImGuiCol_SliderGrab] = old_col;
                style.Colors[ImGuiCol_SliderGrabActive] = old_col_active;
                ImGui::Columns(1);

                mScore();
                mScore.RoundDown(10);
            }
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
            case TH20_ST1_BOSS1:
            case TH20_ST2_BOSS1:
            case TH20_ST3_BOSS1:
            case TH20_ST4_BOSS1:
            case TH20_ST5_BOSS1:
            case TH20_ST6_BOSS1:
            case TH20_ST6_BOSS5:
            case TH20_ST6_BOSS9:
            case TH20_ST7_MID1:
            case TH20_ST7_BOSS1:
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
        Gui::GuiCombo mCycle { TH14_CYCLE, TH20_CYCLE_LIST };

        Gui::GuiSlider<int, ImGuiDataType_S32> mChapter { TH_CHAPTER, 0, 0 };
        Gui::GuiDrag<int64_t, ImGuiDataType_S64> mScore { TH_SCORE, 0, 9999999990, 10, 100000000 };
        Gui::GuiSlider<int, ImGuiDataType_S32> mLife { TH_LIFE, 0, 7 };
        Gui::GuiSlider<int, ImGuiDataType_S32> mLifeFragment { TH_LIFE_FRAGMENT, 0, 2 };
        Gui::GuiSlider<int, ImGuiDataType_S32> mBomb { TH_BOMB, 0, 7 };
        Gui::GuiSlider<int, ImGuiDataType_S32> mBombFragment { TH_BOMB_FRAGMENT, 0, 2 };
        Gui::GuiSlider<int, ImGuiDataType_S32> mPower { TH_POWER, 100, 400 };
        Gui::GuiSlider<int, ImGuiDataType_S32> mValue { TH_VALUE, 0, 1000000 };

        Gui::GuiSlider<int, ImGuiDataType_S32> mHyper { TH20_HYPER, 0, 10000, 1, 1000 };
        Gui::GuiSlider<int, ImGuiDataType_S32> mStone { TH20_STONE_GAUGE, 0, 10000, 1, 1000 };
        Gui::GuiSlider<int, ImGuiDataType_S32> mStoneSummoned { TH20_STONE_SUMMONED, 0, 26 };
        Gui::GuiSlider<int32_t, ImGuiDataType_S32> mLevelR { TH20_STONE_LEVEL_R, 0, 5 };
        Gui::GuiSlider<int32_t, ImGuiDataType_S32> mPriorityR { TH20_STONE_PRIORITY_R, 0, 1000 };
        Gui::GuiSlider<int32_t, ImGuiDataType_S32> mLevelB { TH20_STONE_LEVEL_B, 0, 5 };
        Gui::GuiSlider<int32_t, ImGuiDataType_S32> mPriorityB { TH20_STONE_PRIORITY_B, 0, 1000 };
        Gui::GuiSlider<int32_t, ImGuiDataType_S32> mLevelY { TH20_STONE_LEVEL_Y, 0, 5 };
        Gui::GuiSlider<int32_t, ImGuiDataType_S32> mPriorityY { TH20_STONE_PRIORITY_Y, 0, 1000 };
        Gui::GuiSlider<int32_t, ImGuiDataType_S32> mLevelG { TH20_STONE_LEVEL_G, 0, 5 };
        Gui::GuiSlider<int32_t, ImGuiDataType_S32> mPriorityG { TH20_STONE_PRIORITY_G, 0, 1000 };

        int mChapterSetup[7][2] {
            { 3, 2 },
            { 3, 3 },
            { 4, 3 },
            { 5, 2 },
            { 5, 2 },
            { 4, 1 },
            { 4, 3 },
        };

        int mDiffculty = 0;
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
            uint32_t index = GetMemContent(RVA(0x1C6124), 0x5738);
            char* repName = (char*)GetMemAddr(RVA(0x1C6124), index * 4 + 0x5740, 0x260);
            std::wstring repDir(mAppdataPath);
            repDir.append(L"\\ShanghaiAlice\\th20\\replay\\");
            repDir.append(mb_to_utf16(repName, 932));

            std::string param;
            if (ReplayLoadParam(repDir.c_str(), param) && mRepParam.ReadJson(param))
                mParamStatus = true;
            else
                mRepParam.Reset();

            uint32_t* savedStones = (uint32_t*)GetMemAddr(RVA(0x1C6124), index * 4 + 0x5740, 0x1C, 0xDC);
            memcpy(replayStones, savedStones, sizeof(replayStones));
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
            mHyperGLock.SetTextOffsetRel(x_offset_1, x_offset_2);
            mWonderStGLock.SetTextOffsetRel(x_offset_1, x_offset_2);
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
            mHyperGLock();
            mWonderStGLock();
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

        HOTKEY_DEFINE(mMuteki, TH_MUTEKI, "F1", VK_F1)
        PATCH_HK(0xF87FC, "01"),
        PATCH_HK(0xF8734, "EB")
        HOTKEY_ENDDEF();

        HOTKEY_DEFINE(mInfLives, TH_INFLIVES, "F2", VK_F2)
        PATCH_HK(0xF849D, "00")
        HOTKEY_ENDDEF();

        HOTKEY_DEFINE(mInfBombs, TH_INFBOMBS, "F3", VK_F3)
        PATCH_HK(0xE1722, "0F1F00")
        HOTKEY_ENDDEF();

        HOTKEY_DEFINE(mInfPower, TH_INFPOWER, "F4", VK_F4)
        PATCH_HK(0xE16A2, "0F1F00") //0xE16A8
        HOTKEY_ENDDEF();

        HOTKEY_DEFINE(mHyperGLock, TH20_HYP_LOCK, "F5", VK_F5)
        PATCH_HK(0x133935, "0F1F00")
        HOTKEY_ENDDEF();

        HOTKEY_DEFINE(mWonderStGLock, TH20_WST_LOCK, "F6", VK_F6)
        PATCH_HK(0x77F75, "0F1F00")
        HOTKEY_ENDDEF();

        HOTKEY_DEFINE(mTimeLock, TH_TIMELOCK, "F7", VK_F7)
        PATCH_HK(0x86FDD, "EB"),
        PATCH_HK(0xA871E, "31")
        HOTKEY_ENDDEF();

        HOTKEY_DEFINE(mAutoBomb, TH_AUTOBOMB, "F8", VK_F8)
        PATCH_HK(0xF79AC, "0F"),
        PATCH_HK(0xF79B3, "0F")
        HOTKEY_ENDDEF();
    public:
        Gui::GuiHotKey mElBgm { TH_EL_BGM, "F9", VK_F9 };
    };

    // TODO(?)
    //static constinit HookCtx listIterUnlinkFix = { .addr = 0x11AB2, .data = PatchCode("e800000000") };
    EHOOK_ST(th20_piv_overflow_fix, 0xC496B, 2, {
        uintptr_t stats = RVA(0x1BA5F0);
        int32_t piv = *(int32_t*)(stats + 0x44);
        int32_t piv_base = *(int32_t*)(stats + 0x40); // always 10000?
        int32_t piv_divisor = *(int32_t*)(stats + 0x48); // always 5000?

        int32_t half_piv_base = piv_base / 2;
        int64_t uh_oh = (int64_t)piv_base * piv;
        pCtx->Esi = (int32_t)(uh_oh / piv_divisor) + half_piv_base;
        pCtx->Eip = RVA(0xC49C4);
    });
    PATCH_ST(th20_piv_uncap_1, 0xA9FE5, "89D00F1F00");
    PATCH_ST(th20_piv_uncap_2, 0xB82E6, "89D00F1F00");
    PATCH_ST(th20_score_uncap, 0xE14F2, "EB");
    EHOOK_ST(th20_score_uncap_stage_tr, 0x10910F, 2, {
        pCtx->Esi += 8;
        pCtx->Edi += 8;
        pCtx->Ecx -= 2;
    });
    PATCH_ST(th20_infinite_stones, 0x11784B, "EB");
    PATCH_ST(th20_hitbox_scale_fix, 0xFF490, "B864000000C3");

    class THAdvOptWnd : public Gui::PPGuiWnd {
        SINGLETON(THAdvOptWnd);

    public:
    private:
    private:
        bool pivOverflowFix = false;
        bool pivUncap = false;
        bool scoreUncap = false;
        bool infiniteStones = false;
        bool plHitboxScaleFix = false;

        void MasterDisableInit()
        {
        }
        void FpsInit()
        {
            mOptCtx.fps_dbl = 60.0;

            if (*(uint8_t*)RVA(0x1C4F89) == 3) {
                mOptCtx.fps_status = 1;

                DWORD oldProtect;
                VirtualProtect((void*)RVA(0x19EAF), 4, PAGE_EXECUTE_READWRITE, &oldProtect);
                *(double**)RVA(0x19EAF) = &mOptCtx.fps_dbl;
                VirtualProtect((void*)RVA(0x19EAF), 4, oldProtect, &oldProtect);
            } else
                mOptCtx.fps_status = 0;
        }
        void FpsSet()
        {
            if (mOptCtx.fps_status == 1) {
                mOptCtx.fps_dbl = (double)mOptCtx.fps;
            }
        }
        void GameplayInit()
        {
        }
        void GameplaySet()
        {
        }

        // From zero318
        struct ZUNListIter;
        struct ZUNListIterable;

        struct ZUNList {
            void* data; // 0x0
            ZUNList* next; // 0x4
            ZUNList* prev; // 0x8
            ZUNListIterable* list; // 0xC
            ZUNListIter* current_iter; // 0x10
        };

        struct ZUNListIter {
            ZUNList* current; // 0x0
            ZUNList* next; // 0x4
        };

        struct ZUNListIterable : ZUNList {
            ZUNList* tail; // 0x14
            ZUNListIter iter; // 0x18
        };

        static void __fastcall UnlinkNodeHook(ZUNListIterable* self, void*, ZUNList* node)
        {
            memset(&self->iter, 0, sizeof(self->iter));
            asm_call_rel<0x11AD0, Thiscall>(self, node);
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
            MasterDisableInit();

            th20_piv_overflow_fix.Setup();
            th20_piv_uncap_1.Setup();
            th20_piv_uncap_2.Setup();
            th20_score_uncap.Setup();
            th20_score_uncap_stage_tr.Setup();
            th20_infinite_stones.Setup();
            th20_hitbox_scale_fix.Setup();

            // TODO(?)
            /*
            // thcrap base_tsa already patches this to fix the crash, don't try to rehook it if it's being used
            if (*(uint32_t*)RVA(0x11AD0) == 0x51EC8B55) {
                *(uintptr_t*)((uintptr_t)listIterUnlinkFix.data.buffer.ptr + 1) = (uintptr_t)&UnlinkNodeHook - RVA(0x11AB2 + 5);
                listIterUnlinkFix.Setup();
                listIterUnlinkFix.Enable();
            }
            */
        }

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
            ImGui::TextUnformatted(S(TH_ADV_OPT));
            ImGui::Separator();
            ImGui::BeginChild("Adv. Options", ImVec2(0.0f, 0.0f));

            if (BeginOptGroup<TH_GAME_SPEED>()) {
                if (GameFPSOpt(mOptCtx))
                    FpsSet();
                EndOptGroup();
            }
            if (BeginOptGroup<TH_GAMEPLAY>()) {
                if (ImGui::Checkbox(S(TH20_PIV_OVERFLOW_FIX), &pivOverflowFix)) {
                    th20_piv_overflow_fix.Toggle(pivOverflowFix);
                    th20_score_uncap_stage_tr.Toggle(scoreUncap || pivOverflowFix || pivUncap);
                }
                ImGui::SameLine();
                if (ImGui::Checkbox(S(TH20_UNCAP_PIV), &pivUncap)) {
                    th20_piv_uncap_1.Toggle(pivUncap);
                    th20_piv_uncap_2.Toggle(pivUncap);
                    th20_score_uncap_stage_tr.Toggle(scoreUncap || pivOverflowFix || pivUncap);
                }
                ImGui::SameLine();
                if (ImGui::Checkbox(S(TH20_UNCAP_SCORE), &scoreUncap)) {
                    th20_score_uncap.Toggle(scoreUncap);
                    th20_score_uncap_stage_tr.Toggle(scoreUncap || pivOverflowFix || pivUncap);
                }

                if (ImGui::Checkbox(S(TH20_FAKE_UNLOCK_STONES), &infiniteStones))
                    th20_infinite_stones.Toggle(infiniteStones);
                ImGui::SameLine();
                HelpMarker(S(TH20_FAKE_UNLOCK_STONES_DESC));
                ImGui::SameLine();
                if (ImGui::Checkbox(S(TH20_FIX_HITBOX), &plHitboxScaleFix))
                    th20_hitbox_scale_fix.Toggle(plHitboxScaleFix);
                ImGui::SameLine();
                HelpMarker(S(TH20_FIX_HITBOX_DESC));

                ImGui::SetNextItemWidth(180.0f);
                EndOptGroup();
            }

            AboutOpt("Guy, zero318, rue, and you!");
            ImGui::EndChild();
            ImGui::SetWindowFocus();
        }

        adv_opt_ctx mOptCtx;
    };

    void ECLStdExec(ECLHelper& ecl, unsigned int start, int std_id, int ecl_time = 0)
    {
        if (start)
            ecl.SetPos(start);
        ecl << ecl_time << 0x0014026e << 0x01ff0000 << 0x00000000 << std_id;
    }
    void ECLJump(ECLHelper& ecl, unsigned int start, unsigned int dest, int at_frame, int ecl_time = 0)
    {
        ecl.SetPos(start);
        ecl << ecl_time << 0x0018000C << 0x02ff0000 << 0x00000000 << dest - start << at_frame;
    }

    constexpr unsigned int st1PostMaple = 0x7eec;
    constexpr unsigned int st2PostMaple = 0xa45c;
    constexpr unsigned int st3PostMaple = 0xb5bc;
    constexpr unsigned int st4PostMaple = 0xa740;
    constexpr unsigned int st5PostMaple = 0x5058;
    constexpr unsigned int st6PostMaple = 0x4b80;
    constexpr unsigned int st7PostMaple = 0x6b28;
    constexpr unsigned int stdInterruptSize = 0x14;
    __declspec(noinline) void THStageWarp(ECLHelper& ecl, int stage, int portion)
    {
        if (stage == 1) {
            constexpr unsigned int st1MainFront = 0x84e4;
            constexpr unsigned int st1MainSub00 = 0x5ad8;
            constexpr unsigned int st1MainLatter = 0x85ac;
            constexpr unsigned int st1MainLatterWait = 0x5d28;

            switch (portion) {
            case 1:
                break;
            case 2: {
                constexpr unsigned int mainSub02Call = 0x5b98;
                ECLJump(ecl, st1PostMaple, st1MainFront, 60, 90);
                ECLJump(ecl, st1MainSub00, mainSub02Call, 0, 0);
                break;
            }
            case 3: {
                constexpr unsigned int mainSub03Call = 0x5bf8;
                ECLJump(ecl, st1PostMaple, st1MainFront, 60, 90);
                ECLJump(ecl, st1MainSub00, mainSub03Call, 0, 0);
                break;
            }
            case 4: {
                constexpr unsigned int mainSub07Call = 0x5d3c;
                ECLStdExec(ecl, st1PostMaple, 1, 1);
                ECLJump(ecl, st1PostMaple + stdInterruptSize, st1MainLatter, 60, 90);
                ECLJump(ecl, st1MainLatterWait, mainSub07Call, 0, 0);
                break;
            }
            case 5: {
                constexpr unsigned int mainSub09Call = 0x5de8;
                ECLStdExec(ecl, st1PostMaple, 1, 1);
                ECLJump(ecl, st1PostMaple + stdInterruptSize, st1MainLatter, 60, 90);
                ECLJump(ecl, st1MainLatterWait, mainSub09Call, 0, 0);
                break;
            }
            default:
                break;
            }
        } else if (stage == 2) {
            constexpr unsigned int st2MainFront = 0xaa54;
            constexpr unsigned int st2MainSub00 = 0x7778;
            constexpr unsigned int st2MainLatter = 0xab1c;
            constexpr unsigned int st2MainLatterWait = 0x798c;

            switch (portion) {
            case 1:
                break;
            case 2: {
                constexpr unsigned int mainSub02Call = 0x7838;
                ECLJump(ecl, st2PostMaple, st2MainFront, 60, 90);
                ECLJump(ecl, st2MainSub00, mainSub02Call, 0, 0);
                break;
            }
            case 3: {
                constexpr unsigned int mainSub04Call = 0x78f8;
                ECLJump(ecl, st2PostMaple, st2MainFront, 60, 90);
                ECLJump(ecl, st2MainSub00, mainSub04Call, 0, 0);
                break;
            }
            case 4: {
                constexpr unsigned int mainSub07Call = 0x79a0;
                ECLStdExec(ecl, st2PostMaple, 1, 1);
                ECLJump(ecl, st2PostMaple + stdInterruptSize, st2MainLatter, 60, 90);
                ECLJump(ecl, st2MainLatterWait, mainSub07Call, 0, 0);
                break;
            }
            case 5: {
                constexpr unsigned int mainSub08Call = 0x79ec;
                ECLStdExec(ecl, st2PostMaple, 1, 1);
                ECLJump(ecl, st2PostMaple + stdInterruptSize, st2MainLatter, 60, 90);
                ECLJump(ecl, st2MainLatterWait, mainSub08Call, 0, 0);
                break;
            }
            case 6: {
                constexpr unsigned int mainSub09Call = 0x7a4c;
                ECLStdExec(ecl, st2PostMaple, 1, 1);
                ECLJump(ecl, st2PostMaple + stdInterruptSize, st2MainLatter, 60, 90);
                ECLJump(ecl, st2MainLatterWait, mainSub09Call, 0, 0);
                break;
            }
            default:
                break;
            }
        } else if (stage == 3) {
            constexpr unsigned int st3MainFront = 0xbbb4;
            constexpr unsigned int st3MainSub00 = 0x904c;

            switch (portion) {
            case 1:
                break;
            case 2: {
                constexpr unsigned int mainSub01Call = 0x90ac;
                ECLJump(ecl, st3PostMaple, st3MainFront, 60, 90);
                ECLJump(ecl, st3MainSub00, mainSub01Call, 0, 0);
                break;
            }
            case 3: {
                constexpr unsigned int mainSub03Call = 0x916c;
                ECLJump(ecl, st3PostMaple, st3MainFront, 60, 90);
                ECLJump(ecl, st3MainSub00, mainSub03Call, 0, 0);
                break;
            }
            case 4: {
                constexpr unsigned int mainSub04Call = 0x91cc;
                ECLJump(ecl, st3PostMaple, st3MainFront, 60, 90);
                ECLJump(ecl, st3MainSub00, mainSub04Call, 0, 0);
                break;
            }
            case 5: {
                constexpr unsigned int mainSub05Call = 0x922c;
                ECLStdExec(ecl, st3PostMaple, 1, 1);
                ECLJump(ecl, st3PostMaple + stdInterruptSize, st3MainFront, 60, 90);
                ECLJump(ecl, st3MainSub00, mainSub05Call, 0, 0);
                break;
            }
            case 6: {
                constexpr unsigned int mainSub06Call = 0x928c;
                ECLStdExec(ecl, st3PostMaple, 1, 1);
                ECLJump(ecl, st3PostMaple + stdInterruptSize, st3MainFront, 60, 90);
                ECLJump(ecl, st3MainSub00, mainSub06Call, 0, 0);
                break;
            }
            case 7: {
                constexpr unsigned int mainSub07Call = 0x92ec;
                ECLStdExec(ecl, st3PostMaple, 1, 1);
                ECLJump(ecl, st3PostMaple + stdInterruptSize, st3MainFront, 60, 90);
                ECLJump(ecl, st3MainSub00, mainSub07Call, 0, 0);
                break;
            }
            default:
                break;
            }
        } else if (stage == 4) {
            constexpr unsigned int st4MainFront = 0xad38;
            constexpr unsigned int st4MainSub00 = 0x3a5c;
            constexpr unsigned int st4MainLatter = 0xae00;
            constexpr unsigned int st4MainLatterWait = 0x3d04;

            switch (portion) {
            case 1:
                break;
            case 2: {
                constexpr unsigned int mainSub01Call = 0x3abc;
                ECLJump(ecl, st4PostMaple, st4MainFront, 60, 90);
                ECLJump(ecl, st4MainSub00, mainSub01Call, 0, 0);
                break;
            }
            case 3: {
                constexpr unsigned int mainSub02Call = 0x3b1c;
                ECLJump(ecl, st4PostMaple, st4MainFront, 60, 90);
                ECLJump(ecl, st4MainSub00, mainSub02Call, 0, 0);
                break;
            }
            case 4: {
                constexpr unsigned int mainSub04Call = 0x3bdc;
                ECLJump(ecl, st4PostMaple, st4MainFront, 60, 90);
                ECLJump(ecl, st4MainSub00, mainSub04Call, 0, 0);
                break;
            }
            case 5: {
                constexpr unsigned int mainSub05Call = 0x3c3c;
                ECLJump(ecl, st4PostMaple, st4MainFront, 60, 90);
                ECLJump(ecl, st4MainSub00, mainSub05Call, 0, 0);
                break;
            }
            case 6: {
                constexpr unsigned int mainSub07Call = 0x3d18;
                ECLStdExec(ecl, st4PostMaple, 1, 1);
                ECLJump(ecl, st4PostMaple + stdInterruptSize, st4MainLatter, 60, 90);
                ECLJump(ecl, st4MainLatterWait, mainSub07Call, 0, 0);
                break;
            }
            case 7: {
                constexpr unsigned int mainSub08Call = 0x3d78;
                ECLStdExec(ecl, st4PostMaple, 1, 1);
                ECLJump(ecl, st4PostMaple + stdInterruptSize, st4MainLatter, 60, 90);
                ECLJump(ecl, st4MainLatterWait, mainSub08Call, 0, 0);
                break;
            }
            default:
                break;
            }
        } else if (stage == 5) {
            constexpr unsigned int st5MainFront = 0x5650;
            constexpr unsigned int st5MainSub00 = 0x29c0;
            constexpr unsigned int st5MainLatter = 0x5718;
            constexpr unsigned int st5MainLatterWait = 0x2c08;

            switch (portion) {
            case 1:
                break;
            case 2: {
                constexpr unsigned int mainSub01Call = 0x2a20;
                ECLJump(ecl, st5PostMaple, st5MainFront, 60, 90);
                ECLJump(ecl, st5MainSub00, mainSub01Call, 0, 0);
                break;
            }
            case 3: {
                constexpr unsigned int mainSub02Call = 0x2a80;
                ECLJump(ecl, st5PostMaple, st5MainFront, 60, 90);
                ECLJump(ecl, st5MainSub00, mainSub02Call, 0, 0);
                break;
            }
            case 4: {
                constexpr unsigned int mainSub03Call = 0x2ae0;
                ECLJump(ecl, st5PostMaple, st5MainFront, 60, 90);
                ECLJump(ecl, st5MainSub00, mainSub03Call, 0, 0);
                break;
            }
            case 5: {
                constexpr unsigned int mainSub04Call = 0x2b40;
                ECLJump(ecl, st5PostMaple, st5MainFront, 60, 90);
                ECLJump(ecl, st5MainSub00, mainSub04Call, 0, 0);
                break;
            }
            case 6: {
                constexpr unsigned int mainSub06Call = 0x2c1c;
                ECLJump(ecl, st5PostMaple + stdInterruptSize, st5MainLatter, 60, 90);
                ECLJump(ecl, st5MainLatterWait, mainSub06Call, 0, 0);
                break;
            }
            case 7: {
                constexpr unsigned int mainSub07Call = 0x2cb4;
                ECLStdExec(ecl, st5PostMaple, 1, 1);
                ECLJump(ecl, st5PostMaple + stdInterruptSize, st5MainLatter, 60, 90);
                ECLJump(ecl, st5MainLatterWait, mainSub07Call, 0, 0);
                break;
            }
            default:
                break;
            }
        } else if (stage == 6) {
            constexpr unsigned int st6MainFront = 0x5178;
            constexpr unsigned int st6MainSub00 = 0x20bc;

            switch (portion) {
            case 1:
                break;
            case 2: {
                constexpr unsigned int mainSub01Call = 0x211c;
                ECLJump(ecl, st6PostMaple, st6MainFront, 60, 90);
                ECLJump(ecl, st6MainSub00, mainSub01Call, 0, 0);
                break;
            }
            case 3: {
                constexpr unsigned int mainSub02Call = 0x217c;
                ECLJump(ecl, st6PostMaple, st6MainFront, 60, 90);
                ECLJump(ecl, st6MainSub00, mainSub02Call, 0, 0);
                break;
            }
            case 4: {
                constexpr unsigned int mainSub03Call = 0x21dc;
                ECLJump(ecl, st6PostMaple, st6MainFront, 60, 90);
                ECLJump(ecl, st6MainSub00, mainSub03Call, 0, 0);
                break;
            }
            case 5: {
                constexpr unsigned int mainSub05Call = 0x229c;
                ECLStdExec(ecl, st6PostMaple, 1, 1);
                ECLJump(ecl, st6PostMaple + stdInterruptSize, st6MainFront, 60, 90);
                ECLJump(ecl, st6MainSub00, mainSub05Call, 0, 0);
                break;
            }
            default:
                break;
            }
        } else if (stage == 7) {
            constexpr unsigned int st7MainFront = 0x72b8;
            constexpr unsigned int st7MainSub00 = 0x3954;
            constexpr unsigned int st7MainLatter = 0x73b0;
            constexpr unsigned int st7MainLatterWait = 0x3af4;

            switch (portion) {
            case 1:
                break;
            case 2: {
                constexpr unsigned int mainSub01Call = 0x39b4;
                ECLJump(ecl, st7PostMaple, st7MainFront, 60, 90);
                ECLJump(ecl, st7MainSub00, mainSub01Call, 0, 0);
                break;
            }
            case 3: {
                constexpr unsigned int mainSub02Call = 0x3a14;
                ECLJump(ecl, st7PostMaple, st7MainFront, 60, 90);
                ECLJump(ecl, st7MainSub00, mainSub02Call, 0, 0);
                break;
            }
            case 4: {
                constexpr unsigned int mainSub03Call = 0x3a74;
                ECLJump(ecl, st7PostMaple, st7MainFront, 60, 90);
                ECLJump(ecl, st7MainSub00, mainSub03Call, 0, 0);
                break;
            }
            case 5: {
                constexpr unsigned int mainSub04Call = 0x3b08;
                ECLStdExec(ecl, st7PostMaple, 1, 1);
                ECLJump(ecl, st7PostMaple + stdInterruptSize, st7MainLatter, 60, 90);
                ECLJump(ecl, st7MainLatterWait, mainSub04Call, 0, 0);
                break;
            }
            case 6: {
                constexpr unsigned int mainSub05Call = 0x3b68;
                ECLStdExec(ecl, st7PostMaple, 1, 1);
                ECLJump(ecl, st7PostMaple + stdInterruptSize, st7MainLatter, 60, 90);
                ECLJump(ecl, st7MainLatterWait, mainSub05Call, 0, 0);
                break;
            }
            case 7: {
                constexpr unsigned int mainSub06Call = 0x3bec;
                ECLStdExec(ecl, st7PostMaple, 1, 1);
                ECLJump(ecl, st7PostMaple + stdInterruptSize, st7MainLatter, 60, 90);
                ECLJump(ecl, st7MainLatterWait, mainSub06Call, 0, 0);
                break;
            }
            default:
                break;
            }
        }
    }
    __declspec(noinline) void THPatch(ECLHelper& ecl, th_sections_t section)
    {
        constexpr unsigned int st1BossCreateCall = 0x8680;
        constexpr unsigned int st1bsPrePushSpellID = 0x3b8;
        constexpr unsigned int st1bsPostNotSpellPracCheck = 0x4a0;
        constexpr unsigned int st1bsSpellHealthVal = 0x4b0;
        constexpr unsigned int st1bsSpellSubCallOrd = 0x4d0;

        constexpr unsigned int st2BossCreateCall = 0xabf0;
        constexpr unsigned int st2bsPrePushSpellID = 0x3d0;
        constexpr unsigned int st2bsPostNotSpellPracCheck = 0x4b8;
        constexpr unsigned int st2bsSpellHealthVal = 0x4c8;
        constexpr unsigned int st2bsSpellSubCallOrd = 0x4e8;
        constexpr unsigned int st2bsNonSubCallOrd = 0x6f4 + 0x18;

        constexpr unsigned int st3BossCreateCall = 0xbc74;
        constexpr unsigned int st3bsPrePushSpellID = 0x458;
        constexpr unsigned int st3bsPostNotSpellPracCheck = 0x540;
        constexpr unsigned int st3bsSpellHealthVal = 0x550;
        constexpr unsigned int st3bsSpellSubCallOrd = 0x570;
        constexpr unsigned int st3bsNonSubCallOrd = 0x880 + 0x18;

        constexpr unsigned int st4MBossCreateCall = 0xadbc;
        constexpr unsigned int st4mbsPreChargeAnim = 0x310;
        constexpr unsigned int st4mbsPostChargeAnim = 0x3a8;
        constexpr unsigned int st4mbsPreWait = 0x3c8;
        constexpr unsigned int st4mbsPostWait = 0x3dc;
        constexpr unsigned int st4mbsPreWait2 = 0x47c;
        constexpr unsigned int st4mbsPostWait2 = 0x490;

        constexpr unsigned int st4BossCreateCall = 0xaed4;
        constexpr unsigned int st4bsPrePushSpellID = 0x41c;
        constexpr unsigned int st4bsPostNotSpellPracCheck = 0x504;
        constexpr unsigned int st4bsSpellHealthVal = 0x514;
        constexpr unsigned int st4bsSpellSubCallOrd = 0x534;
        constexpr unsigned int st4bsNonSubCallOrd = 0x844 + 0x18;

        constexpr unsigned int st5BossCreateCall = 0x57ec;
        constexpr unsigned int st5bsPrePushSpellID = 0x424;
        constexpr unsigned int st5bsPostNotSpellPracCheck = 0x50c;
        constexpr unsigned int st5bsSpellHealthVal = 0x51c;
        constexpr unsigned int st5bsSpellSubCallOrd = 0x53c;
        constexpr unsigned int st5bsNonSubCallOrd = 0x84c + 0x18;

        constexpr unsigned int st6BossCreateCall = 0x52c4;
        constexpr unsigned int st6bsPrePushSpellID = 0x534;
        constexpr unsigned int st6bsPostNotSpellPracCheck = 0x61c;
        constexpr unsigned int st6bsSpellHealthVal = 0x62c;
        constexpr unsigned int st6bsSpellSubCallOrd = 0x64c;
        constexpr unsigned int st6bsNonSubCallOrd = 0xc88 + 0x18;
        constexpr unsigned int st6bsSetYPos = 0x324 + 0x14;

        constexpr unsigned int st7MBossCreateCall = 0x7324;
        constexpr unsigned int st7MBossPreMsg = 0x7354;
        constexpr unsigned int st7MBossPostMsg = 0x73a0;
        constexpr unsigned int st7mbsPrePushSpellID = 0x464;
        constexpr unsigned int st7mbsPostNotSpellPracCheck = 0x4b4;
        constexpr unsigned int st7mbsNonSubCallOrd = 0x4c8 + 0x1d;

        constexpr unsigned int st7BossCreateCall = 0x7484;
        constexpr unsigned int st7bsPrePushSpellID = 0x5d0;
        constexpr unsigned int st7bsPostNotSpellPracCheck = 0x6b8;
        constexpr unsigned int st7bsSpellHealthVal = 0x6b8 + 0x10;
        constexpr unsigned int st7bsSpellSubCallOrd = 0x6cc + 0x1c;
        constexpr unsigned int st7bsNonSubCallOrd = 0x1010 + 0x18;
        constexpr unsigned int st7bsSetYPos = 0x3c0 + 0x14;

        switch (section) {
        case THPrac::TH20::TH20_ST1_MID1: {
            constexpr unsigned int st1MBossCreateCall = 0x8568;
            ECLStdExec(ecl, st1PostMaple, 1, 1);
            ECLJump(ecl, st1PostMaple + stdInterruptSize, st1MBossCreateCall, 60, 90);
            break;
        }
        case THPrac::TH20::TH20_ST1_BOSS1: {
            constexpr unsigned int st1BossDialogueCall = 0x866c;
            ECLStdExec(ecl, st1PostMaple, 1, 1);
            if (thPracParam.dlg)
                ECLJump(ecl, st1PostMaple + stdInterruptSize, st1BossDialogueCall, 60);
            else
                ECLJump(ecl, st1PostMaple + stdInterruptSize, st1BossCreateCall, 60);
            break;
        }
        case THPrac::TH20::TH20_ST1_BOSS2: {
            ECLStdExec(ecl, st1PostMaple, 1, 1);
            ECLJump(ecl, st1PostMaple + stdInterruptSize, st1BossCreateCall, 60);
            ecl.SetFile(2);
            ECLJump(ecl, st1bsPrePushSpellID, st1bsPostNotSpellPracCheck, 1); // Utilize Spell Practice Jump
            break;
        }
        case THPrac::TH20::TH20_ST1_BOSS3: {
            constexpr unsigned int st1bsNonSubCallOrd = 0x6dc + 0x18;
            constexpr unsigned int st1bsNon2InvulnCallVal = 0x133c + 0x10;
            constexpr unsigned int st1bsNon2BossItemCallSomething = 0x1428 + 0x4; // 32th cringequit on me when
            constexpr unsigned int st1bsNon2PlaySoundSomething = 0x1554 + 0x4; // I asked what these were so :shrug:
            constexpr unsigned int st1bsNon2PostLifeMarker = 0x16c0;
            constexpr unsigned int st1bsNon2PostWait = 0x17ac; // 0x1608 previously

            ECLStdExec(ecl, st1PostMaple, 1, 1);
            ECLJump(ecl, st1PostMaple + stdInterruptSize, st1BossCreateCall, 60);
            ecl.SetFile(2);
            ecl << pair { st1bsNonSubCallOrd, (int8_t)0x32 }; // Set nonspell ID in sub call to '2'
            ecl << pair { st1bsNon2InvulnCallVal, (int16_t)0 }; // Disable Invincible
            ecl << pair { st1bsNon2BossItemCallSomething, (int16_t)0 }; // Disable item drops
            ecl << pair { st1bsNon2PlaySoundSomething, (int16_t)0 }; // Disable sound effect
            ECLJump(ecl, st1bsNon2PostLifeMarker, st1bsNon2PostWait, 0); // Skip wait
            break;
        }
        case THPrac::TH20::TH20_ST1_BOSS4: {
            ECLStdExec(ecl, st1PostMaple, 1, 1);
            ECLJump(ecl, st1PostMaple + stdInterruptSize, st1BossCreateCall, 60);
            ecl.SetFile(2);
            ECLJump(ecl, st1bsPrePushSpellID, st1bsPostNotSpellPracCheck, 1); // Utilize Spell Practice Jump
            ecl << pair { st1bsSpellHealthVal, 2000 }; // Set correct health (set in skipped non)
            ecl << pair { st1bsSpellSubCallOrd, (int8_t)0x32 }; // Set spell ID in sub call to '2'
            break;
        }
        case THPrac::TH20::TH20_ST1_BOSS5: {
            ECLStdExec(ecl, st1PostMaple, 1, 1);
            ECLJump(ecl, st1PostMaple + stdInterruptSize, st1BossCreateCall, 60);
            ecl.SetFile(2);
            ECLJump(ecl, st1bsPrePushSpellID, st1bsPostNotSpellPracCheck, 1); // Utilize Spell Practice Jump
            ecl << pair { st1bsSpellHealthVal, 2500 }; // Set correct health (set in skipped non)
            ecl << pair { st1bsSpellSubCallOrd, (int8_t)0x33 }; // Set spell ID in sub call to '3'
            break;
        }

        case THPrac::TH20::TH20_ST2_MID1: {
            constexpr unsigned int st2MBossCreateCall = 0xaad8;
            ECLJump(ecl, st2PostMaple, st2MBossCreateCall, 60, 90);
            break;
        }
        case THPrac::TH20::TH20_ST2_BOSS1: {
            constexpr unsigned int st2BossDialogueCall = 0xabdc;
            ECLStdExec(ecl, st2PostMaple, 1, 1);
            if (thPracParam.dlg)
                ECLJump(ecl, st2PostMaple + stdInterruptSize, st2BossDialogueCall, 60);
            else
                ECLJump(ecl, st2PostMaple + stdInterruptSize, st2BossCreateCall, 60);
            break;
        }
        case THPrac::TH20::TH20_ST2_BOSS2: {
            ECLStdExec(ecl, st2PostMaple, 1, 1);
            ECLJump(ecl, st2PostMaple + stdInterruptSize, st2BossCreateCall, 60);
            ecl.SetFile(2);
            ECLJump(ecl, st2bsPrePushSpellID, st2bsPostNotSpellPracCheck, 1); // Utilize Spell Practice Jump
            break;
        }
        case THPrac::TH20::TH20_ST2_BOSS3: {
            constexpr unsigned int st2bsNon2InvulnCallVal = 0x12a8 + 0x10;
            constexpr unsigned int st2bsNon2BossItemCallSomething = 0x1394 + 0x4;
            constexpr unsigned int st2bsNon2PlaySoundSomething = 0x14c0 + 0x4;
            constexpr unsigned int st2bsNon2PostLifeMarker = 0x162c;
            constexpr unsigned int st2bsNon2PostWait = 0x1718;

            ECLStdExec(ecl, st2PostMaple, 1, 1);
            ECLJump(ecl, st2PostMaple + stdInterruptSize, st2BossCreateCall, 60);
            ecl.SetFile(2);
            ecl << pair { st2bsNonSubCallOrd, (int8_t)0x32 }; // Set nonspell ID in sub call to '2'
            ecl << pair { st2bsNon2InvulnCallVal, (int16_t)0 }; // Disable Invincible
            ecl << pair { st2bsNon2BossItemCallSomething, (int16_t)0 }; // Disable item drops
            ecl << pair { st2bsNon2PlaySoundSomething, (int16_t)0 }; // Disable sound effect
            ECLJump(ecl, st2bsNon2PostLifeMarker, st2bsNon2PostWait, 0); // Skip wait
            break;
        }
        case THPrac::TH20::TH20_ST2_BOSS4: {
            ECLStdExec(ecl, st2PostMaple, 1, 1);
            ECLJump(ecl, st2PostMaple + stdInterruptSize, st2BossCreateCall, 60);
            ecl.SetFile(2);
            ECLJump(ecl, st2bsPrePushSpellID, st2bsPostNotSpellPracCheck, 1); // Utilize Spell Practice Jump
            ecl << pair { st2bsSpellHealthVal, 2600 }; // Set correct health (set in skipped non)
            ecl << pair { st2bsSpellSubCallOrd, (int8_t)0x32 }; // Set spell ID in sub call to '2'
            break;
        }
        case THPrac::TH20::TH20_ST2_BOSS5: {
            ECLStdExec(ecl, st2PostMaple, 1, 1);
            ECLJump(ecl, st2PostMaple + stdInterruptSize, st2BossCreateCall, 60);
            ecl.SetFile(2);
            ECLJump(ecl, st2bsPrePushSpellID, st2bsPostNotSpellPracCheck, 1); // Utilize Spell Practice Jump
            ecl << pair { st2bsSpellHealthVal, 3000 }; // Set correct health (set in skipped non)
            ecl << pair { st2bsSpellSubCallOrd, (int8_t)0x33 }; // Set spell ID in sub call to '3'
            break;
        }

        case THPrac::TH20::TH20_ST3_BOSS1: {
            constexpr unsigned int st3BossDialogueCall = 0xbc60;
            ECLStdExec(ecl, st3PostMaple, 1, 1);
            if (thPracParam.dlg)
                ECLJump(ecl, st3PostMaple + stdInterruptSize, st3BossDialogueCall, 60);
            else
                ECLJump(ecl, st3PostMaple + stdInterruptSize, st3BossCreateCall, 60);
            break;
        }
        case THPrac::TH20::TH20_ST3_BOSS2: {
            ECLStdExec(ecl, st3PostMaple, 1, 1);
            ECLJump(ecl, st3PostMaple + stdInterruptSize, st3BossCreateCall, 60);
            ecl.SetFile(2);
            ECLJump(ecl, st3bsPrePushSpellID, st3bsPostNotSpellPracCheck, 1); // Utilize Spell Practice Jump
            break;
        }
        case THPrac::TH20::TH20_ST3_BOSS3: {
            constexpr unsigned int st3bsNon2InvulnCallVal = 0x127c + 0x10;
            constexpr unsigned int st3bsNon2BossItemCallSomething = 0x1368 + 0x4;
            constexpr unsigned int st3bsNon2PlaySoundSomething = 0x1494 + 0x4;
            constexpr unsigned int st3bsNon2PostLifeMarker = 0x1600;
            constexpr unsigned int st3bsNon2PostWait = 0x172c;

            ECLStdExec(ecl, st3PostMaple, 1, 1);
            ECLJump(ecl, st3PostMaple + stdInterruptSize, st3BossCreateCall, 60);
            ecl.SetFile(2);
            ecl << pair { st3bsNonSubCallOrd, (int8_t)0x32 }; // Set nonspell ID in sub call to '2'
            ecl << pair { st3bsNon2InvulnCallVal, (int16_t)0 }; // Disable Invincible
            ecl << pair { st3bsNon2BossItemCallSomething, (int16_t)0 }; // Disable item drops
            ecl << pair { st3bsNon2PlaySoundSomething, (int16_t)0 }; // Disable sound effect
            ECLJump(ecl, st3bsNon2PostLifeMarker, st3bsNon2PostWait, 0); // Skip wait
            break;
        }
        case THPrac::TH20::TH20_ST3_BOSS4: {
            ECLStdExec(ecl, st3PostMaple, 1, 1);
            ECLJump(ecl, st3PostMaple + stdInterruptSize, st3BossCreateCall, 60);
            ecl.SetFile(2);
            ECLJump(ecl, st3bsPrePushSpellID, st3bsPostNotSpellPracCheck, 1); // Utilize Spell Practice Jump
            ecl << pair { st3bsSpellHealthVal, 3200 }; // Set correct health (set in skipped non)
            ecl << pair { st3bsSpellSubCallOrd, (int8_t)0x32 }; // Set spell ID in sub call to '2'
            break;
        }
        case THPrac::TH20::TH20_ST3_BOSS5: {
            constexpr unsigned int st3bsNon3InvulnCallVal = 0x1f28 + 0x10;
            constexpr unsigned int st3bsNon3BossItemCallSomething = 0x2014 + 0x4;
            constexpr unsigned int st3bsNon3PlaySoundSomething = 0x2140 + 0x4;
            constexpr unsigned int st3bsNon3PostLifeCount = 0x22c0; // is this right? life count = 0? zun...
            constexpr unsigned int st3bsNon3PostWait = 0x23ec;

            ECLStdExec(ecl, st3PostMaple, 1, 1);
            ECLJump(ecl, st3PostMaple + stdInterruptSize, st3BossCreateCall, 60);
            ecl.SetFile(2);
            ecl << pair { st3bsNonSubCallOrd, (int8_t)0x33 }; // Set nonspell ID in sub call to '3'
            ecl << pair { st3bsNon3InvulnCallVal, (int16_t)0 }; // Disable Invincible
            ecl << pair { st3bsNon3BossItemCallSomething, (int16_t)0 }; // Disable item drops
            ecl << pair { st3bsNon3PlaySoundSomething, (int16_t)0 }; // Disable sound effect
            ECLJump(ecl, st3bsNon3PostLifeCount, st3bsNon3PostWait, 0); // Skip wait
            break;
        }
        case THPrac::TH20::TH20_ST3_BOSS6: {
            ECLStdExec(ecl, st3PostMaple, 1, 1);
            ECLJump(ecl, st3PostMaple + stdInterruptSize, st3BossCreateCall, 60);
            ecl.SetFile(2);
            ECLJump(ecl, st3bsPrePushSpellID, st3bsPostNotSpellPracCheck, 1); // Utilize Spell Practice Jump
            ecl << pair { st3bsSpellHealthVal, 3400 }; // Set correct health (set in skipped non)
            ecl << pair { st3bsSpellSubCallOrd, (int8_t)0x33 }; // Set spell ID in sub call to '3'
            break;
        }
        case THPrac::TH20::TH20_ST3_BOSS7: {
            ECLStdExec(ecl, st3PostMaple, 1, 1);
            ECLJump(ecl, st3PostMaple + stdInterruptSize, st3BossCreateCall, 60);
            ecl.SetFile(2);
            ECLJump(ecl, st3bsPrePushSpellID, st3bsPostNotSpellPracCheck, 1); // Utilize Spell Practice Jump
            ecl << pair { st3bsSpellHealthVal, 3400 }; // Set correct health (set in skipped non)
            ecl << pair { st3bsSpellSubCallOrd, (int8_t)0x34 }; // Set spell ID in sub call to '4'
            break;
        }

        case THPrac::TH20::TH20_ST4_MID1: {
            ECLJump(ecl, st4PostMaple, st4MBossCreateCall, 60, 90);
            ecl.SetFile(3);
            ECLJump(ecl, st4mbsPreChargeAnim, st4mbsPostChargeAnim, 0);
            ECLJump(ecl, st4mbsPreWait, st4mbsPostWait, 0);
            ECLJump(ecl, st4mbsPreWait2, st4mbsPostWait2, 0);
            break;
        }
        case THPrac::TH20::TH20_ST4_MID2: {
            constexpr unsigned int st4mbsNonSubCallOrd = 0x4a4 + 0x19;
            constexpr unsigned int st4mbsNon2BossItemCallSomething = 0xd40 + 0x4;
            constexpr unsigned int st4mbsNon2PlaySoundSomething = 0xe6c + 0x4;
            constexpr unsigned int st4mbsNon2PreWait = 0x1028;
            constexpr unsigned int st4mbsNon2PostWait = 0x103c;
            constexpr unsigned int st4mbsNon2PreWait2 = 0x1028;
            constexpr unsigned int st4mbsNon2PostWait2 = 0x103c;
            constexpr unsigned int st4mbsNon2BulletClear = 0x1a8 + 0x4;

            ECLJump(ecl, st4PostMaple, st4MBossCreateCall, 60, 90);
            ecl.SetFile(3);
            ECLJump(ecl, st4mbsPreChargeAnim, st4mbsPostChargeAnim, 0);
            ecl << pair { st4mbsNonSubCallOrd, (int8_t)0x32 }; // Set nonspell ID in sub call to '2'
            ecl << pair { st4mbsNon2BossItemCallSomething, (int16_t)0 }; // Disable item drops
            ecl << pair { st4mbsNon2PlaySoundSomething, (int16_t)0 }; // Disable sound effect
            ecl << pair { st4mbsNon2BulletClear, (int16_t)0 }; // Disable bullet clear
            ECLJump(ecl, st4mbsNon2PreWait, st4mbsNon2PostWait, 0); // Skip wait
            ECLJump(ecl, st4mbsPreWait, st4mbsPostWait, 0);
            ECLJump(ecl, st4mbsPreWait2, st4mbsPostWait2, 0);
            break;
        }
        case THPrac::TH20::TH20_ST4_BOSS1: {
            constexpr unsigned int st4BossDialogueCall = 0xaec0;
            ECLStdExec(ecl, st4PostMaple, 1, 1);
            if (thPracParam.dlg)
                ECLJump(ecl, st4PostMaple + stdInterruptSize, st4BossDialogueCall, 60);
            else
                ECLJump(ecl, st4PostMaple + stdInterruptSize, st4BossCreateCall, 60);
            break;
        }
        case THPrac::TH20::TH20_ST4_BOSS2: {
            ECLStdExec(ecl, st4PostMaple, 1, 1);
            ECLJump(ecl, st4PostMaple + stdInterruptSize, st4BossCreateCall, 60);
            ecl.SetFile(2);
            ECLJump(ecl, st4bsPrePushSpellID, st4bsPostNotSpellPracCheck, 1); // Utilize Spell Practice Jump
            break;
        }
        case THPrac::TH20::TH20_ST4_BOSS3: {
            constexpr unsigned int st4bsNon2InvulnCallVal = 0x1540 + 0x10;
            constexpr unsigned int st4bsNon2BossItemCallSomething = 0x162c + 0x4;
            constexpr unsigned int st4bsNon2PlaySoundSomething = 0x1758 + 0x4;
            constexpr unsigned int st4bsNon2PostLifeMarker = 0x18c4;
            constexpr unsigned int st4bsNon2PostWait1 = 0x1970;
            constexpr unsigned int st4bsNon2PostInit = 0x19e8;
            constexpr unsigned int st4bsNon2PostWait2 = 0x1a40;

            ECLStdExec(ecl, st4PostMaple, 1, 1);
            ECLJump(ecl, st4PostMaple + stdInterruptSize, st4BossCreateCall, 60);
            ecl.SetFile(2);
            ecl << pair { st4bsNonSubCallOrd, (int8_t)0x32 }; // Set nonspell ID in sub call to '2'
            ecl << pair { st4bsNon2InvulnCallVal, (int16_t)0 }; // Disable Invincible
            ecl << pair { st4bsNon2BossItemCallSomething, (int16_t)0 }; // Disable item drops
            ecl << pair { st4bsNon2PlaySoundSomething, (int16_t)0 }; // Disable sound effect
            ECLJump(ecl, st4bsNon2PostLifeMarker, st4bsNon2PostWait1, 0); // Skip wait 1
            ECLJump(ecl, st4bsNon2PostInit, st4bsNon2PostWait2, 0); // Skip wait 2
            break;
        }
        case THPrac::TH20::TH20_ST4_BOSS4: {
            ECLStdExec(ecl, st4PostMaple, 1, 1);
            ECLJump(ecl, st4PostMaple + stdInterruptSize, st4BossCreateCall, 60);
            ecl.SetFile(2);
            ECLJump(ecl, st4bsPrePushSpellID, st4bsPostNotSpellPracCheck, 1); // Utilize Spell Practice Jump
            ecl << pair { st4bsSpellSubCallOrd, (int8_t)0x32 }; // Set spell ID in sub call to '2'
            break;
        }
        case THPrac::TH20::TH20_ST4_BOSS5: {
            constexpr unsigned int st4bsNon3InvulnCallVal = 0x2584 + 0x10;
            constexpr unsigned int st4bsNon3BossItemCallSomething = 0x2670 + 0x4;
            constexpr unsigned int st4bsNon3PlaySoundSomething = 0x279c + 0x4;
            constexpr unsigned int st4bsNon3PostLifeCount = 0x291c;
            constexpr unsigned int st4bsNon3PostWait1 = 0x29c8;
            constexpr unsigned int st4bsNon3PostInit = 0x2a40;
            constexpr unsigned int st4bsNon3PostWait2 = 0x2a98;

            ECLStdExec(ecl, st4PostMaple, 1, 1);
            ECLJump(ecl, st4PostMaple + stdInterruptSize, st4BossCreateCall, 60);
            ecl.SetFile(2);
            ecl << pair { st4bsNonSubCallOrd, (int8_t)0x33 }; // Set nonspell ID in sub call to '3'
            ecl << pair { st4bsNon3InvulnCallVal, (int16_t)0 }; // Disable Invincible
            ecl << pair { st4bsNon3BossItemCallSomething, (int16_t)0 }; // Disable item drops
            ecl << pair { st4bsNon3PlaySoundSomething, (int16_t)0 }; // Disable sound effect
            ECLJump(ecl, st4bsNon3PostLifeCount, st4bsNon3PostWait1, 0); // Skip wait 1
            ECLJump(ecl, st4bsNon3PostInit, st4bsNon3PostWait2, 0); // Skip wait 2
            break;
        }
        case THPrac::TH20::TH20_ST4_BOSS6: {
            ECLStdExec(ecl, st4PostMaple, 1, 1);
            ECLJump(ecl, st4PostMaple + stdInterruptSize, st4BossCreateCall, 60);
            ecl.SetFile(2);
            ECLJump(ecl, st4bsPrePushSpellID, st4bsPostNotSpellPracCheck, 1); // Utilize Spell Practice Jump
            ecl << pair { st4bsSpellHealthVal, 2500 }; // Set correct health (set in skipped non)
            ecl << pair { st4bsSpellSubCallOrd, (int8_t)0x33 }; // Set spell ID in sub call to '3'
            break;
        }
        case THPrac::TH20::TH20_ST4_BOSS7: {
            ECLStdExec(ecl, st4PostMaple, 1, 1);
            ECLJump(ecl, st4PostMaple + stdInterruptSize, st4BossCreateCall, 60);
            ecl.SetFile(2);
            ECLJump(ecl, st4bsPrePushSpellID, st4bsPostNotSpellPracCheck, 1); // Utilize Spell Practice Jump
            ecl << pair { st4bsSpellHealthVal, 4300 }; // Set correct health (set in skipped non)
            ecl << pair { st4bsSpellSubCallOrd, (int8_t)0x34 }; // Set spell ID in sub call to '4'
            break;
        }

        case THPrac::TH20::TH20_ST5_MID1: {
            constexpr unsigned int st5MBossCreateCall = 0x56d4;
            constexpr unsigned int st5mbsPreChargeAnim = 0x33c;
            constexpr unsigned int st5mbsPostChargeAnim = 0x3d4;
            constexpr unsigned int st5mbsPreWait = 0x3f4;
            constexpr unsigned int st5mbsPostWait = 0x408;
            constexpr unsigned int st5mbsPreWait2 = 0x474;
            constexpr unsigned int st5mbsPostWait2 = 0x488;

            ECLJump(ecl, st5PostMaple, st5MBossCreateCall, 0);
            ecl.SetFile(3);
            ECLJump(ecl, st5mbsPreChargeAnim, st5mbsPostChargeAnim, 0);
            ECLJump(ecl, st5mbsPreWait, st5mbsPostWait, 0);
            ECLJump(ecl, st5mbsPreWait2, st5mbsPostWait2, 0);
            break;
        }
        case THPrac::TH20::TH20_ST5_BOSS1: {
            constexpr unsigned int st5BossDialogueCall = 0x57d8;
            ECLStdExec(ecl, st5PostMaple, 1, 1);
            if (thPracParam.dlg)
                ECLJump(ecl, st5PostMaple + stdInterruptSize, st5BossDialogueCall, 60);
            else
                ECLJump(ecl, st5PostMaple + stdInterruptSize, st5BossCreateCall, 60);
            break;
        }
        case THPrac::TH20::TH20_ST5_BOSS2: {
            ECLStdExec(ecl, st5PostMaple, 1, 1);
            ECLJump(ecl, st5PostMaple + stdInterruptSize, st5BossCreateCall, 60);
            ecl.SetFile(2);
            ECLJump(ecl, st5bsPrePushSpellID, st5bsPostNotSpellPracCheck, 1); // Utilize Spell Practice Jump
            break;
        }
        case THPrac::TH20::TH20_ST5_BOSS3: {
            constexpr unsigned int st5bsNon2InvulnCallVal = 0x1470 + 0x10;
            constexpr unsigned int st5bsNon2BossItemCallSomething = 0x155c + 0x4;
            constexpr unsigned int st5bsNon2PlaySoundSomething = 0x1688 + 0x4;
            constexpr unsigned int st5bsNon2PostLifeMarker = 0x17f4;
            constexpr unsigned int st5bsNon2PostWait = 0x18a0;

            ECLStdExec(ecl, st5PostMaple, 1, 1);
            ECLJump(ecl, st5PostMaple + stdInterruptSize, st5BossCreateCall, 60);
            ecl.SetFile(2);
            ecl << pair { st5bsNonSubCallOrd, (int8_t)0x32 }; // Set nonspell ID in sub call to '2'
            ecl << pair { st5bsNon2InvulnCallVal, (int16_t)0 }; // Disable Invincible
            ecl << pair { st5bsNon2BossItemCallSomething, (int16_t)0 }; // Disable item drops
            ecl << pair { st5bsNon2PlaySoundSomething, (int16_t)0 }; // Disable sound effect
            ECLJump(ecl, st5bsNon2PostLifeMarker, st5bsNon2PostWait, 0); // Skip wait
            break;
        }
        case THPrac::TH20::TH20_ST5_BOSS4: {
            ECLStdExec(ecl, st5PostMaple, 1, 1);
            ECLJump(ecl, st5PostMaple + stdInterruptSize, st5BossCreateCall, 60);
            ecl.SetFile(2);
            ECLJump(ecl, st5bsPrePushSpellID, st5bsPostNotSpellPracCheck, 1); // Utilize Spell Practice Jump
            ecl << pair { st5bsSpellHealthVal, 3200 }; // Set correct health (set in skipped non)
            ecl << pair { st5bsSpellSubCallOrd, (int8_t)0x32 }; // Set spell ID in sub call to '2'
            break;
        }
        case THPrac::TH20::TH20_ST5_BOSS5: {
            constexpr unsigned int st5bsNon3InvulnCallVal = 0x2498 + 0x10;
            constexpr unsigned int st5bsNon3BossItemCallSomething = 0x2584 + 0x4;
            constexpr unsigned int st5bsNon3PlaySoundSomething = 0x26b0 + 0x4;
            constexpr unsigned int st5bsNon3PostLifeCount = 0x2830;
            constexpr unsigned int st5bsNon3PostWait = 0x28c8;

            ECLStdExec(ecl, st5PostMaple, 1, 1);
            ECLJump(ecl, st5PostMaple + stdInterruptSize, st5BossCreateCall, 60);
            ecl.SetFile(2);
            ecl << pair { st5bsNonSubCallOrd, (int8_t)0x33 }; // Set nonspell ID in sub call to '3'
            ecl << pair { st5bsNon3InvulnCallVal, (int16_t)0 }; // Disable Invincible
            ecl << pair { st5bsNon3BossItemCallSomething, (int16_t)0 }; // Disable item drops
            ecl << pair { st5bsNon3PlaySoundSomething, (int16_t)0 }; // Disable sound effect
            ECLJump(ecl, st5bsNon3PostLifeCount, st5bsNon3PostWait, 0); // Skip wait
            break;
        }
        case THPrac::TH20::TH20_ST5_BOSS6: {
            ECLStdExec(ecl, st5PostMaple, 1, 1);
            ECLJump(ecl, st5PostMaple + stdInterruptSize, st5BossCreateCall, 60);
            ecl.SetFile(2);
            ECLJump(ecl, st5bsPrePushSpellID, st5bsPostNotSpellPracCheck, 1); // Utilize Spell Practice Jump
            ecl << pair { st5bsSpellHealthVal, 3200 }; // Set correct health (set in skipped non)
            ecl << pair { st5bsSpellSubCallOrd, (int8_t)0x33 }; // Set spell ID in sub call to '3'
            break;
        }
        case THPrac::TH20::TH20_ST5_BOSS7: {
            ECLStdExec(ecl, st5PostMaple, 1, 1);
            ECLJump(ecl, st5PostMaple + stdInterruptSize, st5BossCreateCall, 60);
            ecl.SetFile(2);
            ECLJump(ecl, st5bsPrePushSpellID, st5bsPostNotSpellPracCheck, 1); // Utilize Spell Practice Jump
            ecl << pair { st5bsSpellHealthVal, 3200 }; // Set correct health (set in skipped non)
            ecl << pair { st5bsSpellSubCallOrd, (int8_t)0x34 }; // Set spell ID in sub call to '4'
            break;
        }

        case THPrac::TH20::TH20_ST6_BOSS1: {
            constexpr unsigned int st6BossDialogueCall = 0x52b0;
            ECLStdExec(ecl, st6PostMaple, 1, 1);
            if (thPracParam.dlg)
                ECLJump(ecl, st6PostMaple + stdInterruptSize, st6BossDialogueCall, 60);
            else
                ECLJump(ecl, st6PostMaple + stdInterruptSize, st6BossCreateCall, 60);
            break;
        }
        case THPrac::TH20::TH20_ST6_BOSS2: {
            ECLStdExec(ecl, st6PostMaple, 1, 1);
            ECLJump(ecl, st6PostMaple + stdInterruptSize, st6BossCreateCall, 60);
            ecl.SetFile(2);
            ECLJump(ecl, st6bsPrePushSpellID, st6bsPostNotSpellPracCheck, 1); // Utilize Spell Practice Jump
            break;
        }
        case THPrac::TH20::TH20_ST6_BOSS3: {
            constexpr unsigned int st6bsNon2InvulnCallVal = 0x1448 + 0x10;
            constexpr unsigned int st6bsNon2BossItemCallSomething = 0x1534 + 0x4;
            constexpr unsigned int st6bsNon2PlaySoundSomething = 0x1660 + 0x4;
            constexpr unsigned int st6bsNon2PostProtectRange = 0x17e0;
            constexpr unsigned int st6bsNon2PostWait = 0x188c;

            ECLStdExec(ecl, st6PostMaple, 1, 1);
            ECLJump(ecl, st6PostMaple + stdInterruptSize, st6BossCreateCall, 60);
            ecl.SetFile(2);
            ecl << pair { st6bsSetYPos, (float)128.0f }; // Fix boss starting too high
            ecl << pair { st6bsNonSubCallOrd, (int8_t)0x32 }; // Set nonspell ID in sub call to '2'
            ecl << pair { st6bsNon2InvulnCallVal, (int16_t)0 }; // Disable Invincible
            ecl << pair { st6bsNon2BossItemCallSomething, (int16_t)0 }; // Disable item drops
            ecl << pair { st6bsNon2PlaySoundSomething, (int16_t)0 }; // Disable sound effect
            ECLJump(ecl, st6bsNon2PostProtectRange, st6bsNon2PostWait, 0); // Skip wait
            break;
        }
        case THPrac::TH20::TH20_ST6_BOSS4: {
            ECLStdExec(ecl, st6PostMaple, 1, 1);
            ECLJump(ecl, st6PostMaple + stdInterruptSize, st6BossCreateCall, 60);
            ecl.SetFile(2);
            ECLJump(ecl, st6bsPrePushSpellID, st6bsPostNotSpellPracCheck, 1); // Utilize Spell Practice Jump
            ecl << pair { st6bsSpellSubCallOrd, (int8_t)0x32 }; // Set spell ID in sub call to '2'
            break;
        }
        case THPrac::TH20::TH20_ST6_BOSS5: {
            constexpr unsigned int st6bsNon3InvulnCallVal = 0x1ec8 + 0x10;
            constexpr unsigned int st6bsNon3BossItemCallSomething = 0x21f8 + 0x4;
            constexpr unsigned int st6bsNon3PlaySoundSomething = 0x1660 + 0x4;
            constexpr unsigned int st6bsNon3PostLifeCount = 0x2300;
            constexpr unsigned int st6bsNon3PostWait = 0x2398;

            constexpr unsigned int st6bsNon3PreDialogue = 0x225c;
            constexpr unsigned int st6bsNon3PostDialogue = 0x2294;

            ECLStdExec(ecl, st6PostMaple, 1, 1);
            ECLJump(ecl, st6PostMaple + stdInterruptSize, st6BossCreateCall, 60);
            ecl.SetFile(2);
            ecl << pair { st6bsSetYPos, (float)128.0f }; // Fix boss starting too high
            ecl << pair { st6bsNonSubCallOrd, (int8_t)0x33 }; // Set nonspell ID in sub call to '3'
            ecl << pair { st6bsNon3BossItemCallSomething, (int16_t)0 }; // Disable item drops
            ecl << pair { st6bsNon3PlaySoundSomething, (int16_t)0 }; // Disable sound effect
            ECLJump(ecl, st6bsNon3PostLifeCount, st6bsNon3PostWait, 0); // Skip wait
            if (!thPracParam.dlg) {
                ecl << pair { st6bsNon3InvulnCallVal, (int16_t)0 }; // Disable Invincible
                ECLJump(ecl, st6bsNon3PreDialogue, st6bsNon3PostDialogue, 0); // Skip dialogue
            }
            break;
        }
        case THPrac::TH20::TH20_ST6_BOSS6: {
            ECLStdExec(ecl, st6PostMaple, 1, 1);
            ECLJump(ecl, st6PostMaple + stdInterruptSize, st6BossCreateCall, 60);
            ecl.SetFile(2);
            ECLJump(ecl, st6bsPrePushSpellID, st6bsPostNotSpellPracCheck, 1); // Utilize Spell Practice Jump
            ecl << pair { st6bsSpellHealthVal, 4000 }; // Set correct health (set in skipped non)
            ecl << pair { st6bsSpellSubCallOrd, (int8_t)0x33 }; // Set spell ID in sub call to '3'
            break;
        }
        case THPrac::TH20::TH20_ST6_BOSS7: {
            constexpr unsigned int st6bsNon4InvulnCallVal = 0x2b78 + 0x10;
            constexpr unsigned int st6bsNon4BossItemCallSomething = 0x2c64 + 0x4;
            constexpr unsigned int st6bsNon4PlaySoundSomething = 0x2d90 + 0x4;
            constexpr unsigned int st6bsNon4PostProtectRange = 0x2f10;
            constexpr unsigned int st6bsNon4PostWait = 0x2fbc;

            ECLStdExec(ecl, st6PostMaple, 1, 1);
            ECLJump(ecl, st6PostMaple + stdInterruptSize, st6BossCreateCall, 60);
            ecl.SetFile(2);
            ecl << pair { st6bsSetYPos, (float)144.0f }; // Fix boss starting too high
            ecl << pair { st6bsNonSubCallOrd, (int8_t)0x34 }; // Set nonspell ID in sub call to '4'
            ecl << pair { st6bsNon4InvulnCallVal, (int16_t)0 }; // Disable Invincible
            ecl << pair { st6bsNon4BossItemCallSomething, (int16_t)0 }; // Disable item drops
            ecl << pair { st6bsNon4PlaySoundSomething, (int16_t)0 }; // Disable sound effect
            ECLJump(ecl, st6bsNon4PostProtectRange, st6bsNon4PostWait, 0); // Skip wait
            break;
        }
        case THPrac::TH20::TH20_ST6_BOSS8: {
            ECLStdExec(ecl, st6PostMaple, 1, 1);
            ECLJump(ecl, st6PostMaple + stdInterruptSize, st6BossCreateCall, 60);
            ecl.SetFile(2);
            ECLJump(ecl, st6bsPrePushSpellID, st6bsPostNotSpellPracCheck, 1); // Utilize Spell Practice Jump
            ecl << pair { st6bsSpellHealthVal, 4000 }; // Set correct health (set in skipped non)
            ecl << pair { st6bsSpellSubCallOrd, (int8_t)0x34 }; // Set spell ID in sub call to '4'
            break;
        }
        case THPrac::TH20::TH20_ST6_BOSS9: {
            constexpr unsigned int st6bsNon5InvulnCallVal = 0x38a0 + 0x10;
            constexpr unsigned int st6bsNon5BossItemCallSomething = 0x39a4 + 0x4;
            constexpr unsigned int st6bsNon5PlaySoundSomething = 0x3ad0 + 0x4;
            constexpr unsigned int st6bsNon5PostLifeMarker = 0x3cf8;
            constexpr unsigned int st6bsNon5PostWait = 0x3db8;

            constexpr unsigned int st6bsNon5PreDialogue = 0x3c28;
            constexpr unsigned int st6bsNon5PostDialogue = 0x3ca0;

            ECLStdExec(ecl, st6PostMaple, 1, 1);
            ECLJump(ecl, st6PostMaple + stdInterruptSize, st6BossCreateCall, 60);
            ecl.SetFile(2);
            ecl << pair { st6bsSetYPos, (float)192.0f }; // Fix boss starting too high
            ecl << pair { st6bsNonSubCallOrd, (int8_t)0x35 }; // Set nonspell ID in sub call to '5'
            ecl << pair { st6bsNon5BossItemCallSomething, (int16_t)0 }; // Disable item drops
            ecl << pair { st6bsNon5PlaySoundSomething, (int16_t)0 }; // Disable sound effect
            ECLJump(ecl, st6bsNon5PostLifeMarker, st6bsNon5PostWait, 0); // Skip wait
            if (!thPracParam.dlg) {
                ecl << pair { st6bsNon5InvulnCallVal, (int16_t)0 }; // Disable Invincible
                ECLJump(ecl, st6bsNon5PreDialogue, st6bsNon5PostDialogue, 0); // Skip dialogue
            }
            break;
        }
        case THPrac::TH20::TH20_ST6_BOSS10: {
            ECLStdExec(ecl, st6PostMaple, 1, 1);
            ECLJump(ecl, st6PostMaple + stdInterruptSize, st6BossCreateCall, 60);
            ecl.SetFile(2);
            ECLJump(ecl, st6bsPrePushSpellID, st6bsPostNotSpellPracCheck, 1); // Utilize Spell Practice Jump
            ecl << pair { st6bsSpellHealthVal, 4000 }; // Set correct health (set in skipped non)
            ecl << pair { st6bsSpellSubCallOrd, (int8_t)0x35 }; // Set spell ID in sub call to '5'
            break;
        }
        case THPrac::TH20::TH20_ST6_BOSS11: {
            ECLStdExec(ecl, st6PostMaple, 1, 1);
            ECLJump(ecl, st6PostMaple + stdInterruptSize, st6BossCreateCall, 60);
            ecl.SetFile(2);
            ECLJump(ecl, st6bsPrePushSpellID, st6bsPostNotSpellPracCheck, 1); // Utilize Spell Practice Jump
            ecl << pair { st6bsSpellHealthVal, 4500 }; // Set correct health (set in skipped non)
            ecl << pair { st6bsSpellSubCallOrd, (int8_t)0x36 }; // Set spell ID in sub call to '6'
            break;
        }
        case THPrac::TH20::TH20_ST6_BOSS12: {
            constexpr unsigned int st6bsSpell7HealthVal = 0xa4dc + 0x10;
            constexpr unsigned int st6bsSpell10SubCallOrd = 0xa9d8 + 0x1e;

            ECLStdExec(ecl, st6PostMaple, 1, 1);
            ECLJump(ecl, st6PostMaple + stdInterruptSize, st6BossCreateCall, 60);
            ecl.SetFile(2);
            ECLJump(ecl, st6bsPrePushSpellID, st6bsPostNotSpellPracCheck, 1); // Utilize Spell Practice Jump
            ecl << pair { st6bsSpellSubCallOrd, (int8_t)0x37 }; // Set spell ID in sub call to '7'

            switch (thPracParam.phase) {
            case 1: // P2
                ecl << pair { st6bsSpell7HealthVal, 8000 };
                break;
            case 2: // P3
                ecl << pair { st6bsSpell7HealthVal, 5000 };
                ecl << pair { st6bsSpell10SubCallOrd, (int8_t)0x32 }; // BossCard7_1 -> BossCard7_2
                break;
            case 3: // P4
                ecl << pair { st6bsSpell7HealthVal, 2000 };
                ecl << pair { st6bsSpell10SubCallOrd, (int8_t)0x33 }; // BossCard7_1 -> BossCard7_3
                break;
            }
            break;
        }

        case THPrac::TH20::TH20_ST7_MID1: {
            ECLJump(ecl, st7PostMaple, st7MBossCreateCall, 60, 90);
            if (thPracParam.dlg) break;

            ECLJump(ecl, st7MBossPreMsg, st7MBossPostMsg, 60, 90);
            ecl.SetFile(3);
            ECLJump(ecl, st7mbsPrePushSpellID, st7mbsPostNotSpellPracCheck, 1); // Utilize Spell Practice Jump
            break;
        }
        case THPrac::TH20::TH20_ST7_MID2: {
            ECLJump(ecl, st7PostMaple, st7MBossCreateCall, 60, 90);
            ECLJump(ecl, st7MBossPreMsg, st7MBossPostMsg, 60, 90);
            ecl.SetFile(3);
            ECLJump(ecl, st7mbsPrePushSpellID, st7mbsPostNotSpellPracCheck, 1); // Utilize Spell Practice Jump
            ecl << pair { st7mbsNonSubCallOrd, (int8_t)0x32 }; // Set nonspell ID in sub call to '2'
            break;
        }
        case THPrac::TH20::TH20_ST7_MID3: {
            ECLJump(ecl, st7PostMaple, st7MBossCreateCall, 60, 90);
            ECLJump(ecl, st7MBossPreMsg, st7MBossPostMsg, 60, 90);
            ecl.SetFile(3);
            ECLJump(ecl, st7mbsPrePushSpellID, st7mbsPostNotSpellPracCheck, 1); // Utilize Spell Practice Jump
            ecl << pair { st7mbsNonSubCallOrd, (int8_t)0x33 }; // Set nonspell ID in sub call to '3'
            break;
        }
        case THPrac::TH20::TH20_ST7_BOSS1: {
            constexpr unsigned int st7BossDialogueCall = 0x7470;
            ECLStdExec(ecl, st7PostMaple, 1, 1);
            if (thPracParam.dlg)
                ECLJump(ecl, st7PostMaple + stdInterruptSize, st7BossDialogueCall, 60);
            else
                ECLJump(ecl, st7PostMaple + stdInterruptSize, st7BossCreateCall, 60);
            break;
        }
        case THPrac::TH20::TH20_ST7_BOSS2: {
            ECLStdExec(ecl, st7PostMaple, 1, 1);
            ECLJump(ecl, st7PostMaple + stdInterruptSize, st7BossCreateCall, 60);
            ecl.SetFile(2);
            ECLJump(ecl, st7bsPrePushSpellID, st7bsPostNotSpellPracCheck, 1); // Utilize Spell Practice Jump
            break;
        }
        case THPrac::TH20::TH20_ST7_BOSS3: {
            constexpr unsigned int st7bsNon2InvulnCallVal = 0x1ba4 + 0x10;
            constexpr unsigned int st7bsNon2BossItemCallSomething = 0x1c88 + 0x4;
            constexpr unsigned int st7bsNon2PlaySoundSomething = 0x1db4 + 0x4;
            constexpr unsigned int st7bsNon2PostProtectRange = 0x1f74;
            constexpr unsigned int st7bsNon2PostWait = 0x2020;

            ECLStdExec(ecl, st7PostMaple, 1, 1);
            ECLJump(ecl, st7PostMaple + stdInterruptSize, st7BossCreateCall, 60);
            ecl.SetFile(2);
            ecl << pair { st7bsSetYPos, (float)128.0f }; // Fix boss starting too high
            ecl << pair { st7bsNonSubCallOrd, (int8_t)0x32 }; // Set nonspell ID in sub call to '2'
            ecl << pair { st7bsNon2InvulnCallVal, (int16_t)0 }; // Disable Invincible
            ecl << pair { st7bsNon2BossItemCallSomething, (int16_t)0 }; // Disable item drops
            ecl << pair { st7bsNon2PlaySoundSomething, (int16_t)0 }; // Disable sound effect
            ECLJump(ecl, st7bsNon2PostProtectRange, st7bsNon2PostWait, 0); // Skip wait
            break;
        }
        case THPrac::TH20::TH20_ST7_BOSS4: {
            ECLStdExec(ecl, st7PostMaple, 1, 1);
            ECLJump(ecl, st7PostMaple + stdInterruptSize, st7BossCreateCall, 60);
            ecl.SetFile(2);
            ECLJump(ecl, st7bsPrePushSpellID, st7bsPostNotSpellPracCheck, 1); // Utilize Spell Practice Jump
            ecl << pair { st7bsSpellSubCallOrd, (int8_t)0x32 }; // Set spell ID in sub call to '2'
            break;
        }
        case THPrac::TH20::TH20_ST7_BOSS5: {
            constexpr unsigned int st7bsNon3InvulnCallVal = 0x2618 + 0x10;
            constexpr unsigned int st7bsNon3BossItemCallSomething = 0x26fc + 0x4;
            constexpr unsigned int st7bsNon3PlaySoundSomething = 0x2828 + 0x4;
            constexpr unsigned int st7bsNon3PostProtectRange = 0x29e8;
            constexpr unsigned int st7bsNon3PostWait = 0x2a94;

            ECLStdExec(ecl, st7PostMaple, 1, 1);
            ECLJump(ecl, st7PostMaple + stdInterruptSize, st7BossCreateCall, 60);
            ecl.SetFile(2);
            ecl << pair { st7bsSetYPos, (float)128.0f }; // Fix boss starting too high
            ecl << pair { st7bsNonSubCallOrd, (int8_t)0x33 }; // Set nonspell ID in sub call to '3'
            ecl << pair { st7bsNon3InvulnCallVal, (int16_t)0 }; // Disable Invincible
            ecl << pair { st7bsNon3BossItemCallSomething, (int16_t)0 }; // Disable item drops
            ecl << pair { st7bsNon3PlaySoundSomething, (int16_t)0 }; // Disable sound effect
            ECLJump(ecl, st7bsNon3PostProtectRange, st7bsNon3PostWait, 0); // Skip wait
            break;
        }
        case THPrac::TH20::TH20_ST7_BOSS6: {
            ECLStdExec(ecl, st7PostMaple, 1, 1);
            ECLJump(ecl, st7PostMaple + stdInterruptSize, st7BossCreateCall, 60);
            ecl.SetFile(2);
            ECLJump(ecl, st7bsPrePushSpellID, st7bsPostNotSpellPracCheck, 1); // Utilize Spell Practice Jump
            ecl << pair { st7bsSpellSubCallOrd, (int8_t)0x33 }; // Set spell ID in sub call to '3'
            break;
        }
        case THPrac::TH20::TH20_ST7_BOSS7: {
            constexpr unsigned int st7bsNon4InvulnCallVal = 0x308c + 0x10;
            constexpr unsigned int st7bsNon4BossItemCallSomething = 0x3170 + 0x4;
            constexpr unsigned int st7bsNon4PlaySoundSomething = 0x329c + 0x4;
            constexpr unsigned int st7bsNon4PostProtectRange = 0x345c;
            constexpr unsigned int st7bsNon4PostWait = 0x3508;

            ECLStdExec(ecl, st7PostMaple, 1, 1);
            ECLJump(ecl, st7PostMaple + stdInterruptSize, st7BossCreateCall, 60);
            ecl.SetFile(2);
            ecl << pair { st7bsSetYPos, (float)128.0f }; // Fix boss starting too high
            ecl << pair { st7bsNonSubCallOrd, (int8_t)0x34 }; // Set nonspell ID in sub call to '4'
            ecl << pair { st7bsNon4InvulnCallVal, (int16_t)0 }; // Disable Invincible
            ecl << pair { st7bsNon4BossItemCallSomething, (int16_t)0 }; // Disable item drops
            ecl << pair { st7bsNon4PlaySoundSomething, (int16_t)0 }; // Disable sound effect
            ECLJump(ecl, st7bsNon4PostProtectRange, st7bsNon4PostWait, 0); // Skip wait
            break;
        }
        case THPrac::TH20::TH20_ST7_BOSS8: {
            ECLStdExec(ecl, st7PostMaple, 1, 1);
            ECLJump(ecl, st7PostMaple + stdInterruptSize, st7BossCreateCall, 60);
            ecl.SetFile(2);
            ECLJump(ecl, st7bsPrePushSpellID, st7bsPostNotSpellPracCheck, 1); // Utilize Spell Practice Jump
            ecl << pair { st7bsSpellSubCallOrd, (int8_t)0x34 }; // Set spell ID in sub call to '4'
            break;
        }
        case THPrac::TH20::TH20_ST7_BOSS9: {
            constexpr unsigned int st7bsNon5InvulnCallVal = 0x3b00 + 0x10;
            constexpr unsigned int st7bsNon5BossItemCallSomething = 0x3be4 + 0x4;
            constexpr unsigned int st7bsNon5PlaySoundSomething = 0x3d10 + 0x4;
            constexpr unsigned int st7bsNon5PostProtectRange = 0x3e90;
            constexpr unsigned int st7bsNon5PostWait = 0x3f7c;

            ECLStdExec(ecl, st7PostMaple, 1, 1);
            ECLJump(ecl, st7PostMaple + stdInterruptSize, st7BossCreateCall, 60);
            ecl.SetFile(2);
            ecl << pair { st7bsSetYPos, (float)128.0f }; // Fix boss starting too high
            ecl << pair { st7bsNonSubCallOrd, (int8_t)0x35 }; // Set nonspell ID in sub call to '5'
            ecl << pair { st7bsNon5InvulnCallVal, (int16_t)0 }; // Disable Invincible
            ecl << pair { st7bsNon5BossItemCallSomething, (int16_t)0 }; // Disable item drops
            ecl << pair { st7bsNon5PlaySoundSomething, (int16_t)0 }; // Disable sound effect
            ECLJump(ecl, st7bsNon5PostProtectRange, st7bsNon5PostWait, 0); // Skip wait
            break;
        }
        case THPrac::TH20::TH20_ST7_BOSS10: {
            ECLStdExec(ecl, st7PostMaple, 1, 1);
            ECLJump(ecl, st7PostMaple + stdInterruptSize, st7BossCreateCall, 60);
            ecl.SetFile(2);
            ECLJump(ecl, st7bsPrePushSpellID, st7bsPostNotSpellPracCheck, 1); // Utilize Spell Practice Jump
            ecl << pair { st7bsSpellSubCallOrd, (int8_t)0x35 }; // Set spell ID in sub call to '5'
            ecl << pair { st7bsSpellHealthVal, 5000 }; // Set correct health (set in skipped non)
            break;
        }
        case THPrac::TH20::TH20_ST7_BOSS11: {
            constexpr unsigned int st7bsNon6InvulnCallVal = 0x4574 + 0x10;
            constexpr unsigned int st7bsNon6BossItemCallSomething = 0x4658 + 0x4;
            constexpr unsigned int st7bsNon6PlaySoundSomething = 0x4784 + 0x4;
            constexpr unsigned int st7bsNon6PostProtectRange = 0x4944;
            constexpr unsigned int st7bsNon6PostWait = 0x49f0;

            ECLStdExec(ecl, st7PostMaple, 1, 1);
            ECLJump(ecl, st7PostMaple + stdInterruptSize, st7BossCreateCall, 60);
            ecl.SetFile(2);
            ecl << pair { st7bsSetYPos, (float)128.0f }; // Fix boss starting too high
            ecl << pair { st7bsNonSubCallOrd, (int8_t)0x36 }; // Set nonspell ID in sub call to '6'
            ecl << pair { st7bsNon6InvulnCallVal, (int16_t)0 }; // Disable Invincible
            ecl << pair { st7bsNon6BossItemCallSomething, (int16_t)0 }; // Disable item drops
            ecl << pair { st7bsNon6PlaySoundSomething, (int16_t)0 }; // Disable sound effect
            ECLJump(ecl, st7bsNon6PostProtectRange, st7bsNon6PostWait, 0); // Skip wait
            break;
        }
        case THPrac::TH20::TH20_ST7_BOSS12: {
            ECLStdExec(ecl, st7PostMaple, 1, 1);
            ECLJump(ecl, st7PostMaple + stdInterruptSize, st7BossCreateCall, 60);
            ecl.SetFile(2);
            ECLJump(ecl, st7bsPrePushSpellID, st7bsPostNotSpellPracCheck, 1); // Utilize Spell Practice Jump
            ecl << pair { st7bsSpellSubCallOrd, (int8_t)0x36 }; // Set spell ID in sub call to '6'
            ecl << pair { st7bsSpellHealthVal, 5000 }; // Set correct health (set in skipped non)
            break;
        }
        case THPrac::TH20::TH20_ST7_BOSS13: {
            constexpr unsigned int st7bsNon7InvulnCallVal = 0x4fe8 + 0x10;
            constexpr unsigned int st7bsNon7BossItemCallSomething = 0x50cc + 0x4;
            constexpr unsigned int st7bsNon7PlaySoundSomething = 0x51f8 + 0x4;
            constexpr unsigned int st7bsNon7PostProtectRange = 0x53b8;
            constexpr unsigned int st7bsNon7PostWait = 0x5464;

            ECLStdExec(ecl, st7PostMaple, 1, 1);
            ECLJump(ecl, st7PostMaple + stdInterruptSize, st7BossCreateCall, 60);
            ecl.SetFile(2);
            ecl << pair { st7bsSetYPos, (float)128.0f }; // Fix boss starting too high
            ecl << pair { st7bsNonSubCallOrd, (int8_t)0x37 }; // Set nonspell ID in sub call to '7'
            ecl << pair { st7bsNon7InvulnCallVal, (int16_t)0 }; // Disable Invincible
            ecl << pair { st7bsNon7BossItemCallSomething, (int16_t)0 }; // Disable item drops
            ecl << pair { st7bsNon7PlaySoundSomething, (int16_t)0 }; // Disable sound effect
            ECLJump(ecl, st7bsNon7PostProtectRange, st7bsNon7PostWait, 0); // Skip wait
            break;
        }
        case THPrac::TH20::TH20_ST7_BOSS14: {
            ECLStdExec(ecl, st7PostMaple, 1, 1);
            ECLJump(ecl, st7PostMaple + stdInterruptSize, st7BossCreateCall, 60);
            ecl.SetFile(2);
            ECLJump(ecl, st7bsPrePushSpellID, st7bsPostNotSpellPracCheck, 1); // Utilize Spell Practice Jump
            ecl << pair { st7bsSpellSubCallOrd, (int8_t)0x37 }; // Set spell ID in sub call to '7'
            ecl << pair { st7bsSpellHealthVal, 3500 }; // Set correct health (set in skipped non)
            break;
        }
        case THPrac::TH20::TH20_ST7_BOSS15: {
            constexpr unsigned int st7bsNon8InvulnCallVal = 0x5a5c + 0x10;
            constexpr unsigned int st7bsNon8BossItemCallSomething = 0x5b40 + 0x4;
            constexpr unsigned int st7bsNon8PlaySoundSomething = 0x5c6c + 0x4;
            constexpr unsigned int st7bsNon8PostProtectRange = 0x5e2c;
            constexpr unsigned int st7bsNon8PostWait = 0x5ef8;

            ECLStdExec(ecl, st7PostMaple, 1, 1);
            ECLJump(ecl, st7PostMaple + stdInterruptSize, st7BossCreateCall, 60);
            ecl.SetFile(2);
            ecl << pair { st7bsSetYPos, (float)128.0f }; // Fix boss starting too high
            ecl << pair { st7bsNonSubCallOrd, (int8_t)0x38 }; // Set nonspell ID in sub call to '8'
            ecl << pair { st7bsNon8InvulnCallVal, (int16_t)0 }; // Disable Invincible
            ecl << pair { st7bsNon8BossItemCallSomething, (int16_t)0 }; // Disable item drops
            ecl << pair { st7bsNon8PlaySoundSomething, (int16_t)0 }; // Disable sound effect
            ECLJump(ecl, st7bsNon8PostProtectRange, st7bsNon8PostWait, 0); // Skip wait
            break;
        }
        case THPrac::TH20::TH20_ST7_BOSS16: {
            ECLStdExec(ecl, st7PostMaple, 1, 1);
            ECLJump(ecl, st7PostMaple + stdInterruptSize, st7BossCreateCall, 60);
            ecl.SetFile(2);
            ECLJump(ecl, st7bsPrePushSpellID, st7bsPostNotSpellPracCheck, 1); // Utilize Spell Practice Jump
            ecl << pair { st7bsSpellSubCallOrd, (int8_t)0x38 }; // Set spell ID in sub call to '8'
            ecl << pair { st7bsSpellHealthVal, 6000 }; // Set correct health (set in skipped non)
            break;
        }
        case THPrac::TH20::TH20_ST7_BOSS17: {
            constexpr unsigned int st7BossSpell9Duration = 0xdd50 + 0x18;
            constexpr unsigned int st7BossSpell9ChoccyPostSetup = 0xe150;
            constexpr unsigned int st7BossSpell9ChoccyPreFinale = 0xedb8;
            constexpr unsigned int st7BossSpell9BulletsPostSetup = 0xf68c;
            constexpr unsigned int st7BossSpell9BulletsPreFinale = 0xfccc;

            ECLStdExec(ecl, st7PostMaple, 1, 1);
            ECLJump(ecl, st7PostMaple + stdInterruptSize, st7BossCreateCall, 60);
            ecl.SetFile(2);
            ECLJump(ecl, st7bsPrePushSpellID, st7bsPostNotSpellPracCheck, 1); // Utilize Spell Practice Jump
            ecl << pair { st7bsSpellSubCallOrd, (int8_t)0x39 }; // Set spell ID in sub call to '9'
            ecl << pair { st7bsSpellHealthVal, 6000 }; // Set correct health (set in skipped non)

            if (thPracParam.phase == 1) { //Finale
                ecl << pair { st7BossSpell9Duration, 5040 - 3400 + 90 }; // Adjust duration (we skipped 3400f worth of waits in the attacks but the boss waits for 90 before attacking)
                ECLJump(ecl, st7BossSpell9ChoccyPostSetup, st7BossSpell9ChoccyPreFinale, 0);
                ECLJump(ecl, st7BossSpell9BulletsPostSetup, st7BossSpell9BulletsPreFinale, 0);
            }
            break;
        }
        case THPrac::TH20::TH20_ST7_BOSS18: {
            constexpr unsigned int st7BossSpell10PostCheck = 0xebc;
            constexpr unsigned int st7BossSpell10SpellHealthVal = 0xebc + 0x10;

            ECLStdExec(ecl, st7PostMaple, 1, 1);
            ECLJump(ecl, st7PostMaple + stdInterruptSize, st7BossCreateCall, 60);
            ecl.SetFile(2);
            ECLJump(ecl, st7bsPrePushSpellID, st7BossSpell10PostCheck, 1); // Utilize Spell Practice Jump

            switch (thPracParam.phase) {
            case 1: //P2
                ecl << pair { st7BossSpell10SpellHealthVal, 5300 };
                break;
            case 2: //P3
                ecl << pair { st7BossSpell10SpellHealthVal, 3200 };
                break;
            case 3: //P4
                ecl << pair { st7BossSpell10SpellHealthVal, 1500 };
                break;
            }

            break;
        }
        default:
            break;
        }
    }
    __declspec(noinline) void THSectionPatch()
    {
        ECLHelper ecl;
        ecl.SetBaseAddr((void*)GetMemAddr(RVA(0x1BA570), 0x104, 0xc));

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
        else if (thPracParam.dlg && thPracParam.section != TH20_ST6_BOSS5 && thPracParam.section != TH20_ST6_BOSS9)
            return 0;
        else
            return th_sections_bgm[thPracParam.section];
    }
    void THSaveReplay(char* repName)
    {
        ReplaySaveParam(mb_to_utf16(repName, 932).c_str(), thPracParam.GetJson());
    }

    void __fastcall AnmVM_Reset_hook(uintptr_t self)
    {
        uint32_t fast_id = *(uint32_t*)(self + 0x4C4);
        memset((void*)self, 0, 0x5E4);
        *(uint32_t*)(self + 0x4C4) = fast_id;
        asm_call_rel<0x299D0, Thiscall>(self);
    }

    static char* sReplayPath = nullptr;
    static constinit HookCtx anmUninitFixHooks[] = {
        { .addr = 0x38974, .data = PatchCode("e800000000") },
        { .addr = 0x48411, .data = PatchCode("e800000000") },
        { .addr = 0x4CA14, .data = PatchCode("e800000000") },
        { .addr = 0x4CAB9, .data = PatchCode("e800000000") },
        { .addr = 0x4E2B3, .data = PatchCode("e800000000") },
        { .addr = 0x708EA, .data = PatchCode("e800000000") },
        { .addr = 0x7F1B1, .data = PatchCode("e800000000") },
        { .addr = 0x7F2F6, .data = PatchCode("e800000000") },
        { .addr = 0x82252, .data = PatchCode("e800000000") },
        { .addr = 0xD3208, .data = PatchCode("e800000000") },
        { .addr = 0xD32D2, .data = PatchCode("e800000000") },
        { .addr = 0xD3E57, .data = PatchCode("e800000000") },
        { .addr = 0xD4237, .data = PatchCode("e800000000") },
    };



    HOOKSET_DEFINE(THMainHook)
    EHOOK_DY(th20_boss_bgm, 0xBAC98, 2, {
        if (THBGMTest()) {
            PushHelper32(pCtx, 1);
            pCtx->Eip = RVA(0xBAC9A);
        }
    })
    EHOOK_DY(th20_everlasting_bgm_1, 0x28C90, 1, {
        int32_t retn_addr = ((int32_t*)pCtx->Esp)[0] - ingame_image_base;
        int32_t bgm_cmd = ((int32_t*)pCtx->Esp)[1];
        int32_t bgm_id = ((int32_t*)pCtx->Esp)[2];
        // 4th stack item = i32 call_addr

        bool el_switch;
        bool is_practice;
        bool result;

        el_switch = *(THOverlay::singleton().mElBgm) && !THGuiRep::singleton().mRepStatus && (thPracParam.mode == 1) && thPracParam.section;
        is_practice = (*((int32_t*)RVA(0x1ba5d4)) & 0x1);

        result = ElBgmTest<0xD9B90, 0xD9BFE, 0xE60B8, 0xE64B8, 0xffffffff>(
            el_switch, is_practice, retn_addr, bgm_cmd, bgm_id, 0xffffffff);

        if (result) {
            pCtx->Eip = RVA(0x28DD5);
        }
    })
    EHOOK_DY(th20_everlasting_bgm_2, 0xE5C91, 5, {
        bool el_switch = *(THOverlay::singleton().mElBgm) && !THGuiRep::singleton().mRepStatus && (thPracParam.mode == 1) && thPracParam.section;
        if (el_switch)
            pCtx->Eip = RVA(0xE5CF7);
    })
    EHOOK_DY(th20_patch_main, 0xBBD56, 1, {
        if (thPracParam.mode == 1) {
            *(int32_t*)RVA(0x1BA5F0) = (int32_t)(thPracParam.score / 10);
            *(int32_t*)RVA(0x1BA568 + 0x140) = thPracParam.life;
            *(int32_t*)RVA(0x1BA568 + 0x148) = thPracParam.life_fragment;
            *(int32_t*)RVA(0x1BA568 + 0x154) = thPracParam.bomb;
            *(int32_t*)RVA(0x1BA568 + 0x158) = thPracParam.bomb_fragment;
            *(int32_t*)RVA(0x1BA568 + 0xB8) = thPracParam.power;
            *(int32_t*)RVA(0x1BA568 + 0xCC) = thPracParam.value;

            THSectionPatch();
        }
        thPracParam._playLock = true;
    })
    EHOOK_DY(th20_patch_stones, 0x1336F1, 1, {
        if (thPracParam.mode != 1)
            return;

        uintptr_t player_stats = RVA(0x1BA5F0);
        *(int32_t*)(player_stats + 0x4C) = (int32_t)(thPracParam.hyper * *(int32_t*)(player_stats + 0x50));
        if ((int32_t)thPracParam.hyper == 1) { // call the hyper start method
            int32_t* gauge_manager_ptr = (int32_t*)RVA(0x1BA568 + 0x2C);
            asm_call_rel<0x134D00, Fastcall>(*gauge_manager_ptr);
        }

        if (thPracParam.stoneMax) // backwards compatibility
            *(int32_t*)(player_stats + 0x60) = thPracParam.stoneMax;
        *(int32_t*)(player_stats + 0x5C) = (int32_t)(thPracParam.stone * *(int32_t*)(player_stats + 0x60));
        *(int32_t*)(player_stats + 0x64) = thPracParam.priorityR;
        *(int32_t*)(player_stats + 0x68) = thPracParam.priorityB;
        *(int32_t*)(player_stats + 0x6C) = thPracParam.priorityY;
        *(int32_t*)(player_stats + 0x70) = thPracParam.priorityG;
        *(int32_t*)(player_stats + 0x74) = thPracParam.levelR;
        *(int32_t*)(player_stats + 0x78) = thPracParam.levelB;
        *(int32_t*)(player_stats + 0x7C) = thPracParam.levelY;
        *(int32_t*)(player_stats + 0x80) = thPracParam.levelG;
        *(int32_t*)(player_stats + 0x94) = thPracParam.cycle;
    })
    EHOOK_DY(th20_patch_ex_stones_fix, 0x6415A, 3, {
        if (thPracParam.mode == 1)
            pCtx->Eip = RVA(0x6417F);
    })
    EHOOK_DY(th20_param_reset, 0x129EA6, 3, {
        thPracParam.Reset();
    })
    EHOOK_DY(th20_prac_menu_1, 0x1294A3, 3, {
        THGuiPrac::singleton().State(1);
    })
    EHOOK_DY(th20_prac_menu_2, 0x1294D0, 3, {
        THGuiPrac::singleton().State(2);
    })
    EHOOK_DY(th20_prac_menu_3, 0x1299AE, 2, {
        THGuiPrac::singleton().State(3);
    })
    EHOOK_DY(th20_prac_menu_4, 0x129AD8, 3, {
        THGuiPrac::singleton().State(4);
    })
    PATCH_DY(th20_prac_menu_enter_1, 0x129595, "eb")
    EHOOK_DY(th20_prac_menu_enter_2, 0x129A98, 3, {
        pCtx->Eax = thPracParam.stage;
    })
    PATCH_DY(th20_disable_prac_menu_1, 0x129B40, "c3")
    PATCH_DY(th20_prac_menu_ignore_locked, 0x12CA30, "b001c3")
    EHOOK_DY(th20_extra_prac_fix, 0x11EB3D, 2, {
        *(uint32_t*)RVA(0x1BA568 + 0x88 + 0x1E0) = *(uint32_t*)RVA(0x1B0A60);
    })
    PATCH_DY(th20_instant_esc_r, 0xE2EB5, "EB")
    EHOOK_DY(th20_timer_desync_fix, 0xBA99F, 6, {
        uint32_t stage = *(uint32_t*)RVA(0x1BA568 + 0x88 + 0x1F4) - 1;
        if (*(uint32_t*)(*(uintptr_t*)RVA(0x1BA828) + 0x108)) {
            // Playback
            int32_t offset = stage != 0 && !*(uint32_t*)RVA(0x1C06A0) ? 30 : 0;
            if (thPracParam.reimuR2Timer[stage])
                asm_call_rel<0x23520, Thiscall>(*(uintptr_t*)RVA(0x1BA568 + 4) + 0x22B4 + 0x12580, thPracParam.reimuR2Timer[stage] + offset);
            if (thPracParam.passiveMeterTimer[stage])
                asm_call_rel<0x23520, Thiscall>(*(uintptr_t*)RVA(0x1C6118) + 0x28, thPracParam.passiveMeterTimer[stage]);
        } else {
            // Recording
            thPracParam.reimuR2Timer[stage] = *(int32_t*)(*(uintptr_t*)RVA(0x1BA568 + 4) + 0x22B4 + 0x12580 + 4);
            thPracParam.passiveMeterTimer[stage] = *(int32_t*)(*(uintptr_t*)RVA(0x1C6118) + 0x28 + 4);
        }
    })
    EHOOK_DY(th20_fix_rep_stone_init, 0xBB0A0, 5, {
        if (*(uint32_t*)(*(uintptr_t*)RVA(0x1BA828) + 0x108)) {
            // Yes, the order really is swapped like this
            auto selected = (uint32_t*)(RVA(0x1BA568) + 0x88 + 0x1C);
            selected[0] = replayStones[0];
            selected[1] = replayStones[2];
            selected[2] = replayStones[1];
            selected[3] = replayStones[3];
        }
    })
    PATCH_DY(th20_fix_rep_results_skip, 0x110D61, "7B4BFAFF")
    EHOOK_DY(th20_rep_save, 0x109D6A, 3, {
        if (sReplayPath) {
            THSaveReplay(sReplayPath);
            free(sReplayPath);
            sReplayPath = nullptr;
        }
    })
    EHOOK_DY(th20_rep_get_path, 0x1098E1, 5, {
        sReplayPath = _strdup((char*)pCtx->Edx);
    })
    EHOOK_DY(th20_rep_menu_1, 0x123614, 3, {
        THGuiRep::singleton().State(1);
    })
    EHOOK_DY(th20_rep_menu_2, 0x12391C, 5, {
        THGuiRep::singleton().State(2);
    })
    EHOOK_DY(th20_rep_menu_3, 0x123DAE, 2, {
        THGuiRep::singleton().State(3);
    })
    EHOOK_DY(th20_update, 0x12A72, 1, {
        GameGuiBegin(IMPL_WIN32_DX9, !THAdvOptWnd::singleton().IsOpen());

        // Gui components update
        THGuiPrac::singleton().Update();
        THGuiRep::singleton().Update();
        THOverlay::singleton().Update();

        bool drawCursor = THAdvOptWnd::StaticUpdate() || THGuiPrac::singleton().IsOpen();
        GameGuiEnd(drawCursor);
    })
    EHOOK_DY(th20_render, 0x12CEA, 1, {
        GameGuiRender(IMPL_WIN32_DX9);
    })
    HOOKSET_ENDDEF()

    static __declspec(noinline) void THGuiCreate()
    {
        if (ImGui::GetCurrentContext())
            return;

        // Init
        GameGuiInit(IMPL_WIN32_DX9, RVA(0x1C4D48), RVA(0x1B6758),
            Gui::INGAGME_INPUT_GEN2, RVA(0x1B88C0), RVA(0x1B88B8), 0,
            -2, *(float*)RVA(0x1B8818), 0.0f);

        SetDpadHook(0x22651, 6);

        // Gui components creation
        THGuiPrac::singleton();
        THGuiRep::singleton();
        THOverlay::singleton();

        // Hooks
        EnableAllHooks(THMainHook);

        // Replay menu string fixes
        auto stageStrings = (const char**)RVA(0x1AFFD0);
        stageStrings[4] = "St4";
        stageStrings[5] = "St5";
        stageStrings[6] = "St6";
        stageStrings[7] = "Ex ";
        stageStrings[8] = "All";
        *(const char**)RVA(0x1B0A7C) = "%s  %s %.2d/%.2d/%.2d %.2d:%.2d %s %s %s %s %2.1f%%";

        // AnmVM reset uninitialized memory fix
        for (size_t i = 0; i < elementsof(anmUninitFixHooks); i++) {
            *(uintptr_t*)((uintptr_t)anmUninitFixHooks[i].data.buffer.ptr + 1) = (uintptr_t)&AnmVM_Reset_hook - RVA(anmUninitFixHooks[i].addr) - 5;
            anmUninitFixHooks[i].Setup();
            anmUninitFixHooks[i].Enable();
        }

        //  Reset thPracParam
        thPracParam.Reset();
    }

    HOOKSET_DEFINE(THInitHook)
    PATCH_DY(th20_startup_1, 0x11F82C, "EB")
    PATCH_DY(th20_startup_2, 0x11E8C9, "EB")
    EHOOK_DY(th20_gui_init_1, 0x12A0A3, 7, {
        self->Disable();
        THGuiCreate();
    })
    EHOOK_DY(th20_gui_init_2, 0x1C725, 1, {
        self->Disable();
        THGuiCreate();
    })
    HOOKSET_ENDDEF()
}

void TH20Init()
{
    ingame_image_base = CurrentImageBase;
    EnableAllHooks(TH20::THInitHook);
}
}
