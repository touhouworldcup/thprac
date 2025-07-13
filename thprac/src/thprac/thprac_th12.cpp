#include "thprac_games.h"
#include "thprac_utils.h"
#include <format>


namespace THPrac {
namespace TH12 {
    int g_lock_timer = 0;

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
        int32_t power;
        int32_t value;
        int32_t graze;
        int32_t ufo_side;
        int32_t ventra_1;
        int32_t ventra_2;
        int32_t ventra_3;

        bool dlg;

        bool _playLock = false;
        void Reset()
        {
            memset(this, 0, sizeof(THPracParam));
        }
        bool ReadJson(std::string& json)
        {
            ParseJson();

            ForceJsonValue(game, "th12");
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
            GetJsonValue(graze);
            GetJsonValue(ufo_side);
            GetJsonValue(ventra_1);
            GetJsonValue(ventra_2);
            GetJsonValue(ventra_3);

            return true;
        }
        std::string GetJson()
        {
            CreateJson();

            AddJsonValueEx(version, GetVersionStr(), jalloc);
            AddJsonValueEx(game, "th12", jalloc);
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
            AddJsonValue(graze);
            AddJsonValue(ufo_side);
            if (ventra_1) {
                AddJsonValue(ventra_1);
                if (ventra_2) {
                    AddJsonValue(ventra_2);
                    if (ventra_3) {
                        AddJsonValue(ventra_3);
                    }
                }
            }


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
                mDiffculty = *((int32_t*)0x4aebd0);
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
                thPracParam.graze = *mGraze;
                thPracParam.ufo_side = *mUfoSide;

                if (*mVentra1) {
                    thPracParam.ventra_1 = *mVentra1;
                    if (*mVentra2) {
                        thPracParam.ventra_2 = *mVentra2;
                        if (*mVentra3) {
                            thPracParam.ventra_3 = *mVentra3;
                        }
                    }
                }

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
                SetSize(350.f, 410.f);
                SetPos(260.f, 50.f);
                SetItemWidth(-65.0f);
                break;
            case Gui::LOCALE_EN_US:
                SetSize(425.f, 410.f);
                SetPos(185.f, 50.f);
                SetItemWidth(-65.0f);
                break;
            case Gui::LOCALE_JA_JP:
                SetSize(350.f, 410.f);
                SetPos(260.f, 50.f);
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

            PracticeMenu();
        }
        const th_glossary_t* SpellPhase()
        {
            auto section = CalcSection();
            if (section == TH12_ST6_BOSS8)
                return TH12_SPELL_PHASE4_LOCK;
            else if(section == TH12_ST6_BOSS10) {
                return TH_SPELL_PHASE1;
            } else if (section == TH12_ST7_END_S10) {
                return TH_SPELL_PHASE3;
            } else if (section == 10000 + 4 * 100 + 8) {
                return TH_PHASE_INF_MODE;
            } else if (section == 10000 + 5 * 100 + 6)
            {
                return TH_PHASE_INF_MODE;
            }else if (section == TH12_ST5_MID2) {
                return TH_TIMEOUT_SETTING;
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
                auto power_str = std::to_string((float)(*mPower) / 100.0f).substr(0, 4);
                mPower(power_str.c_str());
                mUfoSide();
                mVentra1();
                if (*mVentra1) {
                    mVentra2();
                    if (*mVentra2) {
                        mVentra3();
                    }
                }
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
            case TH12_ST1_BOSS1:
            case TH12_ST2_BOSS1:
            case TH12_ST3_BOSS1:
            case TH12_ST4_BOSS1:
            case TH12_ST5_BOSS1:
            case TH12_ST5_MID1:
            case TH12_ST6_BOSS1:
            case TH12_ST7_END_NS1:
            case TH12_ST7_MID1:
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
        Gui::GuiCombo mUfoSide { TH12_UFO_SIDE, TH_SIDE_SELECT };
        Gui::GuiCombo mVentra1 { TH12_VENTRA_1, TH12_VENTRA_SELECT };
        Gui::GuiCombo mVentra2 { TH12_VENTRA_2, TH12_VENTRA_SELECT };
        Gui::GuiCombo mVentra3 { TH12_VENTRA_3, TH12_VENTRA_SELECT };

        Gui::GuiSlider<int, ImGuiDataType_S32> mChapter { TH_CHAPTER, 0, 0 };
        Gui::GuiDrag<int64_t, ImGuiDataType_S64> mScore { TH_SCORE, 0, 9999999990, 10, 100000000 };
        Gui::GuiSlider<int, ImGuiDataType_S32> mLife { TH_LIFE, 0, 9 };
        Gui::GuiSlider<int, ImGuiDataType_S32> mLifeFragment { TH_LIFE_FRAGMENT, 0, 3 };
        Gui::GuiSlider<int, ImGuiDataType_S32> mBomb { TH_BOMB, 0, 8 };
        Gui::GuiSlider<int, ImGuiDataType_S32> mBombFragment { TH_BOMB_FRAGMENT, 0, 2 };
        Gui::GuiSlider<int, ImGuiDataType_S32> mPower { TH_POWER, 0, 400 };
        Gui::GuiDrag<int, ImGuiDataType_S32> mValue { TH_VALUE, 0, 999990, 10, 100000 };
        Gui::GuiDrag<int, ImGuiDataType_S32> mGraze { TH_GRAZE, 0, 999999, 1, 100000 };

        Gui::GuiNavFocus mNavFocus { TH_STAGE, TH_MODE, TH_WARP,
            TH_MID_STAGE, TH_END_STAGE, TH_NONSPELL, TH_SPELL, TH_PHASE, TH_CHAPTER,
            TH_SCORE, TH_LIFE, TH_LIFE_FRAGMENT, TH_BOMB, TH_BOMB_FRAGMENT,
            TH_POWER, TH_VALUE, TH_GRAZE, TH12_UFO_SIDE, TH12_VENTRA_1, TH12_VENTRA_2, TH12_VENTRA_3 };

        int mChapterSetup[7][2] {
            { 3, 2 },
            { 3, 2 },
            { 4, 3 },
            { 4, 4 },
            { 4, 3 },
            { 3, 0 },
            { 4, 4 },
        };

        int mDiffculty = 0;
    };
    class THGuiRep : public Gui::GameGuiWnd {
        THGuiRep() noexcept
        {
        }
        SINGLETON(THGuiRep);
    public:

        void CheckReplay()
        {
            uint32_t index = GetMemContent(0x4b4530, 0x5a78);
            char* repName = (char*)GetMemAddr(0x4b4530, index * 4 + 0x5a80, 0x1e0);
            std::wstring repDir(L"replay/");
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
        HOTKEY_DEFINE(mMuteki, TH_MUTEKI, "F1", VK_F1)
        PATCH_HK(0x43837F, "01"),
        PATCH_HK(0x436d2f, "eb"),
        PATCH_HK(0x4383cb, "83c4109090")
        HOTKEY_ENDDEF();
        
        
        HOTKEY_DEFINE(mInfBombs, TH_INFBOMBS, "F3", VK_F3)
        PATCH_HK(0x422F27, "00")
        HOTKEY_ENDDEF();
        
        HOTKEY_DEFINE(mInfPower, TH_INFPOWER, "F4", VK_F4)
        PATCH_HK(0x43944B, "48")
        HOTKEY_ENDDEF();
        
        HOTKEY_DEFINE(mAutoBomb, TH_AUTOBOMB, "F6", VK_F6)
        PATCH_HK(0x436D9B, "c6")
        HOTKEY_ENDDEF();

    public:
        Gui::GuiHotKey mInfLives { TH_INFLIVES2, "F2", VK_F2,};
        HOTKEY_DEFINE(mTimeLock, TH_TIMELOCK, "F5", VK_F5)
        PATCH_HK(0x40DB71, "eb"),
        PATCH_HK(0x414A9B, "90")
        HOTKEY_ENDDEF();
        Gui::GuiHotKey mElBgm { TH_EL_BGM, "F7", VK_F7 };
        Gui::GuiHotKey mInGameInfo { THPRAC_INGAMEINFO, "F8", VK_F8 };
    };

    class TH12InGameInfo : public Gui::GameGuiWnd {

        TH12InGameInfo() noexcept
        {
            SetTitle("igi");
            SetFade(0.9f, 0.9f);
            SetPosRel(425.0f / 640.0f, 338.0f / 480.0f);
            SetSizeRel(210.0f / 640.0f, 0.0f);
            SetWndFlag(ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | 
                ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav | 0);
            OnLocaleChange();
        }
        SINGLETON(TH12InGameInfo);

    public:
        int32_t mMissCount;
        int32_t mBombCount;

        int32_t mRUFOCount;
        int32_t mGUFOCount;
        int32_t mBUFOCount;
        int32_t mCUFOCount;

        int32_t mRVentraCount_Get;
        int32_t mGVentraCount_Get;
        int32_t mBVentraCount_Get;

        int32_t mRVentraCount_Drop;
        int32_t mGVentraCount_Drop;
        int32_t mBVentraCount_Drop;
        int32_t mCVentraCount_Drop;

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
            int32_t cur_player_type = (*(DWORD*)(0x004B0C90)) * 2 + (*(DWORD*)(0x4B0C94));
            int32_t diff = *((int32_t*)0x4B0CA8);
            auto diff_pl = std::format("{} ({})", S(IGI_DIFF[diff]), S(IGI_PL_12[cur_player_type]));
            auto diff_pl_sz = ImGui::CalcTextSize(diff_pl.c_str());

            ImGui::SetCursorPosX(ImGui::GetWindowSize().x * 0.5 - diff_pl_sz.x * 0.5);
            ImGui::Text(diff_pl.c_str());

            auto ufo_cnt = std::format("{}/{}/{}/{}", mRUFOCount, mGUFOCount, mBUFOCount, mCUFOCount);
            auto ufo_cnt_sz = ImGui::CalcTextSize(ufo_cnt.c_str()).x;

            ImGui::Columns(2);
            auto width1 = ImGui::GetColumnWidth(1);
            if (width1 < ufo_cnt_sz)
                ImGui::SetColumnWidth(0, ImGui::GetWindowContentRegionWidth() - ufo_cnt_sz);

            ImGui::Text(S(THPRAC_INGAMEINFO_MISS_COUNT));
            ImGui::NextColumn();
            ImGui::Text("%8d", mMissCount);
            ImGui::NextColumn();
            ImGui::Text(S(THPRAC_INGAMEINFO_BOMB_COUNT));
            ImGui::NextColumn();
            ImGui::Text("%8d", mBombCount);

            ImGui::NextColumn();
            ImGui::Text(S(THPRAC_INGAMEINFO_12_UFO_COUNT3));
            ImGui::NextColumn();
            ImGui::Text("%8d", mRVentraCount_Drop + mGVentraCount_Drop + mBVentraCount_Drop + mCVentraCount_Drop);
            ImGui::NextColumn();
            ImGui::Text(S(THPRAC_INGAMEINFO_12_UFO_COUNT2));
            ImGui::NextColumn();
            ImGui::Text("%8d", mRVentraCount_Get + mGVentraCount_Get + mBVentraCount_Get);

            auto last_item_width = ImGui::GetItemRectSize().x;

            ImGui::NextColumn();
            ImGui::Text(S(THPRAC_INGAMEINFO_12_UFO_COUNT1));
            ImGui::NextColumn();
            if (ufo_cnt_sz * 0.5 < last_item_width)
                ImGui::SetCursorPosX(ImGui::GetCursorPosX() + last_item_width - ufo_cnt_sz * 0.5);
            ImGui::TextColored({ 1.0f, 0.5f, 0.5f, 1.0f }, "%d", mRUFOCount);
            ImGui::SameLine(0.0f,0.0f);
            ImGui::Text("/");
            ImGui::SameLine(0.0f,0.0f);
            ImGui::TextColored({ 0.5f, 1.0f, 0.5f, 1.0f }, "%d", mGUFOCount);
            ImGui::SameLine(0.0f, 0.0f);
            ImGui::Text("/");
            ImGui::SameLine(0.0f, 0.0f);
            ImGui::TextColored({ 0.5f, 0.75f, 1.0f, 1.0f }, "%d", mBUFOCount);
            ImGui::SameLine(0.0f, 0.0f);
            ImGui::Text("/");
            ImGui::SameLine(0.0f, 0.0f);
            if (g_adv_igi_options.th12_chromatic_ufo) {
                float r, g, b;
                static float h = 0.0f;
                h += 0.016f;
                ImGui::ColorConvertHSVtoRGB(h, 0.5f, 1.0f, r, g, b);
                ImGui::TextColored({ r, g, b, 1.0f }, "%d", mCUFOCount);
            }else{
                ImGui::Text("%d", mCUFOCount);
            }
            // ImGui::TextColored({ 1.0f, 1.0f, 1.0f, 1.0f }, "%d", mCUFOCount);
            // ImGui::Text(ufo_cnt.c_str());

        }

        virtual void OnPreUpdate() override
        {
            if (*(THOverlay::singleton().mInGameInfo) && *(DWORD*)(0x004B4514)) {
                SetPosRel(425.0f / 640.0f, 338.0f / 480.0f);
                SetSizeRel(210.0f / 640.0f, 0.0f);
                Open();
            } else {
                Close();
            }
        }

    public:
    };

    
    EHOOK_ST(th12_all_clear_bonus_1, 0x420a9b, 7, {
        pCtx->Eip = 0x420acd;
    });
    EHOOK_ST(th12_all_clear_bonus_2, 0x420bc2, 4, {
        if (GetMemContent(0x4b0ce0) & 0x10) {
            pCtx->Eip = 0x420aa4;
        } else if (GetMemContent(0x4b44e8, 0x74) && GetMemContent(0x4b4518, 0x1c, 0xa) & 1) {
            pCtx->Eip = 0x420ac3;
        }
    });
    EHOOK_ST(th12_all_clear_bonus_3, 0x420c6b, 4, {
        if (GetMemContent(0x4b0ce0) & 0x10) {
            pCtx->Eip = 0x420aa4;
        } else if (GetMemContent(0x4b44e8, 0x74) && GetMemContent(0x4b4518, 0x1c, 0xa) & 1) {
            pCtx->Eip = 0x420ac3;
        }
    });
    EHOOK_ST(th12_master_disable2, 0x40DF33,6,
    {
        *(DWORD*)(pCtx->Esi + 0x00018F9C) = 0;
    });

    float g_bossMoveDownRange = BOSS_MOVE_DOWN_RANGE_INIT;
    EHOOK_ST(th12_bossmovedown, 0x00417562, 5, {
        float* y_pos = (float*)(pCtx->Ebx + 0x15F8);
        float* y_range = (float*)(pCtx->Ebx + 0x1600);
        float y_max = (*y_pos) + (*y_range) * 0.5f;
        float y_min2 = y_max - (*y_range) * (1.0f - g_bossMoveDownRange);
        *y_pos = (y_max + y_min2) * 0.5f;
        *y_range = (y_max - y_min2);
    });

    class THAdvOptWnd : public Gui::PPGuiWnd {
        SINGLETON(THAdvOptWnd);    
    public:
        bool forceBossMoveDown = false;
    private:
        void MasterDisableInit()
        {
            th12_master_disable2.Setup();
            th12_master_disable2.Toggle(g_adv_igi_options.disable_master_autoly);
        }
        void FpsInit()
        {
            mOptCtx.vpatch_base = (int32_t)GetModuleHandleW(L"vpatch_th12.dll");
            if (mOptCtx.vpatch_base) {
                uint64_t hash[2];
                CalcFileHash(L"vpatch_th12.dll", hash);
                if (hash[0] != 666604866657820391ll || hash[1] != 18391463919001639953ll)
                    mOptCtx.fps_status = -1;
                else if (*(int32_t*)(mOptCtx.vpatch_base + 0x1b024) == 0) {
                    mOptCtx.fps_status = 2;
                    mOptCtx.fps = *(int32_t*)(mOptCtx.vpatch_base + 0x1b034);
                }
            } else if (*(uint8_t*)0x4cead3 == 3) {
                mOptCtx.fps_status = 1;

                DWORD oldProtect;
                VirtualProtect((void*)0x45044e, 4, PAGE_EXECUTE_READWRITE, &oldProtect);
                *(double**)0x45044e = &mOptCtx.fps_dbl;
                VirtualProtect((void*)0x45044e, 4, oldProtect, &oldProtect);
            } else
                mOptCtx.fps_status = 0;
        }
        void FpsSet()
        {
            if (mOptCtx.fps_status == 1) {
                mOptCtx.fps_dbl = 1.0 / (double)mOptCtx.fps;
            } else if (mOptCtx.fps_status == 2) {
                *(int32_t*)(mOptCtx.vpatch_base + 0x18abc) = mOptCtx.fps;
                *(int32_t*)(mOptCtx.vpatch_base + 0x1b034) = mOptCtx.fps;
            }
        }
        void GameplayInit()
        {
            th12_all_clear_bonus_1.Setup();
            th12_all_clear_bonus_2.Setup();
            th12_all_clear_bonus_3.Setup();
        }
        void GameplaySet()
        {
            th12_all_clear_bonus_1.Toggle(mOptCtx.all_clear_bonus);
            th12_all_clear_bonus_2.Toggle(mOptCtx.all_clear_bonus);
            th12_all_clear_bonus_3.Toggle(mOptCtx.all_clear_bonus);
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
            th12_bossmovedown.Setup();
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
            ImGui::TextUnformatted(S(TH_ADV_OPT));
            ImGui::Separator();
            ImGui::BeginChild("Adv. Options", ImVec2(0.0f, 0.0f));

            if (BeginOptGroup<TH_GAME_SPEED>()) {
                if (GameFPSOpt(mOptCtx))
                    FpsSet();
                EndOptGroup();
            }
            if (BeginOptGroup<TH_GAMEPLAY>()) {
                DisableKeyOpt();
                KeyHUDOpt();
                InfLifeOpt();
                if (ImGui::Checkbox(S(TH_BOSS_FORCE_MOVE_DOWN), &forceBossMoveDown)) {
                    th12_bossmovedown.Toggle(forceBossMoveDown);
                }
                ImGui::SameLine();
                HelpMarker(S(TH_BOSS_FORCE_MOVE_DOWN_DESC));
                ImGui::Checkbox(S(THPRAC_INGAMEINFO_12_UFO_SUPER_ULTRA_CHROMATIC_INFO_PRO), &g_adv_igi_options.th12_chromatic_ufo);
                ImGui::SetNextItemWidth(180.0f);
                if (ImGui::DragFloat(S(TH_BOSS_FORCE_MOVE_DOWN_RANGE), &g_bossMoveDownRange, 0.002f, 0.0f, 1.0f))
                    g_bossMoveDownRange = std::clamp(g_bossMoveDownRange, 0.0f, 1.0f);

                if (ImGui::Checkbox(S(TH_DISABLE_MASTER), &g_adv_igi_options.disable_master_autoly)) {
                    th12_master_disable2.Toggle(g_adv_igi_options.disable_master_autoly);
                }
                ImGui::SameLine();
                HelpMarker(S(TH_DISABLE_MASTER_DESC));
                if (GameplayOpt(mOptCtx))
                    GameplaySet();
                EndOptGroup();
            }
            SSS_UI();
            AboutOpt();
            ImGui::EndChild();
            ImGui::SetWindowFocus();
        }

        adv_opt_ctx mOptCtx;
    };

    void ECLJump(ECLHelper& ecl, unsigned int start, unsigned int ecl_time, int length)
    {
        ecl.SetPos(start);
        ecl << ecl_time << 0x0018000C << 0x02ff0000 << 0x00000000 << length << 0;
    }
    void ECLJumpEx(ECLHelper& ecl, unsigned int start, unsigned int dest, int at_frame, int ecl_time = 0)
    {
        ecl.SetPos(start);
        ecl << ecl_time << 0x0018000C << 0x02ff0000 << 0x00000000 << dest - start << at_frame;
    }
    void ECLSetHealth(ECLHelper& ecl, unsigned int start, unsigned int ecl_time, int health)
    {
        ecl.SetPos(start);
        ecl << ecl_time << 0x0014019b << 0x01ff0000 << 0x00000000 << health;
    }
    void ECLCallSub(ECLHelper& ecl, unsigned int start, unsigned int ecl_time, unsigned int sub_length, char* sub)
    {
        ecl.SetPos(start);
        ecl << ecl_time << 0x000b << sub_length + 0x14 << 0x01ff0000 << 0x00000000 << sub_length;
        for (unsigned int i = 0; i < sub_length; i++)
            ecl << (int8_t)sub[i];
    }
    __declspec(noinline) void THStageWarp(ECLHelper& ecl, int stage, int portion)
    {
        if (stage == 1) {
            switch (portion) {
            case 1:
                break;
            case 2:
                ecl << pair{0x12f6c, (int16_t)0} << pair{0x12fb8, (int16_t)0};
                ECLJumpEx(ecl, 0x133d4, 0x1346c, 60, 90);
                break;
            case 3:
                ecl << pair{0x12f6c, (int16_t)0} << pair{0x12fb8, (int16_t)0};
                ECLJumpEx(ecl, 0x133d4, 0x13548, 60, 90);
                break;
            case 4:
                ecl << pair{0x12f6c, (int16_t)0} << pair{0x12fb8, (int16_t)0};
                ECLJumpEx(ecl, 0x133d4, 0x1369c, 60, 90);
                break;
            case 5:
                ecl << pair{0x12f6c, (int16_t)0} << pair{0x12fb8, (int16_t)0};
                ECLJumpEx(ecl, 0x133d4, 0x136ec, 60, 90);
                break;
            default:
                break;
            }
        } else if (stage == 2) {
            switch (portion) {
            case 1:
                break;
            case 2:
                ECLJumpEx(ecl, 0x13108, 0x131b4, 60, 90);
                break;
            case 3:
                ECLJumpEx(ecl, 0x13108, 0x1323c, 60, 90);
                break;
            case 4:
                ECLJumpEx(ecl, 0x13108, 0x132e8, 60, 90);
                break;
            case 5:
                ECLJumpEx(ecl, 0x13108, 0x13360, 60, 90);
                break;
            default:
                break;
            }
        } else if (stage == 3) {
            switch (portion) {
            case 1:
                break;
            case 2:
                ECLJumpEx(ecl, 0x16864, 0x168dc, 60, 90);
                break;
            case 3:
                ECLJumpEx(ecl, 0x16864, 0x16910, 60, 90);
                break;
            case 4:
                ECLJumpEx(ecl, 0x16864, 0x16944, 60, 90);
                break;
            case 5:
                ECLJumpEx(ecl, 0x16864, 0x169ac, 60);
                ECLJumpEx(ecl, 0x10548, 0x1226c, 0);
                ECLJumpEx(ecl, 0x1227c, 0x1241c, 0);
                ecl << pair{0x12470, (int16_t)0} << pair{0x124dc, (int16_t)0};
                ecl.SetPos(0x12654);
                ecl << 0 << 0x001001a3 << 0x00ff0000 << 0
                    << 0 << 0x0014019c << 0x01ff0000 << 0 << -1
                    << 0 << 0x00100001 << 0x00ff0000 << 0;
                break;
            case 6:
                ECLJumpEx(ecl, 0x16864, 0x16a30, 60, 90);
                break;
            case 7:
                ECLJumpEx(ecl, 0x16864, 0x16a64, 60, 90);
                break;
            default:
                break;
            }
        } else if (stage == 4) {
            switch (portion) {
            case 1:
                break;
            case 2:
                ECLJumpEx(ecl, 0x13800, 0x13868, 0, 90);
                break;
            case 3:
                ECLJumpEx(ecl, 0x13800, 0x1389c, 0, 90);
                break;
            case 4:
                ECLJumpEx(ecl, 0x13800, 0x138d0, 0, 0);
                break;
            case 5:
                ECLJumpEx(ecl, 0x13800, 0x13948, 0, 90);
                break;
            case 6:
                ECLJumpEx(ecl, 0x13800, 0x1397c, 0, 90);
                break;
            case 7:
                ECLJumpEx(ecl, 0x13800, 0x139b0, 0, 90);
                break;
            case 8:
                if (thPracParam.phase == 1) {
                    ECLJumpEx(ecl, 0x12B10, 0x12110, 0, 0);
                }
                ECLJumpEx(ecl, 0x13800, 0x139e4, 0, 90);
                break;
            default:
                break;
            }
        } else if (stage == 5) {
            switch (portion) {
            case 1:
                break;
            case 2:
                ECLJumpEx(ecl, 0x158c0, 0x15938, 60, 90);
                break;
            case 3:
                ECLJumpEx(ecl, 0x158c0, 0x1596c, 60, 90);
                break;
            case 4:
                ECLJumpEx(ecl, 0x158c0, 0x159a0, 60, 90);
                break;
            case 5:
                ECLJumpEx(ecl, 0x158c0, 0x15a3c, 181, 90);
                break;
            case 6:
                if (thPracParam.phase == 1) {
                    ECLJumpEx(ecl, 0x14214, 0x134F4, 0, 0);
                    ECLJumpEx(ecl, 0x1224C, 0x134F4, 0, 0);
                    ECLJumpEx(ecl, 0x158c0, 0x15a80, 181, 0);
                } else {
                    ECLJumpEx(ecl, 0x158c0, 0x15a80, 181, 90);
                }
                break;
            case 7:
                ECLJumpEx(ecl, 0x158c0, 0x15ab4, 181, 90);
                break;
            default:
                break;
            }
        } else if (stage == 6) {
            switch (portion) {
            case 1:
                break;
            case 2:
                ECLJumpEx(ecl, 0x1a3c0, 0x1a438, 60, 90);
                break;
            case 3:
                ECLJumpEx(ecl, 0x1a3c0, 0x1a46c, 60, 90);
                break;
            default:
                break;
            }
        } else if (stage == 7) {
            switch (portion) {
            case 1:
                break;
            case 2:
                ECLJumpEx(ecl, 0xdb14, 0xdb8c, 60, 90);
                break;
            case 3:
                ECLJumpEx(ecl, 0xdb14, 0xdbc0, 60, 90);
                break;
            case 4:
                ECLJumpEx(ecl, 0xdb14, 0xdbf4, 60, 90);
                break;
            case 5:
                ECLJumpEx(ecl, 0xdb14, 0xdc8c, 60, 90);
                break;
            case 6:
                ECLJumpEx(ecl, 0xdb14, 0xdcc0, 60, 90);
                break;
            case 7:
                ECLJumpEx(ecl, 0xdb14, 0xdcf4, 60, 90);
                break;
            case 8:
                ECLJumpEx(ecl, 0xdb14, 0xdd28, 60, 90);
                break;
            default:
                break;
            }
        }
    }
    __declspec(noinline) void THPatch(ECLHelper& ecl, th_sections_t section)
    {
        auto nue = [&]() {
            ECLJumpEx(ecl, 0xdb14, 0xdd80, 71);
            ecl.SetFile(2);
            ecl << pair{0x774, 60} << pair{0x7e0, 60}; // Invincible Frame & Wait Time
            ecl << pair{0x790, (int16_t)0x0}; // Skip Teleportation
            ECLJumpEx(ecl, 0x7fc, 0x8b8, 2); // Skip Animation
            ecl.SetPos(0x6f0);
            ecl << 0 << 0x00300106 << 0x02ff0000 << 0 << 0 << 0; // Change Sprite
            ecl.SetPos(0x7a4);
            ecl << 0 << 0x002c012d << 0x04ff0000 << 0 << 60 << 4 << 0 << 0x42c00000; // Move 301
        };

        switch (section) {
        case THPrac::TH12::TH12_ST1_MID1:
            ECLJumpEx(ecl, 0x133b0, 0x13658, 59);
            break;
        case THPrac::TH12::TH12_ST1_MID2:
            ECLJumpEx(ecl, 0x133b0, 0x13658, 59);
            ecl << pair{0x0c820, 0x0fa0};
            ecl << pair{0x0c8bc, 0x14b4};
            ECLJumpEx(ecl, 0xcde8, 0xce40, 0);
            ecl << pair{0x0ccfc, (int16_t)0x0};
            ecl << pair{0x0ce74, 60};
            break;
        case THPrac::TH12::TH12_ST1_MID3:
            ECLJumpEx(ecl, 0x133b0, 0x13658, 59);
            ecl << pair{0x0c820, 0x0fa0};
            ecl << pair{0x0c8bc, 0x14b4};
            ECLJumpEx(ecl, 0xcde8, 0xce40, 0);
            ecl << pair{0x0ccfc, (int16_t)0x0};
            ecl << pair{0x0ce74, 60};

            ECLSetHealth(ecl, 0x0ce74, 60, 0x514);
            ecl << pair{0x0ce7a, (int16_t)0x20};
            ecl << pair{0xce94, 9999};
            break;
        case THPrac::TH12::TH12_ST1_BOSS1:
            if (thPracParam.dlg)
                ECLJumpEx(ecl, 0x133b0, 0x13788, 59);
            else
                ECLJumpEx(ecl, 0x133b0, 0x137ac, 59);
            break;
        case THPrac::TH12::TH12_ST1_BOSS2:
            ECLJumpEx(ecl, 0x133b0, 0x137ac, 59);
            ecl << pair{0x0ae0, 0x5dc};
            ecl << pair{0xcec, 60};
            break;
        case THPrac::TH12::TH12_ST1_BOSS3:
            ECLJumpEx(ecl, 0x133b0, 0x137ac, 59);
            ecl << pair{0x0be4, (int8_t)0x32};
            ECLJumpEx(ecl, 0x190c, 0x1978, 0);
            ecl << pair{0x17e4, (int16_t)0x0} << pair{0x197c, (int16_t)0x0};
            ecl.SetPos(0xb4c);
            ecl << 0.0f << 128.0f << 280.0f << 256.0f;
            break;
        case THPrac::TH12::TH12_ST1_BOSS4:
            ECLJumpEx(ecl, 0x133b0, 0x137ac, 59);
            ecl << pair{0x0be4, (int8_t)0x32};
            ECLJumpEx(ecl, 0x190c, 0x1978, 0);
            ecl << pair{0x17e4, (int16_t)0x0};
            ecl << pair{0x162c, 0x4b0};
            ecl << pair{0x1640, 0x3e8};
            ecl.SetPos(0xb4c);
            ecl << 0.0f << 128.0f << 280.0f << 256.0f;
            break;
        case THPrac::TH12::TH12_ST2_MID1:
            ECLJumpEx(ecl, 0x13108, 0x132a4, 59);
            break;
        case THPrac::TH12::TH12_ST2_MID2:
            ECLJumpEx(ecl, 0x13108, 0x132a4, 59);
            ecl.SetPos(0x09ea4);
            ecl << 0 << 0x0014019f << 0x01ff0000 << 0 << 60;
            ECLSetHealth(ecl, 0x09ed8, 60, 1350);
            ecl << pair{0x9eb8, 60};
            break;
        case THPrac::TH12::TH12_ST2_BOSS1:
            if (thPracParam.dlg)
                ECLJumpEx(ecl, 0x13108, 0x134d8, 59);
            else
                ECLJumpEx(ecl, 0x13108, 0x134fc, 59);
            break;
        case THPrac::TH12::TH12_ST2_BOSS2:
            ECLJumpEx(ecl, 0x13108, 0x134fc, 59);
            ecl << pair{0x0d18, 0x44c};
            ecl << pair{0xeac, 60};
            break;
        case THPrac::TH12::TH12_ST2_BOSS3:
            ECLJumpEx(ecl, 0x13108, 0x134fc, 59);
            ecl << pair{0x0df4, (int8_t)0x32};
            ECLJumpEx(ecl, 0x1268, 0x12d4, 0);
            ecl << pair{0x1188, (int16_t)0x0} << pair{0x12d8, (int16_t)0x0};
            break;
        case THPrac::TH12::TH12_ST2_BOSS4:
            ECLJumpEx(ecl, 0x13108, 0x134fc, 59);
            ecl << pair{0x0df4, (int8_t)0x32};
            ECLJumpEx(ecl, 0x1268, 0x12d4, 0);
            ecl << pair{0x1188, (int16_t)0x0};
            ecl << pair{0x0fe4, 0x6a4};
            break;
        case THPrac::TH12::TH12_ST2_BOSS5:
            ECLJumpEx(ecl, 0x13108, 0x134fc, 59);
            ecl << pair{0xdec, 0xc} << pair(0xdf4, 'draC') << pair{0xdf8, 0x00000033};
            ECLJumpEx(ecl, 0x32e8, 0x3340, 0);
            ECLJumpEx(ecl, 0x34dc, 0x3518, 0);
            ecl << pair{0x35b4, (int16_t)0x0};
            break;
        case THPrac::TH12::TH12_ST3_MID1:
            ECLJumpEx(ecl, 0x16864, 0x169ac, 59);
            break;
        case THPrac::TH12::TH12_ST3_MID2:
            ECLJumpEx(ecl, 0x16864, 0x169ac, 59);
            ecl << pair{0x1047c, 0x6a0};
            ECLJumpEx(ecl, 0x10930, 0x10988, 0);
            ECLJumpEx(ecl, 0x1099c, 0x109c0, 0);
            ecl << pair{0x10998, 60};
            ecl << pair{0x108a0, 60};
            ecl << pair{0x108b4, (int16_t)0x0};
            ecl << pair{0x1086c, (int16_t)0x0} << pair{0x10a5c, (int16_t)0x0};
            break;
        case THPrac::TH12::TH12_ST3_BOSS1:
            if (thPracParam.dlg)
                ECLJumpEx(ecl, 0x16864, 0x16a98, 59);
            else
                ECLJumpEx(ecl, 0x16864, 0x16abc, 59);
            break;
        case THPrac::TH12::TH12_ST3_BOSS2:
            ECLJumpEx(ecl, 0x16864, 0x16abc, 59);
            ecl << pair{0x0bec, 0x6a4};
            ecl << pair{0xdac, 60};
            break;
        case THPrac::TH12::TH12_ST3_BOSS3:
            ECLJumpEx(ecl, 0x16864, 0x16abc, 59);
            ecl << pair{0x0cc8, (int8_t)0x32};
            ECLJumpEx(ecl, 0x27f0, 0x285c, 0);
            ecl << pair{0x2710, (int16_t)0x0} << pair{0x28a0, (int16_t)0x0};
            break;
        case THPrac::TH12::TH12_ST3_BOSS4:
            ECLJumpEx(ecl, 0x16864, 0x16abc, 59);
            ecl << pair{0x0cc8, (int8_t)0x32};
            ECLJumpEx(ecl, 0x27f0, 0x285c, 0);
            ecl << pair{0x2710, (int16_t)0x0};
            ecl << pair{0x256c, 0x7d0};
            break;
        case THPrac::TH12::TH12_ST3_BOSS5:
            ECLJumpEx(ecl, 0x16864, 0x16abc, 59);
            ecl << pair{0x0cc8, (int8_t)0x33};
            ECLJumpEx(ecl, 0x40b0, 0x411c, 0);
            ecl << pair{0x3fd0, (int16_t)0x0} << pair{0x4140, (int16_t)0x0};
            break;
        case THPrac::TH12::TH12_ST3_BOSS6:
            ECLJumpEx(ecl, 0x16864, 0x16abc, 59);
            ecl << pair{0x0cc8, (int8_t)0x33};
            ECLJumpEx(ecl, 0x40b0, 0x411c, 0);
            ecl << pair{0x3fd0, (int16_t)0x0};
            ecl << pair{0x3e2c, 0x9c4};
            break;
        case THPrac::TH12::TH12_ST4_MID1:
            ECLJumpEx(ecl, 0x13800, 0x13904, 0);
            break;
        case THPrac::TH12::TH12_ST4_BOSS1:
            if (thPracParam.dlg)
                ECLJumpEx(ecl, 0x13800, 0x13a4c, 119);
            else
                ECLJumpEx(ecl, 0x13800, 0x13a70, 119);
            break;
        case THPrac::TH12::TH12_ST4_BOSS2:
            ECLJumpEx(ecl, 0x13800, 0x13a70, 119);
            ecl << pair{0x34d4, 0x708};
            ecl << pair{0x3694, 60};
            break;
        case THPrac::TH12::TH12_ST4_BOSS3:
            ECLJumpEx(ecl, 0x13800, 0x13a70, 119);
            ecl << pair{0x35b0, (int8_t)0x32};
            ECLJumpEx(ecl, 0x4020, 0x408c, 0);
            ecl << pair{0x3f40, (int16_t)0x0} << pair{0x40d0, (int16_t)0x0};
            break;
        case THPrac::TH12::TH12_ST4_BOSS4:
            ECLJumpEx(ecl, 0x13800, 0x13a70, 119);
            ecl << pair{0x35b0, (int8_t)0x32};
            ECLJumpEx(ecl, 0x4020, 0x408c, 0);
            ecl << pair{0x3f40, (int16_t)0x0};
            ecl << pair{0x3d9c, 0x834};
            break;
        case THPrac::TH12::TH12_ST4_BOSS5:
            ECLJumpEx(ecl, 0x13800, 0x13a70, 119);
            ecl << pair{0x35b0, (int8_t)0x33};
            ECLJumpEx(ecl, 0x5178, 0x51e4, 0);
            ecl << pair{0x507c, (int16_t)0x0} << pair{0x5228, (int16_t)0x0};
            break;
        case THPrac::TH12::TH12_ST4_BOSS6:
            ECLJumpEx(ecl, 0x13800, 0x13a70, 119);
            ecl << pair{0x35b0, (int8_t)0x33};
            ECLJumpEx(ecl, 0x5178, 0x51e4, 0);
            ecl << pair{0x507c, (int16_t)0x0};
            ecl << pair{0x4ec4, 0x835};
            ecl << pair{0x4ed8, 0x835};
            break;
        case THPrac::TH12::TH12_ST4_BOSS7:
            ECLJumpEx(ecl, 0x13800, 0x13a70, 119);
            ecl << pair{0x35a8, 0xc} << pair(0x35b0, 'draC') << pair{0x35b4, 0x00000034};
            ECLJumpEx(ecl, 0x86c4, 0x8730, 0);
            ECLJumpEx(ecl, 0x8798, 0x87d8, 0);
            ecl << pair{0x8760, 20};
            ecl << pair{0x8614, (int16_t)0x0} << pair{0x8774, (int16_t)0x0};
            break;
        case THPrac::TH12::TH12_ST5_MID1:
            if (thPracParam.dlg)
                ECLJumpEx(ecl, 0x158c0, 0x159d4, 179);
            else {
                ECLJumpEx(ecl, 0x158c0, 0x159e8, 179);
                ecl << pair{0xd6ac, (int16_t)0x0} << pair{0xd8b8, (int16_t)0x0};
                ecl << pair{0xd6f0, 60};
            }
            break;
        case THPrac::TH12::TH12_ST5_MID2:
            ECLJumpEx(ecl, 0x158c0, 0x159e8, 179);
            ecl << pair{0xd6ac, (int16_t)0x0};
            ecl << pair{0xd6f0, 60};
            ecl << pair{0x0d7d4, 0x7d0};
            switch (thPracParam.phase) {
            default:
            case 0:
                break;
            case 1:
                ecl << pair { 0xdd54, (int16_t)442 };
                break;
            }
            break;
        case THPrac::TH12::TH12_ST5_BOSS1:
            if (thPracParam.dlg)
                ECLJumpEx(ecl, 0x158c0, 0x15ae8, 190);
            else
                ECLJumpEx(ecl, 0x158c0, 0x15b0c, 190);
            break;
        case THPrac::TH12::TH12_ST5_BOSS2:
            ECLJumpEx(ecl, 0x158c0, 0x15b0c, 190);
            ecl << pair{0x0b00, 0x7d0};
            ecl << pair{0xcec, 60};
            break;
        case THPrac::TH12::TH12_ST5_BOSS3:
            ECLJumpEx(ecl, 0x158c0, 0x15b0c, 190);
            ecl << pair{0x0bf0, (int8_t)0x32};
            ECLJumpEx(ecl, 0x1fbc, 0x2028, 0);
            ecl << pair{0x1edc, (int16_t)0x0} << pair{0x206c, (int16_t)0x0};
            ecl << pair{0xb20, 0x43000000};

            ecl << pair(0x1f04, 2640 - 120);
            break;
        case THPrac::TH12::TH12_ST5_BOSS4:
            ECLJumpEx(ecl, 0x158c0, 0x15b0c, 190);
            ecl << pair{0x0bf0, (int8_t)0x32};
            ECLJumpEx(ecl, 0x1fbc, 0x2028, 0);
            ecl << pair{0x1edc, (int16_t)0x0};
            ecl << pair{0x1d38, 0x7d0};
            break;
        case THPrac::TH12::TH12_ST5_BOSS5:
            ECLJumpEx(ecl, 0x158c0, 0x15b0c, 190);
            ecl << pair{0x0bf0, (int8_t)0x33};
            ECLJumpEx(ecl, 0x35c0, 0x362c, 0);
            ecl << pair{0x3498, (int16_t)0x0} << pair{0x3670, (int16_t)0x0};
            ecl << pair{0xb20, 0x43000000};

            ecl << pair(0x34c0, 2400 - 90) << pair(0x34ec, 2400 - 90);
            break;
        case THPrac::TH12::TH12_ST5_BOSS6:
            ECLJumpEx(ecl, 0x158c0, 0x15b0c, 190);
            ecl << pair{0x0bf0, (int8_t)0x33};
            ECLJumpEx(ecl, 0x35c0, 0x362c, 0);
            ecl << pair{0x3498, (int16_t)0x0};
            ecl << pair{0x32e0, 0x76c};
            ecl << pair{0x32f4, 0x514};
            break;
        case THPrac::TH12::TH12_ST5_BOSS7:
            ECLJumpEx(ecl, 0x158c0, 0x15b0c, 190);
            ecl << pair{0x0bf0, (int8_t)0x34};
            ECLJumpEx(ecl, 0x412c, 0x4198, 0);
            ecl << pair{0x4078, (int16_t)0x0} << pair{0x41bc, (int16_t)0x0};
            break;
        case THPrac::TH12::TH12_ST6_MID1:
            ECLJumpEx(ecl, 0x1a3c0, 0x1a4a0, 60);
            break;
        case THPrac::TH12::TH12_ST6_BOSS1:
            if (thPracParam.dlg)
                ECLJumpEx(ecl, 0x1a3c0, 0x1a4e4, 59);
            else
                ECLJumpEx(ecl, 0x1a3c0, 0x1a508, 59);
            break;
        case THPrac::TH12::TH12_ST6_BOSS2:
            ECLJumpEx(ecl, 0x1a3c0, 0x1a508, 59);
            ecl << pair{0x968, 0x898};
            ecl << pair{0xb10, 60};
            break;
        case THPrac::TH12::TH12_ST6_BOSS3:
            ECLJumpEx(ecl, 0x1a3c0, 0x1a508, 59);
            ecl << pair{0x0a44, (int8_t)0x32};
            ECLJumpEx(ecl, 0x29f0, 0x2a5c, 0);
            ecl << pair{0x2910, (int16_t)0x0};
            ecl << pair{0x298c, 60};
            ECLJumpEx(ecl, 0x2a9c, 0x2af0, 0);
            break;
        case THPrac::TH12::TH12_ST6_BOSS4:
            ECLJumpEx(ecl, 0x1a3c0, 0x1a508, 59);
            ecl << pair{0x0a44, (int8_t)0x32};
            ECLJumpEx(ecl, 0x29f0, 0x2a5c, 0);
            ecl << pair{0x2910, (int16_t)0x0};
            ecl << pair{0x298c, 60};
            ECLJumpEx(ecl, 0x2a9c, 0x2b04, 0); // Changed

            ECLSetHealth(ecl, 0x2d04, 0x0, 0x960);
            ecl << pair{0x2d18, 9999};
            break;
        case THPrac::TH12::TH12_ST6_BOSS5:
            ECLJumpEx(ecl, 0x1a3c0, 0x1a508, 59);
            ecl << pair{0x0a44, (int8_t)0x33};
            ECLJumpEx(ecl, 0x42e0, 0x4798, 0);
            ecl << pair{0x41e8, (int16_t)0x0} << pair{0x420c, (int16_t)0x0};
            ecl << pair{0x428c, 0};
            break;
        case THPrac::TH12::TH12_ST6_BOSS6:
            ECLJumpEx(ecl, 0x1a3c0, 0x1a508, 59);
            ecl << pair{0x0a44, (int8_t)0x33};
            ECLJumpEx(ecl, 0x42e0, 0x47ac, 0); // Changed
            ecl << pair{0x41e8, (int16_t)0x0} << pair{0x420c, (int16_t)0x0};
            ecl << pair{0x428c, 0};

            ecl << pair{0x3f54, 0x9c4};
            break;
        case THPrac::TH12::TH12_ST6_BOSS7:
            ECLJumpEx(ecl, 0x1a3c0, 0x1a508, 59);
            ecl << pair{0x0a44, (int8_t)0x34};
            ECLJumpEx(ecl, 0x6378, 0x6830, 0);
            ecl << pair{0x6280, (int16_t)0x0} << pair{0x62a4, (int16_t)0x0};
            ecl << pair{0x6324, 0};
            break;
        case THPrac::TH12::TH12_ST6_BOSS8:
            ECLJumpEx(ecl, 0x1a3c0, 0x1a508, 59);
            ecl << pair{0x0a44, (int8_t)0x34};
            ECLJumpEx(ecl, 0x6378, 0x6844, 0); // Changed
            ecl << pair{0x6280, (int16_t)0x0} << pair{0x62a4, (int16_t)0x0};
            ecl << pair{0x6324, 0};

            ecl << pair{0x5fd8, 0x1388};

            switch (thPracParam.phase) {
            default:
            case 0:
                break;
            case 1:
                ECLJumpEx(ecl, 0xd264, 0xd294, 120);
                ecl << pair { 0xd508, (int16_t)0x0 };

                ecl << pair { 0x0ec0c, 0x0 };
                ecl << pair { 0x10188, 0x0 };
                break;
            case 2:
                ECLJumpEx(ecl, 0xd264, 0xd294, 120);
                ecl << pair { 0xd508, (int16_t)0x0 };

                ecl << pair { 0x0ec0c, 0x0 };
                ecl << pair { 0x0f468, 0x0 };
                ecl << pair { 0x10188, 0x0 };
                break;
            case 3:
                ECLJumpEx(ecl, 0xd264, 0xd294, 120);
                ecl << pair { 0xd508, (int16_t)0x0 };

                ecl << pair { 0x0ec0c, 0x0 };
                ecl << pair { 0x0f468, 0x0 };
                ecl << pair { 0x0f650, 0x3d };
                ecl << pair { 0x10188, 0x0 };
                break;
            case 4:
                ECLJumpEx(ecl, 0xd264, 0xd294, 120);
                ecl << pair { 0xd508, (int16_t)0x0 };
                ecl << pair { 0xD620, (int16_t)0x0 };

                ecl << pair { 0x0ec0c, 0x0 };
                ecl << pair { 0x0f468, 0x0 };
                ecl << pair { 0x0f650, 0x3d };
                ecl << pair { 0x0fa90, 0x0 };
                ecl << pair { 0x10188, 0x0 };
                break;
            case 5:
                ECLJumpEx(ecl, 0xd264, 0xd294, 120);
                ecl << pair{0xd508, (int16_t)0x0};

                ecl << pair{0x0ec0c, 0x0};
                ecl << pair{0x0f468, 0x0};
                ecl << pair{0x0f650, 0x3d};
                ecl << pair{0x0fa90, 0x0};
                ecl << pair{0x0ffdc, 99999999};
                ecl << pair{0x10188, 0x0};
                break;
            case 6:
                ECLJumpEx(ecl, 0xd264, 0xd294, 120);
                ecl << pair { 0xd508, (int16_t)0x0 };

                ecl << pair { 0x0ec0c, 0x0 };
                ecl << pair { 0x0f468, 0x0 };
                ecl << pair { 0x0f650, 0x3d };
                ecl << pair { 0x0fa90, 0x0 };
                ecl << pair { 0x0ffdc, 0x0 };
                ecl << pair { 0x10188, 0x0 };
                break;
            }
            break;
        case THPrac::TH12::TH12_ST6_BOSS9:
            ECLJumpEx(ecl, 0x1a3c0, 0x1a508, 59);
            ecl << pair{0x0a44, (int8_t)0x35};
            ECLJumpEx(ecl, 0x9030, 0x9508, 0);
            ecl << pair{0x8f44, (int16_t)0x0};
            ecl << pair{0x8fdc, 10};
            break;
        case THPrac::TH12::TH12_ST6_BOSS10:
            ECLJumpEx(ecl, 0x1a3c0, 0x1a508, 59);
            ecl << pair{0x0a44, (int8_t)0x36};
            ECLJumpEx(ecl, 0x98f8, 0x99ac, 0);
            ecl << pair{0x9834, (int16_t)0x0};
            ecl << pair{0x9894, 10};

            if (thPracParam.phase == 1) {
                ecl << pair{0x12a28, 0x118c};
            }
            break;
        case THPrac::TH12::TH12_ST7_MID1:
            if (thPracParam.dlg)
                ECLJumpEx(ecl, 0xdb14, 0xdc28, 59);
            else
                ECLJumpEx(ecl, 0xdb14, 0xdc3c, 59);
            break;
        case THPrac::TH12::TH12_ST7_MID2:
            ECLJumpEx(ecl, 0xdb14, 0xdc3c, 59);
            ecl << pair{0x4099, (int8_t)0x32};
            ECLJumpEx(ecl, 0x52a0, 0x530c, 0);
            ECLJumpEx(ecl, 0x5374, 0x53b4, 0);
            ecl << pair{0x5194, (int16_t)0x0} << pair{0x5350, (int16_t)0x0};
            break;
        case THPrac::TH12::TH12_ST7_MID3:
            ECLJumpEx(ecl, 0xdb14, 0xdc3c, 59);
            ecl << pair{0x4099, (int8_t)0x33};
            ECLJumpEx(ecl, 0x5fd4, 0x6040, 0);
            ECLJumpEx(ecl, 0x60a8, 0x60e8, 0);
            ecl << pair{0x5ec8, (int16_t)0x0} << pair{0x6084, (int16_t)0x0};
            break;
        case THPrac::TH12::TH12_ST7_END_NS1:
            if (thPracParam.dlg)
                ECLJumpEx(ecl, 0xdb14, 0xdd5c, 71);
            else
                nue();
            break;
        case THPrac::TH12::TH12_ST7_END_S1:
            nue();
            ecl << pair{0x788, 0x8fc};
            ecl << pair{0xab0, 9999};
            break;
        case THPrac::TH12::TH12_ST7_END_NS2:
            nue();
            ecl << pair{0x98c, (int8_t)0x32};
            ECLJumpEx(ecl, 0x185c, 0x18dc, 0);
            ecl << pair{0x1768, (int16_t)0x0} << pair{0x1920, (int16_t)0x0};
            ecl << pair{0x7bc, 0} << pair{0x7c0, 0x43000000};
            break;
        case THPrac::TH12::TH12_ST7_END_S2:
            nue();
            ecl << pair{0x98c, (int8_t)0x32};
            ECLJumpEx(ecl, 0x185c, 0x18dc, 0);
            ecl << pair{0x1768, (int16_t)0x0};
            ecl << pair{0x7bc, 0} << pair{0x7c0, 0x43000000};
            ecl << pair{0x15c4, 0x514};
            break;
        case THPrac::TH12::TH12_ST7_END_NS3:
            nue();
            ecl << pair{0x98c, (int8_t)0x33};
            ECLJumpEx(ecl, 0x2240, 0x22c0, 0);
            ecl << pair{0x2160, (int16_t)0x0} << pair{0x2304, (int16_t)0x0};
            ecl << pair{0x7bc, 0} << pair{0x7c0, 0x43000000};
            break;
        case THPrac::TH12::TH12_ST7_END_S3:
            nue();
            ecl << pair{0x98c, (int8_t)0x33};
            ECLJumpEx(ecl, 0x2240, 0x22c0, 0);
            ecl << pair{0x2160, (int16_t)0x0};
            ecl << pair{0x7bc, 0} << pair{0x7c0, 0x43000000};
            ecl << pair{0x1fbc, 0x0a8c};
            break;
        case THPrac::TH12::TH12_ST7_END_NS4:
            nue();
            ecl << pair{0x98c, (int8_t)0x34};
            ECLJumpEx(ecl, 0x2dcc, 0x2e4c, 0);
            ecl << pair{0x2cd8, (int16_t)0x0} << pair{0x2e90, (int16_t)0x0};
            ecl << pair{0x7bc, 0} << pair{0x7c0, 0x43000000};
            break;
        case THPrac::TH12::TH12_ST7_END_S4:
            nue();
            ecl << pair{0x98c, (int8_t)0x34};
            ECLJumpEx(ecl, 0x2dcc, 0x2e4c, 0);
            ecl << pair{0x2cd8, (int16_t)0x0};
            ecl << pair{0x7bc, 0} << pair{0x7c0, 0x43000000};
            ecl << pair{0x2b34, 0x6A4};
            break;
        case THPrac::TH12::TH12_ST7_END_NS5:
            nue();
            ecl << pair{0x98c, (int8_t)0x35};
            ECLJumpEx(ecl, 0x3a2c, 0x3aac, 0);
            ecl << pair{0x3938, (int16_t)0x0} << pair{0x3af0, (int16_t)0x0};
            ecl << pair{0x7bc, 0} << pair{0x7c0, 0x43000000};
            break;
        case THPrac::TH12::TH12_ST7_END_S5:
            nue();
            ecl << pair{0x98c, (int8_t)0x35};
            ECLJumpEx(ecl, 0x3a2c, 0x3aac, 0);
            ecl << pair{0x3938, (int16_t)0x0};
            ecl << pair{0x7bc, 0} << pair{0x7c0, 0x43000000};
            ecl << pair{0x3794, 0x0e10};
            break;
        case THPrac::TH12::TH12_ST7_END_NS6:
            nue();
            ecl << pair{0x98c, (int8_t)0x36};
            ECLJumpEx(ecl, 0x44cc, 0x454c, 0);
            ecl << pair{0x43d8, (int16_t)0x0} << pair{0x4590, (int16_t)0x0};
            ecl << pair{0x7bc, 0} << pair{0x7c0, 0x43000000};
            break;
        case THPrac::TH12::TH12_ST7_END_S6:
            nue();
            ecl << pair{0x98c, (int8_t)0x36};
            ECLJumpEx(ecl, 0x44cc, 0x454c, 0);
            ecl << pair{0x43d8, (int16_t)0x0};
            ecl << pair{0x7bc, 0} << pair{0x7c0, 0x43000000};
            ecl << pair{0x4234, 0x0a8c};
            break;
        case THPrac::TH12::TH12_ST7_END_NS7:
            nue();
            ecl << pair{0x98c, (int8_t)0x37};
            ECLJumpEx(ecl, 0x51e8, 0x5268, 0);
            ecl << pair{0x50f4, (int16_t)0x0} << pair{0x52ac, (int16_t)0x0};
            ecl << pair{0x7bc, 0} << pair{0x7c0, 0x43000000};
            break;
        case THPrac::TH12::TH12_ST7_END_S7:
            nue();
            ecl << pair{0x98c, (int8_t)0x37};
            ECLJumpEx(ecl, 0x51e8, 0x5268, 0);
            ecl << pair{0x50f4, (int16_t)0x0};
            ecl << pair{0x7bc, 0} << pair{0x7c0, 0x43000000};
            ecl << pair{0x4f50, 0x898};
            break;
        case THPrac::TH12::TH12_ST7_END_NS8:
            nue();
            ecl << pair{0x98c, (int8_t)0x38};
            ECLJumpEx(ecl, 0x5f04, 0x5f84, 0);
            ecl << pair{0x5e10, (int16_t)0x0} << pair{0x5fa8, (int16_t)0x0};
            ecl << pair{0x7bc, 0} << pair{0x7c0, 0x43000000};
            break;
        case THPrac::TH12::TH12_ST7_END_S8:
            nue();
            ecl << pair{0x98c, (int8_t)0x38};
            ECLJumpEx(ecl, 0x5f04, 0x5f84, 0);
            ecl << pair{0x5e10, (int16_t)0x0};
            ecl << pair{0x7bc, 0} << pair{0x7c0, 0x43000000};
            ecl << pair{0x5c6c, 0x898};
            break;
        case THPrac::TH12::TH12_ST7_END_S9:
            nue();
            ecl << pair{0x98c, (int8_t)0x39};
            ECLJumpEx(ecl, 0x71dc, 0x725c, 0);
            ecl << pair{0x7114, (int16_t)0x0} << pair{0x72a0, (int16_t)0x0};
            ecl << pair{0x7bc, 0} << pair{0x7c0, 0x43000000};
            break;
        case THPrac::TH12::TH12_ST7_END_S10:
            nue();
            ecl << pair{0x98c, (int8_t)0x31} << pair{0x98d, (int8_t)0x30};
            ECLJumpEx(ecl, 0xfc0, 0x106c, 0);
            ecl << pair{0xe6c, (int16_t)0x0} << pair{0xed8, (int16_t)0x0} << pair{0xeec, (int16_t)0x0};
            ecl << pair{0x7bc, 0} << pair{0x7c0, 0x43000000};

            switch (thPracParam.phase) {
                case 1:
                    ecl << pair{0x7a74, 180};
                    ECLSetHealth(ecl, 0x7b00, 0, 4600);
                    ECLJumpEx(ecl, 0x7b14, 0x7e8c, 0);
                    break;
                case 2:
                    ecl << pair{0x7a74, 180};
                    ECLSetHealth(ecl, 0x7b00, 0, 3000);
                    ECLJumpEx(ecl, 0x7b14, 0x823c, 0);
                    break;
                case 3:
                    ecl << pair{0x7a74, 180};
                    ECLSetHealth(ecl, 0x7b00, 0, 1150);
                    ECLJumpEx(ecl, 0x7b14, 0x8610, 0);
                    break;
                case 4:
                    ecl << pair(0x7b48, 0x0e1c); // EXTRA
                    ecl << pair(0x8958, (int16_t)0x15); // EXTRA
                    ecl << pair(0x895e, (int16_t)0x0ff); // EXTRA
            }
            break;
        default:
            break;
        }
    }
    __declspec(noinline) void THSectionPatch()
    {
        ECLHelper ecl;
        ecl.SetBaseAddr((void*)GetMemAddr(0x4b43dc, 0x64, 0xC));

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
    __declspec(noinline) void TH12AddVentra(int ventra)
    {
        int32_t item_struct = GetMemAddr(0x4b44f0, 0x171254);
        ventra -= 1;
#ifndef __clang__
        __asm mov eax, ventra;
        __asm mov ecx, item_struct;
        __asm mov edx, 0x4270b0;
        __asm call edx;
#else
        asm volatile(
            "call *%[func]"
            :
            : [func] "r"(0x4270b0), "c"(item_struct), "a"(ventra)
        );
#endif
    }

    static void RenderLockTimer(ImDrawList* p)
    {
        if (*THOverlay::singleton().mTimeLock && g_lock_timer > 0) {
            std::string time_text = std::format("{:.2f}", (float)g_lock_timer / 60.0f);
            auto sz = ImGui::CalcTextSize(time_text.c_str());
            p->AddRectFilled({ 32.0f, 0.0f }, { 110.0f, sz.y }, 0xFFFFFFFF);
            p->AddText({ 110.0f - sz.x, 0.0f }, 0xFF000000, time_text.c_str());
        }
    }

    HOOKSET_DEFINE(THMainHook)
    EHOOK_DY(th12_inf_lives, 0x4381E7,6,{
        if ((*(THOverlay::singleton().mInfLives))) {
            if (!g_adv_igi_options.map_inf_life_to_no_continue) {
                *(DWORD*)(0x004B0C98) = *(DWORD*)(0x004B0C98)+1;
            } else {
                if (*(DWORD*)(0x004B0C98) == 0)
                    *(DWORD*)(0x004B0C98) = 1;
            }
        }
    })
    EHOOK_DY(th12_everlasting_bgm, 0x454960, 10, {
        int32_t retn_addr = ((int32_t*)pCtx->Esp)[0];
        int32_t bgm_cmd = ((int32_t*)pCtx->Esp)[1];
        int32_t bgm_id = ((int32_t*)pCtx->Esp)[2];
        // 4th stack item = i32 call_addr

        bool el_switch;
        bool is_practice;
        bool result;

        el_switch = *(THOverlay::singleton().mElBgm) && !THGuiRep::singleton().mRepStatus && thPracParam.mode && thPracParam.section;
        is_practice = (*((int32_t*)0x4b0ce0) & 0x1);
        result = ElBgmTest<0x430183, 0x4226c7, 0x432820, 0x432982, 0xffffffff>(
            el_switch, is_practice, retn_addr, bgm_cmd, bgm_id, 0xffffffff);

        if (result) {
            pCtx->Eip = 0x4549f4;
        }
    })
    EHOOK_DY(th12_param_reset, 0x43fdfb, 7, {
        thPracParam.Reset();
    })
    EHOOK_DY(th12_prac_menu_1, 0x446059, 7, {
        THGuiPrac::singleton().State(1);
    })
    EHOOK_DY(th12_prac_menu_2, 0x44607b, 3, {
        THGuiPrac::singleton().State(2);
    })
    EHOOK_DY(th12_prac_menu_3, 0x4462d3, 7, {
        THGuiPrac::singleton().State(3);
    })
    EHOOK_DY(th12_prac_menu_4, 0x44636b, 7, {
        THGuiPrac::singleton().State(4);
    })
    PATCH_DY(th12_prac_menu_enter_1, 0x446138, "eb")
    EHOOK_DY(th12_prac_menu_enter_2, 0x446333, 1, {
        pCtx->Eax = thPracParam.stage;
    })
    EHOOK_DY(th12_disable_prac_menu_1, 0x4464e5, 2, {
        pCtx->Eip = 0x44651a;
    })
    PATCH_DY(th12_disable_prac_menu_2, 0x44603e, "83c4106690")
    EHOOK_DY(th12_patch_main, 0x40e8df, 1, {
        if (thPracParam.mode == 1) {
            *(int32_t*)(0x4b0c44) = (int32_t)(thPracParam.score / 10);
            *(int32_t*)(0x4b0c98) = thPracParam.life;
            *(int32_t*)(0x4b0c9c) = thPracParam.life_fragment ? (thPracParam.life_fragment + 1) : 0;
            *(int32_t*)(0x4b0ca0) = thPracParam.bomb;
            *(int32_t*)(0x4b0ca4) = thPracParam.bomb_fragment * 2;
            *(int32_t*)(0x4b0c48) = thPracParam.power;
            *(int32_t*)(0x4b0c78) = thPracParam.value * 100;
            *(int32_t*)(0x4b0cdc) = thPracParam.graze;
            *(int32_t*)(*(uint32_t*)0x4b44f0 + 0x666fe0) = thPracParam.ufo_side;

            if (thPracParam.ventra_1) {
                TH12AddVentra(thPracParam.ventra_1);
                if (thPracParam.ventra_2) {
                    TH12AddVentra(thPracParam.ventra_2);
                    if (thPracParam.ventra_3) {
                        TH12AddVentra(thPracParam.ventra_3);
                    }
                }
            }

            THSectionPatch();
        }
        thPracParam._playLock = true;
    })
    EHOOK_DY(th12_bgm, 0x42293a, 1, {
        if (THBGMTest()) {
            PushHelper32(pCtx, 1);
            pCtx->Eip = 0x42293b;
        }
    })
    EHOOK_DY(th12_rep_save, 0x43c32c, 7, {
        char* repName = (char*)(pCtx->Esp + 0x28);
        if (thPracParam.mode)
            THSaveReplay(repName);
    })
    EHOOK_DY(th12_rep_menu_1, 0x4467df, 3, {
        THGuiRep::singleton().State(1);
    })
    EHOOK_DY(th12_rep_menu_2, 0x4468f8, 5, {
        THGuiRep::singleton().State(2);
    })
    EHOOK_DY(th12_rep_menu_3, 0x446ab1, 2, {
        THGuiRep::singleton().State(3);
    })
    EHOOK_DY(th12_update, 0x4625fb, 1, {
        GameGuiBegin(IMPL_WIN32_DX9, !THAdvOptWnd::singleton().IsOpen());

        // Gui components update
        THGuiPrac::singleton().Update();
        THGuiRep::singleton().Update();
        THOverlay::singleton().Update();
        TH12InGameInfo::singleton().Update();
        
        auto p = ImGui::GetOverlayDrawList();
        // in case boss movedown do not disabled when playing normal games
        {
            if (THAdvOptWnd::singleton().forceBossMoveDown) {
                auto sz = ImGui::CalcTextSize(S(TH_BOSS_FORCE_MOVE_DOWN));
                p->AddRectFilled({ 120.0f, 0.0f }, { sz.x + 120.0f, sz.y }, 0xFFCCCCCC);
                p->AddText({ 120.0f, 0.0f }, 0xFFFF0000, S(TH_BOSS_FORCE_MOVE_DOWN));
            }
        }

        if (*(DWORD*)0x004B4514)
            RenderBlindView(9, *(DWORD*)(0x4ce8f0), *(ImVec2*)(*(DWORD*)0x004B4514 + 0x97C), { 192.0f, 0.0f }, { 32.0f, 16.0f }, ImGui::GetIO().DisplaySize.x / 640.0f);
        
        if (g_adv_igi_options.show_keyboard_monitor && *(DWORD*)(0x004B4514)) {
            g_adv_igi_options.keyboard_style.size = { 40.0f, 40.0f };
            KeysHUD(12, { 1280.0f, 0.0f }, { 835.0f, 0.0f }, g_adv_igi_options.keyboard_style);
        }

        RenderLockTimer(p);

        bool drawCursor = THAdvOptWnd::StaticUpdate() || THGuiPrac::singleton().IsOpen();
        GameGuiEnd(drawCursor);
    })
    EHOOK_DY(th12_player_state, 0x436BA0,1,{
        if (g_adv_igi_options.show_keyboard_monitor)
            RecordKey(12, *(DWORD*)(0x4D49D0));
    })
    EHOOK_DY(th12_render, 0x462722, 1, {
        GameGuiRender(IMPL_WIN32_DX9);
    })
    HOOKSET_ENDDEF()

    HOOKSET_DEFINE(THInGameInfo)
    EHOOK_DY(th12_game_start, 0x421DEF,6,// gamestart-bomb set
    {
        TH12InGameInfo::singleton().mBombCount = 0;
        TH12InGameInfo::singleton().mMissCount = 0;
        TH12InGameInfo::singleton().mRUFOCount = 0;
        TH12InGameInfo::singleton().mGUFOCount = 0;
        TH12InGameInfo::singleton().mBUFOCount = 0;
        TH12InGameInfo::singleton().mCUFOCount = 0;

        TH12InGameInfo::singleton().mRVentraCount_Get = 0;
        TH12InGameInfo::singleton().mBVentraCount_Get = 0;
        TH12InGameInfo::singleton().mGVentraCount_Get = 0;

        TH12InGameInfo::singleton().mRVentraCount_Drop = 0;
        TH12InGameInfo::singleton().mBVentraCount_Drop = 0;
        TH12InGameInfo::singleton().mGVentraCount_Drop = 0;
        TH12InGameInfo::singleton().mCVentraCount_Drop = 0;
    })
    EHOOK_DY(th12_bomb_dec, 0x422F28,5, // bomb dec
    {
        TH12InGameInfo::singleton().mBombCount++;
    })
    EHOOK_DY(th12_life_dec, 0x4381E2,5, // life dec
    {
        TH12InGameInfo::singleton().mMissCount++;
    })
    EHOOK_DY(th12_ufo_spawn, 0x44A909, 7, // spawn ufo
    {
        int32_t type = pCtx->Eax;
        if (type == 0)
            TH12InGameInfo::singleton().mRUFOCount++;
        else if (type == 1)
            TH12InGameInfo::singleton().mBUFOCount++;
        else if (type == 2)
            TH12InGameInfo::singleton().mGUFOCount++;
        else if (type == 3)
            TH12InGameInfo::singleton().mCUFOCount++;
    })
    EHOOK_DY(th12_ufo_item, 0x4270b0, 3, { // ufo item eat
        int32_t type = pCtx->Eax;
        switch (type)
        {
            case 0:TH12InGameInfo::singleton().mRVentraCount_Get++;break;
            case 1:TH12InGameInfo::singleton().mBVentraCount_Get++;break;
            case 2:TH12InGameInfo::singleton().mGVentraCount_Get++;break;
            default:
                break;
        }
    })
    EHOOK_DY(th12_ufo_item2, 0x427411, 6, { // ufo item drop
        int32_t type = pCtx->Eax;
        switch (type)
        {
            case 0:TH12InGameInfo::singleton().mRVentraCount_Drop++;break;
            case 1:TH12InGameInfo::singleton().mBVentraCount_Drop++;break;
            case 2:TH12InGameInfo::singleton().mGVentraCount_Drop++;break;

            case 3:TH12InGameInfo::singleton().mCVentraCount_Drop++;break;
            case 4:TH12InGameInfo::singleton().mCVentraCount_Drop++;break;
            case 5:TH12InGameInfo::singleton().mCVentraCount_Drop++;break;
            default:
                break;
        }
    })
    EHOOK_DY(th12_lock_timer1, 0x41D4E5,6, // initialize
    {
        g_lock_timer = 0;
    })
    EHOOK_DY(th12_lock_timer2, 0x418745,5, // SetNextPattern case 414
    {
        g_lock_timer = 0;
    })
    EHOOK_DY(th12_lock_timer3, 0x417AEF,2, // set boss mode case 412
    {
        g_lock_timer = 0;
    })
    EHOOK_DY(th12_lock_timer4, 0x41A821,6, // decrease time (update)
    {
        g_lock_timer++;
    })
    HOOKSET_ENDDEF()

    static __declspec(noinline) void THGuiCreate()
    {
        // Init
        if (ImGui::GetCurrentContext()) {
            return;
        }
        GameGuiInit(IMPL_WIN32_DX9, 0x4ce8f0, 0x4cf3f0,
            Gui::INGAGME_INPUT_GEN2, 0x4d48c4, 0x4d48c0, 0,
            -1);
        SetDpadHook(0x462CAF, 2);

        // Gui components creation
        THGuiPrac::singleton();
        THGuiRep::singleton();
        THOverlay::singleton();
        TH12InGameInfo::singleton();

        // Hooks
        EnableAllHooks(THMainHook);
        EnableAllHooks(THInGameInfo);

        // Reset thPracParam
        thPracParam.Reset();
    }
    HOOKSET_DEFINE(THInitHook)
    PATCH_DY(th12_disable_demo, 0x43f78f, "ffffff7f")
    PATCH_DY(th12_disable_mutex, 0x44f603, "eb")
    PATCH_DY(th12_startup_1, 0x43f357, "eb")
    PATCH_DY(th12_startup_2, 0x43fe69, "eb")
    EHOOK_DY(th12_gui_init_1, 0x440808, 3, {
        self->Disable();
        THGuiCreate();
    })
    EHOOK_DY(th12_gui_init_2, 0x4511f9, 1, {
        self->Disable();
        THGuiCreate();
    })
    HOOKSET_ENDDEF()
}

void TH12Init()
{
    EnableAllHooks(TH12::THInitHook);
    TryKeepUpRefreshRate((void*)0x450dcc);
    if (GetModuleHandleA("vpatch_th12.dll")) {
        TryKeepUpRefreshRate((void*)((DWORD)GetModuleHandleA("vpatch_th12.dll") + 0x54ce));
    }
}
}
