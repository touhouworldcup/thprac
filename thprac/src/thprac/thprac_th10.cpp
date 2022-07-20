#include "thprac_utils.h"

namespace THPrac {
namespace TH10 {
    using std::pair;
    struct THPracParam {
        int32_t mode;
        int32_t stage;
        int32_t section;
        int32_t phase;
        int32_t life;
        int32_t power;
        int32_t faith;
        int32_t faith_bar;
        int32_t st6_boss9_spd;
        int64_t score;

        bool dlg;
        bool real_bullet_sprite;

        bool _playLock = false;
        void Reset()
        {
            memset(this, 0, sizeof(THPracParam));
        }
        bool ReadJson(std::string& json)
        {
            Reset();
            ParseJson();

            ForceJsonValue(game, "th10");
            GetJsonValue(mode);
            GetJsonValue(stage);
            GetJsonValue(section);
            GetJsonValue(phase);
            GetJsonValueEx(dlg, Bool);
            GetJsonValue(life);
            GetJsonValue(power);
            GetJsonValue(faith);
            GetJsonValue(faith_bar);
            GetJsonValue(score);
            GetJsonValue(real_bullet_sprite);
            GetJsonValue(st6_boss9_spd)
            else
                st6_boss9_spd = -1;

            return true;
        }
        std::string GetJson()
        {
            CreateJson();

            AddJsonValueEx(version, GetVersionStr(), jalloc);
            AddJsonValueEx(game, "th10", jalloc);
            AddJsonValue(mode);
            AddJsonValue(stage);
            if (section)
                AddJsonValue(section);
            if (phase)
                AddJsonValue(phase);
            if (dlg)
                AddJsonValue(dlg);

            AddJsonValue(life);
            AddJsonValue(power);
            AddJsonValue(faith);
            AddJsonValue(faith_bar);
            AddJsonValue(st6_boss9_spd);
            AddJsonValue(score);
            AddJsonValue(real_bullet_sprite);

            ReturnJson();
        }
    };
    THPracParam thPracParam {};

    class THGuiPrac : public Gui::GameGuiWnd {
        THGuiPrac() noexcept
        {
            *mLife = 9;
            *mPower = 100;
            *mMode = 1;
            *mFaith = 50000;
            *mFaithBar = 130;
            *mSt6Boss9Spd = 160;

            SetFade(0.8f, 0.1f);
            SetStyle(ImGuiStyleVar_WindowRounding, 0.0f);
            SetStyle(ImGuiStyleVar_WindowBorderSize, 0.0f);
        }
        SINGLETON(THGuiPrac);
    public:

        __declspec(noinline) void State(int state)
        {
            static int diff_prev = -1;
            switch (state) {
            case 0:
                break;
            case 1:
                mDiffculty = *((int32_t*)0x474c74);
                SetFade(0.8f, 0.1f);
                Open();
                thPracParam.Reset();
                switch (mDiffculty) {
                case 0:
                    mSt6Boss9Spd.SetBound(0, 140);
                    if (diff_prev != mDiffculty)
                        mSt6Boss9Spd.SetValue(140);
                    break;
                case 1:
                    mSt6Boss9Spd.SetBound(0, 120);
                    if (diff_prev != mDiffculty)
                        mSt6Boss9Spd.SetValue(120);
                    break;
                case 2:
                    mSt6Boss9Spd.SetBound(0, 100);
                    if (diff_prev != mDiffculty)
                        mSt6Boss9Spd.SetValue(100);
                    break;
                case 3:
                    mSt6Boss9Spd.SetBound(0, 50);
                    if (diff_prev != mDiffculty)
                        mSt6Boss9Spd.SetValue(50);
                    break;
                }
                diff_prev = mDiffculty;
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
                thPracParam.phase = *mPhase;
                if (SectionHasDlg(thPracParam.section))
                    thPracParam.dlg = *mDlg;
                thPracParam.life = *mLife;
                thPracParam.power = *mPower;
                thPracParam.faith = *mFaith;
                thPracParam.faith_bar = *mFaithBar;
                thPracParam.st6_boss9_spd = *mSt6Boss9Spd;
                thPracParam.score = *mScore;
                                           
                if (thPracParam.section == TH10_ST6_BOSS4 || thPracParam.section == TH10_ST6_BOSS8)
                    thPracParam.real_bullet_sprite = *mRealBulletSprite;
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
                AutoSize(0.0f, 0.0f, XSSS(0)[TH10_ST7_END_S9], XSTR(TH_MID_STAGE), 10.5f, 300.0f);
                AutoPos(0.5f, 0.57f);
                break;
            case Gui::LOCALE_EN_US:
                AutoSize(0.0f, 0.0f, XSSS(0)[TH10_ST7_END_S2], XSTR(TH_END_STAGE), 10.5f, 300.0f);
                AutoPos(0.5f, 0.57f);
                break;
            case Gui::LOCALE_JA_JP:
                AutoSize(0.0f, 0.0f, XSSS(0)[TH10_ST7_END_S9], XSTR(TH10_FAITH_BAR), 10.5f, 300.0f);
                AutoPos(0.5f, 0.57f);
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
                    switch (CalcSection()) {
                    case TH10_ST6_BOSS8:
                    case TH10_ST6_BOSS4:
                        mRealBulletSprite();
                        break;
                    case TH10_ST6_BOSS9:
                        mSt6Boss9Spd();
                        break;
                    case TH10_ST7_END_S10:
                        mPhase(TH_PHASE, TH_SPELL_PHASE1);
                        break;
                    }                        
                }

                mLife();
                auto power_str = std::to_string((float)(*mPower) * 5.0f / 100.0f).substr(0, 4);
                mPower(power_str.c_str());
                mFaith();
                mFaith.RoundDown(10);
                mFaithBar();
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
            case TH10_ST1_BOSS1:
            case TH10_ST2_BOSS1:
            case TH10_ST3_BOSS1:
            case TH10_ST4_BOSS1:
            case TH10_ST5_BOSS1:
            case TH10_ST6_BOSS1:
            case TH10_ST7_END_NS1:
            case TH10_ST7_MID1:
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
        Gui::GuiSlider<int, ImGuiDataType_S32> mLife { TH_LIFE, 0, 9 };
        Gui::GuiDrag<int, ImGuiDataType_S32> mGraze { TH_GRAZE, 0, 999999, 1, 100000 };
        Gui::GuiSlider<int, ImGuiDataType_S32> mPower { TH_POWER, 0, 100 };
        Gui::GuiDrag<int64_t, ImGuiDataType_S64> mScore { TH_SCORE, 0, 9999999990, 10, 100000000 };
        Gui::GuiDrag<int, ImGuiDataType_S32> mFaith { TH_FAITH, 0, 999990, 10, 100000 };
        Gui::GuiSlider<int, ImGuiDataType_S32> mFaithBar { TH10_FAITH_BAR, 0, 130, 1, 10 };
        Gui::GuiSlider<int, ImGuiDataType_S32> mSt6Boss9Spd { TH_DELAY, 0, 160, 1, 10 };
        Gui::GuiCheckBox mRealBulletSprite { TH_REAL_BULLET_SIZE };

        Gui::GuiNavFocus mNavFocus { TH_STAGE, TH_MODE, TH_WARP, TH_DELAY, TH_REAL_BULLET_SIZE,
            TH_MID_STAGE, TH_END_STAGE, TH_NONSPELL, TH_SPELL, TH_PHASE, TH_CHAPTER,
            TH_LIFE, TH_FAITH, TH10_FAITH_BAR, TH_SCORE, TH_POWER, TH_GRAZE };

        int mChapterSetup[7][2] {
            { 3, 2 },
            { 3, 2 },
            { 4, 3 },
            { 4, 4 },
            { 4, 2 },
            { 4, 0 },
            { 4, 2 },
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
            uint32_t index = GetMemContent(0x47784c, 0x59dc);
            char* repName = (char*)GetMemAddr(0x47784c, index * 4 + 0x59e4, 0x1d4);
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
            //mInfBombs.SetTextOffsetRel(x_offset_1, x_offset_2);
            mInfPower.SetTextOffsetRel(x_offset_1, x_offset_2);
            mTimeLock.SetTextOffsetRel(x_offset_1, x_offset_2);
            mAutoBomb.SetTextOffsetRel(x_offset_1, x_offset_2);
            mElBgm.SetTextOffsetRel(x_offset_1, x_offset_2);
        }
        virtual void OnContentUpdate() override
        {
            mMuteki();
            mInfLives();
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
            new HookCtx((void*)0x426D05, "\x01", 1),
            new HookCtx((void*)0x425a2b, "\xeb", 1),
            new HookCtx((void*)0x426D69, "\x83\xc4\x08\x90\x90", 5) } };
        Gui::GuiHotKey mInfLives { TH_INFLIVES, "F2", VK_F2, {
            new HookCtx((void*)0x426A15, "\x90", 1) } };
        Gui::GuiHotKey mInfPower { TH_INFPOWER, "F3", VK_F3, {
            new HookCtx((void*)0x4259DB, "\x00", 1),
            new HookCtx((void*)0x425C4A, "\x00", 1),
            new HookCtx((void*)0x425ABD, "\x00", 1) } };
        Gui::GuiHotKey mTimeLock { TH_TIMELOCK, "F4", VK_F4, {
            new HookCtx((void*)0x408D93, "\xeb", 1), 
            new HookCtx((void*)0x40E5B0, "\x90", 1) } };
        Gui::GuiHotKey mAutoBomb { TH_AUTOBOMB, "F5", VK_F5, {
            new HookCtx((void*)0x425C13, "\xc6", 1) } };

    public:
        Gui::GuiHotKey mElBgm { TH_EL_BGM, "F6", VK_F6 };
    };

    class THAdvOptWnd : public Gui::PPGuiWnd {
        EHOOK_ST(th10_all_clear_bonus_1, (void*)0x416c3d)
        {
            pCtx->Eip = 0x416c56;
        }
        EHOOK_ST(th10_all_clear_bonus_2, (void*)0x416d6c)
        {
            if (GetMemContent(0x474ca0) & 0x10) {
                pCtx->Eip = 0x416c46;
            }
        }
        EHOOK_ST(th10_all_clear_bonus_3, (void*)0x416e49)
        {
            if (GetMemContent(0x474ca0) & 0x10) {
                pCtx->Eip = 0x416c46;
            }
        }
    private:
        void FpsInit()
        {
            mOptCtx.vpatch_base = (int32_t)GetModuleHandleW(L"vpatch_th10.dll");
            if (mOptCtx.vpatch_base) {
                uint64_t hash[2];
                CalcFileHash(L"vpatch_th10.dll", hash);
                if (hash[0] != 9704945468076323108ll || hash[1] != 99312983382598050ll)
                    mOptCtx.fps_status = -1;
                else if (*(int32_t*)(mOptCtx.vpatch_base + 0x1b024) == 0) {
                    mOptCtx.fps_status = 2;
                    mOptCtx.fps = *(int32_t*)(mOptCtx.vpatch_base + 0x1b034);
                }
            } else
                mOptCtx.fps_status = 0;
        }
        void FpsSet()
        {
            if (mOptCtx.fps_status == 1) {
                mOptCtx.fps_dbl = 1.0 / (double)mOptCtx.fps;
            } else if (mOptCtx.fps_status == 2) {
                *(int32_t*)(mOptCtx.vpatch_base + 0x18ab4) = mOptCtx.fps;
                *(int32_t*)(mOptCtx.vpatch_base + 0x1b034) = mOptCtx.fps;
            }
        }
        void GameplayInit()
        {
            th10_all_clear_bonus_1.Setup();
            th10_all_clear_bonus_2.Setup();
            th10_all_clear_bonus_3.Setup();
        }
        void GameplaySet()
        {
            th10_all_clear_bonus_1.Toggle(mOptCtx.all_clear_bonus);
            th10_all_clear_bonus_2.Toggle(mOptCtx.all_clear_bonus);
            th10_all_clear_bonus_3.Toggle(mOptCtx.all_clear_bonus);
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
            values.emplace_back("Score", (int64_t) * (int32_t*)(0x474c44) * 10ll);
            values.emplace_back("Faith", *(int32_t*)(0x474c4c) * 10);
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
            OnLocaleChange();

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

    void* THStage4ANM(int16_t time_delta)
    {
#define ANMChangeWord(buffer, pos, value) *((int16_t*)((size_t)buffer + pos)) = value;

        void* buffer = (void*)GetMemContent(0x4776e8, 0x178, 0x108);

        int16_t d1 = 8300 - time_delta;
        int16_t d2 = 8600 - time_delta;
        int16_t d3 = 15000 - time_delta;

        ANMChangeWord(buffer, 0xf4, d1);
        ANMChangeWord(buffer, 0x110, d2);
        ANMChangeWord(buffer, 0x118, d2);

        ANMChangeWord(buffer, 0x164, 0);
        ANMChangeWord(buffer, 0x178, d1);
        ANMChangeWord(buffer, 0x194, d2);
        ANMChangeWord(buffer, 0x19c, d2);

        ANMChangeWord(buffer, 0x1e8, 0);
        ANMChangeWord(buffer, 0x1fc, d1);
        ANMChangeWord(buffer, 0x218, d2);
        ANMChangeWord(buffer, 0x220, d2);

        ANMChangeWord(buffer, 0x1002e4, 0);
        ANMChangeWord(buffer, 0x1002f8, d1);
        ANMChangeWord(buffer, 0x100314, d2);
        ANMChangeWord(buffer, 0x10031c, d3);
        ANMChangeWord(buffer, 0x10032c, d3);

        return nullptr;

#undef ANMChangeWord
    }
    void* THStage4STD(int32_t time_delta)
    {
        void* buffer = (void*)GetMemContent(0x4776e8, 0x10);

        struct th10_std {
            int32_t time;
            int16_t ins;
            int16_t length;
            int32_t param1;
            int32_t param2;
        };
        union {
            th10_std* p_std;
            int8_t* p_int8;
        } std;

        std.p_int8 = (int8_t*)buffer;
        std.p_int8 += 0x5e0;

        while (std.p_std->time != 0xffffffff) {
            if (std.p_std->ins == 1) {
                auto jmp_time = std.p_std->param2;
                std.p_std->param2 = (jmp_time - time_delta >= 0) ? jmp_time - time_delta : 0;
            }
            auto time = std.p_std->time;
            std.p_std->time = (time - time_delta >= 0) ? time - time_delta : 0;
            std.p_int8 += std.p_std->length;
        }

        return nullptr;
    }
    void* THStage6ANM()
    {
        void* buffer = (void*)GetMemContent(0x4776e8, 0x178, 0x108);

#define ANMChangeWord(buffer, pos, value) *((int16_t*)((size_t)buffer + pos)) = value;
        if (thPracParam.mode == 1 && thPracParam.section >= TH10_ST6_BOSS1 && thPracParam.section <= TH10_ST6_BOSS9) {
            ANMChangeWord(buffer, 0x0801d4, 0);
            ANMChangeWord(buffer, 0x0801e8, 0);
            ANMChangeWord(buffer, 0x080224, 0);
            ANMChangeWord(buffer, 0x080238, 0);

            ANMChangeWord(buffer, 0x0c0348, 0);
            ANMChangeWord(buffer, 0x0c0364, 0);
            ANMChangeWord(buffer, 0x0c03b4, 0);
            ANMChangeWord(buffer, 0x0c03d0, 0);
            ANMChangeWord(buffer, 0x0c0420, 0);
            ANMChangeWord(buffer, 0x0c043c, 0);
            ANMChangeWord(buffer, 0x0c048c, 0);
            ANMChangeWord(buffer, 0x0c04a8, 0);
            ANMChangeWord(buffer, 0x0c04f8, 0);
            ANMChangeWord(buffer, 0x0c0514, 0);
            ANMChangeWord(buffer, 0x0c0564, 0);
            ANMChangeWord(buffer, 0x0c0580, 0);
            ANMChangeWord(buffer, 0x0c05d0, 0);
            ANMChangeWord(buffer, 0x0c05ec, 0);

            ANMChangeWord(buffer, 0x01047f8, 0);
            ANMChangeWord(buffer, 0x0104804, 0);
            ANMChangeWord(buffer, 0x010484c, 0);
            ANMChangeWord(buffer, 0x0104858, 0);
        }
        return nullptr;
#undef ANMChangeWord
    }
    void* THStage6STD()
    {
        void* buffer = (void*)GetMemContent(0x4776e8, 0x10);

        if (thPracParam.mode == 1 && thPracParam.section >= TH10_ST6_BOSS1 && thPracParam.section <= TH10_ST6_BOSS9) {
            struct th10_std {
                int32_t time;
                int16_t ins;
                int16_t length;
                int32_t param1;
                int32_t param2;
            };
            union {
                th10_std* p_std;
                int8_t* p_int8;
            } std;

            std.p_int8 = (int8_t*)buffer;
            std.p_int8 += 0x994;

            while (std.p_std->time != 0xffffffff) {
                if (std.p_std->ins == 1) {
                    auto jmp_time = std.p_std->param2;
                    std.p_std->param2 = (jmp_time - 3487 >= 0) ? jmp_time - 3487 : 0;
                }
                auto time = std.p_std->time;
                std.p_std->time = (time - 3487 >= 0) ? time - 3487 : 0;
                std.p_int8 += std.p_std->length;
            }

            *((int32_t*)((size_t)buffer + 0xb68)) = 1;
            *((int32_t*)((size_t)buffer + 0xb84)) = 1;
            *((int32_t*)((size_t)buffer + 0xba0)) = 1;
        }
        return nullptr;
    }

    struct StdStatus {
        uint32_t fogChgTime;
        uint32_t fogCol;
        uint32_t fogStart;
        uint32_t fogEnd;
        uint32_t camPosChgTime;
        uint32_t camPosX;
        uint32_t camPosY;
        uint32_t camPosZ;
        uint32_t camDirChgTime;
        uint32_t camDirX;
        uint32_t camDirY;
        uint32_t camDirZ;
    };
    StdStatus thSt4StdStatus {};
    __declspec(noinline) void STDSt4SetStatus()
    {
        uint32_t stdStructPtr = GetMemContent(0x4776e8);
        if (GetMemContent(0x4776e8, 0x16c) && thSt4StdStatus.fogChgTime) {
            *(uint32_t*)(stdStructPtr + 0x15c) = thSt4StdStatus.fogChgTime;
            *(uint32_t*)(stdStructPtr + 0x158) = thSt4StdStatus.fogChgTime - 1;
            *(float*)(stdStructPtr + 0x160) = (float)thSt4StdStatus.fogChgTime;
        }
        if (GetMemContent(0x4776e8, 0xe0) && thSt4StdStatus.camPosChgTime) {
            *(uint32_t*)(stdStructPtr + 0xd0) = thSt4StdStatus.camPosChgTime;
            *(uint32_t*)(stdStructPtr + 0xcc) = thSt4StdStatus.camPosChgTime - 1;
            *(float*)(stdStructPtr + 0xd4) = (float)thSt4StdStatus.camPosChgTime;
        }
        if (GetMemContent(0x4776e8, 0x94) && thSt4StdStatus.camDirChgTime) {
            *(uint32_t*)(stdStructPtr + 0x84) = thSt4StdStatus.camDirChgTime;
            *(uint32_t*)(stdStructPtr + 0x80) = thSt4StdStatus.camDirChgTime - 1;
            *(float*)(stdStructPtr + 0x88) = (float)thSt4StdStatus.camDirChgTime;
        }
        memset(&thSt4StdStatus.fogChgTime, 0, sizeof(StdStatus));
    }
    EHOOK_G1(th10_st4_std, (void*)0x4042b4)
    {
        STDSt4SetStatus();
        th10_st4_std::GetHook().Disable();
    }
    void STDSt4Jump(uint32_t pos, uint32_t time, StdStatus& status)
    {
        VFile std;
        std.SetFile((void*)GetMemContent(0x4776e8, 0x10), 999999);
        std.SetPos(0x600);
        std << 0 << 0x00140002 << status.camPosX << status.camPosY << status.camPosZ
            << 0 << 0x00140004 << status.camDirX << status.camDirY << status.camDirZ
            << 0 << 0x00140008 << status.fogCol << status.fogStart << status.fogEnd
            << 0 << 0x00100001 << pos << time;
        thSt4StdStatus = status;

        th10_st4_std::GetHook().Enable();
    }
    void ECLJump(ECLHelper& ecl, unsigned int start, unsigned int dest, int at_frame, int ecl_time = 0)
    {
        ecl.SetPos(start);
        ecl << ecl_time << 0x0018000C << 0x02ff0000 << 0x00000000 << dest - start << at_frame;
    }
    void ECLTimeFix(ECLHelper& ecl, size_t pos, int32_t delta_time, unsigned int ins_count = 0xFFFFFFFF)
    {
        ecl.SetPos(pos);
        int32_t ecl_time;
        int16_t ecl_ins;
        int16_t ecl_length;
        size_t p = pos;
        for (unsigned int i = 0; i < ins_count; i++) {
            ecl >> ecl_time;
            ecl >> ecl_ins;
            ecl >> ecl_length;
            if (ecl_time == 0x484c4345)
                break;
            if ((ecl_time + delta_time) >= 0)
                ecl_time += delta_time;
            ecl.SetPos(p);
            ecl << ecl_time;
            p += ecl_length;
            ecl.SetPos(p);
        }
    }
    void ECLSetHealth(ECLHelper& ecl, size_t pos, int32_t health, int32_t ecl_time = 0, uint8_t rank_mask = 0xff, bool stall = false)
    {
        ecl.SetPos(pos);
        ecl << ecl_time << 0x0014014b << (uint16_t)0x0000 << rank_mask << (uint8_t)0x01 << health;
        if (stall)
            ecl << 9999 << 0x00100000 << 0x00ff0000 << 0;
    }
    template <class... Args>
    void ECLVoid(ECLHelper& ecl, size_t pos, Args... rest)
    {
        ecl.SetPos(pos + 4);
        ecl << (int16_t)0;
        ECLVoid(ecl, rest...);
    }
    void ECLVoid(ECLHelper& ecl, size_t pos)
    {
        ecl.SetPos(pos + 4);
        ecl << (int16_t)0;
    }
    void ECLSt6Boss(ECLHelper& ecl)
    {
        int32_t ecl_time;
        int16_t ecl_ins;
        int16_t ecl_length;
        size_t p = 0x68c;

        ecl << pair(0x754, (int16_t)0);
        ECLJump(ecl, 0x10100, 0x10120, 0);
        ECLJump(ecl, 0xfb0c, 0xfb64, 3669);
        ecl.SetPos(p);
        while (true) {
            ecl >> ecl_time;
            ecl >> ecl_ins;
            ecl >> ecl_length;
            if (ecl_time == 0x484c4345)
                break;

            if (ecl_time > 60)
                ecl_time = 60;
            ecl.SetPos(p);
            ecl << ecl_time;
            p += ecl_length;
            ecl.SetPos(p);
        }
    }
    void ECLSt7MidBoss(ECLHelper& ecl)
    {
        ecl << pair(0x18983, (int8_t)0x37)
            << pair(0x10bb4, 60) << pair(0x10bc4, 60) << pair(0x10bd8, 60)
            << pair(0x10bec, 60) << pair(0x10c04, 60) << pair(0x10c18, 60)
            << pair(0x10c28, 60) << pair(0x10c48, 60) << pair(0x10c5c, 60)
            << pair(0x10c70, 60) << pair(0x10c88, 60) << pair(0x10c9c, 60)
            << pair(0x10cb4, 60) << pair(0x10cc8, 60) << pair(0x10cdc, 60)
            << pair(0x10afc, (int16_t)0);
        ECLJump(ecl, 0x171a8, 0x171cc, 61);
    }
    void ECLSt7Boss(ECLHelper& ecl)
    {
        ecl << pair(0x18983, (int8_t)0x42) << pair(0x18984, (int8_t)0x6f)
            << pair(0x18985, (int8_t)0x73) << pair(0x18986, (int8_t)0x73);
        ECLJump(ecl, 0x18118, 0x1813c, 1);
        ecl << pair(0x1816c, 0) << pair(0x18170, (int16_t)0);
    }
    __declspec(noinline) void THStageWarp(ECLHelper& ecl, int stage, int portion)
    {
        /*
        if (GetMemContent(0x4776e8, 0x3c) == 7400) {
            char tempBuf[256] {};
            std::fstream recFile("rec.cpp", std::ios_base::in | std::ios_base::out | std::ios_base::app);
            uint32_t* stdDataPtr = (uint32_t*)GetMemAddr(0x4776e8, 0x50);
            recFile << "{\n";

            sprintf_s(tempBuf, "%d, 0x%x, 0x%x, 0x%x, // Fog Color\n",
                GetMemContent(0x4776e8, 0x15c), GetMemContent(0x4776e8, 0x100), GetMemContent(0x4776e8, 0xe8), GetMemContent(0x4776e8, 0xec));
            recFile << tempBuf;
            sprintf_s(tempBuf, "%d, 0x%x, 0x%x, 0x%x, // Cam Pos\n",
                GetMemContent(0x4776e8, 0xd0), GetMemContent(0x4776e8, 0x9c), GetMemContent(0x4776e8, 0xa0), GetMemContent(0x4776e8, 0xa4));
            recFile << tempBuf;
            sprintf_s(tempBuf, "%d, 0x%x, 0x%x, 0x%x, // Cam Dir\n",
                GetMemContent(0x4776e8, 0x84), GetMemContent(0x4776e8, 0x50), GetMemContent(0x4776e8, 0x54), GetMemContent(0x4776e8, 0x58));
            recFile << tempBuf;

            recFile << "}";
            recFile.close();
        }
        */
        StdStatus st4_c2 {
            208, 0xffffffff, 0x43f00000, 0x44610000, // Fog Color
            208, 0x0, 0x42c80000, 0xc3e10000, // Cam Pos
            208, 0x0, 0x43700000, 0x43a50000, // Cam Dir
        };
        StdStatus st4_c3 {
            240, 0xffffffff, 0x43f00000, 0x44610000, // Fog Color
            1159, 0x0, 0x42c80000, 0xc3e10000, // Cam Pos
            440, 0x0, 0x43700000, 0x43a50000, // Cam Dir
        };
        StdStatus st4_c4 {
            240, 0xffffffff, 0x43f00000, 0x44610000, // Fog Color
            2678, 0x0, 0x42c80000, 0xc3e10000, // Cam Pos
            440, 0x0, 0x43700000, 0x43a50000, // Cam Dir
        };
        StdStatus st4_c5 {
            0, 0xff704030, 0x43960000, 0x44160000, // Fog Color
            99, 0xc3480000, 0x434a0000, 0xc3a50000, // Cam Pos
            0, 0x42f00000, 0xc3960000, 0x43a50000, // Cam Dir
        };
        StdStatus st4_c6 {
            0, 0xff704030, 0x43960000, 0x44160000, // Fog Color
            487, 0xc3480000, 0x43e5f287, 0xc3a50000, // Cam Pos
            0, 0x42f00000, 0xc3960000, 0x43a50000, // Cam Dir
        };
        StdStatus st4_c7 {
            0, 0xff500000, 0x43960000, 0x44480000, // Fog Color
            70, 0x0, 0x43f10000, 0xc3e10000, // Cam Pos
            0, 0x0, 0xc3960000, 0x43a50000, // Cam Dir
        };
        StdStatus st4_c8 {
            247, 0xff500000, 0x43960000, 0x44480000, // Fog Color
            247, 0x0, 0x43f10000, 0xc3e10000, // Cam Pos
            247, 0x0, 0xc3960000, 0x43a50000, // Cam Dir
        };
        auto st4_boss_timeskip = [&](uint32_t stdTime) {
            ecl << pair(0x14aa4, 8600 - stdTime) << pair(0x14ab4, 8600 - stdTime) << pair(0x14ae4, 8600 - stdTime)
                << pair(0x14af8, 8601 - stdTime) << pair(0x14b08, 8601 - stdTime) << pair(0x14b18, 8601 - stdTime)
                << pair(0x14b2c, 8602 - stdTime);
            ecl << pair(0xf900, 3600 - stdTime)
                << pair(0xf930, 4100 - stdTime) << pair(0xf950, 4100 - stdTime);
        };

        if (stage == 1) {
            switch (portion) {
            case 1:
                break;
            case 2:
                ECLJump(ecl, 0x9cb8, 0x9f40, 390);
                break;
            case 3:
                ECLJump(ecl, 0x9cb8, 0xa5b8, 1190);
                break;
            case 4:
                ECLJump(ecl, 0x9cb8, 0xaf74, 2220);
                ECLJump(ecl, 0xafa8, 0xafd8, 2510);
                break;
            case 5:
                ECLJump(ecl, 0x9cb8, 0xaf74, 2220);
                ECLJump(ecl, 0xafa8, 0xb1f0, 3330);
                break;
            default:
                break;
            }
        } else if (stage == 2) {
            switch (portion) {
            case 1:
                break;
            case 2:
                ECLJump(ecl, 0xb764, 0xba7c, 470);
                break;
            case 3:
                ECLJump(ecl, 0xb764, 0xbfe4, 884);
                break;
            case 4:
                ECLJump(ecl, 0xb764, 0xc028, 2884);
                break;
            case 5:
                ECLJump(ecl, 0xb764, 0xcb54, 4024);
                break;
            default:
                break;
            }
        } else if (stage == 3) {
            switch (portion) {
            case 1:
                break;
            case 2:
                ECLJump(ecl, 0xca7c, 0xcb64, 990);
                break;
            case 3:
                ECLJump(ecl, 0xca7c, 0xcff4, 1970);
                break;
            case 4:
                ECLJump(ecl, 0xca7c, 0xd220, 2830);
                break;
            case 5:
                ECLJump(ecl, 0xca7c, 0xd384, 3190);
                break;
            case 6:
                ECLJump(ecl, 0xca7c, 0xd694, 3915);
                break;
            case 7:
                ECLJump(ecl, 0xca7c, 0xe1f4, 5195);
                break;
            default:
                break;
            }
        } else if (stage == 4) {
            switch (portion) {
            case 1:
                break;
            case 2:
                //c2
                STDSt4Jump(0x94, 409, st4_c2);
                THStage4ANM(409);
                ECLJump(ecl, 0xdf30, 0xe048, 409);
                st4_boss_timeskip(409);
                break;
            case 3:
                //c3
                STDSt4Jump(0x94, 1360, st4_c3);
                THStage4ANM(1360);
                ECLJump(ecl, 0xdf30, 0xe918, 1360);
                st4_boss_timeskip(1360);
                break;
            case 4:
                //c4
                STDSt4Jump(0x94, 2879, st4_c4);
                THStage4ANM(2879);
                ECLJump(ecl, 0xdf30, 0xed44, 310);
                ECLJump(ecl, 0xed6c, 0xf30c, 1459);
                st4_boss_timeskip(2879);
                break;
            case 5:
                //c5
                STDSt4Jump(0x16c, 4040, st4_c5);
                THStage4ANM(4040);
                ECLJump(ecl, 0x14a64, 0x14a84, 0);
                ECLJump(ecl, 0xf900, 0xf930, 4040);
                st4_boss_timeskip(4040);
                break;
            case 6:
                //c6
                STDSt4Jump(0x1cc, 5080, st4_c6);
                THStage4ANM(5080);
                ECLJump(ecl, 0x14a64, 0x14a84, 0);
                ECLJump(ecl, 0xf900, 0x105a4, 980);
                st4_boss_timeskip(5080);
                break;
            case 7:
                //c7
                STDSt4Jump(0x280, 6199, st4_c7);
                THStage4ANM(6199);
                ECLJump(ecl, 0x14a64, 0x14a84, 0);
                ECLJump(ecl, 0xf900, 0x12d24, 409);
                st4_boss_timeskip(6199);
                break;
            case 8:
                //c8
                STDSt4Jump(0x2e0, 7400, st4_c8);
                THStage4ANM(7400);
                ECLJump(ecl, 0x14a64, 0x14a84, 0);
                ECLJump(ecl, 0xf900, 0x12e00, 1610);
                st4_boss_timeskip(7400);
                break;
            default:
                break;
            }
        } else if (stage == 5) {
            switch (portion) {
            case 1:
                break;
            case 2:
                ECLJump(ecl, 0xe39c, 0x119a4, 150); // -878
                ecl << pair(0x143e4, 3800 - 878) << pair(0x143f4, 3800 - 878) << pair(0x14424, 3800 - 878)
                    << pair(0x14434, 3800 - 878) << pair(0x14454, 3800 - 878);
                break;
            case 3:
                ECLJump(ecl, 0xe39c, 0x11e7c, 30); // -1373
                ecl << pair(0x143e4, 3800 - 1373) << pair(0x143f4, 3800 - 1373) << pair(0x14424, 3800 - 1373)
                    << pair(0x14434, 3800 - 1373) << pair(0x14454, 3800 - 1373);
                break;
            case 4:
                ECLJump(ecl, 0xe39c, 0x12e54, 30); // -2873
                ecl << pair(0x143e4, 3800 - 2873) << pair(0x143f4, 3800 - 2873) << pair(0x14424, 3800 - 2873)
                    << pair(0x14434, 3800 - 2873) << pair(0x14454, 3800 - 2873);
                break;
            case 5:
                ecl << pair(0x14ba4, (int16_t)0);
                ECLJump(ecl, 0x143e4, 0x132b8, 30);
                break;
            case 6:
                ecl << pair(0x14ba4, (int16_t)0);
                ECLJump(ecl, 0x143e4, 0x136e8, 930);
                break;
            default:
                break;
            }
        } else if (stage == 6) {
            switch (portion) {
            case 1:
                break;
            case 2:
                ECLJump(ecl, 0xe8ec, 0xed70, 210); // -715
                ECLJump(ecl, 0xfb20, 0xfb40, 3668, 3368 - 715 + 300);
                break;
            case 3:
                ECLJump(ecl, 0xe8ec, 0xf63c, 110); // -1260
                ECLJump(ecl, 0xfb20, 0xfb40, 3668, 3368 - 1260 + 300);
                break;
            case 4:
                ECLJump(ecl, 0xe8ec, 0xf8c4, 70); // -1965
                ECLJump(ecl, 0xfb20, 0xfb40, 3668, 3368 - 1965 + 300);
                break;
            default:
                break;
            }
        } else if (stage == 7) {
            switch (portion) {
            case 1:
                break;
            case 2:
                ECLJump(ecl, 0x1366c, 0x15744, 40);
                break;
            case 3:
                ECLJump(ecl, 0x1366c, 0x1610c, 110);
                break;
            case 4:
                ECLJump(ecl, 0x1366c, 0x16fe4, 110);
                break;
            case 5:
                ECLJump(ecl, 0x1366c, 0x1721c, 110);
                break;
            case 6:
                ECLJump(ecl, 0x1366c, 0x18024, 790);
                break;
            default:
                break;
            }
        }
    }
    __declspec(noinline) void THPatch(ECLHelper& ecl, th_sections_t section)
    {
        switch (section) {
        case THPrac::TH10::TH10_ST1_MID1:
            ECLJump(ecl, 0x9cb8, 0xaf74, 2200);
            break;
        case THPrac::TH10::TH10_ST1_MID2_EN:
            ECLJump(ecl, 0x9cb8, 0xaf74, 2200);
            ecl << pair(0x6e24, 4200) << pair(0x6e38, 1650);
            ECLVoid(ecl, 0x7134);
            break;
        case THPrac::TH10::TH10_ST1_MID2_HL:
            ECLJump(ecl, 0x9cb8, 0xaf74, 2200);
            ecl << pair(0x6e24, 4200) << pair(0x6e38, 1650);
            ECLVoid(ecl, 0x7134);
            break;
        case THPrac::TH10::TH10_ST1_BOSS1:
            if (thPracParam.dlg)
                ECLJump(ecl, 0x9cb8, 0xba8c, 4370);
            else {
                ECLJump(ecl, 0x9cb8, 0xbab0, 4370);
                *(uint32_t*)0x474c84 = 24;
            }
            break;
        case THPrac::TH10::TH10_ST1_BOSS2:
            ECLJump(ecl, 0x9cb8, 0xbab0, 4370);
            ecl << pair(0x7a4, 0x898);
            *(uint32_t*)0x474c84 = 24;
            break;
        case THPrac::TH10::TH10_ST1_BOSS3:
            ECLJump(ecl, 0x9cb8, 0xbab0, 4370);
            ecl << pair(0x850, (int8_t)0x32);
            ECLJump(ecl, 0x15ec, 0x1640, 60);
            *(uint32_t*)0x474c84 = 24;
            break;
        case THPrac::TH10::TH10_ST1_BOSS4:
            ECLJump(ecl, 0x9cb8, 0xbab0, 4370);
            ecl << pair(0x850, (int8_t)0x32);
            ECLJump(ecl, 0x15ec, 0x1640, 60);
            ecl << pair(0x143c, 2200);
            *(uint32_t*)0x474c84 = 24;
            break;
        case THPrac::TH10::TH10_ST2_MID1:
            ECLJump(ecl, 0xb730, 0xbfe4, 2200);
            ECLJump(ecl, 0x8f50, 0x96f0, 1578);
            break;
        case THPrac::TH10::TH10_ST2_BOSS1:
            if (thPracParam.dlg)
                ECLJump(ecl, 0xb730, 0xd23c, 5864);
            else
                ECLJump(ecl, 0xb730, 0xd260, 5864);
            break;
        case THPrac::TH10::TH10_ST2_BOSS2:
            ECLJump(ecl, 0xb730, 0xd260, 5864);
            ecl << pair(0x87c, 2700);
            break;
        case THPrac::TH10::TH10_ST2_BOSS3:
            ECLJump(ecl, 0xb730, 0xd260, 5864);
            ecl << pair(0x990, (int8_t)0x32);
            ECLJump(ecl, 0x1d8c, 0x1df8, 120);
            break;
        case THPrac::TH10::TH10_ST2_BOSS4:
            ECLJump(ecl, 0xb730, 0xd260, 5864);
            ecl << pair(0x990, (int8_t)0x32);
            ECLJump(ecl, 0x1d8c, 0x1df8, 120);
            ecl << pair(0x1bc8, 5400);
            break;
        case THPrac::TH10::TH10_ST2_BOSS5:
            ECLJump(ecl, 0xb730, 0xd260, 5864);
            ecl << pair(0x990, (int8_t)0x32);
            ECLJump(ecl, 0x1d8c, 0x1df8, 120);
            ecl << pair(0x1bf4, (int8_t)0x33)
                << pair(0x1bc8, 2700) << pair(0x1be0, 2700);
            break;
        case THPrac::TH10::TH10_ST3_MID1:
            ECLJump(ecl, 0xca48, 0xd320, 3190);
            ecl << pair(0xd320, 3190) << pair(0xd350, 3190);
            break;
        case THPrac::TH10::TH10_ST3_BOSS1:
            ECLJump(ecl, 0xca48, 0xe354, 5885);
            if (!thPracParam.dlg)
                ECLJump(ecl, 0xe384, 0xe3a8, 5885);
            break;
        case THPrac::TH10::TH10_ST3_BOSS2:
            ECLJump(ecl, 0xca48, 0xe354, 5885);
            ECLJump(ecl, 0xe384, 0xe3a8, 5885);
            ecl << pair(0x884, 2300);
            break;
        case THPrac::TH10::TH10_ST3_BOSS3:
            ECLJump(ecl, 0xca48, 0xe354, 5885);
            ECLJump(ecl, 0xe384, 0xe3a8, 5885);
            ecl << pair(0x998, (int8_t)0x32);
            ECLJump(ecl, 0x1474, 0x14e0, 120);
            break;
        case THPrac::TH10::TH10_ST3_BOSS4:
            ECLJump(ecl, 0xca48, 0xe354, 5885);
            ECLJump(ecl, 0xe384, 0xe3a8, 5885);
            ecl << pair(0x998, (int8_t)0x32);
            ECLJump(ecl, 0x1474, 0x14e0, 120);
            ecl << pair(0x12cc, 2100);
            break;
        case THPrac::TH10::TH10_ST3_BOSS5:
            ECLJump(ecl, 0xca48, 0xe354, 5885);
            ECLJump(ecl, 0xe384, 0xe3a8, 5885);
            ecl << pair(0x998, (int8_t)0x33);
            ECLJump(ecl, 0x2e48, 0x2eb4, 120);
            break;
        case THPrac::TH10::TH10_ST3_BOSS6:
            ECLJump(ecl, 0xca48, 0xe354, 5885);
            ECLJump(ecl, 0xe384, 0xe3a8, 5885);
            ecl << pair(0x998, (int8_t)0x33);
            ECLJump(ecl, 0x2e48, 0x2eb4, 120);
            ecl << pair(0x2c58, 2300);
            break;
        case THPrac::TH10::TH10_ST4_MID1:
            THStage4ANM(3600);
            THStage4STD(3600);
            ECLJump(ecl, 0x14a64, 0x14a84, 3600);
            ECLJump(ecl, 0xf8ec, 0xf900, 3600);
            break;
        case THPrac::TH10::TH10_ST4_BOSS1:
            THStage4ANM(8688);
            THStage4STD(8688);
            ECLJump(ecl, 0x14a64, 0x14aa4, 8600);
            if (!thPracParam.dlg)
                ECLJump(ecl, 0x14ae4, 0x14b08, 8601);
            break;
        case THPrac::TH10::TH10_ST4_BOSS2:
            THStage4ANM(8688);
            THStage4STD(8688);
            ECLJump(ecl, 0x14a64, 0x14aa4, 8600);
            ECLJump(ecl, 0x14ae4, 0x14b08, 8601);
            ecl << pair(0x7ec, 2500);
            break;
        case THPrac::TH10::TH10_ST4_BOSS3:
            THStage4ANM(8688);
            THStage4STD(8688);
            ECLJump(ecl, 0x14a64, 0x14aa4, 8600);
            ECLJump(ecl, 0x14ae4, 0x14b08, 8601);
            ecl << pair(0x808, 0.0f) << pair(0x80c, 128.0f);
            ecl << pair(0x928, (int8_t)0x32);
            ECLJump(ecl, 0x19c0, 0x1a44, 120);
            break;
        case THPrac::TH10::TH10_ST4_BOSS4:
            THStage4ANM(8688);
            THStage4STD(8688);
            ECLJump(ecl, 0x14a64, 0x14aa4, 8600);
            ECLJump(ecl, 0x14ae4, 0x14b08, 8601);
            ecl << pair(0x928, (int8_t)0x32);
            ECLJump(ecl, 0x19c0, 0x1a44, 120);
            ecl << pair(0x1804, 2500);
            break;
        case THPrac::TH10::TH10_ST4_BOSS5:
            THStage4ANM(8688);
            THStage4STD(8688);
            ECLJump(ecl, 0x14a64, 0x14aa4, 8600);
            ECLJump(ecl, 0x14ae4, 0x14b08, 8601);
            ecl << pair(0x808, 0.0f) << pair(0x80c, 128.0f);
            ecl << pair(0x928, (int8_t)0x33);
            ECLJump(ecl, 0x2628, 0x2694, 120);
            break;
        case THPrac::TH10::TH10_ST4_BOSS6:
            THStage4ANM(8688);
            THStage4STD(8688);
            ECLJump(ecl, 0x14a64, 0x14aa4, 8600);
            ECLJump(ecl, 0x14ae4, 0x14b08, 8601);
            ecl << pair(0x928, (int8_t)0x33);
            ECLJump(ecl, 0x2628, 0x2694, 120);
            ecl << pair(0x2440, 0);

            ecl << pair(0x47e8, 99999999);
            break;
        case THPrac::TH10::TH10_ST4_BOSS7:
            THStage4ANM(8688);
            THStage4STD(8688);
            ECLJump(ecl, 0x14a64, 0x14aa4, 8600);
            ECLJump(ecl, 0x14ae4, 0x14b08, 8601);
            ecl << pair(0x928, (int8_t)0x33);
            ECLJump(ecl, 0x2628, 0x2694, 120);
            ecl << pair(0x242c, 2300) << pair(0x2440, 0)
                << pair(0x2498, (int8_t)0x34);
            ecl.SetPos(0x6bc4);
            ECLJump(ecl, 0x6b28, 0x6b74, 0);
            break;
        case THPrac::TH10::TH10_ST5_MID1:
            ECLJump(ecl, 0x14ba0, 0x14bc0, 3800);
            ECLJump(ecl, 0x143d0, 0x143e4, 3800);
            break;
        case THPrac::TH10::TH10_ST5_MID2:
            ECLJump(ecl, 0x14ba0, 0x14bc0, 3800);
            ECLJump(ecl, 0x143d0, 0x143e4, 3800);
            ecl << pair(0xb4e0, 2900);
            break;
        case THPrac::TH10::TH10_ST5_BOSS1:
            ECLJump(ecl, 0x14ba0, 0x14bc0, 0);
            ecl << pair(0x14bdb, (int8_t)0x42) << pair(0x14bdc, 0x0073736f);
            if (!thPracParam.dlg)
                ecl << pair(0x1432c, (int16_t)0) << pair(0x14370, (int16_t)0);
            break;
        case THPrac::TH10::TH10_ST5_BOSS2:
            ECLJump(ecl, 0x14ba0, 0x14bc0, 0);
            ecl << pair(0x14bdb, (int8_t)0x42) << pair(0x14bdc, 0x0073736f)
                << pair(0x1432c, (int16_t)0) << pair(0x14370, (int16_t)0);
            ecl << pair(0x9ec, 2300) << pair(0xa00, 2400);
            break;
        case THPrac::TH10::TH10_ST5_BOSS3:
            ECLJump(ecl, 0x14ba0, 0x14bc0, 0);
            ecl << pair(0x14bdb, (int8_t)0x42) << pair(0x14bdc, 0x0073736f)
                << pair(0x1432c, (int16_t)0) << pair(0x14370, (int16_t)0);
            ecl << pair(0xb14, (int8_t)0x32);
            ECLJump(ecl, 0x2280, 0x22ec, 120);
            break;
        case THPrac::TH10::TH10_ST5_BOSS4:
            ECLJump(ecl, 0x14ba0, 0x14bc0, 0);
            ecl << pair(0x14bdb, (int8_t)0x42) << pair(0x14bdc, 0x0073736f)
                << pair(0x1432c, (int16_t)0) << pair(0x14370, (int16_t)0);
            ecl << pair(0xb14, (int8_t)0x32);
            ECLJump(ecl, 0x2280, 0x22ec, 120);
            ecl << pair(0x20c8, 2400);
            break;
        case THPrac::TH10::TH10_ST5_BOSS5:
            ECLJump(ecl, 0x14ba0, 0x14bc0, 0);
            ecl << pair(0x14bdb, (int8_t)0x42) << pair(0x14bdc, 0x0073736f)
                << pair(0x1432c, (int16_t)0) << pair(0x14370, (int16_t)0);
            ecl << pair(0xb14, (int8_t)0x33);
            ECLJump(ecl, 0x31b0, 0x321c, 100);
            break;
        case THPrac::TH10::TH10_ST5_BOSS6:
            ECLJump(ecl, 0x14ba0, 0x14bc0, 0);
            ecl << pair(0x14bdb, (int8_t)0x42) << pair(0x14bdc, 0x0073736f)
                << pair(0x1432c, (int16_t)0) << pair(0x14370, (int16_t)0);
            ecl << pair(0xb14, (int8_t)0x33);
            ECLJump(ecl, 0x31b0, 0x321c, 100);
            ecl << pair(0x2ff8, 2400);
            break;
        case THPrac::TH10::TH10_ST5_BOSS7:
            ECLJump(ecl, 0x14ba0, 0x14bc0, 0);
            ecl << pair(0x14bdb, (int8_t)0x42) << pair(0x14bdc, 0x0073736f)
                << pair(0x1432c, (int16_t)0) << pair(0x14370, (int16_t)0);
            ecl << pair(0xb14, (int8_t)0x33);
            ECLJump(ecl, 0x31b0, 0x321c, 100);
            ecl << pair(0x2ff8, 2400);

            ecl << pair(0x3024, (int8_t)0x34);
            ECLJump(ecl, 0x7b74, 0x7c04, 90);
            ecl << pair(0x7c34, 60);
            break;
        case THPrac::TH10::TH10_ST6_BOSS1:
            if (thPracParam.dlg) {
                ECLJump(ecl, 0xfb0c, 0xfb40, 3668);
                ECLJump(ecl, 0x10100, 0x10120, 0);
            } else
                ECLSt6Boss(ecl);
            ECLJump(ecl, 0x9bc, 0x9e8, 80);
            ecl << pair(0x994, 0) << pair(0x9a8, 0) << pair(0x990, 40);
            break;
        case THPrac::TH10::TH10_ST6_BOSS2:
            ECLSt6Boss(ecl);
            ECLJump(ecl, 0x9bc, 0x9e8, 80);
            ecl << pair(0x994, 0) << pair(0x9a8, 0) << pair(0x990, 40);
            ecl << pair(0x71c, 2500);
            break;
        case THPrac::TH10::TH10_ST6_BOSS3:
            ECLSt6Boss(ecl);
            ecl << pair(0x8c8, (int8_t)0x32);
            ECLJump(ecl, 0xf9c, 0x1028, 60);
            ECLJump(ecl, 0x1058, 0x1098, 120);
            ecl << pair(0xf38, 60); // Invi. time
            ecl << pair(0x730, 0) << pair(0x734, 0x42e00000); // Pos
            break;
        case THPrac::TH10::TH10_ST6_BOSS4:
            ECLSt6Boss(ecl);
            ecl << pair(0x8c8, (int8_t)0x32);
            ECLJump(ecl, 0xf9c, 0x1028, 60);
            ECLJump(ecl, 0x1058, 0x1098, 120);
            ecl << pair(0xf38, 60);
            ecl << pair(0x730, 0) << pair(0x734, 0x42e00000);

            ecl << pair(0x0d58, 2500);
            ecl.SetPos(0x1058);
            ecl << 0 << 0x00180103 << 0x02ff0000 << 0 << 1 << 13
                << 0 << 0x00180103 << 0x02ff0000 << 0 << 2 << 14
                << 0 << 0x00180103 << 0x02ff0000 << 0 << 3 << 18
                << 0 << 0x00180103 << 0x02ff0000 << 0 << 4 << 19
                << 9999 << 0x00100000 << 0x00ff0000 << 0;
            break;
        case THPrac::TH10::TH10_ST6_BOSS5:
            ECLSt6Boss(ecl);
            ecl << pair(0x8c8, (int8_t)0x33);
            ECLJump(ecl, 0x2190, 0x21fc, 0);
            ECLJump(ecl, 0x225c, 0x227c, 90);
            ecl << pair(0x212c, 0); // Invi. time
            ecl << pair(0x730, 0) << pair(0x734, 0x43000000); // Pos
            break;
        case THPrac::TH10::TH10_ST6_BOSS6:
            ECLSt6Boss(ecl);
            ecl << pair(0x8c8, (int8_t)0x33);
            ECLJump(ecl, 0x2190, 0x21fc, 0);
            ECLJump(ecl, 0x225c, 0x227c, 90);
            ecl << pair(0x212c, 0); // Invi. time
            ecl << pair(0x730, 0) << pair(0x734, 0x43000000); // Pos

            ecl << pair(0x22ac, 9999);
            ecl << pair(0x1ed8, 2900) << pair(0x1f4c, 2900);
            break;
        case THPrac::TH10::TH10_ST6_BOSS7:
            ECLSt6Boss(ecl);
            ecl << pair(0x8c8, (int8_t)0x34);
            ECLJump(ecl, 0x2970, 0x29c8, 0);
            ECLJump(ecl, 0x2a28, 0x2a48, 90);
            ecl << pair(0x290c, 0); // Invi. time
            ecl << pair(0x730, 0) << pair(0x734, 0x43000000); // Pos
            break;
        case THPrac::TH10::TH10_ST6_BOSS8:
            ECLSt6Boss(ecl);
            ecl << pair(0x8c8, (int8_t)0x34);
            ECLJump(ecl, 0x2970, 0x29c8, 0);
            ECLJump(ecl, 0x2a28, 0x2a48, 90);
            ecl << pair(0x290c, 0); // Invi. time
            ecl << pair(0x730, 0) << pair(0x734, 0x43000000); // Pos

            ecl << pair(0x2a78, 9999);
            ecl << pair(0x272c, 3200);
            break;
        case THPrac::TH10::TH10_ST6_BOSS9:
            ECLSt6Boss(ecl);
            ecl << pair(0x8c8, (int8_t)0x34);
            ECLJump(ecl, 0x2970, 0x29c8, 0);
            ECLJump(ecl, 0x2a28, 0x2a48, 90);
            ecl << pair(0x290c, 0); // Invi. time
            ecl << pair(0x730, 0) << pair(0x734, 0x43200000); // Pos

            //ecl << pair(0x2a58, 9999);
            ecl << pair(0x272c, 3200);

            ecl << pair(0x2758, (int8_t)0x35);
            ECLJump(ecl, 0x8a4c, 0x8aa4, 90);
            ecl << pair(0x88bc, 0);
            ecl.SetPos(0x2a58);
            ecl << 0 << 0x00180103 << 0x02ff0000 << 0 << 3 << 18
                << 0 << 0x00180103 << 0x02ff0000 << 0 << 4 << 19
                << 9999 << 0x00100000 << 0x00ff0000 << 0;

            if (thPracParam.st6_boss9_spd >= 0) {
                ecl.SetPos(0x8d38 + *((int32_t*)0x474c74) * 4);
                ecl << thPracParam.st6_boss9_spd;
            }
            break;
        case THPrac::TH10::TH10_ST7_MID1:
            ecl << pair(0x18983, (int8_t)0x37)
                << pair(0x10bb4, 60) << pair(0x10bc4, 60) << pair(0x10bd8, 60)
                << pair(0x10bec, 60) << pair(0x10c04, 60) << pair(0x10c18, 60)
                << pair(0x10c28, 60) << pair(0x10c48, 60) << pair(0x10c5c, 60)
                << pair(0x10c70, 60) << pair(0x10c88, 60) << pair(0x10c9c, 60)
                << pair(0x10cb4, 60) << pair(0x10cc8, 60) << pair(0x10cdc, 60)
                << pair(0x10afc, (int16_t)0);
            if (!thPracParam.dlg)
                ECLJump(ecl, 0x171a8, 0x171cc, 61);
            break;
        case THPrac::TH10::TH10_ST7_MID2:
            ECLSt7MidBoss(ecl);
            ecl << pair(0x10ac4, 2300);
            break;
        case THPrac::TH10::TH10_ST7_MID3:
            ECLSt7MidBoss(ecl);
            ecl << pair(0x10ac4, 2300);
            ecl << pair(0x10d65, (int8_t)0x32);
            ECLJump(ecl, 0x118d8, 0x11930, 120);
            break;
        case THPrac::TH10::TH10_ST7_MID4:
            ECLSt7MidBoss(ecl);
            ecl << pair(0x10ac4, 2300);
            ecl << pair(0x10d65, (int8_t)0x33);
            ECLJump(ecl, 0x126cc, 0x12724, 90);
            break;
        case THPrac::TH10::TH10_ST7_END_NS1:
            if (thPracParam.dlg)
                ecl << pair(0x18983, (int8_t)0x42) << pair(0x18984, (int8_t)0x6f)
                    << pair(0x18985, (int8_t)0x73) << pair(0x18986, (int8_t)0x73);
            else {
                ECLSt7Boss(ecl);
                *(uint32_t*)0x474c84 = 24;
            }
            break;
        case THPrac::TH10::TH10_ST7_END_S1:
            ECLSt7Boss(ecl);
            ecl << pair(0xbe0, 2800);
            ECLJump(ecl, 0xd3c, 0xd68, 0);
            ecl.SetPos(0xde8);
            ecl << 0 << 0x002c014e << 0x04ff0000 << 0 << 0 << 0xaf0 << 0xe10
                << 0xc << 0x73736f42 << 0x64726143 << 0x31;
            *(uint32_t*)0x474c84 = 24;
            break;
        case THPrac::TH10::TH10_ST7_END_NS2:
            ECLSt7Boss(ecl);
            ecl << pair(0xcf4, (int8_t)0x32);
            ECLJump(ecl, 0x12b8, 0x1324, 110);
            *(uint32_t*)0x474c84 = 24;
            break;
        case THPrac::TH10::TH10_ST7_END_S2:
            ECLSt7Boss(ecl);
            ecl << pair(0xcf4, (int8_t)0x32);
            ECLJump(ecl, 0x12b8, 0x1324, 110);
            ecl << pair(0x10e8, 3600);
            *(uint32_t*)0x474c84 = 24;
            break;
        case THPrac::TH10::TH10_ST7_END_NS3:
            ECLSt7Boss(ecl);
            ecl << pair(0xcf4, (int8_t)0x33);
            ECLJump(ecl, 0x17b8, 0x1824, 120);
            *(uint32_t*)0x474c84 = 24;
            break;
        case THPrac::TH10::TH10_ST7_END_S3:
            ECLSt7Boss(ecl);
            ecl << pair(0xcf4, (int8_t)0x33);
            ECLJump(ecl, 0x17b8, 0x1824, 120);
            ecl << pair(0x15cc, 2800);
            *(uint32_t*)0x474c84 = 24;
            break;
        case THPrac::TH10::TH10_ST7_END_NS4:
            ECLSt7Boss(ecl);
            ecl << pair(0xcf4, (int8_t)0x34);
            ECLJump(ecl, 0x207c, 0x20e8, 120);
            *(uint32_t*)0x474c84 = 24;
            break;
        case THPrac::TH10::TH10_ST7_END_S4:
            ECLSt7Boss(ecl);
            ecl << pair(0xcf4, (int8_t)0x34);
            ECLJump(ecl, 0x207c, 0x20e8, 120);
            ecl << pair(0x1e70, 3600);
            *(uint32_t*)0x474c84 = 24;
            break;
        case THPrac::TH10::TH10_ST7_END_NS5:
            ECLSt7Boss(ecl);
            ecl << pair(0xcf4, (int8_t)0x35);
            ECLJump(ecl, 0x2628, 0x2694, 110);
            *(uint32_t*)0x474c84 = 24;
            break;
        case THPrac::TH10::TH10_ST7_END_S5:
            ECLSt7Boss(ecl);
            ecl << pair(0xcf4, (int8_t)0x35);
            ECLJump(ecl, 0x2628, 0x2694, 110);
            ecl << pair(0x2408, 3500);
            *(uint32_t*)0x474c84 = 24;
            break;
        case THPrac::TH10::TH10_ST7_END_NS6:
            ECLSt7Boss(ecl);
            ecl << pair(0xcf4, (int8_t)0x36);
            ECLJump(ecl, 0x2b48, 0x2bb4, 120);
            *(uint32_t*)0x474c84 = 24;
            break;
        case THPrac::TH10::TH10_ST7_END_S6:
            ECLSt7Boss(ecl);
            ecl << pair(0xcf4, (int8_t)0x36);
            ECLJump(ecl, 0x2b48, 0x2bb4, 120);
            ecl << pair(0x293c, 3500);
            *(uint32_t*)0x474c84 = 24;
            break;
        case THPrac::TH10::TH10_ST7_END_NS7:
            ECLSt7Boss(ecl);
            ecl << pair(0xcf4, (int8_t)0x37);
            ECLJump(ecl, 0x340c, 0x3478, 120);
            *(uint32_t*)0x474c84 = 24;
            break;
        case THPrac::TH10::TH10_ST7_END_S7:
            ECLSt7Boss(ecl);
            ecl << pair(0xcf4, (int8_t)0x37);
            ECLJump(ecl, 0x340c, 0x3478, 120);
            ecl << pair(0x3200, 4500);
            *(uint32_t*)0x474c84 = 24;
            break;
        case THPrac::TH10::TH10_ST7_END_NS8:
            ECLSt7Boss(ecl);
            ecl << pair(0xcf4, (int8_t)0x38);
            ECLJump(ecl, 0x3988, 0x39f4, 110);
            *(uint32_t*)0x474c84 = 24;
            break;
        case THPrac::TH10::TH10_ST7_END_S8:
            ECLSt7Boss(ecl);
            ecl << pair(0xcf4, (int8_t)0x38);
            ECLJump(ecl, 0x3988, 0x39f4, 110);
            ecl << pair(0x3798, 1);
            *(uint32_t*)0x474c84 = 24;
            break;
        case THPrac::TH10::TH10_ST7_END_S9:
            ECLSt7Boss(ecl);
            ecl << pair(0xcec, 0xc) << pair(0xcf4, 0x64726143)
                << pair(0xcf8, 0x39);
            ECLJump(ecl, 0xb3d8, 0xb444, 80);
            *(uint32_t*)0x474c84 = 24;
            break;
        case THPrac::TH10::TH10_ST7_END_S10:
            ECLSt7Boss(ecl);
            ecl << pair(0xcec, 0xc) << pair(0xcf4, 0x64726143)
                << pair(0xcf8, 0x3031);
            ECLJump(ecl, 0x41d8, 0x4244, 80);
            ecl << pair(0x41b8, 20);

            if (thPracParam.phase == 1) {
                ecl << pair(0x4338, 5);
                ECLJump(ecl, 0x4af0, 0x5198, 0);
            }
            *(uint32_t*)0x474c84 = 24;
            break;
        default:
            break;
        }
    }
    __declspec(noinline) void THSectionPatch()
    {
        ECLHelper ecl;
        ecl.SetBaseAddr((void*)GetMemAddr(0x477704, 0x54, 0xC));

        auto section = thPracParam.section;
        if (section >= 10000 && section < 20000) {
            int stage = (section - 10000) / 100;
            int portionId = (section - 10000) % 100;
            THStageWarp(ecl, stage, portionId);
        } else {
            THPatch(ecl, (th_sections_t)section);
        }

        THStage6ANM();
        THStage6STD();
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
    void THReimuAFix(uint8_t* repBuffer)
    {
        unsigned char p1[128] = {
            0x00, 0x00, 0x00, 0x00, 0xC0, 0xA8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0xC0, 0xA8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x0C, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0xD0, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
        };
        unsigned char p2[128] = {
            0x80, 0xF3, 0xFF, 0xFF, 0x60, 0x9F, 0x00, 0x00, 0x80, 0x0C, 0x00, 0x00,
            0x60, 0x9F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xF3, 0xFF, 0xFF,
            0x60, 0x9F, 0x00, 0x00, 0x80, 0x0C, 0x00, 0x00, 0x60, 0x9F, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x80, 0xF3, 0xFF, 0xFF, 0x20, 0x03, 0x00, 0x00,
            0x80, 0x0C, 0x00, 0x00, 0x20, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0xC0, 0xF9, 0xFF, 0xFF, 0xD0, 0x07, 0x00, 0x00, 0x40, 0x06, 0x00, 0x00,
            0xD0, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
        };
        unsigned char p3[128] = {
            0x80, 0xF3, 0xFF, 0xFF, 0x60, 0x9F, 0x00, 0x00, 0x80, 0x0C, 0x00, 0x00,
            0x60, 0x9F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0xA8, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xF3, 0xFF, 0xFF,
            0x60, 0x9F, 0x00, 0x00, 0x80, 0x0C, 0x00, 0x00, 0x60, 0x9F, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0xC0, 0xA8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x80, 0xF3, 0xFF, 0xFF, 0x20, 0x03, 0x00, 0x00,
            0x80, 0x0C, 0x00, 0x00, 0x20, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x80, 0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0xC0, 0xF9, 0xFF, 0xFF, 0xD0, 0x07, 0x00, 0x00, 0x40, 0x06, 0x00, 0x00,
            0xD0, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xB8, 0x0B, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
        };
        unsigned char p4[128] = {
            0x80, 0xF3, 0xFF, 0xFF, 0x60, 0x9F, 0x00, 0x00, 0x80, 0x0C, 0x00, 0x00,
            0x60, 0x9F, 0x00, 0x00, 0xC0, 0xF9, 0xFF, 0xFF, 0xC0, 0xA8, 0x00, 0x00,
            0x40, 0x06, 0x00, 0x00, 0xC0, 0xA8, 0x00, 0x00, 0x80, 0xF3, 0xFF, 0xFF,
            0x60, 0x9F, 0x00, 0x00, 0x80, 0x0C, 0x00, 0x00, 0x60, 0x9F, 0x00, 0x00,
            0xC0, 0xF9, 0xFF, 0xFF, 0xC0, 0xA8, 0x00, 0x00, 0x40, 0x06, 0x00, 0x00,
            0xC0, 0xA8, 0x00, 0x00, 0x80, 0xF3, 0xFF, 0xFF, 0x20, 0x03, 0x00, 0x00,
            0x80, 0x0C, 0x00, 0x00, 0x20, 0x03, 0x00, 0x00, 0xC0, 0xF9, 0xFF, 0xFF,
            0x80, 0x0C, 0x00, 0x00, 0x40, 0x06, 0x00, 0x00, 0x80, 0x0C, 0x00, 0x00,
            0xC0, 0xF9, 0xFF, 0xFF, 0xD0, 0x07, 0x00, 0x00, 0x40, 0x06, 0x00, 0x00,
            0xD0, 0x07, 0x00, 0x00, 0xE0, 0xFC, 0xFF, 0xFF, 0xB8, 0x0B, 0x00, 0x00,
            0x20, 0x03, 0x00, 0x00, 0xB8, 0x0B, 0x00, 0x00
        };

        repBuffer += 0x198;
        auto power = thPracParam.power;
        if (power >= 80) {
            memcpy(repBuffer, p4, 128);
        } else if (power >= 60) {
            memcpy(repBuffer, p3, 128);

        } else if (power >= 40) {
            memcpy(repBuffer, p2, 128);

        } else if (power >= 20) {
            memcpy(repBuffer, p1, 128);

        } else {
            memset(repBuffer, 0, 128);
        }
    }
    void THReimuBFix(uint8_t* repBuffer)
    {
        unsigned char p1[128] = {
            0x00, 0x00, 0x00, 0x00, 0xC0, 0x8F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0xC0, 0x8F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xF3, 0xFF, 0xFF,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x30, 0xF8, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
        };
        unsigned char p2[128] = {
            0x80, 0xF3, 0xFF, 0xFF, 0x40, 0x9C, 0x00, 0x00, 0x80, 0x0C, 0x00, 0x00,
            0x40, 0x9C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xF3, 0xFF, 0xFF,
            0x40, 0x9C, 0x00, 0x00, 0x80, 0x0C, 0x00, 0x00, 0x40, 0x9C, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x80, 0xF3, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00,
            0x80, 0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0xC0, 0xF9, 0xFF, 0xFF, 0x30, 0xF8, 0xFF, 0xFF, 0x40, 0x06, 0x00, 0x00,
            0x30, 0xF8, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
        };
        unsigned char p3[128] = {
            0x80, 0xF3, 0xFF, 0xFF, 0x40, 0x9C, 0x00, 0x00, 0x80, 0x0C, 0x00, 0x00,
            0x40, 0x9C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0x8F, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xF3, 0xFF, 0xFF,
            0x40, 0x9C, 0x00, 0x00, 0x80, 0x0C, 0x00, 0x00, 0x40, 0x9C, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0xC0, 0x8F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x80, 0xF3, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00,
            0x80, 0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x80, 0xF3, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0xC0, 0xF9, 0xFF, 0xFF, 0x30, 0xF8, 0xFF, 0xFF, 0x40, 0x06, 0x00, 0x00,
            0x30, 0xF8, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x48, 0xF4, 0xFF, 0xFF,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
        };
        unsigned char p4[128] = {
            0x80, 0xF3, 0xFF, 0xFF, 0x40, 0x9C, 0x00, 0x00, 0x80, 0x0C, 0x00, 0x00,
            0x40, 0x9C, 0x00, 0x00, 0xC0, 0xF9, 0xFF, 0xFF, 0xC0, 0x8F, 0x00, 0x00,
            0x40, 0x06, 0x00, 0x00, 0xC0, 0x8F, 0x00, 0x00, 0x80, 0xF3, 0xFF, 0xFF,
            0x40, 0x9C, 0x00, 0x00, 0x80, 0x0C, 0x00, 0x00, 0x40, 0x9C, 0x00, 0x00,
            0xC0, 0xF9, 0xFF, 0xFF, 0xC0, 0x8F, 0x00, 0x00, 0x40, 0x06, 0x00, 0x00,
            0xC0, 0x8F, 0x00, 0x00, 0x80, 0xF3, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00,
            0x80, 0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0xF9, 0xFF, 0xFF,
            0x80, 0xF3, 0xFF, 0xFF, 0x40, 0x06, 0x00, 0x00, 0x80, 0xF3, 0xFF, 0xFF,
            0xC0, 0xF9, 0xFF, 0xFF, 0x30, 0xF8, 0xFF, 0xFF, 0x40, 0x06, 0x00, 0x00,
            0x30, 0xF8, 0xFF, 0xFF, 0xE0, 0xFC, 0xFF, 0xFF, 0x48, 0xF4, 0xFF, 0xFF,
            0x20, 0x03, 0x00, 0x00, 0x48, 0xF4, 0xFF, 0xFF
        };

        repBuffer += 0x198;
        auto power = thPracParam.power;
        if (power >= 80) {
            memcpy(repBuffer, p4, 128);
        } else if (power >= 60) {
            memcpy(repBuffer, p3, 128);

        } else if (power >= 40) {
            memcpy(repBuffer, p2, 128);

        } else if (power >= 20) {
            memcpy(repBuffer, p1, 128);

        } else {
            memset(repBuffer, 0, 128);
        }
    }
    void THReimuCFix(uint8_t* repBuffer)
    {
        unsigned char p1[128] = {
            0x00, 0x00, 0x00, 0x00, 0xC0, 0x8F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0xC0, 0x8F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xF3, 0xFF, 0xFF,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
        };
        unsigned char p2[128] = {
            0x40, 0xED, 0xFF, 0xFF, 0x40, 0x9C, 0x00, 0x00, 0xC0, 0x12, 0x00, 0x00,
            0x40, 0x9C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0xED, 0xFF, 0xFF,
            0x40, 0x9C, 0x00, 0x00, 0xC0, 0x12, 0x00, 0x00, 0x40, 0x9C, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x40, 0xED, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00,
            0xC0, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
        };
        unsigned char p3[128] = {
            0x00, 0x00, 0x00, 0x00, 0xC0, 0x8F, 0x00, 0x00, 0x40, 0xED, 0xFF, 0xFF,
            0x40, 0x9C, 0x00, 0x00, 0xC0, 0x12, 0x00, 0x00, 0x40, 0x9C, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0xC0, 0x8F, 0x00, 0x00, 0x40, 0xED, 0xFF, 0xFF, 0x40, 0x9C, 0x00, 0x00,
            0xC0, 0x12, 0x00, 0x00, 0x40, 0x9C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xF3, 0xFF, 0xFF,
            0x40, 0xED, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0xC0, 0x12, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
        };
        unsigned char p4[128] = {
            0x80, 0xF3, 0xFF, 0xFF, 0x40, 0x9C, 0x00, 0x00, 0x80, 0x0C, 0x00, 0x00,
            0x40, 0x9C, 0x00, 0x00, 0x00, 0xE7, 0xFF, 0xFF, 0x40, 0x9C, 0x00, 0x00,
            0x00, 0x19, 0x00, 0x00, 0x40, 0x9C, 0x00, 0x00, 0x80, 0xF3, 0xFF, 0xFF,
            0x40, 0x9C, 0x00, 0x00, 0x80, 0x0C, 0x00, 0x00, 0x40, 0x9C, 0x00, 0x00,
            0x00, 0xE7, 0xFF, 0xFF, 0x40, 0x9C, 0x00, 0x00, 0x00, 0x19, 0x00, 0x00,
            0x40, 0x9C, 0x00, 0x00, 0x80, 0xF3, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00,
            0x80, 0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE7, 0xFF, 0xFF,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x19, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
        };

        repBuffer += 0x198;
        auto power = thPracParam.power;
        if (power >= 80) {
            memcpy(repBuffer, p4, 128);
        } else if (power >= 60) {
            memcpy(repBuffer, p3, 128);

        } else if (power >= 40) {
            memcpy(repBuffer, p2, 128);

        } else if (power >= 20) {
            memcpy(repBuffer, p1, 128);

        } else {
            memset(repBuffer, 0, 128);
        }
    }
    void THMarisaAFix(uint8_t* repBuffer)
    {
        unsigned char p1[128] = {
            0x00, 0x00, 0x00, 0x00, 0x40, 0x9C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x40, 0x9C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
        };
        unsigned char p2[128] = {
            0x00, 0x00, 0x00, 0x00, 0x40, 0x9C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x40, 0x9C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x40, 0x9C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x9C, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
        };
        unsigned char p3[128] = {
            0x00, 0x00, 0x00, 0x00, 0x40, 0x9C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x40, 0x9C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x9C, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x40, 0x9C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x9C, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x40, 0x9C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
        };
        unsigned char p4[128] = {
            0x00, 0x00, 0x00, 0x00, 0x40, 0x9C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x40, 0x9C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x9C, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x40, 0x9C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x40, 0x9C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x9C, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x40, 0x9C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x40, 0x9C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
        };

        repBuffer += 0x198;
        auto power = thPracParam.power;
        if (power >= 80) {
            memcpy(repBuffer, p4, 128);
        } else if (power >= 60) {
            memcpy(repBuffer, p3, 128);

        } else if (power >= 40) {
            memcpy(repBuffer, p2, 128);

        } else if (power >= 20) {
            memcpy(repBuffer, p1, 128);

        } else {
            memset(repBuffer, 0, 128);
        }
    }
    void THMarisaBFix(uint8_t* repBuffer)
    {
        unsigned char p1[128] = {
            0x00, 0x00, 0x00, 0x00, 0xC0, 0x8F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x40, 0x9C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xF3, 0xFF, 0xFF,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0xA0, 0xF6, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
        };
        unsigned char p2[128] = {
            0x80, 0xF3, 0xFF, 0xFF, 0x40, 0x9C, 0x00, 0x00, 0x80, 0x0C, 0x00, 0x00,
            0x40, 0x9C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x40, 0x9C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x9C, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x80, 0xF3, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00,
            0x80, 0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0xE0, 0xFC, 0xFF, 0xFF, 0xA0, 0xF6, 0xFF, 0xFF, 0x20, 0x03, 0x00, 0x00,
            0xA0, 0xF6, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
        };
        unsigned char p3[128] = {
            0x80, 0xF3, 0xFF, 0xFF, 0x40, 0x9C, 0x00, 0x00, 0x80, 0x0C, 0x00, 0x00,
            0x40, 0x9C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0x8F, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x40, 0x9C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x9C, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x40, 0x9C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x80, 0xF3, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00,
            0x80, 0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x80, 0xF3, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0xC0, 0xF9, 0xFF, 0xFF, 0xA0, 0xF6, 0xFF, 0xFF, 0x40, 0x06, 0x00, 0x00,
            0xA0, 0xF6, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0xA0, 0xF6, 0xFF, 0xFF,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
        };
        unsigned char p4[128] = {
            0x80, 0xF3, 0xFF, 0xFF, 0x40, 0x9C, 0x00, 0x00, 0x80, 0x0C, 0x00, 0x00,
            0x40, 0x9C, 0x00, 0x00, 0xE0, 0xFC, 0xFF, 0xFF, 0xC0, 0x8F, 0x00, 0x00,
            0x20, 0x03, 0x00, 0x00, 0xC0, 0x8F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x40, 0x9C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x9C, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x40, 0x9C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x40, 0x9C, 0x00, 0x00, 0x80, 0xF3, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00,
            0x80, 0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE0, 0xFC, 0xFF, 0xFF,
            0x80, 0xF3, 0xFF, 0xFF, 0x20, 0x03, 0x00, 0x00, 0x80, 0xF3, 0xFF, 0xFF,
            0xC0, 0xF9, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x40, 0x06, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0xE0, 0xFC, 0xFF, 0xFF, 0xA0, 0xF6, 0xFF, 0xFF,
            0x20, 0x03, 0x00, 0x00, 0xA0, 0xF6, 0xFF, 0xFF
        };

        repBuffer += 0x198;
        auto power = thPracParam.power;
        if (power >= 80) {
            memcpy(repBuffer, p4, 128);
        } else if (power >= 60) {
            memcpy(repBuffer, p3, 128);

        } else if (power >= 40) {
            memcpy(repBuffer, p2, 128);

        } else if (power >= 20) {
            memcpy(repBuffer, p1, 128);

        } else {
            memset(repBuffer, 0, 128);
        }
    }
    void THMarisaCFix(uint8_t* repBuffer)
    {
        unsigned char p1[128] = {
            0x00, 0x00, 0x00, 0x00, 0xC0, 0x8F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x40, 0x9C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xF3, 0xFF, 0xFF,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0xC0, 0x8F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
        };
        unsigned char p2[128] = {
            0xC0, 0xF9, 0xFF, 0xFF, 0xC0, 0x8F, 0x00, 0x00, 0x40, 0x06, 0x00, 0x00,
            0xC0, 0x8F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x40, 0x9C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x9C, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0xC0, 0xF9, 0xFF, 0xFF, 0x80, 0xF3, 0xFF, 0xFF,
            0x40, 0x06, 0x00, 0x00, 0x80, 0xF3, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0xC0, 0xF9, 0xFF, 0xFF, 0xC0, 0x8F, 0x00, 0x00, 0x40, 0x06, 0x00, 0x00,
            0xC0, 0x8F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
        };
        unsigned char p3[128] = {
            0x10, 0xF5, 0xFF, 0xFF, 0x88, 0x90, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x30, 0x8E, 0x00, 0x00, 0xF0, 0x0A, 0x00, 0x00, 0x88, 0x90, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x40, 0x9C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x9C, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x40, 0x9C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x10, 0xF5, 0xFF, 0xFF, 0x48, 0xF4, 0xFF, 0xFF,
            0x00, 0x00, 0x00, 0x00, 0xF0, 0xF1, 0xFF, 0xFF, 0xF0, 0x0A, 0x00, 0x00,
            0x48, 0xF4, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x10, 0xF5, 0xFF, 0xFF, 0x88, 0x90, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x30, 0x8E, 0x00, 0x00, 0xF0, 0x0A, 0x00, 0x00, 0x88, 0x90, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
        };
        unsigned char p4[128] = {
            0x98, 0xEF, 0xFF, 0xFF, 0x88, 0x90, 0x00, 0x00, 0x88, 0xFA, 0xFF, 0xFF,
            0x30, 0x8E, 0x00, 0x00, 0x78, 0x05, 0x00, 0x00, 0x30, 0x8E, 0x00, 0x00,
            0x68, 0x10, 0x00, 0x00, 0x88, 0x90, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x40, 0x9C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x9C, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x40, 0x9C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x40, 0x9C, 0x00, 0x00, 0x98, 0xEF, 0xFF, 0xFF, 0x48, 0xF4, 0xFF, 0xFF,
            0x88, 0xFA, 0xFF, 0xFF, 0xF0, 0xF1, 0xFF, 0xFF, 0x78, 0x05, 0x00, 0x00,
            0xF0, 0xF1, 0xFF, 0xFF, 0x68, 0x10, 0x00, 0x00, 0x48, 0xF4, 0xFF, 0xFF,
            0x98, 0xEF, 0xFF, 0xFF, 0x88, 0x90, 0x00, 0x00, 0x88, 0xFA, 0xFF, 0xFF,
            0x30, 0x8E, 0x00, 0x00, 0x78, 0x05, 0x00, 0x00, 0x30, 0x8E, 0x00, 0x00,
            0x68, 0x10, 0x00, 0x00, 0x88, 0x90, 0x00, 0x00
        };

        repBuffer += 0x198;
        auto power = thPracParam.power;
        if (power >= 80) {
            memcpy(repBuffer, p4, 128);
        } else if (power >= 60) {
            memcpy(repBuffer, p3, 128);

        } else if (power >= 40) {
            memcpy(repBuffer, p2, 128);

        } else if (power >= 20) {
            memcpy(repBuffer, p1, 128);

        } else {
            memset(repBuffer, 0, 128);
        }
    }
    void THRepPowerFix(uint8_t* repBuffer)
    {
        if (thPracParam.mode == 1) {
            switch (*(repBuffer + 0x50) * 3 + *(repBuffer + 0x54)) {
            case 0:
                THReimuAFix(repBuffer);
                break;
            case 1:
                THReimuBFix(repBuffer);
                break;
            case 2:
                THReimuCFix(repBuffer);
                break;
            case 3:
                THMarisaAFix(repBuffer);
                break;
            case 4:
                THMarisaBFix(repBuffer);
                break;
            case 5:
                THMarisaCFix(repBuffer);
                break;
            default:
                break;
            }
        }
    }
    void THDataInit()
    {
        AnlyDataInit();

        DataRef<DATA_SCENE_ID>(U32_ARG(0x491fb8));
        DataRef<DATA_RND_SEED>(U16_ARG(0x4918b0));
        DataRef<DATA_DIFFCULTY>(U8_ARG(0x474c74));
        DataRef<DATA_SHOT_TYPE>(U8_ARG(0x474c68));
        DataRef<DATA_SUB_SHOT_TYPE>(U8_ARG(0x474c6c));
        DataRef<DATA_STAGE>(U8_ARG(0x474c7c));
        DataRef<DATA_STARTING_STAGE>(U8_ARG(0x474c80));
    }
    PATCH_ST(th10_real_bullet_sprite, (void*)0x406e03, "\x0F\x84\x13\x05\x00\x00", 6);
    HOOKSET_DEFINE(THMainHook)
    EHOOK_DY(th10_everlasting_bgm, (void*)0x43e460)
    {
        int32_t retn_addr = ((int32_t*)pCtx->Esp)[0];
        int32_t bgm_cmd = ((int32_t*)pCtx->Esp)[1];
        int32_t bgm_id = ((int32_t*)pCtx->Esp)[2];
        int32_t call_addr = ((int32_t*)pCtx->Esp)[3];

        bool el_switch;
        bool is_practice;
        bool result;

        el_switch = *(THOverlay::singleton().mElBgm) && !THGuiRep::singleton().mRepStatus && thPracParam.mode && thPracParam.section;
        is_practice = (*((int32_t*)0x474ca0) & 0x1);
        result = ElBgmTest<0x420b42, 0x4180a7, 0x422be1, 0x422c51, 0x4183e6>(
            el_switch, is_practice, retn_addr, bgm_cmd, bgm_id, call_addr);

        if (result) {
            pCtx->Eip = 0x43e4c5;
        }
    }
    EHOOK_DY(th10_param_reset, (void*)0x42d436)
    {
        thPracParam.Reset();
    }
    EHOOK_DY(th10_prac_menu_1, (void*)0x431060)
    {
        THGuiPrac::singleton().State(1);
    }
    EHOOK_DY(th10_prac_menu_2, (void*)0x431085)
    {
        THGuiPrac::singleton().State(2);
    }
    EHOOK_DY(th10_prac_menu_3, (void*)0x431320)
    {
        THGuiPrac::singleton().State(3);
    }
    EHOOK_DY(th10_prac_menu_4, (void*)0x4313b4)
    {
        THGuiPrac::singleton().State(4);
    }
    PATCH_DY(th10_prac_menu_enter_1, (void*)0x43115d, "\xeb", 1);
    EHOOK_DY(th10_prac_menu_enter_2, (void*)0x431377)
    {
        pCtx->Eax = thPracParam.stage;
    }
    EHOOK_DY(th10_bgm, (void*)0x4183e0)
    {
        if (THBGMTest()) {
            PushHelper32(pCtx, 1);
            pCtx->Eip = 0x4183e1;
        } 
    }
    EHOOK_DY(th10_logo, (void*)0x413e4b)
    {
        if (thPracParam.mode == 1 && thPracParam.section) {
            if (thPracParam.section <= 10000 || thPracParam.section >= 20000 || thPracParam.section % 100 != 1) {
                pCtx->Eip = 0x413f87;
            }
        }
    }
    EHOOK_DY(th10_rep_menu_1, (void*)0x4317b1)
    {
        THGuiRep::singleton().State(1);
    }
    EHOOK_DY(th10_rep_menu_2, (void*)0x431869)
    {
        THGuiRep::singleton().State(2);
    }
    EHOOK_DY(th10_rep_menu_3, (void*)0x4319b4)
    {
        THGuiRep::singleton().State(3);
    }
    EHOOK_DY(th10_patch_main, (void*)0x417c5e)
    {
        if (thPracParam.mode == 1) {
            *(int32_t*)(0x474c70) = thPracParam.life;
            *(int32_t*)(0x474c48) = thPracParam.power;
            *(int32_t*)(0x474c4c) = thPracParam.faith / 10;
            *(int32_t*)(0x474c44) = (int32_t)(thPracParam.score / 10);
            th10_real_bullet_sprite.Toggle(thPracParam.real_bullet_sprite);

            if (thPracParam.faith_bar) {
                *(int32_t*)(0x474c54) = thPracParam.faith_bar - 1;
                *(int32_t*)(0x474c58) = thPracParam.faith_bar;
                *(float*)(0x474c5c) = (float)(thPracParam.faith_bar);
            }

            if (*((int32_t*)0x474c74) == 4) {
                if (thPracParam.score >= 100000000)
                    *(int8_t*)(0x474c9c) = 2;
                else if (thPracParam.score >= 30000000)
                    *(int8_t*)(0x474c9c) = 1;
            } else {
                if (thPracParam.score >= 150000000)
                    *(int8_t*)(0x474c9c) = 4;
                else if (thPracParam.score >= 80000000)
                    *(int8_t*)(0x474c9c) = 3;
                else if (thPracParam.score >= 40000000)
                    *(int8_t*)(0x474c9c) = 2;
                else if (thPracParam.score >= 20000000)
                    *(int8_t*)(0x474c9c) = 1;
            }

            THSectionPatch();
        }
        thPracParam._playLock = true;
    }
    EHOOK_DY(th10_rep_save, (void*)0x42a1e8)
    {
        char* repName = (char*)(pCtx->Esp + 0x20);
        if (thPracParam.mode)
            THSaveReplay(repName);
    }
    EHOOK_DY(th10_rep_power_fix, (void*)0x42a322)
    {
        uint8_t* repBuffer = (uint8_t*)pCtx->Eax;
        THRepPowerFix(repBuffer);
    }
    EHOOK_DY(th10_disable_prac_menu_1, (void*)0x43154d)
    {
        pCtx->Eip = 0x431576;
    }
    EHOOK_DY(th10_disable_prac_menu_2, (void*)0x43104e)
    {
        pCtx->Eip = 0x431054;
    }
    PATCH_DY(th10_disable_prac_menu_3, (void*)0x43111b, "\x00", 1);
    EHOOK_DY(th10_update, (void*)0x449d0e)
    {
        GameGuiBegin(IMPL_WIN32_DX9, !THAdvOptWnd::singleton().IsOpen());

        // Gui components update
        THGuiPrac::singleton().Update();
        THGuiRep::singleton().Update();
        THOverlay::singleton().Update();
        bool drawCursor = THAdvOptWnd::StaticUpdate() || THGuiPrac::singleton().IsOpen();

        GameGuiEnd(drawCursor);
    }
    EHOOK_DY(th10_render, (void*)0x4394fa)
    {
        GameGuiRender(IMPL_WIN32_DX9);
    }
    HOOKSET_ENDDEF()

    HOOKSET_DEFINE(THInitHook)
    static __declspec(noinline) void THGuiCreate()
    {
        // Init
        GameGuiInit(IMPL_WIN32_DX9, 0x491c30, 0x4924f0, 0x4390e0,
            Gui::INGAGME_INPUT_GEN2, 0x474e36, 0x474e34, 0,
            -1);

        // Gui components creation
        THGuiPrac::singleton();
        THGuiRep::singleton();
        THOverlay::singleton();

        // Hooks
        THMainHook::singleton().EnableAllHooks();
        th10_real_bullet_sprite.Setup();
        THDataInit();

        // Reset thPracParam
        thPracParam.Reset();
    }
    static __declspec(noinline) void THInitHookDisable()
    {
        auto& s = THInitHook::singleton();
        s.th10_gui_init_1.Disable();
        s.th10_gui_init_2.Disable();
    }
    PATCH_DY(th10_disable_demo, (void*)0x42ce44, "\xff\xff\xff\x7f", 4);
    EHOOK_DY(th10_disable_mutex, (void*)0x43a024)
    {
        pCtx->Eip = 0x43a051;
    }
    PATCH_DY(th10_startup_1, (void*)0x42ca31, "\xeb", 1);
    PATCH_DY(th10_startup_2, (void*)0x42d04f, "\xeb\x23", 2);
    EHOOK_DY(th10_gui_init_1, (void*)0x42d516)
    {
        THGuiCreate();
        THInitHookDisable();
    }
    EHOOK_DY(th10_gui_init_2, (void*)0x439d16)
    {
        THGuiCreate();
        THInitHookDisable();
    }
    HOOKSET_ENDDEF()
}

void TH10Init()
{
    TH10::THInitHook::singleton().EnableAllHooks();
}
}
