#include "thprac_games.h"
#include "thprac_utils.h"


namespace THPrac {
namespace TH143 {
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
            mMuteki.SetTextOffsetRel(x_offset_1, x_offset_2);
            mInfItems.SetTextOffsetRel(x_offset_1, x_offset_2);
            mTimeLock.SetTextOffsetRel(x_offset_1, x_offset_2);
            mElBgm.SetTextOffsetRel(x_offset_1, x_offset_2);
        }
        virtual void OnContentUpdate() override
        {
            mMuteki();
            mInfItems();
            mTimeLock();
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
        PATCH_HK(0x44F272, "01")
        HOTKEY_ENDDEF();

        HOTKEY_DEFINE(mInfItems, TH_INFITEMS, "F2", VK_F2)
        PATCH_HK(0x457860, "0F1F00"),
        PATCH_HK(0x457F48, "0F1F00"),
        PATCH_HK(0x458FCB, "0F1F00"),
        PATCH_HK(0x45946A, "0F1F00"),
        PATCH_HK(0x459608, "0F1F00"),
        PATCH_HK(0x4597A6, "0F1F00"),
        PATCH_HK(0x459950, "0F1F00"),
        PATCH_HK(0x459EFF, "0F1F00"),
        PATCH_HK(0x459F4A, "0F1F00"),
        PATCH_HK(0x459F94, "0F1F00"),
        PATCH_HK(0x459FF4, "0F1F00"),
        PATCH_HK(0x45A5E3, "0F1F00"),
        PATCH_HK(0x45B4EA, "0F1F00"),
        PATCH_HK(0x45B9BC, "0F1F00"),
        PATCH_HK(0x45BDE8, "0F1F00"),
        PATCH_HK(0x45C1AD, "0F1F00"),
        PATCH_HK(0x45C7FD, "0F1F00"),
        PATCH_HK(0x45CE94, "0F1F00")
        HOTKEY_ENDDEF();

        HOTKEY_DEFINE(mTimeLock, TH_TIMELOCK, "F3", VK_F3)
        PATCH_HK(0x41894d, "eb"),
        PATCH_HK(0x4215C8, "90")
        HOTKEY_ENDDEF();
    public:
        Gui::GuiHotKey mElBgm { TH_EL_BGM, "F7", VK_F7 };
    };

    class THAdvOptWnd : public Gui::GameGuiWnd {
        // Option Related Functions
    private:
        void FpsInit()
        {
            if (*(uint8_t*)0x4e49c9 == 3) {
                mOptCtx.fps_status = 1;

                DWORD oldProtect;
                VirtualProtect((void*)0x46f052, 4, PAGE_EXECUTE_READWRITE, &oldProtect);
                *(double**)0x46f052 = &mOptCtx.fps_dbl;
                VirtualProtect((void*)0x46f052, 4, oldProtect, &oldProtect);
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
        if (Gui::GetChordPressed(Gui::GetAdvancedMenuChord())) {
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
    EHOOK_DY(th143_render_1, 0x46ed3f, 5, {
        THGuiUpdate();
    })
    EHOOK_DY(th143_render_2, 0x46f14f, 5, {
        THGuiUpdate();
    })
    EHOOK_DY(th143_render_3, 0x46f2ee, 5, {
        THGuiUpdate();
    })
    HOOKSET_ENDDEF()

    static __declspec(noinline) void THGuiCreate()
    {
        if (ImGui::GetCurrentContext()) {
            return;
        }
        // Init
        GameGuiInit(IMPL_WIN32_DX9, 0x4e47d8, 0x5390a0,
            Gui::INGAGME_INPUT_GEN2, 0x4e19dc, 0x4e19d8, 0,
            (*((int32_t*)0x53b0dc) >> 2) & 0xf);

        SetDpadHook(0x401D72, 3);

        // Gui components creation
        THOverlay::singleton();

        // Hooks
        EnableAllHooks(THMainHook);
    }
    HOOKSET_DEFINE(THInitHook)
    PATCH_DY(th143_startup_1, 0x45ef95, "eb")
    PATCH_DY(th143_startup_2, 0x45fbaf, "eb")
    EHOOK_DY(th143_gui_init_1, 0x45fd77, 3, {
        self->Disable();
        THGuiCreate();
    })
    EHOOK_DY(th143_gui_init_2, 0x46fc6b, 1, {
        self->Disable();
        THGuiCreate();
    })
    HOOKSET_ENDDEF()
}

void TH143Init()
{
    EnableAllHooks(TH143::THInitHook);
}
}
