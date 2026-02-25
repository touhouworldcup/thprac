#include "thprac_games.h"
#include "thprac_utils.h"
#include <queue>
#include <string>

extern void TH185Warps0(THPrac::stage_warps_t& out) noexcept;
extern void TH185Warps1(THPrac::stage_warps_t& out) noexcept;
extern void TH185Warps2(THPrac::stage_warps_t& out) noexcept;
extern void TH185Warps3(THPrac::stage_warps_t& out) noexcept;
extern void TH185Warps4(THPrac::stage_warps_t& out) noexcept;
extern void TH185Warps5(THPrac::stage_warps_t& out) noexcept;
extern void TH185Warps6(THPrac::stage_warps_t& out) noexcept;
extern void TH185Warps7(THPrac::stage_warps_t& out) noexcept;
extern void TH185Warps8(THPrac::stage_warps_t& out) noexcept;
extern void TH185Warps9(THPrac::stage_warps_t& out) noexcept;

namespace THPrac {
namespace TH185 {
    struct card_names_t {
        static constexpr size_t count = 86;
        Gui::locale_t locale;
        char strings[count][50];
        const char* ptrs[count];

        constexpr card_names_t() : locale(Gui::locale_t::LOCALE_NONE) {
            for (int i = 0; i < count; ++i) {
                ptrs[i] = strings[i];
            }
        }
    } card_names;

    enum addrs {
        BLACK_MARKET_PTR = 0x4d7ac4,
        CARD_DESC_LIST = 0x4ca370,
        ENEMY_MANAGER = 0x4d7af4,
        GAME_THREAD_PTR = 0x4d7b0c
    };

    bool isItemEnabled = false;
    bool wave_forced = false;

    __declspec(noinline) void AddCard(uint32_t cardId)
    {
        if (cardId < 85) {
            asm_call<0x414F20, Thiscall>(*(uint32_t*)0x4d7ab8, cardId, 2);
        }
    }

    struct THPracParam {
        int32_t mode;

        int32_t bulletMoney;
        int32_t funds;
        int32_t life;
        int32_t difficulty;

        std::vector<unsigned int> warp;
        std::vector<unsigned int> additional_cards;
    };
    THPracParam thPracParam {};
    stage_warps_t warps = {};

    bool StageWarpsLoad(size_t stage, stage_warps_t& out) noexcept {
        out = {
            .label = S(TH_PROGRESS),
            .type = stage_warps_t::TYPE_SLIDER,
        };
        switch (stage) {
        case 0: ::TH185Warps0(out); return true;
        case 1: ::TH185Warps1(out); return true;
        case 2: ::TH185Warps2(out); return true;
        case 3: ::TH185Warps3(out); return true;
        case 4: ::TH185Warps4(out); return true;
        case 5: ::TH185Warps5(out); return true;
        case 6: ::TH185Warps6(out); return true;
        case 7: ::TH185Warps7(out); return true;
        case 8: ::TH185Warps8(out); return true;
        }
        return true;
    }

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
        Gui::GuiHotKeyChord mMenu { "ModMenuToggle", "BACKSPACE", Gui::GetBackspaceMenuChord() };

        HOTKEY_DEFINE(mMuteki, TH_MUTEKI, "F1", VK_F1)
        PATCH_HK(0x4635a5, "01")
        HOTKEY_ENDDEF();
        
        HOTKEY_DEFINE(mInfLives, TH_INFLIVES, "F2", VK_F2)
        PATCH_HK(0x40aec3, "660f1f440000"),
        PATCH_HK(0x463281, "00")
        HOTKEY_ENDDEF();
        
        HOTKEY_DEFINE(mInfBMoney, TH185_INF_BMONEY, "F3", VK_F3)
        PATCH_HK(0x40ed5f, "662e0f1f840000000000662e0f1f840000000000"),
        PATCH_HK(0x41ee2d, "660f1f440000")
        HOTKEY_ENDDEF();
        
        HOTKEY_DEFINE(mTimeLock, TH_TIMELOCK, "F4", VK_F4)
        PATCH_HK(0x434c85, "660f1f440000"),
        PATCH_HK(0x436E38, "0f1f8400000000000f1f8400000000000f1f840000000000")
        HOTKEY_ENDDEF();

        HOTKEY_DEFINE(mZeroCD, TH18_ZERO_CD, "F5", VK_F5)
        EHOOK_HK(0x462146, 3, {
            struct Timer {
                int32_t prev;
                int32_t cur;
                float cur_f;
                void* unused;
                uint32_t control;
            };
            Timer* timer = (Timer*)(pCtx->Ecx + 0x34);
            *timer = { -1, 0, 0, 0, 0 };
        })
        HOTKEY_ENDDEF();

        HOTKEY_DEFINE(mWholesale, TH185_WHOLESALE, "F6", VK_F6)
        EHOOK_HK(0x41da02, 6, {
            uint8_t wholesale[] = { 0x53, 0x00, 0x00, 0x00, 0xF8, 0xA3, 0x4C, 0x00, 0x3C, 0xA4, 0x4C, 0x00, 0x80, 0xA4, 0x4C, 0x00, 0xC4, 0xA4, 0x4C, 0x00, 0x08, 0xA5, 0x4C, 0x00, 0x4C, 0xA5, 0x4C, 0x00, 0x90, 0xA5, 0x4C, 0x00, 0xD4, 0xA5, 0x4C, 0x00, 0x18, 0xA6, 0x4C, 0x00, 0x5C, 0xA6, 0x4C, 0x00, 0xA0, 0xA6, 0x4C, 0x00, 0xE4, 0xA6, 0x4C, 0x00, 0x28, 0xA7, 0x4C, 0x00, 0x6C, 0xA7, 0x4C, 0x00, 0xB0, 0xA7, 0x4C, 0x00, 0xF4, 0xA7, 0x4C, 0x00, 0x38, 0xA8, 0x4C, 0x00, 0x7C, 0xA8, 0x4C, 0x00, 0xC0, 0xA8, 0x4C, 0x00, 0x04, 0xA9, 0x4C, 0x00, 0x48, 0xA9, 0x4C, 0x00, 0x8C, 0xA9, 0x4C, 0x00, 0xD0, 0xA9, 0x4C, 0x00, 0x14, 0xAA, 0x4C, 0x00, 0x58, 0xAA, 0x4C, 0x00, 0x9C, 0xAA, 0x4C, 0x00, 0xE0, 0xAA, 0x4C, 0x00, 0x24, 0xAB, 0x4C, 0x00, 0x68, 0xAB, 0x4C, 0x00, 0xAC, 0xAB, 0x4C, 0x00, 0xF0, 0xAB, 0x4C, 0x00, 0x34, 0xAC, 0x4C, 0x00, 0x78, 0xAC, 0x4C, 0x00, 0xBC, 0xAC, 0x4C, 0x00, 0x00, 0xAD, 0x4C, 0x00, 0x44, 0xAD, 0x4C, 0x00, 0x88, 0xAD, 0x4C, 0x00, 0xCC, 0xAD, 0x4C, 0x00, 0x10, 0xAE, 0x4C, 0x00, 0x54, 0xAE, 0x4C, 0x00, 0x98, 0xAE, 0x4C, 0x00, 0xDC, 0xAE, 0x4C, 0x00, 0x20, 0xAF, 0x4C, 0x00, 0x64, 0xAF, 0x4C, 0x00, 0xA8, 0xAF, 0x4C, 0x00, 0xEC, 0xAF, 0x4C, 0x00, 0x30, 0xB0, 0x4C, 0x00, 0x74, 0xB0, 0x4C, 0x00, 0xB8, 0xB0, 0x4C, 0x00, 0xFC, 0xB0, 0x4C, 0x00, 0x40, 0xB1, 0x4C, 0x00, 0x84, 0xB1, 0x4C, 0x00, 0xC8, 0xB1, 0x4C, 0x00, 0x0C, 0xB2, 0x4C, 0x00, 0x50, 0xB2, 0x4C, 0x00, 0x94, 0xB2, 0x4C, 0x00, 0xD8, 0xB2, 0x4C, 0x00, 0x1C, 0xB3, 0x4C, 0x00, 0x60, 0xB3, 0x4C, 0x00, 0xA4, 0xB3, 0x4C, 0x00, 0xE8, 0xB3, 0x4C, 0x00, 0x2C, 0xB4, 0x4C, 0x00, 0x70, 0xB4, 0x4C, 0x00, 0xB4, 0xB4, 0x4C, 0x00, 0xF8, 0xB4, 0x4C, 0x00, 0x3C, 0xB5, 0x4C, 0x00, 0x80, 0xB5, 0x4C, 0x00, 0xC4, 0xB5, 0x4C, 0x00, 0x08, 0xB6, 0x4C, 0x00, 0x4C, 0xB6, 0x4C, 0x00, 0x90, 0xB6, 0x4C, 0x00, 0xD4, 0xB6, 0x4C, 0x00, 0x18, 0xB7, 0x4C, 0x00, 0x5C, 0xB7, 0x4C, 0x00, 0xA0, 0xB7, 0x4C, 0x00, 0xE4, 0xB7, 0x4C, 0x00, 0x28, 0xB8, 0x4C, 0x00, 0x6C, 0xB8, 0x4C, 0x00, 0xB0, 0xB8, 0x4C, 0x00, 0xF4, 0xB8, 0x4C, 0x00, 0x38, 0xB9, 0x4C, 0x00, 0x7C, 0xB9, 0x4C, 0x00, 0xC0, 0xB9, 0x4C, 0x00 };
            memcpy((void*)(pCtx->Esi + 0xA40), wholesale, sizeof(wholesale));
        })
        HOTKEY_ENDDEF();
    };

    EHOOK_ST(th185_replaymanager_on_tick_cutoff, 0x468764, 1, {
        pCtx->Eax = 3;
    });

    struct THWaveSelect : public Gui::GameGuiWnd {
        SINGLETON(THWaveSelect);

        THWaveSelect() noexcept
        {
            SetWndFlag(
                ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav | 0);

            SetFade(0.5f, 0.5f);
            SetSizeRel(0.3f, 0.6f);
            SetPosRel(0.35f, 0.2f);
            SetTitle("THWaveSelect");

            th185_replaymanager_on_tick_cutoff.Setup();
        }

        virtual void OnContentUpdate() override
        {
            ImGui::TextUnformatted(S(TH185_FORCE_WAVE));
            ImGui::Separator();

            const ImVec4 green = { 0, 1, 0, 1 };

            if (selectedWave == -1) {
                ImGui::TextColored(green, "[%s]", S(TH185_NONE_RANDOM));
            } else {
                ImGui::TextUnformatted(S(TH185_NONE_RANDOM));
            }

            for (size_t i = 0; i < waveOpts.size(); i++) {
                auto [id, name] = waveOpts[i];
                if (selectedWave == i) {
                    ImGui::TextColored(green, "[%s]", name);
                } else {
                    ImGui::TextUnformatted(name);
                }
            }

            if (Gui::InGameInputGet(VK_DOWN)) {
                if (++selectedWave >= waveOpts.size()) {
                    selectedWave = SIZE_MAX;
                }
            }
            if (Gui::InGameInputGet(VK_UP)) {
                if (selectedWave == SIZE_MAX) {
                    selectedWave = waveOpts.size() - 1;
                } else {
                    --selectedWave;
                }
            }
            if (Gui::InGameInputGet('Z') || Gui::KeyboardInputGetRaw(VK_RETURN)) {
                th185_replaymanager_on_tick_cutoff.Disable();
            }
        }

    public:
        size_t selectedWave = SIZE_MAX;
        std::vector<std::pair<size_t, const char*>> waveOpts;
    };

    class THGuiPrac : public Gui::GameGuiWnd {
        THGuiPrac() noexcept
        {
            *mMode = 1;
            *mLife = 9;

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
                StageWarpsLoad(mStage, warps);
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
                thPracParam.life = *mLife;
                thPracParam.funds = *mFunds;
                thPracParam.difficulty = *mDifficulty;
                thPracParam.bulletMoney = *mBulletMoney;
                thPracParam.additional_cards = {};

                for (auto c : mAdditionalCards) {
                    if (c) thPracParam.additional_cards.push_back(c - 1);
                    else   break;
                }

                break;
            case 2:
                Close();
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

            StageWarpsLoad(mStage, warps);
        }
        virtual void OnContentUpdate() override
        {
            ImGui::TextUnformatted(S(TH_MENU));
            ImGui::Separator();

            PracticeMenu();
        }

        const char* difficulties[8] = {
            "Very Easy",
            "Easy",
            "Normal",
            "Little Hard",
            "Hard",
            "Very Hard",
            "Lunatic",
            "Over Drive"
        };

        void PracticeMenu()
        {
            mMode();
            if (*mMode == 1) {
                StageWarpsRender(warps, mWarp, 0);

                mLife();
                mFunds();
                mBulletMoney();
                mDifficulty(difficulties[*mDifficulty]);

                if (card_names.locale != Gui::LocaleGet()) {
                    card_names.locale = Gui::LocaleGet();
                    for (int i = 0; i < card_names.count; ++i) {
                        std::strcpy(card_names.strings[i], S(TH185_CARD_LIST[i]));
                    }
                }

                ImGui::Separator();
                ImGui::TextUnformatted(S(TH185_ADDITIONAL_CARDS));
                return Gui::MultiComboSelect(mAdditionalCards, card_names.ptrs, card_names.count, S(TH18_CARD_FORMAT));
            }
        }

        Gui::GuiCombo mMode { TH_MODE, TH_MODE_SELECT };
        size_t mStage;
        Gui::GuiDrag<int32_t, ImGuiDataType_S32> mBulletMoney { TH185_BULLET_MONEY, 0, INT_MAX };
        Gui::GuiSlider<int, ImGuiDataType_S32> mLife { TH_LIFE, 0, 9 };
        Gui::GuiDrag<int, ImGuiDataType_S32> mFunds { TH18_FUNDS, 0, 999990, 1, 100000 };
        Gui::GuiSlider<int, ImGuiDataType_S32> mDifficulty { TH_DIFFICULTY, 0, 7 };
        std::vector<unsigned int> mWarp;
        std::vector<unsigned int> mAdditionalCards = { 0 };

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
            SetTitle(S(TH_ADV_OPT));
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
        if (Gui::GetChordPressed(Gui::GetAdvancedMenuChord())) {
            if (advOptWnd->IsOpen())
                advOptWnd->Close();
            else
                advOptWnd->Open();
        }
        advOptWnd->Update();

        return advOptWnd->IsOpen();
    }

    PATCH_ST(th185_prac_disable_arrows, 0x46d39f, "e9cd000000");
    EHOOK_ST(th185_prac_leave, 0x46d481, 2, {
        if (isItemEnabled) {
            pCtx->Eip = 0x46d9c0;
        } else {
            self->Disable();
            THGuiPrac::singleton().State(2);
            th185_prac_disable_arrows.Disable();
            pCtx->Eip = 0x46d9c0;
        }
    });
    PATCH_ST(th185_unhardcode_bosses, 0x43d0f6, "eb");

    HOOKSET_DEFINE(THMainHook)
    EHOOK_DY(th185_wave_fire, 0x43cf47, 2, {
        if (!thPracParam.mode)
            return;

        auto getWaveName = [](const char* subName) -> const char* {
            const char* retStr = subName;

            auto subList = GetMemContent(0x004d7af4, 0x4f34, 0x10c);
            uint8_t* sub = ThModern_ECLGetSub(subName, subList);

            struct th10_instr_t {
                uint32_t time;
                uint16_t id;
                uint16_t size;
                uint16_t param_mask;
                uint8_t rank_mask;
                uint8_t param_count;
                uint32_t zero;
            };

            uint8_t* p = sub + ((uint32_t*)sub)[1];
            th10_instr_t* instr = (th10_instr_t*)p;

            while (*(uint32_t*)p != 0x484C4345) {
                if (instr->id == 1006) {
                    retStr = (char*)(p + sizeof(th10_instr_t) + 4);
                    break;
                }
                p += instr->size;
                instr = (th10_instr_t*)p;
            }

            return retStr;
        };

        auto& wave_select = THWaveSelect::singleton();

        if (wave_select.IsClosed()) {
            auto*  weights = (uint32_t*)GetMemAddr(ENEMY_MANAGER, 0x324);
            auto** sub_names = (const char**)0x4B5D50;

            wave_select.waveOpts = {};
            wave_select.selectedWave = SIZE_MAX;

            for (size_t i = 0; i < 110; i++) {
                if (weights[i]) {
                    wave_select.waveOpts.push_back({ i, getWaveName(sub_names[i]) });
                }
            }

            if (wave_select.waveOpts.size() <= 1)
                return;

            pCtx->Eax = ULONG_MAX; // Safe. DWORD == unsigned long
            pCtx->Eip = 0x43d57a;
            th185_replaymanager_on_tick_cutoff.Enable();
            wave_select.Open();
        } else {
            wave_select.Close();
        }

        if (wave_forced) {
            wave_forced = false;
            pCtx->Eax = ULONG_MAX; // Safe. DWORD == unsigned long
            pCtx->Eip = 0x43d57a;
        }
    })

    PATCH_DY(th185_unblock_all, 0x46d532, "eb")
    EHOOK_DY(th185_gui_update, 0x4013dd, 1, {
        GameGuiBegin(IMPL_WIN32_DX9);

        // Gui components update
        THOverlay::singleton().Update();
        THGuiPrac::singleton().Update();

        if (uintptr_t game_thread = GetMemContent(GAME_THREAD_PTR);
            game_thread && !(GetMemContent(game_thread + 0xb0) & 0x10))
            THWaveSelect::singleton().Update();

        GameGuiEnd(UpdateAdvOptWindow() || isItemEnabled || THGuiPrac::singleton().IsOpen());
    })
    EHOOK_DY(th185_gui_render, 0x4014fa, 1, {
        GameGuiRender(IMPL_WIN32_DX9);
    })
    EHOOK_DY(th185_patch_main, 0x448fb2, 1,{
        // 0x4d1024 = phase

        if (thPracParam.mode) {
            *(int32_t*)(0x4d1070) = thPracParam.bulletMoney;
            *(int32_t*)(0x4d1074) = thPracParam.bulletMoney;
            *(int32_t*)(0x4d106c) = thPracParam.funds;
            *(int32_t*)(0x4d10bc) = thPracParam.life;
            *(int32_t*)(0x4d1038) = thPracParam.difficulty;

            th185_unhardcode_bosses.Enable();

            // I want additional cards to always appear in the same wave slot
            // but ZUN never initializes this variable outside the ecl script
            // therefore, I have to set it myself.
            *(int32_t*)(0x4d1024) = 1;
            StageWarpsApply(warps, thPracParam.warp, ThModern_ECLGetSub, GetMemContent(0x004d7af4, 0x4f34, 0x10c), 0);
            for (auto& c : thPracParam.additional_cards)
                AddCard(c);

        } else {
            th185_unhardcode_bosses.Disable();
        }
    })
    EHOOK_DY(th185_restart, 0x45f86d, 2, {
        auto s1 = pCtx->Esp + 0xc;
        auto s2 = pCtx->Edi + 0x1e4;
        auto s3 = *(DWORD*)(pCtx->Edi + 0x1e8);

        asm_call<0x479220, Stdcall>(0x7, pCtx->Ecx);

        uint32_t* ret = asm_call<0x48AAA0, Thiscall, uint32_t*>(s2, s1, 125, pCtx->Ecx);

        asm_call<0x48a540, Stdcall>(*ret, 0x6);

        // Restart New 1
        asm_call<0x48a540, Stdcall>(s3, 0x1);

        // Set restart flag, same under replay save status
        asm_call<0x41C7e0, Thiscall>(pCtx->Esi, 0x5);

        // Switch menu state to close
        asm_call<0x45e050, Thiscall>(pCtx->Edi, 19);

        pCtx->Edx = *(DWORD*)0x4ce400;
        pCtx->Eip = 0x45f918;
    })
    EHOOK_DY(th185_exit, 0x45f918, 5, {
        if (Gui::KeyboardInputGetRaw('Q'))
            pCtx->Eip = 0x45f92e;

    })
    EHOOK_DY(th185_change_gamemode, 0x45b77a, 7, {
        THWaveSelect::singleton().Close();
        th185_replaymanager_on_tick_cutoff.Disable();
    })
    EHOOK_DY(th185_force_wave, 0x43d156, 7, {
        auto& wave_select = THWaveSelect::singleton();
        if (auto i = wave_select.selectedWave; i != -1 && thPracParam.mode) {
            auto& [id, name] = wave_select.waveOpts[i];
            pCtx->Esi = id;
            wave_forced = true;
        }
    })
    EHOOK_DY(th185_prac_confirm, 0x46d523, 6, {
        if (isItemEnabled) {
            pCtx->Eip = 0x46d9c0;
            return;
        }
        auto& p = THGuiPrac::singleton();
        if (p.IsOpen()) {
            p.State(1);
            th185_prac_disable_arrows.Disable();
            th185_prac_leave.Disable();
        } else {
            p.State(0);
            th185_prac_disable_arrows.Enable();
            th185_prac_leave.Enable();
            pCtx->Eip = 0x46d9c0;
        }
    })
    PATCH_DY(th185_disable_topmost, 0x4747ac, "00")
    HOOKSET_ENDDEF()

    static __declspec(noinline) void THGuiCreate()
    {
        if (ImGui::GetCurrentContext()) {
            return;
        }
        // Init
        GameGuiInit(IMPL_WIN32_DX9, 0x4d52c8, 0x571d50,
            Gui::INGAGME_INPUT_GEN2, 0x4ce400, 0x4ce3f8, 0,
            -2, *(float*)0x573dc0, 0.0f);

        SetDpadHook(0x40175F, 3);

        // Gui components creation
        THOverlay::singleton();
        THGuiPrac::singleton();
        THWaveSelect::singleton();

        th185_prac_disable_arrows.Setup();
        th185_prac_leave.Setup();
        th185_unhardcode_bosses.Setup();

        // Hooks
        EnableAllHooks(THMainHook);
    }
    HOOKSET_DEFINE(THInitHook)
    EHOOK_DY(th185_gui_init_1, 0x46ce39, 2, {
        self->Disable();
        THGuiCreate();
    })
    EHOOK_DY(th185_gui_init_2, 0x476580, 1, {
        self->Disable();
        THGuiCreate();
    })
    HOOKSET_ENDDEF()
}

void TH185Init()
{
    EnableAllHooks(TH185::THInitHook);
}

}
