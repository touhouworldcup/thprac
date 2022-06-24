#include "thprac_utils.h"

namespace THPrac {
namespace TH08 {
    using std::pair;
    struct THPracParam {
        int32_t mode;
        int32_t stage;
        int32_t warp;
        int32_t section;
        int32_t phase;
        int32_t frame;
        int32_t mimic;
        float life;
        float bomb;
        int64_t score;
        float power;
        int32_t graze;
        int32_t point;
        int32_t point_total;
        int32_t point_stage;
        int32_t time;
        int32_t value;
        int16_t gauge;
        int32_t night;
        int32_t familiar;
        int32_t rank;
        bool rankLock;

        bool _playLock = false;
        void Reset()
        {
            memset(this, 0, sizeof(THPracParam));
        }
        bool ReadJson(std::string& json)
        {
            Reset();
            ParseJson();

            ForceJsonValue(game, "th08");
            GetJsonValue(mode);
            GetJsonValue(stage);
            GetJsonValue(section);
            GetJsonValue(phase);
            GetJsonValue(frame);
            GetJsonValue(life);
            GetJsonValue(bomb);
            GetJsonValue(score);
            GetJsonValue(power);
            GetJsonValue(graze);
            GetJsonValue(point);
            GetJsonValue(point_total);
            GetJsonValue(point_stage);
            GetJsonValue(time);
            GetJsonValue(value);
            GetJsonValue(gauge);
            GetJsonValue(night);
            GetJsonValue(familiar);
            GetJsonValue(rank);
            GetJsonValueEx(rankLock, Bool);

            return true;
        }
        std::string GetJson()
        {
            CreateJson();

            AddJsonValueEx(version, GetVersionStr(), jalloc);
            AddJsonValueEx(game, "th08", jalloc);
            AddJsonValue(mode);
            AddJsonValue(stage);
            if (section)
                AddJsonValue(section);
            if (phase)
                AddJsonValue(phase);
            if (frame)
                AddJsonValue(frame);

            AddJsonValueEx(life, (int)life);
            AddJsonValueEx(bomb, (int)bomb);
            AddJsonValue(score);
            AddJsonValueEx(power, (int)power);
            AddJsonValue(graze);
            AddJsonValue(point_total);
            AddJsonValue(point_stage);
            AddJsonValue(time);
            AddJsonValue(value);
            AddJsonValue(gauge);
            AddJsonValue(night);
            AddJsonValue(familiar);
            AddJsonValue(rank);
            AddJsonValue(rankLock);

            ReturnJson();
        }
    };
    THPracParam thPracParam {};

    class THGuiPrac : public Gui::GameGuiWnd {
        THGuiPrac() noexcept
        {
            *mLife = 8;
            *mBomb = 8;
            *mPower = 128;
            *mMode = 1;
            *mValue = 60000;
            mGauge.SetCurrentStep(1000);
            *mRank = 12;

            SetFade(0.8f, 0.1f);
            SetStyle(ImGuiStyleVar_WindowRounding, 0.0f);
            SetStyle(ImGuiStyleVar_WindowBorderSize, 0.0f);
            OnLocaleChange();
        }
        SINGLETON(THGuiPrac);
    public:

        void SetDiffculty(int rank)
        {
            mDiffculty = rank;
        }
        int GetStage()
        {
            return *mStage;
        }
        __declspec(noinline) void State(int state)
        {
            int gaugeType = 0;
            switch (state) {
            case 0:
                break;
            case 1:
                SetFade(0.8f, 0.1f);
                Open();
                mDiffculty = (int)(*((int8_t*)0x17ce891));
                switch (*((int8_t*)0x164d0b1)) {
                case 3:
                    mGauge.SetBound(-5000, 10000);
                    gaugeType = 1;
                    break;
                case 10:
                    mGauge.SetBound(-5000, 5000);
                    gaugeType = 2;
                    break;
                case 4:
                case 6:
                case 8:
                    mGauge.SetBound(-10000, 2000);
                    gaugeType = 3;
                    break;
                case 5:
                case 7:
                case 9:
                case 11:
                    mGauge.SetBound(-2000, 10000);
                    gaugeType = 4;
                    break;
                default:
                    mGauge.SetBound(-10000, 10000);
                    gaugeType = 0;
                    break;
                }
                if (mGaugeType != gaugeType)
                    *mGauge = 0;
                mGaugeType = gaugeType;
                break;
            case 2:
                break;
            case 3:
                SetFade(0.8f, 0.8f);
                Close();

                // Fill Param
                thPracParam.mode = *mMode;
                thPracParam.stage = *mStage;
                thPracParam.section = CalcSection();
                //thPracParam.phase = SpellPhase() ? *mPhase : 0;
                thPracParam.frame = *mFrame;
                thPracParam.score = *mScore;
                thPracParam.life = (float)*mLife;
                thPracParam.bomb = (float)*mBomb;
                thPracParam.power = (float)*mPower;
                thPracParam.graze = *mGraze;
                thPracParam.point = 0;
                thPracParam.point_total = *mPointTotal;
                thPracParam.point_stage = *mPointStage;
                thPracParam.time = *mTime;
                thPracParam.value = *mValue;
                thPracParam.gauge = (int16_t)*mGauge;
                thPracParam.night = *mNight;
                thPracParam.familiar = *mFamiliar;
                thPracParam.rank = *mRank;
                thPracParam.rankLock = *mRankLock;
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
                SetSize(370.f, 390.f);
                SetPos(245.f, 75.f);
                SetItemWidth(-60.0f);
                break;
            case Gui::LOCALE_EN_US:
                SetSize(440.f, 375.f);
                SetPos(190.f, 75.f);
                SetItemWidth(-80.0f);
                break;
            case Gui::LOCALE_JA_JP:
                SetSize(380.f, 390.f);
                SetPos(250.f, 75.f);
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

        void PracticeMenu()
        {
            mMode();
            if (mStage())
                *mSection = *mChapter = 0;
            if (*mMode == 1) {
                if (mWarp())
                    *mSection = *mChapter = *mPhase = *mFrame = 0;
                if (*mWarp) {
                    SectionWidget();
                    //SpellPhase();
                }

                mLife();
                mBomb();
                mScore();
                mScore.RoundDown(10);
                mPower();
                mGraze();
                //mPoint();
                mPointTotal();
                mPointStage();
                mTime();
                mValue();
                mValue.RoundDown(10);

                char temp_str[256];
                float gauge_f = (float)*mGauge / 100.0f;
                sprintf_s(temp_str, "%3.2f%%%%", gauge_f);
                mGauge(temp_str);

                auto night = *mNight;
                if (night < 2)
                    sprintf_s(temp_str, "11:%s", night % 2 ? "30" : "00");
                else
                    sprintf_s(temp_str, "%02d:%s", (night - 2) / 2, night % 2 ? "30" : "00");
                mNight(temp_str);

                if (CheckIfBoss()) {
                    mFamiliar();
                }

                mRank();
                mRankLock();
                if (mDiffculty > 1)
                    mRank.SetBound(8, *mRankLock ? 99 : 12);
                else
                    mRank.SetBound(8, *mRankLock ? 99 : 16);
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
            static char chapterStr[256] {};
            auto& chapterCounts = mChapterSetup[*mStage];

            switch (*mWarp) {
            case 1: // Chapter
                mChapter.SetBound(1, chapterCounts[0] + chapterCounts[1]);

                if (chapterCounts[1] == 0) {
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
                if (mSection(TH_WARP_SELECT_FRAME[*mWarp],
                        th_sections_cba[*mStage][*mWarp - 2],
                        th_sections_str[::THPrac::Gui::LocaleGet()][mDiffculty]))
                    *mPhase = 0;
                break;
            case 4:
            case 5: // Non-spell & Spellcard
                if (mSection(TH_WARP_SELECT_FRAME[*mWarp],
                        th_sections_cbt[*mStage][*mWarp - 4],
                        th_sections_str[::THPrac::Gui::LocaleGet()][mDiffculty]))
                    *mPhase = 0;
                break;
            case 6:
                mFrame();
                break;
            }
        }
        bool CheckIfBoss()
        {
            auto section = CalcSection();
            for (auto s : th_sections_cba[*mStage][0]) {
                if (section == s) {
                    return true;
                }
            }
            for (auto s : th_sections_cba[*mStage][1]) {
                if (section == s) {
                    return true;
                }
            }
            return false;
        }
        // Data
        Gui::GuiCombo mMode { TH_MODE, TH_MODE_SELECT };
        Gui::GuiCombo mStage { TH_STAGE, TH08_STAGE_SELECT };
        Gui::GuiCombo mWarp { TH_WARP, TH_WARP_SELECT_FRAME };
        Gui::GuiCombo mSection { TH_MODE };
        Gui::GuiCombo mPhase { TH_PHASE };

        Gui::GuiSlider<int, ImGuiDataType_S32> mChapter { TH_CHAPTER, 0, 0 };
        Gui::GuiDrag<int, ImGuiDataType_S32> mFrame { TH_FRAME, 0, INT_MAX };
        Gui::GuiSlider<int, ImGuiDataType_S32> mLife { TH_LIFE, 0, 8 };
        Gui::GuiSlider<int, ImGuiDataType_S32> mBomb { TH_BOMB, 0, 8 };
        Gui::GuiDrag<int64_t, ImGuiDataType_S64> mScore { TH_SCORE, 0, 9999999990, 10, 100000000 };
        Gui::GuiSlider<int, ImGuiDataType_S32> mPower { TH_POWER, 0, 128 };
        Gui::GuiDrag<int, ImGuiDataType_S32> mGraze { TH_GRAZE, 0, INT_MAX, 1, 10000 };
        Gui::GuiDrag<int, ImGuiDataType_S32> mPoint { TH_POINT, 0, 9999, 1, 1000 };
        Gui::GuiDrag<int, ImGuiDataType_S32> mPointTotal { TH_POINT_TOTAL, 0, 9999, 1, 1000 };
        Gui::GuiDrag<int, ImGuiDataType_S32> mPointStage { TH_POINT_STAGE, 0, 9999, 1, 1000 };
        Gui::GuiDrag<int, ImGuiDataType_S32> mTime { TH08_TIME, 0, INT_MAX, 1, 1000 };
        Gui::GuiDrag<int, ImGuiDataType_S32> mValue { TH08_VALUE, 0, 9999999, 10, 100000 };
        Gui::GuiSlider<int, ImGuiDataType_S32> mGauge { TH08_GAUGE, -10000, 10000, 1, 1000 };
        Gui::GuiSlider<int, ImGuiDataType_S32> mFamiliar { TH08_FAMILIAR, 0, 2000, 1, 100 };
        Gui::GuiSlider<int, ImGuiDataType_S32> mNight { TH08_NIGHT, 0, 11, 1, 1 };
        Gui::GuiSlider<int, ImGuiDataType_S32> mRank { TH_BULLET_RANK, 8, 16, 1, 10, 10 };
        Gui::GuiCheckBox mRankLock { TH_BULLET_RANKLOCK };

        Gui::GuiNavFocus mNavFocus { TH_STAGE, TH_MODE, TH_WARP, TH_FRAME,
            TH_MID_STAGE, TH_END_STAGE, TH_NONSPELL, TH_SPELL, TH_PHASE, TH_CHAPTER,
            TH_LIFE, TH_BOMB, TH_SCORE, TH_POWER, TH_GRAZE, TH_POINT, TH_POINT_TOTAL, TH_POINT_STAGE,
            TH08_TIME, TH08_VALUE, TH08_GAUGE, TH08_NIGHT, TH_BULLET_RANK, TH_BULLET_RANKLOCK };

        int mChapterSetup[9][2] {
            { 1, 1 },
            { 4, 0 },
            { 2, 1 },
            { 4, 2 },
            { 4, 2 },
            { 3, 2 },
            { 2, 0 },
            { 2, 0 },
            { 3, 4 }
        };

        int mGaugeType;
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
            uint32_t index = GetMemContent(0x18bde08, 0xc28c);
            char* raw = (char*)GetMemAddr(0x18bde08, index * 512 + 0x70);
            std::wstring repName = mb_to_utf16(raw);
            //auto pos = repName.rfind('/');
            //if (pos != std::string::npos)
            //	repName = repName.substr(pos + 1);

            std::string param;
            if (ReplayLoadParam(repName.c_str(), param) && mRepParam.ReadJson(param))
                mParamStatus = true;
            else
                mRepParam.Reset();
        }

        bool mRepStatus = false;
        void State(int state)
        {
            switch (state) {
            case 1:
                thPracParam.Reset();
                mRepStatus = false;
                mParamStatus = false;
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
                    *((int32_t*)0x17ce8cc) = 2;
                }
            }
            //if (*((int32_t*)0x6c6ea4) == 2)
            //	SetPos(500.0f, 300.0f);
            //else
            //	SetPos(10.0f, 10.0f);
        }

        Gui::GuiHotKey mMenu { "ModMenuToggle", "BACKSPACE", VK_BACK };
        Gui::GuiHotKey mMuteki { TH_MUTEKI, "F1", VK_F1, {
            new HookCtxPatch((void*)0x44abda, "\xB9\x70\xA6\x4E\x00\xE8\xBC\x1F\x00\x00\xE9\xC0\x02\x00\x00", 15),
            new HookCtxPatch((void*)0x44Ab86, "\x03", 1) } };
        Gui::GuiHotKey mInfLives { TH_INFLIVES, "F2", VK_F2, {
            new HookCtxPatch((void*)0x44D0FA, "\x00", 1) } };
        Gui::GuiHotKey mInfBombs { TH_INFBOMBS, "F3", VK_F3, {
            new HookCtxPatch((void*)0x44CA78, "\x00", 1),
            new HookCtxPatch((void*)0x44CAA4, "\x00", 1) } };
        Gui::GuiHotKey mInfPower { TH_INFPOWER, "F4", VK_F4, {
            new HookCtxPatch((void*)0x43B295, "\x2e\xe9\x61", 3),
            new HookCtxPatch((void*)0x44CDB1, "\x00", 1) } };
        Gui::GuiHotKey mTimeLock { TH_TIMELOCK, "F5", VK_F5, {
            new HookCtxPatch((void*)0x416CBE, "\x2e\xe9", 2),
            new HookCtxPatch((void*)0x42DDB5, "\xeb", 1) } };
        Gui::GuiHotKey mAutoBomb { TH_AUTOBOMB, "F6", VK_F6, {
            new HookCtxPatch((void*)0x44CC18, "\xff\x89", 2),
            new HookCtxPatch((void*)0x44CC21, "\x66\xC7\x05\x28\xD5\x64\x01\x02", 8),
            new HookCtxPatch((void*)0x44C85D, "\x30", 1) } };

    public:
        Gui::GuiHotKey mElBgm { TH_EL_BGM, "F7", VK_F7 };
    };

    class THAdvOptWnd : public Gui::PPGuiWnd {
        EHOOK_ST(th08_all_clear_bonus_1, (void*)0x435f8e)
        {
            pCtx->Eip = 0x435f92;
        }
        EHOOK_ST(th08_all_clear_bonus_2, (void*)0x4384b9)
        {
            pCtx->Eip = 0x4384c1;
        } 
        EHOOK_ST(th08_all_clear_bonus_3, (void*)0x438568)
        {
            pCtx->Eip = 0x438570;
        }
        PATCH_ST(th08_DOSWNC_1, (void*)0x415A4E, "\x39\xC0", 2);
        PATCH_ST(th08_DOSWNC_2, (void*)0x416463, "\x39\xC0", 2)
    private:
        void FpsInit()
        {
            mOptCtx.vpatch_base = (int32_t)GetModuleHandleW(L"vpatch_th08.dll");
            if (mOptCtx.vpatch_base) {
                uint64_t hash[2];
                CalcFileHash(L"vpatch_th08.dll", hash);
                if (hash[0] != 14324321420199198230ll || hash[1] != 10561235471127337137ll)
                    mOptCtx.fps_status = -1;
                else if (*(int32_t*)(mOptCtx.vpatch_base + 0x17024) == 0) {
                    mOptCtx.fps_status = 2;
                    mOptCtx.fps = *(int32_t*)(mOptCtx.vpatch_base + 0x17034);
                }
            } else
                mOptCtx.fps_status = 0;
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
            th08_all_clear_bonus_1.Setup();
            th08_all_clear_bonus_2.Setup();
            th08_all_clear_bonus_3.Setup();
            th08_DOSWNC_1.Setup();
            th08_DOSWNC_2.Setup();
        }
        void GameplaySet()
        {
            th08_all_clear_bonus_1.Toggle(mOptCtx.all_clear_bonus);
            th08_all_clear_bonus_2.Toggle(mOptCtx.all_clear_bonus);
            th08_all_clear_bonus_3.Toggle(mOptCtx.all_clear_bonus);
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
            int32_t* score1 = (int32_t*)GetMemAddr(0x160f510, 0x8);
            int32_t* score2 = (int32_t*)GetMemAddr(0x160f510, 0x0);
            int32_t* graze1 = (int32_t*)GetMemAddr(0x160f510, 0x4);
            int32_t* graze2 = (int32_t*)GetMemAddr(0x160f510, 0xc);
            int32_t* point_total_1 = (int32_t*)GetMemAddr(0x160f510, 0x30);
            int32_t* pTime1 = (int32_t*)0x164cfb4;
            int32_t* value = (int32_t*)GetMemAddr(0x160f510, 0x24);
            values.clear();
            values.emplace_back("Score", (int64_t)*score1 * 10ll);
            values.emplace_back("Graze", *graze2);
            values.emplace_back("Point", *point_total_1);
            values.emplace_back("Time", *pTime1);
            values.emplace_back("PIV", *value);
        }
        void DataRecMenu()
        {
            *((int32_t*)0x17ce8cc) = 2;
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
            static bool DOSWNC = false;
            *((int32_t*)0x17ce8cc) = 2;
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
                
                if (ImGui::Checkbox(XSTR(TH08_DOSWNC), &DOSWNC)) {
                    th08_DOSWNC_1.Toggle(DOSWNC);
                    th08_DOSWNC_2.Toggle(DOSWNC);
                }
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

    void* THStage4ANM()
    {
        void* buffer = (void*)GetMemContent(0x18bdc90, 0x98);
        if (thPracParam.mode == 1 && thPracParam.stage >= 3 && thPracParam.stage <= 4 && thPracParam.section) {
            //auto section = thPracParam.section;
            //if (section >= 10000 && section < 20000) {
            //    int portionId = (section - 10000) % 100;
            //    if (portionId <= 4) {
            //        return nullptr;
            //    }
            //}
            VFile anm;
            anm.SetFile(buffer, 0x99999);
            anm << pair(0x8029c, 0) << pair(0x802b0, 0) << pair(0x802bc, 4000)
                << pair(0x802f8, 0) << pair(0x8030c, 0) << pair(0x802fc, 1);
        }

        return nullptr;
    }

    struct __th08_ins_header {
        int32_t time;
        int16_t opcode;
        int16_t length;
        int16_t rankMask;
        int16_t paramMask;
    };
    EHOOK_G1(th08_name_fix, (void*)0x42a8fb)
    {
        int32_t temp;
        if (thPracParam.mode == 1) {
            int32_t func = 0x437f5c;
            int32_t name = 0;
            switch (thPracParam.stage) {
            case 5:
                if (thPracParam.section < TH08_ST5_BOSS1)
                    break;
                name = 0x16;
                break;
            case 6:
                if (thPracParam.section == 0)
                    break;
                *((int32_t*)0x4e4b64) = 2;
                break;
            case 7:
                if (thPracParam.section == 0)
                    break;
                temp = *((int32_t*)0x4ecc9c);
                *((int32_t*)0x4ecc9c) = *((int32_t*)0x4ecca0);
                *((int32_t*)0x4ecca0) = temp;
                *((int32_t*)0x4e4b64) = 2;
                name = 0x18;
                break;
            case 8:
                if (thPracParam.section < TH08_ST7_END_NS1)
                    break;
                temp = *((int32_t*)0x4ecc9c);
                *((int32_t*)0x4ecc9c) = *((int32_t*)0x4ecca0);
                *((int32_t*)0x4ecca0) = temp;
                *((int32_t*)0x4e4b64) = 2;
                name = 0x19;
                break;
            default:
                break;
            }
            if (name) {
                __asm
                {
						mov ecx, name
						call func
                }
            }
        }
        th08_name_fix::GetHook().Disable();
    }
    void ECLWarp(int32_t time1, int32_t offset1,
        int32_t time2 = -1, int32_t offset2 = -1,
        int32_t time3 = -1, int32_t offset3 = -1,
        int32_t time4 = -1, int32_t offset4 = -1)
    {
        int32_t* target = (int32_t*)0xf54cf8;
        int32_t pECL = *((int32_t*)0x4ECCB8);

        if (time1 != -1) {
            target[0] = time1;
            if (offset1 != -1)
                target[1] = pECL + offset1;
        }
        target += 4;
        if (time2 != -1) {
            target[0] = time2;
            if (offset2 != -1)
                target[1] = pECL + offset2;
        }
        target += 4;
        if (time3 != -1) {
            target[0] = time3;
            if (offset3 != -1)
                target[1] = pECL + offset3;
        }
        target += 4;
        if (time4 != -1) {
            target[0] = time4;
            if (offset4 != -1)
                target[1] = pECL + offset4;
        }
        target += 4;
    }
    void ECLTimeFix(int offset, int32_t time, unsigned int count)
    {
        __th08_ins_header* ins = (__th08_ins_header*)GetMemAddr(0x4ECCB8, offset);
        for (unsigned int i = 0; i < count; ++i) {
            ins->time = time;
            ins = (__th08_ins_header*)((int32_t)ins + ins->length);
        }
    }
    void ECLCallSub(ECLHelper& ecl, int offset, int sub_id, int ecl_time = -1, bool stall = false)
    {
        ecl.SetPos(offset);
        ecl << (ecl_time == -1 ? 0 : ecl_time) << 0x00100034 << 0x0000ff00
            << sub_id;
        if (stall)
            ecl << 0xffffffff << 0x000Cffff << 0x00FFff00;
    }
    void ECLJump(ECLHelper& ecl, int offset, int dest, int time = 0, int ecl_time = -1)
    {
        ecl.SetPos(offset);
        ecl << (ecl_time == -1 ? 0 : ecl_time) << 0x00140004 << 0x0000ff00
            << time << (dest - offset);
    }
    void ECLSetTime(ECLHelper& ecl, int offset, int32_t time, int32_t sub_id, int ecl_time = -1, bool stall = true)
    {
        ecl.SetPos(offset);
        ecl << (ecl_time == -1 ? 0 : ecl_time) << 0x00140086 << 0x0000ff00
            << time << sub_id;
        if (stall)
            ecl << 0x999999 << 0x000C0000 << 0x0000ff00;
    }
    void ECLSetHealth(ECLHelper& ecl, int offset, int32_t health, int ecl_time = -1, bool stall = true)
    {
        ecl.SetPos(offset);
        ecl << (ecl_time == -1 ? 0 : ecl_time) << 0x00100083 << 0x0000ff00
            << health;
        if (stall)
            ecl << 0x999999 << 0x000C0000 << 0x0000ff00;
    }
    void ECLCheckTime(int32_t time)
    {
        int32_t* pTime1 = (int32_t*)0x164cfb4;
        int32_t* pTime2 = (int32_t*)GetMemAddr(0x160f510, 0x3c);
        int32_t* pTime3 = (int32_t*)GetMemAddr(0x160f510, 0x44);
        if (*pTime1 < time) {
            *pTime1 = time;
            *pTime2 = time;
            *pTime3 = time;
        }
    }
    void STDJump(int offset, int32_t ins_ord, int32_t time, int32_t ins_time = 0)
    {
        VFile std;
        std.SetFile(*((void**)0x4E4824), 0x9999);
        std.SetPos(offset);
        std << ins_time << 0x000c0004 << ins_ord << time << 0;
    }
    void STDStage4Fix(bool isLastSpell = false)
    {
        VFile std;
        std.SetFile(*((void**)0x4E4824), 0x9999);
        std << pair(0xed8, 1) << pair(0xf14, 1);
        if (isLastSpell) {
            std.SetPos(0x1394);
            std << 6926 << 0x000c001f << 2 << 0 << 0
                << 6926 << 0x000c0004 << 1 << 0 << 0;
            std.SetPos(0xf48);
            std << 0 << 0x000c0004 << 70 << 6926 << 0;
        }
    }
    void MSGNameFix()
    {
        th08_name_fix::GetHook().Enable();
    }
    __declspec(noinline) void THStageWarp(ECLHelper& ecl, int stage, int portion)
    {
        if (stage == 1) {
            switch (portion) {
            case 1:
                ECLWarp(340, 0x9c38, 20, 0xad18);
                break;
            case 2:
                ECLWarp(2875, 0xaba4, 20, 0xad18);
                break;
            default:
                break;
            }
        } else if (stage == 2) {
            switch (portion) {
            case 1:
                ECLWarp(400, 0xa194, 20, 0xc0cc);
                break;
            case 2:
                ECLWarp(990, 0xa8d8, 20, 0xc0cc);
                break;
            case 3:
                ECLWarp(2350, 0xb418, 20, 0xc0cc);
                break;
            case 4:
                ECLWarp(3350, 0xba78, 20, 0xc0cc);
                break;
            default:
                break;
            }
        } else if (stage == 3) {
            switch (portion) {
            case 1:
                ECLWarp(340, 0xb258, 20, 0xb91c, 20, 0xbb74, 20, 0xc0ac);
                break;
            case 2:
                ECLWarp(1220, 0xb368, 200, 0xba14, 20, 0xbb74, 20, 0xc0ac);
                break;
            case 3:
                ECLWarp(3103, 0xb664, 380, 0xbacc, 20, 0xbb74, 160, 0xc304);
                break;
            default:
                break;
            }
        } else if (stage == 4) {
            switch (portion) {
            case 1:
                ECLWarp(340, 0xa2d4, 20, 0xbafc);
                break;
            case 2:
                ECLWarp(1180, 0xa834, 20, 0xbafc);
                break;
            case 3:
                ECLWarp(2240, 0xa8ac, 360, 0xbed4);
                break;
            case 4:
                ECLWarp(3180, 0xabcc, 360, 0xbed4);
                break;
            case 5:
                ECLWarp(4903, 0xb2d8, 360, 0xbed4);
                STDJump(0xf48, 56, 6558);
                break;
            case 6:
                ECLWarp(5363, 0xb598, 360, 0xbed4);
                STDJump(0xf48, 56, 6558);
                break;
            default:
                break;
            }
        } else if (stage == 5) {
            switch (portion) {
            case 1:
                ECLWarp(340, 0xc538, 20, 0xdd80);
                break;
            case 2:
                ECLWarp(1180, 0xcab8, 20, 0xdd80);
                break;
            case 3:
                ECLWarp(2240, 0xcb30, 360, 0xe158);
                break;
            case 4:
                ECLWarp(3180, 0xce50, 360, 0xe158);
                break;
            case 5:
                ECLWarp(4903, 0xd55c, 360, 0xe158);
                STDJump(0xf48, 56, 6558);
                break;
            case 6:
                ECLWarp(5363, 0xd81c, 360, 0xe158);
                STDJump(0xf48, 56, 6558);
                break;
            default:
                break;
            }
        } else if (stage == 6) {
            switch (portion) {
            case 1:
                ECLWarp(200, 0xa674);
                break;
            case 2:
                ECLWarp(1040, 0xa694);
                break;
            case 3:
                ECLWarp(2990, 0xafdc);
                break;
            case 4:
                ECLWarp(4651, 0xb0e8);
                break;
            case 5:
                ECLWarp(5971, 0xb648);
                break;
            default:
                break;
            }
        } else if (stage == 7) {
            switch (portion) {
            case 1:
                ECLWarp(200, 0xc6f4);
                break;
            case 2:
                ECLWarp(1140, 0xc714);
                break;
            default:
                break;
            }
        } else if (stage == 8) {
            switch (portion) {
            case 1:
                ECLWarp(200, 0x10324);
                break;
            case 2:
                ECLWarp(1140, 0x10364);
                break;
            default:
                break;
            }
        } else if (stage == 9) {
            switch (portion) {
            case 1:
                ECLWarp(340, 0x12544, 20, 0x1535c);
                break;
            case 2:
                ECLWarp(1010, 0x127a8, 210, 0x155c0);
                break;
            case 3:
                ECLWarp(2900, 0x12b6c, 390, 0x15824);
                break;
            case 4:
                ECLWarp(5237, 0x132b8, 810, 0x15d60);
                break;
            case 5:
                ECLWarp(5947, 0x136b4, 20, 0x1535c);
                break;
            case 6:
                ECLWarp(6757, 0x13774, 20, 0x1535c);
                break;
            case 7:
                ECLWarp(8337, 0x151f4, 20, 0x1535c);
                break;
            default:
                break;
            }
        }
    }
    __declspec(noinline) void THPatch(ECLHelper& ecl, th_sections_t section)
    {
        int32_t diff = *((int32_t*)0x160f538);
        int32_t sub_id;
        //int32_t* std_flag = (int32_t*)0x4EA290;

        switch (section) {
        case THPrac::TH08::TH08_ST1_MID1:
            ECLWarp(2935, 0xab78, 20, 0xad18);
            break;
        case THPrac::TH08::TH08_ST1_MID2:
            ECLWarp(2935, 0xab78, 20, 0xad18);
            if (diff == 1 || diff == 2)
                break;
            ECLSetTime(ecl, 0x178c, 0, 22, 60);
            break;
        case THPrac::TH08::TH08_ST1_BOSS1:
            ECLWarp(4175, 0xac84, 0, -1);
            break;
        case THPrac::TH08::TH08_ST1_BOSS2:
            ECLWarp(4175, 0xac84, 0, -1);
            ECLSetTime(ecl, 0x3d30, 0, 38, 60);
            break;
        case THPrac::TH08::TH08_ST1_BOSS3:
            ECLWarp(4175, 0xac84, 0, -1);
            ECLTimeFix(0x3c68, 0, 6);
            ECLJump(ecl, 0x3d08, 0x4e0c, 0, 30);
            break;
        case THPrac::TH08::TH08_ST1_BOSS4:
            ECLWarp(4175, 0xac84, 0, -1);
            ECLTimeFix(0x3c68, 0, 6);
            ECLCallSub(ecl, 0x3d08, 33, 60);
            ECLTimeFix(0x4ed4, 0, 2);
            ECLSetTime(ecl, 0x4ef0, 0, 44);
            break;
        case THPrac::TH08::TH08_ST1_LS:
            ECLWarp(4176, 0xacd0, 0, -1);
            ECLTimeFix(0x58d4, 0, 5);
            ECLTimeFix(0x5944, 30, 6);
            ecl << ECLX(0x5948, (int16_t)0);
            ECLCheckTime(3000);
            break;
        case THPrac::TH08::TH08_ST2_MID1:
            ECLWarp(4870, 0xc020, 0, -1);
            ECLTimeFix(0x1ed8, 0, 7);
            ECLTimeFix(0x1f98, 60, 3);
            break;
        case THPrac::TH08::TH08_ST2_MID2:
            ECLWarp(4870, 0xc020, 0, -1);
            ECLTimeFix(0x1ed8, 0, 7);
            ECLTimeFix(0x1f98, 60, 3);

            ECLSetTime(ecl, 0x22b0, 0, 23);
            break;
        case THPrac::TH08::TH08_ST2_BOSS1:
            ECLWarp(4870, 0xc020, 0, -1);
            ECLJump(ecl, 0x1e34, 0x1ea8);
            ECLTimeFix(0x1ed8, 0, 7);
            ECLCallSub(ecl, 0x1f98, 27, 60, true);
            ECLJump(ecl, 0x37c0, 0x3880, 60);
            ecl << ECLX(0xc026, (int8_t)0x50);
            break;
        case THPrac::TH08::TH08_ST2_BOSS2:
            ECLWarp(4870, 0xc020, 0, -1);
            ECLJump(ecl, 0x1e34, 0x1ea8);
            ECLTimeFix(0x1ed8, 0, 7);
            ECLCallSub(ecl, 0x1f98, 27, 60, true);
            ECLJump(ecl, 0x37c0, 0x3880, 60);
            ecl << ECLX(0xc026, (int8_t)0x50);

            ECLSetTime(ecl, 0x389c, 0, (diff <= 1) ? 33 : 38, 60);
            break;
        case THPrac::TH08::TH08_ST2_BOSS3:
            ECLWarp(4870, 0xc020, 0, -1);
            ECLTimeFix(0x1ed8, 0, 7);
            ECLTimeFix(0x1f98, 0, 3);
            ECLCallSub(ecl, 0x1ed8, 27, 0);
            ecl << ECLX(0xc026, (int8_t)0x50) << ECLX(0x1f9c, (int16_t)0);

            ECLTimeFix(0x37e0, 0, 5);
            ECLCallSub(ecl, 0x389c, 29, 60);
            break;
        case THPrac::TH08::TH08_ST2_BOSS4:
            ECLWarp(4870, 0xc020, 0, -1);
            ECLTimeFix(0x1ed8, 0, 7);
            ECLTimeFix(0x1f98, 0, 3);
            ECLCallSub(ecl, 0x1ed8, 27, 0);
            ecl << ECLX(0xc026, (int8_t)0x50) << ECLX(0x1f9c, (int16_t)0);

            ECLTimeFix(0x37e0, 0, 5);
            ECLCallSub(ecl, 0x389c, 29, 60);

            ECLTimeFix(0x3e14, 0, 1);
            ECLSetTime(ecl, 0x3e30, 0, 44, 0);
            break;
        case THPrac::TH08::TH08_ST2_BOSS5:
            ECLWarp(4870, 0xc020, 0, -1);
            ECLTimeFix(0x1ed8, 0, 7);
            ECLTimeFix(0x1f98, 0, 3);
            ECLCallSub(ecl, 0x1ed8, 27, 0);
            ecl << ECLX(0xc026, (int8_t)0x50) << ECLX(0x1f9c, (int16_t)0);

            ECLTimeFix(0x37e0, 0, 5);
            ECLCallSub(ecl, 0x389c, 29, 60);

            ECLTimeFix(0x3e14, 0, 1);
            ECLSetTime(ecl, 0x3e30, 0, 51, 0);
            ecl << ECLX(0x3db0, (int16_t)0) << ECLX(0x3da8, 51) << ECLX(0x7524, 0)
                << ECLX(0x7508, (int16_t)0) << ECLX(0x7518, (int16_t)0);
            break;
        case THPrac::TH08::TH08_ST2_LS:
            ECLWarp(4871, 0xc07c, 0, -1);
            ECLTimeFix(0x49b0, 0, 5);
            ECLTimeFix(0x4a20, 30, 6);
            ecl << ECLX(0x4a24, (int16_t)0);
            ECLCheckTime(7200);
            break;
        case THPrac::TH08::TH08_ST3_MID1:
            ECLWarp(3080, 0xb5d8, 380, 0xbacc, 0, 0xbb74, 160, 0xc304);
            ECLTimeFix(0x2380, 0, 6);
            ECLTimeFix(0x2440, 60, 3);
            break;
        case THPrac::TH08::TH08_ST3_MID2:
            ECLWarp(3080, 0xb5d8, 380, 0xbacc, 0, 0xbb74, 160, 0xc304);
            ECLTimeFix(0x2380, 0, 6);
            ECLTimeFix(0x2440, 60, 3);

            ECLSetTime(ecl, 0x245c, 0, 30, 60);
            ecl << ECLX(0xb5de, (int8_t)0x50);
            break;
        case THPrac::TH08::TH08_ST3_BOSS1:
            ECLWarp(4663, 0xb87c, 0, -1, 0, -1, 0, -1);
            ecl << ECLX(0xb882, (int8_t)0x2c);
            break;
        case THPrac::TH08::TH08_ST3_BOSS2:
            ECLWarp(4663, 0xb87c, 0, -1, 0, -1, 0, -1);
            ecl << ECLX(0xb882, (int8_t)0x2c);

            if (!diff)
                break;
            ECLSetTime(ecl, 0x3f04, 0, 44, 60);
            break;
        case THPrac::TH08::TH08_ST3_BOSS3:
            ECLWarp(4663, 0xb87c, 0, -1, 0, -1, 0, -1);
            ecl << ECLX(0xb882, (int8_t)0x2c);

            if (!diff)
                break;
            ECLSetTime(ecl, 0x3f04, 0, 47, 60);
            ecl << ECLX(0x3ddc, (int16_t)0);
            break;
        case THPrac::TH08::TH08_ST3_BOSS4:
            ECLWarp(4663, 0xb87c, 0, -1, 0, -1, 0, -1);
            ecl << ECLX(0xb882, (int8_t)0x2c);
            ECLCallSub(ecl, 0x3f04, 38, 60);
            break;
        case THPrac::TH08::TH08_ST3_BOSS5:
            ECLWarp(4663, 0xb87c, 0, -1, 0, -1, 0, -1);
            ecl << ECLX(0xb882, (int8_t)0x2c);
            ECLCallSub(ecl, 0x3f04, 38, 60);

            switch (diff) {
            case 0:
                sub_id = 50;
                break;
            case 1:
                sub_id = 54;
                break;
            case 2:
                sub_id = 58;
                break;
            case 3:
                sub_id = 62;
                break;
            default:
                sub_id = 75;
            }
            ECLSetTime(ecl, 0x4748, 0, sub_id);
            ecl << ECLX(0x472c, 0);
            break;
        case THPrac::TH08::TH08_ST3_BOSS6:
            ECLWarp(4663, 0xb87c, 0, -1, 0, -1, 0, -1);
            ecl << ECLX(0xb882, (int8_t)0x2c);
            ECLCallSub(ecl, 0x3f04, 38, 60);

            ECLSetTime(ecl, 0x4748, 0, 67);
            ECLJump(ecl, 0x462c, 0x46dc, 0, 0);
            ECLJump(ecl, 0x3d80, 0x3df0, 0, 0);
            ecl << ECLX(0x472c, 0);
            break;
        case THPrac::TH08::TH08_ST3_LS:
            ECLWarp(4663, 0xb8cc, 0, -1, 0, -1, 0, -1);
            ECLTimeFix(0x54bc, 0, 5);
            ECLTimeFix(0x552c, 30, 6);
            ecl << ECLX(0x5530, (int16_t)0);
            ECLCheckTime(8800);
            break;
        case THPrac::TH08::TH08_ST4A_BOSS1:
            ECLWarp(4962, 0xb280, 360, 0xbed4);
            STDJump(0xf48, 38, 4562);
            STDStage4Fix();
            break;
        case THPrac::TH08::TH08_ST4A_BOSS2:
            ECLWarp(4962, 0xb280, 360, 0xbed4);
            STDJump(0xf48, 38, 4562);
            STDStage4Fix();

            ECLSetTime(ecl, 0x188c, 0, 28, 60);
            break;
        case THPrac::TH08::TH08_ST4A_BOSS3:
            ECLWarp(4962, 0xb280, 360, 0xbed4);
            ECLTimeFix(0x17d0, 0, 6);
            ECLCallSub(ecl, 0x1870, 19, 0);
            STDJump(0xf48, 38, 4562);
            STDStage4Fix();
            break;
        case THPrac::TH08::TH08_ST4A_BOSS4:
            ECLWarp(4962, 0xb280, 360, 0xbed4);
            ECLCallSub(ecl, 0x1870, 19, 60);
            STDJump(0xf48, 38, 4562);
            STDStage4Fix();

            ECLSetTime(ecl, 0x22f0, 0, 30, 0);
            break;
        case THPrac::TH08::TH08_ST4A_BOSS5:
            ECLWarp(6923, 0xba50, 0, -1);
            ecl << pair(0x2e10, 0);
            STDJump(0xf48, 56, 6558);
            break;
        case THPrac::TH08::TH08_ST4A_BOSS6:
            ECLWarp(6923, 0xba50, 0, -1);
            STDJump(0xf48, 56, 6558);

            ECLSetTime(ecl, 0x2fa8, 0, 33, 0);
            ecl << pair(0xba56, (int8_t)0x50);
            break;
        case THPrac::TH08::TH08_ST4A_BOSS7:
            ECLWarp(6923, 0xba50, 0, -1);
            ecl << pair(0x2e10, 0) << pair(0x2e1c, 26);
            STDStage4Fix();
            ecl << pair(0xba56, (int8_t)0x50);
            break;
        case THPrac::TH08::TH08_ST4A_BOSS8:
            ECLWarp(6923, 0xba50, 0, -1);
            ecl << pair(0x2e10, 0) << pair(0x2e1c, 26);
            STDStage4Fix();
            ecl << pair(0xba56, (int8_t)0x50);

            ECLSetTime(ecl, 0x3850, 0, 41, 60);
            ecl << pair(0x3764, (int16_t)0);
            ecl << pair(0x3818, 60) << pair(0x3834, 60);
            break;
        case THPrac::TH08::TH08_ST4A_BOSS9:
            ECLWarp(6923, 0xba50, 0, -1);
            ecl << pair(0x2e10, 0) << pair(0x2e1c, 26);
            STDStage4Fix();
            ecl << pair(0xba56, (int8_t)0x50);

            ECLCallSub(ecl, 0x3850, 45, 60);
            ecl << pair(0x3818, 60) << pair(0x3834, 60);
            ecl << pair(0x3764, (int16_t)0) << pair(0x37d4, (int16_t)0)
                << pair(0x7d04, 0) << pair(0x7d10, 0)
                << pair(0x7c68, (int16_t)0) << pair(0x7c78, (int16_t)0);
            break;
        case THPrac::TH08::TH08_ST4A_LS:
            STDStage4Fix();
            ECLWarp(6923, 0xbaac, 0, -1);
            ECLTimeFix(0x4168, 0, 5);
            ECLTimeFix(0x41d8, 30, 6);
            ecl << ECLX(0x41dc, (int16_t)0);
            ECLCheckTime(9999);

            ecl.SetPos(0x41c0);
            ecl << 0 << 0x00180093 << 0x0000ff00 << 2;
            break;
        case THPrac::TH08::TH08_ST4B_BOSS1:
            ECLWarp(4962, 0xd504, 360, 0xe158);
            STDJump(0xf48, 38, 4562);
            STDStage4Fix();
            break;
        case THPrac::TH08::TH08_ST4B_BOSS2:
            ECLWarp(4962, 0xd504, 360, 0xe158);
            STDJump(0xf48, 38, 4562);
            STDStage4Fix();

            ECLSetTime(ecl, 0x18d0, 0, 36, 60);
            break;
        case THPrac::TH08::TH08_ST4B_BOSS3:
            ECLWarp(4962, 0xd504, 360, 0xe158);
            ECLTimeFix(0x1814, 0, 5);
            ECLCallSub(ecl, 0x18b4, 22, 0);
            ecl << pair(0x2554, 120) << pair(0x2570, 120);
            STDJump(0xf48, 38, 4562);
            STDStage4Fix();
            break;
        case THPrac::TH08::TH08_ST4B_BOSS4:
            ECLWarp(4962, 0xd504, 360, 0xe158);
            ECLTimeFix(0x1814, 0, 5);
            ECLCallSub(ecl, 0x18b4, 22, 60);
            STDJump(0xf48, 38, 4562);
            STDStage4Fix();

            ECLSetTime(ecl, 0x2570, 0, 41, 0);
            break;
        case THPrac::TH08::TH08_ST4B_BOSS5:
            ECLWarp(6923, 0xdcd4, 0, -1);
            ecl << pair(0x2f14, 0) << pair(0x3030, 120) << pair(0x304c, 120);
            STDJump(0xf48, 56, 6558);
            break;
        case THPrac::TH08::TH08_ST4B_BOSS6:
            ECLWarp(6923, 0xdcd4, 0, -1);
            ecl << pair(0x2f14, 0) << pair(0x3030, 60);
            STDJump(0xf48, 56, 6558);

            ecl << pair(0xdcda, (int8_t)0x50);
            ECLSetTime(ecl, 0x304c, 0, 47, 60);
            break;
        case THPrac::TH08::TH08_ST4B_BOSS7:
            ECLWarp(6923, 0xdcd4, 0, -1);
            ecl << pair(0xdcda, (int8_t)0x50);
            ecl << pair(0x2f14, 0) << pair(0x2f20, 31)
                << pair(0x38b8, 120) << pair(0x38d4, 120);
            STDStage4Fix();
            break;
        case THPrac::TH08::TH08_ST4B_BOSS8:
            ECLWarp(6923, 0xdcd4, 0, -1);
            ecl << pair(0xdcda, (int8_t)0x50);
            ecl << pair(0x2f14, 0) << pair(0x2f20, 31)
                << pair(0x38b8, 120) << pair(0x38d4, 120);
            STDStage4Fix();

            ECLSetTime(ecl, 0x38d4, 0, 55, 60);
            ecl << pair(0x38b8, 60);
            break;
        case THPrac::TH08::TH08_ST4B_BOSS9:
            ECLWarp(6923, 0xdcd4, 0, -1);
            ecl << pair(0xdcda, (int8_t)0x50);
            ecl << pair(0x2f14, 0) << pair(0x2f20, 31);
            STDStage4Fix();

            ECLCallSub(ecl, 0x38d4, 61, 60);
            ecl << pair(0x38b8, 60)
                << pair(0x8b48, 0) << pair(0x8b54, 0) << pair(0x8b68, 0)
                << pair(0x8aac, (int16_t)0) << pair(0x8abc, (int16_t)0)
                << pair(0x3888, (int16_t)0);
            break;
        case THPrac::TH08::TH08_ST4B_LS:
            STDStage4Fix();
            ECLWarp(6923, 0xdd30, 0, -1);
            ECLTimeFix(0x4374, 0, 5);
            ECLTimeFix(0x43e4, 30, 6);
            ecl << ECLX(0x43e8, (int16_t)0);
            ECLCheckTime(8500);

            ecl.SetPos(0x43cc);
            ecl << 0 << 0x00180093 << 0x0000ff00 << 2;
            break;
        case THPrac::TH08::TH08_ST5_MID1:
            ECLWarp(4530, 0xb0bc);
            ECLTimeFix(0x3704, 0, 6);
            ECLTimeFix(0x37c4, 60, 3);
            break;
        case THPrac::TH08::TH08_ST5_MID2:
            ECLWarp(4530, 0xb0bc);
            ECLTimeFix(0x3704, 0, 9);
            ecl << pair(0x37c8, (int16_t)0) << pair(0x37ec, 45);
            ECLJump(ecl, 0x4360, 0x437c, 119, 0);
            ECLTimeFix(0x437c, 120, 4);
            ECLSetHealth(ecl, 0x37e0, 0, 1);
            break;
        case THPrac::TH08::TH08_ST5_BOSS1:
            MSGNameFix();
            ECLWarp(7481, 0xb79c);
            ecl << pair(0xb7a2, (int8_t)0x78);
            ECLCallSub(ecl, 0x4c6c, 51);
            break;
        case THPrac::TH08::TH08_ST5_BOSS2:
            MSGNameFix();
            ECLWarp(7481, 0xb79c);
            ecl << pair(0xb7a2, (int8_t)0x78);
            ECLCallSub(ecl, 0x4c6c, 51);

            ECLSetTime(ecl, 0x4f78, 0, 62, 60);
            break;
        case THPrac::TH08::TH08_ST5_BOSS3:
            MSGNameFix();
            ECLWarp(7481, 0xb79c);
            ecl << pair(0xb7a2, (int8_t)0x78);
            ECLCallSub(ecl, 0x4c6c, 51);
            ECLTimeFix(0x4ebc, 0, 5);
            ECLCallSub(ecl, 0x4f5c, 53, 0);
            ECLTimeFix(0x5214, 60, 9);
            ecl << pair(0x52cc, (int16_t)0);
            break;
        case THPrac::TH08::TH08_ST5_BOSS4:
            MSGNameFix();
            ECLWarp(7481, 0xb79c);
            ecl << pair(0xb7a2, (int8_t)0x78);
            ECLCallSub(ecl, 0x4c6c, 51);
            ECLTimeFix(0x4ebc, 0, 5);
            ECLCallSub(ecl, 0x4f5c, 53, 0);
            ECLTimeFix(0x5214, 60, 9);
            ecl << pair(0x52cc, (int16_t)0);

            ECLSetTime(ecl, 0x52d8, 0, 66, 60);
            break;
        case THPrac::TH08::TH08_ST5_BOSS5:
            MSGNameFix();
            ECLWarp(7481, 0xb79c);
            ecl << pair(0xb7a2, (int8_t)0x78);
            ECLCallSub(ecl, 0x4c6c, 51);
            ECLTimeFix(0x4ebc, 0, 5);
            ECLCallSub(ecl, 0x4f5c, 56, 0);
            ECLTimeFix(0x5744, 60, 11);
            ecl << pair(0x5830, (int16_t)0);
            break;
        case THPrac::TH08::TH08_ST5_BOSS6:
            MSGNameFix();
            ECLWarp(7481, 0xb79c);
            ecl << pair(0xb7a2, (int8_t)0x78);
            ECLCallSub(ecl, 0x4c6c, 51);
            ECLTimeFix(0x4ebc, 0, 5);
            ECLCallSub(ecl, 0x4f5c, 56, 0);
            ECLTimeFix(0x5744, 60, 11);
            ecl << pair(0x5830, (int16_t)0);

            ECLSetTime(ecl, 0x583c, 0, 63, 60);
            break;
        case THPrac::TH08::TH08_ST5_BOSS7:
            MSGNameFix();
            ECLWarp(7481, 0xb79c);
            ecl << pair(0xb7a2, (int8_t)0x78);
            ECLCallSub(ecl, 0x4c6c, 51);
            ECLTimeFix(0x4ebc, 0, 5);
            ECLCallSub(ecl, 0x4f5c, 56, 0);
            ECLTimeFix(0x5744, 60, 11);
            ecl << pair(0x5830, (int16_t)0);

            ECLSetTime(ecl, 0x583c, 0, 75, 60);
            ecl << pair(0x5798, (int16_t)0) << pair(0x57b8, (int16_t)0);
            break;
        case THPrac::TH08::TH08_ST5_LS:
            MSGNameFix();
            ECLWarp(7484, 0xb820);
            ECLTimeFix(0x5e24, 0, 5);
            ECLTimeFix(0x5e94, 30, 6);
            ecl << ECLX(0x5e98, (int16_t)0);
            ECLCheckTime(9999);
            break;
        case THPrac::TH08::TH08_ST6A_MID1:
            ECLWarp(3400, 0xc7d4);
            ecl << pair(0xc7da, (int8_t)0x44);
            ECLTimeFix(0x1de4, 0, 6);
            ecl << pair(0x1ea4, 60) << pair(0x1dd4, (int16_t)0);
            ECLJump(ecl, 0x1ec0, 0x1ee0, 0, 60);
            break;
        case THPrac::TH08::TH08_ST6A_MID2:
            ECLWarp(3400, 0xc7d4);
            ecl << pair(0xc7da, (int8_t)0x44);
            ECLTimeFix(0x1de4, 0, 6);
            ecl << pair(0x1ea4, 60) << pair(0x1dd4, (int16_t)0);
            ECLJump(ecl, 0x1ec0, 0x1ee0, 0, 60);

            ECLSetTime(ecl, 0x1fdc, 0, 21);
            break;
        case THPrac::TH08::TH08_ST6A_BOSS1:
            *((int32_t*)0x4ea290) = 1;
            MSGNameFix();
            ECLWarp(4022, 0xc838);
            ecl << pair(0xc83e, (int8_t)0x38);
            ECLCallSub(ecl, 0x367c, 33);
            break;
        case THPrac::TH08::TH08_ST6A_BOSS2:
            *((int32_t*)0x4ea290) = 1;
            MSGNameFix();
            ECLWarp(4022, 0xc838);
            ecl << pair(0xc83e, (int8_t)0x38);
            ECLCallSub(ecl, 0x367c, 33);

            ECLSetTime(ecl, 0x3de4, 0, 52, 60);
            break;
        case THPrac::TH08::TH08_ST6A_BOSS3:
            *((int32_t*)0x4ea290) = 1;
            MSGNameFix();
            ECLWarp(4022, 0xc838);
            ecl << pair(0xc83e, (int8_t)0x38);
            ECLCallSub(ecl, 0x367c, 33);
            ECLCallSub(ecl, 0x3dc8, 36);
            ECLTimeFix(0x3d28, 0, 6);
            ECLTimeFix(0x454c, 60, 8);
            break;
        case THPrac::TH08::TH08_ST6A_BOSS4:
            *((int32_t*)0x4ea290) = 1;
            MSGNameFix();
            ECLWarp(4022, 0xc838);
            ecl << pair(0xc83e, (int8_t)0x38);
            ECLCallSub(ecl, 0x367c, 33);
            ECLCallSub(ecl, 0x3dc8, 36);
            ECLTimeFix(0x3d28, 0, 6);
            ECLTimeFix(0x454c, 60, 8);

            ECLSetTime(ecl, 0x4600, 0, 56, 60);
            break;
        case THPrac::TH08::TH08_ST6A_BOSS5:
            *((int32_t*)0x4ea290) = 1;
            MSGNameFix();
            ECLWarp(4022, 0xc838);
            ecl << pair(0xc83e, (int8_t)0x38);
            ECLCallSub(ecl, 0x367c, 33);
            ECLCallSub(ecl, 0x3dc8, 40);
            ECLTimeFix(0x3d28, 0, 6);
            ECLTimeFix(0x4c98, 60, 8);
            break;
        case THPrac::TH08::TH08_ST6A_BOSS6:
            *((int32_t*)0x4ea290) = 1;
            MSGNameFix();
            ECLWarp(4022, 0xc838);
            ecl << pair(0xc83e, (int8_t)0x38);
            ECLCallSub(ecl, 0x367c, 33);
            ECLCallSub(ecl, 0x3dc8, 40);
            ECLTimeFix(0x3d28, 0, 6);
            ECLTimeFix(0x4c98, 60, 8);

            ECLSetTime(ecl, 0x4d4c, 0, 63, 60);
            break;
        case THPrac::TH08::TH08_ST6A_BOSS7:
            *((int32_t*)0x4ea290) = 1;
            MSGNameFix();
            ECLWarp(4022, 0xc838);
            ecl << pair(0xc83e, (int8_t)0x38);
            ECLCallSub(ecl, 0x367c, 33);
            ECLCallSub(ecl, 0x3dc8, 44);
            ECLTimeFix(0x3d28, 0, 6);
            ECLTimeFix(0x5534, 60, 8);
            break;
        case THPrac::TH08::TH08_ST6A_BOSS8:
            *((int32_t*)0x4ea290) = 1;
            MSGNameFix();
            ECLWarp(4022, 0xc838);
            ecl << pair(0xc83e, (int8_t)0x38);
            ECLCallSub(ecl, 0x367c, 33);
            ECLCallSub(ecl, 0x3dc8, 44);
            ECLTimeFix(0x3d28, 0, 6);
            ECLTimeFix(0x5534, 60, 8);

            ECLSetTime(ecl, 0x55e8, 0, 68, 60);
            break;
        case THPrac::TH08::TH08_ST6A_BOSS9:
            //*((int32_t*)0x4ea290) = 1;
            MSGNameFix();
            ECLWarp(4022, 0xc838);
            ecl << pair(0xc83e, (int8_t)0x38);
            ECLCallSub(ecl, 0x367c, 33);
            ECLCallSub(ecl, 0x3dc8, 72);
            ECLTimeFix(0x3d28, 0, 6);
            ECLTimeFix(0x9a44, 60, 6);
            ecl << pair(0x3cd8, (int16_t)0)
                << pair(0x99f8, (int16_t)0) << pair(0x9a08, (int16_t)0);
            break;
        case THPrac::TH08::TH08_ST6A_LS:
            MSGNameFix();
            ECLWarp(4083, 0xc89c);
            ecl << pair(0xc8a2, (int8_t)0x44);
            ECLCallSub(ecl, 0x376c, 30);
            ecl << pair(0x3760, (int16_t)0) << pair(0x3918, (int16_t)0)
                << pair(0x3938, (int16_t)0);
            ecl << pair(0xc8e0, 4084) << pair(0xc900, 4084)
                << pair(0xc90c, 4084) << pair(0xc918, 4084);
            break;
        case THPrac::TH08::TH08_ST6B_MID1:
            ECLWarp(3490, 0x104e4);
            ecl << pair(0x104ea, (int8_t)0x44);
            ECLTimeFix(0x1bfc, 0, 6);
            ecl << pair(0x1cbc, 60) << pair(0x1bec, (int16_t)0);
            ECLCallSub(ecl, 0x1cd8, 15, 60);
            break;
        case THPrac::TH08::TH08_ST6B_MID2:
            ECLWarp(3490, 0x104e4);
            ecl << pair(0x104ea, (int8_t)0x44);
            ECLTimeFix(0x1bfc, 0, 6);
            ecl << pair(0x1cbc, 60) << pair(0x1bec, (int16_t)0);
            ECLCallSub(ecl, 0x1cd8, 15, 60);

            ECLSetTime(ecl, 0x1df4, 0, 19);
            break;
        case THPrac::TH08::TH08_ST6B_BOSS1:
            *((int32_t*)0x4ea290) = 1;
            MSGNameFix();
            ECLWarp(4112, 0x10548);
            ecl << pair(0x1054e, (int8_t)0x38);
            ECLCallSub(ecl, 0x3a28, 27);
            break;
        case THPrac::TH08::TH08_ST6B_BOSS2:
            *((int32_t*)0x4ea290) = 1;
            MSGNameFix();
            ECLWarp(4112, 0x10548);
            ecl << pair(0x1054e, (int8_t)0x38);
            ECLCallSub(ecl, 0x3a28, 27);

            ECLSetTime(ecl, 0x3c24, 0, 50, 60);
            break;
        case THPrac::TH08::TH08_ST6B_BOSS3:
            *((int32_t*)0x4ea290) = 1;
            MSGNameFix();
            ECLWarp(4112, 0x10548);
            ecl << pair(0x1054e, (int8_t)0x38);
            ECLCallSub(ecl, 0x3a28, 27);

            ECLCallSub(ecl, 0x3c08, 31);
            ECLTimeFix(0x3b68, 0, 6);
            ECLTimeFix(0x4370, 60, 8);
            break;
        case THPrac::TH08::TH08_ST6B_BOSS4:
            *((int32_t*)0x4ea290) = 1;
            MSGNameFix();
            ECLWarp(4112, 0x10548);
            ecl << pair(0x1054e, (int8_t)0x38);
            ECLCallSub(ecl, 0x3a28, 27);

            ECLCallSub(ecl, 0x3c08, 31);
            ECLTimeFix(0x3b68, 0, 6);
            ECLTimeFix(0x4370, 60, 8);

            ECLSetTime(ecl, 0x4424, 0, 55, 60);
            break;
        case THPrac::TH08::TH08_ST6B_BOSS5:
            *((int32_t*)0x4ea290) = 1;
            MSGNameFix();
            ECLWarp(4112, 0x10548);
            ecl << pair(0x1054e, (int8_t)0x38);
            ECLCallSub(ecl, 0x3a28, 27);

            ECLCallSub(ecl, 0x3c08, 34);
            ECLTimeFix(0x3b68, 0, 6);
            ECLTimeFix(0x4c14, 60, 9);
            break;
        case THPrac::TH08::TH08_ST6B_BOSS6:
            *((int32_t*)0x4ea290) = 1;
            MSGNameFix();
            ECLWarp(4112, 0x10548);
            ecl << pair(0x1054e, (int8_t)0x38);
            ECLCallSub(ecl, 0x3a28, 27);

            ECLCallSub(ecl, 0x3c08, 34);
            ECLTimeFix(0x3b68, 0, 6);
            ECLTimeFix(0x4c14, 60, 9);

            ECLSetTime(ecl, 0x4cec, 0, 61, 60);
            break;
        case THPrac::TH08::TH08_ST6B_BOSS7:
            *((int32_t*)0x4ea290) = 1;
            MSGNameFix();
            ECLWarp(4112, 0x10548);
            ecl << pair(0x1054e, (int8_t)0x38);
            ECLCallSub(ecl, 0x3a28, 27);

            ECLCallSub(ecl, 0x3c08, 38);
            ECLTimeFix(0x3b68, 0, 6);
            ECLTimeFix(0x5570, 60, 8);
            break;
        case THPrac::TH08::TH08_ST6B_BOSS8:
            *((int32_t*)0x4ea290) = 1;
            MSGNameFix();
            ECLWarp(4112, 0x10548);
            ecl << pair(0x1054e, (int8_t)0x38);
            ECLCallSub(ecl, 0x3a28, 27);

            ECLCallSub(ecl, 0x3c08, 38);
            ECLTimeFix(0x3b68, 0, 6);
            ECLTimeFix(0x5570, 60, 8);

            ECLSetTime(ecl, 0x5624, 0, 67, 60);
            break;
        case THPrac::TH08::TH08_ST6B_BOSS9:
            MSGNameFix();
            ECLWarp(4112, 0x10548);
            ecl << pair(0x1054e, (int8_t)0x38);
            ECLCallSub(ecl, 0x3a28, 27);

            ECLCallSub(ecl, 0x3c08, 70);
            ECLTimeFix(0x3b68, 0, 6);
            ECLTimeFix(0xa920, 60, 7);
            ecl << pair(0x3b18, (int16_t)0)
                << pair(0xa8e4, (int16_t)0) << pair(0xa8d4, (int16_t)0);
            break;
        case THPrac::TH08::TH08_ST6B_LS1:
            *((int32_t*)0x4ea290) = 1;
            MSGNameFix();
            ECLWarp(4234, 0x105ac);
            break;
        case THPrac::TH08::TH08_ST6B_LS2:
            *((int32_t*)0x4ea290) = 1;
            MSGNameFix();
            ECLWarp(4235, 0x105ec);
            break;
        case THPrac::TH08::TH08_ST6B_LS3:
            *((int32_t*)0x4ea290) = 1;
            MSGNameFix();
            ECLWarp(4236, 0x1062c);
            break;
        case THPrac::TH08::TH08_ST6B_LS4:
            *((int32_t*)0x4ea290) = 1;
            MSGNameFix();
            ECLWarp(4237, 0x1066c);
            break;
        case THPrac::TH08::TH08_ST6B_LS5:
            *((int32_t*)0x4ea290) = 1;
            MSGNameFix();
            ECLWarp(4238, 0x106ac);
            break;
        case THPrac::TH08::TH08_ST7_MID1:
            ECLWarp(5155, 0x13268, 810, 0x15d60);
            ecl << pair(0x1326e, (int8_t)0x44) << pair(0x3e10, 60);
            ECLTimeFix(0x3d50, 0, 6);
            ECLCallSub(ecl, 0x3e2c, 51, 60);
            break;
            break;
        case THPrac::TH08::TH08_ST7_MID2:
            ECLWarp(5155, 0x13268, 810, 0x15d60);
            ecl << pair(0x1326e, (int8_t)0x44);
            ECLTimeFix(0x3d50, 0, 6);
            ECLCallSub(ecl, 0x3e10, 55);
            ECLTimeFix(0x4b34, 60, 6);
            break;
        case THPrac::TH08::TH08_ST7_MID3:
            ECLWarp(5155, 0x13268, 810, 0x15d60);
            ecl << pair(0x1326e, (int8_t)0x44);
            ECLTimeFix(0x3d50, 0, 6);
            ECLCallSub(ecl, 0x3e10, 59);
            ECLTimeFix(0x5418, 60, 6);
            break;
        case THPrac::TH08::TH08_ST7_END_NS1:
            MSGNameFix();
            ECLWarp(9497, 0x152c8, 0, -1);
            ecl << pair(0x152ce, (int8_t)0x38);
            ECLCallSub(ecl, 0x65c4, 66);
            break;
        case THPrac::TH08::TH08_ST7_END_S1:
            MSGNameFix();
            ECLWarp(9497, 0x152c8, 0, -1);
            ecl << pair(0x152ce, (int8_t)0x38);
            ECLCallSub(ecl, 0x65c4, 66);

            ECLSetTime(ecl, 0x67b0, 0, 93, 60);
            break;
        case THPrac::TH08::TH08_ST7_END_NS2:
            MSGNameFix();
            ECLWarp(9497, 0x152c8, 0, -1);
            ecl << pair(0x152ce, (int8_t)0x38);
            ECLCallSub(ecl, 0x65c4, 66);
            ecl << pair(0x66a4, (int16_t)0);
            ECLCallSub(ecl, 0x67b0, 68, 60);
            ecl << pair(0x6aac, (int16_t)0);
            break;
        case THPrac::TH08::TH08_ST7_END_S2:
            MSGNameFix();
            ECLWarp(9497, 0x152c8, 0, -1);
            ecl << pair(0x152ce, (int8_t)0x38);
            ECLCallSub(ecl, 0x65c4, 66);
            ecl << pair(0x66a4, (int16_t)0);
            ECLCallSub(ecl, 0x67b0, 68, 60);
            ecl << pair(0x6aac, (int16_t)0);

            ECLSetTime(ecl, 0x6b9c, 0, 97);
            break;
        case THPrac::TH08::TH08_ST7_END_NS3:
            MSGNameFix();
            ECLWarp(9497, 0x152c8, 0, -1);
            ecl << pair(0x152ce, (int8_t)0x38);
            ECLCallSub(ecl, 0x65c4, 66);
            ecl << pair(0x66a4, (int16_t)0);
            ECLCallSub(ecl, 0x67b0, 70, 60);
            ecl << pair(0x6e74, (int16_t)0);
            break;
        case THPrac::TH08::TH08_ST7_END_S3:
            MSGNameFix();
            ECLWarp(9497, 0x152c8, 0, -1);
            ecl << pair(0x152ce, (int8_t)0x38);
            ECLCallSub(ecl, 0x65c4, 66);
            ecl << pair(0x66a4, (int16_t)0);
            ECLCallSub(ecl, 0x67b0, 70, 60);
            ecl << pair(0x6e74, (int16_t)0);

            ECLSetTime(ecl, 0x6f64, 0, 100);
            break;
        case THPrac::TH08::TH08_ST7_END_NS4:
            MSGNameFix();
            ECLWarp(9497, 0x152c8, 0, -1);
            ecl << pair(0x152ce, (int8_t)0x38);
            ECLCallSub(ecl, 0x65c4, 66);
            ecl << pair(0x66a4, (int16_t)0);
            ECLCallSub(ecl, 0x67b0, 72, 60);
            ecl << pair(0x724c, (int16_t)0);
            ecl << pair(0x71dc, (int16_t)0);
            break;
        case THPrac::TH08::TH08_ST7_END_S4:
            MSGNameFix();
            ECLWarp(9497, 0x152c8, 0, -1);
            ecl << pair(0x152ce, (int8_t)0x38);
            ECLCallSub(ecl, 0x65c4, 66);
            ecl << pair(0x66a4, (int16_t)0);
            ECLCallSub(ecl, 0x67b0, 72, 60);
            ecl << pair(0x724c, (int16_t)0);
            ecl << pair(0x71dc, (int16_t)0);

            ECLSetTime(ecl, 0x733c, 0, 103);
            break;
        case THPrac::TH08::TH08_ST7_END_NS5:
            MSGNameFix();
            ECLWarp(9497, 0x152c8, 0, -1);
            ecl << pair(0x152ce, (int8_t)0x38);
            ECLCallSub(ecl, 0x65c4, 66);
            ecl << pair(0x66a4, (int16_t)0);
            ECLCallSub(ecl, 0x67b0, 74, 60);
            ecl << pair(0x7548, (int16_t)0);
            break;
        case THPrac::TH08::TH08_ST7_END_S5:
            MSGNameFix();
            ECLWarp(9497, 0x152c8, 0, -1);
            ecl << pair(0x152ce, (int8_t)0x38);
            ECLCallSub(ecl, 0x65c4, 66);
            ecl << pair(0x66a4, (int16_t)0);
            ECLCallSub(ecl, 0x67b0, 74, 60);
            ecl << pair(0x7548, (int16_t)0);

            ECLSetTime(ecl, 0x7638, 0, 108);
            break;
        case THPrac::TH08::TH08_ST7_END_NS6:
            MSGNameFix();
            ECLWarp(9497, 0x152c8, 0, -1);
            ecl << pair(0x152ce, (int8_t)0x38);
            ECLCallSub(ecl, 0x65c4, 66);
            ECLTimeFix(0x66f4, 0, 5);
            ecl << pair(0x66a4, (int16_t)0);
            ECLCallSub(ecl, 0x6794, 76, 0);
            ecl << pair(0x7844, (int16_t)0);
            ECLTimeFix(0x7810, 60, 15);
            break;
        case THPrac::TH08::TH08_ST7_END_S6:
            MSGNameFix();
            ECLWarp(9497, 0x152c8, 0, -1);
            ecl << pair(0x152ce, (int8_t)0x38);
            ECLCallSub(ecl, 0x65c4, 66);
            ECLTimeFix(0x66f4, 0, 5);
            ecl << pair(0x66a4, (int16_t)0);
            ECLCallSub(ecl, 0x6794, 76, 0);
            ecl << pair(0x7844, (int16_t)0);
            ECLTimeFix(0x7810, 60, 15);

            ECLSetTime(ecl, 0x7944, 0, 113, 60);
            break;
        case THPrac::TH08::TH08_ST7_END_NS7:
            MSGNameFix();
            ECLWarp(9497, 0x152c8, 0, -1);
            ecl << pair(0x152ce, (int8_t)0x38);
            ECLCallSub(ecl, 0x65c4, 66);
            ECLTimeFix(0x66f4, 0, 5);
            ecl << pair(0x66a4, (int16_t)0);
            ECLCallSub(ecl, 0x6794, 78, 0);
            ecl << pair(0x7bcc, (int16_t)0);
            ECLTimeFix(0x7b98, 60, 15);
            break;
        case THPrac::TH08::TH08_ST7_END_S7:
            MSGNameFix();
            ECLWarp(9497, 0x152c8, 0, -1);
            ecl << pair(0x152ce, (int8_t)0x38);
            ECLCallSub(ecl, 0x65c4, 66);
            ECLTimeFix(0x66f4, 0, 5);
            ecl << pair(0x66a4, (int16_t)0);
            ECLCallSub(ecl, 0x6794, 78, 0);
            ecl << pair(0x7bcc, (int16_t)0);
            ECLTimeFix(0x7b98, 60, 15);

            ECLSetTime(ecl, 0x7ccc, 0, 118, 60);
            break;
        case THPrac::TH08::TH08_ST7_END_NS8:
            MSGNameFix();
            ECLWarp(9497, 0x152c8, 0, -1);
            ecl << pair(0x152ce, (int8_t)0x38);
            ECLCallSub(ecl, 0x65c4, 66);
            ECLTimeFix(0x66f4, 0, 5);
            ecl << pair(0x66a4, (int16_t)0);
            ECLCallSub(ecl, 0x6794, 80, 0);
            ecl << pair(0x7f44, (int16_t)0);
            ECLTimeFix(0x7f10, 60, 15);
            break;
        case THPrac::TH08::TH08_ST7_END_S8:
            MSGNameFix();
            ECLWarp(9497, 0x152c8, 0, -1);
            ecl << pair(0x152ce, (int8_t)0x38);
            ECLCallSub(ecl, 0x65c4, 66);
            ECLTimeFix(0x66f4, 0, 5);
            ecl << pair(0x66a4, (int16_t)0);
            ECLCallSub(ecl, 0x6794, 80, 0);
            ecl << pair(0x7f44, (int16_t)0);
            ECLTimeFix(0x7f10, 60, 15);

            ECLSetTime(ecl, 0x8044, 0, 126, 60);
            break;
        case THPrac::TH08::TH08_ST7_END_S9:
            MSGNameFix();
            ECLWarp(9497, 0x152c8, 0, -1);
            ecl << pair(0x152ce, (int8_t)0x38);
            ECLCallSub(ecl, 0x65c4, 66);
            ecl << pair(0x66a4, (int16_t)0);
            ECLCallSub(ecl, 0x67b0, 82, 60);

            ECLJump(ecl, 0x95f4, 0x98c4, 10);
            ECLJump(ecl, 0x98dc, 0x9910, 130, 10);
            break;
        case THPrac::TH08::TH08_ST7_END_S10:
            MSGNameFix();
            ECLWarp(9497, 0x152c8, 0, -1);
            ecl << pair(0x152ce, (int8_t)0x38);
            ECLCallSub(ecl, 0x65c4, 66);
            ecl << pair(0x66a4, (int16_t)0);
            ecl.SetPos(0x67b0);
            ecl << 60 << 0x0014004e << 0x0000ff00 << 0x43800000 << 0x42000000;
            ECLCallSub(ecl, 0x67c4, 83, 60);
            ecl << pair(0x8408, (int16_t)0);
            break;
        case THPrac::TH08::TH08_ST7_END_LS:
            MSGNameFix();
            ECLWarp(9678, 0x1530c, 0, -1);
            ECLJump(ecl, 0x851c, 0x8544);
            ECLCheckTime(10);
            break;
        default:
            break;
        }
    }
    __declspec(noinline) void THSectionPatch()
    {
        ECLHelper ecl;
        ecl.SetBaseAddr((void*)GetMemAddr(0x4ECCB8));

        auto section = thPracParam.section;
        if (section >= 10000 && section < 20000) {
            int stage = (section - 10000) / 100;
            int portionId = (section - 10000) % 100;
            THStageWarp(ecl, stage, portionId);
            THStage4ANM();
        } else {
            THPatch(ecl, (th_sections_t)section);
            THStage4ANM();
        }
    }

    void THSetPoint()
    {
        int32_t pPointFunc = 0x440470;
        int32_t* point_stage = (int32_t*)GetMemAddr(0x160f510, 0x2c);
        int32_t* point_total_1 = (int32_t*)GetMemAddr(0x160f510, 0x30);
        int32_t* point_total_2 = (int32_t*)0x164cf9c;
        if (thPracParam.point) {
            *point_stage = *point_total_1 = *point_total_2 = thPracParam.point;
        } else {
            *point_stage = thPracParam.point_stage;
            *point_total_1 = *point_total_2 = thPracParam.point_total;
        }

        int32_t temp = 0;
        int32_t* award = (int32_t*)GetMemAddr(0x160f510, 0x34);
        while (true) {
            __asm
                {
					pushad
					call pPointFunc
					popad
                }
            temp = (int32_t)GetMemContent(0x160f510, 0x38);
            if (*point_total_1 < temp)
                break;
            (*award)++;
        }
    }
    bool THBGMTest()
    {
        if (!thPracParam.mode)
            return 0;
        else if (thPracParam.section >= 10000) {
            int stage = (thPracParam.section - 10000) / 100;
            int portionId = (thPracParam.section - 10000) % 100;
            if ((stage == 4 || stage == 5) && portionId > 4) {
                return 1;
            } else {
                return 0;
            }
        }
        else
            return th_sections_bgm[thPracParam.section];
    }
    void THSaveReplay(char* rep_name)
    {
        ReplaySaveParam(mb_to_utf16(rep_name).c_str(), thPracParam.GetJson());
    }
    void THDataInit()
    {
        AnlyDataInit();

        DataRef<DATA_SCENE_ID>(U32_ARG(0x17ce8b4));
        DataRef<DATA_RND_SEED>(U16_ARG(0x164d520));
        DataRef<DATA_DIFFCULTY>(U8_ARG(0x160f538));
        DataRef<DATA_SHOT_TYPE>(U8_ARG(0x164d0b1));
        //DataRef<DATA_SUB_SHOT_TYPE>(U8_ARG(0x474c6c));
        DataRef<DATA_STAGE>(U8_ARG(0x164d2cc));
    }

    HOOKSET_DEFINE(THMainHook)
    EHOOK_ST(th08_familiar, (void*)0x42a55f)
    {
        uint32_t target = *(uint32_t*)(pCtx->Ebp - 8);
        if (target == 0x57d2f0) {
            if (thPracParam.mode && thPracParam.familiar) {
                int32_t* familiar = (int32_t*)GetMemAddr(0x580670);
                *familiar = thPracParam.familiar;
            }
            THMainHook::singleton().th08_familiar.Disable();
        }
    }
    EHOOK_DY(th08_everlasting_bgm, (void*)0x45e1e0)
    {
        int32_t retn_addr = ((int32_t*)pCtx->Esp)[0];
        int32_t bgm_cmd = ((int32_t*)pCtx->Esp)[1];
        int32_t bgm_id = ((int32_t*)pCtx->Esp)[2];
        int32_t call_addr = ((int32_t*)pCtx->Esp)[7];
        int32_t th08_pause_test = ((int32_t*)pCtx->Esp)[6];

        bool el_switch;
        bool is_practice;
        bool result;

        el_switch = *(THOverlay::singleton().mElBgm) && !THGuiRep::singleton().mRepStatus && thPracParam.mode;
        switch (thPracParam.section) {
        case TH08_ST6A_LS:
        case TH08_ST6B_LS1:
        case TH08_ST6B_LS2:
        case TH08_ST6B_LS3:
        case TH08_ST6B_LS4:
        case TH08_ST6B_LS5:
            el_switch = false;
        }
        is_practice = *((int32_t*)0x17ce8b4) == 2;
        if (retn_addr == 0x4480ed && th08_pause_test == 0x434d08)
            result = ElBgmTest<0x447ef4, 0x4480ed, 0x43a170, 0x406c68, 0x43a048>(
                el_switch, is_practice, 0x447ef4, 2, 2, call_addr);
        else
            result = ElBgmTest<0x447ef4, 0x4480ed, 0x43a170, 0x406c68, 0x43a048>(
                el_switch, is_practice, retn_addr, bgm_cmd, bgm_id, call_addr);

        if (result) {
            pCtx->Eip = 0x45e2be;
        }
    }
    EHOOK_DY(th08_game_init, (void*)0x4674ed)
    {
        thPracParam.Reset();
        THMainHook::singleton().th08_familiar.Disable();
    }
    EHOOK_DY(th08_prac_menu_1, (void*)0x46ae14)
    {
        THGuiPrac::singleton().State(1);
    }
    EHOOK_DY(th08_prac_menu_3, (void*)0x46b0af)
    {
        THGuiPrac::singleton().State(3);
        *((int32_t*)0x164d2cc) = thPracParam.stage; // Stage
        if (thPracParam.stage == 8) {
            int32_t* diff = (int32_t*)0x160f538;
            *diff = 4;
        }
        pCtx->Eip = 0x46b0b4;
    }
    EHOOK_DY(th08_prac_menu_4, (void*)0x46b117)
    {
        THGuiPrac::singleton().State(4);
    }
    EHOOK_DY(th08_rep_menu_1, (void*)0x46e453)
    {
        THGuiRep::singleton().State(1);
    }
    EHOOK_DY(th08_rep_menu_2, (void*)0x46e8d8)
    {
        THGuiRep::singleton().State(2);
    }
    EHOOK_DY(th08_rep_menu_3, (void*)0x46ec2e)
    {
        THGuiRep::singleton().State(3);
    }
    EHOOK_DY(th08_disable_title, (void*)0x439568)
    {
        if (thPracParam.mode == 1 && thPracParam.section) {
            pCtx->Esp += 0xC;
            pCtx->Eip = 0x4395ca;
        }
    }
    EHOOK_DY(th08_disable_muteki, (void*)0x44d886)
    {
        if (thPracParam.mode == 1 && thPracParam.section) {
            pCtx->Eax &= 0xFFFFFF00;
            pCtx->Eip = 0x44d889;
        }
    }
    EHOOK_DY(th08_patch_main, (void*)0x43b935)
    {
        THMainHook::singleton().th08_familiar.Disable();
        if (thPracParam.mode == 1) {
            int32_t real_score = (int32_t)(thPracParam.score / 10);
            int32_t* score1 = (int32_t*)GetMemAddr(0x160f510, 0x8);
            int32_t* score2 = (int32_t*)GetMemAddr(0x160f510, 0x0);
            *score1 = *score2 = real_score;

            float* life = (float*)GetMemAddr(0x160f510, 0x74);
            *life = (thPracParam.life);

            float* bomb = (float*)GetMemAddr(0x160f510, 0x80);
            *bomb = (thPracParam.bomb);

            float* power = (float*)GetMemAddr(0x160f510, 0x98);
            *power = thPracParam.power;

            int32_t* graze1 = (int32_t*)GetMemAddr(0x160f510, 0x4);
            int32_t* graze2 = (int32_t*)GetMemAddr(0x160f510, 0xc);
            *graze1 = *graze2 = thPracParam.graze;

            *(uint32_t*)0xf54cf8 = thPracParam.frame;

            THSetPoint();

            int32_t* pTime1 = (int32_t*)0x164cfb4;
            int32_t* pTime2 = (int32_t*)GetMemAddr(0x160f510, 0x3c);
            int32_t* pTime3 = (int32_t*)GetMemAddr(0x160f510, 0x44);
            *pTime1 = *pTime2 = *pTime3 = thPracParam.time;

            int32_t* value = (int32_t*)GetMemAddr(0x160f510, 0x24);
            *value = thPracParam.value;

            int16_t* gauge = (int16_t*)GetMemAddr(0x160f510, 0x22);
            *gauge = thPracParam.gauge;

            int8_t* night = (int8_t*)GetMemAddr(0x160f510, 0x28);
            *night = (int8_t)thPracParam.night;

            if (thPracParam.familiar) {
                THMainHook::singleton().th08_familiar.Setup();
                THMainHook::singleton().th08_familiar.Enable();
            }

            if (thPracParam.rank) {
                *(int32_t*)(0x164d334) = (int32_t)thPracParam.rank;
                if (thPracParam.rankLock) {
                    *(int32_t*)(0x164d338) = (int32_t)thPracParam.rank;
                    *(int32_t*)(0x164d33c) = (int32_t)thPracParam.rank;
                }
            }

            // ECL Patch
            THSectionPatch();
        }
        thPracParam._playLock = true;
    }
    EHOOK_DY(th08_bgm, (void*)0x43a03c)
    {
        if (THBGMTest()) {
            if (thPracParam.mode == 1 && (thPracParam.section == TH08_ST6A_LS || (thPracParam.section >= TH08_ST6B_LS1 && thPracParam.section <= TH08_ST6B_LS5))) {
                PushHelper32(pCtx, 2);
            } else {
                PushHelper32(pCtx, 1);
            }
            pCtx->Eip = 0x43a03e;
        } 
    }
    EHOOK_DY(th08_save_replay, (void*)0x453acc)
    {
        char* rep_name = *(char**)(pCtx->Ebp - 0x694);
        //MB_INFO("Start!");
        //__asm
        //{
		//		mov eax, [ebp]
		//		mov eax, [eax - 0x694];
		//		mov rep_name, eax;
        //}
        if (thPracParam.mode)
            THSaveReplay(rep_name);
    }
    PATCH_DY(th08_disable_prac_menu1, (void*)0x46f47c, "\x90\x90\x90\x90\x90", 5);
    PATCH_DY(th08_disable_prac_menu2, (void*)0x46f59d, "\x90\x90\x90\x90\x90", 5);
    PATCH_DY(th08_prac_menu_key1, (void*)0x46b06b, "\x01\x00\x00\x00", 4);
    PATCH_DY(th08_prac_menu_key2, (void*)0x46b07b, "\x01\x00\x00\x00", 4);
    PATCH_DY(th08_prac_menu_key3, (void*)0x46b088, "\x01\x00\x00\x00", 4);
    EHOOK_DY(th08_update, (void*)0x43cb37)
    {
        GameGuiBegin(IMPL_WIN32_DX8, !THAdvOptWnd::singleton().IsOpen());

        // Gui components update
        THGuiPrac::singleton().Update();
        THGuiRep::singleton().Update();
        THOverlay::singleton().Update();
        bool drawCursor = THAdvOptWnd::StaticUpdate() || THGuiPrac::singleton().IsOpen();

        GameGuiEnd(drawCursor);
    }
    EHOOK_DY(th08_render, (void*)0x442014)
    {
        GameGuiRender(IMPL_WIN32_DX8);
    }
    HOOKSET_ENDDEF()

    HOOKSET_DEFINE(THInitHook)
    static __declspec(noinline) void THGuiCreate()
    {
        // Init
        GameGuiInit(IMPL_WIN32_DX8, 0x17ce760, 0x17ce700, 0x442390,
            Gui::INGAGME_INPUT_GEN1, 0x164d528, 0x164d530, 0x164d538,
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
        s.th08_gui_init_1.Disable();
        s.th08_gui_init_2.Disable();
    }
    PATCH_DY(th08_disable_dataver, (void*)0x40bb80, "\x33\xc0\xc3", 3);
    PATCH_DY(th08_disable_demo, (void*)0x467aca, "\xff\xff\xff\x7f", 4);
    EHOOK_DY(th08_disable_mutex, (void*)0x44344f)
    {
        pCtx->Eip = 0x44346b;
    }
    EHOOK_DY(th08_gui_init_1, (void*)0x467960)
    {
        THGuiCreate();
        THInitHookDisable();
    }
    EHOOK_DY(th08_gui_init_2, (void*)0x442a7a)
    {
        THGuiCreate();
        THInitHookDisable();
    }
    HOOKSET_ENDDEF()
}

void TH08Init()
{
    TH08::THInitHook::singleton().EnableAllHooks();
    TryKeepUpRefreshRate((void*)0x442591);

    //VFSHook(VFS_TH08, (void*)0x43e660);
    //VFSAddListener("stg4abg.anm", nullptr, TH08::THStage4ANM);
}

/*
	Memo:
	0xF54CF0: Timeline Struct
	0x4ECCB8: ECL Buffer Pointer
	0x4E4824: STD Buffer
	0x4E483C & 0x4E4844: STD Time
	*/

}
