﻿#include "thprac_games.h"
#include "thprac_utils.h"
#include <format>

#include "utils/wininternal.h"

// WTF Microsoft
#undef hyper

namespace THPrac {
namespace TH20TR {
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
        int32_t power;
        int32_t value;

        float hyper;
        float stone;
        int32_t levelR;
        int32_t priorityR;
        int32_t levelB;
        int32_t priorityB;
        int32_t levelY;
        int32_t priorityY;
        int32_t levelG;
        int32_t priorityG;

        bool dlg;

        bool _playLock = false;
        void Reset()
        {
            memset(this, 0, sizeof(THPracParam));
        }
        bool ReadJson(std::string& json)
        {
            ParseJson();

            // Game value is still th20 to maintain compatibility with thprac-th20tr replays
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
            GetJsonValue(power);
            GetJsonValue(value);

            GetJsonValue(hyper);
            GetJsonValue(stone);
            GetJsonValue(levelR);
            GetJsonValue(priorityR);
            GetJsonValue(levelB);
            GetJsonValue(priorityB);
            GetJsonValue(levelY);
            GetJsonValue(priorityY);
            GetJsonValue(levelG);
            GetJsonValue(priorityG);

            return true;
        }
        std::string GetJson()
        {
            if (mode == 1) {
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
                AddJsonValue(power);
                AddJsonValue(value);

                AddJsonValue(hyper);
                AddJsonValue(stone);
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
            *mLife = 9;
            *mBomb = 9;
            *mPower = 400;
            *mValue = 0;
            *mLevelR = 1;
            *mLevelB = 1;
            *mLevelY = 1;
            *mLevelG = 1;

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
                mDiffculty = *((int32_t*)RVA(0x1AEEC0));
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
                thPracParam.power = *mPower;
                thPracParam.value = *mValue;

                thPracParam.hyper = *mHyper / 10000.0f;
                thPracParam.stone = *mStone / 10000.0f;
                thPracParam.levelR = *mLevelR;
                thPracParam.priorityR = *mLevelR;
                thPracParam.levelB = *mLevelB;
                thPracParam.priorityB = *mLevelB;
                thPracParam.levelG = *mLevelG;
                thPracParam.priorityG = *mLevelG;
                thPracParam.levelY = *mLevelY;
                thPracParam.priorityY = *mLevelY;
                break;
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
            return nullptr;
        }
        void PracticeMenu()
        {
            mMode();
            if (mStage())
                *mSection = *mChapter = 0;
            if (*mMode == 1) {
                int mbs = -1;
                if (*mStage == 2) { // Counting from 0
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

                // TODO: This is ass
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
        Gui::GuiCombo mStage { TH_STAGE, TH_STAGE_SELECT_TRIAL };
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
        Gui::GuiSlider<int, ImGuiDataType_S32> mPower { TH_POWER, 100, 400 };
        Gui::GuiSlider<int, ImGuiDataType_S32> mValue { TH_VALUE, 0, 1000000 };

        Gui::GuiSlider<int, ImGuiDataType_S32> mHyper { TH20_HYPER, 0, 10000, 1, 1000 };
        Gui::GuiSlider<int, ImGuiDataType_S32> mStone { TH20_STONE_GAUGE, 0, 10000, 1, 1000 };
        Gui::GuiSlider<int32_t, ImGuiDataType_S32> mLevelR { TH20_STONE_LEVEL_R, 1, 5 };
        Gui::GuiSlider<int32_t, ImGuiDataType_S32> mPriorityR { TH20_STONE_PRIORITY_R, 0, 1000 };
        Gui::GuiSlider<int32_t, ImGuiDataType_S32> mLevelB { TH20_STONE_LEVEL_B, 1, 5 };
        Gui::GuiSlider<int32_t, ImGuiDataType_S32> mPriorityB { TH20_STONE_PRIORITY_B, 0, 1000 };
        Gui::GuiSlider<int32_t, ImGuiDataType_S32> mLevelY { TH20_STONE_LEVEL_Y, 1, 5 };
        Gui::GuiSlider<int32_t, ImGuiDataType_S32> mPriorityY { TH20_STONE_PRIORITY_Y, 0, 1000 };
        Gui::GuiSlider<int32_t, ImGuiDataType_S32> mLevelG { TH20_STONE_LEVEL_G, 1, 5 };
        Gui::GuiSlider<int32_t, ImGuiDataType_S32> mPriorityG { TH20_STONE_PRIORITY_G, 0, 1000 };

        int mChapterSetup[7][2] {
            { 3, 2 },
            { 3, 3 },
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
            uint32_t index = GetMemContent(RVA(0x1C3DB4), 0x5734);
            char* repName = (char*)GetMemAddr(RVA(0x1C3DB4), index * 4 + 0x573C, 0x150);
            std::wstring repDir(mAppdataPath);
            repDir.append(L"\\ShanghaiAlice\\th20tr\\replay\\");
            repDir.append(mb_to_utf16(repName, 932));

            std::string param;
            if (ReplayLoadParam(repDir.c_str(), param) && mRepParam.ReadJson(param))
                mParamStatus = true;
            else
                mRepParam.Reset();

            uint32_t* savedStones = (uint32_t*)GetMemAddr(RVA(0x1C3DB4), index * 4 + 0x573C, 0x1C, 0xDC);
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
            mElBgm.SetTextOffsetRel(x_offset_1, x_offset_2);
            mInternalGauges.SetTextOffsetRel(x_offset_1, x_offset_2);
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
            mElBgm();
            mInternalGauges();
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
        PATCH_HK(0xFB5CC, "01")
        HOTKEY_ENDDEF();

        HOTKEY_DEFINE(mInfLives, TH_INFLIVES, "F2", VK_F2)
        PATCH_HK(0xE3C08, "660F1F440000")
        HOTKEY_ENDDEF();

        HOTKEY_DEFINE(mInfBombs, TH_INFBOMBS, "F3", VK_F3)
        PATCH_HK(0xE40A2, "0F1F00")
        HOTKEY_ENDDEF();

        HOTKEY_DEFINE(mInfPower, TH_INFPOWER, "F4", VK_F4)
        PATCH_HK(0xE4022, "0F1F00")
        HOTKEY_ENDDEF();

        HOTKEY_DEFINE(mHyperGLock, TH20_HYP_LOCK, "F5", VK_F5)
        PATCH_HK(0x133C15, "0F1F00")
        HOTKEY_ENDDEF();

        HOTKEY_DEFINE(mWonderStGLock, TH20_WST_LOCK, "F6", VK_F6)
        PATCH_HK(0x7AB35, "0F1F00")
        HOTKEY_ENDDEF();

        HOTKEY_DEFINE(mTimeLock, TH_TIMELOCK, "F7", VK_F7)
        PATCH_HK(0x8A5FD, "EB"),
            PATCH_HK(0xA8ECE, "21")
                HOTKEY_ENDDEF();

    public:
        Gui::GuiHotKey mElBgm { TH_EL_BGM, "F8", VK_F8 };
        Gui::GuiHotKey mInternalGauges { TH20_INTERNAL_GAUGES, "F9", VK_F9 };
    };

    class TH20TRInternalGauges : public Gui::GameGuiWnd {
        TH20TRInternalGauges() noexcept
        {
            SetTitle("igi");
            SetFade(0.75f, 0.75f);
            SetPosRel((1056.0f - 300.0f / 2.0) / 1280.0f, (700.0f - 192.0f / 2.0) / 960.0f);
            SetSizeRel(300.0f / 1280.0f, 192.0f / 960.0f);
            SetWndFlag(ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav | 0);
            OnLocaleChange();
        }
        SINGLETON(TH20TRInternalGauges);

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
            static const ImVec4 STONE_COLORS[] = {
                { 1.0f, 0.25f, 0.25f, 1.0f },
                { 0.25f, 0.25f, 1.0f, 1.0f },
                { 1.0f, 1.0f, 0.25f, 1.0f },
                { 0.0f, 1.0f, 0.25f, 1.0f },
            };
            static constexpr const char* STONE_NAMES[] = {
                "Red",
                "Blue",
                "Yellow",
                "Green",
            };
            static constexpr const char* STONE_NAMES_SHORT[] = {
                "R",
                "B",
                "Y",
                "G",
            };

            uintptr_t player_stats = RVA(0x1B8670);
            int32_t red_priority = *(int32_t*)(player_stats + 0x64);
            int32_t blue_priority = *(int32_t*)(player_stats + 0x68);
            int32_t yellow_priority = *(int32_t*)(player_stats + 0x6C);
            int32_t green_priority = *(int32_t*)(player_stats + 0x70);

            int32_t next_stone = *(int32_t*)(player_stats + 0x98);
            bool locked = true;
            if (next_stone < 0) {
                locked = false;

                next_stone = 0;
                int32_t max_priority = red_priority;
                if (blue_priority > max_priority) {
                    next_stone = 1;
                    max_priority = blue_priority;
                }
                if (yellow_priority > max_priority) {
                    next_stone = 2;
                    max_priority = yellow_priority;
                }
                if (green_priority > max_priority) {
                    next_stone = 3;
                    max_priority = green_priority;
                }
            }

            auto& style = ImGui::GetStyle();
            auto old_col = style.Colors[ImGuiCol_Text];

            ImGui::Columns(2);
            style.Colors[ImGuiCol_Text] = STONE_COLORS[next_stone];
            ImGui::Text(S(TH20_NEXT_STONE));
            ImGui::NextColumn();
            ImGui::Text("%s%s", STONE_NAMES_SHORT[next_stone], locked ? " (LOCKED)" : "");
            ImGui::NextColumn();
            style.Colors[ImGuiCol_Text] = STONE_COLORS[0];
            ImGui::Text(STONE_NAMES[0]);
            ImGui::NextColumn();
            ImGui::Text("%d", red_priority);
            ImGui::NextColumn();
            style.Colors[ImGuiCol_Text] = STONE_COLORS[1];
            ImGui::Text(STONE_NAMES[1]);
            ImGui::NextColumn();
            ImGui::Text("%d", blue_priority);
            ImGui::NextColumn();
            style.Colors[ImGuiCol_Text] = STONE_COLORS[2];
            ImGui::Text(STONE_NAMES[2]);
            ImGui::NextColumn();
            ImGui::Text("%d", yellow_priority);
            ImGui::NextColumn();
            style.Colors[ImGuiCol_Text] = STONE_COLORS[3];
            ImGui::Text(STONE_NAMES[3]);
            ImGui::NextColumn();
            ImGui::Text("%d", green_priority);

            style.Colors[ImGuiCol_Text] = old_col;
        }

        virtual void OnPreUpdate() override
        {
            if (*(THOverlay::singleton().mInternalGauges) && *(DWORD*)(RVA(0x1B85EC))) {
                SetPosRel((1056.0f - 300.0f / 2.0) / 1280.0f, (700.0f - 192.0f / 2.0) / 960.0f);
                SetSizeRel(300.0f / 1280.0f, 192.0f / 960.0f);
                Open();
            } else {
                Close();
            }
        }

    public:
    };

    static constinit HookCtx listIterUnlinkFix = { .addr = 0x11AB2, .data = PatchCode("e800000000") };
    EHOOK_ST(th20_piv_overflow_fix, 0xC5FDB, 2, {
        uintptr_t stats = RVA(0x1B8670);
        int32_t piv = *(int32_t*)(stats + 0x44);
        int32_t piv_base = *(int32_t*)(stats + 0x40); // always 10000?
        int32_t piv_divisor = *(int32_t*)(stats + 0x48); // always 5000?

        int32_t half_piv_base = piv_base / 2;
        int64_t uh_oh = (int64_t)piv_base * piv;
        pCtx->Esi = (int32_t)(uh_oh / piv_divisor) + half_piv_base;
        pCtx->Eip = RVA(0xC6034);
    });
    PATCH_ST(th20_piv_uncap_1, 0xAA775, "89D00F1F00");
    PATCH_ST(th20_piv_uncap_2, 0xB9316, "89D00F1F00");
    PATCH_ST(th20_score_uncap, 0xE3E72, "EB");
    PATCH_ST(th20_infinite_stones, 0x11A250, "EB");
    PATCH_ST(th20_hitbox_scale_fix, 0x102280, "B864000000C3");

    PATCH_ST(th20_bullet_hitbox_fix_1, 0x3E7F2, "F30F108080000000F30F5C4224");
    PATCH_ST(th20_bullet_hitbox_fix_2, 0x3E80A, "F30F108284000000F30F5C4128");


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
        bool bulletHitboxFix = true;

        void MasterDisableInit()
        {
        }
        void FpsInit()
        {
            mOptCtx.fps_dbl = 60.0;

            if (*(uint8_t*)RVA(0x1C2F99) == 3) {
                mOptCtx.fps_status = 1;

                DWORD oldProtect;
                VirtualProtect((void*)RVA(0x1A22F), 4, PAGE_EXECUTE_READWRITE, &oldProtect);
                *(double**)RVA(0x1A22F) = &mOptCtx.fps_dbl;
                VirtualProtect((void*)RVA(0x1A22F), 4, oldProtect, &oldProtect);
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
            th20_infinite_stones.Setup();
            th20_hitbox_scale_fix.Setup();
            th20_bullet_hitbox_fix_1.Setup();
            th20_bullet_hitbox_fix_2.Setup();

            th20_bullet_hitbox_fix_1.Enable();
            th20_bullet_hitbox_fix_2.Enable();

            // thcrap base_tsa already patches this to fix the crash, don't try to rehook it if it's being used
            if (*(uint32_t*)RVA(0x11AD0) == 0x51EC8B55) {
                *(uintptr_t*)((uintptr_t)listIterUnlinkFix.data.buffer.ptr + 1) = (uintptr_t)&UnlinkNodeHook - RVA(0x11AB2 + 5);
                listIterUnlinkFix.Setup();
                listIterUnlinkFix.Enable();
            }
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
                if (ImGui::Checkbox(S(TH20_PIV_OVERFLOW_FIX), &pivOverflowFix))
                    th20_piv_overflow_fix.Toggle(pivOverflowFix);
                ImGui::SameLine();
                if (ImGui::Checkbox(S(TH20_UNCAP_PIV), &pivUncap)) {
                    th20_piv_uncap_1.Toggle(pivUncap);
                    th20_piv_uncap_2.Toggle(pivUncap);
                }
                ImGui::SameLine();
                if (ImGui::Checkbox(S(TH20_UNCAP_SCORE), &scoreUncap))
                    th20_score_uncap.Toggle(scoreUncap);

                if (ImGui::Checkbox(S(TH20_FAKE_UNLOCK_STONES), &infiniteStones))
                    th20_infinite_stones.Toggle(infiniteStones);
                ImGui::SameLine();
                HelpMarker(S(TH20_FAKE_UNLOCK_STONES_DESC));
                ImGui::SameLine();
                if (ImGui::Checkbox(S(TH20_FIX_HITBOX), &plHitboxScaleFix))
                    th20_hitbox_scale_fix.Toggle(plHitboxScaleFix);
                ImGui::SameLine();
                HelpMarker(S(TH20_FIX_HITBOX_DESC));

                if (ImGui::Checkbox("bullet hitbox fix", &bulletHitboxFix)){
                    th20_bullet_hitbox_fix_1.Toggle(bulletHitboxFix);
                    th20_bullet_hitbox_fix_2.Toggle(bulletHitboxFix);
                }
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
    constexpr unsigned int st3PostMaple = 0xb3e8;
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
            constexpr unsigned int st3MainFront = 0xb9e0;
            constexpr unsigned int st3MainSub00 = 0x903c;

            switch (portion) {
            case 1:
                break;
            case 2: {
                constexpr unsigned int mainSub01Call = 0x909c;
                ECLJump(ecl, st3PostMaple, st3MainFront, 60, 90);
                ECLJump(ecl, st3MainSub00, mainSub01Call, 0, 0);
                break;
            }
            case 3: {
                constexpr unsigned int mainSub03Call = 0x915c;
                ECLJump(ecl, st3PostMaple, st3MainFront, 60, 90);
                ECLJump(ecl, st3MainSub00, mainSub03Call, 0, 0);
                break;
            }
            case 4: {
                constexpr unsigned int mainSub04Call = 0x91bc;
                ECLJump(ecl, st3PostMaple, st3MainFront, 60, 90);
                ECLJump(ecl, st3MainSub00, mainSub04Call, 0, 0);
                break;
            }
            case 5: {
                constexpr unsigned int mainSub05Call = 0x921c;
                ECLStdExec(ecl, st3PostMaple, 1, 1);
                ECLJump(ecl, st3PostMaple + stdInterruptSize, st3MainFront, 60, 90);
                ECLJump(ecl, st3MainSub00, mainSub05Call, 0, 0);
                break;
            }
            case 6: {
                constexpr unsigned int mainSub06Call = 0x927c;
                ECLStdExec(ecl, st3PostMaple, 1, 1);
                ECLJump(ecl, st3PostMaple + stdInterruptSize, st3MainFront, 60, 90);
                ECLJump(ecl, st3MainSub00, mainSub06Call, 0, 0);
                break;
            }
            case 7: {
                constexpr unsigned int mainSub07Call = 0x92dc;
                ECLStdExec(ecl, st3PostMaple, 1, 1);
                ECLJump(ecl, st3PostMaple + stdInterruptSize, st3MainFront, 60, 90);
                ECLJump(ecl, st3MainSub00, mainSub07Call, 0, 0);
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
        constexpr unsigned int st1bsSpellSubCallOrd = 0x4d0;

        constexpr unsigned int st2BossCreateCall = 0xabf0;
        constexpr unsigned int st2bsPrePushSpellID = 0x3d0;
        constexpr unsigned int st2bsPostNotSpellPracCheck = 0x4b8;
        constexpr unsigned int st2bsSpellSubCallOrd = 0x4e8;
        constexpr unsigned int st2bsNonSubCallOrd = 0x628;

        constexpr unsigned int st3BossCreateCall = 0xbaa0;
        constexpr unsigned int st3bsPrePushSpellID = 0x458;
        constexpr unsigned int st3bsPostNotSpellPracCheck = 0x540;
        constexpr unsigned int st3bsSpellSubCallOrd = 0x570;
        constexpr unsigned int st3bsNonSubCallOrd = 0x6d0;

        switch (section) {
        case THPrac::TH20TR::TH20_ST1_MID1: {
            constexpr unsigned int st1MBossCreateCall = 0x8568;
            ECLStdExec(ecl, st1PostMaple, 1, 1);
            ECLJump(ecl, st1PostMaple + stdInterruptSize, st1MBossCreateCall, 60, 90);
            break;
        }
        case THPrac::TH20TR::TH20_ST1_BOSS1: {
            constexpr unsigned int st1BossDialogueCall = 0x866c;
            ECLStdExec(ecl, st1PostMaple, 1, 1);
            if (thPracParam.dlg)
                ECLJump(ecl, st1PostMaple + stdInterruptSize, st1BossDialogueCall, 60);
            else
                ECLJump(ecl, st1PostMaple + stdInterruptSize, st1BossCreateCall, 60);
            break;
        }
        case THPrac::TH20TR::TH20_ST1_BOSS2: {
            ECLStdExec(ecl, st1PostMaple, 1, 1);
            ECLJump(ecl, st1PostMaple + stdInterruptSize, st1BossCreateCall, 60);
            ecl.SetFile(2);
            ECLJump(ecl, st1bsPrePushSpellID, st1bsPostNotSpellPracCheck, 1); // Utilize Spell Practice Jump
            break;
        }
        case THPrac::TH20TR::TH20_ST1_BOSS3: {
            constexpr unsigned int st1bsNonSubCallOrd = 0x610;
            constexpr unsigned int st1bsNon2InvulnCallVal = 0x1268;
            constexpr unsigned int st1bsNon2BossItemCallSomething = 0x1344 + 0x4; // 32th cringequit on me when
            constexpr unsigned int st1bsNon2PlaySoundSomething = 0x1470 + 0x4; // I asked what these were so :shrug:
            constexpr unsigned int st1bsNon2PostLifeMarker = 0x15c8;
            constexpr unsigned int st1bsNon2PostWait = 0x16b4; // 0x1608 previously

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
        case THPrac::TH20TR::TH20_ST1_BOSS4: {
            ECLStdExec(ecl, st1PostMaple, 1, 1);
            ECLJump(ecl, st1PostMaple + stdInterruptSize, st1BossCreateCall, 60);
            ecl.SetFile(2);
            ECLJump(ecl, st1bsPrePushSpellID, st1bsPostNotSpellPracCheck, 1); // Utilize Spell Practice Jump
            ecl << pair { st1bsSpellSubCallOrd, (int8_t)0x32 }; // Set spell ID in sub call to '2'
            break;
        }
        case THPrac::TH20TR::TH20_ST1_BOSS5: {
            ECLStdExec(ecl, st1PostMaple, 1, 1);
            ECLJump(ecl, st1PostMaple + stdInterruptSize, st1BossCreateCall, 60);
            ecl.SetFile(2);
            ECLJump(ecl, st1bsPrePushSpellID, st1bsPostNotSpellPracCheck, 1); // Utilize Spell Practice Jump
            ecl << pair { st1bsSpellSubCallOrd, (int8_t)0x33 }; // Set spell ID in sub call to '3'
            break;
        }
        case THPrac::TH20TR::TH20_ST2_MID1: {
            constexpr unsigned int st2MBossCreateCall = 0xaad8;
            ECLJump(ecl, st2PostMaple, st2MBossCreateCall, 60, 90);
            break;
        }
        case THPrac::TH20TR::TH20_ST2_BOSS1: {
            constexpr unsigned int st2BossDialogueCall = 0xabdc;
            ECLStdExec(ecl, st2PostMaple, 1, 1);
            if (thPracParam.dlg)
                ECLJump(ecl, st2PostMaple + stdInterruptSize, st2BossDialogueCall, 60);
            else
                ECLJump(ecl, st2PostMaple + stdInterruptSize, st2BossCreateCall, 60);
            break;
        }
        case THPrac::TH20TR::TH20_ST2_BOSS2: {
            ECLStdExec(ecl, st2PostMaple, 1, 1);
            ECLJump(ecl, st2PostMaple + stdInterruptSize, st2BossCreateCall, 60);
            ecl.SetFile(2);
            ECLJump(ecl, st2bsPrePushSpellID, st2bsPostNotSpellPracCheck, 1); // Utilize Spell Practice Jump
            break;
        }
        case THPrac::TH20TR::TH20_ST2_BOSS3: {
            constexpr unsigned int st2bsNon2InvulnCallVal = 0x11d4;
            constexpr unsigned int st2bsNon2BossItemCallSomething = 0x12b0 + 0x4;
            constexpr unsigned int st2bsNon2PlaySoundSomething = 0x13dc + 0x4;
            constexpr unsigned int st2bsNon2PostLifeMarker = 0x1534;
            constexpr unsigned int st2bsNon2PostWait = 0x1620;

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
        case THPrac::TH20TR::TH20_ST2_BOSS4: {
            ECLStdExec(ecl, st2PostMaple, 1, 1);
            ECLJump(ecl, st2PostMaple + stdInterruptSize, st2BossCreateCall, 60);
            ecl.SetFile(2);
            ECLJump(ecl, st2bsPrePushSpellID, st2bsPostNotSpellPracCheck, 1); // Utilize Spell Practice Jump
            ecl << pair { st2bsSpellSubCallOrd, (int8_t)0x32 }; // Set spell ID in sub call to '2'
            break;
        }
        case THPrac::TH20TR::TH20_ST2_BOSS5: {
            ECLStdExec(ecl, st2PostMaple, 1, 1);
            ECLJump(ecl, st2PostMaple + stdInterruptSize, st2BossCreateCall, 60);
            ecl.SetFile(2);
            ECLJump(ecl, st2bsPrePushSpellID, st2bsPostNotSpellPracCheck, 1); // Utilize Spell Practice Jump
            ecl << pair { st2bsSpellSubCallOrd, (int8_t)0x33 }; // Set spell ID in sub call to '3'
            break;
        }

        case THPrac::TH20TR::TH20_ST3_BOSS1: {
            constexpr unsigned int st3BossDialogueCall = 0xba8c;
            ECLStdExec(ecl, st3PostMaple, 1, 1);
            if (thPracParam.dlg)
                ECLJump(ecl, st3PostMaple + stdInterruptSize, st3BossDialogueCall, 60);
            else
                ECLJump(ecl, st3PostMaple + stdInterruptSize, st3BossCreateCall, 60);
            break;
        }
        case THPrac::TH20TR::TH20_ST3_BOSS2: {
            ECLStdExec(ecl, st3PostMaple, 1, 1);
            ECLJump(ecl, st3PostMaple + stdInterruptSize, st3BossCreateCall, 60);
            ecl.SetFile(2);
            ECLJump(ecl, st3bsPrePushSpellID, st3bsPostNotSpellPracCheck, 1); // Utilize Spell Practice Jump
            break;
        }
        case THPrac::TH20TR::TH20_ST3_BOSS3: {
            constexpr unsigned int st3bsNon2InvulnCallVal = 0x1074;
            constexpr unsigned int st3bsNon2BossItemCallSomething = 0x1150 + 0x4;
            constexpr unsigned int st3bsNon2PlaySoundSomething = 0x127c + 0x4;
            constexpr unsigned int st3bsNon2PostLifeMarker = 0x13d4;
            constexpr unsigned int st3bsNon2PostWait = 0x1500;

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
        case THPrac::TH20TR::TH20_ST3_BOSS4: {
            ECLStdExec(ecl, st3PostMaple, 1, 1);
            ECLJump(ecl, st3PostMaple + stdInterruptSize, st3BossCreateCall, 60);
            ecl.SetFile(2);
            ECLJump(ecl, st3bsPrePushSpellID, st3bsPostNotSpellPracCheck, 1); // Utilize Spell Practice Jump
            ecl << pair { st3bsSpellSubCallOrd, (int8_t)0x32 }; // Set spell ID in sub call to '2'
            break;
        }
        case THPrac::TH20TR::TH20_ST3_BOSS5: {
            constexpr unsigned int st3bsNon3InvulnCallVal = 0x1d0c;
            constexpr unsigned int st3bsNon3BossItemCallSomething = 0x1de8 + 0x4;
            constexpr unsigned int st3bsNon3PlaySoundSomething = 0x1f14 + 0x4;
            constexpr unsigned int st3bsNon3PostLifeCount = 0x2080; // is this right? life count = 0? zun...
            constexpr unsigned int st3bsNon3PostWait = 0x21ac;

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
        case THPrac::TH20TR::TH20_ST3_BOSS6: {
            ECLStdExec(ecl, st3PostMaple, 1, 1);
            ECLJump(ecl, st3PostMaple + stdInterruptSize, st3BossCreateCall, 60);
            ecl.SetFile(2);
            ECLJump(ecl, st3bsPrePushSpellID, st3bsPostNotSpellPracCheck, 1); // Utilize Spell Practice Jump
            ecl << pair { st3bsSpellSubCallOrd, (int8_t)0x33 }; // Set spell ID in sub call to '3'
            break;
        }
        case THPrac::TH20TR::TH20_ST3_BOSS7: {
            ECLStdExec(ecl, st3PostMaple, 1, 1);
            ECLJump(ecl, st3PostMaple + stdInterruptSize, st3BossCreateCall, 60);
            ecl.SetFile(2);
            ECLJump(ecl, st3bsPrePushSpellID, st3bsPostNotSpellPracCheck, 1); // Utilize Spell Practice Jump
            ecl << pair { st3bsSpellSubCallOrd, (int8_t)0x34 }; // Set spell ID in sub call to '4'
            break;
        }
        default:
            break;
        }
    }
    __declspec(noinline) void THSectionPatch()
    {
        ECLHelper ecl;
        ecl.SetBaseAddr((void*)GetMemAddr(RVA(0x1b85f0), 0x104, 0xc));

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

    static bool sGameStarted = false;
    static char* sReplayPath = nullptr;

    HOOKSET_DEFINE(THMainHook)
    EHOOK_DY(th20_everlasting_bgm, 0x28710, 1, {
        int32_t retn_addr = ((int32_t*)pCtx->Esp)[0] - ingame_image_base;
        int32_t bgm_cmd = ((int32_t*)pCtx->Esp)[1];
        int32_t bgm_id = ((int32_t*)pCtx->Esp)[2];
        // 4th stack item = i32 call_addr

        bool el_switch;
        bool is_practice;
        bool result;

        el_switch = *(THOverlay::singleton().mElBgm) && !THGuiRep::singleton().mRepStatus && (thPracParam.mode == 1) && thPracParam.section;
        is_practice = (*((int32_t*)RVA(0x1b8654)) & 0x1);

        result = ElBgmTest<0xdc6a0, 0xdc70e, 0xe8978, 0xe8d28, 0xffffffff>(
            el_switch, is_practice, retn_addr, bgm_cmd, bgm_id, 0xffffffff);

        if (result) {
            pCtx->Eip = RVA(0x028855);
        }
    })
    EHOOK_DY(th20_param_reset, 0x1294C6, 3, {
        thPracParam.Reset();
    })
    EHOOK_DY(th20_prac_menu_1, 0x12A92A, 5, {
        sGameStarted = false;
        THGuiPrac::singleton().State(1);
    })
    EHOOK_DY(th20_prac_menu_2, 0x12A958, 2, {
        if (!sGameStarted) {
            if (Gui::InGameInputGetConfirm()) {
                sGameStarted = true;
                asm_call_rel<0x238C0, Thiscall>(*(uint32_t*)RVA(0x1C3DB4) + 0x154, 0);
                THGuiPrac::singleton().State(3);
            } else if (Gui::InGameInputGet('X')) {
                THGuiPrac::singleton().State(4);
                asm_call_rel<0x12B640, Thiscall>(*(uint32_t*)RVA(0x1C3DB4), 2);
                pCtx->Eip = RVA(0x12AC10);
            } else {
                THGuiPrac::singleton().State(2);
                pCtx->Eip = RVA(0x12AC10);
            }
        }
    })
    EHOOK_DY(th20_disable_game_start, 0x12AADC, 3, {
        if (THGuiPrac::singleton().IsOpen())
            pCtx->Eip = RVA(0x12AC10);
    })
    EHOOK_DY(th20_stone_menu_fade, 0x11A740, 6, {
        if (THGuiPrac::singleton().IsOpen())
            pCtx->Eip = RVA(0x11A763);
    })
    EHOOK_DY(th20_prac_menu_set_stage, 0x12AB34, 5, {
        *(int*)pCtx->Esp = thPracParam.stage + 1;
    })
    PATCH_DY(th20_instant_esc_r, 0xE59C5, "EB")
    EHOOK_DY(th20_patch_main, 0xBCF34, 1, {
        if (thPracParam.mode == 1) {
            *(int32_t*)RVA(0x1B8670) = (int32_t)(thPracParam.score / 10);
            *(int32_t*)RVA(0x1B8728) = thPracParam.life;
            *(int32_t*)RVA(0x1B8730) = thPracParam.life_fragment;
            *(int32_t*)RVA(0x1B873C) = thPracParam.bomb;
            *(int32_t*)RVA(0x1B8740) = thPracParam.bomb_fragment;
            *(int32_t*)RVA(0x1B86A0) = thPracParam.power;
            *(int32_t*)RVA(0x1B86B4) = thPracParam.value;

            THSectionPatch();
        }
        thPracParam._playLock = true;
    })
    EHOOK_DY(th20_patch_stones, 0x133A21, 1, {
        if (thPracParam.mode != 1)
            return;

        uintptr_t player_stats = RVA(0x1B8670);
        *(int32_t*)(player_stats + 0x4C) = (int32_t)(thPracParam.hyper * *(int32_t*)(player_stats + 0x50));
        if ((int32_t)thPracParam.hyper == 1) { // call the hyper start method
            int32_t* gauge_manager_ptr = (int32_t*)RVA(0x1b8614);
            asm_call_rel<0x134fe0, Fastcall>(*gauge_manager_ptr);
        }

        *(int32_t*)(player_stats + 0x5C) = (int32_t)(thPracParam.stone * *(int32_t*)(player_stats + 0x60));
        *(int32_t*)(player_stats + 0x64) = thPracParam.priorityR;
        *(int32_t*)(player_stats + 0x68) = thPracParam.priorityB;
        *(int32_t*)(player_stats + 0x6C) = thPracParam.priorityY;
        *(int32_t*)(player_stats + 0x70) = thPracParam.priorityG;
        *(int32_t*)(player_stats + 0x74) = thPracParam.levelR;
        *(int32_t*)(player_stats + 0x78) = thPracParam.levelB;
        *(int32_t*)(player_stats + 0x7C) = thPracParam.levelY;
        *(int32_t*)(player_stats + 0x80) = thPracParam.levelG;
    })
    EHOOK_DY(th20_boss_bgm, 0xBBFC8, 2, {
        if (THBGMTest()) {
            PushHelper32(pCtx, 1);
            pCtx->Eip = RVA(0x0bbfcf);
        }
    })
    EHOOK_DY(th20_rep_save, 0x10D813, 3, {
        if (sReplayPath) {
            if (thPracParam.mode == 1)
                THSaveReplay(sReplayPath);
            free(sReplayPath);
            sReplayPath = nullptr;
        }
    })
    EHOOK_DY(th20_rep_get_path, 0x10D3E1, 5, {
        sReplayPath = _strdup((char*)pCtx->Edx);
    })
    EHOOK_DY(th20_rep_menu_1, 0x124D44, 3, {
        THGuiRep::singleton().State(1);
    })
    EHOOK_DY(th20_rep_menu_2, 0x12504C, 5, {
        THGuiRep::singleton().State(2);
    })
    EHOOK_DY(th20_rep_menu_3, 0x1254DE, 2, {
        THGuiRep::singleton().State(3);
    })
    EHOOK_DY(th20_fix_rep_stone_init, 0xBC3D0, 5, {
        if (*(uint32_t*)(*(uintptr_t*)(RVA(0x1B85E8) + 0x88 + 0x238) + 0x108))
            memcpy((void*)(RVA(0x1B85E8) + 0x88 + 0x1C), replayStones, sizeof(replayStones));
    })
    PATCH_DY(th20_fix_rep_results_skip, 0x1133B1, "5B35FAFF")
    EHOOK_DY(th20_update, 0x12824, 1, {
        GameGuiBegin(IMPL_WIN32_DX9, !THAdvOptWnd::singleton().IsOpen());

        // Gui components update
        THGuiPrac::singleton().Update();
        THGuiRep::singleton().Update();
        THOverlay::singleton().Update();
        TH20TRInternalGauges::singleton().Update();

        bool drawCursor = THAdvOptWnd::StaticUpdate() || THGuiPrac::singleton().IsOpen();
        GameGuiEnd(drawCursor);
    })
    EHOOK_DY(th20_render, 0x129C6, 1, {
        GameGuiRender(IMPL_WIN32_DX9);
    })
    HOOKSET_ENDDEF()

    static __declspec(noinline) void THGuiCreate()
    {
        if (ImGui::GetCurrentContext())
            return;

        // Init
        GameGuiInit(IMPL_WIN32_DX9, RVA(0x1C2D58), RVA(0x1B47D8),
            Gui::INGAGME_INPUT_GEN2, RVA(0x1B6940), RVA(0x1B6938), 0,
            -2, *(float*)RVA(0x1B6898), 0.0f);

        SetDpadHook(0x227B1, 6);

        // Gui components creation
        THGuiPrac::singleton();
        THGuiRep::singleton();
        THOverlay::singleton();
        TH20TRInternalGauges::singleton();

        // Hooks
        EnableAllHooks(THMainHook);

        //  Reset thPracParam
        thPracParam.Reset();
    }

    HOOKSET_DEFINE(THInitHook)
    PATCH_DY(th20_startup_1, 0x121A0C, "EB")
    PATCH_DY(th20_startup_2, 0x120BF1, "EB")
    EHOOK_DY(th20_gui_init_1, 0x12967E, 7, {
        self->Disable();
        THGuiCreate();
    })
    EHOOK_DY(th20_gui_init_2, 0x1CA95, 1, {
        self->Disable();
        THGuiCreate();
    })
    HOOKSET_ENDDEF()
}

void TH20TRInit()
{
    ingame_image_base = CurrentImageBase;
    EnableAllHooks(TH20TR::THInitHook);
}
}