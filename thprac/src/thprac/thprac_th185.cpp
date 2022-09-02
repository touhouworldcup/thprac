#include "thprac_utils.h"
#include <queue>

namespace THPrac {
namespace TH185 {
    enum addrs {
        BLACK_MARKET_PTR = 0x4d7ac4,
        CARD_DESC_LIST = 0x4ca370
    };

    __declspec(noinline) void AddCard(uint32_t cardId)
    {
        auto real_AddCard = (void(__thiscall*)(uint32_t, uint32_t, uint32_t))0x414F20;
        if (cardId < 85)
            real_AddCard(*(uint32_t*)0x4d7ab8, cardId, 2);
    }

    struct THPracParam {
        int32_t mode;
        uint32_t stage;

        int32_t bulletMoney;
        int32_t funds;
        int32_t life;

        std::vector<unsigned int> warp;
        std::queue<unsigned int> force_wave;
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
            mMuteki();
            mInfLives();
            mInfBMoney();
            mTimeLock();
            mZeroCD();
            mWholesale();
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
            new HookCtx(0x4635a5, "\x01", 1) } };
        Gui::GuiHotKey mInfLives { TH_INFLIVES, "F2", VK_F2, {
            new HookCtx(0x40aec3, "\x66\x0f\x1f\x44\x00\x00", 6),
            new HookCtx(0x463281, "\x00", 1) } };
        Gui::GuiHotKey mInfBMoney { TH185_INF_BMONEY, "F3", VK_F3, {
            new HookCtx(0x40ed5f, "\x66\x2e\x0f\x1f\x84\x00\x00\x00\x00\x00\x66\x2e\x0f\x1f\x84\x00\x00\x00\x00\x00", 20),
            new HookCtx(0x41ee2d, "\x66\x0f\x1f\x44\x00\x00", 6) } };
        Gui::GuiHotKey mTimeLock { TH_TIMELOCK, "F4", VK_F4, {
            new HookCtx(0x434c85, "\x66\x0f\x1f\x44\x00\x00", 6),
            new HookCtx(0x436E38, "\x0f\x1f\x84\x00\x00\x00\x00\x00\x0f\x1f\x84\x00\x00\x00\x00\x00\x0f\x1f\x84\x00\x00\x00\x00\x00", 24) } };
        Gui::GuiHotKey mZeroCD { TH18_ZERO_CD, "F5", VK_F5, {
            new HookCtx(0x462146, [](PCONTEXT pCtx) {
                struct Timer {
                    int32_t prev;
                    int32_t cur;
                    float cur_f;
                    void* unused;
                    uint32_t control;
                };
                Timer* timer = (Timer*)(pCtx->Ecx + 0x34);
                *timer = { -1, 0, 0, 0, 0 };
            }) } };
        Gui::GuiHotKey mWholesale { "Wholesale", "F6", VK_F6, {
            new HookCtx(0x41da02, [](PCONTEXT pCtx) {
                uint8_t wholesale[] = { 0x53, 0x00, 0x00, 0x00, 0xF8, 0xA3, 0x4C, 0x00, 0x3C, 0xA4, 0x4C, 0x00, 0x80, 0xA4, 0x4C, 0x00, 0xC4, 0xA4, 0x4C, 0x00, 0x08, 0xA5, 0x4C, 0x00, 0x4C, 0xA5, 0x4C, 0x00, 0x90, 0xA5, 0x4C, 0x00, 0xD4, 0xA5, 0x4C, 0x00, 0x18, 0xA6, 0x4C, 0x00, 0x5C, 0xA6, 0x4C, 0x00, 0xA0, 0xA6, 0x4C, 0x00, 0xE4, 0xA6, 0x4C, 0x00, 0x28, 0xA7, 0x4C, 0x00, 0x6C, 0xA7, 0x4C, 0x00, 0xB0, 0xA7, 0x4C, 0x00, 0xF4, 0xA7, 0x4C, 0x00, 0x38, 0xA8, 0x4C, 0x00, 0x7C, 0xA8, 0x4C, 0x00, 0xC0, 0xA8, 0x4C, 0x00, 0x04, 0xA9, 0x4C, 0x00, 0x48, 0xA9, 0x4C, 0x00, 0x8C, 0xA9, 0x4C, 0x00, 0xD0, 0xA9, 0x4C, 0x00, 0x14, 0xAA, 0x4C, 0x00, 0x58, 0xAA, 0x4C, 0x00, 0x9C, 0xAA, 0x4C, 0x00, 0xE0, 0xAA, 0x4C, 0x00, 0x24, 0xAB, 0x4C, 0x00, 0x68, 0xAB, 0x4C, 0x00, 0xAC, 0xAB, 0x4C, 0x00, 0xF0, 0xAB, 0x4C, 0x00, 0x34, 0xAC, 0x4C, 0x00, 0x78, 0xAC, 0x4C, 0x00, 0xBC, 0xAC, 0x4C, 0x00, 0x00, 0xAD, 0x4C, 0x00, 0x44, 0xAD, 0x4C, 0x00, 0x88, 0xAD, 0x4C, 0x00, 0xCC, 0xAD, 0x4C, 0x00, 0x10, 0xAE, 0x4C, 0x00, 0x54, 0xAE, 0x4C, 0x00, 0x98, 0xAE, 0x4C, 0x00, 0xDC, 0xAE, 0x4C, 0x00, 0x20, 0xAF, 0x4C, 0x00, 0x64, 0xAF, 0x4C, 0x00, 0xA8, 0xAF, 0x4C, 0x00, 0xEC, 0xAF, 0x4C, 0x00, 0x30, 0xB0, 0x4C, 0x00, 0x74, 0xB0, 0x4C, 0x00, 0xB8, 0xB0, 0x4C, 0x00, 0xFC, 0xB0, 0x4C, 0x00, 0x40, 0xB1, 0x4C, 0x00, 0x84, 0xB1, 0x4C, 0x00, 0xC8, 0xB1, 0x4C, 0x00, 0x0C, 0xB2, 0x4C, 0x00, 0x50, 0xB2, 0x4C, 0x00, 0x94, 0xB2, 0x4C, 0x00, 0xD8, 0xB2, 0x4C, 0x00, 0x1C, 0xB3, 0x4C, 0x00, 0x60, 0xB3, 0x4C, 0x00, 0xA4, 0xB3, 0x4C, 0x00, 0xE8, 0xB3, 0x4C, 0x00, 0x2C, 0xB4, 0x4C, 0x00, 0x70, 0xB4, 0x4C, 0x00, 0xB4, 0xB4, 0x4C, 0x00, 0xF8, 0xB4, 0x4C, 0x00, 0x3C, 0xB5, 0x4C, 0x00, 0x80, 0xB5, 0x4C, 0x00, 0xC4, 0xB5, 0x4C, 0x00, 0x08, 0xB6, 0x4C, 0x00, 0x4C, 0xB6, 0x4C, 0x00, 0x90, 0xB6, 0x4C, 0x00, 0xD4, 0xB6, 0x4C, 0x00, 0x18, 0xB7, 0x4C, 0x00, 0x5C, 0xB7, 0x4C, 0x00, 0xA0, 0xB7, 0x4C, 0x00, 0xE4, 0xB7, 0x4C, 0x00, 0x28, 0xB8, 0x4C, 0x00, 0x6C, 0xB8, 0x4C, 0x00, 0xB0, 0xB8, 0x4C, 0x00, 0xF4, 0xB8, 0x4C, 0x00, 0x38, 0xB9, 0x4C, 0x00, 0x7C, 0xB9, 0x4C, 0x00, 0xC0, 0xB9, 0x4C, 0x00 };
                memcpy((void*)(pCtx->Esi + 0xA40), wholesale, sizeof(wholesale));
            }) } };
    };

    // If a mod somehow changes the amount of stages there are
    // and increases it, I want to account for that
    std::vector<stage_warps_t> stages = {};

    class THGuiPrac : public Gui::GameGuiWnd {
        THGuiPrac() noexcept
        {
            *mMode = 1;

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
                mStage = GetMemContent(0x4d7c68, 0xfc);
                SetFade(0.8f, 0.1f);
                Open();
                thPracParam = {};
                break;
            case 1:
                SetFade(0.8f, 0.1f);
                Close();

                // Fill Param
                thPracParam.mode = *mMode;
                thPracParam.warp = mWarp;
                thPracParam.stage = mStage;
                thPracParam.life = *mLife;
                thPracParam.funds = *mFunds;
                thPracParam.bulletMoney = *mBulletMoney;
                thPracParam.force_wave = {};

                for (auto& w : mForceWave) {
                    if (w)
                        thPracParam.force_wave.push(w + 29);
                    else
                        break;
                }

                break;
            case 2:
                Close();
                //*mNavFocus = 0;
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

        const char* waves[83] = {
            "None",
            "Wave01t",
            "Wave02t",
            "Wave03t",
            "Wave01",
            "Wave02",
            "Wave03",
            "Wave04",
            "Wave05",
            "Wave06",
            "Wave07",
            "Wave08",
            "Wave09",
            "Wave10",
            "Wave11",
            "Wave12",
            "Wave13",
            "Wave14",
            "Wave15",
            "Wave16",
            "Wave17",
            "Wave18",
            "Wave19",
            "Wave20",
            "Wave21",
            "Wave22",
            "Wave23",
            "Wave24",
            "Wave25",
            "Wave26",
            "Wave27",
            "Wave28",
            "Wave29",
            "Wave30",
            "Wave31",
            "Wave32",
            "Wave33",
            "Wave34",
            "Wave35",
            "Wave36",
            "Wave37",
            "Wave38",
            "Wave39",
            "Wave40",
            "Wave41",
            "Wave42",
            "Wave43",
            "Wave44",
            "Wave45",
            "Wave46",
            "Wave47",
            "Wave48",
            "Wave49",
            "Wave50",
            "Wave51",
            "Wave52",
            "Wave53",
            "Wave54",
            "Wave55",
            "Wave56",
            "Wave57",
            "Wave58",
            "Wave59",
            "Wave60",
            "Wave61",
            "Wave62",
            "Wave63",
            "Wave64",
            "Wave65",
            "Wave66",
            "Wave67",
            "Wave68",
            "Wave69",
            "Wave70",
            "Wave71",
            "Wave72",
            "Wave73",
            "Wave74",
            "Wave75",
            "Wave76",
            "Wave77",
            "Wave78",
            "Wave79"
        };

        void ForceWave(size_t level)
        {
            if (mForceWave.size() <= level)
                mForceWave.resize(level + 1);

            if (ImGui::BeginCombo("Force Wave", waves[mForceWave[level]])) {
                for (size_t i = 0; i < sizeof(waves) / sizeof(const char*); i++) {
                    ImGui::PushID(i);

                    bool item_selected = (i == mForceWave[level]);

                    if (ImGui::Selectable(waves[i], &item_selected))
                        mForceWave[level] = i;

                    if (item_selected)
                        ImGui::SetItemDefaultFocus();

                    ImGui::PopID();
                }
                ImGui::EndCombo();
            }

            if (ImGui::IsItemFocused()) {
                if (Gui::InGameInputGet(VK_LEFT) && mForceWave[level] > 0) {
                    mForceWave[level]--;
                }
                if (Gui::InGameInputGet(VK_RIGHT) && mForceWave[level] + 1 < sizeof(waves) / sizeof(const char*)) {
                    mForceWave[level]++;
                }
            }

            if (mForceWave[level]) {
                ImGui::PushID(++level);
                ForceWave(level);
                ImGui::PopID();
            }
        }

        void PracticeMenu()
        {
            mMode();
            if (*mMode == 1) {
                if (stages.size() > mStage)
                    StageWarpsRender(stages[mStage], mWarp, 0);
                
                mLife();
                mFunds();
                mBulletMoney();

                ForceWave(0);
            }
            // mNavFocus();
        }

        Gui::GuiCombo mMode { TH_MODE, TH_MODE_SELECT };
        size_t mStage;
        Gui::GuiDrag<int32_t, ImGuiDataType_S32> mBulletMoney { TH185_BULLET_MONEY, 0, INT_MAX };
        Gui::GuiSlider<int, ImGuiDataType_S32> mLife { TH_LIFE, 0, 9 };
        Gui::GuiDrag<int, ImGuiDataType_S32> mFunds { TH18_FUNDS, 0, 999990, 1, 100000 };
        // Gui::GuiNavFocus mNavFocus { TH185_MARKET, TH_MODE, TH_WARP };
        std::vector<unsigned int> mWarp;
        std::vector<unsigned int> mForceWave = { 0 };

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

    PATCH_ST(th185_prac_disable_arrows, 0x46d39f, "\xe9\xcd\x00\x00\x00", 5);
    EHOOK_G1(th185_prac_leave, 0x46d481)
    {
        th185_prac_leave::GetHook().Disable();
        THGuiPrac::singleton().State(2);
        th185_prac_disable_arrows.Disable();
        pCtx->Eip = 0x46d9c0;
    }

    HOOKSET_DEFINE(THMainHook)
    EHOOK_DY(th185_gui_update, 0x4013dd)
    {
        GameGuiBegin(IMPL_WIN32_DX9);

        // Gui components update
        THOverlay::singleton().Update();
        THGuiPrac::singleton().Update();

        GameGuiEnd(UpdateAdvOptWindow());
    }
    EHOOK_DY(th185_gui_render, 0x4014fa)
    {
        GameGuiRender(IMPL_WIN32_DX9);
    }

    EHOOK_DY(th185_patch_main, 0x448fb2)
    {
        // 0x4d1024 = phase

        if (thPracParam.mode) {
            *(int32_t*)(0x4d1070) = thPracParam.bulletMoney;
            *(int32_t*)(0x4d1074) = thPracParam.bulletMoney;
            *(int32_t*)(0x4d106c) = thPracParam.funds;
            *(int32_t*)(0x4d10bc) = thPracParam.life;
            if (stages.size() > thPracParam.stage)
                StageWarpsApply(stages[thPracParam.stage], thPracParam.warp, ThModern_ECLGetSub, GetMemContent(0x004d7af4, 0x4f34, 0x10c), 0);
        }
    }
    EHOOK_DY(th185_force_wave, 0x43d156)
    {
        if (thPracParam.force_wave.size()) {
            pCtx->Esi = thPracParam.force_wave.front();
            thPracParam.force_wave.pop();
        }
    }
    EHOOK_DY(th185_prac_confirm, 0x46d523)
    {
        auto& p = THGuiPrac::singleton();
        if (p.IsOpen()) {
            p.State(1);
            th185_prac_disable_arrows.Disable();
            th185_prac_leave::GetHook().Disable();
        } else {
            p.State(0);
            th185_prac_disable_arrows.Enable();
            th185_prac_leave::GetHook().Enable();
            pCtx->Eip = 0x46d9c0;
        }
    }
    PATCH_DY(th185_disable_topmost, 0x4747ac, "\x00", 1);
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

        th185_prac_disable_arrows.Setup();

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

    EHOOK_DY(th185_gui_init_1, 0x46ce39)
    {
        THGuiCreate();
        THInitHookDisable();
    }
    EHOOK_DY(th185_gui_init_2, 0x476580)
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