#include "thprac_utils.h"

namespace THPrac {
namespace TH185 {
    struct THPracParam {
        int32_t mode;
        int32_t stage;
        int32_t section;
        int32_t phase;
        int32_t statisticsMode;

        int32_t life;

        int32_t bulletMoney;
        int32_t power;
        int32_t magicPower;
        int32_t speed;
        int32_t magicBreak;
        int32_t sAttack;
        int32_t magicTime;
        int32_t magicSize;
        int32_t reload;
        int32_t cooltime;


        bool dlg;

        bool _playLock = false;
        void Reset()
        {
            memset(this, 0, sizeof(THPracParam));
        }
        bool ReadJson(std::string& json)
        {
            ParseJson();

            ForceJsonValue(game, "th185");
            GetJsonValue(mode);
            GetJsonValue(stage);
            GetJsonValue(section);
            GetJsonValue(phase);
            GetJsonValueEx(dlg, Bool);
            GetJsonValue(statisticsMode);

            GetJsonValue(life);


            GetJsonValue(bulletMoney);
            GetJsonValue(power);
            GetJsonValue(magicPower);
            GetJsonValue(speed);
            GetJsonValue(reload);
            GetJsonValue(cooltime);
            GetJsonValue(magicBreak);
            GetJsonValue(sAttack);
            GetJsonValue(magicTime);
            GetJsonValue(magicSize);
            GetJsonValue(speed);

            return true;
        }
        std::string GetJson()
        {
            CreateJson();

            AddJsonValueEx(version, GetVersionStr(), jalloc);
            AddJsonValueEx(game, "th185", jalloc);
            AddJsonValue(mode);
            AddJsonValue(stage);
            AddJsonValue(statisticsMode);
            if (section)
                AddJsonValue(section);
            if (phase)
                AddJsonValue(phase);
            if (dlg)
                AddJsonValue(dlg);

            AddJsonValue(life);
            AddJsonValue(bulletMoney);
            AddJsonValue(power);
            AddJsonValue(magicPower);

            AddJsonValue(speed);
            AddJsonValue(reload);
            AddJsonValue(cooltime);
            AddJsonValue(magicBreak);
            AddJsonValue(sAttack);
            AddJsonValue(magicTime);
            AddJsonValue(magicSize);
            AddJsonValue(speed);


            ReturnJson();
        }
    };
    THPracParam thPracParam {};

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
        }
        virtual void OnContentUpdate() override
        {
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
    };
    
    class THGuiPrac : public Gui::GameGuiWnd {
        THGuiPrac() noexcept
        {
            *mMode = 1;
            *mStatisticsMode = 0;
            *mLife = 1;
            *mBulletMoney = 0;
            *mPower = 1;
            *mMagicPower = 100;
            *mSpeed = 100;
            *mReload = 0;
            *mCooltime = 0;
            *mMagicBreak=0;
            *mSAttack=100;
            *mMagicTime=100;
            *mMagicSize=100;

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
                SetFade(0.8f, 0.1f);
                Open();
                thPracParam.Reset();
                break;
            case 1:
                SetFade(0.8f, 0.1f);
                Close();

                // Fill Param
                thPracParam.mode = *mMode;
                thPracParam.stage = *mStage;
                thPracParam.section = CalcSection();
                thPracParam.statisticsMode = *mStatisticsMode;
                thPracParam.life = *mLife;
                thPracParam.bulletMoney = *mBulletMoney;
                thPracParam.power = *mPower;
                thPracParam.magicPower = *mMagicPower;
                thPracParam.speed = *mSpeed;
                thPracParam.reload = *mReload;
                thPracParam.cooltime = *mCooltime;
                thPracParam.magicBreak = *mMagicBreak; 
                thPracParam.sAttack = *mSAttack;
                thPracParam.magicTime = *mMagicTime;
                thPracParam.magicSize = *mMagicSize;
                break;
            case 2:
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
            ImGui::Text(XSTR(TH_MENU));
            ImGui::Separator();

            PracticeMenu();
        }
        th_glossary_t* SpellPhase()
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
                if (mWarp())
                    *mSection = *mChapter = *mPhase = 0;
                if (*mWarp) {
                    SectionWidget();
                    mPhase(TH_PHASE, SpellPhase());
                }
                mStatisticsMode();
                mLife();
                if (*mStatisticsMode == 1) {
                    mBulletMoney();
                    mPower();
                    mMagicPower();
                    mSpeed();
                    mReload();
                    mCooltime();
                    mMagicBreak();
                    mSAttack();
                    mMagicTime();
                    mMagicSize();
                }
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
            //TODO: Add dialogue section
            switch (section) {
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
                        th_sections_str[::THPrac::Gui::LocaleGet()][0]))
                    *mPhase = 0;
                if (SectionHasDlg(th_sections_cba[*mStage][*mWarp - 2][*mSection]))
                    mDlg();
                break;
            case 4:
            case 5: // Non-spell & Spellcard
                if (mSection(TH_WARP_SELECT[*mWarp],
                        th_sections_cbt[*mStage][*mWarp - 4],
                        th_sections_str[::THPrac::Gui::LocaleGet()][0]))
                    *mPhase = 0;
                if (SectionHasDlg(th_sections_cbt[*mStage][*mWarp - 4][*mSection]))
                    mDlg();
                break;
            default:
                break;
            }
        }

        Gui::GuiCombo mMode { TH_MODE, TH_MODE_SELECT };
        Gui::GuiCombo mStage { TH185_MARKET, TH185_MARKET_SELECT };
        Gui::GuiCombo mWarp { TH_WARP, TH_WARP_SELECT };
        Gui::GuiCombo mSection { TH_MODE };
        Gui::GuiCombo mPhase { TH_PHASE };
        Gui::GuiCombo mStatisticsMode { TH185_STATISTICS_MODE, TH_MODE_SELECT };
        Gui::GuiCheckBox mDlg { TH_DLG };

        Gui::GuiSlider<int, ImGuiDataType_S32> mChapter { TH_CHAPTER, 0, 0 };
        Gui::GuiSlider<int, ImGuiDataType_S32> mLife { TH_LIFE, 0, 9 };
        Gui::GuiSlider<int, ImGuiDataType_S32> mPower { TH_POWER, 1, 10, 1, 1 };

        Gui::GuiDrag<int, ImGuiDataType_S32> mBulletMoney { TH185_BULLET_MONEY, 0, 9999999, 1, 100000 };
        Gui::GuiSlider<int, ImGuiDataType_S32> mMagicPower { TH185_MAGIC_POWER, 0, 999, 10, 10 };
        Gui::GuiSlider<int, ImGuiDataType_S32> mReload { TH185_RELOAD, 0, 999, 10, 10 };
        Gui::GuiSlider<int, ImGuiDataType_S32> mCooltime { TH185_COOLTIME, 0, 999, 10, 10 };
        Gui::GuiSlider<int, ImGuiDataType_S32> mMagicBreak { TH185_MAGIC_BREAK, 0, 999, 10, 10 };
        Gui::GuiSlider<int, ImGuiDataType_S32> mSAttack { TH185_ATTACK_STRENGTH, 0, 999, 10, 10 };
        Gui::GuiSlider<int, ImGuiDataType_S32> mMagicTime { TH185_MAGIC_TIME, 0, 999, 10, 10 };
        Gui::GuiSlider<int, ImGuiDataType_S32> mMagicSize { TH185_MAGIC_SIZE, 0, 999, 10, 10 };
        Gui::GuiSlider<int, ImGuiDataType_S32> mSpeed { TH185_SPEED, 0, 999, 10, 10 };

        Gui::GuiNavFocus mNavFocus { TH185_MARKET, TH_MODE, TH_WARP, TH_DLG, TH185_STATISTICS_MODE,
            TH_MID_STAGE, TH_END_STAGE, TH_NONSPELL, TH_SPELL, TH_PHASE, TH_CHAPTER,
            TH185_SPEED, TH_LIFE, TH185_BULLET_MONEY,
            TH_POWER, TH185_MAGIC_POWER, TH185_RELOAD, TH185_COOLTIME, TH185_MAGIC_BREAK, TH185_ATTACK_STRENGTH, TH185_MAGIC_TIME, TH185_MAGIC_SIZE };

        // TODO: Setup chapters
        int mChapterSetup[7][2] {
            { 2, 4 },
            { 3, 3 },
            { 3, 3 },
            { 3, 3 },
            { 4, 6 },
            { 4, 0 },
            { 5, 4 },
        };
    };
    

    class THAdvOptWnd : public Gui::GameGuiWnd {
        // Option Related Functions
    private:
        void FpsInit()
        {
            if (*(uint8_t*)0x4d54e1 == 3) {
                mOptCtx.fps_status = 1;

                DWORD oldProtect;
                VirtualProtect((void*)0x475306, 4, PAGE_EXECUTE_READWRITE, &oldProtect);
                *(double**)0x475306 = &mOptCtx.fps_dbl;
                VirtualProtect((void*)0x475306, 4, oldProtect, &oldProtect);
            } else
                mOptCtx.fps_status = 0;
        }
        void FpsSet()
        {
            if (mOptCtx.fps_status == 1) {
                mOptCtx.fps_dbl = 1.0 / (double)mOptCtx.fps;
            } else if (mOptCtx.fps_status == 2) {
                //*(int32_t*)(mOptCtx.vpatch_base + 0x16a8c) = mOptCtx.fps;
            }
        }
        void GameplayInit()
        {
        }
        void GameplaySet()
        {
        }

    public:
        THAdvOptWnd() noexcept
        {
            SetWndFlag(ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove);
            SetFade(0.8f, 0.8f);
            SetStyle(ImGuiStyleVar_WindowRounding, 0.0f);
            SetStyle(ImGuiStyleVar_WindowBorderSize, 0.0f);
            OnLocaleChange();

            FpsInit();
            GameplayInit();
        }

    protected:
        virtual void OnLocaleChange() override
        {
            SetTitle(XSTR(TH_ADV_OPT));
            switch (Gui::LocaleGet()) {
            case Gui::LOCALE_ZH_CN:
                SetSizeRel(1.0f, 1.0f);
                SetPosRel(0.0f, 0.0f);
                SetItemWidthRel(-0.075f);
                SetAutoSpacing(true);
                break;
            case Gui::LOCALE_EN_US:
                SetSizeRel(1.0f, 1.0f);
                SetPosRel(0.0f, 0.0f);
                SetItemWidthRel(-0.075f);
                SetAutoSpacing(true);
                break;
            case Gui::LOCALE_JA_JP:
                SetSizeRel(1.0f, 1.0f);
                SetPosRel(0.0f, 0.0f);
                SetItemWidthRel(-0.075f);
                SetAutoSpacing(true);
                break;
            default:
                break;
            }
        }
        virtual void OnContentUpdate() override
        {
            ImGui::Text(XSTR(TH_ADV_OPT));
            ImGui::Separator();
            ImGui::BeginChild("Adv. Options", ImVec2(0.0f, 0.0f));

            if (BeginOptGroup<TH_GAME_SPEED>()) {
                if (GameFPSOpt(mOptCtx, false))
                    FpsSet();
                EndOptGroup();
            }

            AboutOpt();
            ImGui::EndChild();
            ImGui::SetWindowFocus();
        }

        adv_opt_ctx mOptCtx;
    };
    bool UpdateAdvOptWindow()
    {
        static THAdvOptWnd* advOptWnd = nullptr;
        if (!advOptWnd)
            advOptWnd = new THAdvOptWnd();
        if (Gui::KeyboardInputUpdate(VK_F12) == 1) {
            if (advOptWnd->IsOpen())
                advOptWnd->Close();
            else
                advOptWnd->Open();
        }
        advOptWnd->Update();

        return advOptWnd->IsOpen();
    }

    HOOKSET_DEFINE(THDisableGameMenuGuiHooks)
        PATCH_DY(th185_disable_game_menu_gui_1, (void*)0x46da47, "\x68\xff\xeb", 3);
        PATCH_DY(th185_disable_game_menu_gui_2, (void*)0x46da4C, "\x90", 1);
        PATCH_DY(th185_disable_game_menu_gui_3, (void*)0x46db5b, "\x90\x90", 2);
        PATCH_DY(th185_disable_game_menu_gui_4, (void*)0x46dbe2, "\x68\xff\x00", 3);
        PATCH_DY(th185_disable_game_menu_gui_5, (void*)0x46dbe7, "\x90", 1);
        PATCH_DY(th185_disable_game_menu_gui_6, (void*)0x46e496, "\x90\x90", 2); 
    HOOKSET_ENDDEF()

    HOOKSET_DEFINE(THMainHook)
    EHOOK_DY(th185_gui_update, (void*)0x4013dd)
    {
        GameGuiBegin(IMPL_WIN32_DX9);

        // Gui components update
        THOverlay::singleton().Update();
        THGuiPrac::singleton().Update();

        GameGuiEnd(UpdateAdvOptWindow());
    }
    EHOOK_DY(th185_gui_render, (void*)0x4014fa)
    {
        GameGuiRender(IMPL_WIN32_DX9);
    }
    EHOOK_DY(th185_prac_menu_1, (void*)0x46ce3b)
    {
        THDisableGameMenuGuiHooks::singleton().EnableAllHooks();
        THGuiPrac::singleton().State(0);
    }
    EHOOK_DY(th185_prac_menu_2, (void*)0x46da64)
    {
        THDisableGameMenuGuiHooks::singleton().DisableAllHooks();
        THGuiPrac::singleton().State(2);
    }
    EHOOK_DY(th185_prac_menu_3, (void*)0x46da12)
    {
        THDisableGameMenuGuiHooks::singleton().EnableAllHooks();
        THGuiPrac::singleton().State(0);
    }
    EHOOK_DY(th185_prac_menu_4, (void*)0x448460)
    {
        THDisableGameMenuGuiHooks::singleton().DisableAllHooks();
        THGuiPrac::singleton().State(1);
    }
    PATCH_DY(th185_unblock_all, (void*)0x46d532, "\xeb", 1);
    EHOOK_DY(th185_stage_jump, (void*)0x46d523)
    {
        THGuiPrac::singleton().State(1);
        *(int32_t*)(pCtx->Esi + 0xFC) = thPracParam.stage;
    }
    EHOOK_DY(th185_set_game_values, (void*)0x4487d2)
    {
        *(int32_t*)(0x4d10ac) = thPracParam.speed;
        *(int32_t*)(0x4d1070) = thPracParam.bulletMoney;
        *(int32_t*)(0x4d1074) = thPracParam.bulletMoney;
        *(int32_t*)(0x4d10bc) = thPracParam.life;
        *(int32_t*)(0x4d1094) = thPracParam.magicBreak;
        *(int32_t*)(0x4d1088) = thPracParam.sAttack;
        *(int32_t*)(0x4D1090) = thPracParam.magicSize;
        *(int32_t*)(0x4D1098) = thPracParam.magicTime;
        *(int32_t*)(0x4D109C) = thPracParam.reload; //-1
        *(int32_t*)(0x4D10B8) = (thPracParam.cooltime-100)*-1;
        *(int32_t*)(0x4d1078) = (thPracParam.power - 1) * 100;
    }
    HOOKSET_ENDDEF()

    HOOKSET_DEFINE(THInitHook)
    static __declspec(noinline) void THGuiCreate()
    {
        // Init
        GameGuiInit(IMPL_WIN32_DX9, 0x4d52c8, 0x571d50, 0x474760,
            Gui::INGAGME_INPUT_GEN2, 0x4ce400, 0x4ce3f8, 0,
            -2, *(float*)0x573dc0, 0.0f);

        // Gui components creation
        THOverlay::singleton();
        THGuiPrac::singleton();

        // Hooks
        THMainHook::singleton().EnableAllHooks();

        // Reset thPracParam
        // thPracParam.Reset();
    }
    static __declspec(noinline) void THInitHookDisable()
    {
        auto& s = THInitHook::singleton();
        s.th185_gui_init_1.Disable();
        s.th185_gui_init_2.Disable();
    }

	EHOOK_DY(th185_gui_init_1, (void*)0x46ce39)
    {
        THGuiCreate();
        THInitHookDisable();
	}
    EHOOK_DY(th185_gui_init_2, (void*)0x476580)
    {
        THGuiCreate();
        THInitHookDisable();
    }
	HOOKSET_ENDDEF()
}

void TH185Init()
{
    TH185::THInitHook::singleton().EnableAllHooks();
}

}