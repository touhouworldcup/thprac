#include "thprac_utils.h"

namespace THPrac {
namespace TH11 {
    using std::pair;
    struct THPracParam {
        int32_t mode;
        int32_t stage;
        int32_t section;
        int32_t phase;
        int32_t life;
        int32_t life_fragment;
        int32_t power;
        int32_t graze;
        int32_t signal;
        int32_t value;
        int64_t score;

        bool dlg;

        bool _playLock = false;
        void Reset()
        {
            memset(this, 0, sizeof(THPracParam));
        }
        bool ReadJson(std::string& json)
        {
            ParseJson();

            ForceJsonValue(game, "th11");
            GetJsonValue(mode);
            GetJsonValue(stage);
            GetJsonValue(section);
            GetJsonValue(phase);
            GetJsonValueEx(dlg, Bool);
            GetJsonValue(life);
            GetJsonValue(life_fragment);
            GetJsonValue(power);
            GetJsonValue(graze);
            GetJsonValue(signal);
            GetJsonValue(value);
            GetJsonValue(score);

            return true;
        }
        std::string GetJson()
        {
            CreateJson();

            AddJsonValueEx(version, GetVersionStr(), jalloc);
            AddJsonValueEx(game, "th11", jalloc);
            AddJsonValue(mode);
            AddJsonValue(stage);
            if (section)
                AddJsonValue(section);
            if (phase)
                AddJsonValue(phase);
            if (dlg)
                AddJsonValue(dlg);

            AddJsonValue(life);
            AddJsonValue(life_fragment);
            AddJsonValue(power);
            AddJsonValue(graze);
            AddJsonValue(signal);
            AddJsonValue(value);
            AddJsonValue(score);

            ReturnJson();
        }
    };
    THPracParam thPracParam {};

    class THGuiPrac : public Gui::GameGuiWnd {
        THGuiPrac() noexcept
        {
            *mLife = 9;
            *mPower = 80;
            *mMode = 1;
            *mValue = 50000;

            SetFade(0.8f, 0.1f);
            SetStyle(ImGuiStyleVar_WindowRounding, 0.0f);
            SetStyle(ImGuiStyleVar_WindowBorderSize, 0.0f);
            OnLocaleChange();
        }
        SINGLETON(THGuiPrac);
    public:

        __declspec(noinline) void State(int state)
        {
            uint32_t mainShot;
            uint32_t subShot;

            switch (state) {
            case 0:
                break;
            case 1:
                mDiffculty = *((int32_t*)0x4a5720);
                mainShot = GetMemContent(0x4a8ecc, 0x38);
                subShot = GetMemContent(0x4a8ecc, 0x3c);
                if (mainShot == 1 && subShot == 0) {
                    if (!isMarisaA) {
                        mPower.SetBound(0, 96);
                        *mPower = 96;
                    }
                    isMarisaA = true;
                } else {
                    if (isMarisaA) {
                        mPower.SetBound(0, 80);
                        *mPower = 80;
                    }
                    isMarisaA = false;
                }

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
                thPracParam.phase = *mPhase;
                if (SectionHasDlg(thPracParam.section))
                    thPracParam.dlg = *mDlg;
                thPracParam.life = *mLife;
                thPracParam.life_fragment = *mLifeFragment;
                thPracParam.power = *mPower;
                thPracParam.graze = *mGraze;
                thPracParam.signal = 0;
                thPracParam.value = *mValue;
                thPracParam.score = *mScore;
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
                SetSize(320.f, 335.f);
                SetPos(150.f, 80.f);
                SetItemWidth(-56.0f);
                break;
            case Gui::LOCALE_EN_US:
                SetSize(440.f, 325.f);
                SetPos(100.f, 90.f);
                SetItemWidth(-58.0f);
                break;
            case Gui::LOCALE_JA_JP:
                SetSize(340.f, 335.f);
                SetPos(130.f, 80.f);
                SetItemWidth(-66.0f);
                break;
            default:
                break;
            }
        }
        virtual void OnContentUpdate() override
        {
            ImGui::TextUnformatted(XSTR(TH_MENU));
            ImGui::Separator();

            PracticeMenu();
        }
        const th_glossary_t* SpellPhase()
        {
            auto section = CalcSection();
            if (section == TH11_ST6_BOSS9) {
                return TH11_SPELL_5PHASE;
            }
            if (section == TH11_ST7_END_S10) {
                return TH_SPELL_PHASE2;
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
                if (isMarisaA) {
                    auto power_str = std::to_string((float)(*mPower) * 8.0f / 96.0f).substr(0, 4);
                    mPower(power_str.c_str());
                } else {
                    auto power_str = std::to_string((float)(*mPower) * 5.0f / 100.0f).substr(0, 4);
                    mPower(power_str.c_str());
                }
                mGraze();
                mValue();
                mValue.RoundDown(10);
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
            case TH11_ST1_BOSS1:
            case TH11_ST2_BOSS1:
            case TH11_ST3_BOSS1:
            case TH11_ST4_BOSS1:
            case TH11_ST5_BOSS1:
            case TH11_ST6_BOSS1:
            case TH11_ST6_MID1:
            case TH11_ST7_END_NS1:
            case TH11_ST7_MID1:
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

        // Data
        Gui::GuiCombo mMode { TH_MODE, TH_MODE_SELECT };
        Gui::GuiCombo mStage { TH_STAGE, TH_STAGE_SELECT };
        Gui::GuiCombo mWarp { TH_WARP, TH_WARP_SELECT };
        Gui::GuiCombo mSection { TH_MODE };
        Gui::GuiCombo mPhase { TH_PHASE };
        Gui::GuiCheckBox mDlg { TH_DLG };

        Gui::GuiSlider<int, ImGuiDataType_S32> mChapter { TH_CHAPTER, 0, 0 };
        Gui::GuiSlider<int, ImGuiDataType_S32> mLife { TH_LIFE, 0, 9 };
        Gui::GuiSlider<int, ImGuiDataType_S32> mLifeFragment { TH_LIFE_FRAGMENT, 0, 4 };
        Gui::GuiDrag<int, ImGuiDataType_S32> mGraze { TH_GRAZE, 0, 999999, 1, 100000 };
        Gui::GuiSlider<int, ImGuiDataType_S32> mPower { TH_POWER, 0, 80 };
        Gui::GuiDrag<int64_t, ImGuiDataType_S64> mScore { TH_SCORE, 0, 9999999990, 10, 100000000 };
        Gui::GuiDrag<int, ImGuiDataType_S32> mValue { TH_FAITH, 0, 999990, 10, 100000 };

        Gui::GuiNavFocus mNavFocus { TH_STAGE, TH_MODE, TH_WARP, TH_DLG,
            TH_MID_STAGE, TH_END_STAGE, TH_NONSPELL, TH_SPELL, TH_PHASE, TH_CHAPTER,
            TH_LIFE, TH_FAITH, TH_SCORE, TH_POWER, TH_GRAZE };

        int mChapterSetup[7][2] {
            { 2, 2 },
            { 2, 2 },
            { 2, 1 },
            { 3, 4 },
            { 3, 3 },
            { 3, 3 },
            { 4, 3 },
        };

        float mStep = 10.0;
        int mDiffculty = 0;
        bool isMarisaA = false;
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
                }
            }
        }

        Gui::GuiHotKey mMenu { "ModMenuToggle", "BACKSPACE", VK_BACK };
        Gui::GuiHotKey mMuteki { TH_MUTEKI, "F1", VK_F1, {
            new HookCtx(0x432AA4, "\x01", 1),
            new HookCtx(0x431205, "\xeb", 1),
            new HookCtx(0x432ae7, "\x83\xc4\x0c\x90\x90", 5) } };
        Gui::GuiHotKey mInfLives { TH_INFLIVES, "F2", VK_F2, {
            new HookCtx(0x4327EC, "\x90", 1) } };
        Gui::GuiHotKey mInfPower { TH_INFPOWER, "F3", VK_F3, {
            new HookCtx(0x4311EB, "\xeb\x0a", 2),
            new HookCtx(0x431298, "\xeb\x09", 2),
            new HookCtx(0x4312E0, "\x0f\x1f\x44\x00", 4) } };
        Gui::GuiHotKey mTimeLock { TH_TIMELOCK, "F4", VK_F4, {
            new HookCtx(0x40C0DD, "\xeb", 1),
            new HookCtx(0x41278C, "\x90", 1) } };
        Gui::GuiHotKey mAutoBomb { TH_AUTOBOMB, "F5", VK_F5, {
            new HookCtx(0x431279, "\xc6", 1) } };

    public:
        Gui::GuiHotKey mElBgm { TH_EL_BGM, "F6", VK_F6 };
    };

    class THAdvOptWnd : public Gui::PPGuiWnd {
        EHOOK_ST(th11_all_clear_bonus_1, 0x41eb9a)
        {
            pCtx->Eip = 0x41ebcc;
        }
        EHOOK_ST(th11_all_clear_bonus_2, 0x41eca6)
        {
            if (GetMemContent(0x4a5758) & 0x10) {
                pCtx->Eip = 0x41eba3;
            } else if (GetMemContent(0x4a8e88, 0x74) && GetMemContent(0x4a8eb8, 0x18, 0xa) & 1) {
                pCtx->Eip = 0x41ebc2;
            }
        }
        EHOOK_ST(th11_all_clear_bonus_3, 0x41ed4a)
        {
            if (GetMemContent(0x4a5758) & 0x10) {
                pCtx->Eip = 0x41eba3;
            } else if (GetMemContent(0x4a8e88, 0x74) && GetMemContent(0x4a8eb8, 0x18, 0xa) & 1) {
                pCtx->Eip = 0x41ebc2;
            }
        }
    private:
        void FpsInit()
        {
            mOptCtx.vpatch_base = (int32_t)GetModuleHandleW(L"vpatch_th11.dll");
            if (mOptCtx.vpatch_base) {
                uint64_t hash[2];
                CalcFileHash(L"vpatch_th11.dll", hash);
                if (hash[0] != 5913416708557704950ll || hash[1] != 10824003281749047314ll)
                    mOptCtx.fps_status = -1;
                else if (*(int32_t*)(mOptCtx.vpatch_base + 0x1b024) == 0) {
                    mOptCtx.fps_status = 2;
                    mOptCtx.fps = *(int32_t*)(mOptCtx.vpatch_base + 0x1b034);
                }
            } else if (*(uint8_t*)0x4c346b == 3) {
                mOptCtx.fps_status = 1;

                DWORD oldProtect;
                VirtualProtect((void*)0x44647e, 4, PAGE_EXECUTE_READWRITE, &oldProtect);
                *(double**)0x44647e = &mOptCtx.fps_dbl;
                VirtualProtect((void*)0x44647e, 4, oldProtect, &oldProtect);
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
            th11_all_clear_bonus_1.Setup();
            th11_all_clear_bonus_2.Setup();
            th11_all_clear_bonus_3.Setup();
        }
        void GameplaySet()
        {
            th11_all_clear_bonus_1.Toggle(mOptCtx.all_clear_bonus);
            th11_all_clear_bonus_2.Toggle(mOptCtx.all_clear_bonus);
            th11_all_clear_bonus_3.Toggle(mOptCtx.all_clear_bonus);
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
            ImGui::TextUnformatted(XSTR(TH_ADV_OPT));
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

    void* THStage6STD()
    {
        void* buffer = (void*)GetMemContent(0x4a8d60, 0x10);

        VFile std;
        std.SetFile(buffer, 999999);
        std.SetPos(0x328);
        std << 0 << 0x0010000e << 0 << 2
            << 0 << 0x00140008 << 0xff000000 << 0x44480000 << 0x44960000
            << 0 << 0x000c0011 << 2
            << 0 << 0x00140002 << 0 << 0xc4e04000 << 0xc4960000
            << 0x8000 << 0x00100001 << 0x234 << 0
            << 0xffffffff << 0xffffffff << 0xffffffff << 0xffffffff << 0xffffffff;

        return nullptr;
    }
    void* THStage6ANM()
    {
        void* buffer = (void*)GetMemContent(0x4a8d60, 0x178, 0x108);

        VFile anm;
        anm.SetFile(buffer, 999999);
        anm.SetPos(0x602ac);
        anm << 0x3f800000 << 0x3f800000;
        anm.SetPos(0x602c4);
        anm << 0x3f800000 << 0x3f800000;

        return nullptr;
    }

    void ECLJump(ECLHelper& ecl, unsigned int start, unsigned int dest, int delay = 0, int ecl_time = 0)
    {
        ecl.SetPos(start);
        ecl << ecl_time << 0x0018000C << 0x02ff0000 << 0x00000000 << dest - start << delay;
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
    void ECLSetHealth(ECLHelper& ecl, size_t pos, int32_t health, int32_t ecl_time = 0, bool stall = true)
    {
        ecl.SetPos(pos);
        ecl << ecl_time << 0x0014014b << 0x01ff0000 << 0 << health;
        if (stall)
            ecl << 9999 << 0x00100000 << 0x00ff0000 << 0;
    }
    template<typename... Args>
    void ECLVoid(ECLHelper& ecl, size_t pos, Args... rest)
    {
        ecl.SetPos(pos + 4);
        ecl << (int16_t)0;
        if constexpr (sizeof...(Args) != 0) {
            ECLVoid(ecl, rest...);
        }
    }
    void ECLSatoriJump(ECLHelper& ecl, int shot_type)
    {
        ECLJump(ecl, 0xf764, 0xfa04);
        ecl << pair{0x1108, 0x10} << pair{0x1110, 0x64726143} << pair{0x1114, 0x67734d32} << pair{0x1118, 0};
        ecl << pair{0x10fe, (int16_t)0x2c};
        ECLVoid(ecl, 0x5568, 0x55b4, 0x5704, 0x5618, 0x5664);
        switch (shot_type) {
        case 0:
            ECLJump(ecl, 0x5820, 0x5870);
            break;
        case 1:
            ECLJump(ecl, 0x5820, 0x58fc);
            break;
        case 2:
            ECLJump(ecl, 0x5820, 0x5988);
            break;
        case 3:
            ECLJump(ecl, 0x5820, 0x5a14);
            break;
        case 4:
            ECLJump(ecl, 0x5820, 0x5aa0);
            break;
        case 5:
            ECLJump(ecl, 0x5820, 0x5b2c);
            break;
        default:
            break;
        }
    }
    __declspec(noinline) void THStageWarp(ECLHelper& ecl, int stage, int portion)
    {
        if (stage == 1) {
            switch (portion) {
            case 1:
                break;
            case 2:
                ECLJump(ecl, 0x10a90, 0x10b28, 60, 90);
                break;
            case 3:
                ECLJump(ecl, 0x10a90, 0x10bf4, 60, 90);
                break;
            case 4:
                ECLJump(ecl, 0x10a90, 0x10c8c, 60, 90);
                break;
            default:
                break;
            }
        } else if (stage == 2) {
            switch (portion) {
            case 1:
                break;
            case 2:
                ECLJump(ecl, 0xf1b4, 0xf218, 160, 90);
                ecl << pair{0xb608, 0};
                break;
            case 3:
                ECLJump(ecl, 0xf1b4, 0xf2c4, 160, 90);
                break;
            case 4:
                ECLJump(ecl, 0xf1b4, 0xf308, 160, 90);
                break;
            default:
                break;
            }
        } else if (stage == 3) {
            switch (portion) {
            case 1:
                break;
            case 2:
                ECLJump(ecl, 0x14ae8, 0x14b18, 160, 90);
                ecl << pair{0xf370, 0};
                break;
            case 3:
                ECLJump(ecl, 0xb34c, 0xb3d8);
                ECLJump(ecl, 0xb458, 0xdf74, 66);
                ECLJump(ecl, 0xdfd0, 0xe124, 66);
                ECLJump(ecl, 0x14ae8, 0x14ba0, 160);
                break;
            default:
                break;
            }
        } else if (stage == 4) {
            switch (portion) {
            case 1:
                break;
            case 2:
                ecl << pair{0xf774, 90} << pair{0xf788, 0};
                ECLJump(ecl, 0xf78c, 0xf7f4);
                break;
            case 3:
                ecl << pair{0xf774, 90} << pair{0xf788, 0};
                ECLJump(ecl, 0xf78c, 0xf848);
                break;
            case 4:
                ecl << pair{0xf774, 90} << pair{0xf788, 0};
                ECLJump(ecl, 0xf78c, 0xf8d0);
                ecl << pair{0xa988, 0};
                break;
            case 5:
                ecl << pair{0xf774, 90} << pair{0xf788, 0};
                ECLJump(ecl, 0xf78c, 0xf8f0);
                break;
            case 6:
                ecl << pair{0xf774, 90} << pair{0xf788, 0};
                ECLJump(ecl, 0xf78c, 0xf978);
                break;
            case 7:
                ecl << pair{0xf774, 90} << pair{0xf788, 0};
                ECLJump(ecl, 0xf78c, 0xf9ac);
                break;
            default:
                break;
            }
        } else if (stage == 5) {
            switch (portion) {
            case 1:
                break;
            case 2:
                ecl << pair{0x6790, 0};
                ECLJump(ecl, 0x67a8, 0x67dc);
                ecl << pair{0x67a4, 30};
                break;
            case 3:
                ecl << pair{0x6790, 0};
                ECLJump(ecl, 0x67a8, 0x6810);
                ecl << pair{0x67a4, 30};
                break;
            case 4:
                ecl << pair{0x6790, 0};
                ECLJump(ecl, 0x67a8, 0x6878);
                break;
            case 5:
                ecl << pair{0x6790, 0};
                ECLJump(ecl, 0x67a8, 0x68ac);
                break;
            case 6:
                ecl << pair{0x6790, 0};
                ECLJump(ecl, 0x67a8, 0x68e0);
                break;
            default:
                break;
            }
        } else if (stage == 6) {
            switch (portion) {
            case 1:
                break;
            case 2:
                ecl << pair{0x40c8, 0};
                ECLJump(ecl, 0x40e0, 0x4114);
                break;
            case 3:
                ecl << pair{0x40c8, 0};
                ECLJump(ecl, 0x40e0, 0x4148);
                break;
            case 4:
                ecl << pair{0x40c8, 0};
                ECLJump(ecl, 0x40e0, 0x41f8);
                break;
            case 5:
                ecl << pair{0x40c8, 0};
                ECLJump(ecl, 0x40e0, 0x422c);
                break;
            case 6:
                ecl << pair{0x40c8, 0};
                ECLJump(ecl, 0x40e0, 0x4260);
                break;
            default:
                break;
            }
        } else if (stage == 7) {
            switch (portion) {
            case 1:
                break;
            case 2:
                ecl << pair{0x4c10, 90};
                ECLJump(ecl, 0x4c14, 0x4c48);
                break;
            case 3:
                ecl << pair{0x4c10, 90};
                ECLJump(ecl, 0x4c14, 0x4c7c);
                break;
            case 4:
                ecl << pair{0x4c10, 90};
                ECLJump(ecl, 0x4c14, 0x4ce4);
                break;
            case 5:
                ecl << pair{0x4c10, 90};
                ECLJump(ecl, 0x4c14, 0x4d7c);
                break;
            case 6:
                ecl << pair{0x4c10, 90};
                ECLJump(ecl, 0x4c14, 0x4db0);
                break;
            case 7:
                ecl << pair{0x4c10, 90};
                ECLJump(ecl, 0x4c14, 0x4de4);
                break;
            default:
                break;
            }
        }
    }
    __declspec(noinline) void THPatch(ECLHelper& ecl, th_sections_t section)
    {
        auto st3_boss = [&]() {
            ECLJump(ecl, 0x14ae8, 0x14ba0);
            ECLJump(ecl, 0x14bd0, 0x14c8c);
            ECLJump(ecl, 0xb404, 0xf44);
            ECLVoid(ecl, 0xb34c, 0xb374, 0xb3a0, 0xb3bc);
        };
        switch (section) {
        case THPrac::TH11::TH11_ST1_MID1:
            ECLJump(ecl, 0x10a90, 0x10bb0);
            ecl << pair{0x10bb0, 0};
            break;
        case THPrac::TH11::TH11_ST1_MID2:
            ECLJump(ecl, 0x10a90, 0x10bb0);
            ecl << pair{0x10bb0, 0}
                << pair{0x7214, 8600} << pair{0x7228, 5200};
            ECLVoid(ecl, 0x7714, 0x7818);
            break;
        case THPrac::TH11::TH11_ST1_MID3_EN:
        case THPrac::TH11::TH11_ST1_MID3_HL:
            ECLJump(ecl, 0x10a90, 0x10bb0);
            ECLJump(ecl, 0x10a90, 0x10bb0);
            ecl << pair{0x10bb0, 0}
                << pair{0x7214, 4300} << pair{0x7228, 900}
                << pair{0x72d0, 4300} << pair{0x7348, 900};
            ECLVoid(ecl, 0x7714, 0x7818);
            ECLVoid(ecl, 0x7714, 0x7e78, 0x8570, 0x7f54);
            break;
        case THPrac::TH11::TH11_ST1_BOSS1:
            if (thPracParam.dlg)
                ECLJump(ecl, 0x10a90, 0x10d68);
            else {
                ECLJump(ecl, 0x10a90, 0x10d8c);
                ecl << pair{0x10d8c, 0};
                ECLVoid(ecl, 0x9bc);
                *(uint32_t*)0x4a5730 = 24;
            }
            break;
        case THPrac::TH11::TH11_ST1_BOSS2:
            ECLJump(ecl, 0x10a90, 0x10d8c);
            ecl << pair{0x10d8c, 0} << pair{0x7dc, 1500};
            ECLVoid(ecl, 0x1fac);
            *(uint32_t*)0x4a5730 = 24;
            break;
        case THPrac::TH11::TH11_ST1_BOSS3:
            ECLJump(ecl, 0x10a90, 0x10d8c);
            ecl << pair{0x10d8c, 0} << pair{0x8e0, (int8_t)0x32};
            ECLTimeFix(ecl, 0x828, -60);
            ECLVoid(ecl, 0x838, 0x1584, 0x1594, 0x15f8);
            *(uint32_t*)0x4a5730 = 24;
            break;
        case THPrac::TH11::TH11_ST1_BOSS4:
            ECLJump(ecl, 0x10a90, 0x10d8c);
            ecl << pair{0x10d8c, 0} << pair{0x8e0, (int8_t)0x32};
            ECLVoid(ecl, 0x838, 0x1584, 0x1594, 0x15f8);
            ecl << pair{0x1298, 1760} << pair{0x2d04, (int16_t)0};
            *(uint32_t*)0x4a5730 = 24;
            break;
        case THPrac::TH11::TH11_ST2_MID1:
            ECLJump(ecl, 0xf1b4, 0xf280);
            ecl << pair{0xf280, 0};
            break;
        case THPrac::TH11::TH11_ST2_MID2:
            ECLJump(ecl, 0xf1b4, 0xf280);
            ecl << pair{0xf280, 0}
                << pair{0x815c, 1000} << pair{0x84ac, (int16_t)0};
            break;
        case THPrac::TH11::TH11_ST2_BOSS1:
            if (thPracParam.dlg)
                ECLJump(ecl, 0xf1b4, 0xf33c);
            else {
                ECLJump(ecl, 0xf1b4, 0xf360);
                ecl << pair{0xf360, 0};
            }
            break;
        case THPrac::TH11::TH11_ST2_BOSS2:
            ECLJump(ecl, 0xf1b4, 0xf360);
            ecl << pair{0xf360, 0}
                << pair{0xc80, 1500} << pair{0xc94, 1500} << pair{0x24c0, (int16_t)0};
            break;
        case THPrac::TH11::TH11_ST2_BOSS3:
            ECLJump(ecl, 0xf1b4, 0xf360);
            ecl << pair{0xf360, 0} << pair{0xd80, (int8_t)0x32};
            ECLTimeFix(ecl, 0xcf8, -60);
            ECLVoid(ecl, 0xd08, 0x1b10, 0x1b20);
            break;
        case THPrac::TH11::TH11_ST2_BOSS4:
            ECLJump(ecl, 0xf1b4, 0xf360);
            ecl << pair{0xf360, 0} << pair{0xd80, (int8_t)0x32} << pair{0x18b8, 3050};
            ECLVoid(ecl, 0xd08, 0x1b10, 0x1b20, 0x3eb4);
            break;
        case THPrac::TH11::TH11_ST2_BOSS5:
            ECLJump(ecl, 0xf1b4, 0xf360);
            ECLTimeFix(ecl, 0xcf8, -60);
            ecl << pair{0xf360, 0}
                << pair{0xd78, 12} << pair{0xd80, 0x64726143} << pair{0xd84, 0x33} // Str
                << pair{0xc80, 1500} << pair{0xc94, 1500};
            ECLVoid(ecl, 0xd08, 0x5538, 0x5548);
            break;
        case THPrac::TH11::TH11_ST3_MID1:
            ECLJump(ecl, 0x14ae8, 0x14ba0);
            ecl << pair{0x14ba0, 0};
            break;
        case THPrac::TH11::TH11_ST3_MID2:
            ECLJump(ecl, 0x14ae8, 0x14ba0);
            ecl << pair{0x14ba0, 0} << pair{0xb2fc, 9000};
            ecl.SetPos(0xb34c);
            ecl << 0 << 0x0028014f << 0x01ff0000 << 0 << 70; // Change Health Jump To Invicible
            ECLJump(ecl, 0xb458, 0xb6c4); // Jump From MBoss To MBoss2
            ECLTimeFix(ecl, 0xb7dc, -60); // Timefix MBoss2
            ecl << pair{0xb420, 0x43100000}; // Fix Boss Position
            ECLVoid(ecl, 0xb7a8); // Void Original Move
            ECLVoid(ecl, 0xb7c8, 0xb730, 0xb764);
            break;
        case THPrac::TH11::TH11_ST3_MID3:
            ECLJump(ecl, 0x14ae8, 0x14ba0);
            ecl << pair{0x14ba0, 0} << pair{0xb2fc, 2000};
            ECLJump(ecl, 0xb404, 0xc0b4);
            ECLVoid(ecl, 0xb34c, 0xb374, 0xc214, 0xc1c8, 0xc10c);
            ecl << pair{0xc0f4, 152.0f};
            break;
        case THPrac::TH11::TH11_ST3_BOSS1:
            st3_boss();
            if (thPracParam.dlg)
                ECLJump(ecl, 0x14bd0, 0x14c68, 0xA0);
            break;
        case THPrac::TH11::TH11_ST3_BOSS2:
            st3_boss();
            ecl << pair{0x1000, 1200} << pair{0x3420, (int16_t)0};
            break;
        case THPrac::TH11::TH11_ST3_BOSS3:
            st3_boss();
            ecl << pair{0x1114, 0x32};
            ECLTimeFix(ecl, 0x103c, -60);
            ECLVoid(ecl, 0x1074, 0x1c44, 0x1c54);
            break;
        case THPrac::TH11::TH11_ST3_BOSS4:
            st3_boss();
            ecl << pair{0x1114, 0x32};
            ECLVoid(ecl, 0x1074, 0x1c44, 0x1c54);
            ecl << pair{0x1a88, 1800} << pair{0x4558, (int16_t)0};
            break;
        case THPrac::TH11::TH11_ST3_BOSS5:
            st3_boss();
            ecl << pair{0x1114, 0x33};
            ECLTimeFix(ecl, 0x103c, -60);
            ECLVoid(ecl, 0x1074, 0x2db4, 0x2dc4);
            break;
        case THPrac::TH11::TH11_ST3_BOSS6:
            st3_boss();
            ecl << pair{0x1114, 0x33};
            ECLVoid(ecl, 0x1074, 0x2db4, 0x2dc4);
            ecl << pair{0x2bf8, 2700} << pair{0x65cc, (int16_t)0};
            break;
        case THPrac::TH11::TH11_ST4_MID1:
            ECLJump(ecl, 0xf764, 0xf7c0);
            ecl << pair{0x95f0, (int16_t)0} << pair{0x7b74, (int16_t)0} << pair{0x7af4, (int16_t)0}
                << pair{0xf7f0, 900};
            break;
        case THPrac::TH11::TH11_ST4_MID2:
            ECLJump(ecl, 0xf764, 0xf8b0);
            ecl << pair{0x9668, (int16_t)0};
            break;
        case THPrac::TH11::TH11_ST4_BOSS1:
            if (thPracParam.dlg)
                ECLJump(ecl, 0xf764, 0xf9e0);
            else
                ECLJump(ecl, 0xf764, 0xfa04);
            break;
        case THPrac::TH11::TH11_ST4_BOSS2:
            ECLJump(ecl, 0xf764, 0xfa04);
            ecl << pair{0xfe4, 1900};
            ECLVoid(ecl, 0x48f0);
            break;
        case THPrac::TH11::TH11_ST4_BOSS3:
            ECLJump(ecl, 0xf764, 0xfa04);
            ecl << pair{0x1110, (int8_t)0x32};
            ECLVoid(ecl, 0x2fa4, 0x1058, 0x2fb4);
            ECLTimeFix(ecl, 0x1020, -60);
            break;
        case THPrac::TH11::TH11_ST4_RA1:
            ECLSatoriJump(ecl, 0);
            break;
        case THPrac::TH11::TH11_ST4_RA2:
            ECLSatoriJump(ecl, 0);
            ecl.SetFile(2);
            ECLJump(ecl, 0x1ac, 0x3f4);
            ecl << pair{0x124, 6000};
            ECLVoid(ecl, 0x128, 0x644, 0x690, 0x654);
            break;
        case THPrac::TH11::TH11_ST4_RA3:
            ECLSatoriJump(ecl, 0);
            ecl.SetFile(2);
            ECLJump(ecl, 0x1ac, 0x1aac);
            ecl << pair{0x124, 3500};
            ECLVoid(ecl, 0x128, 0x1cf8, 0x1d08);
            break;
        case THPrac::TH11::TH11_ST4_RB1:
            ECLSatoriJump(ecl, 1);
            break;
        case THPrac::TH11::TH11_ST4_RB2:
            ECLSatoriJump(ecl, 1);
            ecl.SetFile(3);
            ECLJump(ecl, 0x24c, 0x1bd0);
            ecl << pair{0x170, 3000};
            ECLVoid(ecl, 0x174, 0x1f8, 0x1e20, 0x1e58, 0x1e6c);
            break;
        case THPrac::TH11::TH11_ST4_RB3:
            ECLSatoriJump(ecl, 1);
            ecl.SetFile(3);
            ECLJump(ecl, 0x24c, 0x30e8);
            ecl << pair{0x170, 1500};
            ECLVoid(ecl, 0x174, 0x1f8, 0x3334, 0x3344, 0x33b0);
            ecl << pair{0x3390, 0x3c} << pair{0x33c0, 0x3c};
            break;
        case THPrac::TH11::TH11_ST4_RC1:
            ECLSatoriJump(ecl, 2);
            break;
        case THPrac::TH11::TH11_ST4_RC2:
            ECLSatoriJump(ecl, 2);
            ecl.SetFile(4);
            ECLJump(ecl, 0x310, 0x1ac0);
            ecl << pair{0x180, 4400} << pair{0x230, 5100};
            ECLVoid(ecl, 0x184, 0x2b8, 0x234, 0x1d40, 0x1d78, 0x1d8c);
            break;
        case THPrac::TH11::TH11_ST4_RC3:
            ECLSatoriJump(ecl, 2);
            ecl.SetFile(4);
            ECLJump(ecl, 0x310, 0x2bfc);
            ecl << pair{0x180, 2500} << pair{0x230, 3200};
            ECLVoid(ecl, 0x184, 0x2b8, 0x234, 0x2f1c, 0x2f2c, 0x2f98);
            break;
        case THPrac::TH11::TH11_ST4_MA1:
            ECLSatoriJump(ecl, 3);
            break;
        case THPrac::TH11::TH11_ST4_MA2:
            ECLSatoriJump(ecl, 3);
            ecl.SetFile(5);
            ECLJump(ecl, 0x288, 0x1008);
            ecl << pair{0x198, 3600};
            ECLVoid(ecl, 0x19c, 0x234, 0x1258, 0x1290, 0x12c4);
            break;
        case THPrac::TH11::TH11_ST4_MA3:
            ECLSatoriJump(ecl, 3);
            ecl.SetFile(5);
            ECLJump(ecl, 0x288, 0x217c);
            ecl << pair{0x198, 2000};
            ECLVoid(ecl, 0x19c, 0x234, 0x23c8, 0x23d8, 0x2444);
            break;
        case THPrac::TH11::TH11_ST4_MB1:
            ECLSatoriJump(ecl, 4);
            break;
        case THPrac::TH11::TH11_ST4_MB2:
            ECLSatoriJump(ecl, 4);
            ecl.SetFile(6);
            ECLJump(ecl, 0x214, 0x7b8);
            ecl << pair{0x130, 4400};
            ECLVoid(ecl, 0x134, 0x1b8, 0xa08, 0xa40, 0xa94);
            break;
        case THPrac::TH11::TH11_ST4_MB3:
            ECLSatoriJump(ecl, 4);
            ecl.SetFile(6);
            ECLJump(ecl, 0x214, 0x12ac);
            ecl << pair{0x130, 2400};
            ECLVoid(ecl, 0x134, 0x1b8, 0x14f8, 0x1508, 0x1574);
            break;
        case THPrac::TH11::TH11_ST4_MC1:
            ECLSatoriJump(ecl, 5);
            break;
        case THPrac::TH11::TH11_ST4_MC2:
            ECLSatoriJump(ecl, 5);
            ecl.SetFile(7);
            ECLJump(ecl, 0xdec, 0x1a0c);
            ecl << pair{0xd0c, 4400};
            ECLVoid(ecl, 0xd10, 0xd94, 0x1c5c, 0x1c94, 0x1ce8);
            break;
        case THPrac::TH11::TH11_ST4_MC3:
            ECLSatoriJump(ecl, 5);
            ecl.SetFile(7);
            ECLJump(ecl, 0xdec, 0x2a38);
            ecl << pair{0xd0c, 2400};
            ECLVoid(ecl, 0xd10, 0xd94, 0x2cf4, 0x2d04, 0x2d60);
            break;
        case THPrac::TH11::TH11_ST5_MID1:
            ECLJump(ecl, 0x674c, 0x6844);
            ECLVoid(ecl, 0x29f8);
            break;
        case THPrac::TH11::TH11_ST5_MID2:
            ECLJump(ecl, 0x674c, 0x6844);
            ECLVoid(ecl, 0x29f8);
            ecl.SetFile(2);
            ecl << pair{0x1b8, 9000};
            ecl.SetPos(0x1284);
            ecl << 0 << 0x0014014f << 0x01ff0000 << 0 << 60; // Invicible
            ecl << pair{0x1354, 60};
            ECLVoid(ecl, 0x1354);
            break;
        case THPrac::TH11::TH11_ST5_MID3:
            ECLJump(ecl, 0x674c, 0x6844);
            ECLVoid(ecl, 0x29f8);
            ecl.SetFile(2);
            ecl << pair{0x1b8, 1500} << pair{0x1f8, 1500};
            ecl.SetPos(0x1284);
            ecl << 0 << 0x0014014f << 0x01ff0000 << 0 << 60; // Invicible
            ecl << pair{0x1354, 60};
            ECLVoid(ecl, 0x1354, 0x22a8, 0x21ec);
            break;
        case THPrac::TH11::TH11_ST5_BOSS1:
            if (thPracParam.dlg)
                ECLJump(ecl, 0x674c, 0x6948);
            else
                ECLJump(ecl, 0x674c, 0x696c);
            break;
        case THPrac::TH11::TH11_ST5_BOSS2:
            // A troublesome one. Ew.
            // Use a direct call instead of a thread creation,
            // Make the boss invincible in between.
            ECLJump(ecl, 0x674c, 0x696c);
            ecl.SetFile(3);
            ecl << pair{0xb94, 1500};
            ECLVoid(ecl, 0xc68);
            ecl << pair{0xd00, (int16_t)11};
            ecl.SetPos(0xcdc);
            ecl << 0 << 0x00200142 << 0x01ff0000 << 0 << 65;
            ecl.SetPos(0xd1c);
            ecl << 3 << 0x00140143 << 0x01ff0000 << 0 << 1;
            ecl << 0 << 0x002c014e << 0x04ff0000 << 0 << 0 << 1500 << 3000 << 0xc
                << 0x73736f42 << 0x64726143 << 0x31;
            ecl << 9999 << 0x00100000 << 0x00ff0000 << 0;
            ecl << pair{0x1dac, 0x43960000} << pair{0x1dc0, 2};
            ECLVoid(ecl, 0x15d0, 0x1570);
            break;
        case THPrac::TH11::TH11_ST5_BOSS3:
            ECLJump(ecl, 0x674c, 0x696c);
            ecl.SetFile(3);
            ecl << pair{0x0bf0, (int8_t)0x32};
            ECLVoid(ecl, 0xb64, 0x2d2c, 0x2d64);
            ECLTimeFix(ecl, 0xad8, -60);
            ECLTimeFix(ecl, 0xb28, -1);
            break;
        case THPrac::TH11::TH11_ST5_BOSS4:
            ECLJump(ecl, 0x674c, 0x696c);
            ecl.SetFile(3);
            ecl << pair{0x0bf0, (int8_t)0x32};
            ECLVoid(ecl, 0xb64, 0x2d2c, 0x2d64);
            ECLTimeFix(ecl, 0xad8, -60);
            ECLTimeFix(ecl, 0xb28, -1);
            ECLSetHealth(ecl, 0x2ddc, 2200);
            ECLVoid(ecl, 0x2d78, 0x5360, 0x2d8c);
            break;
        case THPrac::TH11::TH11_ST5_BOSS5:
            ECLJump(ecl, 0x674c, 0x696c);
            ecl.SetFile(3);
            ecl << pair{0x0bf0, (int8_t)0x33};
            ECLVoid(ecl, 0xb64, 0x3e18, 0x3e50);
            ECLTimeFix(ecl, 0xad8, -60);
            ECLTimeFix(ecl, 0xb28, -1);
            break;
        case THPrac::TH11::TH11_ST5_BOSS6:
            ECLJump(ecl, 0x674c, 0x696c);
            ecl.SetFile(3);
            ecl << pair{0x0bf0, (int8_t)0x33};
            ECLVoid(ecl, 0xb64, 0x3e18, 0x3e50);
            ECLTimeFix(ecl, 0xad8, -60);
            ECLTimeFix(ecl, 0xb28, -1);
            ECLSetHealth(ecl, 0x3eb4, 2300);
            ECLVoid(ecl, 0x62f4);
            break;
        case THPrac::TH11::TH11_ST5_BOSS7:
            ECLJump(ecl, 0x674c, 0x696c);
            ecl.SetFile(3);
            ECLVoid(ecl, 0xb64);
            ecl.SetPos(0xbd8);
            ecl << 0 << 0x0014015a << 0x01ff0000 << 0 << 48.0f;
            ECLJump(ecl, 0xbec, 0x72ac);
            ECLVoid(ecl, 0x7548, 0x7580);
            ECLTimeFix(ecl, 0x75e8, -60);
            break;
        case THPrac::TH11::TH11_ST6_MID1:
            THStage6STD();
            THStage6ANM();
            ECLJump(ecl, 0x40b8, 0x41a4);
            ecl.SetFile(2);
            ecl << pair{0x110, 65};
            ecl.SetPos(0x1d0);
            ecl << 60 << 0x00140143 << 0x01ff0000 << 0 << 1;
            break;
        case THPrac::TH11::TH11_ST6_BOSS1:
            THStage6STD();
            THStage6ANM();
            ECLVoid(ecl, 0x460);
            if (thPracParam.dlg)
                ECLJump(ecl, 0x40b8, 0x4294);
            else
                ECLJump(ecl, 0x40b8, 0x42b8);
            break;
        case THPrac::TH11::TH11_ST6_BOSS2:
            THStage6STD();
            THStage6ANM();
            ECLVoid(ecl, 0x460);
            ECLJump(ecl, 0x40b8, 0x42b8);
            ecl.SetFile(3);
            ECLSetHealth(ecl, 0x5f0, 3000);
            ECLVoid(ecl, 0x33d0);
            break;
        case THPrac::TH11::TH11_ST6_BOSS3:
            THStage6STD();
            THStage6ANM();
            ECLVoid(ecl, 0x460);
            ECLJump(ecl, 0x40b8, 0x42b8);
            ecl.SetFile(3);
            ecl << pair{0x518, (int8_t)0x32};
            ECLTimeFix(ecl, 0x450, -60);
            ECLVoid(ecl, 0x48c, 0xc4c, 0xc84);
            break;
        case THPrac::TH11::TH11_ST6_BOSS4:
            THStage6STD();
            THStage6ANM();
            ECLVoid(ecl, 0x460);
            ECLJump(ecl, 0x40b8, 0x42b8);
            ecl.SetFile(3);
            ECLSetHealth(ecl, 0x5f0, 2400);
            ecl << pair{0x5a4, (int8_t)0x32} << pair{0x590, 2400};
            ECLVoid(ecl, 0x4cf0);
            break;
        case THPrac::TH11::TH11_ST6_BOSS5:
            THStage6STD();
            THStage6ANM();
            ECLVoid(ecl, 0x460);
            ECLJump(ecl, 0x40b8, 0x42b8);
            ecl.SetFile(3);
            ecl << pair{0x518, (int8_t)0x33};
            ECLTimeFix(ecl, 0x450, -60);
            ECLVoid(ecl, 0x48c, 0x1b20, 0x1b58);
            break;
        case THPrac::TH11::TH11_ST6_BOSS6:
            THStage6STD();
            THStage6ANM();
            ECLVoid(ecl, 0x460);
            ECLJump(ecl, 0x40b8, 0x42b8);
            ecl.SetFile(3);
            ECLSetHealth(ecl, 0x5f0, 3000);
            ecl << pair{0x5a4, (int8_t)0x33} << pair{0x590, 3000};
            ECLVoid(ecl, 0x60fc);
            break;
        case THPrac::TH11::TH11_ST6_BOSS7:
            THStage6STD();
            THStage6ANM();
            ECLVoid(ecl, 0x460);
            ECLJump(ecl, 0x40b8, 0x42b8);
            ecl.SetFile(3);
            ecl << pair{0x518, (int8_t)0x34};
            ECLTimeFix(ecl, 0x450, -60);
            ECLVoid(ecl, 0x48c, 0x24f8, 0x2530);
            break;
        case THPrac::TH11::TH11_ST6_BOSS8:
            THStage6STD();
            THStage6ANM();
            ECLVoid(ecl, 0x460);
            ECLJump(ecl, 0x40b8, 0x42b8);
            ecl.SetFile(3);
            ECLSetHealth(ecl, 0x5f0, 3500);
            ecl << pair{0x5a4, (int8_t)0x34} << pair{0x590, 3500};
            ECLVoid(ecl, 0x77dc, 0x787c, 0x78d0, 0x78fc, 0x7928);
            ecl.SetPos(0x785c);
            ecl << 0 << 0x00200118 << 0x02ff0000 << 0 << 0 << 0x43600000;
            break;
        case THPrac::TH11::TH11_ST6_BOSS9:
            THStage6STD();
            THStage6ANM();
            ECLVoid(ecl, 0x460);
            ECLJump(ecl, 0x40b8, 0x42b8);
            ecl.SetFile(3);
            ECLSetHealth(ecl, 0x5f0, 3500);
            ecl << pair{0x5a4, (int8_t)0x35} << pair{0x590, 3500};
            ECLVoid(ecl, 0x8e48, 0x8e80, 0x8ee8);
            ecl.SetPos(0x8ec8);
            ecl << 0 << 0x00200118 << 0x02ff0000 << 0 << 0 << 0x43600000;

            switch (thPracParam.phase) {
            case 1:
                ecl << pair{0x8cb0, 4999};
                ECLJump(ecl, 0x9260, 0x9318);
                ecl << pair{0x9344, (int16_t)0};
                break;
            case 2:
                ecl << pair{0x8cb0, 2999};
                ECLJump(ecl, 0x9260, 0x9318);
                ECLJump(ecl, 0x9354, 0x940c);
                ecl << pair{0x9344, (int16_t)0} << pair{0x9438, (int16_t)0};
                break;
            case 3:
                ecl << pair{0x8cb0, 1499};
                ECLJump(ecl, 0x9260, 0x9318);
                ECLJump(ecl, 0x9354, 0x940c);
                ECLJump(ecl, 0x946c, 0x9524);
                ecl << pair{0x9344, (int16_t)0} << pair{0x9438, (int16_t)0} << pair{0x9550, (int16_t)0};
                break;
            case 4:
                ecl << pair{0x8cb0, 1499};
                ECLJump(ecl, 0x9260, 0x9318);
                ECLJump(ecl, 0x9354, 0x940c);
                ECLJump(ecl, 0x946c, 0x9524);
                ecl << pair{0x9344, (int16_t)0} << pair{0x9438, (int16_t)0} << pair{0x9550, (int16_t)0};
                ecl << pair{0x9f70, 2181}; // 2180
                ecl << pair{0xa08c, (int16_t)0};
                break;
            default:
                break;
            }
            break;
        case THPrac::TH11::TH11_ST7_MID1:
            if (thPracParam.dlg)
                ECLJump(ecl, 0x4c00, 0x4d18);
            else
                ECLJump(ecl, 0x4c00, 0x4d2c);
            ecl.SetFile(2);
            ECLVoid(ecl, 0x284);
            break;
        case THPrac::TH11::TH11_ST7_MID2:
            ECLJump(ecl, 0x4c00, 0x4d2c);
            ecl.SetFile(2);
            ECLVoid(ecl, 0x284);
            ecl << pair{0x351, (int8_t)0x32};
            ECLTimeFix(ecl, 0x298, -60);
            ECLVoid(ecl, 0x17cc, 0x1994, 0x19a4);
            break;
        case THPrac::TH11::TH11_ST7_MID3:
            ECLJump(ecl, 0x4c00, 0x4d2c);
            ecl.SetFile(2);
            ECLVoid(ecl, 0x284);
            ecl << pair{0x351, (int8_t)0x33};
            ECLTimeFix(ecl, 0x298, -60);
            ECLVoid(ecl, 0x2048, 0x2210, 0x2220);
            break;
        case THPrac::TH11::TH11_ST7_END_NS1:
            if (thPracParam.dlg)
                ECLJump(ecl, 0x4c00, 0x4e38);
            else
                ECLJump(ecl, 0x4c00, 0x4e5c);
            ECLVoid(ecl, 0x734);
            break;
        case THPrac::TH11::TH11_ST7_END_S1:
            ECLJump(ecl, 0x4c00, 0x4e5c);
            ecl.SetFile(3);
            ECLSetHealth(ecl, 0x734, 3000);
            ECLVoid(ecl, 0x4ab0, 0x4c98);
            break;
        case THPrac::TH11::TH11_ST7_END_NS2:
            ECLJump(ecl, 0x4c00, 0x4e5c);
            ecl.SetFile(3);
            ecl << pair{0x65c, (int8_t)0x32};
            ECLTimeFix(ecl, 0x568, -60);
            ECLVoid(ecl, 0x5b8);
            ecl << pair{0x528, 0x43200000};
            ECLVoid(ecl, 0x1198, 0x11d0);
            break;
        case THPrac::TH11::TH11_ST7_END_S2:
            ECLJump(ecl, 0x4c00, 0x4e5c);
            ecl.SetFile(3);
            ecl << pair{0x65c, (int8_t)0x32};
            ECLTimeFix(ecl, 0x568, -60);
            ECLVoid(ecl, 0x5b8);
            ecl << pair{0x528, 0x43200000};
            ECLVoid(ecl, 0x1198, 0x11d0);
            ECLSetHealth(ecl, 0x1214, 3300);
            ECLVoid(ecl, 0x775c);
            break;
        case THPrac::TH11::TH11_ST7_END_NS3:
            ECLJump(ecl, 0x4c00, 0x4e5c);
            ecl.SetFile(3);
            ecl << pair{0x65c, (int8_t)0x33};
            ECLTimeFix(ecl, 0x568, -60);
            ECLVoid(ecl, 0x5b8);
            ECLVoid(ecl, 0x1870, 0x18a8);
            break;
        case THPrac::TH11::TH11_ST7_END_S3:
            ECLJump(ecl, 0x4c00, 0x4e5c);
            ecl.SetFile(3);
            ecl << pair{0x65c, (int8_t)0x33};
            ECLTimeFix(ecl, 0x568, -60);
            ECLVoid(ecl, 0x5b8);
            ECLVoid(ecl, 0x1870, 0x18a8);
            ECLSetHealth(ecl, 0x190c, 2800);
            ECLVoid(ecl, 0x8278, 0x82b0);
            break;
        case THPrac::TH11::TH11_ST7_END_NS4:
            ECLJump(ecl, 0x4c00, 0x4e5c);
            ecl.SetFile(3);
            ecl << pair{0x65c, (int8_t)0x34};
            ECLTimeFix(ecl, 0x568, -60);
            ECLVoid(ecl, 0x5b8);
            ECLVoid(ecl, 0x1fd0, 0x2008);
            break;
        case THPrac::TH11::TH11_ST7_END_S4:
            ECLJump(ecl, 0x4c00, 0x4e5c);
            ecl.SetFile(3);
            ecl << pair{0x65c, (int8_t)0x34};
            ECLTimeFix(ecl, 0x568, -60);
            ECLVoid(ecl, 0x5b8);
            ECLVoid(ecl, 0x1fd0, 0x2008);
            ecl << pair{0x201c, 60} << pair{0x202c, 60};
            ECLSetHealth(ecl, 0x204c, 3200, 60);
            ECLVoid(ecl, 0x89a0, 0x89d8);
            break;
        case THPrac::TH11::TH11_ST7_END_NS5:
            ECLJump(ecl, 0x4c00, 0x4e5c);
            ecl.SetFile(3);
            ecl << pair{0x65c, (int8_t)0x35};
            ECLTimeFix(ecl, 0x568, -60);
            ECLVoid(ecl, 0x5b8);
            ECLVoid(ecl, 0x2a8c, 0x2ac4);
            break;
        case THPrac::TH11::TH11_ST7_END_S5:
            ECLJump(ecl, 0x4c00, 0x4e5c);
            ecl.SetFile(3);
            ecl << pair{0x65c, (int8_t)0x35};
            ECLTimeFix(ecl, 0x568, -60);
            ECLVoid(ecl, 0x5b8);
            ECLVoid(ecl, 0x2a8c, 0x2ac4);
            ecl << pair{0x2ad8, 60} << pair{0x2ae8, 60};
            ECLSetHealth(ecl, 0x2b08, 1);
            ECLVoid(ecl, 0x9374, 0x93ac);
            break;
        case THPrac::TH11::TH11_ST7_END_NS6:
            ECLJump(ecl, 0x4c00, 0x4e5c);
            ecl.SetFile(3);
            ecl << pair{0x65c, (int8_t)0x36};
            ECLTimeFix(ecl, 0x568, -60);
            ECLVoid(ecl, 0x5b8);
            ECLVoid(ecl, 0x342c, 0x3464);
            break;
        case THPrac::TH11::TH11_ST7_END_S6:
            ECLJump(ecl, 0x4c00, 0x4e5c);
            ecl.SetFile(3);
            ecl << pair{0x65c, (int8_t)0x36};
            ECLTimeFix(ecl, 0x568, -60);
            ECLVoid(ecl, 0x5b8);
            ECLVoid(ecl, 0x342c, 0x3464);
            ECLSetHealth(ecl, 0x34e0, 3200);
            ecl << pair{0x34b0, 60} << pair{0x34c0, 60};
            ECLVoid(ecl, 0xa764, 0xa79c);
            break;
        case THPrac::TH11::TH11_ST7_END_NS7:
            ECLJump(ecl, 0x4c00, 0x4e5c);
            ecl.SetFile(3);
            ecl << pair{0x65c, (int8_t)0x37};
            ECLTimeFix(ecl, 0x568, -60);
            ECLVoid(ecl, 0x5b8);
            ECLVoid(ecl, 0x3b68, 0x3ba0);
            break;
        case THPrac::TH11::TH11_ST7_END_S7:
            ECLJump(ecl, 0x4c00, 0x4e5c);
            ecl.SetFile(3);
            ecl << pair{0x65c, (int8_t)0x37};
            ECLTimeFix(ecl, 0x568, -60);
            ECLVoid(ecl, 0x5b8);
            ECLVoid(ecl, 0x3b68, 0x3ba0);
            ECLSetHealth(ecl, 0x3c1c, 3600);
            ecl << pair{0x3bec, 60} << pair{0x3bfc, 60};
            ECLVoid(ecl, 0xb858, 0xb890);
            break;
        case THPrac::TH11::TH11_ST7_END_NS8:
            ECLJump(ecl, 0x4c00, 0x4e5c);
            ecl.SetFile(3);
            ecl << pair{0x65c, (int8_t)0x38};
            ECLTimeFix(ecl, 0x568, -60);
            ECLVoid(ecl, 0x5b8);
            ECLVoid(ecl, 0x441c, 0x4454);
            break;
        case THPrac::TH11::TH11_ST7_END_S8:
            ECLJump(ecl, 0x4c00, 0x4e5c);
            ecl.SetFile(3);
            ecl << pair{0x65c, (int8_t)0x38};
            ECLTimeFix(ecl, 0x568, -60);
            ECLVoid(ecl, 0x5b8);
            ECLVoid(ecl, 0x441c, 0x4454);
            ECLSetHealth(ecl, 0x44d0, 2000);
            ecl << pair{0x44a0, 60} << pair{0x44b0, 60};
            ECLVoid(ecl, 0xc610, 0xc648);
            break;
        case THPrac::TH11::TH11_ST7_END_S9:
            ECLJump(ecl, 0x4c00, 0x4e5c);
            ecl.SetFile(3);
            ecl << pair{0x65c, (int8_t)0x38};
            ECLTimeFix(ecl, 0x568, -60);
            ECLVoid(ecl, 0x5b8);
            ECLVoid(ecl, 0x441c, 0x4454);
            ECLSetHealth(ecl, 0x44d0, 2000);
            ecl << pair{0x44a0, 60} << pair{0x44b0, 60};
            ECLVoid(ecl, 0xc610, 0xc648);
            ecl << pair{0x42b4, 0x39};
            ECLVoid(ecl, 0xd9e8, 0xda20, 0xda4c);
            break;
        case THPrac::TH11::TH11_ST7_END_S10:
            ECLJump(ecl, 0x4c00, 0x4e5c);
            ecl.SetFile(3);
            ecl << pair{0x65c, (int8_t)0x38};
            ECLTimeFix(ecl, 0x568, -60);
            ECLVoid(ecl, 0x5b8);
            ECLVoid(ecl, 0x441c, 0x4454);
            ECLSetHealth(ecl, 0x44d0, 2000);
            ecl << pair{0x44a0, 60} << pair{0x44b0, 60};
            ECLVoid(ecl, 0xc610, 0xc648);
            ecl << pair{0x42b4, 0x00003031};
            ECLVoid(ecl, 0x5204, 0x523c, 0x5268, 0x5000, 0x504c);

            switch (thPracParam.phase) {
            case 1:
                ecl << pair{0x5100, 8399};
                ECLJump(ecl, 0x53b8, 0x5484, 60);
                break;
            case 2:
                ecl << pair{0x5100, 5599};
                ECLJump(ecl, 0x53b8, 0x55e8, 60);
                break;
            case 3:
                ecl << pair{0x5100, 2799};
                ECLJump(ecl, 0x53b8, 0x574c, 60);
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
        ecl.SetBaseAddr((void*)GetMemAddr(0x4a8d7c, 0x64, 0xC));

        auto section = thPracParam.section;
        if (section >= 10000 && section < 20000) {
            int stage = (section - 10000) / 100;
            int portionId = (section - 10000) % 100;
            THStageWarp(ecl, stage, portionId);
        } else {
            THPatch(ecl, (th_sections_t)section);
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
    EHOOK_DY(th11_everlasting_bgm, 0x44a9c0)
    {
        auto isInReplay = []() -> bool {
            if (*(uintptr_t*)0x4a8eB8)
                return GetMemContent(0x4a8eB8, 0x10);
            return false;
        };

        int32_t retn_addr = ((int32_t*)pCtx->Esp)[0];
        int32_t bgm_cmd = ((int32_t*)pCtx->Esp)[1];
        int32_t bgm_id = ((int32_t*)pCtx->Esp)[2];
        int32_t call_addr = ((int32_t*)pCtx->Esp)[3];

        bool el_switch;
        bool is_practice;
        bool result;

        el_switch = *(THOverlay::singleton().mElBgm) && !isInReplay() && thPracParam.mode && thPracParam.section;
        is_practice = (*((int32_t*)0x4a5758) & 0x1);
        result = ElBgmTest<0x42a183, 0x42028f, 0x42c72a, 0x42c8af, 0xffffffff>(
            el_switch, is_practice, retn_addr, bgm_cmd, bgm_id, 0xffffffff);

        if (result) {
            pCtx->Eip = 0x44aa54;
        }
    }
    EHOOK_DY(th11_game_init, 0x43a0bc)
    {
        thPracParam.Reset();
    }
    PATCH_DY(th11_disable_bg_1, 0x43d715, "\x83\xc4\x0c\x90\x90", 5); // add esp, 0xc
    PATCH_DY(th11_disable_bg_2, 0x43dbe0, "\xeb\x25", 2); // jmp short 0043DC07
    EHOOK_DY(th11_prac_menu_1, 0x43d72f)
    {
        THGuiPrac::singleton().State(1);
    }
    EHOOK_DY(th11_prac_menu_2, 0x43d752)
    {
        THGuiPrac::singleton().State(2);
    }
    EHOOK_DY(th11_prac_menu_3, 0x43d9b5)
    {
        THGuiPrac::singleton().State(3);
    }
    EHOOK_DY(th11_prac_menu_4, 0x43da4c)
    {
        THGuiPrac::singleton().State(4);
    }
    PATCH_DY(th11_prac_menu_enter_1, 0x43d812, "\xeb", 1);
    EHOOK_DY(th11_prac_menu_enter_2, 0x43da13)
    {
        pCtx->Eax = thPracParam.stage;
    }
    EHOOK_DY(th11_patch_main, 0x41fdfb)
    {
        if (thPracParam.mode == 1) {
            uint32_t* target;
            target = (uint32_t*)0x4a5718; // Life
            *target = thPracParam.life;
            target = (uint32_t*)0x4a571c; // Life Fragments
            *target = thPracParam.life_fragment;
            target = (uint32_t*)0x4a56e8; // Power
            *target = thPracParam.power;
            target = (uint32_t*)0x4a5754; // Graze
            *target = thPracParam.graze;
            target = (uint32_t*)0x4a56f4; // Signal
            target = (uint32_t*)0x4a56f0; // Value
            auto value = thPracParam.value;
            value *= 100;
            *target = *((uint32_t*)&value);
            target = (uint32_t*)0x4a56e4; // Score
            auto score = thPracParam.score;
            score /= 10;
            *target = *((uint32_t*)&score);

            THSectionPatch();
        }
        thPracParam._playLock = true;
    }
    EHOOK_DY(th11_disable_logo, 0x41a7ef)
    {
        if (thPracParam.mode == 1 && thPracParam.section) {
            if (thPracParam.section <= 10000 || thPracParam.section >= 20000 || thPracParam.section % 100 != 1) {
                pCtx->Eip = 0x41a8cf;
            }
        }
    }
    EHOOK_DY(th11_rep_save, 0x436b1f)
    {
        char* repName = (char*)(pCtx->Esp + 0x28);
        if (thPracParam.mode)
            THSaveReplay(repName);
    }
    EHOOK_DY(th11_rep_load, 0x435a05)
    {
        thPracParam = {};
        std::string param;
        std::wstring path(L"replay/");
        path += mb_to_utf16((char*)pCtx->Ecx, 932);
        if (ReplayLoadParam(path.c_str(), param))
            thPracParam.ReadJson(param);
    }
    EHOOK_DY(th11_bgm_1, 0x42053b)
    {
        if (THBGMTest()) {
            PushHelper32(pCtx, 1);
            pCtx->Eip = 0x42053d;
        }
    }
    EHOOK_DY(th11_bgm_2, 0x420529)
    {
        if (THBGMTest()) {
            PushHelper32(pCtx, 1);
            pCtx->Eip = 0x42052b;
        }
    }
    EHOOK_DY(th11_bgm_3, 0x420542)
    {
        if (*((int32_t*)0x4a5728) == 6 && thPracParam.mode && thPracParam.section) {
            pCtx->Eip = 0x420547;
        }
    }
    EHOOK_DY(th11_bgm_4, 0x42c707)
    {
        if (*((int32_t*)0x4a5728) == 6 && thPracParam.mode && thPracParam.section) {
            pCtx->Eip = 0x42c717;
        }
    }
    EHOOK_DY(th11_bgm_5, 0x42c890)
    {
        if (*((int32_t*)0x4a5728) == 6 && thPracParam.mode && thPracParam.section) {
            pCtx->Eip = 0x42c89b;
        }
    }
    EHOOK_DY(th11_update, 0x456deb)
    {
        GameGuiBegin(IMPL_WIN32_DX9, !THAdvOptWnd::singleton().IsOpen());

        // Gui components update
        THGuiPrac::singleton().Update();
        THOverlay::singleton().Update();
        bool drawCursor = THAdvOptWnd::StaticUpdate() || THGuiPrac::singleton().IsOpen();

        GameGuiEnd(drawCursor);
    }
    EHOOK_DY(th11_render, 0x456f12)
    {
        GameGuiRender(IMPL_WIN32_DX9);
    }
    HOOKSET_ENDDEF()

    HOOKSET_DEFINE(THInitHook)
    static __declspec(noinline) void THGuiCreate()
    {
        // Init
        GameGuiInit(IMPL_WIN32_DX9, 0x4c3288, 0x4c3d88, 0x445e00,
            Gui::INGAGME_INPUT_GEN2, 0x4c92b4, 0x4c92b0, 0,
            -1);

        // Gui components creation
        THGuiPrac::singleton();
        THOverlay::singleton();

        // Hooks
        THMainHook::singleton().EnableAllHooks();

        // Hooks
        THMainHook::singleton().EnableAllHooks();

        // Reset thPracParam
        thPracParam.Reset();
    }
    static __declspec(noinline) void THInitHookDisable()
    {
        auto& s = THInitHook::singleton();
        s.th11_gui_init_1.Disable();
        s.th11_gui_init_2.Disable();
    }
    PATCH_DY(th11_disable_demo, 0x439a7b, "\xff\xff\xff\x7f", 4);
    EHOOK_DY(th11_disable_mutex, 0x4455b3)
    {
        pCtx->Eip = 0x4455cc;
    }
    PATCH_DY(th11_startup_1, 0x439527, "\xeb", 1);
    PATCH_DY(th11_startup_2, 0x43a12a, "\xeb", 1);
    EHOOK_DY(th11_gui_init_1, 0x43a299)
    {
        THGuiCreate();
        THInitHookDisable();
    }
    EHOOK_DY(th11_gui_init_2, 0x447269)
    {
        THGuiCreate();
        THInitHookDisable();
    }
    HOOKSET_ENDDEF()
}

void TH11Init()
{
    TH11::THInitHook::singleton().EnableAllHooks();
}
}
