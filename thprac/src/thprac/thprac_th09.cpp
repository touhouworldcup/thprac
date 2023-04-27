#include "thprac_utils.h"


namespace THPrac {
namespace TH09 {
    PATCH_ST(th09_ranklock, 0x41ac7f, "\xEB", 1);

    class TH09Tools : public Gui::GameGuiWnd {
        TH09Tools() {
            SetWndFlag(ImGuiWindowFlags_NoCollapse);
            SetFade(0.8f, 0.8f);
            SetStyle(ImGuiStyleVar_WindowRounding, 0.0f);
            SetStyle(ImGuiStyleVar_WindowBorderSize, 0.0f);
            SetSize(320, 480);
            SetAutoSpacing(true);
            SetItemWidthRel(0.0f);
            OnLocaleChange();

            th09_ranklock.Setup();
        }
        SINGLETON(TH09Tools);
    public:
        bool enabled = false;

        bool invinc_p1 = false;
        bool invinc_p2 = false;
        bool infhealth_p1 = false;
        bool infhealth_p2 = false;
        bool chargelock_p1 = false;
        bool chargelock_p2 = false;
        bool cpu_lock_attack_p1 = false;
        bool cpu_lock_attack_p2 = false;
        bool ranklock = false;
        bool o_lily = true;
        bool o_fairy = true;
        bool o_pellets_random = true;
        bool o_pellets_rival = true;
        bool o_large_bullets = true;
        bool o_spirits_random = true;
        bool o_spirits_rival = true;
        bool o_ex = true;
        bool o_lv2 = true;
        bool o_lv3 = true;
        bool o_boss = true;
        int justOpened = 0; // Very hacky
    protected:
        int cpu_charge_p1;
        int cpu_charge_p2;
        virtual void OnLocaleChange() override {
            this->SetTitle(S(TH09_TOOLS_TITLE));
        }
        virtual void OnPreUpdate() override {
            if (justOpened) {
                ImGui::SetWindowFocus(nullptr);
                justOpened--;
            }
        }
        virtual void OnContentUpdate() override {
            uint32_t pl1 = *(uint32_t*)0x4a7d94;
            uint32_t pl2 = *(uint32_t*)0x4a7dcc;

            ImGui::PushID(TH_MUTEKI);
            ImGui::TextUnformatted(S(TH_MUTEKI));
            ImGui::Checkbox(S(TH09_P1), &invinc_p1);
            ImGui::SameLine();
            ImGui::Checkbox(S(TH09_P2), &invinc_p2);
            ImGui::PopID();

            ImGui::PushID(TH_HEALTH);
            ImGui::TextUnformatted(S(TH_HEALTH));
            ImGui::PushID(TH09_P1);
            ImGui::SliderInt("", (int*)(pl1 + 0xa8), 1, 10, S(TH09_P1_FORMAT_INT));
            ImGui::SameLine();
            ImGui::Checkbox(S(TH09_LOCK), &infhealth_p1);
            ImGui::PopID();
            ImGui::PushID(TH09_P2);
            ImGui::SliderInt("", (int*)(pl2 + 0xa8), 1, 10, S(TH09_P2_FORMAT_INT));
            ImGui::SameLine();
            ImGui::Checkbox(S(TH09_LOCK), &infhealth_p2);
            ImGui::PopID();
            ImGui::PopID();

            ImGui::PushID(TH09_INSTANT_DEATH);
            ImGui::TextUnformatted(S(TH09_INSTANT_DEATH));
            ImGui::PushID(TH09_P1);
            if (ImGui::Button(S(TH09_P1))) {
                *(uint32_t*)(pl1 + 0xa8) = 0;
                uint64_t charge_bak = *(uint64_t*)(pl1 + 0x30384);
                *(uint64_t*)(pl1 + 0x30384) = 0;
                asm_call<0x41e420, Thiscall>(pl1);
                *(uint64_t*)(pl1 + 0x30384) = charge_bak;
            }
            ImGui::PopID();
            ImGui::SameLine();
            ImGui::PushID(TH09_P2);
            if (ImGui::Button(S(TH09_P2))) {
                *(uint32_t*)(pl2 + 0xa8) = 0;
                uint64_t charge_bak = *(uint64_t*)(pl2 + 0x30384);
                *(uint64_t*)(pl2 + 0x30384) = 0;
                asm_call<0x41e420, Thiscall>(pl2);
                *(uint64_t*)(pl2 + 0x30384) = charge_bak;
            }
            ImGui::PopID();
            ImGui::PopID();

            auto chargegauge = [&](uint32_t pl, bool* lock_check) {
                float bsize = ImGui::GetFrameHeight();
                ImGuiStyle& style = ImGui::GetStyle();
                const ImVec2 backup_frame_padding = style.FramePadding;
                style.FramePadding.x = style.FramePadding.y;

                float* gauge = (float*)(pl + 0x30388);
                const char* _p;
                if (*(uint32_t*)(pl + 8)) {
                    _p = S(TH09_P2_FORMAT_FLOAT);
                    ImGui::PushID(TH09_P2);
                } else {
                    _p = S(TH09_P1_FORMAT_FLOAT);
                    ImGui::PushID(TH09_P1);
                }
                ImGui::SliderFloat("", gauge, 0, 400, _p);
                ImGui::SameLine(0, style.ItemInnerSpacing.x);
                if (ImGui::Button("-", ImVec2(bsize, bsize)) && *gauge > 0) {
                    *gauge -= 1.0f;
                    *gauge = static_cast<float>(RoundDown(static_cast<long>(*gauge), 100));
                }
                ImGui::SameLine(0, style.ItemInnerSpacing.x);
                if (ImGui::Button("+", ImVec2(bsize, bsize)) && *gauge < 400) {
                    *gauge += 1.0f;
                    *gauge = static_cast<float>(RoundUp(static_cast<long>(*gauge), 100));
                }
                style.FramePadding = backup_frame_padding;
                ImGui::SameLine();
                ImGui::PushID(TH09_LOCK);
                ImGui::Checkbox("", lock_check);
                ImGui::PopID();
                if (ImGui::IsItemHovered()) {
                    ImGui::BeginTooltip();
                    ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
                    ImGui::TextUnformatted(S(TH09_LOCK));
                    ImGui::PopTextWrapPos();
                    ImGui::EndTooltip();
                }
                ImGui::PopID();
            };

            ImGui::PushID(TH09_CHARGE_GAUGE);
            ImGui::TextUnformatted(S(TH09_CHARGE_GAUGE));
            chargegauge(pl1, &chargelock_p1);
            chargegauge(pl2, &chargelock_p2);
            ImGui::PopID();

            if (*(uint32_t*)0x4a7db8 || *(uint32_t*)0x4a7df0) {
                ImGui::PushID(TH09_CPU_CHARGE);
                ImGui::TextUnformatted(S(TH09_CPU_CHARGE));
                if (*(uint32_t*)0x4a7db8) {
                    ImGui::PushID(TH09_P1);
                    cpu_charge_p1 = (int)*(float*)(pl1 + 0x7c) / 100;
                    if (ImGui::SliderInt(S(TH09_P1), &cpu_charge_p1, 1, 4)) {
                        *(float*)(pl1 + 0x7c) = static_cast<float>(cpu_charge_p1) * 100;
                    }
                    ImGui::SameLine();
                    ImGui::Checkbox(S(TH09_LOCK), &cpu_lock_attack_p1);
                    ImGui::PopID();
                }
                if (*(uint32_t*)0x4a7df0) {
                    ImGui::PushID(TH09_P2);
                    cpu_charge_p2 = (int)*(float*)(pl2 + 0x7c) / 100;
                    if (ImGui::SliderInt(S(TH09_P2), &cpu_charge_p2, 1, 4)) {
                        *(float*)(pl2 + 0x7c) = static_cast<float>(cpu_charge_p2) * 100;
                    }
                    ImGui::SameLine();
                    ImGui::Checkbox(S(TH09_LOCK), &cpu_lock_attack_p2);
                    ImGui::PopID();
                }
                ImGui::PopID();
            }

            ImGui::PushID(TH09_RANK);
            ImGui::TextUnformatted(S(TH09_RANK));
            ImGui::SliderInt(S(TH09_RANK), (int*)0x4a7e44, 1, *(uint32_t*)0x4a7e58);
            ImGui::InputInt(S(TH09_RANK_MAX), (int*)0x4a7e58, 1, 5);
            ImGui::InputInt(S(TH09_RANK_INC), (int*)0x4a7e54, 1, 1);
            if (ImGui::Checkbox(S(TH09_LOCK), &ranklock)) {
                th09_ranklock.Toggle(ranklock);
            }
            ImGui::PopID();

            ImGui::PushID(TH09_LV_RANK);
            ImGui::TextUnformatted(S(TH09_LV_RANK));
            ImGui::SliderInt(S(TH09_P1), (int*)(pl1 + 0xa0), 1, 16);
            ImGui::SliderInt(S(TH09_P2), (int*)(pl2 + 0xa0), 1, 16);
            ImGui::PopID();

            ImGui::PushID(TH09_BOSS_RANK);
            ImGui::TextUnformatted(S(TH09_BOSS_RANK));
            ImGui::SliderInt(S(TH09_P1), (int*)(pl1 + 0xa4), 1, 16);
            ImGui::SliderInt(S(TH09_P2), (int*)(pl2 + 0xa4), 1, 16);
            ImGui::PopID();

            ImGui::PushID(TH09_MISC);
            ImGui::TextUnformatted(S(TH09_MISC));
            ImGui::SliderInt(S(TH09_AI_POWER), (int*)0x4a7df4, 1, 60);
            ImGui::InputInt(S(TH09_LILY_TIMER), (int*)0x4a7e5c, 1, 3);
            ImGui::PopID();

            if (BeginOptGroup<TH09_OCCURENCE>()) {
                ImGui::Checkbox(S(TH09_O_LILY), &o_lily);
                ImGui::Checkbox(S(TH09_O_FAIRY), &o_fairy);
                ImGui::Checkbox(S(TH09_O_PELLETS_RANDOM), &o_pellets_random);
                ImGui::Checkbox(S(TH09_O_PELLETS_RIVAL), &o_pellets_rival);
                ImGui::Checkbox(S(TH09_O_LARGE_BULLETS), &o_large_bullets);
                ImGui::Checkbox(S(TH09_O_SPIRITS_RANDOM), &o_spirits_random);
                ImGui::Checkbox(S(TH09_O_SPIRITS_RIVAL), &o_spirits_rival);
                ImGui::Checkbox(S(TH09_O_EX), &o_ex);
                ImGui::Checkbox(S(TH09_O_LV2), &o_lv2);
                ImGui::Checkbox(S(TH09_O_LV3), &o_lv3);
                ImGui::Checkbox(S(TH09_O_BOSS), &o_boss);
                EndOptGroup();
            }

            auto show_stats = [](uint32_t b) {
                ImGui::TextUnformatted(S(TH_BULLETS));
                ImGui::Indent();
                ImGui::Text(S(TH09_STAT_B_FAIRY), *(uint32_t*)(b + 0x25e164));
                ImGui::Text(S(TH09_STAT_B_RIVAL), *(uint32_t*)(b + 0x25e168));
                ImGui::Text(S(TH09_STAT_B_TOTAL), *(uint32_t*)(b + 0x25e16c));
                ImGui::Unindent();
            };

            ImGui::TextUnformatted(S(TH09_STATS));
            if (BeginOptGroup<TH09_PLAYER_1>()) {
                show_stats(*(uint32_t*)0x4a7d98);
                EndOptGroup();
            }
            if (BeginOptGroup<TH09_PLAYER_2>()) {
                show_stats(*(uint32_t*)0x4a7dd0);
                EndOptGroup();
            }
        }
    };

    PATCH_ST(th09_disable_map_select, 0x424671, "\xC2\x04\x00", 3);

    void th09_chargelock_bomb(PCONTEXT pCtx)
    {
        TH09Tools& t = TH09Tools::singleton();
        uint32_t side = *(uint32_t*)(pCtx->Esi + 0x8);
        if ((side == 0 && t.chargelock_p1) || (side == 1 && t.chargelock_p2))
            pCtx->Eip += 12;
    }

    void th09_chargelock_ret(PCONTEXT pCtx)
    {
        TH09Tools& t = TH09Tools::singleton();
        uint32_t side = *(uint32_t*)(pCtx->Ecx + 0x8);
        if ((side == 0 && t.chargelock_p1) || (side == 1 && t.chargelock_p2)) {
            pCtx->Eip = PopHelper32(pCtx);
            pCtx->Esp += 4;
        }
    }

    HOOKSET_DEFINE(TH09PracHook)
    EHOOK_DY(th09_invincible, 0x41e8ec)
    {
        TH09Tools& t = TH09Tools::singleton();
        uint32_t side = *(uint32_t*)(pCtx->Esi + 0x8);
        if ((side == 0 && t.invinc_p1) || (side == 1 && t.invinc_p2))
            pCtx->Eip = 0x41e8f1;
    }
    EHOOK_DY(th09_infhealth, 0x41e5fc)
    {
        TH09Tools& t = TH09Tools::singleton();
        uint32_t side = *(uint32_t*)(pCtx->Esi + 0x8);
        if ((side == 0 && t.infhealth_p1) || (side == 1 && t.infhealth_p2))
            pCtx->Eip = 0x41e63c;
    }
    EHOOK_DY(th09_cpu_lock_attack, 0x404f80)
    {
        TH09Tools& t = TH09Tools::singleton();
        uint32_t side = *(uint32_t*)(pCtx->Esi - 0x1c);
        if ((side == 0 && t.cpu_lock_attack_p1) || (side == 1 && t.cpu_lock_attack_p2))
            pCtx->Eip = 0x404fe1;
    }
    EHOOK_DY(th09_o_lily, 0x41ad66)
    {
        if (!TH09Tools::singleton().o_lily) {
            pCtx->Eip = 0x41ae30;
        }
    }
    EHOOK_DY(th09_o_fairy, 0x40fd0e)
    {
        if (!TH09Tools::singleton().o_fairy) {
            pCtx->Eip = 0x40ff4d;
        }
    }
    EHOOK_DY(th09_o_pellets_random, 0x41d535)
    {
        if (!TH09Tools::singleton().o_pellets_random) {
            pCtx->Eip = 0x41d6dd;
        }
    }
    EHOOK_DY(th09_o_pellets_rival, 0x41e1a6)
    {
        if (!TH09Tools::singleton().o_pellets_rival) {
            pCtx->Eip = 0x41e2c5;
        }
    }
    EHOOK_DY(th09_o_large_bullets, 0x41dd0d)
    {
        if (!TH09Tools::singleton().o_large_bullets) {
            pCtx->Eip = 0x41dea6;
        }
    }
    EHOOK_DY(th09_o_spirits_random, 0x41d3a4)
    {
        if (!TH09Tools::singleton().o_spirits_random) {
            pCtx->Eip = 0x41d526;
        }
    }
    EHOOK_DY(th09_o_spirits_rival, 0x4105d4)
    {
        if (!TH09Tools::singleton().o_spirits_rival) {
            pCtx->Eip = 0x41071d;
        }
    }
    EHOOK_DY(th09_o_ex, 0x41d389)
    {
        if (!TH09Tools::singleton().o_ex) {
            pCtx->Eip = 0x41d38c;
        }
    }
    EHOOK_DY(th09_o_lv2, 0x441350)
    {
        if (!TH09Tools::singleton().o_lv2) {
            pCtx->Eip = PopHelper32(pCtx);
        }
    }
    EHOOK_DY(th09_o_lv3, 0x4413d0)
    {
        if (!TH09Tools::singleton().o_lv3) {
            pCtx->Eip = PopHelper32(pCtx);
        }
    }
    EHOOK_DY(th09_o_boss, 0x441420)
    {
        if (!TH09Tools::singleton().o_boss) {
            pCtx->Eip = PopHelper32(pCtx);
        }
    }
    EHOOK_DY(th09_unpause, 0x434ad8)
    {
        ImGui::SetWindowFocus(nullptr);
    }
    EHOOK_DY(th09_game_end, 0x41b82a)
    {
        TH09Tools& t = TH09Tools::singleton();
        t.Close();
        t.invinc_p1 = false;
        t.invinc_p2 = false;
        t.infhealth_p1 = false;
        t.infhealth_p2 = false;
        t.chargelock_p1 = false;
        t.chargelock_p2 = false;
        t.cpu_lock_attack_p1 = false;
        t.cpu_lock_attack_p2 = false;
        if (t.ranklock) {
            th09_ranklock.Disable();
            t.ranklock = false;
        }
        t.o_lily = true;
        t.o_fairy = true;
        t.o_pellets_random = true;
        t.o_pellets_rival = true;
        t.o_large_bullets = true;
        t.o_spirits_random = true;
        t.o_spirits_rival = true;
        t.o_ex = true;
        t.o_lv2 = true;
        t.o_lv3 = true;
        t.o_boss = true;
        t.enabled = false;
        TH09PracHook::singleton().DisableAllHooks();
    }
#define CHARGELOCK_BOMB(addr) EHOOK_DY(th09_chargelock_bomb_##addr, addr) { th09_chargelock_bomb(pCtx); }
    CHARGELOCK_BOMB(0x41ca86);
    CHARGELOCK_BOMB(0x41cb39);
    CHARGELOCK_BOMB(0x41cab9);
    CHARGELOCK_BOMB(0x41cafa);
    CHARGELOCK_BOMB(0x41e563);
    CHARGELOCK_BOMB(0x41e4e3);
    CHARGELOCK_BOMB(0x41e51c);
    CHARGELOCK_BOMB(0x41e5ac);
#undef CHARGELOCK_BOMB
#define CHARGELOCK_RET(addr) EHOOK_DY(th09_chargelock_ret_##addr, addr) { th09_chargelock_ret(pCtx); }
    CHARGELOCK_RET(0x41f310);
    CHARGELOCK_RET(0x41bc90);
#undef CHARGELOCK_RET
    HOOKSET_ENDDEF()

    class THGuiPrac : public Gui::GameGuiWnd {
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
        bool allow = false;
        SINGLETON(THGuiPrac);
    protected:
        virtual void OnLocaleChange() override
        {
            SetTitle(S(TH_MENU));
            SetSize(200, 38);
            SetPos(220, 74);
            SetItemWidthRel(-0.052f);
        }
        virtual void OnContentUpdate() override { mMode(); }
    };

    class THAdvOptWnd : public Gui::GameGuiWnd {
        // Option Related Functions
    private:
        void FpsInit()
        {
            mOptCtx.vpatch_base = (int32_t)GetModuleHandleW(L"vpatch_th09.dll");
            if (mOptCtx.vpatch_base) {
                uint64_t hash[2];
                CalcFileHash(L"vpatch_th09.dll", hash);
                if (hash[0] != 8777309807944811310ll || hash[1] != 16244273824227920047ll)
                    mOptCtx.fps_status = -1;
                else if (*(int32_t*)(mOptCtx.vpatch_base + 0x17024) == 0) {
                    mOptCtx.fps_status = 2;
                    mOptCtx.fps = *(int32_t*)(mOptCtx.vpatch_base + 0x17034);
                }
            } else
                mOptCtx.fps_status = 0;
        }
        void FpsSet()
        {
            if (mOptCtx.fps_status == 1) {
                mOptCtx.fps_dbl = 1.0 / (double)mOptCtx.fps;
            } else if (mOptCtx.fps_status == 2) {
                *(int32_t*)(mOptCtx.vpatch_base + 0x15a3c) = mOptCtx.fps;
                *(int32_t*)(mOptCtx.vpatch_base + 0x17034) = mOptCtx.fps;
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
        if (Gui::KeyboardInputUpdate(VK_F12) == 1) {
            if (advOptWnd->IsOpen())
                advOptWnd->Close();
            else
                advOptWnd->Open();
        }
        advOptWnd->Update();

        return advOptWnd->IsOpen();
    }

    HOOKSET_DEFINE(THMainHook)
    EHOOK_DY(th09_map_confirm, 0x4263d5)
    {
        THGuiPrac& g = THGuiPrac::singleton();
        if (g.IsClosed()) {
            th09_disable_map_select.Enable();
            g.Open();
            pCtx->Eip = 0x4266ad;
        } else {
            th09_disable_map_select.Disable();
            g.allow = true;
            g.Close();
        }
    }
    EHOOK_DY(th09_map_select_cancel, 0x426507)
    {
        THGuiPrac& g = THGuiPrac::singleton();
        if (g.IsOpen()) {
            th09_disable_map_select.Disable();
            g.Close();
            pCtx->Eip = 0x4266ad;
        }
    }
    EHOOK_DY(th09_update, 0x42c7b5)
    {
        GameGuiBegin(IMPL_WIN32_DX8);

        // Gui components update
        TH09Tools& t = TH09Tools::singleton();
        THGuiPrac& p = THGuiPrac::singleton();
        if (t.enabled) {
            if (Gui::KeyboardInputUpdate(VK_F11) == 1) {
                if (t.IsOpen())
                    t.Close();
                else
                    t.Open();
            }
        }
        t.Update();
        p.Update();

        GameGuiEnd(UpdateAdvOptWindow() || t.IsOpen() || p.IsOpen());
    }
    EHOOK_DY(th09_render, 0x42dd51)
    {
        GameGuiRender(IMPL_WIN32_DX8);
    }
    EHOOK_DY(th09_game_init, 0x41b5c5)
    {
        THGuiPrac& g = THGuiPrac::singleton();
        if (g.allow && *g.mMode) {
            g.allow = false;
            TH09PracHook::singleton().EnableAllHooks();
            TH09Tools& t = TH09Tools::singleton();
            t.enabled = true;
            uint32_t pl1 = *(uint32_t*)0x4a7d94;
            uint32_t pl2 = *(uint32_t*)0x4a7dcc;
            if (*(uint32_t*)0x4a7db8) {
                t.SetPos(0, 0);
                t.Open();
                t.justOpened = 2;
            } else if (*(uint32_t*)0x4a7df0) {
                t.SetPos(320, 0);
                t.Open();
                t.justOpened = 2;
            }
        }
        g.Close();
    }
    EHOOK_DY(th09_gui_reinit, 0x42e50f)
    {
        GameGuiInit(IMPL_WIN32_DX8, 0x4b3108, 0x4b30b0, 0x42d3d0,
            Gui::INGAGME_INPUT_GEN2, 0x4acf3a, 0x4acf38, 0,
            -1);
    }
    HOOKSET_ENDDEF()

    HOOKSET_DEFINE(THInitHook)
    static __declspec(noinline) void THGuiCreate()
    {
        // Init
        GameGuiInit(IMPL_WIN32_DX8, 0x4b3108, 0x4b30b0, 0x42d3d0,
            Gui::INGAGME_INPUT_GEN2, 0x4acf3a, 0x4acf38, 0,
            -1);

        // Gui components creation
        TH09Tools::singleton();

        // Hooks
        THMainHook::singleton().EnableAllHooks();
        th09_disable_map_select.Setup();
    }
    static __declspec(noinline) void THInitHookDisable()
    {
        auto& s = THInitHook::singleton();
        s.th09_gui_init_1.Disable();
        s.th09_gui_init_2.Disable();
    }
    EHOOK_DY(th09_gui_init_1, 0x42a0c4)
    {
        THGuiCreate();
        THInitHookDisable();
    }
    EHOOK_DY(th09_gui_init_2, 0x42e627)
    {
        THGuiCreate();
        THInitHookDisable();
    }
    PATCH_DY(th09_disable_mutex, 0x42d928, "\xEB", 1);
    HOOKSET_ENDDEF()
}

void TH09Init()
{
    TH09::THInitHook::singleton().EnableAllHooks();
}
}
