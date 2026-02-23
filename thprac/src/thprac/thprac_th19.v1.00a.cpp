#include "thprac_utils.h"
#include "thprac_th19.h"

#include "utils/wininternal.h"

namespace THPrac {
namespace TH19 {

namespace V1_00a {

#define Gui__UpdateHearts asm_call_rel<0x101E90, Thiscall>
#define PlayerBarrier__restore asm_call_rel<0xA430, Thiscall>
#define AnmInterrupt asm_call_rel<0xBE070, Stdcall>
#define Ascii__debugf asm_call_rel<0xD7950, Cdecl>
#define AddCard asm_call_rel<0xCFAE0, Thiscall>

    enum addrs {
        GLOBALS = 0x207910,
        GUI_PTR = 0x1AE460,
        ASCII_MANAGER_PTR = 0x1ae444,

        P1_PTR = 0x1AE474,
        P2_PTR = 0x1AE4B0,

        P1_CPU_PTR = 0x1ae4a4,
        P2_CPU_PTR = 0x1ae4e0,

        P1_ABILITYMANAGER = 0x1AE490,

        LATENCY_SETTING = 0x208621,
        FPS_COUNTER_PTR  = 0x1ae45c,

        FPS_USE_IN_CODE = 0xA95B6,

        D3D_DEVICE = 0x208388,
        HWND_PTR = 0x209110,
    };
        
    #define SCALE (*(float*)RVA(0x20B1D0))

    class THAdvOptWnd : public Gui::GameGuiWnd {
        // Option Related Functions
    private:
        void FpsInit()
        {
            if (*(uint8_t*)RVA(LATENCY_SETTING) == 3) {
                mOptCtx.fps_status = 1;

                DWORD oldProtect;
                VirtualProtect((void*)RVA(FPS_USE_IN_CODE), 4, PAGE_EXECUTE_READWRITE, &oldProtect);
                *(double**)RVA(FPS_USE_IN_CODE) = &mOptCtx.fps_dbl;
                VirtualProtect((void*)RVA(FPS_USE_IN_CODE), 4, oldProtect, &oldProtect);
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
            case LOCALE_ZH_CN:
                SetSizeRel(1.0f, 1.0f);
                SetPosRel(0.0f, 0.0f);
                SetItemWidthRel(-0.075f);
                SetAutoSpacing(true);
                break;
            case LOCALE_EN_US:
                SetSizeRel(1.0f, 1.0f);
                SetPosRel(0.0f, 0.0f);
                SetItemWidthRel(-0.075f);
                SetAutoSpacing(true);
                break;
            case LOCALE_JA_JP:
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

    EHOOK_ST(th19_enemy_tick, 0xf6dd0, 1, {
        drawEnemyHP(pCtx, SCALE);
    });

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

            ImGui::TextUnformatted(S(TH_MUTEKI));
            ImGui::Checkbox("P1##invincible_p1", &p1_invincible);
            ImGui::SameLine();
            ImGui::Checkbox("P2##invincible_p2", &p2_invincible);

            ImGui::TextUnformatted(S(TH_LIFE));

            if (ImGui::SliderInt("P1##lives_p1", &globals.side[0].lives, 1, globals.side[0].max_lives)) {
                Gui__UpdateHearts(gui + 0x10);                
            }
            ImGui::SameLine();
            ImGui::PushID((int)&p1_lives_lock);
            ImGui::Checkbox(S(TH09_LOCK), &p1_lives_lock);
            ImGui::PopID();

            if(ImGui::SliderInt("P2##lives_p2", &globals.side[1].lives, 1, globals.side[1].max_lives)) {
                Gui__UpdateHearts(gui + 0x7C);
            }
            ImGui::SameLine();
            ImGui::PushID((int)&p2_lives_lock);
            ImGui::Checkbox(S(TH09_LOCK), &p2_lives_lock);
            ImGui::PopID();

            ImGui::TextUnformatted(S(TH09_INSTANT_DEATH));
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
            if (ImGui::Button(S(TH09_BOTH))) {
                globals.side[0].lives = 0;
                globals.side[1].lives = 0;
                *(int*)(p1 + 0x10) = 4;
                *(int*)(p2 + 0x10) = 4;
            }

            ImGui::TextUnformatted(S(TH09_CHARGE_GAUGE));

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
                    ImGui::TextUnformatted(S(TH09_LOCK));
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
                ImGui::PushID((int)&c3_level_lock);
                ImGui::Checkbox(S(TH09_LOCK), &c3_level_lock);
                ImGui::PopID();

                if (!c4_level_lock) {
                    c4_level_stored = side.c4_level;
                }
                int _c4_level = c4_level_stored + 1;
                ImGui::SliderInt("C4##c4_level", &_c4_level, 1, 8);
                c4_level_stored = _c4_level - 1;
                side.c4_level = c4_level_stored;
                ImGui::SameLine();
                ImGui::PushID((int)&c4_level_lock);
                ImGui::Checkbox(S(TH09_LOCK), &c4_level_lock);
                ImGui::PopID();

                ImGui::PopID();
            };

            ImGui::TextUnformatted(S(TH19_C_RANK_P1));
            c3c4(globals.side[0], p1_c3_level_stored, p1_c4_level_stored, p1_c3_level_lock, p1_c4_level_lock);

            ImGui::TextUnformatted(S(TH19_C_RANK_P2));
            c3c4(globals.side[1], p2_c3_level_stored, p2_c4_level_stored, p2_c3_level_lock, p2_c4_level_lock);
            
            ImGui::TextUnformatted(S(TH09_CPU_CHARGE));

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

            ImGui::TextUnformatted(S(TH19_BARRIER));
            
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

            ImGui::TextUnformatted(S(TH09_MISC));

            if (!rank_lock) {
                rank_stored = globals.difficulty;
            }
            ImGui::SliderInt("##rank", &rank_stored, 0, 7, S(TH19_RANK_FORMAT));
            ImGui::SameLine();
            ImGui::PushID((int)&rank_lock);
            ImGui::Checkbox(S(TH09_LOCK), &rank_lock);
            ImGui::PopID();
            globals.difficulty = rank_stored;

            if (ImGui::Checkbox(S(TH19_SHOW_ENEMY_HP), &hp_show)) {
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
    EHOOK_DY(th19_prac_init, 0x106878, 3, {
        auto& t = TH19Tools::singleton();
        t.Open();

        t.SetSizeRel(0.5f, 1.0f);
        if (GetMemContent(RVA(P1_CPU_PTR)) && !GetMemContent(RVA(P2_CPU_PTR))) {
            t.SetPosRel(0.0f, 0.0f);
        } else if (GetMemContent(RVA(P2_CPU_PTR)) && !GetMemContent(RVA(P1_CPU_PTR))) {
            t.SetPosRel(0.5f, 0.0f);
        } else {
            t.SetPosRel(0.25f, 0.0f);
        }

        t.allow = true;
    })
    
    // In the gamemode switching code where the game switches to main menu mode
    EHOOK_DY(th19_prac_uninit, 0x11FA07, 6, {
        TH19Tools::singleton().Close();
        TH19Tools::singleton().allow = false;
    })

    EHOOK_DY(th19_invincible, 0x130ACC, 7, {
        TH19Tools& t = TH19Tools::singleton();
        if (((pCtx->Edi == *(DWORD*)RVA(P1_PTR)) && t.p1_invincible)
            || ((pCtx->Edi == *(DWORD*)RVA(P2_PTR)) && t.p2_invincible))
        {
            pCtx->Eip = RVA(0x130AD3);
        }
    })

    EHOOK_DY(th19_invincible_barrier, 0x12FBC0, 1, {
        TH19Tools& t = TH19Tools::singleton();
        DWORD p = pCtx->Ecx - 0x18;

        if (((p == *(DWORD*)RVA(P1_PTR)) && t.p1_invincible)
            || ((p == *(DWORD*)RVA(P2_PTR)) && t.p2_invincible)) {
            pCtx->Eax = 0;
            pCtx->Eip = PopHelper32(pCtx);
        }
    })
    
    EHOOK_DY(th19_lock_lives, 0x123EB5, 1, {
        Globals& globals = *(Globals*)RVA(GLOBALS);
        TH19Tools& t = TH19Tools::singleton();

        if (((pCtx->Edi == (uintptr_t)&globals.side[0]) && t.p1_lives_lock)
            || ((pCtx->Edi == (uintptr_t)&globals.side[1]) && t.p2_lives_lock))
        {
            pCtx->Eip++;
        }
    })

    EHOOK_DY(th19_lock_cpu_next_charge, 0xE9D33, 6, {
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
    })

    HOOKSET_ENDDEF()

    struct THVSSelect : public Gui::GameGuiWnd {
        THVSSelect()
        {
            SetFade(0.8f, 0.1f);
            SetStyle(ImGuiStyleVar_WindowRounding, 0.0f);
            SetStyle(ImGuiStyleVar_WindowBorderSize, 0.0f);
            SetAutoSpacing(true);
            OnLocaleChange();
        }

    public:
        Gui::GuiCombo mMode { TH_MODE, TH_MODE_SELECT };
        SINGLETON(THVSSelect);

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

    bool storymode;

    const char* cards[] = {
        "None",
        "Physical Enhancement Jizou",
        "Yin-Yang Orb",
        "Amulet of Benevolence",
        "Mini-Hakkero",
        "Hoarders' Teachings",
        "Amulet of Kanako Yasaka",
        "Amulet of Suwako Moriya",
        "Cat Shikigami (Front)",
        "Cat Shikigami (Side)",
        "Ferocious Komainu",
        "Vigilant Komainu",
        "Pendulum",
        "Nazrin's Rods",
        "The Moon Rabbit Has Landed",
        "The Urban Rabbit Has Landed",
        "Spirit Hauling Cart",
        "Feline Understanding",
        "Fox in a Tube (Vertical)",
        "Fox in a Tube (Horizontal)",
        "Mobile Leaf",
        "Just One Puff",
        "Hard-Working Otter",
        "Jittery Otter",
        "Keiga Intimidation",
        "Keiga Harsh Hierarchy",
        "Gorged Animal Spirit",
        "Bottom of the Food Chain",
        "Clingy Sake Gourd",
        "Drunken Fist, All the Time",
        "Far-Reaching Pole",
        "Kiketsu Inductee",
        "Handy/Hazardous Bear Trap",
        "Friend of the Keiga Family",
        "Extremely Cursed Blood",
        "Chupacabra's Fame",
        "Crimson Glory of Disgrace",
        "Taunting Road to Yomi",
        "Land of the Heartless",
        "Violence is Hell's Flower",
    };

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
        Gui::GuiCombo mStage { TH_STAGE, TH_STAGE_SELECT_NOEX };
        std::vector<unsigned int> mAdditionalCards = { 0 };
        SINGLETON(THGuiPrac);
    
    protected:
        virtual void OnLocaleChange() override
        {
            SetTitle(S(TH_MENU));
            SetSizeRel(0.6f, 0.8f);
            SetPosRel(0.2f, 0.1f);
            SetItemWidthRel(-0.052f);
        }
        virtual void OnContentUpdate() override {
            mMode();

            if (!*mMode) {
                return;
            }
            mStage();
            ImGui::Separator();
            ImGui::TextUnformatted("Additonal cards");
            Gui::MultiComboSelect(mAdditionalCards, cards, elementsof(cards), S(TH18_CARD_FORMAT));
        }
    };
    
    PATCH_ST(th19_vs_mode_disable_movement, 0x142131, "eb");
    PATCH_ST(th19_charsel_disable_movement, 0x140850, "90e9");

    void draw_slowdown()
    {
        UINT_PTR FPS_COUNTER = *(UINT_PTR*)RVA(FPS_COUNTER_PTR);
        double n0 = *(double*)(FPS_COUNTER + 0x28);
        double n1 = *(double*)(FPS_COUNTER + 0x30);
        double slowdown = 0.0;

        if (n1 != 0.0)
            slowdown = 100.0 - 100.0 * (n0 / n1);

        Float3 pos { 490.0f, 470.0f, 0.0f };

        Ascii__debugf(*(UINT_PTR*)RVA(ASCII_MANAGER_PTR), &pos, "Slowdown: %2.1f%%", slowdown);
    }

    HOOKSET_DEFINE(THMainHook)

    EHOOK_DY(th19_update_begin, 0xC89E0, 1, {
        GameGuiBegin(IMPL_WIN32_DX9);
    })

    EHOOK_DY(th19_update_end, 0xC8B75, 1, {
        THVSSelect::singleton().Update();
        THGuiPrac::singleton().Update();

        auto& t = TH19Tools::singleton();
        if (t.allow) {
            if (Gui::GetChordPressed(Gui::GetBackspaceMenuChord())) {
                if (t.IsOpen()) {
                    t.Close();
                } else {
                    t.Open();
                }
            }
            t.Update();
        }

        GameGuiEnd(UpdateAdvOptWindow() || THVSSelect::singleton().IsOpen() || THGuiPrac::singleton().IsOpen() || t.IsOpen());
    })

    EHOOK_DY(th19_render, 0xC8C8D, 1, {
        GameGuiRender(IMPL_WIN32_DX9);
    })

    EHOOK_DY(th19_draw_slowdown_1, 0xFCA02, 1, {
        draw_slowdown();
    })

    EHOOK_DY(th19_draw_slowdown_2, 0xFCACF, 1, {
        draw_slowdown();
    })

    EHOOK_DY(th19_vs_mode_enter, 0x14220F, 2, {
        if (*(uint32_t*)(pCtx->Edi + 0x2c) > 2) {
            return;
        }

        auto& p = THVSSelect::singleton();

        if (p.IsClosed()) {
            p.Open();
            pCtx->Eip = RVA(0x14231C);
            th19_vs_mode_disable_movement.Enable();
            DisableAllHooks(TH19PracHook);
            return;
        }

        p.Close();
        if (*p.mMode) {
            EnableAllHooks(TH19PracHook);
        }
        th19_vs_mode_disable_movement.Disable();
    })
    EHOOK_DY(th19_vs_mode_exit, 0x1421CB, 2, {
        DisableAllHooks(TH19PracHook);
        auto& p = THVSSelect::singleton();
        if (p.IsClosed()) {
            return;
        }
        p.Close();
        th19_vs_mode_disable_movement.Disable();
        pCtx->Eip = RVA(0x142204);
    })

    EHOOK_DY(th19_main_menu_confirm, 0x142F7C, 2, {
        storymode = GetMemContent(pCtx->Esi + 0x2c) == 0;
    })

    EHOOK_DY(th19_character_select_confirm, 0x14112D, 4, {
        if (!storymode)
            return;
        
        auto& g = THGuiPrac::singleton();

        if (g.IsClosed()) {            
            g.Open();
            th19_charsel_disable_movement.Enable();

            pCtx->Eip = RVA(0x1412AC);
            return;
        } else {
            g.Close();
            th19_charsel_disable_movement.Disable();
        }
    })

    EHOOK_DY(th19_character_select_abort, 0x141239, 5, {
        auto& g = THGuiPrac::singleton();

        if (g.IsOpen()) {
            g.Close();
            pCtx->Eip = RVA(0x141301);
        }
    })

    EHOOK_DY(th19_story_force_stage, 0x141568, 10, {
        Globals& globals = *(Globals*)RVA(GLOBALS);
        auto& g = THGuiPrac::singleton();
        if (!storymode || !*g.mMode) {
            DisableAllHooks(TH19PracHook);
            TH19Tools::singleton().allow = false;
            TH19Tools::singleton().Close();
    
            return;
        }

        // Globals::
        globals.story_stage = *g.mStage + 1;
        
        for (const auto i : g.mAdditionalCards) {
            AddCard(GetMemContent(RVA(P1_ABILITYMANAGER)), i - 1, 1);
        }

        EnableAllHooks(TH19PracHook);
        TH19Tools::singleton().allow = true;
        TH19Tools::singleton().Open();

    })

    HOOKSET_ENDDEF()
    static __declspec(noinline) void THGuiCreate()
    {
        if (ImGui::GetCurrentContext()) {
            return;
        }
        th19_vs_mode_disable_movement.Setup();
        th19_charsel_disable_movement.Setup();

        // Init
        GameGuiInit(IMPL_WIN32_DX9, RVA(D3D_DEVICE), RVA(HWND_PTR),
            Gui::INGAGME_INPUT_GEN2, GetMemContent(RVA(0x1AE3A0)) + 0x30 + 0x2B0, GetMemContent(RVA(0x1AE3A0)) + 0x30 + 0x10, 0,
            -2, SCALE, 0.0f);

        SetDpadHook(0xAB7C1, 3);

        //// Gui components creation
        // THOverlay::singleton();
        THVSSelect::singleton();
        TH19Tools::singleton();
        //
        // Hooks
        EnableAllHooks(THMainHook);
    }
    HOOKSET_DEFINE(THInitHook)
    // in the part of MainMenu::on_tick responsible for the title screen menu itself
    EHOOK_DY(th19_gui_init_1, 0x1439A2, 3, {
        self->Disable();
        THGuiCreate();
    })

    // After InputManager is initialized
    EHOOK_DY(th19_gui_init_2, 0x2A8A, 1,{
        self->Disable();
        THGuiCreate();
    })
    HOOKSET_ENDDEF()
}

}
void TH19_v1_00a_Init()
{
    ingame_image_base = CurrentImageBase;
    EnableAllHooks(TH19::V1_00a::THInitHook);
}
}