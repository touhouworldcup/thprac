#include "thprac_utils.h"
#include "thprac_th19.h"

// Part of a fix to improve some performance issues
// To make it work I replace certain functions in the game's code. In order
// for that replacement to work, my code needs to match the calling convention
// of the game's original function EXACTLY. The problem here is that during
// compilation of TH19, MSVC decided to make up calling conventions, that you
// cannot explicitly tell MSVC to replicate.
//
// One of the functions I'm replacing has a calling convention where 4 float
// parameters are taken in the first 4 XMM registers, and 4 more floats are on
// the stack. This is easy enough to replicate with vectorcall. In vectorcall,
// the first 6 float parameters are in the first 6 XMM registers, with the
// remaining ones on the stack. To replicate that I can just declare my first 4
// parameters, declare two dummy parameters, then declare the final 4.
//
// Problem: that function does caller stack cleanup (meaning that whoever calls
// that function must clean the stack parameters off the stack themselves).
// Vectorcall does callee cleanup, meaning that the function being called is
// itself responsible for cleaning up the parameters that were passed on the
// stack. Trying to do both callee and caller stack cleanup will crash
//
// It is impossible to explicitly tell MSVC to generate a function with a
// calling convention like that. The way I made the build that I sent you is by
// binary hacking thprac.exe to modify my replacement function to do caller
// stack cleanup.
//
// To work around this, th19_fast_msvc.obj is precompiled, and binary hacked to
// replace all ret 14 instructions with just ret. This is the only way to get
// a function out of MSVC that takes parameters in XMM registers and does
// caller stack cleanup.

#ifndef _DEBUG
extern "C" {
uint32_t* __vectorcall CPUHitInf_CheckColliders(void*, int, uint32_t*, float, float, float);
bool __vectorcall _RxD1E00_fast(uint32_t, uint32_t, float, float, float, float, float, float, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);
}
#else
// why
extern "C" {
uint32_t* __vectorcall CPUHitInf_CheckColliders(void*, int, uint32_t*, float, float, float) {return 0;};
bool __vectorcall _RxD1E00_fast(uint32_t, uint32_t, float, float, float, float, float, float, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t) {return 0;};
}
#endif

namespace THPrac {
namespace TH19 {
namespace V1_10c {

#define Gui__UpdateHearts asm_call_rel<0x1141C0, Thiscall>
#define PlayerBarrier__restore asm_call_rel<0xB420, Thiscall>
#define AnmInterrupt asm_call_rel<0xCB830, Stdcall>
#define Ascii__debugf asm_call_rel<0xE75A0, Cdecl>
#define AddCard asm_call_rel<0xDEBA0, Thiscall>

    enum addrs {
        GLOBALS = 0x22B0C0,
        GUI_PTR = 0x1D1A50,
        ASCII_MANAGER_PTR = 0x1D1A34,

        P1_PTR = 0x1D1A64,
        P2_PTR = 0x1D1AA0,

        P1_CPU_PTR = 0x1D1A94,
        P2_CPU_PTR = 0x1D1AD0,

        P1_ABILITYMANAGER = 0x1D1A80,

        LATENCY_SETTING = 0x22BDE1,
        FPS_COUNTER_PTR = 0x1D1A4C,

        FPS_USE_IN_CODE = 0xB6090,

        D3D_DEVICE = 0x22BB48,
        HWND_PTR = 0x22BB88,
        WNDPROC_ADDR = 0xB6A30,

    };
        
    #define SCALE (*(float*)RVA(0x22EEB0))

#ifndef _DEBUG
    static uint8_t cpu_check_collider_patch_code[] = {
        0xe9, 0x00, 0x00, 0x00, 0x00, 0xcc, 0xcc, 0xcc,
    };
    static uint8_t patch_RxD1E00_code[] = {
        0xe9, 0x00, 0x00, 0x00, 0x00, 0xcc, 0xcc, 0xcc,
    };

    HookCtx th19_patch_cpu_check_colliders(0xF8F60, (char*)cpu_check_collider_patch_code, sizeof(cpu_check_collider_patch_code));
    HookCtx th19_patch_RxD1E00(0xD1E00, (char*)patch_RxD1E00_code, sizeof(patch_RxD1E00_code));
 #else
    // why
        static uint8_t cpu_check_collider_patch_code[] = {
        0xe9, 0x00, 0x00, 0x00, 0x00, 0xcc, 0xcc, 0xcc,
    };
    static uint8_t patch_RxD1E00_code[] = {
        0xe9, 0x00, 0x00, 0x00, 0x00, 0xcc, 0xcc, 0xcc,
    };

    HookCtx th19_patch_cpu_check_colliders(0xF8F60, (char*)cpu_check_collider_patch_code, sizeof(cpu_check_collider_patch_code));
    HookCtx th19_patch_RxD1E00(0xD1E00, (char*)patch_RxD1E00_code, sizeof(patch_RxD1E00_code));

#endif

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
                mOptCtx.fps_dbl = 60.0;
                VirtualProtect((void*)RVA(FPS_USE_IN_CODE), 4, oldProtect, &oldProtect);
            } else
                mOptCtx.fps_status = 0;
        }
        void FpsSet()
        {
            if (mOptCtx.fps_status == 1) {
                mOptCtx.fps_dbl = (double)mOptCtx.fps;
            }
        }
        void GameplayInit()
        {
        }
        void GameplaySet()
        {
        }

        bool perf_fix = false;

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

#ifndef _DEBUG
            if (BeginOptGroup<TH_PERFORMANCE>()) {
                if (ImGui::Checkbox("Replace certain functions with faster variants", &this->perf_fix)) {
                    if (this->perf_fix) {
                        th19_patch_cpu_check_colliders.Enable();
                        th19_patch_RxD1E00.Enable();
                    } else {
                        th19_patch_cpu_check_colliders.Disable();
                        th19_patch_RxD1E00.Disable();
                    }
                }
                EndOptGroup();
            }
#else
            //why
            if (BeginOptGroup<TH_PERFORMANCE>()) {
                if (ImGui::Checkbox("Replace certain functions with faster variants", &this->perf_fix)) {
                    if (this->perf_fix) {
                        th19_patch_cpu_check_colliders.Enable();
                        th19_patch_RxD1E00.Enable();
                    } else {
                        th19_patch_cpu_check_colliders.Disable();
                        th19_patch_RxD1E00.Disable();
                    }
                }
                EndOptGroup();
            }
#endif
            
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
            SetSizeRel(0.5, 1);
            OnLocaleChange();

            th19_enemy_tick.Setup();
        }

        EHOOK_ST(th19_enemy_tick, 0x1089D0)
        {
            drawEnemyHP(pCtx, SCALE);
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
                int* ptr = GetMemAddr<int*>(addr, 0x30, 0x36EEB0);

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
            SetTitle("TH19 Tools###_th19_tools");
        }
        SINGLETON(TH19Tools);
    };


    HOOKSET_DEFINE(TH19PracHook)

    // In the loader thread, right before the instruction that tells the main thread that loading has finished
    EHOOK_DY(th19_prac_init, 0x118D5E) {
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
    }
    
    // When gamemode is changed from 7
    EHOOK_DY(th19_prac_uninit, 0x132E3D) {
        if (GetMemContent(pCtx->Esi + 0x90C) == 7) {
            TH19Tools::singleton().Close();
            TH19Tools::singleton().allow = false;
        }
    }

    EHOOK_DY(th19_invincible, 0x145EE3) {
        TH19Tools& t = TH19Tools::singleton();
        if (((pCtx->Edi == *(DWORD*)RVA(P1_PTR)) && t.p1_invincible)
            || ((pCtx->Edi == *(DWORD*)RVA(P2_PTR)) && t.p2_invincible))
        {
            pCtx->Eip = RVA(0x145EF0);
        }
    }

    EHOOK_DY(th19_invincible_barrier, 0x144C90) {
        TH19Tools& t = TH19Tools::singleton();
        DWORD p = pCtx->Ecx - 0x18;

        if (((p == *(DWORD*)RVA(P1_PTR)) && t.p1_invincible)
            || ((p == *(DWORD*)RVA(P2_PTR)) && t.p2_invincible)) {
            pCtx->Eax = 0;
            pCtx->Eip = PopHelper32(pCtx);
        }
    }
    
    EHOOK_DY(th19_lock_lives, 0x137795) {
        Globals& globals = *(Globals*)RVA(GLOBALS);
        TH19Tools& t = TH19Tools::singleton();

        if (((pCtx->Edi == (uintptr_t)&globals.side[0]) && t.p1_lives_lock)
            || ((pCtx->Edi == (uintptr_t)&globals.side[1]) && t.p2_lives_lock))
        {
            pCtx->Eip++;
        }
    }

    EHOOK_DY(th19_lock_cpu_next_charge, 0xFA722)
    {
        auto ptr = pCtx->Ebx;
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
            pCtx->Eip = RVA(0xFA728);
        }
    }

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
    
    PATCH_ST(th19_vs_mode_disable_movement, 0x159482, "\xeb", 1);
    PATCH_ST(th19_charsel_disable_movement, 0x1571B7, "\x90\xe9", 2);

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

    EHOOK_DY(th19_update_begin, 0xD7000) {
        GameGuiBegin(IMPL_WIN32_DX9);
    }

    EHOOK_DY(th19_update_end, 0xD715F) {
        #if 1
        THVSSelect::singleton().Update();
        THGuiPrac::singleton().Update();

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
        #endif
        GameGuiEnd(UpdateAdvOptWindow() || THVSSelect::singleton().IsOpen() || THGuiPrac::singleton().IsOpen() || t.IsOpen());
    }

    EHOOK_DY(th19_render, 0xD72C0) {
        GameGuiRender(IMPL_WIN32_DX9);
    }
    EHOOK_DY(th19_draw_slowdown_1, 0x10E972)
    {
        draw_slowdown();
    }

    EHOOK_DY(th19_draw_slowdown_2, 0x10EA3F)
    {
        draw_slowdown();
    }

    EHOOK_DY(th19_vs_mode_enter, 0x159577)
    {
        if (*(uint32_t*)(pCtx->Edi + 0x2c) > 2) {
            return;
        }

        auto& p = THVSSelect::singleton();

        if (p.IsClosed()) {
            p.Open();
            pCtx->Eip = RVA(0x159680);
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
    EHOOK_DY(th19_vs_mode_exit, 0x159532) {
        TH19PracHook::singleton().DisableAllHooks();
        auto& p = THVSSelect::singleton();
        if (p.IsClosed()) {
            return;
        }
        p.Close();
        th19_vs_mode_disable_movement.Disable();
        pCtx->Eip = RVA(0x15956B);
    }

    EHOOK_DY(th19_main_menu_confirm, 0x15A455) {
        storymode = GetMemContent(pCtx->Esi + 0x2C) == 0;
    }

    EHOOK_DY(th19_character_select_confirm, 0x157C8A) {
        if (!storymode)
            return;
        
        auto& g = THGuiPrac::singleton();

        if (g.IsClosed()) {            
            g.Open();
            th19_charsel_disable_movement.Enable();

            pCtx->Eip = RVA(0x157D64);
            return;
        } else {
            g.Close();
            th19_charsel_disable_movement.Disable();
        }
    }

    EHOOK_DY(th19_character_select_abort, 0x157D87)
    {
        auto& g = THGuiPrac::singleton();

        if (g.IsOpen()) {
            g.Close();
            pCtx->Eip = RVA(0x157E6E);
        }
    }

    EHOOK_DY(th19_story_force_stage, 0x158423)
    {
        Globals& globals = *(Globals*)RVA(GLOBALS);
        auto& g = THGuiPrac::singleton();
        if (!storymode || !*g.mMode) {
            TH19PracHook::singleton().DisableAllHooks();
            TH19Tools::singleton().allow = false;
            TH19Tools::singleton().Close();
    
            return;
        }

        // Globals::
        globals.story_stage = *g.mStage + 1;
        
        for (const auto i : g.mAdditionalCards) {
            AddCard(GetMemContent(RVA(P1_ABILITYMANAGER)), i - 1, 1);
        }

        TH19PracHook::singleton().EnableAllHooks();
        TH19Tools::singleton().allow = true;
        TH19Tools::singleton().Open();

    }
    HOOKSET_ENDDEF()

    HOOKSET_DEFINE(THInitHook)
    static __declspec(noinline) void THGuiCreate()
    {
        *(uintptr_t*)(cpu_check_collider_patch_code + 1) = (uintptr_t)&CPUHitInf_CheckColliders - RVA((uintptr_t)th19_patch_cpu_check_colliders.mTarget + 5);
        *(uintptr_t*)(patch_RxD1E00_code + 1) = (uintptr_t)&_RxD1E00_fast - RVA((uintptr_t)th19_patch_RxD1E00.mTarget + 5);

        th19_vs_mode_disable_movement.Setup();
        th19_charsel_disable_movement.Setup();

        th19_patch_cpu_check_colliders.Setup();
        th19_patch_RxD1E00.Setup();

        // Init
        GameGuiInit(IMPL_WIN32_DX9, RVA(D3D_DEVICE), RVA(HWND_PTR),
            Gui::INGAGME_INPUT_GEN2, GetMemContent(RVA(0x1D19B0)) + 0x30 + 0x2B0, GetMemContent(RVA(0x1D19B0)) + 0x30 + 0x10, 0,
            -2, SCALE, 0.0f);

        //// Gui components creation
        //THOverlay::singleton();
        THGuiPrac::singleton();
        THVSSelect::singleton();
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
    EHOOK_DY(th19_gui_init_1, 0x15AF9C)
    {
        THGuiCreate();
        THInitHookDisable();
    }

    // After InputManager is initialized
    EHOOK_DY(th19_gui_init_2, 0x134220)
    {
        THGuiCreate();
        THInitHookDisable();
    }
    HOOKSET_ENDDEF()
}
}

void TH19_v1_10c_Init()
{
    ingame_image_base = (uintptr_t)GetModuleHandleW(NULL);
    TH19::V1_10c::THInitHook::singleton().EnableAllHooks();
}
}