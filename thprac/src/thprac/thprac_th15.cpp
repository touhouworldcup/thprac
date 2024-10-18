#include "thprac_games.h"
#include "thprac_utils.h"


namespace THPrac {
namespace TH15 {
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

        bool dlg;

        bool _playLock = false;
        void Reset()
        {
            memset(this, 0, sizeof(THPracParam));
        }
        bool ReadJson(std::string& json)
        {
            ParseJson();

            ForceJsonValue(game, "th15");
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

            return true;
        }
        std::string GetJson()
        {
            CreateJson();

            AddJsonValueEx(version, GetVersionStr(), jalloc);
            AddJsonValueEx(game, "th15", jalloc);
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
        SINGLETON(THGuiPrac)
    public:

        __declspec(noinline) void State(int state)
        {
            switch (state) {
            case 0:
                break;
            case 1:
                mDiffculty = *((int32_t*)0x4e7410);
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
            ImGui::TextUnformatted(S(TH_MENU));
            ImGui::Separator();

            PracticeMenu();
        }
        const th_glossary_t* SpellPhase()
        {
            auto section = CalcSection();
            if (section == TH15_ST6_BOSS11) {
                return TH_SPELL_PHASE2;
            } else if (section == TH15_ST7_END_S10) {
                return TH15_SPELL_PHASE_EXTRA_LAST;
            } else if (section == TH15_ST5_MID1) {
                return TH15_ITS_LUNATIC_TIME;
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
                ;
                if (*mStage == 6) {
                    mLifeFragment.SetBound(0, 5);
                } else {
                    mLifeFragment.SetBound(0, 3);
                }
                mLifeFragment();
                mBomb();
                mBombFragment();
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
            case TH15_ST1_BOSS1:
            case TH15_ST2_BOSS1:
            case TH15_ST3_BOSS1:
            case TH15_ST4_BOSS1:
            case TH15_ST5_BOSS1:
            case TH15_ST6_BOSS1:
            case TH15_ST7_END_NS1:
            case TH15_ST7_MID1:
                return true;
            default:
                return false;
            }
        }
        void SectionWidget()
        {
            static int Morbius = 0;
            const char _MorbStr[] = "MORBIUS";
            if (Gui::KeyboardInputUpdate(_MorbStr[Morbius]))
                Morbius++;
            if (Morbius == 7)
                th_sections_str[::THPrac::Gui::LocaleGet()][mDiffculty][TH15_ST5_MID1] = "it's morbin time";

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
        Gui::GuiSlider<int, ImGuiDataType_S32> mBombFragment { TH_BOMB_FRAGMENT, 0, 4 };
        Gui::GuiSlider<int, ImGuiDataType_S32> mPower { TH_POWER, 100, 400 };
        Gui::GuiDrag<int, ImGuiDataType_S32> mValue { TH_VALUE, 0, 999990, 10, 100000 };
        Gui::GuiDrag<int, ImGuiDataType_S32> mGraze { TH_GRAZE, 0, 999999, 1, 100000 };

        Gui::GuiNavFocus mNavFocus { TH_STAGE, TH_MODE, TH_WARP, TH_DLG,
            TH_MID_STAGE, TH_END_STAGE, TH_NONSPELL, TH_SPELL, TH_PHASE, TH_CHAPTER,
            TH_SCORE, TH_LIFE, TH_LIFE_FRAGMENT, TH_BOMB, TH_BOMB_FRAGMENT,
            TH_POWER, TH_VALUE, TH_GRAZE };

        int mChapterSetup[7][2] {
            { 2, 4 },
            { 3, 3 },
            { 3, 3 },
            { 3, 3 },
            { 4, 6 },
            { 4, 0 },
            { 5, 4 },
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
            uint32_t index = GetMemContent(0x4e9be0, 0x5a00);
            char* repName = (char*)GetMemAddr(0x4e9be0, index * 4 + 0x5a08, 0x21c);
            std::wstring repDir(mAppdataPath);
            repDir.append(L"\\ShanghaiAlice\\th15\\replay\\");
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
        Gui::GuiHotKey mMuteki { TH_MUTEKI, "F1", VK_F1, {
            new HookCtx(0x4566a5, "\x01", 1) } };
        Gui::GuiHotKey mInfLives { TH_INFLIVES, "F2", VK_F2, {
            new HookCtx(0x456397, "\x90", 1) } };
        Gui::GuiHotKey mInfBombs { TH_INFBOMBS, "F3", VK_F3, {
            new HookCtx(0x414963, "\x90", 1) } };
        Gui::GuiHotKey mInfPower { TH_INFPOWER, "F4", VK_F4, {
            new HookCtx(0x4582fa, "\x45", 1) } };
        Gui::GuiHotKey mTimeLock { TH_TIMELOCK, "F5", VK_F5, {
            new HookCtx(0x41fdf5, "\xeb", 1),
            new HookCtx(0x428b5d, "\xa7", 1) } };
        Gui::GuiHotKey mAutoBomb { TH_AUTOBOMB, "F6", VK_F6, {
            new HookCtx(0x454cc9, "\xc6", 1) } };

    public:
        Gui::GuiHotKey mElBgm { TH_EL_BGM, "F7", VK_F7 };
        Gui::GuiHotKey mInGameInfo { THPRAC_INGAMEINFO, "F8", VK_F8 };
    };

    
    class TH15InGameInfo : public Gui::GameGuiWnd {

        TH15InGameInfo() noexcept
        {
            SetTitle("igi");
            SetFade(0.9f, 0.9f);
            SetPos(-10000.0f, -10000.0f);
            SetSize(0.0f, 0.0f);
            SetWndFlag(
                ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav | 0);
            OnLocaleChange();
        }
        SINGLETON(TH15InGameInfo);

    public:
        int32_t mMissCount;
        int32_t mBombCount;
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
            if (!*(DWORD*)(0x004E9BB8)){
                SetPos(-10000.0f, -10000.0f); //fly~
                return;
            }
            bool is_in_P_mode = (*(byte*)(0x004E7795)) & 0x1;
            if (is_in_P_mode) // pplayer
            {
                SetPosRel(900.0f / 1280.0f, 520.0f / 960.0f);
                SetSizeRel(340.0f / 1280.0f, 350.0f / 960.0f);
                int cur_stage = *(int*)(0x004E73F0);
                int tot_re = *(int*)(0x004E7594);
                int cur_re = *(int*)(0x004E75B8);
                int* stage_re = (int*)(0x004E759C);
                ImGui::Columns(2);
                ImGui::Text(S(THPRAC_INGAMEINFO_15_RE_TIMES_TOTAL));
                ImGui::NextColumn();
                ImGui::Text("%8d", tot_re);
                ImGui::NextColumn();
                ImGui::Text(S(THPRAC_INGAMEINFO_15_RE_TIMES_CURRENT));
                ImGui::NextColumn();
                ImGui::Text("%8d", cur_re);

                ImGui::Columns(1);
                ImGui::NewLine();
                ImGui::Columns(2);
                // ImGui::NextColumn();
                // ImGui::NewLine();
                // ImGui::NextColumn();
                // ImGui::NewLine();
                // ImGui::NextColumn();

                for (int i = 0; i < std::min(7,cur_stage); i++){
                    ImGui::Text(S(THPRAC_INGAMEINFO_15_RE_TIMES_STAGE), i + 1);
                    ImGui::NextColumn();
                    ImGui::Text("%8d", stage_re[i]);
                    ImGui::NextColumn();
                }
            } else {
                SetPosRel(900.0f / 1280.0f, 520.0f / 960.0f);
                SetSizeRel(340.0f / 1280.0f, 100.0f / 960.0f);
                ImGui::Columns(2);
                ImGui::Text(S(THPRAC_INGAMEINFO_MISS_COUNT));ImGui::NextColumn();ImGui::Text("%8d",mMissCount);
                ImGui::NextColumn();
                ImGui::Text(S(THPRAC_INGAMEINFO_BOMB_COUNT));ImGui::NextColumn();ImGui::Text("%8d",mBombCount);
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

    class THAdvOptWnd : public Gui::PPGuiWnd {
        EHOOK_ST(th15_all_clear_bonus_1, 0x43d99d)
        {
            pCtx->Eip = 0x43d9aa;
        }
        EHOOK_ST(th15_all_clear_bonus_2, 0x43daac)
        {
            *(int32_t*)(GetMemAddr(0x4e9a8c, 0x160)) = *(int32_t*)(0x4e740c);
            if (GetMemContent(0x4e7794) & 0x10) {
                typedef void (*PScoreFunc)();
                PScoreFunc a = (PScoreFunc)0x4512a0;
                a();
                pCtx->Eip = 0x43d9a2;
            }
        }
        EHOOK_ST(th15_all_clear_bonus_3, 0x43dcb5)
        {
            *(int32_t*)(GetMemAddr(0x4e9a8c, 0x160)) = *(int32_t*)(0x4e740c);
            if (GetMemContent(0x4e7794) & 0x10) {
                typedef void (*PScoreFunc)();
                PScoreFunc a = (PScoreFunc)0x4512a0;
                a();
                pCtx->Eip = 0x43d9a2;
            }
        }
        HookCtx* th15_master_disable[3];
        bool disableMaster = false;
    private:
        void MasterDisableInit()
        {
            th15_master_disable[0] = new HookCtx(0x420346, "\xEB", 1);
            th15_master_disable[0]->Setup();
            th15_master_disable[1] = new HookCtx(0x42036B, "\xEB", 1);
            th15_master_disable[1]->Setup();
            th15_master_disable[2] = new HookCtx(0x420296, "\x00", 1);
            th15_master_disable[2]->Setup();
        }
        void FpsInit()
        {
            mOptCtx.vpatch_base = (int32_t)GetModuleHandleW(L"vpatch_th15.dll");
            if (mOptCtx.vpatch_base) {
                uint64_t hash[2];
                CalcFileHash(L"vpatch_th15.dll", hash);
                if (hash[0] != 16371671977271057239ll || hash[1] != 17823539316282081507ll)
                    mOptCtx.fps_status = -1;
                else if (*(int32_t*)(mOptCtx.vpatch_base + 0x42fbc) == 0) {
                    mOptCtx.fps_status = 2;
                    mOptCtx.fps = *(int32_t*)(mOptCtx.vpatch_base + 0x42fcc);
                }
            } else if (*(uint8_t*)0x4e79c9 == 3) {
                mOptCtx.fps_status = 1;

                DWORD oldProtect;
                VirtualProtect((void*)0x472802, 4, PAGE_EXECUTE_READWRITE, &oldProtect);
                *(double**)0x472802 = &mOptCtx.fps_dbl;
                VirtualProtect((void*)0x472802, 4, oldProtect, &oldProtect);
            } else
                mOptCtx.fps_status = 0;
        }
        void FpsSet()
        {
            if (mOptCtx.fps_status == 1) {
                mOptCtx.fps_dbl = 1.0 / (double)mOptCtx.fps;
            } else if (mOptCtx.fps_status == 2) {
                *(int32_t*)(mOptCtx.vpatch_base + 0x40a34) = mOptCtx.fps;
                *(int32_t*)(mOptCtx.vpatch_base + 0x42fcc) = mOptCtx.fps;
            }
        }
        void GameplayInit()
        {
            th15_all_clear_bonus_1.Setup();
            th15_all_clear_bonus_2.Setup();
            th15_all_clear_bonus_3.Setup();
        }
        void GameplaySet()
        {
            th15_all_clear_bonus_1.Toggle(mOptCtx.all_clear_bonus);
            th15_all_clear_bonus_2.Toggle(mOptCtx.all_clear_bonus);
            th15_all_clear_bonus_3.Toggle(mOptCtx.all_clear_bonus);
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
            ImGui::TextUnformatted(S(TH_ADV_OPT));
            ImGui::Separator();
            ImGui::BeginChild("Adv. Options", ImVec2(0.0f, 0.0f));

            if (BeginOptGroup<TH_GAME_SPEED>()) {
                if (GameFPSOpt(mOptCtx))
                    FpsSet();
                EndOptGroup();
            }
            if (BeginOptGroup<TH_GAMEPLAY>()) {
                DisableXKeyOpt();
                if (ImGui::Checkbox(S(TH_DISABLE_MASTER), &disableMaster)) {
                    th15_master_disable[0]->Toggle(disableMaster);
                    th15_master_disable[1]->Toggle(disableMaster);
                    th15_master_disable[2]->Toggle(disableMaster);
                }
                ImGui::SameLine();
                HelpMarker(S(TH_DISABLE_MASTER_DESC));
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

    EHOOK_G1(th15_chapter_set, 0x43dd58)
    {
        if (th15_chapter_set::GetData() != -1) {
            *((int32_t*)0x4e73f8) = th15_chapter_set::GetData();
            th15_chapter_set::GetData() = -1;
        }
        th15_chapter_set::GetHook().Disable();
    }
    EHOOK_G1(th15_chapter_disable, 0x43d0b5)
    {
        --th15_chapter_disable::GetData();
        if (!th15_chapter_disable::GetData())
            th15_chapter_disable::GetHook().Disable();
        pCtx->Eip = 0x43d0d5;
    }
    EHOOK_G1(th15_st7boss1_chapter_bonus, 0x43dece)
    {
        th15_st7boss1_chapter_bonus::GetHook().Disable();
        *(uint32_t*)0x4e7484 = 1;
    }
    EHOOK_G1(th15_stars_bgm_sync, 0x48c294)
    {
        int32_t call_addr = *(int32_t*)(pCtx->Esp + 0x8);

        if (thPracParam.mode == 1 && thPracParam.section == TH15_ST6_STARS) {
            if (call_addr == 0x48B4EB) {
                *(uint32_t*)(pCtx->Esp + 0x10) = 0x8fc768;
                th15_stars_bgm_sync::GetHook().Disable();
            }
        }
    }
    void ECLSetChapter(size_t chapter)
    {
        th15_chapter_set::GetData() = chapter;
        th15_chapter_set::GetHook().Enable();
    }
    void ECLSkipChapter(size_t times)
    {
        th15_chapter_disable::GetData() = times;
        th15_chapter_disable::GetHook().Enable();
    }
    void ECLStarsBGMSync()
    {
        th15_stars_bgm_sync::GetHook().Enable();
    }
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
                ECLJump(ecl, 0x7248, 0x7494, 60, 90); // 0x7544
                ECLJump(ecl, 0x3f4c, 0x3fa8, 0, 0);
                break;
            case 3:
                ECLJump(ecl, 0x7248, 0x7544, 60, 90);
                ECLSetChapter(2);
                break;
            case 4:
                ECLJump(ecl, 0x7248, 0x7544, 60, 90);
                ECLJump(ecl, 0x4098, 0x40fc, 0, 0);
                break;
            case 5:
                ECLJump(ecl, 0x7248, 0x75f8, 60, 90);
                ECLSetChapter(5);
                break;
            case 6:
                ECLJump(ecl, 0x7248, 0x75f8, 60, 90);
                ECLJump(ecl, 0x4150, 0x41b8, 0, 0);
                break;
            default:
                break;
            }
        } else if (stage == 2) {
            switch (portion) {
            case 1:
                break;
            case 2:
                ECLJump(ecl, 0x74a0, 0x76ec, 60, 90); // 0x778c
                ECLJump(ecl, 0x42b4, 0x4354, 0, 0);
                break;
            case 3:
                ECLJump(ecl, 0x74a0, 0x76ec, 60, 90); // 0x778c
                ECLJump(ecl, 0x42b4, 0x43d4, 0, 0);
                break;
            case 4:
                ECLJump(ecl, 0x74a0, 0x778c, 60, 29);
                ECLSetChapter(4);
                break;
            case 5:
                ECLJump(ecl, 0x74a0, 0x778c, 60, 0);
                ECLJump(ecl, 0x443c, 0x448c, 0, 0);
                break;
            case 6:
                ECLJump(ecl, 0x74a0, 0x778c, 60, 90);
                ECLJump(ecl, 0x443c, 0x450c, 0, 0);
                break;
            default:
                break;
            }
        } else if (stage == 3) {
            switch (portion) {
            case 1:
                break;
            case 2:
                ECLJump(ecl, 0xa280, 0xa4cc, 60, 90); // 0xa56c
                ECLJump(ecl, 0x5b6c, 0x5c00, 0, 0);
                break;
            case 3:
                ECLJump(ecl, 0xa280, 0xa4cc, 60, 30); // 0xa56c
                ECLJump(ecl, 0x5b6c, 0x5c6c, 0, 0);
                break;
            case 4:
                ECLJump(ecl, 0xa280, 0xa56c, 60, 50);
                ECLSetChapter(4);
                break;
            case 5:
                ECLJump(ecl, 0xa280, 0xa56c, 60, 20);
                ECLJump(ecl, 0x5d0c, 0x5d70, 0, 0);
                break;
            case 6:
                ECLJump(ecl, 0xa280, 0xa56c, 60, 90);
                ECLJump(ecl, 0x5d0c, 0x5df0, 0, 0);
                break;
            default:
                break;
            }
        } else if (stage == 4) {
            switch (portion) {
            case 1:
                break;
            case 2:
                ECLJump(ecl, 0x8634, 0x8880, 60, 90); // 0x8930
                ECLJump(ecl, 0x406c, 0x40ec, 0, 0);
                break;
            case 3:
                ECLJump(ecl, 0x8634, 0x8880, 60, 90); // 0x8930
                ECLJump(ecl, 0x406c, 0x4158, 0, 0);
                ecl << pair{0x5cc4, 0};
                break;
            case 4:
                ECLJump(ecl, 0x8634, 0x8930, 60, 90);
                ecl << pair{0x6bc0, 0};
                break;
            case 5:
                ECLJump(ecl, 0x8634, 0x8930, 60, 90);
                ECLJump(ecl, 0x41e0, 0x4244, 0, 0);
                ecl << pair{0x6e74, 0};
                break;
            case 6:
                ECLJump(ecl, 0x8634, 0x8930, 60, 90);
                ECLJump(ecl, 0x41e0, 0x42c4, 0, 0);
                break;
            default:
                break;
            }
        } else if (stage == 5) {
            switch (portion) {
            case 1:
                break;
            case 2:
                ECLJump(ecl, 0x9f34, 0xa180, 60, 90); // 0xa1a0
                ECLJump(ecl, 0x6ed8, 0x6f58, 0, 0);
                break;
            case 3:
                ECLJump(ecl, 0x9f34, 0xa180, 60, 90); // 0xa1a0
                ECLJump(ecl, 0x6ed8, 0x7040, 0, 0);
                ecl << pair{0x7ba0, 0};
                break;
            case 4:
                ECLJump(ecl, 0x9f34, 0xa180, 60, 90); // 0xa1a0
                ECLJump(ecl, 0x6ed8, 0x7098, 0, 0);
                break;
            case 5:
                ECLJump(ecl, 0x9f34, 0xa1a0, 60, 90);
                ecl << pair{0x70f0, (int16_t)0};
                break;
            case 6:
                ECLJump(ecl, 0x9f34, 0xa1a0, 60, 90);
                ECLJump(ecl, 0x70ec, 0x716c, 0, 0);
                ecl << pair{0x81c0, 0};
                break;
            case 7:
                ECLJump(ecl, 0x9f34, 0xa1a0, 60, 90);
                ECLJump(ecl, 0x70ec, 0x71ec, 0, 0);
                ecl << pair{0x845c, 0};
                break;
            case 8:
                ECLJump(ecl, 0x9f34, 0xa1a0, 60, 90);
                ECLJump(ecl, 0x70ec, 0x726c, 0, 0);
                ecl << pair{0x87e0, 0};
                break;
            case 9:
                ECLJump(ecl, 0x9f34, 0xa1a0, 60, 90);
                ECLJump(ecl, 0x70ec, 0x72ec, 0, 0);
                ecl << pair{0x8ce8, 0};
                break;
            case 10:
                ECLJump(ecl, 0x9f34, 0xa1a0, 60, 90);
                ECLJump(ecl, 0x70ec, 0x736c, 0, 0);
                ecl << pair{0x9a50, 0};
                break;
            default:
                break;
            }
        } else if (stage == 6) {
            switch (portion) {
            case 1:
                break;
            case 2:
                ECLJump(ecl, 0x91f8, 0x93e0, 60, 90);
                ECLJump(ecl, 0x2b34, 0x2be8, 0, 0);
                ecl << pair{0x38d0, 0};
                break;
            case 3:
                ECLJump(ecl, 0x91f8, 0x93e0, 60, 90);
                ECLJump(ecl, 0x2b34, 0x2c68, 0, 0);
                ecl << pair{0x40ac, 0};
                break;
            case 4:
                ECLJump(ecl, 0x91f8, 0x93e0, 60, 90);
                ECLJump(ecl, 0x2b34, 0x2ce8, 0, 0);
                ecl << pair{0x43b0, 0};
                break;
            default:
                break;
            }
        } else if (stage == 7) {
            switch (portion) {
            case 1:
                break;
            case 2:
                ECLJump(ecl, 0x8f94, 0x9208, 60, 90); // 0x9338
                ECLJump(ecl, 0x5d08, 0x5d88, 0, 0);
                break;
            case 3:
                ECLJump(ecl, 0x8f94, 0x9208, 60, 90); // 0x9338
                ECLJump(ecl, 0x5d08, 0x5e08, 0, 0);
                break;
            case 4:
                ECLJump(ecl, 0x8f94, 0x9208, 60, 90); // 0x9338
                ECLJump(ecl, 0x5d08, 0x5e9c, 0, 0);
                break;
            case 5:
                ECLJump(ecl, 0x8f94, 0x9208, 60, 90); // 0x9338
                ECLJump(ecl, 0x5d08, 0x5f30, 0, 0);
                break;
            case 6:
                ECLJump(ecl, 0x8f94, 0x9338, 60, 90);
                break;
            case 7:
                ECLJump(ecl, 0x8f94, 0x9338, 60, 10);
                ECLJump(ecl, 0x5f84, 0x6018, 0, 0);
                break;
            case 8:
                ECLJump(ecl, 0x8f94, 0x9338, 60, 90);
                ECLJump(ecl, 0x5f84, 0x60c0, 0, 0);
                break;
            case 9:
                ECLJump(ecl, 0x8f94, 0x9338, 60, 90);
                ECLJump(ecl, 0x5f84, 0x6154, 0, 0);
                break;
            default:
                break;
            }
        }
    }
    __declspec(noinline) void THPatch(ECLHelper& ecl, th_sections_t section)
    {
        auto st7_hide_subboss = [&]() {
            ecl.SetFile(4);
            // Void Particle Effect & Wait
            ecl << pair{0x190, (int16_t)0} << pair{0x1dc, (int16_t)0} << pair{0x228, (int16_t)0};
            // Jump Through Dialogue
            ECLJump(ecl, 0x2a0, 0x2f4, 5);
            // Remove Movement Restriction & Move Directly
            ecl.SetPos(0x480);
            ecl << 0 << 0x001001f9 << 0x00ff0000 << 0
                << 0 << 0x00340190 << 0x02ff0000 << 0 << 0x43400000 << 0xc2800000;
        };
        auto st7_enter_spell = [&](int ordinal, int health, bool is_junko = false) {
            ecl.SetFile(3);
            ecl.SetPos(0x6d0);
            ecl << 0 << 0x001401ff << 0x01ff0000 << 0 << health;
            if (is_junko) {
                ecl << 0 << 0x0014012e << 0x01ff0000 << 0 << 5
                    << 0 << 0x0018012f << 0x02ff0000 << 0 << 3 << 6;
                ecl << 0 << 0x0020000f << 0x01ff0000 << 0 << 0xc
                    << 0x73736f42 << 0x6f705f34 << 0x00000073; // "Boss", "4_po", "s"
            }
            ecl << 0 << 0x0020000b << 0x01ff0000 << 0 << 0xc
                << 0x73736f42 << 0x64726143 << ordinal;
        };

        switch (section) {
        case THPrac::TH15::TH15_ST1_MID1:
            ECLJump(ecl, 0x7220, 0x74d8, 60);
            ECLSkipChapter(1);
            break;
        case THPrac::TH15::TH15_ST1_MID2:
            ECLJump(ecl, 0x7220, 0x758c, 60);
            ECLSkipChapter(1);
            break;
        case THPrac::TH15::TH15_ST1_MID3:
            ECLJump(ecl, 0x7220, 0x75b4, 60);
            ecl.SetFile(4);
            ECLSkipChapter(1);
            ECLJump(ecl, 0x444, 0x560, 0); // Utilize Spell Practice Jump
            ecl << pair{0x434, 60} << pair{0x570, 60}; // Move Speed
            ecl << pair{0x588, 9999}; // Do not use spellcard
            ecl.SetPos(0x384);
            ecl << 0 << 0x001c0203 << 0x01ff0000 << 0 << 60; // Muteki
            break;
        case THPrac::TH15::TH15_ST1_BOSS1:
            if (thPracParam.dlg)
                ECLJump(ecl, 0x7220, 0x7678, 60);
            else
                ECLJump(ecl, 0x7220, 0x768c, 60);
            ecl.SetFile(2);
            ECLSkipChapter(1);
            break;
        case THPrac::TH15::TH15_ST1_BOSS2:
            ECLJump(ecl, 0x7220, 0x768c, 60);
            ecl.SetFile(2);
            ECLSkipChapter(2);
            ECLJump(ecl, 0x3b8, 0x4a0, 1); // Utilize Spell Practice Jump
            ecl << pair{0x4b0, 1700}; // Set Health
            ecl << pair{0x4d0, (int8_t)0x31}; // Set Spell Ordinal
            break;
        case THPrac::TH15::TH15_ST1_BOSS3:
            ECLJump(ecl, 0x7220, 0x768c, 60);
            ecl.SetFile(2);
            ECLSkipChapter(2);
            ecl << pair{0x5d0, (int8_t)0x32}; // Change Nonspell
            ecl << pair{0x13ec, (int16_t)0} << pair{0x151c, (int16_t)0}; // Disable Item Drops & SE
            break;
        case THPrac::TH15::TH15_ST1_BOSS4:
            ECLJump(ecl, 0x7220, 0x768c, 60);
            ecl.SetFile(2);
            ECLSkipChapter(2);
            ECLJump(ecl, 0x3b8, 0x4a0, 1); // Utilize Spell Practice Jump
            ecl << pair{0x4b0, 1900}; // Set Health
            ecl << pair{0x4d0, (int8_t)0x32}; // Set Spell Ordinal
            break;
        case THPrac::TH15::TH15_ST2_MID1:
            ECLJump(ecl, 0x7444, 0x7720, 60);
            ECLSkipChapter(1);
            break;
        case THPrac::TH15::TH15_ST2_BOSS1:
            if (thPracParam.dlg)
                ECLJump(ecl, 0x7444, 0x7820, 60);
            else
                ECLJump(ecl, 0x7444, 0x7834, 60);
            ecl.SetFile(2);
            ECLSkipChapter(1);
            break;
        case THPrac::TH15::TH15_ST2_BOSS2:
            ECLJump(ecl, 0x7444, 0x7834, 60);
            ecl.SetFile(2);
            ECLSkipChapter(2);
            ECLJump(ecl, 0x40c, 0x4f4, 1); // Utilize Spell Practice Jump
            ecl << pair{0x504, 2200}; // Set Health
            ecl << pair{0x524, (int8_t)0x31}; // Set Spell Ordinal
            break;
        case THPrac::TH15::TH15_ST2_BOSS3:
            ECLJump(ecl, 0x7444, 0x7834, 60);
            ecl.SetFile(2);
            ECLSkipChapter(2);
            ecl << pair{0x644, (int8_t)0x32}; // Change Nonspell
            ecl << pair{0x1214, (int16_t)0} << pair{0x1344, (int16_t)0}; // Disable Item Drops & SE
            ecl << pair{0x14bc, 59} << pair{0x1508, 0} << pair{0x1120, 60}; // Change Move Time, Wait Time & Inv. Time
            break;
        case THPrac::TH15::TH15_ST2_BOSS4:
            ECLJump(ecl, 0x7444, 0x7834, 60);
            ecl.SetFile(2);
            ECLSkipChapter(2);
            ECLJump(ecl, 0x40c, 0x4f4, 1); // Utilize Spell Practice Jump
            ecl << pair{0x504, 2400}; // Set Health
            ecl << pair{0x524, (int8_t)0x32}; // Set Spell Ordinal
            break;
        case THPrac::TH15::TH15_ST2_BOSS5:
            ECLJump(ecl, 0x7444, 0x7834, 60);
            ecl.SetFile(2);
            ECLSkipChapter(2);
            ecl << pair{0x644, (int8_t)0x33}; // Change Nonspell
            ecl << pair{0x1ea8, (int16_t)0} << pair{0x1fd8, (int16_t)0}; // Disable Item Drops & SE
            ecl << pair{0x2150, 59} << pair{0x219c, 0} << pair{0x1db4, 60}; // Change Move Time, Wait Time & Inv. Time
            break;
        case THPrac::TH15::TH15_ST2_BOSS6:
            ECLJump(ecl, 0x7444, 0x7834, 60);
            ecl.SetFile(2);
            ECLSkipChapter(2);
            ECLJump(ecl, 0x40c, 0x4f4, 1); // Utilize Spell Practice Jump
            ecl << pair{0x504, 2500}; // Set Health
            ecl << pair{0x524, (int8_t)0x33}; // Set Spell Ordinal
            break;
        case THPrac::TH15::TH15_ST3_MID1:
            ECLJump(ecl, 0xa258, 0xa500, 60);
            ECLSkipChapter(1);
            break;
        case THPrac::TH15::TH15_ST3_MID2:
            ECLJump(ecl, 0xa258, 0xa528, 60);
            ecl.SetFile(3);
            ECLSkipChapter(1);
            ECLJump(ecl, 0x360, 0x47c, 0); // Utilize Spell Practice Jump
            ecl << pair{0x350, 60} << pair{0x48c, 60}; // Move Speed
            ecl << pair{0x4a4, 9999}; // Do not use spellcard
            ecl.SetPos(0x2a4);
            ecl << 0 << 0x001c0203 << 0x01ff0000 << 0 << 60; // Muteki
            break;
        case THPrac::TH15::TH15_ST3_BOSS1:
            if (thPracParam.dlg)
                ECLJump(ecl, 0xa258, 0xa600, 60);
            else
                ECLJump(ecl, 0xa258, 0xa614, 60);
            ecl.SetFile(2);
            ECLSkipChapter(1);
            break;
        case THPrac::TH15::TH15_ST3_BOSS2:
            ECLJump(ecl, 0xa258, 0xa614, 60);
            ecl.SetFile(2);
            ECLSkipChapter(2);
            ECLJump(ecl, 0x448, 0x530, 1); // Utilize Spell Practice Jump
            ecl << pair{0x540, 2000}; // Set Health
            ecl << pair{0x560, (int8_t)0x31}; // Set Spell Ordinal
            break;
        case THPrac::TH15::TH15_ST3_BOSS3:
            ECLJump(ecl, 0xa258, 0xa614, 60);
            ecl.SetFile(2);
            ECLSkipChapter(2);
            ecl << pair{0x680, (int8_t)0x32}; // Change Nonspell
            ecl << pair{0x15d8, (int16_t)0} << pair{0x1708, (int16_t)0}; // Disable Item Drops & SE
            ecl << pair{0x18ac, 0} << pair{0x14e4, 60}; // Change Wait Time & Inv. Time
            break;
        case THPrac::TH15::TH15_ST3_BOSS4:
            ECLJump(ecl, 0xa258, 0xa614, 60);
            ecl.SetFile(2);
            ECLSkipChapter(2);
            ECLJump(ecl, 0x448, 0x530, 1); // Utilize Spell Practice Jump
            ecl << pair{0x540, 2100}; // Set Health
            ecl << pair{0x560, (int8_t)0x32}; // Set Spell Ordinal
            break;
        case THPrac::TH15::TH15_ST3_BOSS5:
            ECLJump(ecl, 0xa258, 0xa614, 60);
            ecl.SetFile(2);
            ECLSkipChapter(2);
            ecl << pair{0x680, (int8_t)0x33}; // Change Nonspell
            ecl << pair{0x27b0, (int16_t)0} << pair{0x28e0, (int16_t)0}; // Disable Item Drops & SE
            ecl << pair{0x2a98, 0} << pair{0x2a58, 60}; // Change Wait Time & Inv. Time
            break;
        case THPrac::TH15::TH15_ST3_BOSS6:
            ECLJump(ecl, 0xa258, 0xa614, 60);
            ecl.SetFile(2);
            ECLSkipChapter(2);
            ECLJump(ecl, 0x448, 0x530, 1); // Utilize Spell Practice Jump
            ecl << pair{0x540, 2500}; // Set Health
            ecl << pair{0x560, (int8_t)0x33}; // Set Spell Ordinal
            break;
        case THPrac::TH15::TH15_ST3_BOSS7:
            ECLJump(ecl, 0xa258, 0xa614, 60);
            ecl.SetFile(2);
            ECLSkipChapter(2);
            ECLJump(ecl, 0x448, 0x530, 1); // Utilize Spell Practice Jump
            ecl << pair{0x540, 3000}; // Set Health
            ecl << pair{0x560, (int8_t)0x34}; // Set Spell Ordinal
            break;
        case THPrac::TH15::TH15_ST4_MID1:
            ECLJump(ecl, 0x860c, 0x88ec, 60);
            ECLSkipChapter(1);
            break;
        case THPrac::TH15::TH15_ST4_BOSS1:
            if (thPracParam.dlg)
                ECLJump(ecl, 0x860c, 0x899c, 60);
            else
                ECLJump(ecl, 0x860c, 0x89b0, 60);
            ecl.SetFile(2);
            ECLSkipChapter(1);
            break;
        case THPrac::TH15::TH15_ST4_BOSS2:
            ECLJump(ecl, 0x860c, 0x89b0, 60);
            ecl.SetFile(2);
            ECLSkipChapter(2);
            ECLJump(ecl, 0x41e0, 0x42c8, 1); // Utilize Spell Practice Jump
            ecl << pair{0x42d8, 2300}; // Set Health
            ecl << pair{0x42f8, (int8_t)0x31}; // Set Spell Ordinal
            break;
        case THPrac::TH15::TH15_ST4_BOSS3:
            ECLJump(ecl, 0x860c, 0x89b0, 60);
            ecl.SetFile(2);
            ECLSkipChapter(2);
            ecl << pair{0x4418, (int8_t)0x32}; // Change Nonspell
            ecl << pair{0x4f24, (int16_t)0} << pair{0x5054, (int16_t)0}; // Disable Item Drops & SE
            ecl << pair{0x51cc, 20} << pair{0x4e30, 0}; // Change Wait Time & Inv. Time
            break;
        case THPrac::TH15::TH15_ST4_BOSS4:
            ECLJump(ecl, 0x860c, 0x89b0, 60);
            ecl.SetFile(2);
            ECLSkipChapter(2);
            ECLJump(ecl, 0x41e0, 0x42c8, 1); // Utilize Spell Practice Jump
            ecl << pair{0x42d8, 3100}; // Set Health
            ecl << pair{0x42f8, (int8_t)0x32}; // Set Spell Ordinal
            break;
        case THPrac::TH15::TH15_ST4_BOSS5:
            ECLJump(ecl, 0x860c, 0x89b0, 60);
            ecl.SetFile(2);
            ECLSkipChapter(2);
            ecl << pair{0x4418, (int8_t)0x33}; // Change Nonspell
            ecl << pair{0x5e70, (int16_t)0} << pair{0x5d40, (int16_t)0}; // Disable Item Drops & SE
            ecl << pair{0x5ffc, 20} << pair{0x5fe8, 0}; // Change Wait Time & Inv. Time
            break;
        case THPrac::TH15::TH15_ST4_BOSS6:
            ECLJump(ecl, 0x860c, 0x89b0, 60);
            ecl.SetFile(2);
            ECLSkipChapter(2);
            ECLJump(ecl, 0x41e0, 0x42c8, 1); // Utilize Spell Practice Jump
            ecl << pair{0x42d8, 2500}; // Set Health
            ecl << pair{0x42f8, (int8_t)0x33}; // Set Spell Ordinal
            break;
        case THPrac::TH15::TH15_ST4_BOSS7:
            ECLJump(ecl, 0x860c, 0x89b0, 60);
            ecl.SetFile(2);
            ECLSkipChapter(2);
            ECLJump(ecl, 0x41e0, 0x42b4, 1); // Utilize Spell Practice Jump
            ecl.SetPos(0x42b4);
            ecl << 1 << 0x00140279 << 0x01ff0000 << 0 << 1;
            ecl << pair{0x42d8, 3400}; // Set Health
            ecl << pair{0x42f8, (int8_t)0x34}; // Set Spell Ordinal
            break;
        case THPrac::TH15::TH15_ST5_MID1:
            switch (thPracParam.phase) {
            case 1:
                ECLJump(ecl, 0x9f0c, 0xa1a0, 60);
                ECLJump(ecl, 0x70ec, 0x7100, 0);
                break;
            case 2:
                ECLJump(ecl, 0x9f0c, 0xa1a0, 60);
                ECLJump(ecl, 0x70ec, 0x716c, 0);
                break;
            case 3:
                ECLJump(ecl, 0x9f0c, 0xa1a0, 60);
                ECLJump(ecl, 0x70ec, 0x71ec, 0);
                break;
            case 4:
                ECLJump(ecl, 0x9f0c, 0xa1a0, 60);
                ECLJump(ecl, 0x70ec, 0x726c, 0);
                break;
            case 5:
                ECLJump(ecl, 0x9f0c, 0xa1a0, 60);
                ECLJump(ecl, 0x70ec, 0x72ec, 0);
                break;
            case 6:
                ECLJump(ecl, 0x9f0c, 0xa1a0, 60);
                ECLJump(ecl, 0x70ec, 0x736c, 0);
                break;

            default:
                ECLJump(ecl, 0x9f0c, 0xa180, 60);
                ECLJump(ecl, 0x6ed8, 0x7074, 0);
                break;
            }
            ECLSkipChapter(1);
            break;
        case THPrac::TH15::TH15_ST5_BOSS1:
            if (thPracParam.dlg)
                ECLJump(ecl, 0x9f0c, 0xa20c, 60);
            else
                ECLJump(ecl, 0x9f0c, 0xa220, 60);
            ecl.SetFile(3);
            ECLSkipChapter(1);
            break;
        case THPrac::TH15::TH15_ST5_BOSS2:
            ECLJump(ecl, 0x9f0c, 0xa220, 60);
            ecl.SetFile(3);
            ECLSkipChapter(2);
            ECLJump(ecl, 0x528, 0x610, 0); // Utilize Spell Practice Jump
            ecl << pair{0x620, 1}; // Set Health
            ecl << pair{0x640, (int8_t)0x31}; // Set Spell Ordinal
            break;
        case THPrac::TH15::TH15_ST5_BOSS3:
            ECLJump(ecl, 0x9f0c, 0xa220, 60);
            ecl.SetFile(3);
            ECLSkipChapter(2);
            ecl << pair{0x760, (int8_t)0x32}; // Change Nonspell
            ecl << pair{0x1394, (int16_t)0} << pair{0x14c4, (int16_t)0}; // Disable Item Drops & SE
            ecl << pair{0x163c, 59} << pair{0x165c, 0} << pair{0x12ac, 0}; // Change Move Time, Wait Time & Inv. Time
            break;
        case THPrac::TH15::TH15_ST5_BOSS4:
            ECLJump(ecl, 0x9f0c, 0xa220, 60);
            ecl.SetFile(3);
            ECLSkipChapter(2);
            ECLJump(ecl, 0x528, 0x610, 0); // Utilize Spell Practice Jump
            ecl << pair{0x620, 3400}; // Set Health
            ecl << pair{0x640, (int8_t)0x32}; // Set Spell Ordinal
            break;
        case THPrac::TH15::TH15_ST5_BOSS5:
            ECLJump(ecl, 0x9f0c, 0xa220, 60);
            ecl.SetFile(3);
            ECLSkipChapter(2);
            ecl << pair{0x760, (int8_t)0x33}; // Change Nonspell
            ecl << pair{0x2744, (int16_t)0} << pair{0x2874, (int16_t)0}; // Disable Item Drops & SE
            ecl << pair{0x29ec, 59} << pair{0x2a0c, 0}; // Change Move Time & Wait Time
            break;
        case THPrac::TH15::TH15_ST5_BOSS6:
            ECLJump(ecl, 0x9f0c, 0xa220, 60);
            ecl.SetFile(3);
            ECLSkipChapter(2);
            ECLJump(ecl, 0x528, 0x610, 0); // Utilize Spell Practice Jump
            ecl << pair{0x620, 2320}; // Set Health
            ecl << pair{0x640, (int8_t)0x33}; // Set Spell Ordinal
            break;
        case THPrac::TH15::TH15_ST5_BOSS7:
            ECLJump(ecl, 0x9f0c, 0xa220, 60);
            ecl.SetFile(3);
            ECLSkipChapter(2);
            ECLJump(ecl, 0x528, 0x610, 0); // Utilize Spell Practice Jump
            ecl << pair{0x620, 3001}; // Set Health
            ecl << pair{0x640, (int8_t)0x34}; // Set Spell Ordinal
            break;
        case THPrac::TH15::TH15_ST5_BOSS8:
            ECLJump(ecl, 0x9f0c, 0xa220, 60);
            ecl.SetFile(3);
            ECLSkipChapter(2);
            ECLJump(ecl, 0x528, 0x610, 0); // Utilize Spell Practice Jump
            ecl << pair{0x620, 1}; // Set Health
            ecl << pair{0x640, (int8_t)0x35}; // Set Spell Ordinal
            break;
        case THPrac::TH15::TH15_ST6_STARS:
            ECLJump(ecl, 0x91d0, 0x93e0, 0);
            ECLJump(ecl, 0x2b34, 0x2c40, 60);
            ECLSkipChapter(1);
            ECLStarsBGMSync();
            break;
        case THPrac::TH15::TH15_ST6_BOSS1:
            if (thPracParam.dlg)
                ECLJump(ecl, 0x91d0, 0x94ec, 60);
            else
                ECLJump(ecl, 0x91d0, 0x9500, 60);
            ECLSkipChapter(1);
            break;
        case THPrac::TH15::TH15_ST6_BOSS2:
            ECLJump(ecl, 0x91d0, 0x9500, 60);
            ecl.SetFile(2);
            ECLSkipChapter(2);
            ECLJump(ecl, 0x5a0, 0x688, 0); // Utilize Spell Practice Jump
            ecl << pair{0x698, 4000}; // Set Health
            ecl << pair{0x6b8, (int8_t)0x31}; // Set Spell Ordinal
            break;
        case THPrac::TH15::TH15_ST6_BOSS3:
            ECLJump(ecl, 0x91d0, 0x9500, 60);
            ecl.SetFile(2);
            ECLSkipChapter(2);
            ecl << pair{0x7d8, (int8_t)0x32}; // Change Nonspell
            ecl << pair{0x1274, (int16_t)0} << pair{0x13a4, (int16_t)0}; // Disable Item Drops & SE
            ecl << pair{0x1528, 0} << pair{0x1178, 0}; // Change Wait Time & Inv. Time
            break;
        case THPrac::TH15::TH15_ST6_BOSS4:
            ECLJump(ecl, 0x91d0, 0x9500, 60);
            ecl.SetFile(2);
            ECLSkipChapter(2);
            ECLJump(ecl, 0x5a0, 0x688, 0); // Utilize Spell Practice Jump
            ecl << pair{0x698, 2600}; // Set Health
            ecl << pair{0x6b8, (int8_t)0x32}; // Set Spell Ordinal
            break;
        case THPrac::TH15::TH15_ST6_BOSS5:
            ECLJump(ecl, 0x91d0, 0x9500, 60);
            ecl.SetFile(2);
            ECLSkipChapter(2);
            ecl << pair{0x7d8, (int8_t)0x33}; // Change Nonspell
            ecl << pair{0x1f84, (int16_t)0} << pair{0x20b4, (int16_t)0}; // Disable Item Drops & SE
            break;
        case THPrac::TH15::TH15_ST6_BOSS6:
            ECLJump(ecl, 0x91d0, 0x9500, 60);
            ecl.SetFile(2);
            ECLSkipChapter(2);
            ECLJump(ecl, 0x5a0, 0x63c, 0); // Utilize Spell Practice Jump
            ecl << pair{0x698, 4000}; // Set Health
            ecl << pair{0x6b8, (int8_t)0x33}; // Set Spell Ordinal
            ecl.SetPos(0x63c); // Boss Effect
            ecl << 0 << 0x0014012e << 0x01ff0000 << 0 << 3
                << 0 << 0x0018012f << 0x02ff0000 << 0 << 3 << 6;
            ecl << 0 << 0x0020000f << 0x01ff0000 << 0 << 0xc
                << 0x73736f42 << 0x6f705f33 << 0x00000073; // "Boss", "3_po", "s"
            break;
        case THPrac::TH15::TH15_ST6_BOSS7:
            ECLJump(ecl, 0x91d0, 0x9500, 60);
            ecl.SetFile(2);
            ECLSkipChapter(2);
            ecl << pair{0x7d8, (int8_t)0x34}; // Change Nonspell
            ecl << pair{0x302c, (int16_t)0} << pair{0x315c, (int16_t)0}; // Disable Item Drops & SE
            break;
        case THPrac::TH15::TH15_ST6_BOSS8:
            ECLJump(ecl, 0x91d0, 0x9500, 60);
            ecl.SetFile(2);
            ECLSkipChapter(2);
            ECLJump(ecl, 0x5a0, 0x63c, 0); // Utilize Spell Practice Jump
            ecl << pair{0x698, 3700}; // Set Health
            ecl << pair{0x6b8, (int8_t)0x34}; // Set Spell Ordinal
            ecl.SetPos(0x63c); // Boss Effect
            ecl << 0 << 0x0014012e << 0x01ff0000 << 0 << 3
                << 0 << 0x0018012f << 0x02ff0000 << 0 << 3 << 6;
            ecl << 0 << 0x0020000f << 0x01ff0000 << 0 << 0xc
                << 0x73736f42 << 0x6f705f34 << 0x00000073; // "Boss", "4_po", "s"
            break;
        case THPrac::TH15::TH15_ST6_BOSS9:
            ECLJump(ecl, 0x91d0, 0x9500, 60);
            ecl.SetFile(2);
            ECLSkipChapter(2);
            ECLJump(ecl, 0x5a0, 0x688, 0); // Utilize Spell Practice Jump
            ecl << pair{0x698, 5000}; // Set Health
            ecl << pair{0x6b8, (int8_t)0x35}; // Set Spell Ordinal
            break;
        case THPrac::TH15::TH15_ST6_BOSS10:
            ECLJump(ecl, 0x91d0, 0x9500, 60);
            ecl.SetFile(2);
            ECLSkipChapter(2);
            ECLJump(ecl, 0x5a0, 0x688, 0); // Utilize Spell Practice Jump
            ecl << pair{0x698, 6000}; // Set Health
            ecl << pair{0x6b8, (int8_t)0x36}; // Set Spell Ordinal
            break;
        case THPrac::TH15::TH15_ST6_BOSS11:
            ECLJump(ecl, 0x91d0, 0x9500, 60);
            ecl.SetFile(2);
            ECLSkipChapter(2);
            ECLJump(ecl, 0x5a0, 0x63c, 0); // Utilize Spell Practice Jump
            ecl << pair{0x698, 8000}; // Set Health
            ecl << pair{0x6b8, (int8_t)0x37}; // Set Spell Ordinal
            ecl.SetPos(0x63c); // Boss Effect
            ecl << 0 << 0x0014012e << 0x01ff0000 << 0 << 3
                << 0 << 0x0018012f << 0x02ff0000 << 0 << 3 << 6;
            ecl << 0 << 0x0020000f << 0x01ff0000 << 0 << 0xc
                << 0x73736f42 << 0x6f705f34 << 0x00000073; // "Boss", "4_po", "s"

            if (thPracParam.phase) {
                if (thPracParam.phase == 1)
                    ecl << pair{0x698, 5500};
                else if (thPracParam.phase == 2)
                    ecl << pair{0x698, 3500};
                else if (thPracParam.phase == 3)
                    ecl << pair{0x698, 1200};
            }
            break;
        case THPrac::TH15::TH15_ST7_MID1:
            if (thPracParam.dlg)
                ECLJump(ecl, 0x8f6c, 0x9270, 60);
            else {
                ECLJump(ecl, 0x8f6c, 0x92a8, 60);
                ECLSkipChapter(1);
                ecl.SetFile(2);

                ecl.SetPos(0x34c); // Inv. Time
                ecl << 0 << 0x00200203 << 0x01ff0000 << 0 << 60;
                ecl.SetPos(0x37c); // Boss Movement Restriction
                ecl << 0 << 0x002c01f8 << 0x04ff0000 << 0
                    << 0x0 << 0x43000000 << 0x438c0000 << 0x43800000;
                ecl << pair{0x33c, (int16_t)0}; // Void Wait
                ecl << pair{0x36c, 60} << pair{0x328, 60}; // Wait Time & Move Time
                ECLJump(ecl, 0x48c, 0x508, 0); // Spell Jump
                ecl << pair{0x2fc, 2200} << pair{0x525, (int8_t)0x31}; // Spell Health & Spell Ordinal
            }
            break;
        case THPrac::TH15::TH15_ST7_MID2:
            ECLJump(ecl, 0x8f6c, 0x92a8, 60);
            ECLSkipChapter(1);
            ecl.SetFile(2);

            ecl.SetPos(0x34c); // Inv. Time
            ecl << 0 << 0x00200203 << 0x01ff0000 << 0 << 60;
            ecl.SetPos(0x37c); // Boss Movement Restriction
            ecl << 0 << 0x002c01f8 << 0x04ff0000 << 0
                << 0x0 << 0x43000000 << 0x438c0000 << 0x43800000;
            ecl << pair{0x33c, (int16_t)0}; // Void Wait
            ecl << pair{0x36c, 60} << pair{0x328, 60}; // Wait Time & Move Time
            ECLJump(ecl, 0x48c, 0x508, 0); // Spell Jump
            ecl << pair{0x2fc, 2200} << pair{0x525, (int8_t)0x32}; // Spell Health & Spell Ordinal
            break;
        case THPrac::TH15::TH15_ST7_MID3:
            ECLJump(ecl, 0x8f6c, 0x92a8, 60);
            ECLSkipChapter(1);
            ecl.SetFile(2);

            ecl.SetPos(0x34c); // Inv. Time
            ecl << 0 << 0x00200203 << 0x01ff0000 << 0 << 60;
            ecl.SetPos(0x37c); // Boss Movement Restriction
            ecl << 0 << 0x002c01f8 << 0x04ff0000 << 0
                << 0x0 << 0x43000000 << 0x438c0000 << 0x43800000;
            ecl << pair{0x33c, (int16_t)0}; // Void Wait
            ecl << pair{0x36c, 60} << pair{0x328, 60}; // Wait Time & Move Time
            ECLJump(ecl, 0x48c, 0x508, 0); // Spell Jump
            ecl << pair{0x2fc, 3000} << pair{0x525, (int8_t)0x33}; // Spell Health & Spell Ordinal
            break;
        case THPrac::TH15::TH15_ST7_END_NS1:
            if (thPracParam.dlg)
                ECLJump(ecl, 0x8f6c, 0x93b8, 60);
            else {
                th15_st7boss1_chapter_bonus::GetHook().Enable();
                ECLJump(ecl, 0x8f6c, 0x93cc, 60);
                st7_hide_subboss();
            }
            ECLSkipChapter(1);
            break;
        case THPrac::TH15::TH15_ST7_END_S1:
            ECLJump(ecl, 0x8f6c, 0x93cc, 60);
            ECLSkipChapter(2);
            st7_hide_subboss();
            st7_enter_spell(0x31, 3000);
            break;
        case THPrac::TH15::TH15_ST7_END_NS2:
            ECLJump(ecl, 0x8f6c, 0x93cc, 60);
            ECLSkipChapter(2);
            st7_hide_subboss();

            ecl.SetFile(3);
            ecl << pair{0x708, 0x32}; // Change Nonspell
            ecl << pair{0x16f4, (int16_t)0} << pair{0x1824, (int16_t)0}; // Disable Item Drops & SE
            ecl << pair{0x199c, 59} << pair{0x19bc, 0} << pair{0x15dc, 0}; // Change Move Time, Wait Time & Inv. Time

            ecl.SetFile(3);
            ecl << pair{0x5d8, (int16_t)0} << pair{0x191c, (int16_t)0}; // Void 303-0 & 306-0
            ecl << pair{0x594, 14} << pair{0x6a0, 120}; // Change 306-0 & 303-2
            ECLJump(ecl, 0x1a90, 0x1ae8, 24); // Jump Over Sprite Change
            break;
        case THPrac::TH15::TH15_ST7_END_S2:
            ECLJump(ecl, 0x8f6c, 0x93cc, 60);
            ECLSkipChapter(2);
            st7_hide_subboss();
            st7_enter_spell(0x32, 3400);

            ecl.SetFile(3);
            ecl << pair{0x5d8, (int16_t)0}; // Void 303-0
            ecl << pair{0x594, 14} << pair{0x6a0, 120}; // Change 306-0 & 303-2
            break;
        case THPrac::TH15::TH15_ST7_END_NS3:
            ECLJump(ecl, 0x8f6c, 0x93cc, 60);
            ECLSkipChapter(2);
            st7_hide_subboss();

            ecl.SetFile(3);
            ecl << pair{0x708, 0x33}; // Change Nonspell
            ecl << pair{0x2430, (int16_t)0} << pair{0x2560, (int16_t)0}; // Disable Item Drops & SE
            ecl << pair{0x26d8, 59} << pair{0x26f8, 0} << pair{0x2318, 0}; // Change Move Time, Wait Time & Inv. Time

            ecl.SetFile(3);
            ecl << pair{0x5d8, (int16_t)0} << pair{0x2658, (int16_t)0}; // Void 303-0 & 306-0
            ecl << pair{0x594, 7} << pair{0x6a0, 132}; // Change 306-0 & 303-2
            ECLJump(ecl, 0x276c, 0x2798, 24); // Jump Over Sprite Change
            break;
        case THPrac::TH15::TH15_ST7_END_S3:
            ECLJump(ecl, 0x8f6c, 0x93cc, 60);
            ECLSkipChapter(2);
            st7_hide_subboss();
            st7_enter_spell(0x33, 3000);

            ecl.SetFile(3);
            ecl << pair{0x5d8, (int16_t)0}; // Void 303-0
            ecl << pair{0x594, 7} << pair{0x6a0, 132}; // Change 306-0 & 303-2
            break;
        case THPrac::TH15::TH15_ST7_END_NS4:
            ECLJump(ecl, 0x8f6c, 0x93cc, 60);
            ECLSkipChapter(2);
            st7_hide_subboss();

            ecl.SetFile(3);
            ecl << pair{0x708, 0x34}; // Change Nonspell
            ecl << pair{0x32a0, (int16_t)0}; // Don't Activate Sub Boss
            ecl << pair{0x32e8, (int16_t)0} << pair{0x3418, (int16_t)0}; // Disable Item Drops & SE
            ECLJump(ecl, 0x3548, 0x35b0, 0); // Skip Move
            ecl << pair{0x31c4, 10}; // Inv. Time

            ecl.SetFile(3);
            ecl << pair{0x5d8, (int16_t)0} << pair{0x3510, (int16_t)0}; // Void 303-0 & 306-0
            ecl << pair{0x594, 0} << pair{0x6a0, 116}; // Change 306-0 & 303-2
            ecl << pair{0x57c, 5}; // Change 302
            break;
        case THPrac::TH15::TH15_ST7_END_S4:
            ECLJump(ecl, 0x8f6c, 0x93cc, 60);
            ECLSkipChapter(2);
            st7_hide_subboss();
            st7_enter_spell(0x34, 2000, true);

            ecl.SetFile(3);
            ecl << pair{0x5d8, (int16_t)0}; // Void 303-0
            ecl << pair{0x594, 0} << pair{0x6a0, 116}; // Change 306-0 & 303-2
            ecl << pair{0x57c, 5}; // Change 302
            break;
        case THPrac::TH15::TH15_ST7_END_NS5:
            ECLJump(ecl, 0x8f6c, 0x93cc, 60);
            ECLSkipChapter(2);
            st7_hide_subboss();

            ecl.SetFile(3);
            ecl << pair{0x708, 0x35}; // Change Nonspell
            ecl << pair{0x4464, (int16_t)0}; // Don't Activate Sub Boss
            ecl << pair{0x44c0, (int16_t)0} << pair{0x45f0, (int16_t)0}; // Disable Item Drops & SE
            ECLJump(ecl, 0x4720, 0x4788, 0); // Skip Move
            ecl << pair{0x4388, 0}; // Inv. Time

            ecl.SetFile(3);
            ecl << pair{0x46e8, (int16_t)0}; // Void 306-0
            break;
        case THPrac::TH15::TH15_ST7_END_S5:
            ECLJump(ecl, 0x8f6c, 0x93cc, 60);
            ECLSkipChapter(2);
            st7_hide_subboss();
            st7_enter_spell(0x35, 3500);
            break;
        case THPrac::TH15::TH15_ST7_END_NS6:
            ECLJump(ecl, 0x8f6c, 0x93cc, 60);
            ECLSkipChapter(2);
            st7_hide_subboss();

            ecl.SetFile(3);
            ecl << pair{0x708, 0x36}; // Change Nonspell
            ecl << pair{0x5368, (int16_t)0} << pair{0x5498, (int16_t)0}; // Disable Item Drops & SE
            ecl << pair{0x5610, 59} << pair{0x5630, 0} << pair{0x5250, 0}; // Change Move Time, Wait Time & Inv. Time

            ecl.SetFile(3);
            ecl << pair{0x5d8, (int16_t)0} << pair{0x5590, (int16_t)0}; // Void 303-0 & 306-0
            ecl << pair{0x594, 14} << pair{0x6a0, 120}; // Change 306-0 & 303-2
            ECLJump(ecl, 0x5694, 0x56ec, 24); // Jump Over Sprite Change
            break;
        case THPrac::TH15::TH15_ST7_END_S6:
            ECLJump(ecl, 0x8f6c, 0x93cc, 60);
            ECLSkipChapter(2);
            st7_hide_subboss();
            st7_enter_spell(0x36, 3500);
            ecl << pair{0xd898, (int16_t)0}; // Void 316

            ecl.SetFile(3);
            ecl << pair{0x5d8, (int16_t)0}; // Void 303-0
            ecl << pair{0x594, 14} << pair{0x6a0, 120}; // Change 306-0 & 303-2
            break;
        case THPrac::TH15::TH15_ST7_END_NS7:
            ECLJump(ecl, 0x8f6c, 0x93cc, 60);
            ECLSkipChapter(2);
            st7_hide_subboss();

            ecl.SetFile(3);
            ecl << pair{0x708, 0x37}; // Change Nonspell
            ecl << pair{0x6198, (int16_t)0} << pair{0x62c8, (int16_t)0}; // Disable Item Drops & SE
            ecl << pair{0x6440, 59} << pair{0x6460, 0} << pair{0x6080, 0}; // Change Move Time, Wait Time & Inv. Time

            ecl.SetFile(3);
            ecl << pair{0x5d8, (int16_t)0} << pair{0x63c0, (int16_t)0}; // Void 303-0 & 306-0
            ecl << pair{0x594, 7} << pair{0x6a0, 132}; // Change 306-0 & 303-2
            ECLJump(ecl, 0x64d4, 0x652c, 24); // Jump Over Sprite Change
            break;
        case THPrac::TH15::TH15_ST7_END_S7:
            ECLJump(ecl, 0x8f6c, 0x93cc, 60);
            ECLSkipChapter(2);
            st7_hide_subboss();
            st7_enter_spell(0x37, 3000);

            ecl.SetFile(3);
            ecl << pair{0x5d8, (int16_t)0}; // Void 303-0
            ecl << pair{0x594, 7} << pair{0x6a0, 132}; // Change 306-0 & 303-2
            break;
        case THPrac::TH15::TH15_ST7_END_NS8:
            ECLJump(ecl, 0x8f6c, 0x93cc, 60);
            ECLSkipChapter(2);
            st7_hide_subboss();

            ecl.SetFile(3);
            ecl << pair{0x708, 0x38}; // Change Nonspell
            ecl << pair{0x7004, (int16_t)0}; // Don't Activate Sub Boss
            ecl << pair{0x708c, (int16_t)0} << pair{0x71bc, (int16_t)0}; // Disable Item Drops & SE
            ECLJump(ecl, 0x72ec, 0x7354, 0); // Skip Move
            ecl << pair{0x6f54, 0}; // Inv. Time

            ecl.SetFile(3);
            ecl << pair{0x5d8, (int16_t)0} << pair{0x72b4, (int16_t)0}; // Void 303-0 & 306-0
            ecl << pair{0x594, 0} << pair{0x6a0, 116}; // Change 306-0 & 303-2
            ecl << pair{0x57c, 5}; // Change 302
            break;
        case THPrac::TH15::TH15_ST7_END_S8:
            ECLJump(ecl, 0x8f6c, 0x93cc, 60);
            ECLSkipChapter(2);
            st7_hide_subboss();
            st7_enter_spell(0x38, 3000, true);

            ecl.SetFile(3);
            ecl << pair{0x5d8, (int16_t)0}; // Void 303-0
            ecl << pair{0x594, 0} << pair{0x6a0, 116}; // Change 306-0 & 303-2
            ecl << pair{0x57c, 5}; // Change 302
            break;
        case THPrac::TH15::TH15_ST7_END_S9:
            ECLJump(ecl, 0x8f6c, 0x93cc, 60);
            ECLSkipChapter(2);
            st7_hide_subboss();

            ecl.SetFile(3);
            ecl << pair{0x4a8, 0} << pair{0x4ac, 0x43600000}; // Pos
            ecl << pair{0x5d8, (int16_t)0}; // Void 303-0
            ecl << pair{0x57c, 5} << pair{0x6b4, 5}; // Change 302
            ecl << pair{0x594, 0} << pair(0x688, -1) << pair(0x6a0, -1); // Change 306-0, 303-1 & 303-2
            ecl << pair{0x644, (int16_t)0} << pair{0x584, (int16_t)0}; // Void 504, 303

            ecl.SetFile(3);
            ecl.SetPos(0x6d0);
            // 306
            ecl << 0 << 0x00180132 << 0x02ff0000 << 0 << 0 << 0;
            // Flag
            ecl << 0 << 0x001401f6 << 0x01ff0000 << 0 << 0x20;
            // Health
            ecl << 0 << 0x001401ff << 0x01ff0000 << 0 << 3000;
            // Card
            ecl << 0 << 0x0020000b << 0x01ff0000 << 0 << 0xc
                << 0x73736f42 << 0x64726143 << 0x39;
            break;
        case THPrac::TH15::TH15_ST7_END_S10:
            ECLJump(ecl, 0x8f6c, 0x93cc, 60);
            ECLSkipChapter(2);

            // Hecatia
            ecl.SetFile(3);
            ecl << pair{0x4a8, 0xc2800000} << pair{0x4ac, 0x43000000}; // Pos
            ecl << pair{0x644, (int16_t)0}; // Void 504
            ecl << pair(0x688, -1) << pair(0x6a0, -1); // Change 303-1 & 303-2
            ecl.SetPos(0x6d0);
            // Effect
            ecl << 5 << 0x0014012e << 0x01ff0000 << 0 << 5
                << 5 << 0x0018012f << 0x02ff0000 << 0 << 3 << 6;
            ecl << 5 << 0x0020000f << 0x01ff0000 << 0 << 0xc
                << 0x73736f42 << 0x6f705f34 << 0x00000073; // "Boss", "4_po", "s"
            ecl << 5 << 0x0014012e << 0x01ff0000 << 0 << 3;
            // Health
            ecl << 5 << 0x001401ff << 0x01ff0000 << 0 << 7000;
            // Card
            ecl << 6 << 0x0020000b << 0x01ff0000 << 0 << 0xc
                << 0x73736f42 << 0x64726143 << 0x3031;

            // Junko
            ecl.SetFile(4);
            ecl << pair{0x184, 0x42800000} << pair{0x188, 0x43000000}; // Pos
            ecl << pair{0x328, (int16_t)0}; // Void 504
            ecl << pair(0x36c, -1) << pair(0x384, -1); // Change 303-1 & 303-2
            ecl << pair{0x2b4, (int16_t)0}; // Void 401
            ecl.SetPos(0x3b4);
            // Health
            ecl << 5 << 0x001401ff << 0x01ff0000 << 0 << 7000;
            // Inv.
            ecl << 5 << 0x00140203 << 0x01ff0000 << 0 << 14;
            // Stall
            ecl << 9999 << 0x00100000 << 0x00ff0000 << 0;

            switch (thPracParam.phase) {
            case 1:
                ecl.SetFile(3);
                ECLJump(ecl, 0x8f30, 0x8fc8, 94);
                ecl.SetFile(4);
                ECLJump(ecl, 0xb08, 0xba0, 0);
                break;
            case 2:
                ecl.SetFile(3);
                ECLJump(ecl, 0x8f30, 0x8fc8, 94);
                break;
            case 3:
                ecl.SetFile(4);
                ECLJump(ecl, 0xb08, 0xba0, 0);
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
        ecl.SetBaseAddr((void*)GetMemAddr(0x4e9a80, 0x17c, 0xC));

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
        ReplaySaveParam(mb_to_utf16(repName, 932).c_str(), thPracParam.GetJson());
    }

    static bool frameStarted = false;

    HOOKSET_DEFINE(THMainHook)
    EHOOK_DY(th15_everlasting_bgm, 0x476f10)
    {
        int32_t retn_addr = ((int32_t*)pCtx->Esp)[0];
        int32_t bgm_cmd = ((int32_t*)pCtx->Esp)[1];
        int32_t bgm_id = ((int32_t*)pCtx->Esp)[2];
        // 4th stack item = i32 call_addr

        bool el_switch;
        bool is_practice;
        bool result;

        el_switch = *(THOverlay::singleton().mElBgm) && !THGuiRep::singleton().mRepStatus && thPracParam.mode && thPracParam.section;
        is_practice = (*((int32_t*)0x4e7794) & 0x1);
        if (thPracParam.mode && thPracParam.section == TH15_ST6_STARS)
            el_switch = false;
        result = ElBgmTest<0x44d413, 0x43cb40, 0x450efd, 0x452a63, 0xffffffff>(
            el_switch, is_practice, retn_addr, bgm_cmd, bgm_id, 0xffffffff);

        if (result) {
            pCtx->Eip = 0x476fa3;
        }
    }
    EHOOK_DY(th15_param_reset, 0x461070)
    {
        thPracParam.Reset();
    }
    EHOOK_DY(th15_prac_menu_1, 0x4677be)
    {
        THGuiPrac::singleton().State(1);
    }
    EHOOK_DY(th15_prac_menu_2, 0x4677e1)
    {
        THGuiPrac::singleton().State(2);
    }
    EHOOK_DY(th15_prac_menu_3, 0x467b2a)
    {
        THGuiPrac::singleton().State(3);
    }
    EHOOK_DY(th15_prac_menu_4, 0x467bcb)
    {
        THGuiPrac::singleton().State(4);
    }
    PATCH_DY(th15_prac_menu_enter_1, 0x4678a2, "\xeb", 1);
    EHOOK_DY(th15_prac_menu_enter_2, 0x467b85)
    {
        pCtx->Ecx = thPracParam.stage;
    }
    EHOOK_DY(th15_disable_prac_menu_1, 0x467d31)
    {
        pCtx->Eip = 0x467d6e;
    }
    EHOOK_DY(th15_menu_rank_fix, 0x4527be)
    {
        *((int32_t*)0x4e7424) = -1;
        *((int32_t*)0x4e7410) = *((int32_t*)0x4e0ef4);
    }
    EHOOK_DY(th15_patch_main, 0x43c68c)
    {
        th15_chapter_set::GetHook().Disable();
        th15_chapter_disable::GetHook().Disable();
        th15_stars_bgm_sync::GetHook().Disable();
        if (thPracParam.mode == 1) {
            *(int32_t*)(0x4E740C) = (int32_t)(thPracParam.score / 10);
            *(int32_t*)(0x4E7450) = thPracParam.life;
            *(int32_t*)(0x4E7454) = thPracParam.life_fragment;
            *(int32_t*)(0x4E745C) = thPracParam.bomb;
            *(int32_t*)(0x4E7460) = thPracParam.bomb_fragment;
            *(int32_t*)(0x4E7440) = thPracParam.power;
            *(int32_t*)(0x4E7434) = thPracParam.value * 100;
            *(int32_t*)(0x4E741C) = thPracParam.graze; // 0x4E7420: Chapter Graze

            THSectionPatch();
        }
        thPracParam._playLock = true;
    }
    EHOOK_DY(th15_bgm, 0x43d5c1)
    {
        if (THBGMTest()) {
            PushHelper32(pCtx, 1);
            pCtx->Eip = 0x43d5c3;
        }
    }
    EHOOK_DY(th15_rep_save, 0x45cc49)
    {
        char* repName = (char*)(pCtx->Esp + 0x38);
        if (thPracParam.mode)
            THSaveReplay(repName);
    }
    EHOOK_DY(th15_rep_menu_1, 0x468197)
    {
        THGuiRep::singleton().State(1);
    }
    EHOOK_DY(th15_rep_menu_2, 0x4682b0)
    {
        THGuiRep::singleton().State(2);
    }
    EHOOK_DY(th15_rep_menu_3, 0x468474)
    {
        THGuiRep::singleton().State(3);
    }
    EHOOK_DY(th15_update, 0x4015fa)
    {
        GameGuiBegin(IMPL_WIN32_DX9, !THAdvOptWnd::singleton().IsOpen());

        // Gui components update
        THGuiPrac::singleton().Update();
        THGuiRep::singleton().Update();
        THOverlay::singleton().Update();
        TH15InGameInfo::singleton().Update();
        bool drawCursor = THAdvOptWnd::StaticUpdate() || THGuiPrac::singleton().IsOpen();
        GameGuiEnd(drawCursor);
    }
    EHOOK_DY(th15_render, 0x40170a)
    {
        GameGuiRender(IMPL_WIN32_DX9);
    }
    HOOKSET_ENDDEF()

    HOOKSET_DEFINE(THInitHook)
    static __declspec(noinline) void THGuiCreate()
    {
        // Init
        GameGuiInit(IMPL_WIN32_DX9, 0x4e77d8, 0x519bb0, 0x471f10,
            Gui::INGAGME_INPUT_GEN2, 0x4e6d1c, 0x4e6d18, 0,
            (*((int32_t*)0x51bbec) >> 2) & 0xf);

        // Gui components creation
        THGuiPrac::singleton();
        THGuiRep::singleton();
        THOverlay::singleton();
        TH15InGameInfo::singleton();

        // Hooks
        THMainHook::singleton().EnableAllHooks();

        // Reset thPracParam
        thPracParam.Reset();
    }
    static __declspec(noinline) void THInitHookDisable()
    {
        auto& s = THInitHook::singleton();
        s.th15_gui_init_1.Disable();
        s.th15_gui_init_2.Disable();
    }
    PATCH_DY(th15_disable_demo, 0x460900, "\xff\xff\xff\x7f", 4);
    EHOOK_DY(th15_disable_mutex, 0x4713ec)
    {
        pCtx->Eip = 0x4715a9;
    }
    PATCH_DY(th15_startup_1, 0x46055f, "\x90\x90", 2);
    PATCH_DY(th15_startup_2, 0x4610c3, "\xeb", 1);
    EHOOK_DY(th15_gui_init_1, 0x4617ff)
    {
        THGuiCreate();
        THInitHookDisable();
    }
    EHOOK_DY(th15_gui_init_2, 0x47341b)
    {
        THGuiCreate();
        THInitHookDisable();
    }

#pragma region igi
    EHOOK_DY(th15_game_start, 0x43E6EE) // gamestart-bomb set
    {
        TH15InGameInfo::singleton().mBombCount = 0;
        TH15InGameInfo::singleton().mMissCount = 0;
    }
    EHOOK_DY(th15_bomb_dec, 0x41497A) // bomb dec
    {
        TH15InGameInfo::singleton().mBombCount++;
    }
    EHOOK_DY(th15_life_dec, 0x456398) // life dec
    {
        TH15InGameInfo::singleton().mMissCount++;
    }
#pragma endregion

    
    HOOKSET_ENDDEF()
}

void TH15Init()
{
    TH15::THInitHook::singleton().EnableAllHooks();
    TryKeepUpRefreshRate((void*)0x47356c, (void*)0x47333d);
    if (GetModuleHandleA("vpatch_th15.dll")) {
        TryKeepUpRefreshRate((void*)((DWORD)GetModuleHandleA("vpatch_th15.dll") + 0x6bd9));
    }
}
}
