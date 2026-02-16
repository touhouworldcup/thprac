#include "thprac_games.h"
#include "thprac_utils.h"


namespace THPrac {
namespace Alcostg {
    enum addrs {
        PLAYER_PTR = 0x474194,
    };

    static __forceinline void call_0x413ef0(uint16_t beer) {
#ifndef __clang__
    __asm {
        mov eax, 0x48e580;
        mov cx, beer;
        mov edx, 0x413ef0;
        call edx;
    }
#else
    asm volatile(
        "call *%[func]"
        :
        : [func] "r"(0x413ef0), "a"(0x48e580), "c"(beer));
#endif
    }

    using std::pair;
    struct THPracParam {
        int32_t mode;
        int32_t stage;
        int32_t section;
        int32_t progress;
        int32_t time;
        int16_t beer;
        int16_t beer_max;
        int64_t score;

        void Reset()
        {
            memset(this, 0, sizeof(THPracParam));
        }
        bool ReadJson(std::string& json)
        {
            ParseJson();

            ForceJsonValue(game, "alcostg");
            GetJsonValue(mode);
            GetJsonValue(stage);
            GetJsonValue(section);
            GetJsonValue(progress);
            GetJsonValue(time);
            GetJsonValue(beer);
            GetJsonValue(beer_max);
            GetJsonValue(score);

            return true;
        }
        std::string GetJson()
        {
            CreateJson();

            AddJsonValueEx(version, GetVersionStr(), jalloc);
            AddJsonValueEx(game, "alcostg", jalloc);
            AddJsonValue(mode);
            AddJsonValue(stage);
            if (section)
                AddJsonValue(section);
            if (progress)
                AddJsonValue(progress);

            AddJsonValue(time);
            AddJsonValue(beer);
            AddJsonValue(beer_max);
            AddJsonValue(score);

            ReturnJson();
        }
    };
    THPracParam thPracParam {};
    bool thLock { false };
    bool thHardLock { false };
    bool thRestart { false };

    unsigned int alcostg_beer_cd = 0;
    EHOOK_ST(alcostg_add_beer, 0x4264fc, 1, {
        if (thPracParam.mode == 1) {
            if (++alcostg_beer_cd == 20) {
                call_0x413ef0(thPracParam.beer * 100);
                self->Disable();
            }
        }
    });
    namespace AlcostgBeer {
        // TODO: Should this function take (void)?
        static void Set([[maybe_unused]] int16_t beer)
        {
            alcostg_beer_cd = 0;
            alcostg_add_beer.Enable();
        }
        static void Reset()
        {
            alcostg_beer_cd = 0;
            alcostg_add_beer.Disable();
        }
    };

    class THGuiPrac : public Gui::GameGuiWnd {
        THGuiPrac() noexcept
        {
            *mBeer = 30;
            *mBeerMax = 30;
            mTime.SetCurrentStep(30);

            SetFade(0.8f, 0.1f);
            SetStyle(ImGuiStyleVar_WindowRounding, 0.0f);
            SetStyle(ImGuiStyleVar_WindowBorderSize, 0.0f);
            OnLocaleChange();
        }
        SINGLETON(THGuiPrac);
    public:

        int mState = 0;
        __declspec(noinline) int State()
        {
            uint32_t menu_id;

            switch (mState) {
            case 0:
                menu_id = GetMemContent(0x4741ac, 0x28);
                if (menu_id != 0)
                    return 1;
                SetFade(0.8f, 0.1f);
                Open();
                thPracParam.Reset();
                mState = 1;
                break;
            case 1:
                if (IsOpen()) {
                    if (Gui::InGameInputGetConfirm()) {
                        SetFade(0.8f, 0.1f);
                        Close();

                        // Fill Param
                        thPracParam.mode = *mMode;
                        thPracParam.stage = *mStage;
                        if (*mWarp == 1)
                            thPracParam.progress = *mProgress;
                        else
                            thPracParam.section = *mWarp ? th_sections_cba[*mStage][*mWarp - 1][*mSection] : 0;
                        thPracParam.time = *mTime;
                        thPracParam.beer = static_cast<int16_t>(*mBeer);
                        thPracParam.beer_max = static_cast<int16_t>(*mBeerMax);
                        thPracParam.score = *mScore;

                        mState = 0;

                        return 1;
                    } else if (Gui::InGameInputGet('X')) {
                        mState = 2;
                    }
                }
                break;
            case 2:
                SetFade(0.8f, 0.1f);
                Close();
                if (IsClosed())
                    mState = 0;
            default:
                break;
            }

            return 0;
        }

    protected:
        virtual void OnPreUpdate() override
        {
        }
        virtual void OnLocaleChange() override
        {
            SetTitle(S(TH_MENU));
            switch (Gui::LocaleGet()) {
            case Gui::LOCALE_ZH_CN:
                if (*mMode) {
                    SetPos(120.f, 170.f);
                    SetSize(400.f, 265.f);
                } else {
                    SetPos(170.f, 175.f);
                    SetSize(300.f, 80.f);
                }
                SetItemWidth(-65.0f);
                break;
            case Gui::LOCALE_EN_US:
                if (*mMode) {
                    SetPos(120.f, 170.f);
                    SetSize(400.f, 265.f);
                } else {
                    SetPos(170.f, 175.f);
                    SetSize(300.f, 80.f);
                }
                SetItemWidth(-80.0f);
                break;
            case Gui::LOCALE_JA_JP:
                if (*mMode) {
                    SetPos(120.f, 170.f);
                    SetSize(400.f, 265.f);
                } else {
                    SetPos(170.f, 175.f);
                    SetSize(300.f, 80.f);
                }
                SetItemWidth(-76.0f);
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
        th_glossary_t* SpellPhase()
        {
            return nullptr;
        }
        void PracticeMenu()
        {
            mMode();
            if (mModePrv) {
                char temp[256];
                if (mStage()) {
                    *mSection = 0;
                    switch (*mStage) {
                    case 0:
                        mTime.SetBound(0, 210);
                        mTimeBase = 17;
                        mProgress.SetBound(1, 19);
                        break;
                    case 1:
                        mTime.SetBound(0, 150);
                        mTimeBase = 21;
                        mProgress.SetBound(1, 11);
                        break;
                    case 2:
                        mTime.SetBound(0, 270);
                        mTimeBase = 0;
                        mProgress.SetBound(1, 11);
                        break;
                    }
                    *mTime = 0;
                    *mProgress = 1;
                }
                if (mWarp()) {
                    *mSection = *mPhase = 0;
                    *mProgress = 1;
                }
                if (*mWarp) {
                    if (*mWarp == 1) {
                        if (*mStage == 2 && *mProgress == 6)
                            sprintf_s(temp, S(ALCOSTG_ORDER_MBOSS), *mProgress);
                        else
                            sprintf_s(temp, S(ALCOSTG_ORDER), *mProgress);
                        mProgress(temp);
                    } else {
                        if (mSection(TH_WARP_ALCOSTG[*mWarp], XCBA(*mStage, *mWarp - 1), XSSS(mDiffculty)))
                            *mPhase = 0;
                        mPhase(TH_PHASE, SpellPhase());
                    }
                }

                sprintf_s(temp, "%02d:%02d", *mTime / 60 + mTimeBase, *mTime % 60);
                mTime(temp);
                sprintf_s(temp, "%03.1fL", ((float)(*mBeer) / 10.0f));
                mBeer(temp);
                sprintf_s(temp, "%03.1fL", ((float)(*mBeerMax) / 10.0f));
                auto prevBeerMax = *mBeerMax;
                if (mBeerMax(temp)) {
                    mBeer.SetBound(0, *mBeerMax);
                    if (prevBeerMax == *mBeer)
                        *mBeer = *mBeerMax;
                }
                mScore();
                mScore.RoundDown(10);
            }

            mNavFocus();
            if (mModePrv != *mMode) {
                OnLocaleChange();
                mModePrv = *mMode;
            }
        }

        int mModePrv = 0;
        unsigned int mTimeBase = 17;
        Gui::GuiCombo mMode { TH_MODE, ALCOSTG_MODE_SELECT };

    protected:
        Gui::GuiCombo mStage { TH_STAGE, ALCOSTG_STAGE_SELECT };
        Gui::GuiCombo mWarp { TH_WARP, TH_WARP_ALCOSTG };
        Gui::GuiCombo mSection { TH_MODE };
        Gui::GuiCombo mPhase { TH_PHASE };

        Gui::GuiDrag<int64_t, ImGuiDataType_S64> mScore { TH_SCORE, 0, 99999990, 10, 10000000 };
        Gui::GuiSlider<int, ImGuiDataType_S32> mTime { ALCOSTG_TIME, 0, 210, 1, 60, 30 }; // 1 for 2 sec
        Gui::GuiSlider<int, ImGuiDataType_S32> mBeer { ALCOSTG_BEER, 0, 30, 1, 10 }; // 3000
        Gui::GuiSlider<int, ImGuiDataType_S32> mBeerMax { ALCOSTG_BEER_MAX, 0, 30, 1, 10 };
        Gui::GuiSlider<int, ImGuiDataType_S32> mProgress { TH_PROGRESS, 1, 19, 1, 1 };

        Gui::GuiNavFocus mNavFocus { TH_MODE, TH_STAGE, TH_WARP, TH_PROGRESS,
            TH_MID_STAGE, TH_END_STAGE, TH_NONSPELL, TH_SPELL, TH_PHASE,
            TH_SCORE, ALCOSTG_TIME, ALCOSTG_BEER, ALCOSTG_BEER_MAX };

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
            uint32_t index = GetMemContent(0x4741ac, 0x5650);
            char* repName = (char*)GetMemAddr(0x4741ac, index * 4 + 0x5658, 0x104);
            std::wstring repDir(L"replay\\");
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
                AlcostgBeer::Reset();
                thLock = false;
                thHardLock = false;
                thRestart = false;
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
            mFreeMiss.SetTextOffsetRel(x_offset_1, x_offset_2);
            mFreeBomb.SetTextOffsetRel(x_offset_1, x_offset_2);
            mAutoBomb.SetTextOffsetRel(x_offset_1, x_offset_2);
            mLockTimeBar.SetTextOffsetRel(x_offset_1, x_offset_2);
            mLockTimeBoss.SetTextOffsetRel(x_offset_1, x_offset_2);
            mElBgm.SetTextOffsetRel(x_offset_1, x_offset_2);
        }
        virtual void OnContentUpdate() override
        {
            mMuteki();
            mFreeMiss();
            mFreeBomb();
            mAutoBomb();
            mLockTimeBar();
            mLockTimeBoss();
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

        Gui::GuiHotKeyChord mMenu { "ModMenuToggle", "BACKSPACE", Gui::GetBackspaceMenuChord() };

        HOTKEY_DEFINE(mMuteki, TH_MUTEKI, "F1", VK_F1)
        PATCH_HK(0x426eb5, "01"),
        PATCH_HK(0x425cfa, "eb"),
        PATCH_HK(0x426f19, "83c4089090")
        HOTKEY_ENDDEF();

        HOTKEY_DEFINE(mFreeMiss, ALCOSTG_FREE_MISS, "F2", VK_F2)
        PATCH_HK(0x42722c, "83c4049090"),
        PATCH_HK(0x426c2b, "eb60")
        HOTKEY_ENDDEF();
        
        HOTKEY_DEFINE(mFreeBomb, ALCOSTG_FREE_BOMB, "F3", VK_F3)
        PATCH_HK(0x427310, "c3")
        HOTKEY_ENDDEF();
        
        HOTKEY_DEFINE(mAutoBomb, TH_AUTOBOMB, "F4", VK_F4)
        PATCH_HK(0x425dee, "c6")
        HOTKEY_ENDDEF();
        
        HOTKEY_DEFINE(mLockTimeBar, ALCOSTG_LOCK_TIME_BAR, "F5", VK_F5)
        PATCH_HK(0x419510, "c3")
        HOTKEY_ENDDEF();
        
        HOTKEY_DEFINE(mLockTimeBoss, ALCOSTG_LOCK_TIME_BOSS, "F6", VK_F6)
        PATCH_HK(0x4094b9, "eb"),
        PATCH_HK(0x40ed74, "90")
        HOTKEY_ENDDEF();

    public:
        Gui::GuiHotKey mElBgm { TH_EL_BGM, "F7", VK_F7 };
    };

    class THAdvOptWnd : public Gui::GameGuiWnd {
        // Option Related Functions
    private:
        void FpsInit()
        {
            mOptCtx.vpatch_base = (int32_t)GetModuleHandleW(L"vpatch_alcostg.dll");
            if (mOptCtx.vpatch_base) {
                uint64_t hash[2];
                CalcFileHash(L"vpatch_alcostg.dll", hash);
                if (hash[0] != 6266639508503889982ll || hash[1] != 5871049704103251000ll)
                    mOptCtx.fps_status = -1;
                else if (*(int32_t*)(mOptCtx.vpatch_base + 0x1b024) == 0) {
                    mOptCtx.fps_status = 2;
                    mOptCtx.fps = *(int32_t*)(mOptCtx.vpatch_base + 0x18ad4);
                }
            } else
                mOptCtx.fps_status = 0;
        }
        void FpsSet()
        {
            if (mOptCtx.fps_status == 1) {
                mOptCtx.fps_dbl = 1.0 / (double)mOptCtx.fps;
            } else if (mOptCtx.fps_status == 2) {
                *(int32_t*)(mOptCtx.vpatch_base + 0x18ad4) = mOptCtx.fps;
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
            SetTitle(S(TH_SPELL_PRAC));
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
            ImGui::TextUnformatted(S(TH_ADV_OPT));
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
    bool UpdateAdvOptWindow()
    {
        static THAdvOptWnd* advOptWnd = nullptr;
        if (!advOptWnd)
            advOptWnd = new THAdvOptWnd();
        if (Gui::GetChordPressed(Gui::GetAdvancedMenuChord())) {
            if (advOptWnd->IsOpen())
                advOptWnd->Close();
            else
                advOptWnd->Open();
        }
        advOptWnd->Update();

        return advOptWnd->IsOpen();
    }

    void AlcostgSt3Std()
    {
        VFile std;
        std.SetFile((void*)GetMemContent(0x474048, 0x10), 0x9999);

        std << pair{0x0618, 0};
        std << pair{0x0624, 0};
        std << pair{0x0640, 0};
        std << pair{0x065c, 0};
        std << pair{0x0670, 0};
        std << pair{0x068c, 0};
        std << pair{0x069c, 0};
        std << pair{0x06b0, 0};
        std << pair{0x06bc, 0};
        std << pair{0x06d0, 0};
        std << pair{0x06e4, 128};
        std << pair{0x06f0, 0};
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
    __declspec(noinline) void AlcostgSt1Mid(ECLHelper& ecl)
    {
        size_t jmp1 = 0;
        size_t jmp2 = 0;
        size_t jmp3 = 0;

        ECLJump(ecl, 0x4910, 0x49ac, 90);
        switch (thPracParam.progress) {
        case 1:
            break;
        case 2:
            jmp1 = 0x4a1c;
            break;
        case 3:
            jmp1 = 0x4a1c;
            jmp2 = 0x4a8c;
            break;
        case 4:
            jmp1 = 0x4a1c;
            jmp2 = 0x4ad4;
            break;
        case 5:
            jmp1 = 0x4a1c;
            jmp2 = 0x4b1c;
            break;
        case 6:
            jmp1 = 0x4a1c;
            jmp2 = 0x4b64;
            break;
        case 7:
            jmp1 = 0x4a1c;
            jmp2 = 0x4bac;
            break;
        case 8:
            jmp1 = 0x4a1c;
            jmp2 = 0x4bf4;
            break;
        case 9:
            jmp1 = 0x4a1c;
            jmp2 = 0x4c3c;
            break;
        case 10:
            jmp1 = 0x4a1c;
            jmp2 = 0x4c84;
            break;
        case 11:
            jmp1 = 0x4a1c;
            jmp2 = 0x4ccc;
            break;
        case 12:
            jmp1 = 0x4a1c;
            jmp2 = 0x4d14;
            break;
        case 13:
            jmp1 = 0x4a1c;
            jmp2 = 0x4d5c;
            break;
        case 14:
            jmp1 = 0x4a1c;
            jmp2 = 0x4d5c;
            jmp3 = 0x4dcc;
            break;
        case 15:
            jmp1 = 0x4a1c;
            jmp2 = 0x4d5c;
            jmp3 = 0x4e14;
            break;
        case 16:
            jmp1 = 0x4a1c;
            jmp2 = 0x4d5c;
            jmp3 = 0x4e5c;
            break;
        case 17:
            jmp1 = 0x4a1c;
            jmp2 = 0x4d5c;
            jmp3 = 0x4ea4;
            break;
        case 18:
            jmp1 = 0x4a1c;
            jmp2 = 0x4d5c;
            jmp3 = 0x4eec;
            break;
        case 19:
            jmp1 = 0x4a1c;
            jmp2 = 0x4d5c;
            jmp3 = 0x4f34;
            break;
        default:
            break;
        }

        if (jmp1)
            ECLJump(ecl, 0x49d4, jmp1, 90);
        if (jmp2)
            ECLJump(ecl, 0x4a44, jmp2, 90);
        if (jmp3)
            ECLJump(ecl, 0x4d84, jmp3, 90);
    }
    __declspec(noinline) void AlcostgSt2Mid(ECLHelper& ecl)
    {
        size_t jmp1 = 0;

        ECLJump(ecl, 0x7240, 0x72dc, 90);
        switch (thPracParam.progress) {
        case 1:
            break;
        case 2:
            jmp1 = 0x7324;
            break;
        case 3:
            jmp1 = 0x736c;
            break;
        case 4:
            jmp1 = 0x73b4;
            break;
        case 5:
            jmp1 = 0x73fc;
            break;
        case 6:
            jmp1 = 0x7444;
            break;
        case 7:
            jmp1 = 0x748c;
            break;
        case 8:
            jmp1 = 0x74d4;
            break;
        case 9:
            jmp1 = 0x751c;
            break;
        case 10:
            jmp1 = 0x7578;
            break;
        case 11:
            jmp1 = 0x75c0;
            break;
        default:
            break;
        }

        if (jmp1)
            ECLJump(ecl, 0x72dc, jmp1, 90);
    }
    __declspec(noinline) void AlcostgSt3Mid(ECLHelper& ecl)
    {
        size_t jmp1 = 0;

        ECLJump(ecl, 0x7ca0, 0x7d4c, 120);
        switch (thPracParam.progress) {
        case 1:
            break;
        case 2:
            jmp1 = 0x7da8;
            break;
        case 3:
            jmp1 = 0x7e04;
            break;
        case 4:
            jmp1 = 0x7e60;
            break;
        case 5:
            jmp1 = 0x7ea8;
            break;
        case 6:
            jmp1 = 0x7f04;
            break;
        case 7:
            jmp1 = 0x7f60;
            break;
        case 8:
            jmp1 = 0x7fa8;
            break;
        case 9:
            jmp1 = 0x7ff0;
            break;
        case 10:
            jmp1 = 0x804c;
            break;
        case 11:
            jmp1 = 0x8094;
            break;
        default:
            break;
        }

        if (jmp1)
            ECLJump(ecl, 0x7d4c, jmp1, 120);
    }
    __declspec(noinline) void THPatch(ECLHelper& ecl, th_sections_t section)
    {
        if (thPracParam.progress) {
            switch (thPracParam.stage) {
            case 0:
                AlcostgSt1Mid(ecl);
                break;
            case 1:
                AlcostgSt2Mid(ecl);
                break;
            case 2:
                AlcostgSt3Mid(ecl);
                break;
            default:
                break;
            }
        } else {
            auto st1_boss = [&]() {
                ECLJump(ecl, 0xa094, 0xa0d8, 60);
                ecl << pair{0x5cc, 60} << pair{0x5b8, 120};
                ECLTimeFix(ecl, 0x604, -180);
            };
            auto st2_boss = [&]() {
                ECLJump(ecl, 0xcb84, 0xcbb4, 60);
                ECLJump(ecl, 0x5d4, 0x5f4, 0);
                ECLJump(ecl, 0x61c, 0x65c, 80);
                ECLTimeFix(ecl, 0x6c0, -30);
                ecl << pair{0x684, 60} << pair{0x5d0, 120};
            };
            auto st3_boss = [&]() {
                ECLJump(ecl, 0xe9a4, 0xe9d4, 60);
                ecl.SetPos(0x6a4);
                ecl << 0 << 0x00200119 << 0x04ff0000 << 0 << 60 << 4 << 0.0f << 162.25f; // 0x4d726c
                ECLJump(ecl, ecl.GetPos(), 0x810, 560);
                ecl << pair{0x664, 60} << pair{0x894, 620} << pair{0x898, (int16_t)0};
                AlcostgSt3Std();
            };

            switch (section) {
            case THPrac::Alcostg::ALCOSTG_ST1_BOSS1:
                st1_boss();
                break;
            case THPrac::Alcostg::ALCOSTG_ST1_BOSS2:
                st1_boss();
                ecl << pair{0x5ec, 2000};
                ECLTimeFix(ecl, 0x78c, -60, 4);
                ecl << pair{0x77c, (int16_t)0};
                break;
            case THPrac::Alcostg::ALCOSTG_ST1_BOSS3:
                st1_boss();
                ECLJump(ecl, 0x6e0, 0x78c, 60);
                ECLJump(ecl, 0x7e4, 0x178c, 0);
                ECLJump(ecl, 0x1908, 0x19ec, 60);
                ecl << pair{0x180c, 60};
                break;
            case THPrac::Alcostg::ALCOSTG_ST1_BOSS4:
                THPatch(ecl, ALCOSTG_ST1_BOSS3);
                ecl << pair{0x17b0, 2700};
                break;
            case THPrac::Alcostg::ALCOSTG_ST2_BOSS1:
                st2_boss();
                break;
            case THPrac::Alcostg::ALCOSTG_ST2_BOSS2:
                st2_boss();
                ecl << pair{0x604, 3000};
                ECLTimeFix(ecl, 0x828, -60, 4);
                ecl << pair{0x818, (int16_t)0};
                break;
            case THPrac::Alcostg::ALCOSTG_ST2_BOSS3:
                st2_boss();
                ECLJump(ecl, 0x77c, 0x828, 60);
                ECLJump(ecl, 0x880, 0x200c, 0);
                ECLJump(ecl, 0x21a4, 0x2288, 60);
                ecl << pair{0x20bc, 60};
                break;
            case THPrac::Alcostg::ALCOSTG_ST2_BOSS4:
                THPatch(ecl, ALCOSTG_ST2_BOSS3);
                ecl << pair{0x2030, 5700};
                break;
            case THPrac::Alcostg::ALCOSTG_ST2_BOSS5:
                THPatch(ecl, ALCOSTG_ST2_BOSS3);
                ecl << pair{0x205c, 0x33};
                ecl << pair{0x2030, 3000} << pair{0x2048, 3000};
                ECLJump(ecl, 0x5170, 0x51a4, 60);
                ecl << pair{0x51f8, 60};
                break;
            case THPrac::Alcostg::ALCOSTG_ST3_BOSS1:
                st3_boss();
                break;
            case THPrac::Alcostg::ALCOSTG_ST3_BOSS2:
                st3_boss();
                ecl << pair{0x678, 3500};
                break;
            case THPrac::Alcostg::ALCOSTG_ST3_BOSS3:
                st3_boss();
                ECLJump(ecl, 0x998, 0x11c8, 0);
                ECLJump(ecl, 0x1380, 0x1468, 120);
                ecl << pair{0x1248, 0} << pair{0x135c, (int16_t)0};
                break;
            case THPrac::Alcostg::ALCOSTG_ST3_BOSS4:
                THPatch(ecl, ALCOSTG_ST3_BOSS3);
                ecl << pair{0x11ec, 2000};
                break;
            case THPrac::Alcostg::ALCOSTG_ST3_BOSS5:
                st3_boss();
                ECLJump(ecl, 0x998, 0x1d24, 0);
                ECLJump(ecl, 0x1f30, 0x2000, 80);
                ecl << pair{0x1da4, 40} << pair{0x1f0c, (int16_t)0};
                break;
            case THPrac::Alcostg::ALCOSTG_ST3_BOSS6:
                THPatch(ecl, ALCOSTG_ST3_BOSS5);
                ecl << pair{0x1d48, 5000};
                ecl << pair{0x204c, 0} << pair{0x2060, 0};
                break;
            case THPrac::Alcostg::ALCOSTG_ST3_BOSS7:
                st3_boss();
                ECLJump(ecl, 0x998, 0x24f8, 0);
                ECLJump(ecl, 0x2714, 0x27e4, 80);
                ecl << pair{0x2588, 40} << pair{0x26f0, (int16_t)0};
                break;
            case THPrac::Alcostg::ALCOSTG_ST3_BOSS8:
                THPatch(ecl, ALCOSTG_ST3_BOSS7);
                ecl << pair{0x251c, 3600};
                ecl << pair{0x2854, 0} << pair{0x2868, 0};
                break;
            case THPrac::Alcostg::ALCOSTG_ST3_BOSS9:
                THPatch(ecl, ALCOSTG_ST3_BOSS7);
                ecl << pair{0x2548, 0x35};
                ecl << pair{0x251c, 3600};
                ecl << pair{0x2854, 0} << pair{0x2868, 0};
                ECLJump(ecl, 0x54e0, 0x5500, 140);
                ecl << pair{0x53fc, 60} << pair{0x5404, (int16_t)0} << pair{0x5480, (int16_t)0};
                break;
            default:
                break;
            }
        }
    }

    int THBGMTest()
    {
        if (thLock)
            return 0;
        if (!thPracParam.mode)
            return 0;
        return th_sections_bgm[thPracParam.section];
    }
    void THSaveReplay(char* repName)
    {
        ReplaySaveParam(mb_to_utf16(repName, 932).c_str(), thPracParam.GetJson());
    }
    void THTrackerUpdate()
    {
        ImGui::SetNextWindowSize({ 120.0f, 0.0f });
        ImGui::SetNextWindowPos({ 517.0f, 150.0f });
        ImGui::Begin("Tracker", nullptr,
            ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav);

        ImGui::BeginTable("Tracker table", 2);
        ImGui::TableNextRow();

        ImGui::TableNextColumn();
        ImGui::TextUnformatted("Miss");
        ImGui::TableNextColumn();
        ImGui::Text("%d", tracker_info.th10.misses);

        ImGui::TableNextRow();

        ImGui::TableNextColumn();
        ImGui::TextUnformatted("Bomb");
        ImGui::TableNextColumn();
        ImGui::Text("%d", tracker_info.th10.bombs);

        ImGui::EndTable();

        ImGui::End();
    }
    
    HOOKSET_DEFINE(THMainHook)
    { .addr = 0x418402, .name = "alcostg_game_start", .callback = tracker_reset, .data = PatchHookImpl(6) },
    { .addr = 0x425e1d, .name = "alcostg_bomb",       .callback = th10_tracker_count_bomb, .data = PatchHookImpl(5) },
    { .addr = 0x425cb6, .name = "alcostg_bomb2",      .callback = th10_tracker_count_bomb, .data = PatchHookImpl(5) },
    { .addr = 0x427231, .name = "alcostg_miss",       .callback = th10_tracker_count_miss, .data = PatchHookImpl(6) },
    EHOOK_DY(alcostg_on_restart, 0x4187a8, 6, {
        thRestart = true;
        thLock = thHardLock;
    })
    EHOOK_DY(alcostg_everlasting_bgm, 0x43a580, 1, {
        int32_t retn_addr = ((int32_t*)pCtx->Esp)[0];
        int32_t bgm_cmd = ((int32_t*)pCtx->Esp)[1];
        int32_t bgm_id = ((int32_t*)pCtx->Esp)[2];
        int32_t call_addr = ((int32_t*)pCtx->Esp)[3];

        bool el_switch;
        bool is_practice;
        bool result;

        el_switch = *(THOverlay::singleton().mElBgm) && !THGuiRep::singleton().mRepStatus && thPracParam.mode && thPracParam.section;
        if (thLock)
            el_switch = false;

        is_practice = false;
        if (thRestart) {
            is_practice = true;
            thRestart = false;
        }

        if (retn_addr == 0x42093f && call_addr == 0x412bf6) {
            thLock = true;
            result = ElBgmTest<0x420802, 0x418b7d, 0x422b44, 0x422bc1, 0x418e91>(
                el_switch, is_practice, 0x420802, 2, 2, call_addr);
        } else {
            result = ElBgmTest<0x420802, 0x418b7d, 0x422b44, 0x422bc1, 0x418e91>(
                el_switch, is_practice, retn_addr, bgm_cmd, bgm_id, call_addr);
        }

        if (result) {
            pCtx->Eip = 0x43a5e5;
        }
    })
    EHOOK_DY(alcostg_param_reset, 0x42c96b, 6, {
        thPracParam.Reset();
        AlcostgBeer::Reset();
        thLock = false;
        thHardLock = false;
        thRestart = false;
    })
    EHOOK_DY(alcostg_prac_menu_1, 0x42cb0c, 5, {
        if (THGuiPrac::singleton().State()) {
        } else {
            pCtx->Eip = 0x42cc50;
        }
    })
    EHOOK_DY(alcostg_prac_menu_2, 0x42ca20, 6, {
        if (THGuiPrac::singleton().mState) {
            pCtx->Eip = 0x42cb0c;
        }
    })
    EHOOK_DY(alcostg_prac_menu_enter, 0x42cbbd, 5, {
        if (thPracParam.mode == 1) {
            pCtx->Edx = thPracParam.stage + 1;
        }
    })
    EHOOK_DY(alcostg_patch_main, 0x4186ff, 3, {
        AlcostgBeer::Reset();
        if (!thLock && thPracParam.mode == 1) {
            if (thPracParam.stage == 0) {
                *(int32_t*)(0x48e5d0) = *(int32_t*)(0x48e5d4) = (thPracParam.time * 30);
                *(float*)(0x48e5d8) = (float)(thPracParam.time * 30);
            } else if (thPracParam.stage == 1) {
                *(int32_t*)(0x48e5d0) = *(int32_t*)(0x48e5d4) = (thPracParam.time * 60);
                *(float*)(0x48e5d8) = (float)(thPracParam.time * 60);
            } else if (thPracParam.stage == 2) {
                *(int32_t*)(0x48e5d0) = *(int32_t*)(0x48e5d4) = (thPracParam.time * 46);
                *(float*)(0x48e5d8) = (float)(thPracParam.time * 46);
            }
            *(int16_t*)(0x48e60a) = thPracParam.beer_max * 100;
            AlcostgBeer::Set(thPracParam.beer * 100);
            *(int32_t*)(0x48e584) = *(int32_t*)(0x48e58c) = (int32_t)(thPracParam.score);

            ECLHelper ecl;
            ecl.SetBaseAddr((void*)GetMemAddr(0x474064, 0x60, 0xC));
            THPatch(ecl, (th_sections_t)thPracParam.section);
        }
    })
    EHOOK_DY(alcostg_logo, 0x414de7, 7, {
        if (!thLock && thPracParam.mode == 1 && (thPracParam.section || thPracParam.progress)) {
            pCtx->Eip = 0x414e3b;
        }
    })
    EHOOK_DY(alcostg_bgm, 0x418e8a, 2, {
        if (THBGMTest()) {
            PushHelper32(pCtx, 1);
            pCtx->Eip = 0x418e8c;
        }
    })
    EHOOK_DY(alcostg_rep_save, 0x429d8b, 6, {
        char* repName = (char*)(pCtx->Esp + 0x1c);
        if (thPracParam.mode)
            THSaveReplay(repName);
    })
    EHOOK_DY(alcostg_rep_menu_1, 0x42f081, 3, {
        THGuiRep::singleton().State(1);
    })
    EHOOK_DY(alcostg_rep_menu_2, 0x42f13c, 5, {
        THGuiRep::singleton().State(2);
    })
    EHOOK_DY(alcostg_rep_menu_3, 0x42f287, 6, {
        THGuiRep::singleton().State(3);
    })
    EHOOK_DY(alcostg_rep_menu_enter, 0x42f2de, 1, {
        int stage = pCtx->Edx;
        if (stage != thPracParam.stage)
            thHardLock = thLock = true;
    })
    EHOOK_DY(alcostg_update, 0x445eee, 3, {
        GameGuiBegin(IMPL_WIN32_DX9);

        // Gui components update
        THGuiPrac::singleton().Update();
        THOverlay::singleton().Update();

        if (tracker_open && GetMemContent(PLAYER_PTR)) {
            THTrackerUpdate();
        }

        GameGuiEnd(UpdateAdvOptWindow() || THGuiPrac::singleton().IsOpen());
    })
    EHOOK_DY(alcostg_render, 0x43564a, 5, {
        GameGuiRender(IMPL_WIN32_DX9);
    })
    HOOKSET_ENDDEF()

    static __declspec(noinline) void THGuiCreate()
    {
        if (ImGui::GetCurrentContext()) {
            return;
        }
        // Init
        GameGuiInit(IMPL_WIN32_DX9, 0x48e648, 0x48ef20,
            Gui::INGAGME_INPUT_GEN2, 0x471514, 0x471510, 0,
            -1);

        // Gui components creation
        THGuiPrac::singleton();
        THGuiRep::singleton();
        THOverlay::singleton();

        // Hooks
        EnableAllHooks(THMainHook);
        alcostg_add_beer.Setup();

        // Reset thPracParam
        thPracParam.Reset();
    }

    HOOKSET_DEFINE(THInitHook)
    PATCH_DY(alcostg_startup_1, 0x42c281, "eb")
    EHOOK_DY(alcostg_gui_init_1, 0x42ca17, 3, {
        THGuiCreate();
        self->Disable();
    })
    EHOOK_DY(alcostg_gui_init_2, 0x435e66, 1, {
        THGuiCreate();
        self->Disable();
    })
    HOOKSET_ENDDEF()
}

void AlcostgInit()
{
    EnableAllHooks(Alcostg::THInitHook);
}
}
