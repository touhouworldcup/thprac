#include "thprac_games.h"
#include "thprac_utils.h"
#include "thprac_th19.h"

namespace THPrac {
namespace TH19 {
#define Gui__UpdateHearts asm_call_rel<0x101E90, Thiscall>

#define PlayerBarrier__restore asm_call_rel<0xA430, Thiscall>

#define AnmInterrupt asm_call_rel<0xBE070, Stdcall>

    enum addrs {
        SCALE = 0x20B1D0,
        GLOBALS = 0x207910,
        GUI_PTR = 0x1AE460,
        P1_PTR = 0x1AE474,
        P2_PTR = 0x1AE4B0,

        P1_CPU_PTR = 0x1ae4a4,
        P2_CPU_PTR = 0x1ae4e0,
    };

    #define SCALE (*(float*)RVA(SCALE))

    class THAdvOptWnd : public Gui::GameGuiWnd {
        // Option Related Functions
    private:
        void FpsInit()
        {
            if (*(uint8_t*)RVA(0x208621) == 3) {
                mOptCtx.fps_status = 1;

                DWORD oldProtect;
                VirtualProtect((void*)RVA(0xA95B6), 4, PAGE_EXECUTE_READWRITE, &oldProtect);
                *(double**)RVA(0xA95B6) = &mOptCtx.fps_dbl;
                VirtualProtect((void*)RVA(0xA95B6), 4, oldProtect, &oldProtect);
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
        if (Gui::KeyboardInputUpdate(VK_F12) == 1) {
            if (advOptWnd->IsOpen())
                advOptWnd->Close();
            else
                advOptWnd->Open();
        }
        advOptWnd->Update();

        return advOptWnd->IsOpen();
    }

    struct TH19Tools : public Gui::GameGuiWnd {
        TH19Tools()
        {
            SetWndFlag(ImGuiWindowFlags_NoCollapse);
            SetFade(0.8f, 0.8f);
            SetStyle(ImGuiStyleVar_WindowRounding, 0.0f);
            SetStyle(ImGuiStyleVar_WindowBorderSize, 0.0f);
            SetAutoSpacing(true);
            SetItemWidthRel(0.0f);
            OnLocaleChange();

            th19_enemy_tick.Setup();
        }

        EHOOK_ST(th19_enemy_tick, 0xf6dd0)
        {
            uint32_t side = *(uint32_t*)(pCtx->Ecx + 0x5638);

            if (!GameState_Assert(side <= 1)) {
                return;
            }

            static const ImVec2 side_offsets[] = {
                { 164.0f, 16.0f },
                { 476.0f, 16.0f },
            };

            const ImVec2& offset = side_offsets[side];

            uint32_t flags = *(uint32_t*)(pCtx->Ecx + 0x516c);
            if (flags & 0x21) {
                return;
            }

            float* enm_pos = (float*)(pCtx->Ecx + 0x48);
            uint32_t hp = *(uint32_t*)(pCtx->Ecx + 0x5008);

            std::string hp_str = std::to_string(hp);

            auto* drawList = ImGui::GetOverlayDrawList();
            auto* font = ImGui::GetFont();

            ImVec2 textSize = font->CalcTextSizeA(font->FontSize, ImGui::GetIO().DisplaySize.x, 0.0f, hp_str.c_str(), hp_str.c_str() + hp_str.size());
            
            ImVec2 upperLeft = ImVec2((enm_pos[0] + offset.x) * SCALE, (enm_pos[1] + offset.y) * SCALE);
            ImVec2 lowerRight = ImVec2(upperLeft.x + textSize.x, upperLeft.y + textSize.y);

            drawList->AddRectFilled(upperLeft, lowerRight, 0xAA555555);
            drawList->AddText(upperLeft, 0xFFFF55FF, hp_str.c_str(), hp_str.c_str() + hp_str.size());
        }

        bool allow = false;

        bool p1_gauge_lock = false;
        bool p2_gauge_lock = false;

        int p1_gauge_stored;
        int p2_gauge_stored;

        bool p1_c3_level_lock = false;
        bool p1_c4_level_lock = false;
        bool p2_c3_level_lock = false;
        bool p2_c4_level_lock = false;

        int p1_c3_level_stored;
        int p1_c4_level_stored;
        int p2_c3_level_stored;
        int p2_c4_level_stored;

        bool p1_invincible = false;
        bool p2_invincible = false;

        bool p1_lives_lock = false;
        bool p2_lives_lock = false;

        bool p1_cpu_next_charge_lock = false;
        bool p2_cpu_next_charge_lock = false;

        int rank_stored;
        bool rank_lock = false;

        bool hp_show = false;

        virtual void OnContentUpdate() override
        {
            // A reference is basically a pointer that pretends to not be a pointer
            Globals& globals = *(Globals*)RVA(GLOBALS);
            uintptr_t p1 = *(uintptr_t*)RVA(P1_PTR);
            uintptr_t p2 = *(uintptr_t*)RVA(P2_PTR);
            uintptr_t gui = *(uintptr_t*)RVA(GUI_PTR);

            if (!p1 || !p2 || !gui) {
                return;
            }

            // BIG TODO AT THE END: translation support

            ImGui::TextUnformatted("Invincible");
            ImGui::Checkbox("P1##invincible_p1", &p1_invincible);
            ImGui::SameLine();
            ImGui::Checkbox("P2##invincible_p2", &p2_invincible);

            ImGui::TextUnformatted("Lives");

            if (ImGui::SliderInt("P1##lives_p1", &globals.side[0].lives, 1, globals.side[0].max_lives)) {
                Gui__UpdateHearts(gui + 0x10);                
            }
            ImGui::SameLine();
            ImGui::Checkbox("Lock##lives_lock_p1", &p1_lives_lock);

            if(ImGui::SliderInt("P2##lives_p2", &globals.side[1].lives, 1, globals.side[1].max_lives)) {
                Gui__UpdateHearts(gui + 0x7C);
            }
            ImGui::SameLine();
            ImGui::Checkbox("Lock##lives_lock_p2", &p2_lives_lock);
            
            ImGui::TextUnformatted("Instant death");
            if (ImGui::Button("P1##instant_death_p1")) {
                globals.side[0].lives = 0;
                *(int*)(p1 + 0x10) = 4;
            }
            ImGui::SameLine();
            if (ImGui::Button("P2##instant_death_p2")) {
                globals.side[1].lives = 0;
                *(int*)(p2 + 0x10) = 4;
            }
            ImGui::SameLine();
            if (ImGui::Button("Both##instant_death_both")) {
                globals.side[0].lives = 0;
                globals.side[1].lives = 0;
                *(int*)(p1 + 0x10) = 4;
                *(int*)(p2 + 0x10) = 4;
            }

            ImGui::TextUnformatted("Charge Gauge");

            auto chargegauge = [](GlobalsSide& side, bool& lock, const char* format, int& gauge_store) {
                float bsize = ImGui::GetFrameHeight();
                ImGuiStyle& style = ImGui::GetStyle();
                const ImVec2 backup_frame_padding = style.FramePadding;
                style.FramePadding.x = style.FramePadding.y;

                ImGui::PushID(format);

                if (!lock) {
                    gauge_store = side.gauge;
                }
                ImGui::SliderInt("##_gauge", &gauge_store, 0, 2500, format);

                ImGui::SameLine(0, style.ItemInnerSpacing.x);
                if (ImGui::Button("-##_gauge_subtract", ImVec2(bsize, bsize))) {
                    if (gauge_store > side.c4_threshold) {
                        gauge_store = side.c4_threshold;
                    } else if (gauge_store > side.c3_threshold) {
                        gauge_store = side.c3_threshold;
                    } else if (gauge_store > side.c2_threshold) {
                        gauge_store = side.c2_threshold;
                    } else if (gauge_store > side.c1_threshold) {
                        gauge_store = side.c1_threshold;
                    } else {
                        gauge_store = 0;
                    }
                }
                ImGui::SameLine(0, style.ItemInnerSpacing.x);
                if (ImGui::Button("+##_gauge_add", ImVec2(bsize, bsize))) {
                    if (gauge_store < side.c1_threshold) {
                        gauge_store = side.c1_threshold;
                    } else if (gauge_store < side.c2_threshold) {
                        gauge_store = side.c2_threshold;
                    } else if (gauge_store < side.c3_threshold) {
                        gauge_store = side.c3_threshold;
                    } else if (gauge_store < side.c4_threshold) {
                        gauge_store = side.c4_threshold;
                    } else {
                        gauge_store = 2500;
                    }
                }
                style.FramePadding = backup_frame_padding;
                ImGui::SameLine();
                ImGui::Checkbox("##_gauge_lock", &lock);
                ImGui::PopID();
                if (ImGui::IsItemHovered()) {
                    ImGui::BeginTooltip();
                    ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
                    ImGui::TextUnformatted("Lock");
                    ImGui::PopTextWrapPos();
                    ImGui::EndTooltip();
                }
                side.gauge = gauge_store;
            };

            chargegauge(globals.side[0], p1_gauge_lock, "P1: %d", p1_gauge_stored);
            chargegauge(globals.side[1], p2_gauge_lock, "P2: %d", p2_gauge_stored);

            auto c3c4 = [](GlobalsSide& side, int& c3_level_stored, int& c4_level_stored, bool& c3_level_lock, bool& c4_level_lock) {
                ImGui::PushID((int) &side);

                if (!c3_level_lock) {
                    c3_level_stored = side.c3_level;
                }
                int _c3_level = c3_level_stored + 1;
                ImGui::SliderInt("C3##c3_level", &_c3_level, 1, 8);
                c3_level_stored = _c3_level - 1;
                side.c3_level = c3_level_stored;
                ImGui::SameLine();
                ImGui::Checkbox("Lock##c3_level_lock", &c3_level_lock);

                if (!c4_level_lock) {
                    c4_level_stored = side.c4_level;
                }
                int _c4_level = c4_level_stored + 1;
                ImGui::SliderInt("C4##c4_level", &_c4_level, 1, 8);
                c4_level_stored = _c4_level - 1;
                side.c4_level = c4_level_stored;
                ImGui::SameLine();
                ImGui::Checkbox("Lock##c4_level_lock", &c4_level_lock);

                ImGui::PopID();
            };

            ImGui::TextUnformatted("C3/C4 Level (P1)");
            c3c4(globals.side[0], p1_c3_level_stored, p1_c4_level_stored, p1_c3_level_lock, p1_c4_level_lock);

            ImGui::TextUnformatted("C3/C4 Level (P2)");
            c3c4(globals.side[1], p2_c3_level_stored, p2_c4_level_stored, p2_c3_level_lock, p2_c4_level_lock);
            
            ImGui::TextUnformatted("CPU Next Charge");

            auto cpu_next_charge = [](uintptr_t addr, const char* label, bool& lock) {
                if (!GetMemContent(addr)) {
                    return;
                }
                int* ptr = GetMemAddr<int*>(addr, 0x30, 0xEA630);

                ImGui::PushID(addr);

                ImGui::SliderInt(label, ptr, 2, 4);
                ImGui::SameLine();
                ImGui::Checkbox("Lock", &lock);

                ImGui::PopID();
            };

            cpu_next_charge(RVA(P1_CPU_PTR), "P1", p1_cpu_next_charge_lock);
            cpu_next_charge(RVA(P2_CPU_PTR), "P2", p2_cpu_next_charge_lock);

            ImGui::TextUnformatted("Barrier");
            
            auto _barrier = [](PlayerBarrier* barrier, const char* label) {
                bool barrier_bool = false;

                if (barrier->state == BARRIER_DISABLED) { 
                    ImGui::BeginDisabled();
                } else if (barrier->state == BARRIER_ACTIVE) {
                    barrier_bool = true;
                }
                
                ImGui::Checkbox(label, &barrier_bool);

                if (barrier->state == BARRIER_DISABLED) {
                    ImGui::EndDisabled();
                } else if (barrier_bool && (barrier->state == BARRIER_INACTIVE)) {
                    PlayerBarrier__restore(barrier);
                } else if (!barrier_bool && (barrier->state == BARRIER_ACTIVE)) {
                    // I am not calling PlayerBarrier::break (Rx12FBC0) here because:
                    // ---
                    // 1) That function is hooked to do nothing and return 0 if invincibility is enabled
                    // 1.5) I kinda wrote myself in a corner when I put all of this code *above* TH19PracHook....
                    // 2) But none of that matters because the function does a bunch of other stuff that I don't want.
                    //    So I decided to just put in the parts that I do want.
                    barrier->state = BARRIER_INACTIVE;
                    barrier->no_hit_timer = {};
                    barrier->no_hit_timer.previous = -1;
                    AnmInterrupt(barrier->anm_id, 1);
                    barrier->anm_id = 0;
                }
            };

            _barrier((PlayerBarrier*)(p1 + 0x18), "P1##p1_barrier");
            ImGui::SameLine();
            _barrier((PlayerBarrier*)(p2 + 0x18), "P2##p2_barrier");

            ImGui::TextUnformatted("Misc.");

            if (!rank_lock) {
                rank_stored = globals.difficulty;
            }
            ImGui::SliderInt("##rank", &rank_stored, 0, 7, "Rank: %d");
            ImGui::SameLine();
            ImGui::Checkbox("Lock##rank_lock", &rank_lock);
            globals.difficulty = rank_stored;

            if (ImGui::Checkbox("Show enemy HP##hp_show", &hp_show)) {
                if (hp_show) {
                    th19_enemy_tick.Enable();
                } else {
                    th19_enemy_tick.Disable(); 
                }
            }

        }
        virtual void OnLocaleChange() override
        {
            SetTitle("TH19 Tools");
            SetSizeRel(0.5, 1);
            SetPosRel(0.5, 0);
        }
        SINGLETON(TH19Tools);
    };

    HOOKSET_DEFINE(TH19PracHook)

    // In the loader thread, right before the instruction that tells the main thread that loading has finished
    EHOOK_DY(th19_prac_init, 0x106878) {
        TH19Tools::singleton().Open();
        TH19Tools::singleton().allow = true;
    }
    
    // In the gamemode switching code where the game switches to main menu mode
    EHOOK_DY(th19_prac_uninit, 0x11FA07) {
        TH19Tools::singleton().Close();
        TH19Tools::singleton().allow = false;
    }

    EHOOK_DY(th19_invincible, 0x130ACC) {
        TH19Tools& t = TH19Tools::singleton();
        if (((pCtx->Edi == *(DWORD*)RVA(P1_PTR)) && t.p1_invincible)
            || ((pCtx->Edi == *(DWORD*)RVA(P2_PTR)) && t.p2_invincible))
        {
            pCtx->Eip = RVA(0x130AD3);
        }
    }

    EHOOK_DY(th19_invincible_barrier, 0x12FBC0)
    {
        TH19Tools& t = TH19Tools::singleton();
        DWORD p = pCtx->Ecx - 0x18;

        if (((p == *(DWORD*)RVA(P1_PTR)) && t.p1_invincible)
            || ((p == *(DWORD*)RVA(P2_PTR)) && t.p2_invincible)) {
            pCtx->Eax = 0;
            pCtx->Eip = PopHelper32(pCtx);
        }
    }
    
    EHOOK_DY(th19_lock_lives, 0x123EB5)
    {
        Globals& globals = *(Globals*)RVA(GLOBALS);
        TH19Tools& t = TH19Tools::singleton();

        if (((pCtx->Edi == (uintptr_t)&globals.side[0]) && t.p1_lives_lock)
            || ((pCtx->Edi == (uintptr_t)&globals.side[1]) && t.p2_lives_lock))
        {
            pCtx->Eip++;
        }
    }

    EHOOK_DY(th19_lock_cpu_next_charge, 0xE9D33) {
        auto ptr = pCtx->Edi;
        auto& t = TH19Tools::singleton();

        auto CHK = [ptr](uintptr_t addr) -> bool {
            auto cpu_ptr = GetMemContent(RVA(addr));
            if (cpu_ptr) {
                return GetMemContent(cpu_ptr + 0x30) == ptr;
            } else {
                return false;
            }
        };

        if (
            (CHK(P1_CPU_PTR) && t.p1_cpu_next_charge_lock) ||
            (CHK(P2_CPU_PTR) && t.p2_cpu_next_charge_lock)
        )
        {
            pCtx->Eip = RVA(0xE9D3E);
        }
    }

    HOOKSET_ENDDEF()

    struct THGuiPrac : public Gui::GameGuiWnd {
        THGuiPrac()
        {
            SetFade(0.8f, 0.1f);
            SetStyle(ImGuiStyleVar_WindowRounding, 0.0f);
            SetStyle(ImGuiStyleVar_WindowBorderSize, 0.0f);
            SetAutoSpacing(true);
            OnLocaleChange();
        }

    public:
        Gui::GuiCombo mMode { TH_MODE, TH_MODE_SELECT };
        SINGLETON(THGuiPrac);

    protected:
        virtual void OnLocaleChange() override
        {
            SetTitle(S(TH_MENU));
            SetSizeRel(0.3125f, 0.075f);
            SetPosRel(0.34375f, 0.15f);
            SetItemWidthRel(-0.052f);
        }
        virtual void OnContentUpdate() override { mMode(); }
    };

    PATCH_ST(th19_vs_mode_disable_movement, 0x142131, "\xeb", 1);

    HOOKSET_DEFINE(THMainHook)

    EHOOK_DY(th19_update_begin, 0xC89E0) {
        GameGuiBegin(IMPL_WIN32_DX9);
    }

    EHOOK_DY(th19_update_end, 0xC8B75) {
        auto& p = THGuiPrac::singleton();
        p.Update();

        auto& t = TH19Tools::singleton();
        if (t.allow) {
            if (Gui::KeyboardInputUpdate(VK_F11) == 1) {
                if (t.IsOpen()) {
                    t.Close();
                } else {
                    t.Open();
                }
            }
            t.Update();
        }
        
        GameGuiEnd(UpdateAdvOptWindow() || p.IsOpen() || t.IsOpen());
    }

    EHOOK_DY(th19_render, 0xC8C8D) {
        GameGuiRender(IMPL_WIN32_DX9);
    }

    EHOOK_DY(th19_vs_mode_enter, 0x14220F)
    {
        if (*(uint32_t*)(pCtx->Edi + 0x2c) > 2) {
            return;
        }

        auto& p = THGuiPrac::singleton();

        if (p.IsClosed()) {
            p.Open();
            pCtx->Eip = RVA(0x14231C);
            th19_vs_mode_disable_movement.Enable();
            TH19PracHook::singleton().DisableAllHooks();
            return;
        }

        p.Close();
        if (*p.mMode) {
            TH19PracHook::singleton().EnableAllHooks();
        }
        th19_vs_mode_disable_movement.Disable();
    }
    EHOOK_DY(th19_vs_mode_exit, 0x1421CB) {
        TH19PracHook::singleton().DisableAllHooks();
        auto& p = THGuiPrac::singleton();
        if (p.IsClosed()) {
            return;
        }
        p.Close();
        th19_vs_mode_disable_movement.Disable();
        pCtx->Eip = RVA(0x142204);
    }

    HOOKSET_ENDDEF()

    HOOKSET_DEFINE(THInitHook)
    static __declspec(noinline) void THGuiCreate()
    {
        th19_vs_mode_disable_movement.Setup();

        // Init
        GameGuiInit(IMPL_WIN32_DX9, RVA(0x208388), RVA(0x209110), RVA(0xA9EE0),
            Gui::INGAGME_INPUT_GEN2, GetMemContent(RVA(0x1AE3A0)) + 0x30 + 0x2B0, GetMemContent(RVA(0x1AE3A0)) + 0x30 + 0x10, 0,
            -2, SCALE, 0.0f);

        //// Gui components creation
        //THOverlay::singleton();
        THGuiPrac::singleton();
        TH19Tools::singleton();
        //
        // Hooks
        THMainHook::singleton().EnableAllHooks();
    }
    static __declspec(noinline) void THInitHookDisable()
    {
        auto& s = THInitHook::singleton();
        s.th19_gui_init_1.Disable();
        s.th19_gui_init_2.Disable();
    }

    // in the part of MainMenu::on_tick responsible for the title screen menu itself
    EHOOK_DY(th19_gui_init_1, 0x1439A2)
    {
        THGuiCreate();
        THInitHookDisable();
    }

    // After InputManager is initialized
    EHOOK_DY(th19_gui_init_2, 0x2A8A)
    {
        THGuiCreate();
        THInitHookDisable();
    }
    HOOKSET_ENDDEF()
}


void TH19Init()
{    
    ingame_image_base = (uintptr_t)GetModuleHandleW(NULL);
    TH19::THInitHook::singleton().EnableAllHooks();
}
}