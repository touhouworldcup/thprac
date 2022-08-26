#include "thprac_utils.h"
#include <optional>

namespace THPrac {
namespace TH185 {
    __declspec(noinline) void AddCard(uint32_t cardId) {
        auto real_AddCard = (void(__thiscall*)(uint32_t, uint32_t, uint32_t))0x414F20;
        if (cardId < 85)
            real_AddCard(*(uint32_t*)0x4d7ab8, cardId, 2);
    }

    struct ecl_write_t {
        uint32_t off;
        std::vector<uint8_t> bytes;
        void apply(uint8_t* start) {
            for (unsigned int i = 0; i < bytes.size(); i++) {
                start[off + i] = bytes[i];
            }
        }
    };

    struct ecl_jump_t {
        uint32_t off;
        uint32_t dest;
        uint32_t at_frame;
        uint32_t ecl_time;
    };

    struct stage_warps_t;

    struct section_param_t {
        const char* label;
        std::unordered_map<std::string, std::vector<ecl_jump_t>>  jumps;
        std::unordered_map<std::string, std::vector<ecl_write_t>> writes;
        stage_warps_t* phases;
    };

    struct stage_warps_t {
        const char* label;
        enum {
            TYPE_SLIDER,
            TYPE_COMBO
        } type;
        std::vector<section_param_t> section_param;
    };

    void StageWarpsRender(stage_warps_t& warps, std::vector<unsigned int>& out_warp, size_t level) {
        if (warps.section_param.size() == 0)
            return;

        if (out_warp.size() <= level)
            out_warp.resize(level + 1);

        switch (warps.type) {
        case stage_warps_t::TYPE_SLIDER:
            ImGui::SliderInt(warps.label, (int*)&out_warp[level], 0, warps.section_param.size() - 1, warps.section_param[out_warp[level]].label);
            break;
        case stage_warps_t::TYPE_COMBO:
            if (ImGui::BeginCombo(warps.label, warps.section_param[out_warp[level]].label)) {
                for (unsigned int i = 0; i < warps.section_param.size(); i++) {
                    ImGui::PushID(i);
                    
                    bool item_selected = (out_warp[level] == i);

                    if (ImGui::Selectable(warps.section_param[i].label, &item_selected)) {
                        out_warp[level] = i;
                    }

                    if (item_selected)
                        ImGui::SetItemDefaultFocus();

                    ImGui::PopID();
                }
                ImGui::EndCombo();
            }
            break;
        }
        
        if (ImGui::IsItemFocused()) {
            if (Gui::InGameInputGet(VK_LEFT) && out_warp[level] > 0) {
                out_warp[level]--;
            }
            if (Gui::InGameInputGet(VK_RIGHT) && out_warp[level] + 1 < warps.section_param.size()) {
                out_warp[level]++;
            }
        }
        
        if (warps.section_param[out_warp[level]].phases)
            StageWarpsRender(*warps.section_param[out_warp[level]].phases, out_warp, level + 1);
    }

    void StageWarpsApply(stage_warps_t& warps, std::vector<unsigned int>& in_warp, size_t level) {
        auto& param = warps.section_param[in_warp[level]];

        auto ECLGetSub = [](const char* name) -> uint8_t* {
            struct ecl_sub_t {
                const char* name;
                uint8_t* data;
            };
            auto subs = (ecl_sub_t*)GetMemContent(0x004d7af4, 0x4f34, 0x10c);

            while (strcmp(subs->name, name))
                subs++;
            return subs->data;
        };

        // This entire block gives me the idea to convert to jumps once there's a JSON file.
        // But for readability, as long as there is no JSON file, this block will have to stay
        for (auto& jumps : param.jumps) {
            uint8_t* ecl = ECLGetSub(jumps.first.c_str());
            for (auto& jmp : jumps.second) {
                ecl_write_t real_write;
                real_write.off = jmp.off;
                union i32b {
                    uint32_t i;
                    uint8_t b[4];
                    i32b(uint32_t a) : i(a) {}
                };

                i32b ecl_time = jmp.ecl_time;
                uint8_t instr[] = { 0x0c, 0x00, 0x18, 0x00, 0x00, 0x00, 0xff, 0x2c, 0x00, 0x00, 0x00, 0x00 };
                i32b dest = jmp.dest - jmp.off;
                i32b at_frame = jmp.at_frame;
                
                #define BYTES_APPEND(a)                    \
                for (size_t j = 0; j < sizeof(a); j++) \
                    real_write.bytes.push_back(a[j]);
                
                BYTES_APPEND(ecl_time.b);
                BYTES_APPEND(instr);
                BYTES_APPEND(dest.b);
                BYTES_APPEND(at_frame.b);
                #undef BYTES_APPEND
                
                real_write.apply(ecl);
            }
        }

        for (auto& writes : param.writes) {
            uint8_t* ecl = ECLGetSub(writes.first.c_str());
            for (auto& write : writes.second) {
                write.apply(ecl);
            }
        }

        if (param.phases)
            StageWarpsApply(*param.phases, in_warp, level + 1);
    }

    struct THPracParam {
        int32_t mode;
        int32_t stage;
        std::vector<unsigned int> warp;

        //bool _playLock = false;
        void Reset()
        {
            *this = {};
        }
        /*
        bool ReadJson(std::string& json)
        {
            ParseJson();

            ForceJsonValue(game, "th185");
            GetJsonValue(mode);
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
        */
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
    

    stage_warps_t mike = {
        .label = "Attack",
        .type = stage_warps_t::TYPE_COMBO,
        .section_param = {
            {
                .label = "Nonspell 1"
            },
            {
                .label = "Spell 1"
            }
        }
    };

    stage_warps_t stages[9] = {
        {},
        {
            .label = "Progress",
            .type = stage_warps_t::TYPE_SLIDER,
            .section_param = { 
                {
                    .label = "Wave 1"
                },
                {
                    .label = "Wave 2",
                    .jumps = {
                        { "main", {
                            {
                                .off = 0x258,
                                .dest = 0x380
                            }
                        } }
                    },
                    .writes = {
                        { "main", {
                            {
                                .off = 254,
                                .bytes = { 2 }
                            }
                        } }
                    },
                },
                {
                    .label = "Wave 3"
                },
                {
                    .label = "Mike Goutokuji",
                    .phases = &mike
                },
                {
                    .label = "Minoriko Aki"
                },
                {
                    .label = "Eternity Larva"
                },
                {
                    .label = "Nemuno Sakata"
                }
            }
        },
    };

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
                thPracParam.Reset();
                break;
            case 1:
                SetFade(0.8f, 0.1f);
                Close();

                // Fill Param
                thPracParam.mode = *mMode;
                thPracParam.warp = mWarp;
                thPracParam.stage = mStage;

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

        void PracticeMenu()
        {
            mMode();
            if (*mMode == 1) {
                StageWarpsRender(stages[mStage], mWarp, 0);                
            }
            //mNavFocus();
        }

        Gui::GuiCombo mMode { TH_MODE, TH_MODE_SELECT };
        size_t mStage;
        //Gui::GuiNavFocus mNavFocus { TH185_MARKET, TH_MODE, TH_WARP };
        std::vector<unsigned int> mWarp;

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

    PATCH_ST(th185_prac_disable_arrows, (void*)0x46d39f, "\xe9\xcd\x00\x00\x00", 5);
    EHOOK_G1(th185_prac_leave, (void*)0x46d481)
    {
        th185_prac_leave::GetHook().Disable();
        THGuiPrac::singleton().State(2);
        th185_prac_disable_arrows.Disable();
        pCtx->Eip = 0x46d9c0;
    }

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

    EHOOK_DY(th185_patch_main, (void*)0x448fb2)
    {
        /*
            *(int32_t*)(0x4d10ac) = thPracParam.speed;
            *(int32_t*)(0x4d1070) = thPracParam.bulletMoney;
            *(int32_t*)(0x4d1074) = thPracParam.bulletMoney;
            *(int32_t*)(0x4d10bc) = thPracParam.life;
            *(int32_t*)(0x4d1094) = thPracParam.magicBreak;
            *(int32_t*)(0x4d1088) = thPracParam.sAttack;
            *(int32_t*)(0x4D1090) = thPracParam.magicSize;
            *(int32_t*)(0x4D1098) = thPracParam.magicTime;
            *(int32_t*)(0x4D109C) = thPracParam.reload; //-1
            *(int32_t*)(0x4D10B8) = (thPracParam.cooltime - 100) * -1;
            *(int32_t*)(0x4d1078) = (thPracParam.power - 1) * 100;

            *(int32_t*)0x4d1024 = 1;
            AddCard(24);
            *(int32_t*)0x4d1024 = 2;
            AddCard(24);
            *(int32_t*)0x4d1024 = 0;
            //*(int32_t*)0x4d1024 = 1;
            // AddCard(24);
        */
        if (thPracParam.mode) {
            StageWarpsApply(stages[thPracParam.stage], thPracParam.warp, 0);
        }
    }
    EHOOK_DY(th185_prac_confirm, (void*)0x46d523)
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
    PATCH_DY(th185_disable_topmost, (void*)0x4747ac, "\x00", 1);
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