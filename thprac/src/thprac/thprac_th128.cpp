#include "thprac_utils.h"

namespace THPrac {
namespace TH128 {
    using std::pair;
    struct THPracParam {
        int32_t mode;
        int32_t stage;
        int32_t section;
        int32_t phase;

        int64_t score;
        int32_t motivation;
        int32_t perfect_freeze;
        int32_t ice_power;
        int32_t ice_area;

        bool dlg;

        bool _playLock = false;
        void Reset()
        {
            memset(this, 0, sizeof(THPracParam));
        }
        bool ReadJson(std::string& json)
        {
            ParseJson();

            ForceJsonValue(game, "th128");
            GetJsonValue(mode);
            GetJsonValue(stage);
            GetJsonValue(section);
            GetJsonValue(phase);
            GetJsonValueEx(dlg, Bool);

            GetJsonValue(score);
            GetJsonValue(motivation);
            GetJsonValue(perfect_freeze);
            GetJsonValue(ice_power);
            GetJsonValue(ice_area);

            return true;
        }
        std::string GetJson()
        {
            CreateJson();

            AddJsonValueEx(version, GetVersionStr(), jalloc);
            AddJsonValueEx(game, "th128", jalloc);
            AddJsonValue(mode);
            AddJsonValue(stage);
            if (section)
                AddJsonValue(section);
            if (phase)
                AddJsonValue(phase);
            if (dlg)
                AddJsonValue(dlg);

            AddJsonValue(score);
            AddJsonValue(motivation);
            AddJsonValue(perfect_freeze);
            AddJsonValue(ice_power);
            AddJsonValue(ice_area);

            ReturnJson();
        }
    };
    THPracParam thPracParam {};
    bool thLock { false };
    bool thHardLock { false };

    class THGuiPrac : public Gui::GameGuiWnd {
        THGuiPrac() noexcept
        {
            *mLife = 100000;
            mLife.SetCurrentStep(10000);
            *mBomb = 30000;
            mBomb.SetCurrentStep(10000);
            *mArea = 3000;
            mArea.SetCurrentStep(100);
            *mPower = 10000;
            mPower.SetCurrentStep(1000);

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
                thPracParam.Reset();
            case 2:
                break;
            case 3:
                SetFade(0.8f, 0.1f);
                Close();
                *mNavFocus = 0;

                // Fill Param
                thPracParam.mode = *mMode;
                thPracParam.stage = mStageMap[*mStart][*mPath][*mStage];
                thPracParam.section = CalcSection();
                thPracParam.phase = SpellPhase() ? *mPhase : 0;
                if (SectionHasDlg(thPracParam.section))
                    thPracParam.dlg = *mDlg;

                thPracParam.score = *mScore;
                thPracParam.motivation = *mLife;
                thPracParam.perfect_freeze = *mBomb;
                thPracParam.ice_power = *mPower;
                thPracParam.ice_area = *mArea;

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
        virtual void OnPreUpdate() override
        {
        }
        virtual void OnLocaleChange() override
        {
            SetTitle(Gui::LocaleGetStr(TH_MENU));
            switch (Gui::LocaleGet()) {
            case Gui::LOCALE_ZH_CN:
                if (*mMode) {
                    SetPos(250.f, 90.f);
                    SetSize(360.f, 360.f);
                } else {
                    SetPos(290.f, 290.f);
                    SetSize(280.f, 100.f);
                }
                SetItemWidth(-65.0f);
                break;
            case Gui::LOCALE_EN_US:
                if (*mMode) {
                    SetPos(250.f, 90.f);
                    SetSize(360.f, 360.f);
                } else {
                    SetPos(250.f, 290.f);
                    SetSize(360.f, 110.f);
                }
                SetItemWidth(-80.0f);
                break;
            case Gui::LOCALE_JA_JP:
                if (*mMode) {
                    SetPos(245.f, 80.f);
                    SetSize(370.f, 375.f);
                } else {
                    SetPos(290.f, 290.f);
                    SetSize(280.f, 100.f);
                }
                SetItemWidth(-76.0f);
                break;
            default:
                break;
            }
        }
        virtual void OnContentUpdate() override
        {
            ImGui::TextUnformatted(Gui::LocaleGetStr(TH_MENU));
            ImGui::Separator();

            PracticeMenu();
        }
        const th_glossary_t* SpellPhase()
        {
            auto section = CalcSection();
            if (section == TH128_EX_END_S10) {
                return TH128_SPELL_4PHASE;
            }
            return nullptr;
        }
        void PracticeMenu()
        {
            if (mStart())
                *mSection = *mChapter = 0;
            mMode();
            if (mModePrv) {
                if (*mStart != 3) {
                    if (mPath(TH128_PATH, (TH128_PATH_SELECT + (*mStart * 3))))
                        *mSection = *mChapter = *mPhase = 0;
                    if (mStage())
                        *mSection = *mChapter = *mPhase = 0;
                }
                if (mWarp())
                    *mSection = *mChapter = *mPhase = 0;
                if (*mWarp) {
                    SectionWidget();
                    mPhase(TH_PHASE, SpellPhase());
                }

                char temp[256];
                sprintf_s(temp, "%d%%%%", *mLife / 100);
                mLife(temp);
                sprintf_s(temp, "%d%%%%", *mBomb / 100);
                mBomb(temp);
                sprintf_s(temp, "%d%%%%", *mArea);
                mArea(temp);
                sprintf_s(temp, "%d%%%%", *mPower / 100);
                mPower(temp);
                mScore();
                mScore.RoundDown(10);
            }

            mNavFocus();
            if (mModePrv != *mMode) {
                OnLocaleChange();
                mModePrv = *mMode;
            }
        }
        int CalcSection()
        {
            int chapterId = 0;
            auto realStage = mStageMap[*mStart][*mPath][*mStage];
            switch (*mWarp) {
            case 1: // Chapter
                // Chapter Id = 10000 + Stage * 100 + Section
                chapterId += (realStage + 1) * 100;
                chapterId += *mChapter;
                chapterId += 10000; // Base of chapter ID is 1000.
                return chapterId;
                break;
            case 2:
            case 3: // Mid boss & End boss
                return th_sections_cba[realStage][*mWarp - 2][*mSection];
                break;
            case 4:
            case 5: // Non-spell & Spellcard
                return th_sections_cbt[realStage][*mWarp - 4][*mSection];
                break;
            default:
                return 0;
                break;
            }
        }
        bool SectionHasDlg(int32_t section)
        {
            switch (section) {
            case TH128_A11_BOSS1:
            case TH128_A12_BOSS1:
            case TH128_A13_BOSS1:
            case TH128_A22_BOSS1:
            case TH128_A23_BOSS1:
            case TH128_B11_BOSS1:
            case TH128_B12_BOSS1:
            case TH128_B13_BOSS1:
            case TH128_B22_BOSS1:
            case TH128_B23_BOSS1:
            case TH128_C11_BOSS1:
            case TH128_C12_BOSS1:
            case TH128_C13_BOSS1:
            case TH128_C22_BOSS1:
            case TH128_C23_BOSS1:
            case TH128_EX_END_NS1:
                return true;
            default:
                return false;
            }
        }
        void SectionWidget()
        {
            static char chapterStr[256] {};
            auto realStage = mStageMap[*mStart][*mPath][*mStage];
            auto& chapterCounts = mChapterSetup[realStage];

            switch (*mWarp) {
            case 1: // Chapter
                mChapter.SetBound(1, chapterCounts[0] + chapterCounts[1]);

                if (chapterCounts[1] == 0 && chapterCounts[2] != 0) {
                    sprintf_s(chapterStr, Gui::LocaleGetStr(TH_STAGE_PORTION_N), *mChapter);
                } else if (*mChapter <= chapterCounts[0]) {
                    sprintf_s(chapterStr, Gui::LocaleGetStr(TH_STAGE_PORTION_1), *mChapter);
                } else {
                    sprintf_s(chapterStr, Gui::LocaleGetStr(TH_STAGE_PORTION_2), *mChapter - chapterCounts[0]);
                };

                mChapter(chapterStr);
                break;
            case 2:
            case 3: // Mid boss & End boss
                if (mSection(TH_WARP_SELECT[*mWarp],
                        th_sections_cba[realStage][*mWarp - 2],
                        th_sections_str[::THPrac::Gui::LocaleGet()][mDiffculty]))
                    *mPhase = 0;
                if (SectionHasDlg(th_sections_cba[*mStage][*mWarp - 2][*mSection]))
                    mDlg();
                break;
            case 4:
            case 5: // Non-spell & Spellcard
                if (mSection(TH_WARP_SELECT[*mWarp],
                        th_sections_cbt[realStage][*mWarp - 4],
                        th_sections_str[::THPrac::Gui::LocaleGet()][mDiffculty]))
                    *mPhase = 0;
                if (SectionHasDlg(th_sections_cbt[*mStage][*mWarp - 4][*mSection]))
                    mDlg();
                break;
            default:
                break;
            }
        }

        static constexpr uint8_t mStageMap[4][2][3] {
            { { 0, 1, 2 }, { 0, 3, 4 } },
            { { 5, 6, 7 }, { 5, 8, 9 } },
            { { 10, 11, 12 }, { 10, 13, 14 } },
            { { 15, 15, 15 }, { 15, 15, 15 } },
        };

        int mModePrv = 0;
        Gui::GuiCombo mMode { TH_MODE, TH128_MODE_SELECT };

    public:
        Gui::GuiCombo mStart { TH128_START, TH128_START_SELECT };
        Gui::GuiCombo mPath { TH128_PATH };

    protected:
        Gui::GuiCombo mStage { TH_STAGE, TH128_STAGE_SELECT };
        Gui::GuiCombo mWarp { TH_WARP, TH_WARP_SELECT };
        Gui::GuiCombo mSection { TH_MODE };
        Gui::GuiCombo mPhase { TH_PHASE };
        Gui::GuiCheckBox mDlg { TH_DLG };

        Gui::GuiSlider<int, ImGuiDataType_S32> mChapter { TH_CHAPTER, 0, 0 };
        Gui::GuiDrag<int64_t, ImGuiDataType_S64> mScore { TH_SCORE, 0, 9999999990, 10, 100000000 };
        Gui::GuiSlider<int, ImGuiDataType_S32> mLife { TH128_MOTIVATION, 0, 100000, 100, 10000 };
        Gui::GuiSlider<int, ImGuiDataType_S32> mBomb { TH128_PERFECT_FREEZE, 0, 30000, 100, 10000 };
        Gui::GuiSlider<int, ImGuiDataType_S32> mPower { TH128_ICE_POWER, 0, 10000, 100, 1000 };
        Gui::GuiSlider<int, ImGuiDataType_S32> mArea { TH128_ICE_AREA, 0, 3000, 10, 1000 };

        Gui::GuiNavFocus mNavFocus { TH128_START, TH_MODE, TH128_PATH, TH_STAGE, TH_WARP,
            TH_MID_STAGE, TH_END_STAGE, TH_NONSPELL, TH_SPELL, TH_PHASE, TH_CHAPTER,
            TH_SCORE, TH128_MOTIVATION, TH128_PERFECT_FREEZE, TH128_ICE_POWER, TH128_ICE_AREA };

        int mChapterSetup[16][2] {
            { 5, 2 }, { 5, 4 }, { 4, 3 }, { 4, 3 }, { 5, 4 },
            { 5, 2 }, { 5, 3 }, { 4, 3 }, { 4, 3 }, { 5, 3 },
            { 4, 2 }, { 5, 3 }, { 5, 4 }, { 5, 4 }, { 5, 3 },
            { 5, 5 },
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
            uint32_t index = GetMemContent(0x4b8a9c, 0x5a2c);
            char* repName = (char*)GetMemAddr(0x4b8a9c, index * 4 + 0x5a34, 0x3b4);
            std::wstring repDir(mAppdataPath);
            repDir.append(L"\\ShanghaiAlice\\th128\\replay\\");
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
                thLock = false;
                thHardLock = false;
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
            new HookCtx(0x43D0D5, "\x01", 1),
            new HookCtx(0x43B7FA, "\xeb", 1),
            new HookCtx(0x43d11b, "\x83\xc4\x10\x90\x90", 5),
            new HookCtx(0x432735, "\xe9\x9f\x00\x00\x00\x90", 6) } };
        Gui::GuiHotKey mInfLives { TH_INFLIVES, "F2", VK_F2, {
            new HookCtx(0x43CDD9, "\x00\x00\x00\x00", 4) } };
        Gui::GuiHotKey mInfBombs { TH_INFBOMBS, "F3", VK_F3, {
            new HookCtx(0x43B7D7, "\x00\x00\x00\x00", 4),
            new HookCtx(0x43B90D, "\x00\x00\x00\x00", 4) } };
        Gui::GuiHotKey mInfPower { TH_INFPOWER, "F4", VK_F4, {
            new HookCtx(0x41F429, "\x00", 1) } };
        Gui::GuiHotKey mTimeLock { TH_TIMELOCK, "F5", VK_F5, {
            new HookCtx(0x417307, "\x90", 1) } };
        Gui::GuiHotKey mAutoBomb { TH_AUTOBOMB, "F6", VK_F6, {
            new HookCtx(0x43B8E8, "\xc6", 1),
            new HookCtx(0x43B8F1, "\x00", 1) } };

    public:
        Gui::GuiHotKey mElBgm { TH_EL_BGM, "F7", VK_F7 };
    };

    class THAdvOptWnd : public Gui::PPGuiWnd {
        // Option Related Functions
    private:
        void FpsInit()
        {
            mOptCtx.vpatch_base = (int32_t)GetModuleHandleW(L"vpatch_th128.dll");
            if (mOptCtx.vpatch_base) {
                uint64_t hash[2];
                CalcFileHash(L"vpatch_th128.dll", hash);
                if (hash[0] != 14796143656184423751ll || hash[1] != 15800222838538749590ll)
                    mOptCtx.fps_status = -1;
                else if (*(int32_t*)(mOptCtx.vpatch_base + 0x1b024) == 0) {
                    mOptCtx.fps_status = 2;
                    mOptCtx.fps = *(int32_t*)(mOptCtx.vpatch_base + 0x1b034);
                }
            } else if (*(uint8_t*)0x4d3053 == 3) {
                mOptCtx.fps_status = 1;

                DWORD oldProtect;
                VirtualProtect((void*)0x454c4a, 4, PAGE_EXECUTE_READWRITE, &oldProtect);
                *(double**)0x454c4a = &mOptCtx.fps_dbl;
                VirtualProtect((void*)0x454c4a, 4, oldProtect, &oldProtect);
            } else
                mOptCtx.fps_status = 0;
        }
        void FpsSet()
        {
            if (mOptCtx.fps_status == 1) {
                mOptCtx.fps_dbl = 1.0 / (double)mOptCtx.fps;
            } else if (mOptCtx.fps_status == 2) {
                *(int32_t*)(mOptCtx.vpatch_base + 0x18a24) = mOptCtx.fps;
                *(int32_t*)(mOptCtx.vpatch_base + 0x1b034) = mOptCtx.fps;
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
            SetTitle(Gui::LocaleGetStr(TH_SPELL_PRAC));
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
            ImGui::TextUnformatted(Gui::LocaleGetStr(TH_ADV_OPT));
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

    void ECLJump(ECLHelper& ecl, unsigned int start, int length)
    {
        ecl.SetPos(start);
        ecl << 0 << 0x0018000C << 0x02ff0000 << 0x00000000 << length << 0;
    }
    void ECLJumpEx(ECLHelper& ecl, unsigned int start, unsigned int dest, int at_frame, int ecl_time = 0)
    {
        ecl.SetPos(start);
        ecl << ecl_time << 0x0018000C << 0x02ff0000 << 0x00000000 << dest - start << at_frame;
    }
    void ECLBoss(ECLHelper& ecl, unsigned int pos, bool dlg)
    {
        ecl.SetPos(pos);
        if (dlg)
            ecl << 0 << 0x001401a2 << 0x01ff0000 << 0 << 0 << 0 << 0x001001a3 << 0x00ff0000 << 0;
        ecl << 0 << 0x00300101 << 0x06ff0000 << 0
            << 8 << 'ssoB' << 0
            << 0xc3100000 << 0xc1800000 << 0x28 << 0x3e8 << 1;
        ecl << 0 << 0x001001a4 << 0x00ff0000 << 0;
        ecl << 0 << 0x001401a2 << 0x01ff0000 << 0 << 1;
        ecl << 0 << 0x001001a3 << 0x00ff0000 << 0;
        ecl << 0 << 0x0010000a << 0x00ff0000 << 0;
    }
    void ECL3Boss(ECLHelper& ecl, unsigned int pos, bool dlg)
    {
        ecl.SetFile(0);
        ecl.SetPos(pos);

        if (dlg)
            ecl << 0 << 0x001401a2 << 0x01ff0000 << 0 << 0 << 0 << 0x001001a3 << 0x00ff0000 << 0;
        ecl << 0 << 0x00300101 << 0x06ff0000 << 0
            << 8 << 'ssoB' << 0x41
            << 0xC3100000 << 0xC2000000 << 0x28 << 0x3e8 << 1;
        if (dlg)
            ecl << 1 << 0x001001a3 << 0x00ff0000 << 0;
        ecl << 1 << 0x00300101 << 0x06ff0000 << 0
            << 8 << 'ssoB' << 0x42
            << 0xC3400000 << 0xC2000000 << 0x28 << 0x3e8 << 1;
        ecl << 1 << 0x00300101 << 0x06ff0000 << 0
            << 8 << 'ssoB' << 0x43
            << 0x43400000 << 0xC2000000 << 0x28 << 0x3e8 << 1;

        ecl << 1 << 0x001001a4 << 0x00ff0000 << 0;
        ecl << 61 << 0x001401a2 << 0x01ff0000 << 0 << 1;
        ecl << 61 << 0x001001a3 << 0x00ff0000 << 0;
        ecl << 61 << 0x0010000a << 0x00ff0000 << 0;
    }
    void ECLMBoss(ECLHelper& ecl, uint8_t ordinal, unsigned int pos_414_cmd_1, unsigned int pos_414_cmd_2 = 0)
    {
        ecl << pair(pos_414_cmd_1, (int16_t)0x0)
            << pair(pos_414_cmd_1 + 0x34, 60)
            << pair(pos_414_cmd_1 + 0x60, (int16_t)0x19f)
            << pair(pos_414_cmd_1 + 0x70, 60)
            << pair(pos_414_cmd_1 + 0xa9, (uint8_t)(0x30 + ordinal));
        if (pos_414_cmd_2)
            ecl << pair(pos_414_cmd_2, (int16_t)0x0);
    }
    void ECLBossCard(ECLHelper& ecl, unsigned int pos_11, unsigned int ordinal, unsigned int health,
        float health_mark = 0.0f)
    {
        ecl.SetPos(pos_11);
        ecl << 0 << 0x0014019b << 0x01ff0000 << 0 << health;
        if (health_mark) {
            ecl << 0 << 0x001c01ab << 0x03ff0000 << 0
                << 0 << health_mark << 0xffffa080;
        }
        ecl << 0 << 0x001c000b << 0x01ff0000 << 0
            << 0xC << 'ssoB' << 'draC';
        if (ordinal == 10)
            ecl << 0x00003031;
        else
            ecl << ordinal + 0x30;
    }
    void ECL3BossCard(ECLHelper& ecl, unsigned int pos_boss1a, unsigned int pos_boss1b, unsigned int pos_boss1c,
        uint8_t ordinal, unsigned int health)
    {
        ecl.SetPos(pos_boss1a);
        ecl << 0 << 0x0014019b << 0x01ff0000 << 0 << health;
        ecl << 0 << 0x001c000b << 0x01ff0000 << 0
            << 0xC << 'ssoB' << 'draC' << (uint8_t)(ordinal + 0x30) << (uint8_t)0x0;

        ecl.SetPos(pos_boss1b);
        ecl << 0 << 0x001c000b << 0x01ff0000 << 0
            << 0xC << 'ssoB' << 'draC' << (uint8_t)(ordinal + 0x30) << 'B' << (uint8_t)0x0;

        ecl.SetPos(pos_boss1c);
        ecl << 0 << 0x001c000b << 0x01ff0000 << 0
            << 0xC << 'ssoB' << 'draC' << (uint8_t)(ordinal + 0x30) << 'C' << (uint8_t)0x0;
    }
    void ECLST3MBossCard(ECLHelper& ecl, unsigned int pos_414_cmd, unsigned int health)
    {
        // 0x2b50
        ecl << pair(pos_414_cmd - 0x1c, (int16_t)0x0)
            << pair(pos_414_cmd, (int16_t)0x0)
            << pair(pos_414_cmd + 0x4c, (int16_t)0x19f)
            << pair(pos_414_cmd + 0x6c, 60);
        ecl.SetPos(pos_414_cmd + 0xfc);
        ecl << 0 << 0x0014019b << 0x01ff0000 << 0 << health;
        ecl << 0 << 0x001c000b << 0x01ff0000 << 0
            << 0xC << "MBossCard1" << (char)'\0';
    }
    __declspec(noinline) void THStageWarp(ECLHelper& ecl, int stage, int portion)
    {
        if (stage == 1) {
            switch (portion) {
            case 1:
                break;
            case 2:
                ECLJumpEx(ecl, 0x81d8, 0x8298, 0, 90);
                break;
            case 3:
                ECLJumpEx(ecl, 0x81d8, 0x82cc, 0, 90);
                break;
            case 4:
                ECLJumpEx(ecl, 0x81d8, 0x8300, 0, 90);
                break;
            case 5:
                ECLJumpEx(ecl, 0x81d8, 0x8334, 0, 90);
                break;
            case 6:
                ECLJumpEx(ecl, 0x81d8, 0x83d0, 0, 90);
                break;
            case 7:
                ECLJumpEx(ecl, 0x81d8, 0x8404, 0, 90);
                break;
            default:
                break;
            }
        } else if (stage == 2) {
            switch (portion) {
            case 1:
                break;
            case 2:
                ECLJumpEx(ecl, 0x8680, 0x877c, 0, 90);
                break;
            case 3:
                ECLJumpEx(ecl, 0x8680, 0x87b0, 0, 90);
                break;
            case 4:
                ECLJumpEx(ecl, 0x8680, 0x87e4, 0, 90);
                break;
            case 5:
                ECLJumpEx(ecl, 0x8680, 0x8838, 0, 90);
                break;
            case 6:
                ECLJumpEx(ecl, 0x8680, 0x88c0, 0, 90);
                break;
            case 7:
                ECLJumpEx(ecl, 0x8680, 0x88c0, 0, 90);
                ECLJumpEx(ecl, 0x615c, 0x6190, 0, 0);
                break;
            case 8:
                ECLJumpEx(ecl, 0x8680, 0x88c0, 0, 90);
                ECLJumpEx(ecl, 0x615c, 0x61c4, 0, 0);
                break;
            case 9:
                ECLJumpEx(ecl, 0x8680, 0x88c0, 0, 90);
                ECLJumpEx(ecl, 0x615c, 0x61f8, 0, 0);
                break;
            default:
                break;
            }
        } else if (stage == 3) {
            switch (portion) {
            case 1:
                break;
            case 2:
                ECLJumpEx(ecl, 0x84b8, 0x85b4, 0, 90);
                break;
            case 3:
                ECLJumpEx(ecl, 0x84b8, 0x85e8, 0, 90);
                break;
            case 4:
                ECLJumpEx(ecl, 0x84b8, 0x861c, 0, 90);
                break;
            case 5:
                ECLJumpEx(ecl, 0x84b8, 0x86c4, 0, 90);
                break;
            case 6:
                ECLJumpEx(ecl, 0x84b8, 0x86c4, 0, 90);
                ECLJumpEx(ecl, 0x5528, 0x555c, 0, 0);
                break;
            case 7:
                ECLJumpEx(ecl, 0x84b8, 0x86c4, 0, 90);
                ECLJumpEx(ecl, 0x5528, 0x5590, 0, 0);
                break;
            default:
                break;
            }
        } else if (stage == 4) {
            switch (portion) {
            case 1:
                break;
            case 2:
                ECLJumpEx(ecl, 0x94a8, 0x95a4, 0, 90);
                break;
            case 3:
                ECLJumpEx(ecl, 0x94a8, 0x95d8, 0, 90);
                break;
            case 4:
                ECLJumpEx(ecl, 0x94a8, 0x960c, 0, 90);
                break;
            case 5:
                ECLJumpEx(ecl, 0x94a8, 0x9694, 0, 90);
                break;
            case 6:
                ECLJumpEx(ecl, 0x94a8, 0x9694, 0, 90);
                ECLJumpEx(ecl, 0x6d84, 0x6db8, 0, 0);
                break;
            case 7:
                ECLJumpEx(ecl, 0x94a8, 0x9694, 0, 90);
                ECLJumpEx(ecl, 0x6d84, 0x6dec, 0, 0);
                break;
            default:
                break;
            }
        } else if (stage == 5) {
            switch (portion) {
            case 1:
                break;
            case 2:
                ECLJumpEx(ecl, 0x7ce4, 0x7de0, 0, 90);
                break;
            case 3:
                ECLJumpEx(ecl, 0x7ce4, 0x7e14, 0, 90);
                break;
            case 4:
                ECLJumpEx(ecl, 0x7ce4, 0x7e48, 0, 90);
                break;
            case 5:
                ECLJumpEx(ecl, 0x7ce4, 0x7e9c, 0, 90);
                break;
            case 6:
                ECLJumpEx(ecl, 0x7ce4, 0x7f6c, 0, 90);
                break;
            case 7:
                ECLJumpEx(ecl, 0x7ce4, 0x7f6c, 0, 90);
                ECLJumpEx(ecl, 0x5408, 0x543c, 0, 0);
                break;
            case 8:
                ECLJumpEx(ecl, 0x7ce4, 0x7f6c, 0, 90);
                ECLJumpEx(ecl, 0x5408, 0x5470, 0, 0);
                break;
            case 9:
                ECLJumpEx(ecl, 0x7ce4, 0x7f6c, 0, 90);
                ECLJumpEx(ecl, 0x5408, 0x54a4, 0, 0);
                break;
            default:
                break;
            }
        } else if (stage == 6) {
            switch (portion) {
            case 1:
                break;
            case 2:
                ECLJumpEx(ecl, 0x7d10, 0x7df8, 0, 90);
                break;
            case 3:
                ECLJumpEx(ecl, 0x7d10, 0x7e2c, 0, 90);
                break;
            case 4:
                ECLJumpEx(ecl, 0x7d10, 0x7e60, 0, 90);
                break;
            case 5:
                ECLJumpEx(ecl, 0x7d10, 0x7e94, 0, 90);
                break;
            case 6:
                ECLJumpEx(ecl, 0x7d10, 0x7f30, 0, 90);
                break;
            case 7:
                ECLJumpEx(ecl, 0x7d10, 0x7f30, 0, 90);
                ECLJumpEx(ecl, 0x5ac4, 0x5ae4, 0, 0);
                break;
            default:
                break;
            }
        } else if (stage == 7) {
            switch (portion) {
            case 1:
                break;
            case 2:
                ECLJumpEx(ecl, 0x9afc, 0x9bf8, 0, 90);
                break;
            case 3:
                ECLJumpEx(ecl, 0x9afc, 0x9c2c, 0, 90);
                break;
            case 4:
                ECLJumpEx(ecl, 0x9afc, 0x9c60, 0, 90);
                break;
            case 5:
                ECLJumpEx(ecl, 0x9afc, 0x9c94, 0, 90);
                break;
            case 6:
                ECLJumpEx(ecl, 0x9afc, 0x9d30, 0, 90);
                break;
            case 7:
                ECLJumpEx(ecl, 0x9afc, 0x9d30, 0, 90);
                ECLJumpEx(ecl, 0x7654, 0x7688, 0, 0);
                break;
            case 8:
                ECLJumpEx(ecl, 0x9afc, 0x9d30, 0, 90);
                ECLJumpEx(ecl, 0x7654, 0x76bc, 0, 0);
                break;
            default:
                break;
            }
        } else if (stage == 8) {
            switch (portion) {
            case 1:
                break;
            case 2:
                ECLJumpEx(ecl, 0x7d54, 0x7e50, 0, 90);
                break;
            case 3:
                ECLJumpEx(ecl, 0x7d54, 0x7e84, 0, 90);
                break;
            case 4:
                ECLJumpEx(ecl, 0x7d54, 0x7eb8, 0, 90);
                break;
            case 5:
                ECLJumpEx(ecl, 0x7d54, 0x7f60, 0, 90);
                break;
            case 6:
                ECLJumpEx(ecl, 0x7d54, 0x7f60, 0, 90);
                ECLJumpEx(ecl, 0x4dc4, 0x4df8, 0, 0);
                break;
            case 7:
                ECLJumpEx(ecl, 0x7d54, 0x7f60, 0, 90);
                ECLJumpEx(ecl, 0x4dc4, 0x4e2c, 0, 0);
                break;
            default:
                break;
            }
        } else if (stage == 9) {
            switch (portion) {
            case 1:
                break;
            case 2:
                ECLJumpEx(ecl, 0x94a8, 0x95a4, 0, 90);
                break;
            case 3:
                ECLJumpEx(ecl, 0x94a8, 0x95d8, 0, 90);
                break;
            case 4:
                ECLJumpEx(ecl, 0x94a8, 0x960c, 0, 90);
                break;
            case 5:
                ECLJumpEx(ecl, 0x94a8, 0x9694, 0, 90);
                break;
            case 6:
                ECLJumpEx(ecl, 0x94a8, 0x9694, 0, 90);
                ECLJumpEx(ecl, 0x6d84, 0x6db8, 0, 0);
                break;
            case 7:
                ECLJumpEx(ecl, 0x94a8, 0x9694, 0, 90);
                ECLJumpEx(ecl, 0x6d84, 0x6dec, 0, 0);
                break;
            default:
                break;
            }
        } else if (stage == 10) {
            switch (portion) {
            case 1:
                break;
            case 2:
                ECLJumpEx(ecl, 0x7b04, 0x7c00, 0, 90);
                break;
            case 3:
                ECLJumpEx(ecl, 0x7b04, 0x7c34, 0, 90);
                break;
            case 4:
                ECLJumpEx(ecl, 0x7b04, 0x7c68, 0, 90);
                break;
            case 5:
                ECLJumpEx(ecl, 0x7b04, 0x7c9c, 0, 90);
                break;
            case 6:
                ECLJumpEx(ecl, 0x7b04, 0x7d58, 0, 90);
                break;
            case 7:
                ECLJumpEx(ecl, 0x7b04, 0x7d58, 0, 90);
                ECLJumpEx(ecl, 0x4600, 0x4634, 0, 0);
                break;
            case 8:
                ECLJumpEx(ecl, 0x7b04, 0x7d58, 0, 90);
                ECLJumpEx(ecl, 0x4600, 0x4668, 0, 0);
                break;
            default:
                break;
            }
        } else if (stage == 11) {
            switch (portion) {
            case 1:
                break;
            case 2:
                ECLJumpEx(ecl, 0x9464, 0x954c, 0, 90);
                break;
            case 3:
                ECLJumpEx(ecl, 0x9464, 0x9580, 0, 90);
                break;
            case 4:
                ECLJumpEx(ecl, 0x9464, 0x95b4, 0, 90);
                break;
            case 5:
                ECLJumpEx(ecl, 0x9464, 0x9628, 0, 90);
                break;
            case 6:
                ECLJumpEx(ecl, 0x9464, 0x965c, 0, 90);
                break;
            default:
                break;
            }
        } else if (stage == 12) {
            switch (portion) {
            case 1:
                break;
            case 2:
                ECLJumpEx(ecl, 0x9afc, 0x9bf8, 0, 90);
                break;
            case 3:
                ECLJumpEx(ecl, 0x9afc, 0x9c2c, 0, 90);
                break;
            case 4:
                ECLJumpEx(ecl, 0x9afc, 0x9c60, 0, 90);
                break;
            case 5:
                ECLJumpEx(ecl, 0x9afc, 0x9c94, 0, 90);
                break;
            case 6:
                ECLJumpEx(ecl, 0x9afc, 0x9d30, 0, 90);
                break;
            case 7:
                ECLJumpEx(ecl, 0x9afc, 0x9d30, 0, 90);
                ECLJumpEx(ecl, 0x7654, 0x7688, 0, 0);
                break;
            case 8:
                ECLJumpEx(ecl, 0x9afc, 0x9d30, 0, 90);
                ECLJumpEx(ecl, 0x7654, 0x76bc, 0, 0);
                break;
            default:
                break;
            }
        } else if (stage == 13) {
            switch (portion) {
            case 1:
                break;
            case 2:
                ECLJumpEx(ecl, 0x8408, 0x8504, 0, 90);
                break;
            case 3:
                ECLJumpEx(ecl, 0x8408, 0x8538, 0, 90);
                break;
            case 4:
                ECLJumpEx(ecl, 0x8408, 0x856c, 0, 90);
                break;
            case 5:
                ECLJumpEx(ecl, 0x8408, 0x85c0, 0, 90);
                break;
            case 6:
                ECLJumpEx(ecl, 0x8408, 0x8690, 0, 90);
                break;
            case 7:
                ECLJumpEx(ecl, 0x8408, 0x8690, 0, 90);
                ECLJumpEx(ecl, 0x5b2c, 0x5b60, 0, 0);
                break;
            case 8:
                ECLJumpEx(ecl, 0x8408, 0x8690, 0, 90);
                ECLJumpEx(ecl, 0x5b2c, 0x5b94, 0, 0);
                break;
            case 9:
                ECLJumpEx(ecl, 0x8408, 0x8690, 0, 90);
                ECLJumpEx(ecl, 0x5b2c, 0x5bc8, 0, 0);
                break;
            default:
                break;
            }
        } else if (stage == 14) {
            switch (portion) {
            case 1:
                break;
            case 2:
                ECLJumpEx(ecl, 0x8680, 0x877c, 0, 90);
                break;
            case 3:
                ECLJumpEx(ecl, 0x8680, 0x87b0, 0, 90);
                break;
            case 4:
                ECLJumpEx(ecl, 0x8680, 0x87e4, 0, 90);
                break;
            case 5:
                ECLJumpEx(ecl, 0x8680, 0x8838, 0, 90);
                break;
            case 6:
                ECLJumpEx(ecl, 0x8680, 0x88c0, 0, 90);
                break;
            case 7:
                ECLJumpEx(ecl, 0x8680, 0x88c0, 0, 90);
                ECLJumpEx(ecl, 0x615c, 0x6190, 0, 0);
                break;
            case 8:
                ECLJumpEx(ecl, 0x8680, 0x88c0, 0, 90);
                ECLJumpEx(ecl, 0x615c, 0x61c4, 0, 0);
                break;
            case 9:
                ECLJumpEx(ecl, 0x8680, 0x88c0, 0, 90);
                ECLJumpEx(ecl, 0x615c, 0x61f8, 0, 0);
                break;
            default:
                break;
            }
        } else if (stage == 15) {
            switch (portion) {
            case 1:
                break;
            case 2:
                ECLJumpEx(ecl, 0x7c20, 0x7d1c, 0, 90);
                break;
            case 3:
                ECLJumpEx(ecl, 0x7c20, 0x7d50, 0, 90);
                break;
            case 4:
                ECLJumpEx(ecl, 0x7c20, 0x7d84, 0, 90);
                break;
            case 5:
                ECLJumpEx(ecl, 0x7c20, 0x7db8, 0, 90);
                break;
            case 6:
                ECLJumpEx(ecl, 0x7c20, 0x7e74, 0, 90);
                break;
            case 7:
                ECLJumpEx(ecl, 0x7c20, 0x7e74, 0, 90);
                ECLJumpEx(ecl, 0x471c, 0x4750, 0, 0);
                break;
            case 8:
                ECLJumpEx(ecl, 0x7c20, 0x7e74, 0, 90);
                ECLJumpEx(ecl, 0x471c, 0x4784, 0, 0);
                break;
            default:
                break;
            }
        } else if (stage == 16) {
            switch (portion) {
            case 1:
                break;
            case 2:
                ECLJumpEx(ecl, 0xd7b8, 0xd8a0, 0, 90);
                break;
            case 3:
                ECLJumpEx(ecl, 0xd7b8, 0xd8d4, 0, 90);
                break;
            case 4:
                ECLJumpEx(ecl, 0xd7b8, 0xd908, 0, 90);
                break;
            case 5:
                ECLJumpEx(ecl, 0xd7b8, 0xd93c, 0, 90);
                break;
            case 6:
                ECLJumpEx(ecl, 0xd7b8, 0xd9e0, 0, 90);
                break;
            case 7:
                ECLJumpEx(ecl, 0xd7b8, 0xd9e0, 0, 90);
                ECLJumpEx(ecl, 0xa99c, 0xa9d0, 0, 0);
                break;
            case 8:
                ECLJumpEx(ecl, 0xd7b8, 0xd9e0, 0, 90);
                ECLJumpEx(ecl, 0xa99c, 0xaa04, 0, 0);
                break;
            case 9:
                ECLJumpEx(ecl, 0xd7b8, 0xd9e0, 0, 90);
                ECLJumpEx(ecl, 0xa99c, 0xaa38, 0, 0);
                break;
            case 10:
                ECLJumpEx(ecl, 0xd7b8, 0xd9e0, 0, 90);
                ECLJumpEx(ecl, 0xa99c, 0xaa6c, 0, 0);
                break;
            default:
                break;
            }
        }
    }
    __declspec(noinline) void THPatch(ECLHelper& ecl, th_sections_t section)
    {
        switch (section) {
        case THPrac::TH128::TH128_A11_MID1:
            ECLJump(ecl, 0x81d8, 0x190);
            break;
        case THPrac::TH128::TH128_A11_MID2:
            ECLJump(ecl, 0x81d8, 0x190);
            ecl << pair{0x2994, 6300};
            ecl << pair{0x29a8, 8200};
            ECLMBoss(ecl, 2, 0x2ab0, 0x2a38);
            break;
        case THPrac::TH128::TH128_A11_MID3:
            ECLJump(ecl, 0x81d8, 0x190);
            ecl << pair{0x2994, 2000};
            ecl << pair{0x29a8, 3000};
            ECLMBoss(ecl, 3, 0x2ab0, 0x2a38);
            break;
        case THPrac::TH128::TH128_A11_BOSS1:
            ECLBoss(ecl, 0x81d8, thPracParam.dlg);
            break;
        case THPrac::TH128::TH128_A11_BOSS2:
            ECLBoss(ecl, 0x81d8, false);
            ecl.SetFile(2);
            ECLBossCard(ecl, 0x268, 1, 1200);
            break;
        case THPrac::TH128::TH128_A11_BOSS3:
            ECLBoss(ecl, 0x81d8, false);
            ecl.SetFile(2);
            ecl << pair{0x280, (int8_t)0x32};
            break;
        case THPrac::TH128::TH128_A11_BOSS4:
            ECLBoss(ecl, 0x81d8, false);
            ecl.SetFile(2);
            ECLBossCard(ecl, 0x268, 2, 1300);
            break;
        case THPrac::TH128::TH128_A12_MID1:
            ECLJump(ecl, 0x8680, 0x1ec);
            break;
        case THPrac::TH128::TH128_A12_MID2:
            ECLJump(ecl, 0x8680, 0x1ec);
            ecl << pair{0x32f8, 12000};
            ECLMBoss(ecl, 2, 0x3338);
            break;
        case THPrac::TH128::TH128_A12_MID3:
            ECLJump(ecl, 0x8680, 0x1ec);
            ecl << pair{0x32f8, 5000};
            ECLMBoss(ecl, 3, 0x3338);
            break;
        case THPrac::TH128::TH128_A12_BOSS1:
            ECLBoss(ecl, 0x8680, thPracParam.dlg);
            break;
        case THPrac::TH128::TH128_A12_BOSS2:
            ECLBoss(ecl, 0x8680, false);
            ecl.SetFile(2);
            ECLBossCard(ecl, 0x2b4, 1, 1400);
            break;
        case THPrac::TH128::TH128_A12_BOSS3:
            ECLBoss(ecl, 0x8680, false);
            ecl.SetFile(2);
            ecl << pair{0x2cc, (int8_t)0x32};
            break;
        case THPrac::TH128::TH128_A12_BOSS4:
            ECLBoss(ecl, 0x8680, false);
            ecl.SetFile(2);
            ECLBossCard(ecl, 0x2b4, 2, 4800, 2400.0f);
            break;
        case THPrac::TH128::TH128_A12_BOSS5:
            ECLBoss(ecl, 0x8680, false);
            ecl.SetFile(2);
            ECLBossCard(ecl, 0x2b4, 3, 2400);
            ECLJump(ecl, 0x2438, 0x270);
            break;
        case THPrac::TH128::TH128_A13_MID1:
            ECLJump(ecl, 0x84b8, 0x198);
            ECLJump(ecl, 0x8680, 0x24);
            break;
        case THPrac::TH128::TH128_A13_MID2:
            ECLJump(ecl, 0x84b8, 0x198);
            ECLJump(ecl, 0x8680, 0x24);
            ECLST3MBossCard(ecl, 0x36f8, 1900);
            break;
        case THPrac::TH128::TH128_A13_BOSS1:
            ECL3Boss(ecl, 0x84b8, thPracParam.dlg);
            break;
        case THPrac::TH128::TH128_A13_BOSS2:
            ECL3Boss(ecl, 0x84b8, false);
            ecl.SetFile(2);
            ECL3BossCard(ecl, 0x4f4, 0x10e8, 0x18d8, 1, 2000);
            break;
        case THPrac::TH128::TH128_A13_BOSS3:
            ECL3Boss(ecl, 0x84b8, false);
            ecl.SetFile(2);
            ecl << pair{0x6d6c, (int8_t)0x32};
            ecl << pair{0x6f24, (int8_t)0x32};
            ecl << pair{0x70dc, (int8_t)0x32};
            break;
        case THPrac::TH128::TH128_A13_BOSS4:
            ECL3Boss(ecl, 0x84b8, false);
            ecl.SetFile(2);
            ECL3BossCard(ecl, 0x4f4, 0x10e8, 0x18d8, 2, 2000);
            break;
        case THPrac::TH128::TH128_A13_BOSS5:
            ECL3Boss(ecl, 0x84b8, false);
            ecl.SetFile(2);
            ecl << pair{0x6d6c, (int8_t)0x33};
            ecl << pair{0x6f24, (int8_t)0x33};
            ecl << pair{0x70dc, (int8_t)0x33};
            break;
        case THPrac::TH128::TH128_A13_BOSS6:
            ECL3Boss(ecl, 0x84b8, false);
            ecl.SetFile(2);
            ECL3BossCard(ecl, 0x4f4, 0x10e8, 0x18d8, 3, 3000);
            break;
        case THPrac::TH128::TH128_A13_BOSS7:
            ECL3Boss(ecl, 0x84b8, false);
            ecl.SetFile(2);
            ecl << pair{0x6d6c, (int8_t)0x34};
            ecl << pair{0x6f24, (int8_t)0x34};
            ecl << pair{0x70dc, (int8_t)0x34};
            break;
        case THPrac::TH128::TH128_A13_BOSS8:
            ECL3Boss(ecl, 0x84b8, false);
            ecl.SetFile(2);
            ECL3BossCard(ecl, 0x4f4, 0x10e8, 0x18d8, 4, 1500);
            break;
        case THPrac::TH128::TH128_A13_BOSS9:
            ECL3Boss(ecl, 0x84b8, false);
            ecl.SetFile(2);
            ECL3BossCard(ecl, 0x4f4, 0x10e8, 0x18d8, 5, 6000);
            break;
        case THPrac::TH128::TH128_A22_MID1:
            ECLJump(ecl, 0x94a8, 0x198);
            break;
        case THPrac::TH128::TH128_A22_MID2:
            ECLJump(ecl, 0x94a8, 0x198);
            ecl << pair{0x36e4, 14000};
            ECLMBoss(ecl, 2, 0x3724);
            break;
        case THPrac::TH128::TH128_A22_MID3:
            ECLJump(ecl, 0x94a8, 0x198);
            ecl << pair{0x36e4, 5000};
            ECLMBoss(ecl, 3, 0x3724);
            break;
        case THPrac::TH128::TH128_A22_BOSS1:
            ECLBoss(ecl, 0x94a8, thPracParam.dlg);
            break;
        case THPrac::TH128::TH128_A22_BOSS2:
            ECLBoss(ecl, 0x94a8, false);
            ecl.SetFile(2);
            ECLBossCard(ecl, 0x300, 1, 2800);
            break;
        case THPrac::TH128::TH128_A22_BOSS3:
            ECLBoss(ecl, 0x94a8, false);
            ecl.SetFile(2);
            ecl << pair{0x318, (int8_t)0x32};
            break;
        case THPrac::TH128::TH128_A22_BOSS4:
            ECLBoss(ecl, 0x94a8, false);
            ecl.SetFile(2);
            ECLBossCard(ecl, 0x300, 2, 4600, 2000.0f);
            break;
        case THPrac::TH128::TH128_A22_BOSS5:
            ECLBoss(ecl, 0x94a8, false);
            ecl.SetFile(2);
            ECLBossCard(ecl, 0x300, 3, 2000);
            ECLJump(ecl, 0x2d78, 0x270);
            break;
        case THPrac::TH128::TH128_A23_MID1:
            ECLJump(ecl, 0x7ce4, 0x200);
            ECLJump(ecl, 0x7f14, 0x24);
            break;
        case THPrac::TH128::TH128_A23_MID2:
            ECLJump(ecl, 0x7ce4, 0x200);
            ECLJump(ecl, 0x7f14, 0x24);
            ECLST3MBossCard(ecl, 0x4038, 2000);
            break;
        case THPrac::TH128::TH128_A23_BOSS1:
            ECL3Boss(ecl, 0x7ce4, thPracParam.dlg);
            break;
        case THPrac::TH128::TH128_A23_BOSS2:
            ECL3Boss(ecl, 0x7ce4, false);
            ecl.SetFile(2);
            ECL3BossCard(ecl, 0x4b4, 0x924, 0x10c0, 1, 2000);
            break;
        case THPrac::TH128::TH128_A23_BOSS3:
            ECL3Boss(ecl, 0x7ce4, false);
            ecl << pair{0x62c8, (int8_t)0x32};
            ecl << pair{0x64a0, (int8_t)0x32};
            ecl << pair{0x6678, (int8_t)0x32};
            ecl.SetFile(2);
            break;
        case THPrac::TH128::TH128_A23_BOSS4:
            ECL3Boss(ecl, 0x7ce4, false);
            ecl.SetFile(2);
            ECL3BossCard(ecl, 0x4b4, 0x924, 0x10c0, 2, 2000);
            break;
        case THPrac::TH128::TH128_A23_BOSS5:
            ECL3Boss(ecl, 0x7ce4, false);
            ecl << pair{0x62c8, (int8_t)0x33};
            ecl << pair{0x64a0, (int8_t)0x33};
            ecl << pair{0x6678, (int8_t)0x33};
            ecl.SetFile(2);
            break;
        case THPrac::TH128::TH128_A23_BOSS6:
            ECL3Boss(ecl, 0x7ce4, false);
            ecl.SetFile(2);
            ECL3BossCard(ecl, 0x4b4, 0x924, 0x10c0, 3, 2600);
            break;
        case THPrac::TH128::TH128_A23_BOSS7:
            ECL3Boss(ecl, 0x7ce4, false);
            ecl << pair{0x62c8, (int8_t)0x34};
            ecl << pair{0x64a0, (int8_t)0x34};
            ecl << pair{0x6678, (int8_t)0x34};
            ecl.SetFile(2);
            break;
        case THPrac::TH128::TH128_A23_BOSS8:
            ECL3Boss(ecl, 0x7ce4, false);
            ecl.SetFile(2);
            ECL3BossCard(ecl, 0x4b4, 0x924, 0x10c0, 4, 2000);
            break;
        case THPrac::TH128::TH128_A23_BOSS9:
            ECL3Boss(ecl, 0x7ce4, false);
            ecl.SetFile(2);
            ECL3BossCard(ecl, 0x4b4, 0x924, 0x10c0, 5, 6000);
            break;
        case THPrac::TH128::TH128_B11_MID1:
            ECLJump(ecl, 0x7d10, 0x1b8);
            break;
        case THPrac::TH128::TH128_B11_MID2:
            ECLJump(ecl, 0x7d10, 0x1b8);
            ecl << pair{0x2c78, 6800};
            ECLMBoss(ecl, 2, 0x2cb8);
            break;
        case THPrac::TH128::TH128_B11_MID3:
            ECLJump(ecl, 0x7d10, 0x1b8);
            ecl << pair{0x2c78, 2500};
            ECLMBoss(ecl, 3, 0x2cb8);
            break;
        case THPrac::TH128::TH128_B11_BOSS1:
            ECLBoss(ecl, 0x7d10, thPracParam.dlg);
            break;
        case THPrac::TH128::TH128_B11_BOSS2:
            ECLBoss(ecl, 0x7d10, false);
            ecl.SetFile(2);
            ECLBossCard(ecl, 0x284, 1, 1300);
            break;
        case THPrac::TH128::TH128_B11_BOSS3:
            ECLBoss(ecl, 0x7d10, false);
            ecl.SetFile(2);
            ecl << pair{0x29c, (int8_t)0x32};
            break;
        case THPrac::TH128::TH128_B11_BOSS4:
            ECLBoss(ecl, 0x7d10, false);
            ecl.SetFile(2);
            ECLBossCard(ecl, 0x284, 2, 1300);
            break;
        case THPrac::TH128::TH128_B12_MID1:
            ECLJump(ecl, 0x9afc, 0x1cc);
            break;
        case THPrac::TH128::TH128_B12_MID2:
            ECLJump(ecl, 0x9afc, 0x1cc);
            ecl << pair{0x2bf8, 15000};
            ECLMBoss(ecl, 2, 0x2c38);
            break;
        case THPrac::TH128::TH128_B12_MID3:
            ECLJump(ecl, 0x9afc, 0x1cc);
            ecl << pair{0x2bf8, 6000};
            ECLMBoss(ecl, 3, 0x2c38);
            break;
        case THPrac::TH128::TH128_B12_BOSS1:
            ECLBoss(ecl, 0x9afc, thPracParam.dlg);
            ecl.SetFile(2);
            break;
        case THPrac::TH128::TH128_B12_BOSS2:
            ECLBoss(ecl, 0x9afc, false);
            ecl.SetFile(2);
            ECLBossCard(ecl, 0x2b4, 1, 2400);
            break;
        case THPrac::TH128::TH128_B12_BOSS3:
            ECLBoss(ecl, 0x9afc, false);
            ecl.SetFile(2);
            ecl << pair{0x2cc, (int8_t)0x32};
            break;
        case THPrac::TH128::TH128_B12_BOSS4:
            ECLBoss(ecl, 0x9afc, false);
            ecl.SetFile(2);
            ECLBossCard(ecl, 0x2b4, 2, 4000, 2000.0f);
            break;
        case THPrac::TH128::TH128_B12_BOSS5:
            ECLBoss(ecl, 0x9afc, false);
            ecl.SetFile(2);
            ECLBossCard(ecl, 0x2b4, 3, 2000);
            ECLJump(ecl, 0x2984, 0x270);
            break;
        case THPrac::TH128::TH128_B13_MID1:
            ECLJump(ecl, 0x7d54, 0x198);
            ECLJump(ecl, 0x7f1c, 0x24);
            break;
        case THPrac::TH128::TH128_B13_MID2:
            ECLJump(ecl, 0x7d54, 0x198);
            ECLJump(ecl, 0x7f1c, 0x24);
            ECLST3MBossCard(ecl, 0x36dc, 1900);
            break;
        case THPrac::TH128::TH128_B13_BOSS1:
            ECL3Boss(ecl, 0x7d54, thPracParam.dlg);
            break;
        case THPrac::TH128::TH128_B13_BOSS2:
            ECL3Boss(ecl, 0x7d54, false);
            ecl.SetFile(2);
            ECL3BossCard(ecl, 0x4cc, 0xdf4, 0x1590, 1, 2000);
            break;
        case THPrac::TH128::TH128_B13_BOSS3:
            ECL3Boss(ecl, 0x7d54, false);
            ecl.SetFile(2);
            ecl << pair{0x7244, (int8_t)0x32};
            ecl << pair{0x741c, (int8_t)0x32};
            ecl << pair{0x75f4, (int8_t)0x32};
            break;
        case THPrac::TH128::TH128_B13_BOSS4:
            ECL3Boss(ecl, 0x7d54, false);
            ecl.SetFile(2);
            ECL3BossCard(ecl, 0x4cc, 0xdf4, 0x1590, 2, 2000);
            break;
        case THPrac::TH128::TH128_B13_BOSS5:
            ECL3Boss(ecl, 0x7d54, false);
            ecl.SetFile(2);
            ecl << pair{0x7244, (int8_t)0x33};
            ecl << pair{0x741c, (int8_t)0x33};
            ecl << pair{0x75f4, (int8_t)0x33};
            break;
        case THPrac::TH128::TH128_B13_BOSS6:
            ECL3Boss(ecl, 0x7d54, false);
            ecl.SetFile(2);
            ECL3BossCard(ecl, 0x4cc, 0xdf4, 0x1590, 3, 3000);
            break;
        case THPrac::TH128::TH128_B13_BOSS7:
            ECL3Boss(ecl, 0x7d54, false);
            ecl.SetFile(2);
            ecl << pair{0x7244, (int8_t)0x34};
            ecl << pair{0x741c, (int8_t)0x34};
            ecl << pair{0x75f4, (int8_t)0x34};
            break;
        case THPrac::TH128::TH128_B13_BOSS8:
            ECL3Boss(ecl, 0x7d54, false);
            ecl.SetFile(2);
            ECL3BossCard(ecl, 0x4cc, 0xdf4, 0x1590, 4, 2000);
            break;
        case THPrac::TH128::TH128_B13_BOSS9:
            ECL3Boss(ecl, 0x7d54, false);
            ecl.SetFile(2);
            ECL3BossCard(ecl, 0x4cc, 0xdf4, 0x1590, 5, 6000);
            break;
        case THPrac::TH128::TH128_B22_MID1:
            ECLJump(ecl, 0x94a8, 0x198);
            break;
        case THPrac::TH128::TH128_B22_MID2:
            ECLJump(ecl, 0x94a8, 0x198);
            ecl << pair{0x36e4, 14000};
            ECLMBoss(ecl, 2, 0x3724);
            break;
        case THPrac::TH128::TH128_B22_MID3:
            ECLJump(ecl, 0x94a8, 0x198);
            ecl << pair{0x36e4, 5000};
            ECLMBoss(ecl, 3, 0x3724);
            break;
        case THPrac::TH128::TH128_B22_BOSS1:
            ECLBoss(ecl, 0x94a8, thPracParam.dlg);
            ecl.SetFile(2);
            break;
        case THPrac::TH128::TH128_B22_BOSS2:
            ECLBoss(ecl, 0x94a8, false);
            ecl.SetFile(2);
            ECLBossCard(ecl, 0x2d4, 1, 2200);
            break;
        case THPrac::TH128::TH128_B22_BOSS3:
            ECLBoss(ecl, 0x94a8, false);
            ecl.SetFile(2);
            ecl << pair{0x2ec, (int8_t)0x32};
            break;
        case THPrac::TH128::TH128_B22_BOSS4:
            ECLBoss(ecl, 0x94a8, false);
            ecl.SetFile(2);
            ECLBossCard(ecl, 0x2d4, 2, 4600, 1900.0f);
            break;
        case THPrac::TH128::TH128_B22_BOSS5:
            ECLBoss(ecl, 0x94a8, false);
            ecl.SetFile(2);
            ECLBossCard(ecl, 0x2d4, 3, 1900);
            ECLJump(ecl, 0x2920, 0x270);
            break;
        case THPrac::TH128::TH128_B23_MID1:
            ECLJump(ecl, 0x7b04, 0x1cc);
            ECLJump(ecl, 0x7d00, 0x24);
            break;
        case THPrac::TH128::TH128_B23_MID2:
            ECLJump(ecl, 0x7b04, 0x1cc);
            ECLJump(ecl, 0x7d00, 0x24);
            ECLST3MBossCard(ecl, 0x2b38, 2000);
            break;
        case THPrac::TH128::TH128_B23_BOSS1:
            ECL3Boss(ecl, 0x7b04, thPracParam.dlg);
            break;
        case THPrac::TH128::TH128_B23_BOSS2:
            ECL3Boss(ecl, 0x7b04, false);
            ecl.SetFile(2);
            ECL3BossCard(ecl, 0x4b4, 0x924, 0x10c0, 1, 2400);
            break;
        case THPrac::TH128::TH128_B23_BOSS3:
            ECL3Boss(ecl, 0x7b04, false);
            ecl.SetFile(2);
            ecl << pair{0x62a8, (int8_t)0x32};
            ecl << pair{0x6460, (int8_t)0x32};
            ecl << pair{0x6618, (int8_t)0x32};
            break;
        case THPrac::TH128::TH128_B23_BOSS4:
            ECL3Boss(ecl, 0x7b04, false);
            ecl.SetFile(2);
            ECL3BossCard(ecl, 0x4b4, 0x924, 0x10c0, 2, 2000);
            break;
        case THPrac::TH128::TH128_B23_BOSS5:
            ECL3Boss(ecl, 0x7b04, false);
            ecl.SetFile(2);
            ecl << pair{0x62a8, (int8_t)0x33};
            ecl << pair{0x6460, (int8_t)0x33};
            ecl << pair{0x6618, (int8_t)0x33};
            break;
        case THPrac::TH128::TH128_B23_BOSS6:
            ECL3Boss(ecl, 0x7b04, false);
            ecl.SetFile(2);
            ECL3BossCard(ecl, 0x4b4, 0x924, 0x10c0, 3, 2000);
            break;
        case THPrac::TH128::TH128_B23_BOSS7:
            ECL3Boss(ecl, 0x7b04, false);
            ecl.SetFile(2);
            ecl << pair{0x62a8, (int8_t)0x34};
            ecl << pair{0x6460, (int8_t)0x34};
            ecl << pair{0x6618, (int8_t)0x34};
            break;
        case THPrac::TH128::TH128_B23_BOSS8:
            ECL3Boss(ecl, 0x7b04, false);
            ecl.SetFile(2);
            ECL3BossCard(ecl, 0x4b4, 0x924, 0x10c0, 4, 2000);
            break;
        case THPrac::TH128::TH128_B23_BOSS9:
            ECL3Boss(ecl, 0x7b04, false);
            ecl.SetFile(2);
            ECL3BossCard(ecl, 0x4b4, 0x924, 0x10c0, 5, 6000);
            break;
        case THPrac::TH128::TH128_C11_MID1:
            ECLJump(ecl, 0x9464, 0x184);
            break;
        case THPrac::TH128::TH128_C11_MID2:
            ECLJump(ecl, 0x9464, 0x184);
            ecl << pair{0x37d4, 6300};
            ECLMBoss(ecl, 2, 0x3814);
            break;
        case THPrac::TH128::TH128_C11_MID3:
            ECLJump(ecl, 0x9464, 0x184);
            ecl << pair{0x37d4, 2000};
            ECLMBoss(ecl, 3, 0x3814);
            break;
        case THPrac::TH128::TH128_C11_BOSS1:
            ECLBoss(ecl, 0x9464, thPracParam.dlg);
            break;
        case THPrac::TH128::TH128_C11_BOSS2:
            ECLBoss(ecl, 0x9464, false);
            ecl.SetFile(2);
            ECLBossCard(ecl, 0x278, 1, 1600);
            break;
        case THPrac::TH128::TH128_C11_BOSS3:
            ECLBoss(ecl, 0x9464, false);
            ecl.SetFile(2);
            ecl << pair{0x290, (int8_t)0x32};
            break;
        case THPrac::TH128::TH128_C11_BOSS4:
            ECLBoss(ecl, 0x9464, false);
            ecl.SetFile(2);
            ECLBossCard(ecl, 0x278, 2, 1600);
            break;
        case THPrac::TH128::TH128_C12_MID1:
            ECLJump(ecl, 0x9afc, 0x1cc);
            break;
        case THPrac::TH128::TH128_C12_MID2:
            ECLJump(ecl, 0x9afc, 0x1cc);
            ecl << pair{0x2bf8, 15000};
            ECLMBoss(ecl, 2, 0x2c38);
            break;
        case THPrac::TH128::TH128_C12_MID3:
            ECLJump(ecl, 0x9afc, 0x1cc);
            ecl << pair{0x2bf8, 6000};
            ECLMBoss(ecl, 3, 0x2c38);
            break;
        case THPrac::TH128::TH128_C12_BOSS1:
            ECLBoss(ecl, 0x9afc, thPracParam.dlg);
            break;
        case THPrac::TH128::TH128_C12_BOSS2:
            ECLBoss(ecl, 0x9afc, false);
            ecl.SetFile(2);
            ECLBossCard(ecl, 0x2d0, 1, 2800);
            break;
        case THPrac::TH128::TH128_C12_BOSS3:
            ECLBoss(ecl, 0x9afc, false);
            ecl.SetFile(2);
            ecl << pair{0x2e8, (int8_t)0x32};
            break;
        case THPrac::TH128::TH128_C12_BOSS4:
            ECLBoss(ecl, 0x9afc, false);
            ecl.SetFile(2);
            ECLBossCard(ecl, 0x2d0, 2, 4000, 2000.0f);
            break;
        case THPrac::TH128::TH128_C12_BOSS5:
            ECLBoss(ecl, 0x9afc, false);
            ecl.SetFile(2);
            ECLBossCard(ecl, 0x2d0, 3, 2000);
            ECLJump(ecl, 0x298c, 0x270);
            break;
        case THPrac::TH128::TH128_C13_MID1:
            ECLJump(ecl, 0x8408, 0x200);
            ECLJump(ecl, 0x8638, 0x24);
            break;
        case THPrac::TH128::TH128_C13_MID2:
            ECLJump(ecl, 0x8408, 0x200);
            ECLJump(ecl, 0x8638, 0x24);
            ECLST3MBossCard(ecl, 0x4058, 2500);
            break;
        case THPrac::TH128::TH128_C13_BOSS1:
            ECL3Boss(ecl, 0x8408, thPracParam.dlg);
            break;
        case THPrac::TH128::TH128_C13_BOSS2:
            ECL3Boss(ecl, 0x8408, false);
            ecl.SetFile(2);
            ECL3BossCard(ecl, 0x4e0, 0xe08, 0x15a4, 1, 3000);
            break;
        case THPrac::TH128::TH128_C13_BOSS3:
            ECL3Boss(ecl, 0x8408, false);
            ecl.SetFile(2);
            ecl << pair{0x7258, (int8_t)0x32};
            ecl << pair{0x7430, (int8_t)0x32};
            ecl << pair{0x7608, (int8_t)0x32};
            break;
        case THPrac::TH128::TH128_C13_BOSS4:
            ECL3Boss(ecl, 0x8408, false);
            ecl.SetFile(2);
            ECL3BossCard(ecl, 0x4e0, 0xe08, 0x15a4, 2, 2000);
            break;
        case THPrac::TH128::TH128_C13_BOSS5:
            ECL3Boss(ecl, 0x8408, false);
            ecl.SetFile(2);
            ecl << pair{0x7258, (int8_t)0x33};
            ecl << pair{0x7430, (int8_t)0x33};
            ecl << pair{0x7608, (int8_t)0x33};
            break;
        case THPrac::TH128::TH128_C13_BOSS6:
            ECL3Boss(ecl, 0x8408, false);
            ecl.SetFile(2);
            ECL3BossCard(ecl, 0x4e0, 0xe08, 0x15a4, 3, 1500);
            break;
        case THPrac::TH128::TH128_C13_BOSS7:
            ECL3Boss(ecl, 0x8408, false);
            ecl.SetFile(2);
            ecl << pair{0x7258, (int8_t)0x34};
            ecl << pair{0x7430, (int8_t)0x34};
            ecl << pair{0x7608, (int8_t)0x34};
            break;
        case THPrac::TH128::TH128_C13_BOSS8:
            ECL3Boss(ecl, 0x8408, false);
            ecl.SetFile(2);
            ECL3BossCard(ecl, 0x4e0, 0xe08, 0x15a4, 4, 2000);
            break;
        case THPrac::TH128::TH128_C13_BOSS9:
            ECL3Boss(ecl, 0x8408, false);
            ecl.SetFile(2);
            ECL3BossCard(ecl, 0x4e0, 0xe08, 0x15a4, 5, 6000);
            break;
        case THPrac::TH128::TH128_C22_MID1:
            ECLJump(ecl, 0x8680, 0x1ec);
            break;
        case THPrac::TH128::TH128_C22_MID2:
            ECLJump(ecl, 0x8680, 0x1ec);
            ecl << pair{0x32f8, 12000};
            ECLMBoss(ecl, 2, 0x3338);
            break;
        case THPrac::TH128::TH128_C22_MID3:
            ECLJump(ecl, 0x8680, 0x1ec);
            ecl << pair{0x32f8, 5000};
            ECLMBoss(ecl, 3, 0x3338);
            break;
        case THPrac::TH128::TH128_C22_BOSS1:
            ECLBoss(ecl, 0x8680, thPracParam.dlg);
            break;
        case THPrac::TH128::TH128_C22_BOSS2:
            ECLBoss(ecl, 0x8680, false);
            ecl.SetFile(2);
            ECLBossCard(ecl, 0x2e8, 1, 2200);
            break;
        case THPrac::TH128::TH128_C22_BOSS3:
            ECLBoss(ecl, 0x8680, false);
            ecl.SetFile(2);
            ecl << pair{0x300, (int8_t)0x32};
            break;
        case THPrac::TH128::TH128_C22_BOSS4:
            ECLBoss(ecl, 0x8680, false);
            ecl.SetFile(2);
            ECLBossCard(ecl, 0x2e8, 2, 5800, 2500.0f);
            break;
        case THPrac::TH128::TH128_C22_BOSS5:
            ECLBoss(ecl, 0x8680, false);
            ecl.SetFile(2);
            ECLBossCard(ecl, 0x2e8, 3, 2500);
            ECLJump(ecl, 0x348c, 0x270);
            break;
        case THPrac::TH128::TH128_C23_MID1:
            ECLJump(ecl, 0x7c20, 0x1cc);
            ECLJump(ecl, 0x7e1c, 0x24);
            break;
        case THPrac::TH128::TH128_C23_MID2:
            ECLJump(ecl, 0x7c20, 0x1cc);
            ECLJump(ecl, 0x7e1c, 0x24);
            ECLST3MBossCard(ecl, 0x2b50, 1900);
            break;
        case THPrac::TH128::TH128_C23_BOSS1:
            ECL3Boss(ecl, 0x7c20,  thPracParam.dlg);
            break;
        case THPrac::TH128::TH128_C23_BOSS2:
            ECL3Boss(ecl, 0x7c20,  false);
            ecl.SetFile(2);
            ECL3BossCard(ecl, 0x4f4, 0x10fc, 0x18ec, 1, 2000);
            break;
        case THPrac::TH128::TH128_C23_BOSS3:
            ECL3Boss(ecl, 0x7c20,  false);
            ecl.SetFile(2);
            ecl << pair{0x6d6c, (int8_t)0x32};
            ecl << pair{0x6f24, (int8_t)0x32};
            ecl << pair{0x70dc, (int8_t)0x32};
            break;
        case THPrac::TH128::TH128_C23_BOSS4:
            ECL3Boss(ecl, 0x7c20,  false);
            ecl.SetFile(2);
            ECL3BossCard(ecl, 0x4f4, 0x10fc, 0x18ec, 2, 2000);
            break;
        case THPrac::TH128::TH128_C23_BOSS5:
            ECL3Boss(ecl, 0x7c20, false);
            ecl.SetFile(2);
            ecl << pair{0x6d6c, (int8_t)0x33};
            ecl << pair{0x6f24, (int8_t)0x33};
            ecl << pair{0x70dc, (int8_t)0x33};
            break;
        case THPrac::TH128::TH128_C23_BOSS6:
            ECL3Boss(ecl, 0x7c20,  false);
            ecl.SetFile(2);
            ECL3BossCard(ecl, 0x4f4, 0x10fc, 0x18ec, 3, 3000);
            break;
        case THPrac::TH128::TH128_C23_BOSS7:
            ECL3Boss(ecl, 0x7c20,  false);
            ecl.SetFile(2);
            ecl << pair{0x6d6c, (int8_t)0x34};
            ecl << pair{0x6f24, (int8_t)0x34};
            ecl << pair{0x70dc, (int8_t)0x34};
            break;
        case THPrac::TH128::TH128_C23_BOSS8:
            ECL3Boss(ecl, 0x7c20,  false);
            ecl.SetFile(2);
            ECL3BossCard(ecl, 0x4f4, 0x10fc, 0x18ec, 4, 1500);
            break;
        case THPrac::TH128::TH128_C23_BOSS9:
            ECL3Boss(ecl, 0x7c20,  false);
            ecl.SetFile(2);
            ECL3BossCard(ecl, 0x4f4, 0x10fc, 0x18ec, 5, 6000);
            break;
        case THPrac::TH128::TH128_EX_MID1:
            ECLJump(ecl, 0x0d7b8, 0x1b8);
            break;
        case THPrac::TH128::TH128_EX_MID2:
            ECLJump(ecl, 0x0d7b8, 0x1b8);
            ecl << pair{0x4975, (int8_t)0x32};
            ecl << pair{0x6f20, (int16_t)0x0};
            ecl << pair{0x4900, 60} << pair{0x4938, 60}
                << pair{0xa51c, 60} << pair{0xa554, 60};
            break;
        case THPrac::TH128::TH128_EX_MID3:
            ECLJump(ecl, 0x0d7b8, 0x1b8);
            ecl << pair{0x4975, (int8_t)0x33};
            ecl << pair{0xa4d9, (int8_t)0x33};
            ecl << pair{0x8de4, (int16_t)0x0};
            ecl << pair{0x4900, 60} << pair{0x4938, 60}
                << pair{0xa51c, 60} << pair{0xa554, 60};
            break;
        case THPrac::TH128::TH128_EX_END_NS1:
            ECLBoss(ecl, 0x0d7b8, thPracParam.dlg);
            break;
        case THPrac::TH128::TH128_EX_END_S1:
            ECLBoss(ecl, 0x0d7b8, false);
            ecl.SetFile(2);
            ECLBossCard(ecl, 0x5f8, 1, 2400);
            break;
        case THPrac::TH128::TH128_EX_END_NS2:
            ECLBoss(ecl, 0x0d7b8, false);
            ecl.SetFile(2);
            ecl << pair{0x610, (int8_t)0x32};
            break;
        case THPrac::TH128::TH128_EX_END_S2:
            ECLBoss(ecl, 0x0d7b8, false);
            ecl.SetFile(2);
            ECLBossCard(ecl, 0x5f8, 2, 2400);
            break;
        case THPrac::TH128::TH128_EX_END_NS3:
            ECLBoss(ecl, 0x0d7b8, false);
            ecl.SetFile(2);
            ecl << pair{0x610, (int8_t)0x33};
            break;
        case THPrac::TH128::TH128_EX_END_S3:
            ECLBoss(ecl, 0x0d7b8, false);
            ecl.SetFile(2);
            ECLBossCard(ecl, 0x5f8, 3, 2400);
            break;
        case THPrac::TH128::TH128_EX_END_NS4:
            ECLBoss(ecl, 0x0d7b8, false);
            ecl.SetFile(2);
            ecl << pair{0x610, (int8_t)0x34};
            break;
        case THPrac::TH128::TH128_EX_END_S4:
            ECLBoss(ecl, 0x0d7b8, false);
            ecl.SetFile(2);
            ECLBossCard(ecl, 0x5f8, 4, 3000);
            break;
        case THPrac::TH128::TH128_EX_END_NS5:
            ECLBoss(ecl, 0x0d7b8, false);
            ecl.SetFile(2);
            ecl << pair{0x610, (int8_t)0x35};
            break;
        case THPrac::TH128::TH128_EX_END_S5:
            ECLBoss(ecl, 0x0d7b8, false);
            ecl.SetFile(2);
            ECLBossCard(ecl, 0x5f8, 5, 4000);
            break;
        case THPrac::TH128::TH128_EX_END_NS6:
            ECLBoss(ecl, 0x0d7b8, false);
            ecl.SetFile(2);
            ecl << pair{0x610, (int8_t)0x36};
            ECLJump(ecl, 0x3c08, 0x24);
            break;
        case THPrac::TH128::TH128_EX_END_S6:
            ECLBoss(ecl, 0x0d7b8, false);
            ecl.SetFile(2);
            ECLBossCard(ecl, 0x5f8, 6, 4000);
            break;
        case THPrac::TH128::TH128_EX_END_NS7:
            ECLBoss(ecl, 0x0d7b8, false);
            ecl.SetFile(2);
            ecl << pair{0x610, (int8_t)0x37};
            break;
        case THPrac::TH128::TH128_EX_END_S7:
            ECLBoss(ecl, 0x0d7b8, false);
            ecl.SetFile(2);
            ECLBossCard(ecl, 0x5f8, 7, 1700);
            ecl << pair{0x554, 224.0f};
            break;
        case THPrac::TH128::TH128_EX_END_NS8:
            ECLBoss(ecl, 0x0d7b8, false);
            ecl.SetFile(2);
            ecl << pair{0x610, (int8_t)0x38};
            break;
        case THPrac::TH128::TH128_EX_END_S8:
            ECLBoss(ecl, 0x0d7b8, false);
            ecl.SetFile(2);
            ECLBossCard(ecl, 0x5f8, 8, 3000);
            ecl << pair{0x554, 224.0f};
            break;
        case THPrac::TH128::TH128_EX_END_S9:
            ECLBoss(ecl, 0x0d7b8, false);
            ecl.SetFile(2);
            ECLBossCard(ecl, 0x5f8, 9, 2400);
            break;
        case THPrac::TH128::TH128_EX_END_S10:
            ECLBoss(ecl, 0x0d7b8, false);
            ecl.SetFile(2);
            ECLBossCard(ecl, 0x5f8, 10, 7000);

            switch (thPracParam.phase) {
            case 1:
                ECLBossCard(ecl, 0x5f8, 10, 4000);
                ECLJumpEx(ecl, 0x69c4, 0x6b38, 0);
                break;
            case 2:
                ECLBossCard(ecl, 0x5f8, 10, 1600);
                ECLJumpEx(ecl, 0x69c4, 0x6b38, 0);
                ECLJumpEx(ecl, 0x6b60, 0x6c8c, 0);
                ecl << pair{0x6cb8, (int16_t)0} << pair{0x6cec, (int16_t)0};
                break;
            case 3:
                ECLBossCard(ecl, 0x5f8, 10, 7000);
                ECLJumpEx(ecl, 0x69c4, 0x6b38, 0);
                ECLJumpEx(ecl, 0x6b60, 0x6c8c, 0);
                ecl << pair{0x6cb8, (int16_t)0} << pair{0x6cec, (int16_t)0};
                ECLJumpEx(ecl, 0x6d24, 0x6e08, 0);
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
        ecl.SetBaseAddr((void*)GetMemAddr(0x4b8948, 0xb4, 0xC));

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
        if (thLock)
            return 0;
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

    HOOKSET_DEFINE(THMainHook)
    EHOOK_DY(th128_on_restart, 0x42657f)
    {
        thLock = thHardLock;
    }
    EHOOK_DY(th128_everlasting_bgm, 0x458ff0)
    {
        int32_t retn_addr = ((int32_t*)pCtx->Esp)[0];
        int32_t bgm_cmd = ((int32_t*)pCtx->Esp)[1];
        int32_t bgm_id = ((int32_t*)pCtx->Esp)[2];
        int32_t call_addr = ((int32_t*)pCtx->Esp)[5];

        bool el_switch;
        bool is_practice;
        bool result;

        el_switch = *(THOverlay::singleton().mElBgm) && !THGuiRep::singleton().mRepStatus && thPracParam.mode && thPracParam.section;
        if (thLock)
            el_switch = false;
        is_practice = (*((int32_t*)0x4b4d50) & 0x1);
        if (retn_addr == 0x434a69 && (call_addr == 0x424828 || call_addr == 0x42483c)) {
            thLock = true;
            result = ElBgmTest<0x434935, 0x4268c7, 0x437003, 0x4371e8, 0x426bdd>(
                el_switch, is_practice, 0x434935, 2, 2, call_addr);
        } else {
            result = ElBgmTest<0x434935, 0x4268c7, 0x437003, 0x4371e8, 0x426bdd>(
                el_switch, is_practice, retn_addr, bgm_cmd, bgm_id, call_addr);
        }

        if (result) {
            pCtx->Eip = 0x45908b;
        }
    }
    EHOOK_DY(th128_param_reset, 0x445e6c)
    {
        thLock = false;
        thHardLock = false;
        thPracParam.Reset();
    }
    EHOOK_DY(th128_prac_menu_1, 0x44a115)
    {
        THGuiPrac::singleton().State(1);
    }
    EHOOK_DY(th128_prac_menu_2, 0x44a13a)
    {
        THGuiPrac::singleton().State(2);
    }
    EHOOK_DY(th128_prac_menu_3, 0x44a2d1)
    {
        THGuiPrac::singleton().State(3);
    }
    EHOOK_DY(th128_prac_menu_4, 0x44a3a6)
    {
        THGuiPrac::singleton().State(4);
    }
    EHOOK_DY(th128_prac_menu_move, 0x44a14c)
    {
        DWORD start;
        DWORD sub_l;
        DWORD sub_r;
        DWORD menu_current = *(DWORD*)(pCtx->Esi + 0x28);

        start = *THGuiPrac::singleton().mStart;

        if (menu_current != start) {
            if (menu_current < start) {
                sub_l = menu_current - (start - 4);
                sub_r = start - menu_current;
            } else {
                sub_l = menu_current - start;
                sub_r = (start + 4) - menu_current;
            }

            if (sub_l < sub_r) {
                sub_l = 0 - sub_l;
                PushHelper32(pCtx, sub_l);
            } else {
                PushHelper32(pCtx, sub_r);
            }
            pCtx->Eax = pCtx->Esi + 0x28;
            PushHelper32(pCtx, 0x44a1bc);
            pCtx->Eip = 0x46a970;
            return;
        }

        pCtx->Eip = 0x44a1bc;
    }
    PATCH_DY(th128_prac_menu_enter_1, 0x44a251, "\xeb", 1);
    EHOOK_DY(th128_prac_menu_enter_2, 0x44a31b)
    {
        *(int32_t*)(0x4d2e44) = *THGuiPrac::singleton().mStart * 2;
        if (thPracParam.mode) {
            switch (thPracParam.stage) {
            case 0:
            case 1:
            case 2:
                *(int32_t*)(0x4b4d44) = 0;
                break;
            case 3:
            case 4:
                *(int32_t*)(0x4b4d44) = 1;
                break;
            case 5:
            case 6:
            case 7:
                *(int32_t*)(0x4b4d44) = 2;
                break;
            case 8:
            case 9:
                *(int32_t*)(0x4b4d44) = 3;
                break;
            case 10:
            case 11:
            case 12:
                *(int32_t*)(0x4b4d44) = 4;
                break;
            case 13:
            case 14:
                *(int32_t*)(0x4b4d44) = 5;
                break;
            case 15:
                *(int32_t*)(0x4b4d44) = 6;
                break;
            }
        } else {
            *(int32_t*)(0x4b4d44) = *THGuiPrac::singleton().mStart * 2;
        }

        pCtx->Eip = 0x44a329;
    }
    EHOOK_DY(th128_prac_menu_enter_3, 0x44a376)
    {
        if (thPracParam.mode) {
            pCtx->Eax = thPracParam.stage + 1;
        }
    }
    PATCH_DY(th128_disable_prac_menu, 0x44a241, "\x00\x00", 2);
    EHOOK_DY(th128_patch_main, 0x42641a)
    {
        if (!thLock && thPracParam.mode == 1) {
            // 0x4b8950 0x58fc
            *(int32_t*)(0x4b4cc4) = (int32_t)(thPracParam.score / 10);
            *(int32_t*)(0x4B4D64) = thPracParam.motivation;
            *(int32_t*)(0x4B4D70) = thPracParam.perfect_freeze;
            *(int32_t*)(0x4B4D6C) = thPracParam.ice_power;
            *(float*)(0x4B4D60) = (float)thPracParam.ice_area;

            THSectionPatch();
        }
        thPracParam._playLock = true;
    }
    EHOOK_DY(th128_disable_logo, 0x4204f6)
    {
        if (!thLock && thPracParam.mode == 1 && thPracParam.section) {
            if (thPracParam.section <= 10000 || thPracParam.section >= 20000 || thPracParam.section % 100 != 1) {
                pCtx->Eip = 0x42063b;
            }
        }
    }
    EHOOK_DY(th128_bgm, 0x426bd6)
    {
        if (THBGMTest()) {
            PushHelper32(pCtx, 1);
            pCtx->Eip = 0x426bd8;
        }
    }
    EHOOK_DY(th128_rep_save, 0x441de8)
    {
        char* repName = (char*)(pCtx->Esp + 0x28);
        if (thPracParam.mode)
            THSaveReplay(repName);
    }
    EHOOK_DY(th128_rep_menu_fix, 0x44b974)
    {
        uint32_t* path = (uint32_t*)(pCtx->Ecx + 0x5c);
        uint32_t base = pCtx->Eax;
        base += 0x16c;
        uint32_t testArray[16];
        for (int i = 0; i < 16; ++i) {
            testArray[i] = *(uint32_t*)base;
            base += 0x24;
        }

        if (testArray[1] || testArray[2]) {
            *path = 0;
        } else if (testArray[3] || testArray[4]) {
            *path = 1;
        } else if (testArray[6] || testArray[7]) {
            *path = 2;
        } else if (testArray[8] || testArray[9]) {
            *path = 3;
        } else if (testArray[11] || testArray[12]) {
            *path = 4;
        } else if (testArray[13] || testArray[14]) {
            *path = 5;
        } else if (testArray[15]) {
            *path = 6;
        }
    }
    EHOOK_DY(th128_rep_menu_1, 0x44b808)
    {
        THGuiRep::singleton().State(1);
    }
    EHOOK_DY(th128_rep_menu_2, 0x44b934)
    {
        THGuiRep::singleton().State(2);
    }
    EHOOK_DY(th128_rep_menu_3, 0x44bbfe)
    {
        THGuiRep::singleton().State(3);
    }
    EHOOK_DY(th128_rep_menu_enter, 0x44bc25)
    {
        int stage = pCtx->Eax;
        --stage;
        if (stage != thPracParam.stage)
            thHardLock = thLock = true;
    }
    EHOOK_DY(th128_update, 0x468434)
    {
        GameGuiBegin(IMPL_WIN32_DX9, !THAdvOptWnd::singleton().IsOpen());

        // Gui components update
        THGuiPrac::singleton().Update();
        THGuiRep::singleton().Update();
        THOverlay::singleton().Update();
        bool drawCursor = THAdvOptWnd::StaticUpdate() || THGuiPrac::singleton().IsOpen();

        GameGuiEnd(drawCursor);
    }
    EHOOK_DY(th128_render, 0x468560)
    {
        GameGuiRender(IMPL_WIN32_DX9);
    }
    HOOKSET_ENDDEF()

    HOOKSET_DEFINE(THInitHook)
    static __declspec(noinline) void THGuiCreate()
    {
        // Init
        GameGuiInit(IMPL_WIN32_DX9, 0x4d2e70, 0x4d3970, 0x454420,
            Gui::INGAGME_INPUT_GEN2, 0x4db004, 0x4db000, 0,
            -1);

        // Gui components creation
        THGuiPrac::singleton();
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
        s.th128_gui_init_1.Disable();
        s.th128_gui_init_2.Disable();
    }
    PATCH_DY(th128_disable_demo, 0x4457ae, "\xff\xff\xff\x7f", 4);
    EHOOK_DY(th128_disable_mutex, 0x453a89)
    {
        pCtx->Eip = 0x453aa2;
    }
    PATCH_DY(th128_startup_1, 0x4452b7, "\xeb", 1);
    PATCH_DY(th128_startup_2, 0x445eb9, "\xeb", 1);
    EHOOK_DY(th128_gui_init_1, 0x446273)
    {
        THGuiCreate();
        THInitHookDisable();
    }
    EHOOK_DY(th128_gui_init_2, 0x455a63)
    {
        THGuiCreate();
        THInitHookDisable();
    }
    HOOKSET_ENDDEF()
}

void TH128Init()
{
    TH128::THInitHook::singleton().EnableAllHooks();
}
}
