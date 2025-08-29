#include "thprac_games.h"
#include "thprac_utils.h"


namespace THPrac {
namespace TH07 {
    using std::pair;
    struct THPracParam {
        int32_t mode;
        int32_t stage;
        int32_t warp;
        int32_t section;
        int32_t phase;
        int32_t frame;
        float life;
        float bomb;
        int64_t score;
        float power;
        int32_t graze;
        int32_t point;
        int32_t point_total;
        int32_t point_stage;
        int32_t cherry;
        int32_t cherryMax;
        int32_t cherryPlus;
        int32_t spellBonus;
        int32_t rank;
        bool rankLock;

        bool dlg;

        bool _playLock = false;
        void Reset()
        {
            memset(this, 0, sizeof(THPracParam));
        }
        bool ReadJson(std::string& json)
        {
            Reset();
            ParseJson();

            ForceJsonValue(game, "th07");
            GetJsonValue(mode);
            GetJsonValue(stage);
            GetJsonValue(section);
            GetJsonValue(phase);
            GetJsonValueEx(dlg, Bool);
            GetJsonValue(frame);
            GetJsonValue(score);
            GetJsonValue(life);
            GetJsonValue(bomb);
            GetJsonValue(power);
            GetJsonValue(graze);
            GetJsonValue(point);
            GetJsonValue(point_total);
            GetJsonValue(point_stage);
            GetJsonValue(cherry);
            GetJsonValue(cherryMax);
            GetJsonValue(cherryPlus);
            GetJsonValue(spellBonus);
            GetJsonValue(rank);
            GetJsonValueEx(rankLock, Bool);

            return true;
        }
        std::string GetJson()
        {
            CreateJson();

            AddJsonValueEx(version, GetVersionStr(), jalloc);
            AddJsonValueEx(game, "th07", jalloc);
            AddJsonValue(mode);
            AddJsonValue(stage);
            if (section)
                AddJsonValue(section);
            if (phase)
                AddJsonValue(phase);
            if (frame)
                AddJsonValue(frame);
            if (dlg)
                AddJsonValue(dlg);

            AddJsonValue(score);
            AddJsonValueEx(life, (int)life);
            AddJsonValueEx(bomb, (int)bomb);
            AddJsonValueEx(power, (int)power);
            AddJsonValue(graze);
            AddJsonValue(point_total);
            AddJsonValue(point_stage);
            AddJsonValue(cherry);
            AddJsonValue(cherryMax);
            AddJsonValue(cherryPlus);
            AddJsonValue(spellBonus);
            AddJsonValue(rank);
            AddJsonValue(rankLock);

            ReturnJson();
        }
    };
    static int32_t thFakeShot = -1;
    THPracParam thPracParam {};
    void THReset()
    {
        thPracParam.Reset();
        thFakeShot = -1;
    }

    class THGuiPrac : public Gui::GameGuiWnd {
        THGuiPrac() noexcept
        {
            *mLife = 8;
            *mBomb = 8;
            *mPower = 128;
            *mMode = 1;
            *mScore = 0;
            *mGraze = 0;
            *mCherryMax = 200000;
            *mRank = 16;

            SetFade(0.8f, 0.1f);
            SetStyle(ImGuiStyleVar_WindowRounding, 0.0f);
            SetStyle(ImGuiStyleVar_WindowBorderSize, 0.0f);
            OnLocaleChange();
        }
        SINGLETON(THGuiPrac);
    public:

        __declspec(noinline) void State(int state)
        {
            int oldRank = mDiffculty;
            switch (state) {
            case 0:
                break;
            case 1:
                SetFade(0.8f, 0.1f);
                Open();
                THReset();
                mDiffculty = (int)(*((int8_t*)0x575a89));
                if (mDiffculty != oldRank) {
                    if (mDiffculty == 0) {
                        mRank.SetBound(12, *mRankLock ? 99 : 20);
                    } else {
                        mRank.SetBound(10, *mRankLock ? 99 : 32);
                    }
                    if (oldRank == 0 && mDiffculty > 0) {
                        *mRank = 32;
                    } else if (oldRank > 0 && mDiffculty == 0) {
                        *mRank = 20;
                    }
                }
            case 2:
                break;
            case 3:
                SetFade(0.8f, 0.8f);
                Close();

                // Fill Param
                thPracParam.mode = *mMode;
                thPracParam.stage = *mStage;
                thPracParam.section = CalcSection();
                thPracParam.phase = SpellPhase() ? *mPhase : 0;
                if (SectionHasDlg(thPracParam.section))
                    thPracParam.dlg = *mDlg;
                thPracParam.frame = *mFrame;
                thPracParam.score = *mScore;
                thPracParam.life = (float)*mLife;
                thPracParam.bomb = (float)*mBomb;
                thPracParam.power = (float)*mPower;
                thPracParam.graze = *mGraze;
                thPracParam.point = 0;
                thPracParam.point_total = *mPointTotal;
                thPracParam.point_stage = *mPointStage;
                thPracParam.cherry = *mCherry;
                thPracParam.cherryMax = *mCherryMax;
                thPracParam.cherryPlus = *mCherryPlus;
                thPracParam.spellBonus = *mSpellBonus;
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
            SetTitle(S(TH_MENU));
            switch (Gui::LocaleGet()) {
            case Gui::LOCALE_ZH_CN:
                SetSize(330.f, 390.f);
                SetPos(260.f, 70.f);
                SetItemWidth(-60.0f);
                break;
            case Gui::LOCALE_EN_US:
                SetSize(400.f, 390.f);
                SetPos(230.f, 70.f);
                SetItemWidth(-80.0f);
                break;
            case Gui::LOCALE_JA_JP:
                SetSize(400.f, 390.f);
                SetPos(230.f, 70.f);
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
            if (section == TH07_ST8_END_S10) {
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
                    *mSection = *mChapter = *mPhase = *mFrame = 0;
                if (*mWarp) {
                    SectionWidget();
                    mPhase(TH_PHASE, SpellPhase());
                }

                mLife();
                mBomb();
                mScore();
                mScore.RoundDown(10);
                mPower();
                mGraze();
                mPointTotal();
                mPointStage();
                mCherry();
                mCherry.RoundDown(10);
                mCherryMax();
                mCherryMax.RoundDown(10);
                mCherryPlus();
                mSpellBonus();
                mRank();
                mRankLock();
                if (mDiffculty == 0)
                    mRank.SetBound(12, *mRankLock ? 99 : 20);
                else
                    mRank.SetBound(10, *mRankLock ? 99 : 32);
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
            case TH07_ST1_BOSS1:
            case TH07_ST2_BOSS1:
            case TH07_ST3_BOSS1:
            case TH07_ST4_BOSS1_LUNASA:
            case TH07_ST4_BOSS1_MERLIN:
            case TH07_ST4_BOSS1_LYRICA:
            case TH07_ST5_BOSS1:
            case TH07_ST5_MID1:
            case TH07_ST6_BOSS1:
            case TH07_ST6_MID1:
            case TH07_ST7_END_NS1:
            case TH07_ST7_MID1:
            case TH07_ST8_END_NS1:
            case TH07_ST8_MID1:
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

                if (chapterCounts[1] == 0) {
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
                if (mSection(TH_WARP_SELECT_FRAME[*mWarp],
                        th_sections_cba[*mStage][*mWarp - 2],
                        th_sections_str[::THPrac::Gui::LocaleGet()][mDiffculty]))
                    *mPhase = 0;
                if (SectionHasDlg(th_sections_cba[*mStage][*mWarp - 2][*mSection]))
                    mDlg();
                break;
            case 4:
            case 5: // Non-spell & Spellcard
                if (mSection(TH_WARP_SELECT_FRAME[*mWarp],
                        th_sections_cbt[*mStage][*mWarp - 4],
                        th_sections_str[::THPrac::Gui::LocaleGet()][mDiffculty]))
                    *mPhase = 0;
                if (SectionHasDlg(th_sections_cbt[*mStage][*mWarp - 4][*mSection]))
                    mDlg();
                break;
            case 6: // Frame
                mFrame();
                break;
            }
        }

        // Data
        Gui::GuiCombo mMode { TH_MODE, TH_MODE_SELECT };
        Gui::GuiCombo mStage { TH_STAGE, TH07_STAGE_SELECT };
        Gui::GuiCombo mWarp { TH_WARP, TH_WARP_SELECT_FRAME };
        Gui::GuiCombo mSection { TH_MODE };
        Gui::GuiCombo mPhase { TH_PHASE };
        Gui::GuiCheckBox mDlg { TH_DLG };

        Gui::GuiSlider<int, ImGuiDataType_S32> mChapter { TH_CHAPTER, 0, 0 };
        Gui::GuiDrag<int, ImGuiDataType_S32> mFrame { TH_FRAME, 0, INT_MAX };
        Gui::GuiSlider<int, ImGuiDataType_S32> mLife { TH_LIFE, 0, 8 };
        Gui::GuiSlider<int, ImGuiDataType_S32> mBomb { TH_BOMB, 0, 8 };
        Gui::GuiDrag<int64_t, ImGuiDataType_S64> mScore { TH_SCORE, 0, 9999999990, 10, 100000000 };
        Gui::GuiSlider<int, ImGuiDataType_S32> mPower { TH_POWER, 0, 128 };
        Gui::GuiDrag<int, ImGuiDataType_S32> mGraze { TH_GRAZE, 0, 99999, 1, 10000 };
        Gui::GuiDrag<int, ImGuiDataType_S32> mPoint { TH_POINT, 0, 9999, 1, 1000 };
        Gui::GuiDrag<int, ImGuiDataType_S32> mPointTotal { TH_POINT_TOTAL, 0, 9999, 1, 1000 };
        Gui::GuiDrag<int, ImGuiDataType_S32> mPointStage { TH_POINT_STAGE, 0, 9999, 1, 1000 };
        Gui::GuiDrag<int, ImGuiDataType_S32> mCherry { TH07_CHERRY, 0, 9999990, 10, 1000000 };
        Gui::GuiDrag<int, ImGuiDataType_S32> mCherryMax { TH07_CHERRYMAX, 0, 9999990, 10, 1000000 };
        Gui::GuiSlider<int, ImGuiDataType_S32> mCherryPlus { TH07_CHERRYPLUS, 0, 50000, 1, 10000 };
        Gui::GuiSlider<int, ImGuiDataType_S32> mSpellBonus { TH07_SPELLBONUS, 0, 30 };
        Gui::GuiSlider<int, ImGuiDataType_S32> mRank { TH_BULLET_RANK, 10, 32, 1, 10, 10 };
        Gui::GuiCheckBox mRankLock { TH_BULLET_RANKLOCK };

        Gui::GuiNavFocus mNavFocus { TH_STAGE, TH_MODE, TH_WARP, TH_FRAME,
            TH_MID_STAGE, TH_END_STAGE, TH_NONSPELL, TH_SPELL, TH_PHASE, TH_CHAPTER,
            TH_LIFE, TH_BOMB, TH_SCORE, TH_POWER, TH_GRAZE, TH_POINT, TH_POINT_TOTAL, TH_POINT_STAGE,
            TH07_CHERRY, TH07_CHERRYMAX, TH07_CHERRYPLUS, TH07_SPELLBONUS, TH_BULLET_RANK, TH_BULLET_RANKLOCK };

        int mChapterSetup[8][2] {
            { 2, 1 },
            { 1, 1 },
            { 2, 1 },
            { 4, 4 },
            { 3, 1 },
            { 2, 0 },
            { 5, 3 },
            { 5, 3 }
        };

        float mStep = 10.0;
        int mDiffculty = 0;
        bool isMarisaA = false;
    };
    class THGuiRep : public Gui::GameGuiWnd {
        THGuiRep() noexcept
        {
        }
        SINGLETON(THGuiRep);
    public:

        void CheckReplay()
        {
            uint32_t* moduleList = (uint32_t*)0x626218;
            while (true) {
                if (moduleList[1] == 0x4554d6)
                    break;
                if (moduleList[5])
                    moduleList = (uint32_t*)moduleList[5];
                else
                    return;
            }
            uint32_t index = GetMemContent((int)(&moduleList[7]), 0xb0b8);
            char* raw = (char*)GetMemAddr((int)(&moduleList[7]), index * 512 + 0x6c);

            std::wstring repName = mb_to_utf16(raw, 932);
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
                THReset();
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
                    *((int32_t*)0x575ab4) = 2;
                }
            }
        }

        Gui::GuiHotKey mMenu { "ModMenuToggle", "BACKSPACE", VK_BACK };

        HOTKEY_DEFINE(mMuteki, TH_MUTEKI, "F1", VK_F1)
        PATCH_HK(0x43Ee14, "03")
        HOTKEY_ENDDEF();
        
        HOTKEY_DEFINE(mInfLives, TH_INFLIVES, "F2", VK_F2)
        PATCH_HK(0x44116B, "00")
        HOTKEY_ENDDEF();
        
        HOTKEY_DEFINE(mInfBombs, TH_INFBOMBS, "F3", VK_F3)
        PATCH_HK(0x440BC7, "00")
        HOTKEY_ENDDEF();
        
        HOTKEY_DEFINE(mInfPower, TH_INFPOWER, "F4", VK_F4)
        PATCH_HK(0x440DD3, "00"),
        PATCH_HK(0x440DBF,"90909090909090")
        HOTKEY_ENDDEF();
        
        HOTKEY_DEFINE(mTimeLock, TH_TIMELOCK, "F5", VK_F5)
        PATCH_HK(0x417726, "eb"),
        PATCH_HK(0x421F91, "eb")
        HOTKEY_ENDDEF();
        
        HOTKEY_DEFINE(mAutoBomb, TH_AUTOBOMB, "F6", VK_F6)
        PATCH_HK(0x440D2C, "ff"),
        PATCH_HK(0x440D35, "66C7054C9E4B0002"),
        PATCH_HK(0x440B8E, "54")
        HOTKEY_ENDDEF();
    public:
        Gui::GuiHotKey mElBgm { TH_EL_BGM, "F7", VK_F7 };
    };

    EHOOK_ST(th07_all_clear_bonus_1, 0x42b3d2, 2, {
        pCtx->Eip = 0x42b3d6;
    });
    EHOOK_ST(th07_all_clear_bonus_2, 0x4280b7, 2, {
        pCtx->Eip = 0x4280bf;
    });

    class THAdvOptWnd : public Gui::PPGuiWnd {
    private:
        void FpsInit()
        {
            mOptCtx.vpatch_base = (int32_t)GetModuleHandleW(L"vpatch_th07.dll");
            if (mOptCtx.vpatch_base) {
                uint64_t hash[2];
                CalcFileHash(L"vpatch_th07.dll", hash);
                if (hash[0] != 9678734212472211387ll || hash[1] != 9671871756369193188ll)
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
            th07_all_clear_bonus_1.Setup();
            th07_all_clear_bonus_2.Setup();
        }
        void GameplaySet()
        {
            th07_all_clear_bonus_1.Toggle(mOptCtx.all_clear_bonus);
            th07_all_clear_bonus_2.Toggle(mOptCtx.all_clear_bonus);
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
            *((int32_t*)0x575ab4) = 2;
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
                EndOptGroup();
            }

            AboutOpt();
            ImGui::EndChild();
            ImGui::SetWindowFocus();
        }

        adv_opt_ctx mOptCtx;
    };

    EHOOK_ST(th07_rb, 0x4157f3, 7, {
        self->Disable();
        *(uint32_t*)(pCtx->Ecx + 0x6f0) = 0x1e0;
        pCtx->Eip = 0x41677b;
    });
    void ECLTimeWarp(int count, uint32_t time)
    {
        uint32_t* addr = (uint32_t*)GetMemAddr(0x9a9af8, 0x9545fc);
        for (auto i = 0; i < count; i++) {
            *addr = time;
            addr = (uint32_t*)((uint32_t)addr + 0x10);
        }
    }
    void ECLST3BG()
    {
        *((uint16_t*)GetMemAddr(0x4b9e44, 0x2df2c, 0x140)) = 2;
        *((uint16_t*)GetMemAddr(0x4b9e44, 0x2df2c, 0x1a4)) = 2;
        *((uint16_t*)GetMemAddr(0x4b9e44, 0x2df2c, 0x208)) = 2;
        *((uint16_t*)GetMemAddr(0x4b9e44, 0x2df2c, 0x26c)) = 2;
    }
    void ECLSetHealth(ECLHelper& ecl, int offset, int32_t ecl_time, int32_t health)
    {
        ecl.SetPos(offset);
        ecl << ecl_time << 0x0010006e << 0x0000ff00 << health;
        ecl << 0x100000 << 0x000c0000 << 0x0000ff00;
    }
    void ECLSetTime(ECLHelper& ecl, int offset, int32_t ecl_time, int32_t time)
    {
        ecl.SetPos(offset);
        ecl << ecl_time << 0x00100072 << 0x0000ff00 << time;
        ecl << 0x100000 << 0x000c0000 << 0x0000ff00;
    }
    void ECLCallSub(ECLHelper& ecl, int offset, int32_t ecl_time, int32_t sub_id)
    {
        ecl.SetPos(offset);
        ecl << ecl_time << 0x00100029 << 0x0000ff00 << sub_id;
        ecl << 0x100000 << 0x000c0000 << 0x0000ff00;
    }
    void ECLForceSpell(ECLHelper& ecl, int offset, int32_t ecl_time, int32_t param110, int32_t param112, int32_t param113, int32_t param114, int32_t param115)
    {
        ecl.SetPos(offset);

        // 112
        ecl << ecl_time << 0x00100070 << 0x0000ff00 << param112;
        // 113e
        ecl << ecl_time << 0x00100071 << 0x00000100 << ((param113 >> 24) & 0x000000FF);
        // 113n
        ecl << ecl_time << 0x00100071 << 0x00000200 << ((param113 >> 16) & 0x000000FF);
        // 113h
        ecl << ecl_time << 0x00100071 << 0x00000400 << ((param113 >> 8) & 0x000000FF);
        // 113l
        ecl << ecl_time << 0x00100071 << 0x00000800 << ((param113 >> 0) & 0x000000FF);

        // 114
        ecl << ecl_time << 0x00100072 << 0x0000ff00 << param114;
        // 115e
        ecl << ecl_time << 0x00100073 << 0x00000100 << ((param115 >> 24) & 0x000000FF);
        // 115n
        ecl << ecl_time << 0x00100073 << 0x00000200 << ((param115 >> 16) & 0x000000FF);
        // 115h
        ecl << ecl_time << 0x00100073 << 0x00000400 << ((param115 >> 8) & 0x000000FF);
        // 115l
        ecl << ecl_time << 0x00100073 << 0x00000800 << ((param115 >> 0) & 0x000000FF);

        // 110
        ecl << ecl_time << 0x0010006e << 0x0000ff00 << param110;
    }
    void ECLNameFix()
    {
        if (thPracParam.stage == 0) {
            asm_call<0x427d92, Fastcall>(0x60f);
        } else if (thPracParam.stage == 3) {
            asm_call<0x427d92, Fastcall>(0x615);
        } else if (thPracParam.stage == 5) {
            *((int32_t*)0x1348024) = 2;
            *((int32_t*)0x9a9ab8) = 9;
            asm_call<0x427d92, Fastcall>(0x619);
        } else if (thPracParam.stage == 6) {
            *((int32_t*)0x1348024) = 1;
            *((int32_t*)0x1348020) = 2;
            *((int32_t*)0x9a9ab8) = 9;
            asm_call<0x427d92, Fastcall>(0x61b);
        } else if (thPracParam.stage == 7) {
            *((int32_t*)0x1348024) = 2;
            *((int32_t*)0x9a9ab8) = 9;
            asm_call<0x427d92, Fastcall>(0x61d);
        }
    }
    __declspec(noinline) void THStageWarp(ECLHelper& ecl, int stage, int portion)
    {
        if (stage == 1) {
            switch (portion) {
            case 1:
                ECLTimeWarp(2, 540);
                break;
            case 2:
                ECLTimeWarp(2, 1341);
                break;
            case 3:
                ECLTimeWarp(2, 3107);
                break;
            }
        } else if (stage == 2) {
            switch (portion) {
            case 1:
                ECLTimeWarp(3, 390);
                break;
            case 2:
                ECLTimeWarp(3, 3366); // 1520
                break;
            default:
                break;
            }
        } else if (stage == 3) {
            ECLST3BG();
            switch (portion) {
            case 1:
                ECLTimeWarp(2, 390);
                break;
            case 2:
                ECLTimeWarp(2, 854);
                break;
            case 3:
                ECLTimeWarp(2, 1805);
                ecl << pair{0xbb3c, (int16_t)1559}
                    << pair{0xbb5c, (int16_t)1560};
                break;
            default:
                break;
            }
        } else if (stage == 4) {
            switch (portion) {
            case 1:
                ECLTimeWarp(2, 80);
                break;
            case 2:
                ECLTimeWarp(2, 1948);
                break;
            case 3:
                ECLTimeWarp(2, 3028);
                break;
            case 4:
                ECLTimeWarp(2, 4288);
                break;
            case 5:
                ECLTimeWarp(2, 7964);
                break;
            case 6:
                ECLTimeWarp(2, 10136);
                break;
            case 7:
                ECLTimeWarp(2, 11396);
                break;
            case 8:
                ECLTimeWarp(2, 13166);
                break;
            default:
                break;
            }
        } else if (stage == 5) {
            switch (portion) {
            case 1:
                ECLTimeWarp(2, 440);
                break;
            case 2:
                ECLTimeWarp(2, 840);
                break;
            case 3:
                ECLTimeWarp(2, 2550);
                break;
            case 4:
                ECLTimeWarp(2, 4883);
                break;
            default:
                break;
            }
        } else if (stage == 6) {
            switch (portion) {
            case 1:
                ECLTimeWarp(2, 660);
                break;
            case 2:
                ECLTimeWarp(2, 1180);
                break;
            default:
                break;
            }
        } else if (stage == 7) {
            switch (portion) {
            case 1:
                ECLTimeWarp(2, 485);
                break;
            case 2:
                ECLTimeWarp(2, 1035);
                break;
            case 3:
                ECLTimeWarp(2, 1955);
                break;
            case 4:
                ECLTimeWarp(2, 2615);
                break;
            case 5:
                ECLTimeWarp(2, 3305);
                break;
            case 6:
                ECLTimeWarp(2, 4028);
                ecl << pair{0xee3e, (int16_t)0x68};
                break;
            case 7:
                ECLTimeWarp(2, 5808);
                break;
            case 8:
                ECLTimeWarp(2, 7088);
            default:
                break;
            }
        } else if (stage == 8) {
            switch (portion) {
            case 1:
                ECLTimeWarp(2, 485);
                break;
            case 2:
                ECLTimeWarp(2, 965);
                break;
            case 3:
                ECLTimeWarp(2, 1595);
                break;
            case 4:
                ECLTimeWarp(2, 2485);
                break;
            case 5:
                ECLTimeWarp(2, 3191);
                break;
            case 6:
                ECLTimeWarp(2, 3928);
                ecl << pair{0x1265a, (int16_t)0x68};
                break;
            case 7:
                ECLTimeWarp(2, 5708);
                break;
            case 8:
                ECLTimeWarp(2, 6988);
            default:
                break;
            }
        }
    }
    __declspec(noinline) void THPatch(ECLHelper& ecl, th_sections_t section)
    {
        auto lunasa = [&]() {
            thFakeShot = 0;
            ecl << pair{0x4f08, 1} << pair{0x4f30, 1} << pair{0x4f74, 1} << pair{0x4f9c, 1};
        };
        auto merlin = [&]() {
            thFakeShot = 4;
            ecl << pair{0x4f08, 2} << pair{0x4f30, 2} << pair{0x4f74, 2} << pair{0x4f9c, 2};
        };
        auto lyrica = [&]() {
            thFakeShot = 2;
            ecl << pair{0x4f08, 3} << pair{0x4f30, 3} << pair{0x4f74, 3} << pair{0x4f9c, 3};
        };
        auto timesave = [&]() {
            ecl << pair{0xc550, 0} << pair{0xc588, 0} << pair{0xc598, 0} << pair{0xf048, 0}
                << pair{0xf080, 0} << pair{0xf090, 0} << pair{0x11670, 0} << pair{0x11684, 0}
                << pair{0x11694, 0};
        };
        auto yuyuko = [&]() {
            ecl << pair{0x425c, (int16_t)0} << pair{0x426c, (int16_t)0}
                << pair{0xe678, (int16_t)0x9d7} << pair{0x3330, 0x22}
                << pair{0x43a8, (int16_t)0} << pair{0x43f0, (int16_t)0}
                << pair{0x43fc, 0} << pair{0x4410, 0} << pair{0x4420, 0} << pair{0x4434, 0} << pair{0x4444, 0};
            ECLTimeWarp(2, 0x9d7);
        };

        switch (section) {
        case THPrac::TH07::TH07_ST1_MID1:
            ECLTimeWarp(2, 0xa60);
            break;
        case THPrac::TH07::TH07_ST1_MID2:
            ECLTimeWarp(2, 0xa60);
            ECLSetTime(ecl, 0x2580, 0x3c, 0x0);
            break;
        case THPrac::TH07::TH07_ST1_BOSS1:
            if (thPracParam.dlg)
                ECLTimeWarp(2, 0x13b2);
            else {
                ECLTimeWarp(2, 0x13b3);
                ECLNameFix();
            }
            break;
        case THPrac::TH07::TH07_ST1_BOSS2:
            ECLNameFix();
            ECLTimeWarp(2, 0x13b3);
            ECLSetTime(ecl, 0x4c20, 0x3c, 0);
            break;
        case THPrac::TH07::TH07_ST1_BOSS3:
            ECLNameFix();
            ECLTimeWarp(2, 0x13b3);
            ecl << pair{0x4c2c, 0x27} << pair{0x4d7c, (int16_t)0} << pair{0x4d8c, (int16_t)0};
            break;
        case THPrac::TH07::TH07_ST1_BOSS4:
            ECLNameFix();
            ECLTimeWarp(2, 0x13b3);
            ecl << pair{0x4c2c, 0x27} << pair{0x4d7c, (int16_t)0} << pair{0x4d8c, (int16_t)0};
            ECLSetTime(ecl, 0x4f44, 0, 0);
            break;
        case THPrac::TH07::TH07_ST2_MID1:
            ECLTimeWarp(3, 0xb0a);
            break;
        case THPrac::TH07::TH07_ST2_MID2:
            ECLTimeWarp(3, 0xb0a);
            ECLSetHealth(ecl, 0x5444, 0, 0x833);
            ecl << pair{0x5388, 0} << pair{0x53a8, 0} << pair{0x53c8, 0}
                << pair{0x53e8, 0} << pair{0x5408, 0} << pair{0x5428, 0};
            break;
        case THPrac::TH07::TH07_ST2_BOSS1:
            if (thPracParam.dlg)
                ECLTimeWarp(3, 0x1dde);
            else
                ECLTimeWarp(3, 0x1ddf);
            break;
        case THPrac::TH07::TH07_ST2_BOSS2:
            ECLTimeWarp(3, 0x1ddf);
            ECLSetTime(ecl, 0x6ba8, 0x3c, 0);
            break;
        case THPrac::TH07::TH07_ST2_BOSS3:
            ECLTimeWarp(3, 0x1ddf);
            ecl << pair{0x6bb4, 0x35} << pair{0x80d4, (int16_t)0} << pair{0x80e4, (int16_t)0};
            break;
        case THPrac::TH07::TH07_ST2_BOSS4:
            ECLTimeWarp(3, 0x1ddf);
            ecl << pair{0x6bb4, 0x35} << pair{0x80d4, (int16_t)0} << pair{0x80e4, (int16_t)0};
            ECLSetTime(ecl, 0x81f4, 0, 0);
            break;
        case THPrac::TH07::TH07_ST2_BOSS5:
            ECLTimeWarp(3, 0x1ddf);
            ecl << pair{0x6bb4, 0x35} << pair{0x80d4, (int16_t)0} << pair{0x80e4, (int16_t)0}
                << pair{0xc698, (int16_t)0} << pair{0xc6a8, (int16_t)0}
                << pair{0xd4ec, (int16_t)0} << pair{0xd4fc, (int16_t)0}
                << pair{0xcdd0, (int16_t)0} << pair{0xcde0, (int16_t)0};
            ECLForceSpell(ecl, 0x81f4, 0, 0x577, 0x578, 0x4444464b, 0xe10, 0x4444464b);
            break;
        case THPrac::TH07::TH07_ST3_MID1:
            ECLST3BG();
            ECLTimeWarp(2, 0x335);
            break;
        case THPrac::TH07::TH07_ST3_MID2:
            ECLST3BG();
            ECLTimeWarp(2, 0x742);
            break;
        case THPrac::TH07::TH07_ST3_MID3:
            ECLST3BG();
            ECLTimeWarp(2, 0x742);
            ECLSetHealth(ecl, 0x2e58, 0x3c, 0x4af);
            break;
        case THPrac::TH07::TH07_ST3_BOSS1:
            ECLST3BG();
            if (thPracParam.dlg)
                ECLTimeWarp(2, 0xd41);
            else
                ECLTimeWarp(2, 0xd42);
            break;
        case THPrac::TH07::TH07_ST3_BOSS2:
            ECLST3BG();
            ECLTimeWarp(2, 0xd42);
            ECLSetTime(ecl, 0x43d0, 0x3c, 0);
            break;
        case THPrac::TH07::TH07_ST3_BOSS3:
            ECLST3BG();
            ECLTimeWarp(2, 0xd42);
            ecl << pair{0x43dc, 36} << pair{0x4d30, (int16_t)0} << pair{0x4d40, (int16_t)0};
            ecl << pair{0x4248, (int16_t)0} << pair{0x4264, (int16_t)0}
                << pair{0x4274, (int16_t)0} << pair{0x4284, (int16_t)0}
                << pair{0x42a4, (int16_t)0} << pair{0x42b4, (int16_t)0};
            break;
        case THPrac::TH07::TH07_ST3_BOSS4:
            ECLST3BG();
            ECLTimeWarp(2, 0xd42);
            ecl << pair{0x42ac, 0x2e} << pair{0x42bc, 0x30} << pair{0x42dc, 0x25};
            ECLSetTime(ecl, 0x43bc, 0x3c, 0);
            break;
        case THPrac::TH07::TH07_ST3_BOSS5:
            ECLST3BG();
            ECLTimeWarp(2, 0xd42);
            ecl << pair{0x43dc, 0x25} << pair{0x4e50, (int16_t)0} << pair{0x4e60, (int16_t)0};
            break;
        case THPrac::TH07::TH07_ST3_BOSS6:
            ECLST3BG();
            ECLTimeWarp(2, 0xd42);
            ecl << pair{0x43dc, 0x25} << pair{0x4e50, (int16_t)0} << pair{0x4e60, (int16_t)0};
            ECLSetTime(ecl, 0x4fb0, 0, 0);
            break;
        case THPrac::TH07::TH07_ST3_BOSS7:
            ECLST3BG();
            ECLTimeWarp(2, 0xd42);
            ecl << pair{0x43dc, 0x25} << pair{0x4e50, (int16_t)0} << pair{0x4e60, (int16_t)0}
                << pair{0x9c94, (int16_t)0} << pair{0x9ca4, (int16_t)0};
            ECLForceSpell(ecl, 0x4fb0, 0, 0x7cf, 0x7d0, 0x38383838, 0xa8c, 0x38383838);
            break;
        case THPrac::TH07::TH07_ST4_MID1:
            ECLTimeWarp(2, 0x1bd2);
            break;
        case THPrac::TH07::TH07_ST4_BOSS1_LUNASA:
            ECLNameFix();
            if (thPracParam.dlg)
                ECLTimeWarp(2, 0x39ea);
            else
                ECLTimeWarp(2, 0x39eb);
            thFakeShot = 0;
            break;
        case THPrac::TH07::TH07_ST4_BOSS1_MERLIN:
            ECLNameFix();
            if (thPracParam.dlg)
                ECLTimeWarp(2, 0x39ea);
            else
                ECLTimeWarp(2, 0x39eb);
            thFakeShot = 4;
            break;
        case THPrac::TH07::TH07_ST4_BOSS1_LYRICA:
            ECLNameFix();
            if (thPracParam.dlg)
                ECLTimeWarp(2, 0x39ea);
            else
                ECLTimeWarp(2, 0x39eb);
            thFakeShot = 2;
            break;
        case THPrac::TH07::TH07_ST4_BOSS2_LUNASA:
            ECLNameFix();
            ECLTimeWarp(2, 0x39eb);
            thFakeShot = 0;
            ECLSetTime(ecl, 0x4934, 0x3c, 0);
            break;
        case THPrac::TH07::TH07_ST4_BOSS2_MERLIN:
            ECLNameFix();
            ECLTimeWarp(2, 0x39eb);
            thFakeShot = 4;
            ECLSetTime(ecl, 0x4934, 0x3c, 0);
            break;
        case THPrac::TH07::TH07_ST4_BOSS2_LYRICA:
            ECLNameFix();
            ECLTimeWarp(2, 0x39eb);
            thFakeShot = 2;
            ECLSetTime(ecl, 0x4934, 0x3c, 0);
            break;
        case THPrac::TH07::TH07_ST4_BOSS3:
            ECLNameFix();
            ECLTimeWarp(2, 0x39eb);
            ecl << pair{0x4880, 0} << pair{0x4adc, 0};
            break;
        case THPrac::TH07::TH07_ST4_BOSS4_LUNASA:
            ECLNameFix();
            ECLTimeWarp(2, 0x39eb);
            ECLCallSub(ecl, 0x4934, 0, 0x2f);
            lunasa();
            break;
        case THPrac::TH07::TH07_ST4_BOSS5_LUNASA:
            ECLNameFix();
            ECLTimeWarp(2, 0x39eb);
            ECLCallSub(ecl, 0x4934, 0, 0x2f);
            ECLSetTime(ecl, 0x51a0, 0x20, 0);
            lunasa();
            timesave();
            break;
        case THPrac::TH07::TH07_ST4_BOSS4_MERLIN:
            ECLNameFix();
            ECLTimeWarp(2, 0x39eb);
            ECLCallSub(ecl, 0x4934, 0, 0x2f);
            merlin();
            break;
        case THPrac::TH07::TH07_ST4_BOSS5_MERLIN:
            ECLNameFix();
            ECLTimeWarp(2, 0x39eb);
            ECLCallSub(ecl, 0x4934, 0, 0x2f);
            ECLSetTime(ecl, 0x51a0, 0x20, 0);
            merlin();
            timesave();
            break;
        case THPrac::TH07::TH07_ST4_BOSS4_LYRICA:
            ECLNameFix();
            ECLTimeWarp(2, 0x39eb);
            ECLCallSub(ecl, 0x4934, 0, 0x2f);
            lyrica();
            break;
        case THPrac::TH07::TH07_ST4_BOSS5_LYRICA:
            ECLNameFix();
            ECLTimeWarp(2, 0x39eb);
            ECLCallSub(ecl, 0x4934, 0, 0x2f);
            ECLSetTime(ecl, 0x51a0, 0x20, 0);
            lyrica();
            timesave();
            break;
        case THPrac::TH07::TH07_ST4_BOSS6:
            ECLNameFix();
            ECLTimeWarp(2, 0x39eb);
            ECLCallSub(ecl, 0x4934, 0, 0x31);
            break;
        case THPrac::TH07::TH07_ST4_BOSS7:
            ECLNameFix();
            ECLTimeWarp(2, 0x39eb);
            ECLCallSub(ecl, 0x4934, 0, 0x31);
            ECLForceSpell(ecl, 0x5488, 0, 0x95f, 0x960, 0x33333333, 0xe10, 0x33333333);
            break;
        case THPrac::TH07::TH07_ST5_MID1:
            ECLTimeWarp(2, 0x12d4);
            if (!thPracParam.dlg)
                ecl << pair{0xb74c, (uint16_t)13};
            break;
        case THPrac::TH07::TH07_ST5_MID2:
            ECLTimeWarp(2, 0x12d4);
            ecl << pair{0xb748, (int16_t)0};
            ECLSetHealth(ecl, 0x38b4, 0x3c, 0x5db);
            break;
        case THPrac::TH07::TH07_ST5_BOSS1:
            if (thPracParam.dlg)
                ECLTimeWarp(2, 0x17e1);
            else
                ECLTimeWarp(2, 0x17e2);
            break;
        case THPrac::TH07::TH07_ST5_BOSS2:
            ECLTimeWarp(2, 0x17e2);
            ECLSetTime(ecl, 0x58f0, 0x3c, 0);
            break;
        case THPrac::TH07::TH07_ST5_BOSS3:
            ECLTimeWarp(2, 0x17e2);
            ECLCallSub(ecl, 0x58f0, 0x3c, 0x34);
            ecl << pair{0x68dc, (int16_t)0} << pair{0x68ec, (int16_t)0};
            break;
        case THPrac::TH07::TH07_ST5_BOSS4:
            ECLTimeWarp(2, 0x17e2);
            ECLCallSub(ecl, 0x58f0, 0x3c, 0x34);
            ecl << pair{0x68dc, (int16_t)0} << pair{0x68ec, (int16_t)0};
            ECLSetTime(ecl, 0x68f8, 0, 0);
            break;
        case THPrac::TH07::TH07_ST5_BOSS5:
            ECLTimeWarp(2, 0x17e2);
            ECLCallSub(ecl, 0x58f0, 0x3c, 0x37);
            ecl << pair{0x7010, (int16_t)0} << pair{0x7020, (int16_t)0}
                << pair{0x7110, 0};
            break;
        case THPrac::TH07::TH07_ST5_BOSS6:
            ecl << pair{0x5820, 0} << pair{0x5840, 0} << pair{0x5860, 0} << pair{0x5880, 0}
                << pair{0x58a0, 0} << pair{0x58c0, 0} << pair{0x58dc, 0}
                << pair{0x7010, (int16_t)0} << pair{0x7020, (int16_t)0} << pair{0xa228, (int16_t)0} << pair{0xa238, (int16_t)0};
            ECLTimeWarp(2, 0x17e2);
            ECLCallSub(ecl, 0x58f0, 0, 0x37);
            ECLSetHealth(ecl, 0x7110, 0, 0x7cf);
            break;
        case THPrac::TH07::TH07_ST6_CHAOS:
            ECLTimeWarp(2, 0x49c);
            break;
        case THPrac::TH07::TH07_ST6_MID1:
            ECLTimeWarp(2, 0x77a);
            if (!thPracParam.dlg)
                ecl << pair{0xe630, (int16_t)0};
            break;
        case THPrac::TH07::TH07_ST6_MID2:
            ECLTimeWarp(2, 0x77a);
            ecl << pair{0xe630, (int16_t)0};
            ECLSetHealth(ecl, 0x17cc, 0x3c, 0x5db);
            break;
        case THPrac::TH07::TH07_ST6_BOSS1:
            if (thPracParam.dlg)
                ECLTimeWarp(2, 2518);
            else {
                ECLTimeWarp(2, 0x9d7);
                ECLNameFix();
                ecl << pair{0xe678, (int16_t)0x9d7};
            }
            break;
        case THPrac::TH07::TH07_ST6_BOSS2:
            ECLNameFix();
            ECLTimeWarp(2, 0x9d7);
            ecl << pair{0xe678, (int16_t)0x9d7};
            ECLSetTime(ecl, 0x3324, 0x3c, 0);
            break;
        case THPrac::TH07::TH07_ST6_BOSS3:
            ECLNameFix();
            yuyuko();
            break;
        case THPrac::TH07::TH07_ST6_BOSS4:
            ECLNameFix();
            yuyuko();
            ECLSetTime(ecl, 0x445c, 0, 0);
            break;
        case THPrac::TH07::TH07_ST6_BOSS5:
            ECLNameFix();
            yuyuko();
            ECLCallSub(ecl, 0x445c, 0, 0x24);
            ecl << pair{0x47a4, (int16_t)0} << pair{0x47b4, (int16_t)0};
            break;
        case THPrac::TH07::TH07_ST6_BOSS6:
            ECLNameFix();
            yuyuko();
            ECLCallSub(ecl, 0x445c, 0, 0x24);
            ecl << pair{0x47a4, (int16_t)0} << pair{0x47b4, (int16_t)0};
            ECLSetTime(ecl, 0x48d4, 0, 0);
            break;
        case THPrac::TH07::TH07_ST6_BOSS7:
            ECLNameFix();
            ECLTimeWarp(2, 0x9d7);
            ecl << pair{0x5574, (int16_t)0} << pair{0x5584, (int16_t)0}
                << pair{0xe678, (int16_t)0x9d7} << pair{0x3330, 0x26};
            break;
        case THPrac::TH07::TH07_ST6_BOSS8:
            ECLNameFix();
            ECLTimeWarp(2, 0x9d7);
            ecl << pair{0x5574, (int16_t)0} << pair{0x5584, (int16_t)0}
                << pair{0xe678, (int16_t)0x9d7} << pair{0x3330, 0x26};
            ECLSetTime(ecl, 0x5694, 0, 0);
            break;
        case THPrac::TH07::TH07_ST6_BOSS9:
            ECLNameFix();
            ecl << pair{0x3240, 0} << pair{0x3260, 0} << pair{0x3280, 0} << pair{0x32a0, 0}
                << pair{0x32c0, 0} << pair{0x32e0, 0} << pair{0x32fc, 0} << pair{0x3310, 0} << pair{0x3324, 0}
                << pair{0xa4b4, (int16_t)0} << pair{0xa4c4, (int16_t)0}
                << pair{0xe678, (int16_t)0x9d7} << pair{0x3330, 0x3a} << pair{0x3188, (int16_t)0};
            ECLTimeWarp(2, 0x9d7);
            break;
        case THPrac::TH07::TH07_ST6_BOSS10:
            ECLNameFix();
            th07_rb.Enable();
            ECLTimeWarp(2, 0x9d7);
            ecl << pair{0xe678, (int16_t)0x9d7} << pair{0x30dc, 0x3e}
                << pair{0xb9f4, (int16_t)0} << pair{0xbab0, 0} << pair{0xc078, (int16_t)0};
            break;
        case THPrac::TH07::TH07_ST7_MID1:
            ECLTimeWarp(2, 0xfe1);
            if (!thPracParam.dlg)
                ecl << pair{0xee58, 0};
            break;
        case THPrac::TH07::TH07_ST7_MID2:
            ECLTimeWarp(2, 0xfe1);
            ecl << pair{0xee58, 0};
            ECLSetTime(ecl, 0x3a70, 0x3c, 0);
            break;
        case THPrac::TH07::TH07_ST7_MID3:
            ECLTimeWarp(2, 0xfe1);
            ecl << pair{0xee58, 0}
                << pair{0x3900, (int16_t)0} << pair{0x3a7c, 0x3d}
                << pair{0x39a0, 0} << pair{0x39c0, 0} << pair{0x39e0, 0} << pair{0x3a00, 0}
                << pair{0x3a20, 0} << pair{0x3a40, 0} << pair{0x3a5c, 0} << pair{0x3a70, 0};
            break;
        case THPrac::TH07::TH07_ST7_END_NS1:
            if (thPracParam.dlg)
                ECLTimeWarp(2, 8798);
            else {
                ECLNameFix();
                ECLTimeWarp(2, 0x225f);
            }
            break;
        case THPrac::TH07::TH07_ST7_END_S1:
            ECLNameFix();
            ECLTimeWarp(2, 0x225f);
            ECLSetTime(ecl, 0x4ce0, 0x3c, 0);
            break;
        case THPrac::TH07::TH07_ST7_END_NS2:
            ECLNameFix();
            ECLTimeWarp(2, 0x225f);
            ecl << pair{0x4df8, (int16_t)0} << pair{0x4e08, (int16_t)0} << pair{0x4e18, (int16_t)0};
            ECLCallSub(ecl, 0x4ce0, 0x3c, 0x46);
            break;
        case THPrac::TH07::TH07_ST7_END_S2:
            ECLNameFix();
            ECLTimeWarp(2, 0x225f);
            ecl << pair{0x4df8, (int16_t)0} << pair{0x4e08, (int16_t)0} << pair{0x4e18, (int16_t)0};
            ECLCallSub(ecl, 0x4ce0, 0x3c, 0x46);
            ECLSetTime(ecl, 0x4f40, 0, 0);
            break;
        case THPrac::TH07::TH07_ST7_END_NS3:
            ECLNameFix();
            ECLTimeWarp(2, 0x225f);
            ecl << pair{0x4fd4, (int16_t)0} << pair{0x4fe4, (int16_t)0};
            ECLCallSub(ecl, 0x4ce0, 0x3c, 0x47);
            break;
        case THPrac::TH07::TH07_ST7_END_S3:
            ECLNameFix();
            ECLTimeWarp(2, 0x225f);
            ecl << pair{0x4fd4, (int16_t)0} << pair{0x4fe4, (int16_t)0};
            ECLCallSub(ecl, 0x4ce0, 0x3c, 0x47);
            ECLSetTime(ecl, 0x510c, 0, 0);
            break;
        case THPrac::TH07::TH07_ST7_END_NS4:
            ECLNameFix();
            ECLTimeWarp(2, 0x225f);
            ecl << pair{0x5210, (int16_t)0} << pair{0x5220, (int16_t)0} << pair{0x5230, (int16_t)0};
            ECLCallSub(ecl, 0x4ce0, 0x3c, 0x49);
            break;
        case THPrac::TH07::TH07_ST7_END_S4:
            ECLNameFix();
            ECLTimeWarp(2, 0x225f);
            ecl << pair{0x5210, (int16_t)0} << pair{0x5220, (int16_t)0} << pair{0x5230, (int16_t)0};
            ECLCallSub(ecl, 0x4ce0, 0x3c, 0x49);
            ECLSetTime(ecl, 0x5358, 0, 0);
            break;
        case THPrac::TH07::TH07_ST7_END_NS5:
            ECLNameFix();
            ECLTimeWarp(2, 0x225f);
            ecl << pair{0x5430, (int16_t)0} << pair{0x5440, (int16_t)0};
            ECLCallSub(ecl, 0x4ce0, 0x3c, 0x4b);
            break;
        case THPrac::TH07::TH07_ST7_END_S5:
            ECLNameFix();
            ECLTimeWarp(2, 0x225f);
            ecl << pair{0x5430, (int16_t)0} << pair{0x5440, (int16_t)0};
            ECLCallSub(ecl, 0x4ce0, 0x3c, 0x4b);
            ECLSetTime(ecl, 0x5590, 0, 0);
            break;
        case THPrac::TH07::TH07_ST7_END_NS6:
            ECLNameFix();
            ECLTimeWarp(2, 0x225f);
            ecl << pair{0x56e4, (int16_t)0} << pair{0x56f4, (int16_t)0} << pair{0x5704, (int16_t)0};
            ECLCallSub(ecl, 0x4ce0, 0x3c, 0x4d);
            break;
        case THPrac::TH07::TH07_ST7_END_S6:
            ECLNameFix();
            ECLTimeWarp(2, 0x225f);
            ecl << pair{0x56e4, (int16_t)0} << pair{0x56f4, (int16_t)0} << pair{0x5704, (int16_t)0};
            ECLCallSub(ecl, 0x4ce0, 0x3c, 0x4d);
            ECLSetTime(ecl, 0x5854, 0, 0);
            break;
        case THPrac::TH07::TH07_ST7_END_NS7:
            ECLNameFix();
            ECLTimeWarp(2, 0x225f);
            ecl << pair{0x5964, (int16_t)0} << pair{0x5974, (int16_t)0};
            ECLCallSub(ecl, 0x4ce0, 0x3c, 0x4f);
            break;
        case THPrac::TH07::TH07_ST7_END_S7:
            ECLNameFix();
            ECLTimeWarp(2, 0x225f);
            ecl << pair{0x5964, (int16_t)0} << pair{0x5974, (int16_t)0};
            ECLCallSub(ecl, 0x4ce0, 0x3c, 0x4f);
            ECLSetTime(ecl, 0x5ac4, 0, 0);
            break;
        case THPrac::TH07::TH07_ST7_END_NS8:
            ECLNameFix();
            ECLTimeWarp(2, 0x225f);
            ecl << pair{0x5c28, (int16_t)0} << pair{0x5c38, (int16_t)0} << pair{0x5c48, (int16_t)0};
            ECLCallSub(ecl, 0x4ce0, 0x3c, 0x51);
            break;
        case THPrac::TH07::TH07_ST7_END_S8:
            ECLNameFix();
            ECLTimeWarp(2, 0x225f);
            ecl << pair{0x5c28, (int16_t)0} << pair{0x5c38, (int16_t)0} << pair{0x5c48, (int16_t)0};
            ECLCallSub(ecl, 0x4ce0, 0x3c, 0x51);
            ECLSetTime(ecl, 0x5dc0, 0, 0);
            break;
        case THPrac::TH07::TH07_ST7_END_S9:
            ECLNameFix();
            ECLTimeWarp(2, 0x225f);
            ecl << pair{0x4b5c, (int16_t)0} << pair{0x5ebc, (int16_t)0} << pair{0x5ecc, (int16_t)0}
                << pair{0x5fdc, 0} << pair{0x5ff0, 0};
            ECLCallSub(ecl, 0x4ce0, 0x3c, 0x53);
            break;
        case THPrac::TH07::TH07_ST7_END_S10:
            ECLNameFix();
            ECLTimeWarp(2, 0x225f);
            ecl << pair{0x4b5c, (int16_t)0} << pair{0x6030, (int16_t)0} << pair{0x6040, (int16_t)0}
                << pair{0x614c, 0} << pair{0x6160, 0};
            ECLCallSub(ecl, 0x4ce0, 0x3c, 0x54);
            break;
        case THPrac::TH07::TH07_ST8_MID1:
            ECLTimeWarp(2, 0xf7d);
            if (!thPracParam.dlg)
                ecl << pair{0x12674, 0};
            break;
        case THPrac::TH07::TH07_ST8_MID2:
            ECLTimeWarp(2, 0xf7d);
            ecl << pair{0x12674, 0};
            ECLSetTime(ecl, 0x4350, 0x3c, 0);
            break;
        case THPrac::TH07::TH07_ST8_MID3:
            ECLTimeWarp(2, 0xf7d);
            ECLCallSub(ecl, 0x4350, 0x0, 0x3f);
            ecl << pair{0x12674, 0}
                << pair{0x41e0, (int16_t)0}
                << pair{0x4280, 0} << pair{0x42a0, 0} << pair{0x42c0, 0} << pair{0x42e0, 0}
                << pair{0x4300, 0} << pair{0x4320, 0} << pair{0x433c, 0};
            break;
        case THPrac::TH07::TH07_ST8_END_NS1:
            if (thPracParam.dlg)
                ECLTimeWarp(2, 9258);
            else {
                ECLNameFix();
                ECLTimeWarp(2, 0x242b);
            }
            break;
        case THPrac::TH07::TH07_ST8_END_S1:
            ECLNameFix();
            ECLTimeWarp(2, 0x242b);
            ECLSetTime(ecl, 0x55ec, 0x3c, 0);
            break;
        case THPrac::TH07::TH07_ST8_END_NS2:
            ECLNameFix();
            ECLTimeWarp(2, 0x242b);
            ecl << pair{0x5704, (int16_t)0} << pair{0x5714, (int16_t)0} << pair{0x5724, (int16_t)0};
            ECLCallSub(ecl, 0x55ec, 0x3c, 0x48);
            break;
        case THPrac::TH07::TH07_ST8_END_S2:
            ECLNameFix();
            ECLTimeWarp(2, 0x242b);
            ecl << pair{0x5704, (int16_t)0} << pair{0x5714, (int16_t)0} << pair{0x5724, (int16_t)0};
            ECLCallSub(ecl, 0x55ec, 0x3c, 0x48);
            ECLSetTime(ecl, 0x584c, 0, 0);
            break;
        case THPrac::TH07::TH07_ST8_END_NS3:
            ECLNameFix();
            ECLTimeWarp(2, 0x242b);
            ecl << pair{0x58e0, (int16_t)0} << pair{0x58f0, (int16_t)0};
            ECLCallSub(ecl, 0x55ec, 0x3c, 0x49);
            break;
        case THPrac::TH07::TH07_ST8_END_S3:
            ECLNameFix();
            ECLTimeWarp(2, 0x242b);
            ecl << pair{0x58e0, (int16_t)0} << pair{0x58f0, (int16_t)0};
            ECLCallSub(ecl, 0x55ec, 0x3c, 0x49);
            ECLSetTime(ecl, 0x5a18, 0, 0);
            break;
        case THPrac::TH07::TH07_ST8_END_NS4:
            ECLNameFix();
            ECLTimeWarp(2, 0x242b);
            ecl << pair{0x5b1c, (int16_t)0} << pair{0x5b2c, (int16_t)0} << pair{0x5b3c, (int16_t)0};
            ECLCallSub(ecl, 0x55ec, 0x3c, 0x4b);
            break;
        case THPrac::TH07::TH07_ST8_END_S4:
            ECLNameFix();
            ECLTimeWarp(2, 0x242b);
            ecl << pair{0x5b1c, (int16_t)0} << pair{0x5b2c, (int16_t)0} << pair{0x5b3c, (int16_t)0};
            ECLCallSub(ecl, 0x55ec, 0x3c, 0x4b);
            ECLSetTime(ecl, 0x5c64, 0, 0);
            break;
        case THPrac::TH07::TH07_ST8_END_NS5:
            ECLNameFix();
            ECLTimeWarp(2, 0x242b);
            ecl << pair{0x5d3c, (int16_t)0} << pair{0x5d4c, (int16_t)0};
            ECLCallSub(ecl, 0x55ec, 0x3c, 0x4d);
            break;
        case THPrac::TH07::TH07_ST8_END_S5:
            ECLNameFix();
            ECLTimeWarp(2, 0x242b);
            ecl << pair{0x5d3c, (int16_t)0} << pair{0x5d4c, (int16_t)0};
            ECLCallSub(ecl, 0x55ec, 0x3c, 0x4d);
            ECLSetTime(ecl, 0x5e9c, 0, 0);
            break;
        case THPrac::TH07::TH07_ST8_END_NS6:
            ECLNameFix();
            ECLTimeWarp(2, 0x242b);
            ecl << pair{0x5ff0, (int16_t)0} << pair{0x6000, (int16_t)0} << pair{0x6010, (int16_t)0};
            ECLCallSub(ecl, 0x55ec, 0x3c, 0x4f);
            break;
        case THPrac::TH07::TH07_ST8_END_S6:
            ECLNameFix();
            ECLTimeWarp(2, 0x242b);
            ecl << pair{0x5ff0, (int16_t)0} << pair{0x6000, (int16_t)0} << pair{0x6010, (int16_t)0};
            ECLCallSub(ecl, 0x55ec, 0x3c, 0x4f);
            ECLSetTime(ecl, 0x6160, 0, 0);
            break;
        case THPrac::TH07::TH07_ST8_END_NS7:
            ECLNameFix();
            ECLTimeWarp(2, 0x242b);
            ecl << pair{0x6270, (int16_t)0} << pair{0x6280, (int16_t)0};
            ECLCallSub(ecl, 0x55ec, 0x3c, 0x51);
            break;
        case THPrac::TH07::TH07_ST8_END_S7:
            ECLNameFix();
            ECLTimeWarp(2, 0x242b);
            ecl << pair{0x6270, (int16_t)0} << pair{0x6280, (int16_t)0};
            ECLCallSub(ecl, 0x55ec, 0x3c, 0x51);
            ECLSetTime(ecl, 0x63d0, 0, 0);
            break;
        case THPrac::TH07::TH07_ST8_END_NS8:
            ECLNameFix();
            ECLTimeWarp(2, 0x242b);
            ecl << pair{0x6534, (int16_t)0} << pair{0x6544, (int16_t)0} << pair{0x6554, (int16_t)0};
            ECLCallSub(ecl, 0x55ec, 0x3c, 0x53);
            break;
        case THPrac::TH07::TH07_ST8_END_S8:
            ECLNameFix();
            ECLTimeWarp(2, 0x242b);
            ecl << pair{0x6534, (int16_t)0} << pair{0x6544, (int16_t)0} << pair{0x6554, (int16_t)0};
            ECLCallSub(ecl, 0x55ec, 0x3c, 0x53);
            ECLSetTime(ecl, 0x66cc, 0, 0);
            break;
        case THPrac::TH07::TH07_ST8_END_S9:
            ECLNameFix();
            ECLTimeWarp(2, 0x242b);
            ecl << pair{0x5468, (int16_t)0} << pair{0x67d0, (int16_t)0} << pair{0x67e0, (int16_t)0}
                << pair{0x68e8, 0} << pair{0x68fc, 0};
            ECLCallSub(ecl, 0x55ec, 0x3c, 0x55);
            break;
        case THPrac::TH07::TH07_ST8_END_S10:
            ECLNameFix();
            ECLTimeWarp(2, 0x242b);
            ecl << pair{0x5468, (int16_t)0} << pair{0x693c, (int16_t)0} << pair{0x694c, (int16_t)0} << pair{0x695c, (int16_t)0}
                << pair{0x6a68, 0} << pair{0x6a7c, 0};
            ECLCallSub(ecl, 0x55ec, 0x3c, 0x56);

            if (thPracParam.phase == 1) {
                ecl << pair{0xe154, 0xa};
            }
            break;
        case THPrac::TH07::TH07_ST8_END_S11:
            ECLNameFix();
            ECLTimeWarp(2, 0x242b);
            ecl << pair{0x5468, (int16_t)0} << pair{0x6abc, (int16_t)0} << pair{0x6acc, (int16_t)0}
                << pair{0x6bd8, 0} << pair{0x6bec, 0};
            ECLCallSub(ecl, 0x55ec, 0x3c, 0x57);
            break;
        default:
            break;
        }
    }
    __declspec(noinline) void THSectionPatch()
    {
        if (thPracParam.section)
            *((int8_t*)(0x4bfee0)) = 0;

        ECLHelper ecl;
        ecl.SetBaseAddr((void*)GetMemAddr(0x1347938));

        auto section = thPracParam.section;
        if (section >= 10000 && section < 20000) {
            int stage = (section - 10000) / 100;
            int portionId = (section - 10000) % 100;
            THStageWarp(ecl, stage, portionId);
        } else {
            THPatch(ecl, (th_sections_t)section);
        }
    }

    void THSetPoint()
    {
        int32_t* point_stage = (int32_t*)GetMemAddr(0x626278, 0x24);
        int32_t* point_total = (int32_t*)GetMemAddr(0x626278, 0x28);
        if (thPracParam.point) {
            *point_stage = *point_total = thPracParam.point;
        } else {
            *point_stage = thPracParam.point_stage;
            *point_total = thPracParam.point_total;
        }

        // Recreation of the original code.
        int32_t* diffculty = (int32_t*)0x626280;
        int32_t* awardLevel = (int32_t*)GetMemAddr(0x626278, 0x2c);
        int32_t* awardReq = (int32_t*)GetMemAddr(0x626278, 0x30);
        while (true) {
            if (*diffculty >= 4) {
                if (*awardLevel)
                    *awardReq = (*awardLevel == 1) ? 500 : 500 * (*awardLevel - 2) + 800;
                else
                    *awardReq = 200;
            } else if (*awardLevel >= 3) {
                *awardReq = (*awardLevel >= 5) ? 200 * (*awardLevel - 5) + 800 : 150 * (*awardLevel - 3) + 300;
            } else {
                *awardReq = 75 * (*awardLevel) + 50;
            }
            if (*point_total < *awardReq)
                break;
            ++(*awardLevel);
        }
    }
    bool THBGMTest()
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
    void THSaveReplay(char* rep_name)
    {
        ReplaySaveParam(mb_to_utf16(rep_name, 932).c_str(), thPracParam.GetJson());
    }

    HOOKSET_DEFINE(THMainHook)
    PATCH_DY(th07_reacquire_input, 0x430f03, "0000000074")
    EHOOK_DY(th07_everlasting_bgm, 0x44d2f0, 1, {
        int32_t retn_addr = ((int32_t*)pCtx->Esp)[0];
        int32_t bgm_cmd = ((int32_t*)pCtx->Esp)[1];
        int32_t bgm_id = ((int32_t*)pCtx->Esp)[2];
        int32_t call_addr = ((int32_t*)pCtx->Esp)[7];

        bool el_switch;
        bool is_practice;
        bool result;

        el_switch = *(THOverlay::singleton().mElBgm) && !THGuiRep::singleton().mRepStatus && thPracParam.mode && thPracParam.section;
        if (thPracParam.mode && thPracParam.section == TH07_ST6_BOSS10)
            el_switch = false;
        is_practice = *((int32_t*)0x575aa8) == 10;
        if (retn_addr == 0x43a180)
            result = ElBgmTest<0x439f47, 0x43a0cb, 0x42d9c4, 0x4034a9, 0x42f21b>(
                el_switch, is_practice, 0x439f47, 2, 2, call_addr);
        else
            result = ElBgmTest<0x439f47, 0x43a0cb, 0x42d9c4, 0x4034a9, 0x42f21b>(
                el_switch, is_practice, retn_addr, bgm_cmd, bgm_id, call_addr);

        if (result) {
            pCtx->Eip = 0x44d3ce;
        }
    })
    EHOOK_DY(th07_prac_menu_1, 0x45a214, 2, {
        THGuiPrac::singleton().State(1);
    })
    EHOOK_DY(th07_prac_menu_3, 0x45a65d, 10, {
        THGuiPrac::singleton().State(3);
        *((int32_t*)0x62f85c) = thPracParam.stage;
        if (thPracParam.stage == 6)
            *((int32_t*)0x626280) = 4;
        else if (thPracParam.stage == 7)
            *((int32_t*)0x626280) = 5;
    })
    EHOOK_DY(th07_prac_menu_4, 0x45a6d4, 2, {
        THGuiPrac::singleton().State(4);
    })
    EHOOK_DY(th07_rep_menu_1, 0x45ac43, 6, {
        THGuiRep::singleton().State(1);
    })
    EHOOK_DY(th07_rep_menu_2, 0x45af96, 5, {
        THGuiRep::singleton().State(2);
    })
    EHOOK_DY(th07_rep_menu_3, 0x45b2c1, 2, {
        THGuiRep::singleton().State(3);
    })
    EHOOK_DY(th07_patch_main, 0x42f2e3, 1, {
        th07_rb.Disable();
        if (thPracParam.mode == 1) {
            float* life = (float*)GetMemAddr(0x626278, 0x5c);
            *life = (thPracParam.life);

            float* bomb = (float*)GetMemAddr(0x626278, 0x68);
            *bomb = (thPracParam.bomb);

            float* power = (float*)GetMemAddr(0x626278, 0x7c);
            *power = thPracParam.power;

            int32_t real_score = (int32_t)(thPracParam.score / 10);
            int32_t* score1 = (int32_t*)GetMemAddr(0x626278, 0x0);
            int32_t* score2 = (int32_t*)GetMemAddr(0x626278, 0x4);
            *score1 = *score2 = real_score;

            int32_t* graze1 = (int32_t*)GetMemAddr(0x626278, 0x14);
            int32_t* graze2 = (int32_t*)GetMemAddr(0x626278, 0x18);
            *graze1 = *graze2 = thPracParam.graze;

            if (thPracParam.frame) {
                ECLST3BG();
                ECLTimeWarp(2, thPracParam.frame);
            }

            THSetPoint();

            int32_t* cherry = (int32_t*)0x62f88c;
            int32_t* cherryMax = (int32_t*)0x62f888;
            int32_t* cherryPlus = (int32_t*)0x62f890;
            int32_t cherryBase = (int32_t)GetMemContent(0x626278, 0x88);
            *cherry = cherryBase + thPracParam.cherry;
            *cherryMax = cherryBase + thPracParam.cherryMax;
            *cherryPlus = cherryBase + thPracParam.cherryPlus;

            int32_t* spellBonus = (int32_t*)GetMemAddr(0x626278, 0x1C);
            *spellBonus = thPracParam.spellBonus;

            if (thPracParam.rank) {
                *(int32_t*)(0x62f8a4) = (int32_t)thPracParam.rank;
                if (thPracParam.rankLock) {
                    *(int32_t*)(0x62f8a8) = (int32_t)thPracParam.rank;
                    *(int32_t*)(0x62f8ac) = (int32_t)thPracParam.rank;
                }
            }

            // ECL Patch
            THSectionPatch();
        }
        thPracParam._playLock = true;
    })
    EHOOK_DY(th07_disable_title, 0x42956b, 5, {
        if (thPracParam.mode == 1 && (thPracParam.section || thPracParam.frame)) {
            pCtx->Esp += 0xC;
            pCtx->Eip = 0x429570;
        }
    })
    EHOOK_DY(th07_fake_shot, 0x40e6ba, 7, {
        if (thFakeShot != -1) {
            pCtx->Eax = thFakeShot;
            pCtx->Eip = 0x40eaca;
        }
    })
    EHOOK_DY(th07_bgm, 0x42f206, 7, {
        if (thPracParam.mode == 1 && thPracParam.section) {
            if (thPracParam.stage == 5) {
                asm_call<0x439dd0, Thiscall>(0x575950, 2, 0x4986b4);
            }

            if (THBGMTest()) {
                if (thPracParam.section == TH07_ST6_BOSS10) {
                    PushHelper32(pCtx, 2);
                    pCtx->Eip = 0x42f211;
                } else {
                    PushHelper32(pCtx, 1);
                    pCtx->Eip = 0x42f211;
                }
            } else {
                PushHelper32(pCtx, 0);
                pCtx->Eip = 0x42f211;
            }

        }
    })
    EHOOK_DY(th07_bgm_st6_1, 0x427eda, 7, {
        if (thPracParam.mode == 1 && thPracParam.section) {
            pCtx->Eip = 0x427efa;
        }
    })
    EHOOK_DY(th07_bgm_st6_2, 0x42d9a5, 10, {
        if (thPracParam.mode == 1 && thPracParam.section) {
            pCtx->Eip = 0x42d9b1;
        }
    })
    EHOOK_DY(th07_bgm_st6_3, 0x40348a, 10, {
        if (thPracParam.mode == 1 && thPracParam.section) {
            pCtx->Eip = 0x403496;
        }
    })
    EHOOK_DY(th07_save_replay, 0x4443fd, 3, {
        char* rep_name = *(char**)(pCtx->Ebp - 0x134);
        if (thPracParam.mode)
            THSaveReplay(rep_name);
    })
    PATCH_DY(th07_disable_prac_menu1, 0x45b9ea, "9090909090")
    PATCH_DY(th07_disable_prac_menu2, 0x45bb1c, "9090909090")
    EHOOK_DY(th07_update, 0x42fdf8, 1, {
        GameGuiBegin(IMPL_WIN32_DX8, !THAdvOptWnd::singleton().IsOpen());

        // Gui components update
        THGuiPrac::singleton().Update();
        THGuiRep::singleton().Update();
        THOverlay::singleton().Update();
        bool drawCursor = THAdvOptWnd::StaticUpdate() || THGuiPrac::singleton().IsOpen();

        GameGuiEnd(drawCursor);
    })
    EHOOK_DY(th07_render, 0x42feb9, 1, {
        GameGuiRender(IMPL_WIN32_DX8);
        if (Gui::KeyboardInputUpdate(VK_HOME) == 1)
            THSnapshot::Snapshot(*(IDirect3DDevice8**)0x575958);
    })
    HOOKSET_ENDDEF()

    static __declspec(noinline) void THGuiCreate()
    {
        if (ImGui::GetCurrentContext()) {
            return;
        }
        // Init
        GameGuiInit(IMPL_WIN32_DX8, 0x575958, 0x575c20,
            Gui::INGAGME_INPUT_GEN1, 0x4b9e4c, 0x4b9e54, 0x4b9e5c,
            -1);

        SetDpadHook(0x430760, 3);

        // Gui components creation
        THGuiPrac::singleton();
        THGuiRep::singleton();
        THOverlay::singleton();
        th07_rb.Setup();

        // Hooks
        EnableAllHooks(THMainHook);

        // Reset thPracParam
        thPracParam.Reset();
    }
    HOOKSET_DEFINE(THInitHook)
    PATCH_DY(th07_disable_dataver, 0x404fe0, "33c0c3")
    PATCH_DY(th07_disable_demo, 0x455a9a, "ffffff7f")
    EHOOK_DY(th07_disable_mutex, 0x435bff, 2, {
        pCtx->Eip = 0x435c1b;
    })
    EHOOK_DY(th07_gui_init_1, 0x45599d, 2, {
        THGuiCreate();
        self->Disable();
    })
    EHOOK_DY(th07_gui_init_2, 0x4351ac, 1, {
        THGuiCreate();
        self->Disable();
    })
    HOOKSET_ENDDEF()
}

void TH07Init()
{
    EnableAllHooks(TH07::THInitHook);
}

}
