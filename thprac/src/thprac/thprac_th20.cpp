#include "thprac_games.h"
#include "thprac_utils.h"
#include "../3rdParties/MinHook/include/MinHook.h"
#include <format>

namespace THPrac {
namespace TH20 {
    using std::pair;

    int g_jump_stage=1;
    int g_life=2;
    int g_bomb=3;
    int g_piv=0;//0~1000000
    int g_power=100;
    int g_hyper=0;
    int g_delta=0;
    bool g_prac_mode = false;

    struct THPracParam {
        int32_t mode;
        int32_t stage;
        int32_t section;
        int32_t phase;

        int64_t score;
        int32_t life;
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

            ForceJsonValue(game, "th20");
            GetJsonValue(mode);
            GetJsonValue(stage);
            GetJsonValue(section);
            GetJsonValue(phase);
            GetJsonValueEx(dlg, Bool);

            GetJsonValue(score);
            GetJsonValue(life);
            GetJsonValue(bomb);
            GetJsonValue(bomb_fragment);
            GetJsonValue(power);
            GetJsonValue(value);
            GetJsonValue(graze);

            return true;
        }
        std::string GetJson()
        {
            if (mode == 1) {
                CreateJson();

                AddJsonValueEx(version, GetVersionStr(), jalloc);
                AddJsonValueEx(game, "th20", jalloc);
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
                AddJsonValue(bomb);
                AddJsonValue(bomb_fragment);
                AddJsonValue(power);
                AddJsonValue(value);
                AddJsonValue(graze);

                ReturnJson();
            } 

            CreateJson();
            jalloc; // Dummy usage to silence C4189
            ReturnJson();
        }
    };
    THPracParam thPracParam {};

//      class THGuiPrac : public Gui::GameGuiWnd {
//         THGuiPrac() noexcept
//         {
//             *mMode = 1;
//             *mLife = 9;
//             *mBomb = 9;
//             *mPower = 400;
//             *mValue = 10000;
// 
//             SetFade(0.8f, 0.1f);
//             SetStyle(ImGuiStyleVar_WindowRounding, 0.0f);
//             SetStyle(ImGuiStyleVar_WindowBorderSize, 0.0f);
//             OnLocaleChange();
//         }
//         SINGLETON(THGuiPrac);
// 
//     public:
//         __declspec(noinline) void State(int state)
//         {
//             switch (state) {
//             case 0:
//                 break;
//             case 1:
//                 mDiffculty = *((int32_t*)0x49f274);
//                 SetFade(0.8f, 0.1f);
//                 Open();
//                 thPracParam.Reset();
//             case 2:
//                 break;
//             case 3:
//                 SetFade(0.8f, 0.1f);
//                 Close();
// 
//                 // Fill Param
//                 thPracParam.mode = *mMode;
//                 thPracParam.stage = *mStage;
//                 thPracParam.section = CalcSection();
//                 thPracParam.phase = SpellPhase() ? *mPhase : 0;
//                 if (SectionHasDlg(thPracParam.section))
//                     thPracParam.dlg = *mDlg;
// 
//                 thPracParam.score = *mScore;
//                 thPracParam.life = *mLife;
//                 thPracParam.bomb = *mBomb;
//                 thPracParam.bomb_fragment = *mBombFragment;
//                 thPracParam.power = *mPower;
//                 thPracParam.value = *mValue;
//                 thPracParam.graze = *mGraze;
//                 break;
//             case 4:
//                 Close();
//                 *mNavFocus = 0;
//                 break;
//             default:
//                 break;
//             }
//         }
// 
//     protected:
//         virtual void OnLocaleChange() override
//         {
//             SetTitle(S(TH_MENU));
//             switch (Gui::LocaleGet()) {
//             case Gui::LOCALE_ZH_CN:
//                 SetSizeRel(0.5f, 0.81f);
//                 SetPosRel(0.27f, 0.18f);
//                 SetItemWidthRel(-0.100f);
//                 SetAutoSpacing(true);
//                 break;
//             case Gui::LOCALE_EN_US:
//                 SetSizeRel(0.6f, 0.79f);
//                 SetPosRel(0.215f, 0.18f);
//                 SetItemWidthRel(-0.100f);
//                 SetAutoSpacing(true);
//                 break;
//             case Gui::LOCALE_JA_JP:
//                 SetSizeRel(0.56f, 0.81f);
//                 SetPosRel(0.230f, 0.18f);
//                 SetItemWidthRel(-0.105f);
//                 SetAutoSpacing(true);
//                 break;
//             default:
//                 break;
//             }
//         }
//         virtual void OnContentUpdate() override
//         {
//             ImGui::TextUnformatted(S(TH_MENU));
//             ImGui::Separator();
// 
//             PracticeMenu();
//         }
//         const th_glossary_t* SpellPhase()
//         {
//             auto section = CalcSection();
//             return nullptr;
//         }
//         void PracticeMenu()
//         {
//             mMode();
//             if (mStage())
//                 *mSection = *mChapter = 0;
//             if (*mMode == 1) {
//                 int mbs = -1;
//                 if (*mStage == 5) { // Counting from 0
//                     mbs = 2;
//                     if (*mWarp == 2)
//                         *mWarp = 0;
//                 }
//                 if (mWarp(mbs))
//                     *mSection = *mChapter = *mPhase = 0;
//                 if (*mWarp) {
//                     SectionWidget();
//                     mPhase(TH_PHASE, SpellPhase());
//                 }
// 
//                 mLife();
//                 mBomb();
//                 mBombFragment();
//                 auto power_str = std::to_string((float)(*mPower) / 100.0f).substr(0, 4);
//                 mPower(power_str.c_str());
//                 mValue();
//                 mValue.RoundDown(10);
//                 mGraze();
//                 mScore();
//                 mScore.RoundDown(10);
//             }
// 
//             mNavFocus();
//         }
//         int CalcSection()
//         {
//             int chapterId = 0;
//             switch (*mWarp) {
//             case 1: // Chapter
//                 // Chapter Id = 10000 + Stage * 100 + Section
//                 chapterId += (*mStage + 1) * 100;
//                 chapterId += *mChapter;
//                 chapterId += 10000; // Base of chapter ID is 1000.
//                 return chapterId;
//                 break;
//             case 2:
//             case 3: // Mid boss & End boss
//                 return th_sections_cba[*mStage][*mWarp - 2][*mSection];
//                 break;
//             case 4:
//             case 5: // Non-spell & Spellcard
//                 return th_sections_cbt[*mStage][*mWarp - 4][*mSection];
//                 break;
//             default:
//                 return 0;
//                 break;
//             }
//         }
//         bool SectionHasDlg(int32_t section)
//         {
//             switch (section) {
//             case TH20_ST1_BOSS1:
//                 return true;
//             default:
//                 return false;
//             }
//         }
//         void SectionWidget()
//         {
//             static char chapterStr[256] {};
//             auto& chapterCounts = mChapterSetup[*mStage];
// 
//             switch (*mWarp) {
//             case 1: // Chapter
//                 mChapter.SetBound(1, chapterCounts[0] + chapterCounts[1]);
// 
//                 if (chapterCounts[1] == 0 && chapterCounts[2] != 0) {
//                     sprintf_s(chapterStr, S(TH_STAGE_PORTION_N), *mChapter);
//                 } else if (*mChapter <= chapterCounts[0]) {
//                     sprintf_s(chapterStr, S(TH_STAGE_PORTION_1), *mChapter);
//                 } else {
//                     sprintf_s(chapterStr, S(TH_STAGE_PORTION_2), *mChapter - chapterCounts[0]);
//                 };
// 
//                 mChapter(chapterStr);
//                 break;
//             case 2:
//             case 3: // Mid boss & End boss
//                 if (mSection(TH_WARP_SELECT[*mWarp],
//                         th_sections_cba[*mStage][*mWarp - 2],
//                         th_sections_str[::THPrac::Gui::LocaleGet()][mDiffculty]))
//                     *mPhase = 0;
//                 if (SectionHasDlg(th_sections_cba[*mStage][*mWarp - 2][*mSection]))
//                     mDlg();
//                 break;
//             case 4:
//             case 5: // Non-spell & Spellcard
//                 if (mSection(TH_WARP_SELECT[*mWarp],
//                         th_sections_cbt[*mStage][*mWarp - 4],
//                         th_sections_str[::THPrac::Gui::LocaleGet()][mDiffculty]))
//                     *mPhase = 0;
//                 if (SectionHasDlg(th_sections_cbt[*mStage][*mWarp - 4][*mSection]))
//                     mDlg();
//                 break;
//             default:
//                 break;
//             }
//         }
// 
//         Gui::GuiCombo mMode { TH_MODE, TH_MODE_SELECT };
//         Gui::GuiCombo mStage { TH_STAGE, TH_STAGE_SELECT };
//         Gui::GuiCombo mWarp { TH_WARP, TH_WARP_SELECT };
//         Gui::GuiCombo mSection { TH_MODE };
//         Gui::GuiCombo mPhase { TH_PHASE };
//         Gui::GuiCheckBox mDlg { TH_DLG };
// 
//         Gui::GuiSlider<int, ImGuiDataType_S32> mChapter { TH_CHAPTER, 0, 0 };
//         Gui::GuiDrag<int64_t, ImGuiDataType_S64> mScore { TH_SCORE, 0, 9999999990, 10, 100000000 };
//         Gui::GuiSlider<int, ImGuiDataType_S32> mLife { TH_LIFE, 0, 9 };
//         Gui::GuiSlider<int, ImGuiDataType_S32> mBomb { TH_BOMB, 0, 9 };
//         Gui::GuiSlider<int, ImGuiDataType_S32> mBombFragment { TH_BOMB_FRAGMENT, 0, 4 };
//         Gui::GuiSlider<int, ImGuiDataType_S32> mPower { TH_POWER, 100, 400 };
//         Gui::GuiDrag<int, ImGuiDataType_S32> mValue { TH_VALUE, 0, 999990, 10, 100000 };
//         Gui::GuiDrag<int, ImGuiDataType_S32> mGraze { TH_GRAZE, 0, 999999, 1, 100000 };
// 
//         Gui::GuiNavFocus mNavFocus { TH_STAGE, TH_MODE, TH_WARP, TH_DLG,
//             TH_MID_STAGE, TH_END_STAGE, TH_NONSPELL, TH_SPELL, TH_PHASE, TH_CHAPTER,
//             TH_SCORE, TH_LIFE, TH_BOMB, TH_BOMB_FRAGMENT, TH16_SEASON_GAUGE,
//             TH_POWER, TH_VALUE, TH_GRAZE };
// 
//         int mChapterSetup[7][2] {
//             { 2, 2 },
//             { 4, 0 },
//             { 4, 2 },
//             { 3, 3 },
//             { 5, 3 },
//             { 3, 0 },
//             { 4, 4 },
//         };
// 
//         int mDiffculty = 0;
//     };
//      class THGuiRep : public Gui::GameGuiWnd {
//         THGuiRep() noexcept
//         {
//             wchar_t appdata[MAX_PATH];
//             GetEnvironmentVariableW(L"APPDATA", appdata, MAX_PATH);
//             mAppdataPath = appdata;
//         }
//         SINGLETON(THGuiRep);
// 
//     public:
//         void CheckReplay()
//         {
//             uint32_t index = GetMemContent(0x4a6f20, 0x5b48);
//             char* repName = (char*)GetMemAddr(0x4a6f20, index * 4 + 0x5b50, 0x21c);
//             std::wstring repDir(mAppdataPath);
//             repDir.append(L"\\ShanghaiAlice\\th16\\replay\\");
//             repDir.append(mb_to_utf16(repName, 932));
// 
//             std::string param;
//             if (ReplayLoadParam(repDir.c_str(), param) && mRepParam.ReadJson(param))
//                 mParamStatus = true;
//             else
//                 mRepParam.Reset();
//         }
// 
//         bool mRepStatus = false;
//         void State(int state)
//         {
//             switch (state) {
//             case 1:
//                 mRepStatus = false;
//                 mParamStatus = false;
//                 thPracParam.Reset();
//                 break;
//             case 2:
//                 CheckReplay();
//                 break;
//             case 3:
//                 mRepStatus = true;
//                 if (mParamStatus)
//                     memcpy(&thPracParam, &mRepParam, sizeof(THPracParam));
//                 break;
//             default:
//                 break;
//             }
//         }
// 
//     protected:
//         std::wstring mAppdataPath;
//         bool mParamStatus = false;
//         THPracParam mRepParam;
//     };

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
        }
        virtual void OnContentUpdate() override
        {
            mMuteki();
            mInfLives();
            mInfBombs();
            mInfPower();
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
        Gui::GuiHotKey mMuteki { TH_MUTEKI, "F1", VK_F1, { new HookCtx(0x0FB5CC, "\x01", 1) } };
        Gui::GuiHotKey mInfBombs { TH_INFBOMBS, "F3", VK_F3, { new HookCtx(0x0E40A2, "\x90\x90\x90", 3) } };
        Gui::GuiHotKey mInfPower { TH_INFPOWER, "F4", VK_F4, { new HookCtx(0x0E4022, "\x90\x90\x90", 3) } };

    public:
        Gui::GuiHotKey mInfLives {
            TH_INFLIVES2,
            "F2",
            VK_F2,
        };
    };
     
//      class TH16InGameInfo : public Gui::GameGuiWnd {
//         TH16InGameInfo() noexcept
//         {
//             SetTitle("igi");
//             SetFade(0.9f, 0.9f);
//             SetPosRel(900.0f / 1280.0f, 500.0f / 960.0f);
//             SetSizeRel(340.0f / 1280.0f, 0.0f);
//             SetWndFlag(ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav | 0);
//             OnLocaleChange();
//         }
//         SINGLETON(TH16InGameInfo);
// 
//     public:
//         int32_t mMissCount;
//         int32_t mBombCount;
//         int32_t mReleaseCount;
// 
//     protected:
//         virtual void OnLocaleChange() override
//         {
//             float x_offset_1 = 0.0f;
//             float x_offset_2 = 0.0f;
//             switch (Gui::LocaleGet()) {
//             case Gui::LOCALE_ZH_CN:
//                 x_offset_1 = 0.12f;
//                 x_offset_2 = 0.172f;
//                 break;
//             case Gui::LOCALE_EN_US:
//                 x_offset_1 = 0.12f;
//                 x_offset_2 = 0.16f;
//                 break;
//             case Gui::LOCALE_JA_JP:
//                 x_offset_1 = 0.18f;
//                 x_offset_2 = 0.235f;
//                 break;
//             default:
//                 break;
//             }
//         }
// 
//         virtual void OnContentUpdate() override
//         {
//             ImGui::Columns(2);
//             ImGui::Text(S(THPRAC_INGAMEINFO_MISS_COUNT));
//             ImGui::NextColumn();
//             ImGui::Text("%8d", mMissCount);
//             ImGui::NextColumn();
//             ImGui::Text(S(THPRAC_INGAMEINFO_BOMB_COUNT));
//             ImGui::NextColumn();
//             ImGui::Text("%8d", mBombCount);
//             ImGui::NextColumn();
//             ImGui::Text(S(THPRAC_INGAMEINFO_16_RELEASE_COUNT));
//             ImGui::NextColumn();
//             ImGui::Text("%8d", mReleaseCount);
//         }
// 
//         virtual void OnPreUpdate() override
//         {
//             // if (*(THOverlay::singleton().mInGameInfo) && *(DWORD*)(RVA2(0x5B85EC))) {
//             //     SetPosRel(900.0f / 1280.0f, 500.0f / 960.0f);
//             //     SetSizeRel(340.0f / 1280.0f, 0.0f);
//             //     Open();
//             // } else {
//             //     Close();
//             // }
//         }
// 
//     public:
//     };

    class THAdvOptWnd : public Gui::PPGuiWnd {
        SINGLETON(THAdvOptWnd);
        
    public:

    private:

    private:
        void MasterDisableInit()
        {
        }
        void FpsInit()
        {
            // if (*(uint8_t*)0x4c12c9 == 3) {
            //     mOptCtx.fps_status = 1;
            // 
            //     DWORD oldProtect;
            //     VirtualProtect((void*)0x45acc1, 4, PAGE_EXECUTE_READWRITE, &oldProtect);
            //     *(double**)0x45acc1 = &mOptCtx.fps_dbl;
            //     VirtualProtect((void*)0x45acc1, 4, oldProtect, &oldProtect);
            // } else
            //     mOptCtx.fps_status = 0;
        }
        void FpsSet()
        {
            if (mOptCtx.fps_status == 1) {
                mOptCtx.fps_dbl = 1.0 / (double)mOptCtx.fps;
            }
        }
        void GameplayInit()
        {
            // th16_all_clear_bonus_1.Setup();
            // th16_all_clear_bonus_2.Setup();
            // th16_all_clear_bonus_3.Setup();
        }
        void GameplaySet()
        {
            // th16_all_clear_bonus_1.Toggle(mOptCtx.all_clear_bonus);
            // th16_all_clear_bonus_2.Toggle(mOptCtx.all_clear_bonus);
            // th16_all_clear_bonus_3.Toggle(mOptCtx.all_clear_bonus);
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
            SetTitle("AdvOptMenu");
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

            // if (BeginOptGroup<TH_GAME_SPEED>()) {
            //     if (GameFPSOpt(mOptCtx))
            //         FpsSet();
            //     EndOptGroup();
            // }
            if (BeginOptGroup<TH_GAMEPLAY>()) {
                DisableKeyOpt();
                // KeyHUDOpt();
                // InfLifeOpt();
                ImGui::SetNextItemWidth(180.0f);
                EndOptGroup();
            }
            ImGui::Checkbox("simple prac mode", &g_prac_mode);
            if (ImGui::DragInt("stage jump", &g_jump_stage, 1.0f, 1, 3))
                g_jump_stage = std::clamp(g_jump_stage, 1, 3);
            if (ImGui::DragInt("life", &g_life, 1.0f, 0, 7))
                g_life = std::clamp(g_life, 0, 7);
            if (ImGui::DragInt("bomb", &g_bomb, 1.0f, 0, 7))
                g_bomb = std::clamp(g_bomb, 0, 7);
            if (ImGui::DragInt("piv", &g_piv, 1000.0f, 0, 1000000))
                g_piv = std::clamp(g_piv, 0, 1000000);
            if (ImGui::DragInt("power", &g_power, 1.0f, 0, 400))
                g_power = std::clamp(g_power, 0, 400);
            if (ImGui::DragInt("hyper", &g_hyper, 10.0f, 0, 1000))
                g_hyper = std::clamp(g_hyper, 0, 1000);
            if (ImGui::DragInt("delta", &g_delta, 10.0f, 0, 5000))
                g_delta = std::clamp(g_delta, 0, 5000);
            static bool insult = false;
            static std::string ins = "";
            if (ImGui::Button("Insult 2un")){
                insult = true;
                ins += "SB 2un SB 2un SB 2un SB 2un SB 2un SB 2un SB 2un\n";
            }
            if (insult)
            {
                ImGui::TextWrapped(ins.c_str());
            }

            AboutOpt();
            ImGui::EndChild();
            ImGui::SetWindowFocus();
        }

        adv_opt_ctx mOptCtx;
    };
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
                ECLJump(ecl, 0x7c10, 0x7df8, 60, 90); // 0x7f14
                ECLJump(ecl, 0x5070, 0x50a4, 0, 0);
                break;
            case 3:
                ECLJump(ecl, 0x7c10, 0x7e94, 60, 90);
                ecl << pair { 0x60d4, 0 };
                break;
            case 4:
                ECLJump(ecl, 0x7c10, 0x7e94, 60, 90);
                ECLJump(ecl, 0x5160, 0x51d0, 0, 0);
                break;
            default:
                break;
            }
        } else if (stage == 2) {
            switch (portion) {
            case 1:
                break;
            case 2:
                ECLJump(ecl, 0x77c8, 0x79b0, 60, 90);
                ECLJump(ecl, 0x4070, 0x40f8, 0, 0);
                break;
            case 3:
                ECLJump(ecl, 0x77c8, 0x79b0, 60, 90);
                ECLJump(ecl, 0x4070, 0x412c, 0, 0);
                break;
            case 4:
                ECLJump(ecl, 0x77c8, 0x79b0, 60, 90);
                ECLJump(ecl, 0x4070, 0x4194, 0, 0);
                break;
            default:
                break;
            }
        } else if (stage == 3) {
            switch (portion) {
            case 1:
                break;
            case 2:
                ECLJump(ecl, 0x8648, 0x88bc, 60, 90); // 0x8960
                ECLJump(ecl, 0x5248, 0x527c, 0, 0);
                break;
            case 3:
                ECLJump(ecl, 0x8648, 0x88bc, 60, 90); // 0x8960
                ECLJump(ecl, 0x5248, 0x52b0, 0, 0);
                break;
            case 4:
                ECLJump(ecl, 0x8648, 0x88bc, 60, 90); // 0x8960
                ECLJump(ecl, 0x5248, 0x5318, 0, 0);
                break;
            case 5:
                ECLJump(ecl, 0x8648, 0x8960, 60, 90);
                break;
            case 6:
                ECLJump(ecl, 0x8648, 0x8960, 60, 90);
                ECLJump(ecl, 0x536c, 0x539c, 0, 0);
                break;
            default:
                break;
            }
        } else if (stage == 4) {
            switch (portion) {
            case 1:
                break;
            case 2:
                ECLJump(ecl, 0x9bb0, 0x9e24, 60, 90); // 0x9ec0
                ECLJump(ecl, 0x6244, 0x6278, 0, 0);
                break;
            case 3:
                ECLJump(ecl, 0x9bb0, 0x9e24, 60, 90); // 0x9ec0
                ECLJump(ecl, 0x6244, 0x62ac, 0, 0);
                break;
            case 4:
                ECLJump(ecl, 0x9bb0, 0x9ec0, 60, 90);
                break;
            case 5:
                ECLJump(ecl, 0x9bb0, 0x9ec0, 60, 90);
                ECLJump(ecl, 0x6314, 0x6348, 0, 0);
                break;
            case 6:
                if (thPracParam.phase == 1) {
                    ECLJump(ecl, 0x8B28, 0x8648, 0, 0);
                }
                ECLJump(ecl, 0x9bb0, 0x9ec0, 60, 90);
                ECLJump(ecl, 0x6314, 0x63a8, 0, 0);
                break;
            default:
                break;
            }
        } else if (stage == 5) {
            switch (portion) {
            case 1:
                break;
            case 2:
                ECLJump(ecl, 0x9070, 0x925c, 60, 90); // 0x92f8
                ECLJump(ecl, 0x4a94, 0x4ac8, 0, 0);
                break;
            case 3:
                ECLJump(ecl, 0x9070, 0x925c, 60, 90); // 0x92f8
                ECLJump(ecl, 0x4a94, 0x4afc, 0, 0);
                break;
            case 4:
                ECLJump(ecl, 0x9070, 0x925c, 60, 90); // 0x92f8
                ECLJump(ecl, 0x4a94, 0x4b30, 0, 0);
                break;
            case 5:
                ECLJump(ecl, 0x9070, 0x925c, 60, 90); // 0x92f8
                ECLJump(ecl, 0x4a94, 0x4b64, 0, 0);
                break;
            case 6:
                ECLJump(ecl, 0x9070, 0x92f8, 60, 90);
                ecl << pair { 0x7418, 0 };
                break;
            case 7:
                ECLJump(ecl, 0x9070, 0x92f8, 60, 90);
                ECLJump(ecl, 0x4bcc, 0x4c00, 0, 0);
                break;
            case 8:
                ECLJump(ecl, 0x9070, 0x92f8, 60, 90);
                ECLJump(ecl, 0x4bcc, 0x4c60, 0, 0);
                break;
            default:
                break;
            }
        } else if (stage == 6) {
            switch (portion) {
            case 1:
                break;
            case 2:
                ECLJump(ecl, 0x5698, 0x58ac, 60, 90);
                ECLJump(ecl, 0x34c0, 0x34f4, 0, 0);
                ecl << pair { 0x3aac, 0 };
                break;
            case 3:
                ECLJump(ecl, 0x5698, 0x58ac, 60, 90);
                ECLJump(ecl, 0x34c0, 0x3538, 0, 0);
                break;
            default:
                break;
            }
        } else if (stage == 7) {
            switch (portion) {
            case 1:
                break;
            case 2:
                ECLJump(ecl, 0x988c, 0x9b18, 60, 90); // 0x9c1c
                ECLJump(ecl, 0x614c, 0x6180, 0, 0);
                break;
            case 3:
                ECLJump(ecl, 0x988c, 0x9b18, 60, 90); // 0x9c1c
                ECLJump(ecl, 0x614c, 0x61b4, 0, 0);
                break;
            case 4:
                ECLJump(ecl, 0x988c, 0x9b18, 60, 90); // 0x9c1c
                ECLJump(ecl, 0x614c, 0x61e8, 0, 0);
                break;
            case 5:
                ECLJump(ecl, 0x988c, 0x9c1c, 60, 90);
                break;
            case 6:
                ECLJump(ecl, 0x988c, 0x9c1c, 60, 10);
                ECLJump(ecl, 0x623c, 0x6284, 0, 0);
                break;
            case 7:
                ECLJump(ecl, 0x988c, 0x9c1c, 60, 90);
                ECLJump(ecl, 0x623c, 0x62e0, 0, 0);
                break;
            case 8:
                ECLJump(ecl, 0x988c, 0x9c1c, 60, 90);
                ECLJump(ecl, 0x623c, 0x633c, 0, 0);
                break;
            default:
                break;
            }
        }
    }
    __declspec(noinline) void THPatch(ECLHelper& ecl, th_sections_t section)
    {
        switch (section) {
        case THPrac::TH20::TH20_ST1_MID1:
            // ECLJump(ecl, 0x7be8, 0x7e50, 60);
            break;
        default:
            break;
        }
    }
    __declspec(noinline) void THSectionPatch()
    {
        ECLHelper ecl;
        ecl.SetBaseAddr((void*)GetMemAddr(0x4a6dc0, 0x17c, 0xC));

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

    HOOKSET_DEFINE(THMainHook)
    EHOOK_DY(th20_jump_stage, 0x0012AB34)
    {
        if (g_prac_mode) {
            *(int*)(pCtx->Esp) = g_jump_stage;
        }
    }
    EHOOK_DY(th20_init_res, 0x00BCF34)
    {
        if (g_prac_mode){
            *(int*)RVA2(0x005B8728) = g_life;
            *(int*)RVA2(0x005B873C) = g_bomb;
            *(int*)RVA2(0x005B86B4) = g_piv;
            *(int*)RVA2(0x005B86A0) = g_power;
            *(int*)RVA2(0x005B86BC) = g_hyper;
            *(int*)RVA2(0x005B86CC) = g_delta;
        }
    }
    EHOOK_DY(th20_inf_lives, 0x0B90F7)
    {
        if ((*(THOverlay::singleton().mInfLives))) {
            if (!g_adv_igi_options.map_inf_life_to_no_continue) {
                *(DWORD*)(pCtx->Ebp - 0xC) = 0;
            } else {
                if (*(DWORD*)(RVA2(0x5B8728)) == 0)
                    *(DWORD*)(pCtx->Ebp - 0xC) = 0;
            }
        }
    }
    // EHOOK_DY(th20_everlasting_bgm, 0x45ed00)
    // {
    //     int32_t retn_addr = ((int32_t*)pCtx->Esp)[0];
    //     int32_t bgm_cmd = ((int32_t*)pCtx->Esp)[1];
    //     int32_t bgm_id = ((int32_t*)pCtx->Esp)[2];
    //     // 4th stack item = i32 call_addr
    // 
    //     bool el_switch;
    //     bool is_practice;
    //     bool result;
    // 
    //     el_switch = *(THOverlay::singleton().mElBgm) && !THGuiRep::singleton().mRepStatus && (thPracParam.mode == 1) && thPracParam.section;
    //     is_practice = (*((int32_t*)0x4a5bec) & 0x1);
    //     result = ElBgmTest<0x43c423, 0x42d6b9, 0x43f199, 0x4409c0, 0xffffffff>(
    //         el_switch, is_practice, retn_addr, bgm_cmd, bgm_id, 0xffffffff);
    // 
    //     if (result) {
    //         pCtx->Eip = 0x45ed93;
    //     }
    // }
    // EHOOK_DY(th20_param_reset, 0x44b610)
    // {
    //     thPracParam.Reset();
    //     thSubSeasonB = -1;
    // }
    // EHOOK_DY(th20_prac_menu_1, 0x450f60)
    // {
    //     THGuiPrac::singleton().State(1);
    // }
    // EHOOK_DY(th20_prac_menu_2, 0x450f83)
    // {
    //     THGuiPrac::singleton().State(2);
    // }
    // EHOOK_DY(th20_prac_menu_3, 0x4512cc)
    // {
    //     THGuiPrac::singleton().State(3);
    // }
    // EHOOK_DY(th20_prac_menu_4, 0x45136d)
    // {
    //     THGuiPrac::singleton().State(4);
    // }
    // PATCH_DY(th20_prac_menu_enter_1, 0x451044, "\xeb", 1);
    // EHOOK_DY(th20_prac_menu_enter_2, 0x451327)
    // {
    //     // Change sub-season to dog days if playing extra
    //     if (thPracParam.stage == 6) {
    //         thSubSeasonB = *((int32_t*)0x4a57ac);
    //         *((int32_t*)0x4a57ac) = 4;
    //     }
    // 
    //     pCtx->Ecx = thPracParam.stage;
    // }
    // EHOOK_DY(th20_disable_prac_menu_1, 0x4514d1)
    // {
    //     pCtx->Eip = 0x45150e;
    // }
    // EHOOK_DY(th20_patch_main, 0x42d1ec)
    // {
    //     if (thPracParam.mode == 1) {
    //         *(int32_t*)(0x4a57b0) = (int32_t)(thPracParam.score / 10);
    //         *(int32_t*)(0x4a57f4) = thPracParam.life;
    //         *(int32_t*)(0x4a5800) = thPracParam.bomb;
    //         *(int32_t*)(0x4a5804) = thPracParam.bomb_fragment;
    //         *(int32_t*)(0x4a57e4) = thPracParam.power;
    //         *(int32_t*)(0x4a57d8) = thPracParam.value * 100;
    //         *(int32_t*)(0x4a57c0) = thPracParam.graze;
    //         THSectionPatch();
    //     }
    //     thPracParam._playLock = true;
    // }
    // EHOOK_DY(th20_bgm, 0x42de8c)
    // {
    //     if (THBGMTest()) {
    //         PushHelper32(pCtx, 1);
    //         pCtx->Eip = 0x42de8e;
    //     }
    // }
    // EHOOK_DY(th20_rep_save, 0x448be4)
    // {
    //     char* repName = (char*)(pCtx->Esp + 0x38);
    //     if (thPracParam.mode == 1) {
    //         THSaveReplay(repName);
    //     } else if (thPracParam.mode == 2) {
    //         if (thPracParam.season_gauge != 3 || thPracParam.phase || thPracParam.bug_fix)
    //             THSaveReplay(repName);
    //     }
    // }
    // EHOOK_DY(th20_rep_menu_1, 0x4518a6)
    // {
    //     THGuiRep::singleton().State(1);
    // }
    // EHOOK_DY(th20_rep_menu_2, 0x4519c6)
    // {
    //     THGuiRep::singleton().State(2);
    // }
    // EHOOK_DY(th20_rep_menu_3, 0x451b86)
    // {
    //     THGuiRep::singleton().State(3);
    // }

    EHOOK_DY(th20_update, 0x012824)
    {
        GameGuiBegin(IMPL_WIN32_DX9, !THAdvOptWnd::singleton().IsOpen());
        // GameGuiBegin(IMPL_WIN32_DX9, true);
    
        // Gui components update
        // THGuiPrac::singleton().Update();
        // THGuiRep::singleton().Update();
        THOverlay::singleton().Update();
        // TH16InGameInfo::singleton().Update();
        // in case boss movedown do not disabled when playing normal games
        // {
        //     if (THAdvOptWnd::singleton().forceBossMoveDown) {
        //         auto p = ImGui::GetOverlayDrawList();
        //         auto sz = ImGui::CalcTextSize(S(TH_BOSS_FORCE_MOVE_DOWN));
        //         p->AddRectFilled({ 120.0f, 0.0f }, { sz.x + 120.0f, sz.y }, 0xFFCCCCCC);
        //         p->AddText({ 120.0f, 0.0f }, 0xFFFF0000, S(TH_BOSS_FORCE_MOVE_DOWN));
        //     }
        // }
        // if (g_adv_igi_options.show_keyboard_monitor && *(DWORD*)(0x004A6EF8))
        //     KeysHUD(16, { 1280.0f, 0.0f }, { 840.0f, 0.0f }, g_adv_igi_options.keyboard_style);
        bool drawCursor = THAdvOptWnd::StaticUpdate();
        // bool drawCursor = false;
        GameGuiEnd(drawCursor);
    }
    // EHOOK_DY(th16_player_state, 0x442560)
    // {
    //     if (g_adv_igi_options.show_keyboard_monitor)
    //         RecordKey(16, *(DWORD*)(0x4A52C8));
    // }
    EHOOK_DY(th20_render, 0x0129C6)
    {
        GameGuiRender(IMPL_WIN32_DX9);
    }
    HOOKSET_ENDDEF()
    // HOOKSET_DEFINE(THInGameInfo)
    // EHOOK_DY(th16_game_start, 0x42E5AE) // gamestart-bomb set
    // {
    //     TH16InGameInfo::singleton().mBombCount = 0;
    //     TH16InGameInfo::singleton().mMissCount = 0;
    //     TH16InGameInfo::singleton().mReleaseCount = 0;
    // }
    // EHOOK_DY(th16_bomb_dec, 0x40DB9C) // bomb dec
    // {
    //     TH16InGameInfo::singleton().mBombCount++;
    // }
    // EHOOK_DY(th16_life_dec, 0x443D3A) // life dec
    // {
    //     TH16InGameInfo::singleton().mMissCount++;
    // }
    // HOOKSET_ENDDEF()
    HOOKSET_DEFINE(THInitHook)

    static __declspec(noinline) void THGuiCreate()
    {
        // Init
        GameGuiInit(IMPL_WIN32_DX9, RVA2(0x5C2D58), RVA2(0x5B47D8),
            Gui::INGAGME_INPUT_GEN2, GetMemContent(RVA2(0x5B6918)) + 0x30, GetMemContent(RVA2(0x5B6918)) + 0x40, 0,
            -2, *(float*)RVA2(0x5B6898), 0.0f);

        // Gui components creation
        // THGuiPrac::singleton();
        // THGuiRep::singleton();
        THOverlay::singleton();
        // TH16InGameInfo::singleton();
        
        // Hooks
        THMainHook::singleton().EnableAllHooks();
        // THInGameInfo::singleton().EnableAllHooks();
        Gui::ImplDX9NewFrame();
        //  Reset thPracParam
        thPracParam.Reset();
    }
    static __declspec(noinline) void THInitHookDisable()
    {
        auto& s = THInitHook::singleton();
        s.th20_gui_init_1.Disable();
        s.th20_gui_init_2.Disable();
    }
    //PATCH_DY(th20_disable_demo, 0x44afb0, "\xff\xff\xff\x7f", 4);
    // EHOOK_DY(th20_disable_mutex, 0x41C3C2)
    // {
    //     pCtx->Eip = RVA2(0x41C3DD);
    // }
    PATCH_DY(th20_startup_1, 0x121A0C, "\xeb", 1);
    PATCH_DY(th20_startup_2, 0x120BF1, "\xeb", 1);
    EHOOK_DY(th20_gui_init_1, 0x12967E)
    {
        THGuiCreate();
        THInitHookDisable();
    }
    EHOOK_DY(th20_gui_init_2, 0x01F348)
    {
        THGuiCreate();
        THInitHookDisable();
    }
    HOOKSET_ENDDEF()
}

int(__stdcall* g_realMultiByteToWideChar)(UINT CodePage, DWORD dwFlags, LPCCH lpMultiByteStr, int cbMultiByte, LPWSTR lpWideCharStr, int cchWideChar);
int __stdcall MultiByteToWideChar_Changed(UINT CodePage, DWORD dwFlags, LPCCH lpMultiByteStr, int cbMultiByte, LPWSTR lpWideCharStr, int cchWideChar)
{
    return g_realMultiByteToWideChar(CP_ACP, dwFlags, lpMultiByteStr, cbMultiByte, lpWideCharStr, cchWideChar);
}

int(__stdcall* g_reaWideCharToMultiByte)(UINT CodePage, DWORD dwFlags, LPCWCH lpWideCharStr, int cchWideChar, LPSTR lpMultiByteStr, int cbMultiByte, LPCCH lpDefaultChar, LPBOOL lpUsedDefaultChar);
int __stdcall WideCharToMultiByte_Changed(UINT CodePage, DWORD dwFlags, LPCWCH lpWideCharStr, int cchWideChar, LPSTR lpMultiByteStr, int cbMultiByte, LPCCH lpDefaultChar, LPBOOL lpUsedDefaultChar)
{
    return g_reaWideCharToMultiByte(CP_ACP, dwFlags, lpWideCharStr, cchWideChar, lpMultiByteStr, cbMultiByte,lpDefaultChar,lpUsedDefaultChar);
}

void TH20Init()
{
    ingame_image_base = (uintptr_t)GetModuleHandleW(NULL);
    
    // 2un used shift-jis encoding for this 2 functions, which can cause problem in no-japanese environment with a no-ascii username...
    
    // LPVOID pTarget1;
    // MH_Initialize();
    // if (MH_OK == MH_CreateHookApiEx(L"kernel32.dll", "MultiByteToWideChar", MultiByteToWideChar_Changed, (void**)&g_realMultiByteToWideChar, &pTarget1)){
    //     // MessageBoxA(NULL, std::format("{:x}", (DWORD)pTarget1).c_str(), "", MB_OK);
    //     MH_EnableHook(pTarget1);
    // }
    // LPVOID pTarget2;
    // if (MH_OK == MH_CreateHookApiEx(L"kernel32.dll", "WideCharToMultiByte", WideCharToMultiByte_Changed, (void**)&g_reaWideCharToMultiByte, &pTarget2))
    // {
    //     // MessageBoxA(NULL, std::format("{:x}", (DWORD)pTarget2).c_str(), "", MB_OK);
    //     MH_EnableHook(pTarget2);
    // }

    TH20::THInitHook::singleton().EnableAllHooks();

    // TryKeepUpRefreshRate((void*)0x45b8da, (void*)0x45b6ad);
}
}
