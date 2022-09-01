#include "thprac_utils.h"

namespace THPrac {
namespace TH165 {
    class THOverlay : public Gui::GameGuiWnd {
        THOverlay() noexcept
        {
            SetTitle("Mod Menu");
            SetFade(0.5f, 0.5f);
            SetCursor(nullptr);
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
            mMuteki.SetTextOffsetRel(x_offset_1, x_offset_2);
            mInfCharge.SetTextOffsetRel(x_offset_1, x_offset_2);
            mTimeLock.SetTextOffsetRel(x_offset_1, x_offset_2);
            mElBgm.SetTextOffsetRel(x_offset_1, x_offset_2);
        }
        virtual void OnContentUpdate() override
        {
            mMuteki();
            mInfCharge();
            mTimeLock();
        }
        virtual void OnPreUpdate() override
        {
            if (mMenu(false) && !ImGui::IsAnyItemActive()) {
                if (*mMenu) {
                    Open();
                } else {
                    Close();
                    //*((int32_t*)0x6c6eb0) = 2;
                }
            }
            //if (*((int32_t*)0x6c6ea4) == 2)
            //	SetPos(500.0f, 300.0f);
            //else
            //	SetPos(10.0f, 10.0f);
        }

        Gui::GuiHotKey mMenu { "ModMenuToggle", "BACKSPACE", VK_BACK };
        Gui::GuiHotKey mMuteki { TH_MUTEKI, "F1", VK_F1, {
            new HookCtx(0x446a80, "\x01", 1) } };
        Gui::GuiHotKey mInfCharge { TH_INFCHARGE, "F2", VK_F2, {
            new HookCtx(0x44c5f8, "\x90\x90", 2) } };
    private:
        Gui::GuiHotKey mTimeLock { TH_TIMELOCK, "F3", VK_F3, {
            new HookCtx(0x419a78, "\xeb\x63", 2) } };

    public:
        Gui::GuiHotKey mElBgm { TH_EL_BGM, "F7", VK_F7 };
    };

    class THAdvOptWnd : public Gui::GameGuiWnd {
        // Option Related Functions
    private:
        void FpsInit()
        {
            #if 0
            mOptCtx.vpatch_base = (int32_t)GetModuleHandleW(L"");
            if (false) {
                if (*(int32_t*)(mOptCtx.vpatch_base + 0x1a024) == 0)
                    mOptCtx.fps_status = 2;
            } else 
            #endif    
            if (*(uint8_t*)0x4b3d09 == 3) {
                mOptCtx.fps_status = 1;

                DWORD oldProtect;
                VirtualProtect((void*)0x460fd7, 4, PAGE_EXECUTE_READWRITE, &oldProtect);
                *(double**)0x460fd7 = &mOptCtx.fps_dbl;
                VirtualProtect((void*)0x460fd7, 4, oldProtect, &oldProtect);
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
            SetTitle(XSTR(TH_SPELL_PRAC));
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
            ImGui::TextUnformatted(XSTR(TH_ADV_OPT));
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

    __declspec(noinline) void THGuiUpdate()
    {
        GameGuiBegin(IMPL_WIN32_DX9);

        // Gui components update
        THOverlay::singleton().Update();

        GameGuiEnd(UpdateAdvOptWindow());
        GameGuiRender(IMPL_WIN32_DX9);
    }

    HOOKSET_DEFINE(THMainHook)
    EHOOK_DY(th165_restart, 0x43cd00)
    {
        auto s1 = pCtx->Esp + 0x14;
        auto s2 = pCtx->Edi + 0x2bc;
        auto s3 = *(DWORD*)(pCtx->Edi + 0x2c0);
        auto lEcx = pCtx->Ecx;
        auto lEdi = pCtx->Edi;
        auto lEsi = pCtx->Esi;

        ASM push lEcx;
        ASM push 0x7;
        ASM mov edx, 0x463e00;
        ASM call edx;

        ASM push lEcx;
        ASM push 0x72;
        ASM push s1;
        ASM mov ecx, s2;
        ASM mov edx, 0x475880;
        ASM call edx;

        ASM push 0x6;
        ASM push[eax];
        ASM mov edx, 0x475400;
        ASM call edx;

        // Restart New 1
        ASM push 0x1;
        ASM push s3;
        ASM mov edx, 0x475400;
        ASM call edx;

        // Restart Mod 1
        ASM push 0x4;
        ASM mov ecx, lEsi;
        ASM mov edx, 0x415d40;
        ASM call edx;

        pCtx->Eip = 0x43cd31;
    }
    EHOOK_DY(th165_render_1, 0x46038d)
    {
        THGuiUpdate();
    }
    EHOOK_DY(th165_render_2, 0x460461)
    {
        THGuiUpdate();
    }
    EHOOK_DY(th165_render_3, 0x4610aa)
    {
        THGuiUpdate();
    }
    HOOKSET_ENDDEF()

    HOOKSET_DEFINE(THInitHook)
    static __declspec(noinline) void THGuiCreate()
    {
        // Init
        GameGuiInit(IMPL_WIN32_DX9, 0x4b3b18, 0x507b70, 0x460950,
            Gui::INGAGME_INPUT_GEN2, 0x4b0ffc, 0x4b0ff8, 0,
            (*((int32_t*)0x509bac) >> 2) & 0xf);

        // Gui components creation
        THOverlay::singleton();

        // Hooks
        THMainHook::singleton().EnableAllHooks();

        // Reset thPracParam
        //thPracParam.Reset();
    }
    static __declspec(noinline) void THInitHookDisable()
    {
        auto& s = THInitHook::singleton();
        s.th165_gui_init_1.Disable();
        s.th165_gui_init_2.Disable();
    }
    PATCH_DY(th165_startup_1, 0x4520cf, "\x90\x90", 2);
    PATCH_DY(th165_startup_2, 0x452fd1, "\xeb", 1);
    EHOOK_DY(th165_gui_init_1, 0x453218)
    {
        THGuiCreate();
        THInitHookDisable();
    }
    EHOOK_DY(th165_gui_init_2, 0x461b27)
    {
        THGuiCreate();
        THInitHookDisable();
    }
    HOOKSET_ENDDEF()
}

void TH165Init()
{
    TH165::THInitHook::singleton().EnableAllHooks();
    TryKeepUpRefreshRate((void*)0x461c7a, (void*)0x461a4d);
}
}
