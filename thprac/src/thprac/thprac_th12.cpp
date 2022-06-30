#include "thprac_utils.h"

namespace THPrac {
namespace TH12 {
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
            SetTitle(XSTR(TH_MENU));
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
            ImGui::Text(XSTR(TH_MENU));
            ImGui::Separator();

            PracticeMenu();
        }
        th_glossary_t* SpellPhase()
        {
            auto section = CalcSection();
            if (section == TH12_ST6_BOSS8 || section == TH12_ST6_BOSS10 || section == TH12_ST7_END_S10) {
                return TH_SPELL_PHASE1;
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

            //ImGui::Text("Pos: %f, %f", ImGui::GetWindowPos().x, ImGui::GetWindowPos().y);
            //ImGui::Text("Size: %f, %f", ImGui::GetWindowSize().x, ImGui::GetWindowSize().y);

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
                break;
            case 4:
            case 5: // Non-spell & Spellcard
                if (mSection(TH_WARP_SELECT[*mWarp],
                        th_sections_cbt[*mStage][*mWarp - 4],
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
        Gui::GuiCombo mUfoSide { TH12_UFO_SIDE, TH12_UFO_SIDE_SELECT };
        Gui::GuiCombo mVentra1 { TH12_VENTRA_1, TH12_VENTRA_SELECT };
        Gui::GuiCombo mVentra2 { TH12_VENTRA_2, TH12_VENTRA_SELECT };
        Gui::GuiCombo mVentra3 { TH12_VENTRA_3, TH12_VENTRA_SELECT };

        Gui::GuiSlider<int, ImGuiDataType_S32> mChapter { TH_CHAPTER, 0, 0 };
        Gui::GuiDrag<int64_t, ImGuiDataType_S64> mScore { TH_SCORE, 0, 9999999990, 10, 100000000 };
        Gui::GuiSlider<int, ImGuiDataType_S32> mLife { TH_LIFE, 0, 9 };
        Gui::GuiSlider<int, ImGuiDataType_S32> mLifeFragment { TH_LIFE_FRAGMENT, 0, 3 };
        Gui::GuiSlider<int, ImGuiDataType_S32> mBomb { TH_BOMB, 0, 9 };
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
                    //*((int32_t*)0x6c6eb0) = 2;
                }
            }
            //if (*((int32_t*)0x6c6ea4) == 2)
            //	SetPos(500.0f, 300.0f);
            //else
            //	SetPos(10.0f, 10.0f);
        }

        Gui::GuiHotKey mMenu { "ModMenuToggle", "BACKSPACE", VK_BACK };
        Gui::GuiHotKey mMuteki { TH_MUTEKI, "F1", VK_F1, {
            new HookCtxPatch((void*)0x43837F, "\x01", 1),
            new HookCtxPatch((void*)0x436d2f, "\xeb", 1),
            new HookCtxPatch((void*)0x4383cb, "\x83\xc4\x10\x90\x90", 5) } };
        Gui::GuiHotKey mInfLives { TH_INFLIVES, "F2", VK_F2, {
            new HookCtxPatch((void*)0x4381E7, "\x66\x0F\x1F\x44\x00", 5) } };
        Gui::GuiHotKey mInfBombs { TH_INFBOMBS, "F3", VK_F3, {
            new HookCtxPatch((void*)0x422F27, "\x00", 1) } };
        Gui::GuiHotKey mInfPower { TH_INFPOWER, "F4", VK_F4, {
            new HookCtxPatch((void*)0x43944B, "\x48", 1) } };
        Gui::GuiHotKey mTimeLock { TH_TIMELOCK, "F5", VK_F5, {
            new HookCtxPatch((void*)0x40DB71, "\xeb", 1),
            new HookCtxPatch((void*)0x414A9B, "\x90", 1) } };
        Gui::GuiHotKey mAutoBomb { TH_AUTOBOMB, "F6", VK_F6, {
            new HookCtxPatch((void*)0x436D9B, "\xc6", 1) } };

    public:
        Gui::GuiHotKey mElBgm { TH_EL_BGM, "F7", VK_F7 };
    };

    class THAdvOptWnd : public Gui::PPGuiWnd {
        EHOOK_ST(th12_all_clear_bonus_1, (void*)0x420a9b)
        {
            pCtx->Eip = 0x420acd;
        }
        EHOOK_ST(th12_all_clear_bonus_2, (void*)0x420bc2)
        {
            if (GetMemContent(0x4b0ce0) & 0x10) {
                pCtx->Eip = 0x420aa4;
            } else if (GetMemContent(0x4b44e8, 0x74) && GetMemContent(0x4b4518, 0x1c, 0xa) & 1) {
                pCtx->Eip = 0x420ac3;
            }
        }
        EHOOK_ST(th12_all_clear_bonus_3, (void*)0x420c6b)
        {
            if (GetMemContent(0x4b0ce0) & 0x10) {
                pCtx->Eip = 0x420aa4;
            } else if (GetMemContent(0x4b44e8, 0x74) && GetMemContent(0x4b4518, 0x1c, 0xa) & 1) {
                pCtx->Eip = 0x420ac3;
            }
        }
    private:
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
        void DatRecInit()
        {
            mOptCtx.data_rec_func = [&](std::vector<RecordedValue>& values) {
                return DataRecFunc(values);
            };
            wchar_t tempStr[MAX_PATH];
            GetCurrentDirectoryW(MAX_PATH, tempStr);
            mOptCtx.data_rec_dir = tempStr;
            mOptCtx.data_rec_dir += L"\\replay";
        }
        void DataRecPreUpd()
        {
            DataRecOpt(mOptCtx, true, thPracParam._playLock);
        }
        void DataRecFunc(std::vector<RecordedValue>& values)
        {
            values.clear();
            values.emplace_back("Score", (int64_t)*(int32_t*)(0x4b0c44) * 10ll);
            values.emplace_back("Graze", *(int32_t*)(0x4b0cdc));
            values.emplace_back("Value", (float)(*(int32_t*)(0x4b0c78)) / 100.0f, "%.2f");
        }
        void DataRecMenu()
        {
            if (DataRecOpt(mOptCtx)) {
                SetContentUpdFunc([&]() { ContentUpdate(); });
            }
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
            SetTitle(XSTR(TH_SPELL_PRAC));
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

            AboutOpt();
            ImGui::EndChild();
            ImGui::SetWindowFocus();
        }
        void PreUpdate()
        {
            DataRecPreUpd();
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
                ecl << pair(0x12f6c, (int16_t)0) << pair(0x12fb8, (int16_t)0);
                ECLJumpEx(ecl, 0x133d4, 0x1346c, 60, 90);
                break;
            case 3:
                ecl << pair(0x12f6c, (int16_t)0) << pair(0x12fb8, (int16_t)0);
                ECLJumpEx(ecl, 0x133d4, 0x13548, 60, 90);
                break;
            case 4:
                ecl << pair(0x12f6c, (int16_t)0) << pair(0x12fb8, (int16_t)0);
                ECLJumpEx(ecl, 0x133d4, 0x1369c, 60, 90);
                break;
            case 5:
                ecl << pair(0x12f6c, (int16_t)0) << pair(0x12fb8, (int16_t)0);
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
                ecl << pair(0x12470, (int16_t)0) << pair(0x124dc, (int16_t)0);
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
                ECLJumpEx(ecl, 0x158c0, 0x15a80, 181, 90);
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
            ecl << pair(0x774, 60) << pair(0x7e0, 60); // Invincible Frame & Wait Time
            ecl << pair(0x790, (int16_t)0x0); // Skip Teleportation
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
            ecl << pair(0x0c820, 0x0fa0);
            ecl << pair(0x0c8bc, 0x14b4);
            ECLJumpEx(ecl, 0xcde8, 0xce40, 0);
            ecl << pair(0x0ccfc, (int16_t)0x0);
            ecl << pair(0x0ce74, 60);
            break;
        case THPrac::TH12::TH12_ST1_MID3:
            ECLJumpEx(ecl, 0x133b0, 0x13658, 59);
            ecl << pair(0x0c820, 0x0fa0);
            ecl << pair(0x0c8bc, 0x14b4);
            ECLJumpEx(ecl, 0xcde8, 0xce40, 0);
            ecl << pair(0x0ccfc, (int16_t)0x0);
            ecl << pair(0x0ce74, 60);

            ECLSetHealth(ecl, 0x0ce74, 60, 0x514);
            ecl << pair(0x0ce7a, (int16_t)0x20);
            ecl << pair(0xce94, 9999);
            break;
        case THPrac::TH12::TH12_ST1_BOSS1:
            ECLJumpEx(ecl, 0x133b0, 0x137ac, 59);
            break;
        case THPrac::TH12::TH12_ST1_BOSS2:
            ECLJumpEx(ecl, 0x133b0, 0x137ac, 59);
            ecl << pair(0x0ae0, 0x5dc);
            ecl << pair(0xcec, 60);
            break;
        case THPrac::TH12::TH12_ST1_BOSS3:
            ECLJumpEx(ecl, 0x133b0, 0x137ac, 59);
            ecl << pair(0x0be4, (int8_t)0x32);
            ECLJumpEx(ecl, 0x190c, 0x1978, 0);
            ecl << pair(0x17e4, (int16_t)0x0) << pair(0x197c, (int16_t)0x0);
            ecl.SetPos(0xb4c);
            ecl << 0.0f << 128.0f << 280.0f << 256.0f;
            break;
        case THPrac::TH12::TH12_ST1_BOSS4:
            ECLJumpEx(ecl, 0x133b0, 0x137ac, 59);
            ecl << pair(0x0be4, (int8_t)0x32);
            ECLJumpEx(ecl, 0x190c, 0x1978, 0);
            ecl << pair(0x17e4, (int16_t)0x0); // << pair(0x197c, (int16_t)0x0);
            ecl << pair(0x162c, 0x4b0);
            ecl << pair(0x1640, 0x3e8);
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
            ecl << pair(0x9eb8, 60);
            //ecl << pair(0x9d7c, 0x546);
            break;
        case THPrac::TH12::TH12_ST2_BOSS1:
            ECLJumpEx(ecl, 0x13108, 0x134fc, 59);
            break;
        case THPrac::TH12::TH12_ST2_BOSS2:
            ECLJumpEx(ecl, 0x13108, 0x134fc, 59);
            ecl << pair(0x0d18, 0x44c);
            ecl << pair(0xeac, 60);
            break;
        case THPrac::TH12::TH12_ST2_BOSS3:
            ECLJumpEx(ecl, 0x13108, 0x134fc, 59);
            ecl << pair(0x0df4, (int8_t)0x32);
            ECLJumpEx(ecl, 0x1268, 0x12d4, 0);
            ecl << pair(0x1188, (int16_t)0x0) << pair(0x12d8, (int16_t)0x0);
            break;
        case THPrac::TH12::TH12_ST2_BOSS4:
            ECLJumpEx(ecl, 0x13108, 0x134fc, 59);
            ecl << pair(0x0df4, (int8_t)0x32);
            ECLJumpEx(ecl, 0x1268, 0x12d4, 0);
            ecl << pair(0x1188, (int16_t)0x0); // << pair(0x12d8, (int16_t)0x0);
            ecl << pair(0x0fe4, 0x6a4);
            break;
        case THPrac::TH12::TH12_ST2_BOSS5:
            ECLJumpEx(ecl, 0x13108, 0x134fc, 59);
            ecl << pair(0xdec, 0xc) << pair(0xdf4, 'draC') << pair(0xdf8, 0x00000033);
            ECLJumpEx(ecl, 0x32e8, 0x3340, 0);
            ECLJumpEx(ecl, 0x34dc, 0x3518, 0);
            ecl << pair(0x35b4, (int16_t)0x0);
            break;
        case THPrac::TH12::TH12_ST3_MID1:
            ECLJumpEx(ecl, 0x16864, 0x169ac, 59);
            break;
        case THPrac::TH12::TH12_ST3_MID2:
            ECLJumpEx(ecl, 0x16864, 0x169ac, 59);
            ecl << pair(0x1047c, 0x6a0);
            ECLJumpEx(ecl, 0x10930, 0x10988, 0);
            ECLJumpEx(ecl, 0x1099c, 0x109c0, 0);
            ecl << pair(0x10998, 60);
            ecl << pair(0x108a0, 60);
            ecl << pair(0x108b4, (int16_t)0x0);
            ecl << pair(0x1086c, (int16_t)0x0) << pair(0x10a5c, (int16_t)0x0);
            break;
        case THPrac::TH12::TH12_ST3_BOSS1:
            ECLJumpEx(ecl, 0x16864, 0x16a98, 59);
            break;
        case THPrac::TH12::TH12_ST3_BOSS2:
            ECLJumpEx(ecl, 0x16864, 0x16abc, 59);
            ecl << pair(0x0bec, 0x6a4);
            ecl << pair(0xdac, 60);
            break;
        case THPrac::TH12::TH12_ST3_BOSS3:
            ECLJumpEx(ecl, 0x16864, 0x16abc, 59);
            ecl << pair(0x0cc8, (int8_t)0x32);
            ECLJumpEx(ecl, 0x27f0, 0x285c, 0);
            ecl << pair(0x2710, (int16_t)0x0) << pair(0x28a0, (int16_t)0x0);
            break;
        case THPrac::TH12::TH12_ST3_BOSS4:
            ECLJumpEx(ecl, 0x16864, 0x16abc, 59);
            ecl << pair(0x0cc8, (int8_t)0x32);
            ECLJumpEx(ecl, 0x27f0, 0x285c, 0);
            ecl << pair(0x2710, (int16_t)0x0); // << pair(0x28a0, (int16_t)0x0);
            ecl << pair(0x256c, 0x7d0);
            break;
        case THPrac::TH12::TH12_ST3_BOSS5:
            ECLJumpEx(ecl, 0x16864, 0x16abc, 59);
            ecl << pair(0x0cc8, (int8_t)0x33);
            ECLJumpEx(ecl, 0x40b0, 0x411c, 0);
            ecl << pair(0x3fd0, (int16_t)0x0) << pair(0x4140, (int16_t)0x0);
            break;
        case THPrac::TH12::TH12_ST3_BOSS6:
            ECLJumpEx(ecl, 0x16864, 0x16abc, 59);
            ecl << pair(0x0cc8, (int8_t)0x33);
            ECLJumpEx(ecl, 0x40b0, 0x411c, 0);
            ecl << pair(0x3fd0, (int16_t)0x0); // << pair(0x4140, (int16_t)0x0);
            ecl << pair(0x3e2c, 0x9c4);
            break;
        case THPrac::TH12::TH12_ST4_MID1:
            ECLJumpEx(ecl, 0x13800, 0x13904, 0);
            break;
        case THPrac::TH12::TH12_ST4_BOSS1:
            ECLJumpEx(ecl, 0x13800, 0x13a70, 119);
            break;
        case THPrac::TH12::TH12_ST4_BOSS2:
            ECLJumpEx(ecl, 0x13800, 0x13a70, 119);
            ecl << pair(0x34d4, 0x708);
            ecl << pair(0x3694, 60);
            break;
        case THPrac::TH12::TH12_ST4_BOSS3:
            ECLJumpEx(ecl, 0x13800, 0x13a70, 119);
            ecl << pair(0x35b0, (int8_t)0x32);
            ECLJumpEx(ecl, 0x4020, 0x408c, 0);
            ecl << pair(0x3f40, (int16_t)0x0) << pair(0x40d0, (int16_t)0x0);
            break;
        case THPrac::TH12::TH12_ST4_BOSS4:
            ECLJumpEx(ecl, 0x13800, 0x13a70, 119);
            ecl << pair(0x35b0, (int8_t)0x32);
            ECLJumpEx(ecl, 0x4020, 0x408c, 0);
            ecl << pair(0x3f40, (int16_t)0x0); // << pair(0x40d0, (int16_t)0x0);
            ecl << pair(0x3d9c, 0x834);
            break;
        case THPrac::TH12::TH12_ST4_BOSS5:
            ECLJumpEx(ecl, 0x13800, 0x13a70, 119);
            ecl << pair(0x35b0, (int8_t)0x33);
            ECLJumpEx(ecl, 0x5178, 0x51e4, 0);
            ecl << pair(0x507c, (int16_t)0x0) << pair(0x5228, (int16_t)0x0);
            break;
        case THPrac::TH12::TH12_ST4_BOSS6:
            ECLJumpEx(ecl, 0x13800, 0x13a70, 119);
            ecl << pair(0x35b0, (int8_t)0x33);
            ECLJumpEx(ecl, 0x5178, 0x51e4, 0);
            ecl << pair(0x507c, (int16_t)0x0); // << pair(0x5228, (int16_t)0x0);
            ecl << pair(0x4ec4, 0x835);
            ecl << pair(0x4ed8, 0x835);
            break;
        case THPrac::TH12::TH12_ST4_BOSS7:
            ECLJumpEx(ecl, 0x13800, 0x13a70, 119);
            ecl << pair(0x35a8, 0xc) << pair(0x35b0, 'draC') << pair(0x35b4, 0x00000034);
            ECLJumpEx(ecl, 0x86c4, 0x8730, 0);
            ECLJumpEx(ecl, 0x8798, 0x87d8, 0);
            ecl << pair(0x8760, 20);
            ecl << pair(0x8614, (int16_t)0x0) << pair(0x8774, (int16_t)0x0);
            break;
        case THPrac::TH12::TH12_ST5_MID1:
            ECLJumpEx(ecl, 0x158c0, 0x159e8, 179);
            ecl << pair(0xd6ac, (int16_t)0x0) << pair(0xd8b8, (int16_t)0x0);
            ecl << pair(0xd6f0, 60);
            break;
        case THPrac::TH12::TH12_ST5_MID2:
            ECLJumpEx(ecl, 0x158c0, 0x159e8, 179);
            ecl << pair(0xd6ac, (int16_t)0x0); // << pair(0xd8b8, (int16_t)0x0);
            ecl << pair(0xd6f0, 60);
            ecl << pair(0x0d7d4, 0x7d0);
            break;
        case THPrac::TH12::TH12_ST5_BOSS1:
            ECLJumpEx(ecl, 0x158c0, 0x15b0c, 190);
            break;
        case THPrac::TH12::TH12_ST5_BOSS2:
            ECLJumpEx(ecl, 0x158c0, 0x15b0c, 190);
            ecl << pair(0x0b00, 0x7d0);
            ecl << pair(0xcec, 60);
            break;
        case THPrac::TH12::TH12_ST5_BOSS3:
            ECLJumpEx(ecl, 0x158c0, 0x15b0c, 190);
            ecl << pair(0x0bf0, (int8_t)0x32);
            ECLJumpEx(ecl, 0x1fbc, 0x2028, 0);
            ecl << pair(0x1edc, (int16_t)0x0) << pair(0x206c, (int16_t)0x0);
            ecl << pair(0xb20, 0x43000000);

            ecl << pair(0x1f04, 2640 - 120);
            break;
        case THPrac::TH12::TH12_ST5_BOSS4:
            ECLJumpEx(ecl, 0x158c0, 0x15b0c, 190);
            ecl << pair(0x0bf0, (int8_t)0x32);
            ECLJumpEx(ecl, 0x1fbc, 0x2028, 0);
            ecl << pair(0x1edc, (int16_t)0x0); // << pair(0x206c, (int16_t)0x0);
            ecl << pair(0x1d38, 0x7d0);
            break;
        case THPrac::TH12::TH12_ST5_BOSS5:
            ECLJumpEx(ecl, 0x158c0, 0x15b0c, 190);
            ecl << pair(0x0bf0, (int8_t)0x33);
            ECLJumpEx(ecl, 0x35c0, 0x362c, 0);
            ecl << pair(0x3498, (int16_t)0x0) << pair(0x3670, (int16_t)0x0);
            ecl << pair(0xb20, 0x43000000);

            ecl << pair(0x34c0, 2400 - 90) << pair(0x34ec, 2400 - 90);
            break;
        case THPrac::TH12::TH12_ST5_BOSS6:
            ECLJumpEx(ecl, 0x158c0, 0x15b0c, 190);
            ecl << pair(0x0bf0, (int8_t)0x33);
            ECLJumpEx(ecl, 0x35c0, 0x362c, 0);
            ecl << pair(0x3498, (int16_t)0x0); // << pair(0x3670, (int16_t)0x0);
            ecl << pair(0x32e0, 0x76c);
            ecl << pair(0x32f4, 0x514);
            break;
        case THPrac::TH12::TH12_ST5_BOSS7:
            ECLJumpEx(ecl, 0x158c0, 0x15b0c, 190);
            ecl << pair(0x0bf0, (int8_t)0x34);
            ECLJumpEx(ecl, 0x412c, 0x4198, 0);
            ecl << pair(0x4078, (int16_t)0x0) << pair(0x41bc, (int16_t)0x0);
            break;
        case THPrac::TH12::TH12_ST6_MID1:
            ECLJumpEx(ecl, 0x1a3c0, 0x1a4a0, 60);
            break;
        case THPrac::TH12::TH12_ST6_BOSS1:
            ECLJumpEx(ecl, 0x1a3c0, 0x1a508, 59);
            break;
        case THPrac::TH12::TH12_ST6_BOSS2:
            ECLJumpEx(ecl, 0x1a3c0, 0x1a508, 59);
            ecl << pair(0x968, 0x898);
            ecl << pair(0xb10, 60);
            break;
        case THPrac::TH12::TH12_ST6_BOSS3:
            ECLJumpEx(ecl, 0x1a3c0, 0x1a508, 59);
            ecl << pair(0x0a44, (int8_t)0x32);
            ECLJumpEx(ecl, 0x29f0, 0x2a5c, 0);
            ecl << pair(0x2910, (int16_t)0x0);
            ecl << pair(0x298c, 60);
            ECLJumpEx(ecl, 0x2a9c, 0x2af0, 0);
            break;
        case THPrac::TH12::TH12_ST6_BOSS4:
            ECLJumpEx(ecl, 0x1a3c0, 0x1a508, 59);
            ecl << pair(0x0a44, (int8_t)0x32);
            ECLJumpEx(ecl, 0x29f0, 0x2a5c, 0);
            ecl << pair(0x2910, (int16_t)0x0);
            ecl << pair(0x298c, 60);
            ECLJumpEx(ecl, 0x2a9c, 0x2b04, 0); // Changed

            ECLSetHealth(ecl, 0x2d04, 0x0, 0x960);
            ecl << pair(0x2d18, 9999);
            break;
        case THPrac::TH12::TH12_ST6_BOSS5:
            ECLJumpEx(ecl, 0x1a3c0, 0x1a508, 59);
            ecl << pair(0x0a44, (int8_t)0x33);
            ECLJumpEx(ecl, 0x42e0, 0x4798, 0);
            ecl << pair(0x41e8, (int16_t)0x0) << pair(0x420c, (int16_t)0x0);
            ecl << pair(0x428c, 0);
            break;
        case THPrac::TH12::TH12_ST6_BOSS6:
            ECLJumpEx(ecl, 0x1a3c0, 0x1a508, 59);
            ecl << pair(0x0a44, (int8_t)0x33);
            ECLJumpEx(ecl, 0x42e0, 0x47ac, 0); // Changed
            ecl << pair(0x41e8, (int16_t)0x0) << pair(0x420c, (int16_t)0x0);
            ecl << pair(0x428c, 0);

            ecl << pair(0x3f54, 0x9c4);
            break;
        case THPrac::TH12::TH12_ST6_BOSS7:
            ECLJumpEx(ecl, 0x1a3c0, 0x1a508, 59);
            ecl << pair(0x0a44, (int8_t)0x34);
            ECLJumpEx(ecl, 0x6378, 0x6830, 0);
            ecl << pair(0x6280, (int16_t)0x0) << pair(0x62a4, (int16_t)0x0);
            ecl << pair(0x6324, 0);
            break;
        case THPrac::TH12::TH12_ST6_BOSS8:
            ECLJumpEx(ecl, 0x1a3c0, 0x1a508, 59);
            ecl << pair(0x0a44, (int8_t)0x34);
            ECLJumpEx(ecl, 0x6378, 0x6844, 0); // Changed
            ecl << pair(0x6280, (int16_t)0x0) << pair(0x62a4, (int16_t)0x0);
            ecl << pair(0x6324, 0);

            ecl << pair(0x5fd8, 0x1388);

            if (thPracParam.phase == 1) {
                ECLJumpEx(ecl, 0xd264, 0xd294, 120);
                ecl << pair(0xd508, (int16_t)0x0);

                ecl << pair(0x0ec0c, 0x0);
                ecl << pair(0x0f468, 0x0);
                ecl << pair(0x0f650, 0x3d);
                ecl << pair(0x0fa90, 0x0);
                ecl << pair(0x0ffdc, 0x0);
                ecl << pair(0x10188, 0x0);
            }
            break;
        case THPrac::TH12::TH12_ST6_BOSS9:
            ECLJumpEx(ecl, 0x1a3c0, 0x1a508, 59);
            ecl << pair(0x0a44, (int8_t)0x35);
            ECLJumpEx(ecl, 0x9030, 0x9508, 0);
            ecl << pair(0x8f44, (int16_t)0x0);
            ecl << pair(0x8fdc, 10);
            break;
        case THPrac::TH12::TH12_ST6_BOSS10:
            ECLJumpEx(ecl, 0x1a3c0, 0x1a508, 59);
            ecl << pair(0x0a44, (int8_t)0x36);
            ECLJumpEx(ecl, 0x98f8, 0x99ac, 0);
            ecl << pair(0x9834, (int16_t)0x0);
            ecl << pair(0x9894, 10);

            if (thPracParam.phase == 1) {
                ecl << pair(0x12a28, 0x118c);
            }
            break;
        case THPrac::TH12::TH12_ST7_MID1:
            ECLJumpEx(ecl, 0xdb14, 0xdc3c, 59);
            break;
        case THPrac::TH12::TH12_ST7_MID2:
            ECLJumpEx(ecl, 0xdb14, 0xdc3c, 59);
            ecl << pair(0x4099, (int8_t)0x32);
            ECLJumpEx(ecl, 0x52a0, 0x530c, 0);
            ECLJumpEx(ecl, 0x5374, 0x53b4, 0);
            ecl << pair(0x5194, (int16_t)0x0) << pair(0x5350, (int16_t)0x0);
            break;
        case THPrac::TH12::TH12_ST7_MID3:
            ECLJumpEx(ecl, 0xdb14, 0xdc3c, 59);
            ecl << pair(0x4099, (int8_t)0x33);
            ECLJumpEx(ecl, 0x5fd4, 0x6040, 0);
            ECLJumpEx(ecl, 0x60a8, 0x60e8, 0);
            ecl << pair(0x5ec8, (int16_t)0x0) << pair(0x6084, (int16_t)0x0);
            break;
        case THPrac::TH12::TH12_ST7_END_NS1:
            nue();
            break;
        case THPrac::TH12::TH12_ST7_END_S1:
            nue();
            ecl << pair(0x788, 0x8fc);
            ecl << pair(0xab0, 9999);
            break;
        case THPrac::TH12::TH12_ST7_END_NS2:
            nue();
            ecl << pair(0x98c, (int8_t)0x32);
            ECLJumpEx(ecl, 0x185c, 0x18dc, 0);
            ecl << pair(0x1768, (int16_t)0x0) << pair(0x1920, (int16_t)0x0);
            ecl << pair(0x7bc, 0) << pair(0x7c0, 0x43000000);
            break;
        case THPrac::TH12::TH12_ST7_END_S2:
            nue();
            ecl << pair(0x98c, (int8_t)0x32);
            ECLJumpEx(ecl, 0x185c, 0x18dc, 0);
            ecl << pair(0x1768, (int16_t)0x0); // << pair(0x1920, (int16_t)0x0);
            ecl << pair(0x7bc, 0) << pair(0x7c0, 0x43000000);
            ecl << pair(0x15c4, 0x514);
            break;
        case THPrac::TH12::TH12_ST7_END_NS3:
            nue();
            ecl << pair(0x98c, (int8_t)0x33);
            ECLJumpEx(ecl, 0x2240, 0x22c0, 0);
            ecl << pair(0x2160, (int16_t)0x0) << pair(0x2304, (int16_t)0x0);
            ecl << pair(0x7bc, 0) << pair(0x7c0, 0x43000000);
            break;
        case THPrac::TH12::TH12_ST7_END_S3:
            nue();
            ecl << pair(0x98c, (int8_t)0x33);
            ECLJumpEx(ecl, 0x2240, 0x22c0, 0);
            ecl << pair(0x2160, (int16_t)0x0); // << pair(0x2304, (int16_t)0x0);
            ecl << pair(0x7bc, 0) << pair(0x7c0, 0x43000000);
            ecl << pair(0x1fbc, 0x0a8c);
            break;
        case THPrac::TH12::TH12_ST7_END_NS4:
            nue();
            ecl << pair(0x98c, (int8_t)0x34);
            ECLJumpEx(ecl, 0x2dcc, 0x2e4c, 0);
            ecl << pair(0x2cd8, (int16_t)0x0) << pair(0x2e90, (int16_t)0x0);
            ecl << pair(0x7bc, 0) << pair(0x7c0, 0x43000000);
            break;
        case THPrac::TH12::TH12_ST7_END_S4:
            nue();
            ecl << pair(0x98c, (int8_t)0x34);
            ECLJumpEx(ecl, 0x2dcc, 0x2e4c, 0);
            ecl << pair(0x2cd8, (int16_t)0x0); // << pair(0x2e90, (int16_t)0x0);
            ecl << pair(0x7bc, 0) << pair(0x7c0, 0x43000000);
            ecl << pair(0x2b34, 0x6A4);
            break;
        case THPrac::TH12::TH12_ST7_END_NS5:
            nue();
            ecl << pair(0x98c, (int8_t)0x35);
            ECLJumpEx(ecl, 0x3a2c, 0x3aac, 0);
            ecl << pair(0x3938, (int16_t)0x0) << pair(0x3af0, (int16_t)0x0);
            ecl << pair(0x7bc, 0) << pair(0x7c0, 0x43000000);
            break;
        case THPrac::TH12::TH12_ST7_END_S5:
            nue();
            ecl << pair(0x98c, (int8_t)0x35);
            ECLJumpEx(ecl, 0x3a2c, 0x3aac, 0);
            ecl << pair(0x3938, (int16_t)0x0); // << pair(0x3af0, (int16_t)0x0);
            ecl << pair(0x7bc, 0) << pair(0x7c0, 0x43000000);
            ecl << pair(0x3794, 0x0e10);
            break;
        case THPrac::TH12::TH12_ST7_END_NS6:
            nue();
            ecl << pair(0x98c, (int8_t)0x36);
            ECLJumpEx(ecl, 0x44cc, 0x454c, 0);
            ecl << pair(0x43d8, (int16_t)0x0) << pair(0x4590, (int16_t)0x0);
            ecl << pair(0x7bc, 0) << pair(0x7c0, 0x43000000);
            break;
        case THPrac::TH12::TH12_ST7_END_S6:
            nue();
            ecl << pair(0x98c, (int8_t)0x36);
            ECLJumpEx(ecl, 0x44cc, 0x454c, 0);
            ecl << pair(0x43d8, (int16_t)0x0); // << pair(0x4590, (int16_t)0x0);
            ecl << pair(0x7bc, 0) << pair(0x7c0, 0x43000000);
            ecl << pair(0x4234, 0x0a8c);
            break;
        case THPrac::TH12::TH12_ST7_END_NS7:
            nue();
            ecl << pair(0x98c, (int8_t)0x37);
            ECLJumpEx(ecl, 0x51e8, 0x5268, 0);
            ecl << pair(0x50f4, (int16_t)0x0) << pair(0x52ac, (int16_t)0x0);
            ecl << pair(0x7bc, 0) << pair(0x7c0, 0x43000000);
            break;
        case THPrac::TH12::TH12_ST7_END_S7:
            nue();
            ecl << pair(0x98c, (int8_t)0x37);
            ECLJumpEx(ecl, 0x51e8, 0x5268, 0);
            ecl << pair(0x50f4, (int16_t)0x0); // << pair(0x52ac, (int16_t)0x0);
            ecl << pair(0x7bc, 0) << pair(0x7c0, 0x43000000);
            ecl << pair(0x4f50, 0x898);
            break;
        case THPrac::TH12::TH12_ST7_END_NS8:
            nue();
            ecl << pair(0x98c, (int8_t)0x38);
            ECLJumpEx(ecl, 0x5f04, 0x5f84, 0);
            ecl << pair(0x5e10, (int16_t)0x0) << pair(0x5fa8, (int16_t)0x0);
            ecl << pair(0x7bc, 0) << pair(0x7c0, 0x43000000);
            break;
        case THPrac::TH12::TH12_ST7_END_S8:
            nue();
            ecl << pair(0x98c, (int8_t)0x38);
            ECLJumpEx(ecl, 0x5f04, 0x5f84, 0);
            ecl << pair(0x5e10, (int16_t)0x0); // << pair(0x5fa8, (int16_t)0x0);
            ecl << pair(0x7bc, 0) << pair(0x7c0, 0x43000000);
            ecl << pair(0x5c6c, 0x898);
            break;
        case THPrac::TH12::TH12_ST7_END_S9:
            nue();
            ecl << pair(0x98c, (int8_t)0x39);
            ECLJumpEx(ecl, 0x71dc, 0x725c, 0);
            ecl << pair(0x7114, (int16_t)0x0) << pair(0x72a0, (int16_t)0x0);
            ecl << pair(0x7bc, 0) << pair(0x7c0, 0x43000000);
            break;
        case THPrac::TH12::TH12_ST7_END_S10:
            nue();
            ecl << pair(0x98c, (int8_t)0x31) << pair(0x98d, (int8_t)0x30);
            ECLJumpEx(ecl, 0xfc0, 0x106c, 0);
            ecl << pair(0xe6c, (int16_t)0x0) << pair(0xed8, (int16_t)0x0) << pair(0xeec, (int16_t)0x0);
            ecl << pair(0x7bc, 0) << pair(0x7c0, 0x43000000);

            if (thPracParam.phase == 1) {
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
        else
            return th_sections_bgm[thPracParam.section];
    }
    void THSaveReplay(char* repName)
    {
        ReplaySaveParam(mb_to_utf16(repName).c_str(), thPracParam.GetJson());
    }
    __declspec(noinline) void TH12AddVentra(int ventra)
    {
        int32_t item_struct = GetMemAddr(0x4b44f0, 0x171254);
        ventra -= 1;
        ASM mov eax, ventra;
        ASM mov ecx, item_struct;
        ASM mov edx, 0x4270b0;
        ASM call edx;
    }
    void THDataInit()
    {
        AnlyDataInit();

        DataRef<DATA_SCENE_ID>(U32_ARG(0x4cee40));
        DataRef<DATA_RND_SEED>(U16_ARG(0x4ce568));
        DataRef<DATA_DIFFCULTY>(U8_ARG(0x4b0ca8));
        DataRef<DATA_SHOT_TYPE>(U8_ARG(0x4b0c90));
        DataRef<DATA_SUB_SHOT_TYPE>(U8_ARG(0x4b0c94));
        DataRef<DATA_STAGE>(U8_ARG(0x4b0cb0));
        DataRef<DATA_STARTING_STAGE>(U8_ARG(0x4b0cb4));
    }

    HOOKSET_DEFINE(THMainHook)
    EHOOK_DY(th12_everlasting_bgm, (void*)0x454960)
    {
        int32_t retn_addr = ((int32_t*)pCtx->Esp)[0];
        int32_t bgm_cmd = ((int32_t*)pCtx->Esp)[1];
        int32_t bgm_id = ((int32_t*)pCtx->Esp)[2];
        int32_t call_addr = ((int32_t*)pCtx->Esp)[3];

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
    }
    EHOOK_DY(th12_param_reset, (void*)0x43fdfb)
    {
        thPracParam.Reset();
    }
    EHOOK_DY(th12_prac_menu_1, (void*)0x446059)
    {
        THGuiPrac::singleton().State(1);
    }
    EHOOK_DY(th12_prac_menu_2, (void*)0x44607b)
    {
        THGuiPrac::singleton().State(2);
    }
    EHOOK_DY(th12_prac_menu_3, (void*)0x4462d3)
    {
        THGuiPrac::singleton().State(3);
    }
    EHOOK_DY(th12_prac_menu_4, (void*)0x44636b)
    {
        THGuiPrac::singleton().State(4);
    }
    PATCH_DY(th12_prac_menu_enter_1, (void*)0x446138, "\xeb", 1);
    EHOOK_DY(th12_prac_menu_enter_2, (void*)0x446333)
    {
        pCtx->Eax = thPracParam.stage;
    }
    EHOOK_DY(th12_disable_prac_menu_1, (void*)0x4464e5)
    {
        pCtx->Eip = 0x44651a;
    }
    EHOOK_DY(th12_disable_prac_menu_2, (void*)0x44603e)
    {
        pCtx->Esp += 0x10;
        pCtx->Eip = 0x446043;
    }
    PATCH_DY(th12_disable_prac_menu_3, (void*)0x446100, "\x00", 1);
    EHOOK_DY(th12_patch_main, (void*)0x40e8df)
    {
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
    }
    EHOOK_DY(th12_bgm, (void*)0x42293a)
    {
        if (THBGMTest()) {
            PushHelper32(pCtx, 1);
            pCtx->Eip = 0x42293b;
        } 
    }
    EHOOK_DY(th12_rep_save, (void*)0x43c32c)
    {
        char* repName = (char*)(pCtx->Esp + 0x28);
        if (thPracParam.mode)
            THSaveReplay(repName);
    }
    EHOOK_DY(th12_rep_menu_1, (void*)0x4467df)
    {
        THGuiRep::singleton().State(1);
    }
    EHOOK_DY(th12_rep_menu_2, (void*)0x4468f8)
    {
        THGuiRep::singleton().State(2);
    }
    EHOOK_DY(th12_rep_menu_3, (void*)0x446ab1)
    {
        THGuiRep::singleton().State(3);
    }
    EHOOK_DY(th12_update, (void*)0x4625fb)
    {
        GameGuiBegin(IMPL_WIN32_DX9, !THAdvOptWnd::singleton().IsOpen());

        // Gui components update
        THGuiPrac::singleton().Update();
        THGuiRep::singleton().Update();
        THOverlay::singleton().Update();
        bool drawCursor = THAdvOptWnd::StaticUpdate() || THGuiPrac::singleton().IsOpen();

        GameGuiEnd(drawCursor);
    }
    EHOOK_DY(th12_render, (void*)0x462722)
    {
        GameGuiRender(IMPL_WIN32_DX9);
    }
    HOOKSET_ENDDEF()

    HOOKSET_DEFINE(THInitHook)
    static __declspec(noinline) void THGuiCreate()
    {
        // Init
        GameGuiInit(IMPL_WIN32_DX9, 0x4ce8f0, 0x4cf3f0, 0x44fe00,
            Gui::INGAGME_INPUT_GEN2, 0x4d48c4, 0x4d48c0, 0,
            -1);

        // Gui components creation
        THGuiPrac::singleton();
        THGuiRep::singleton();
        THOverlay::singleton();

        // Hooks
        THMainHook::singleton().EnableAllHooks();
        THDataInit();

        // Reset thPracParam
        thPracParam.Reset();
    }
    static __declspec(noinline) void THInitHookDisable()
    {
        auto& s = THInitHook::singleton();
        s.th12_gui_init_1.Disable();
        s.th12_gui_init_2.Disable();
    }
    PATCH_DY(th12_disable_demo, (void*)0x43f78f, "\xff\xff\xff\x7f", 4);
    EHOOK_DY(th12_disable_mutex, (void*)0x44f603)
    {
        pCtx->Eip = 0x44f61c;
    }
    PATCH_DY(th12_startup_1, (void*)0x43f357, "\xeb", 1);
    PATCH_DY(th12_startup_2, (void*)0x43fe69, "\xeb", 1);
    EHOOK_DY(th12_gui_init_1, (void*)0x440808)
    {
        THGuiCreate();
        THInitHookDisable();
    }
    EHOOK_DY(th12_gui_init_2, (void*)0x4511f9)
    {
        THGuiCreate();
        THInitHookDisable();
    }
    HOOKSET_ENDDEF()
}

void TH12Init()
{
    TH12::THInitHook::singleton().EnableAllHooks();
    TryKeepUpRefreshRate((void*)0x450dcc);
    if (GetModuleHandleW(L"vpatch_th12.dll")) {
        TryKeepUpRefreshRate((void*)((DWORD)GetModuleHandleW(L"vpatch_th12.dll") + 0x54ce));
    }
}
}
