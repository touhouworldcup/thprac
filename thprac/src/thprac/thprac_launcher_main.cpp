#include "thprac_launcher_main.h"
#include "thprac_gui_locale.h"
#include "thprac_launcher_cfg.h"
#include "thprac_launcher_games.h"
#include "thprac_launcher_links.h"
#include "thprac_launcher_tools.h"
#include "thprac_launcher_others.h"
#include "thprac_launcher_keng.h"
#include "thprac_launcher_utils.h"
#include "thprac_launcher_wnd.h"
#include "thprac_utils.h"
#include <ShlObj.h>
#include <Windows.h>
#include <Shlwapi.h>
#include <cstdio>
#include <sstream>

#include <thread>
#include <numbers>


namespace THPrac {
extern bool g_isLauncher;
const char* gTabToOpen = nullptr;
LauncherTrigger gLauncherTrigger = LAUNCHER_NOTHING;

void GuiLauncherMainSwitchTab(const char* tab)
{
    gTabToOpen = tab;
}
bool GuiTabItem(const char* tabText)
{
    auto flag = 0;
    if (gTabToOpen && !strcmp(gTabToOpen, tabText)) {
        flag = ImGuiTabItemFlags_SetSelected;
        gTabToOpen = nullptr;
    }
    return ImGui::BeginTabItem(tabText, 0, flag);
}
void ErrorMsgBox(th_glossary_t textRef)
{
    auto title = utf8_to_utf16(S(THPRAC_PR_ERROR));
    auto text = utf8_to_utf16(S(textRef));
    MessageBoxW(nullptr, text.c_str(), title.c_str(), MB_OK | MB_ICONERROR | MB_SYSTEMMODAL);
}

void GuiLauncherLocaleInit()
{
    if (LauncherCfgInit(true)) {
        if (!Gui::LocaleInitFromCfg()) {
            Gui::LocaleAutoSet();
        }
        LauncherCfgClose();
    }
}
void GuiLauncherMainTrigger(LauncherTrigger trigger)
{
    gLauncherTrigger = trigger;
}

HHOOK g_hKeyboardHook = nullptr;
LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode < 0 || nCode != HC_ACTION) // do not process message
        return CallNextHookEx(g_hKeyboardHook, nCode, wParam, lParam);

    bool bEatKeystroke = false;
    auto p = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);
    switch (wParam) {
    case WM_KEYDOWN:
    case WM_KEYUP: {
        bEatKeystroke =(p->vkCode == VK_LWIN) || (p->vkCode == VK_RWIN);
        // Note that this will not block the Xbox Game Bar hotkeys (Win+G, Win+Alt+R, etc.)
        break;
    }
    }

    if (bEatKeystroke)
        return 1;
    else
        return CallNextHookEx(g_hKeyboardHook, nCode, wParam, lParam);
}
void DisableWinKey(){
    if (!g_hKeyboardHook)
        g_hKeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, GetModuleHandle(nullptr), 0);
}

void DisableDisableWinKey()
{
    if (g_hKeyboardHook){
        UnhookWindowsHookEx(g_hKeyboardHook);
        g_hKeyboardHook = nullptr;
    }
}

int GuiLauncherMain()
{
    int localeSwitch = -1;

    auto initResult = LauncherWndInit(640, 480, 1280, 960, 960, 720);
    if (initResult <= 0) {
        if (initResult) {
            ErrorMsgBox(THPRAC_PR_ERR_LAUNCHER_INIT);
        }
        return -1;
    }
    if (!LauncherCfgInit()) {
        ErrorMsgBox(THPRAC_PR_ERR_LAUNCHER_CFG);
        return -1;
    }

    int theme;
    if (LauncherSettingGet("theme", theme)) {
        // LauncherSettingSet doesn't take int, only int&.
        // Passing 0 will call the overload with const char*
        int Sus = 0;
        const char* theme_user = nullptr;
        // LauncherSettingGet only accepts signed ints but I want to do an unsigned comparison
        if ((unsigned int)theme > 2) {
            if (LauncherSettingGet("theme_user", theme_user) && theme_user) {
                std::wstring theme_path = LauncherGetDataDir() + L"themes\\" + utf8_to_utf16(theme_user);
                if (!PathFileExistsW(theme_path.c_str())) {
                    LauncherSettingSet("theme", Sus);
                    theme = Sus;
                }
            } else {
                LauncherSettingSet("theme", Sus);
                theme = Sus;
            }
        }

        if (theme_user) {
            SetTheme(theme, utf8_to_utf16(theme_user).c_str());
        } else {
            SetTheme(theme);
        }
    }
    LauncherPeekUpd();
    auto scale = LauncherWndGetScale();
    LauncherOthersInit();
    LauncherKengInit();

    g_hKeyboardHook = nullptr;
    bool disable_win;
    if (LauncherSettingGet("disable_win_key", disable_win) && disable_win) {
        DisableWinKey();
    }
    while (LauncherWndNewFrame()) {
        static bool isOpen = true;
        static bool isMinimize = false;
        bool canMove = false;
        LauncherOthersUpdate();

        ImGui::SetNextWindowSizeConstraints(ImVec2(640.0f * scale, 480.0f * scale), ImVec2(1280.0f * scale, 960.0f * scale));
        ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(960.0f * scale, 720.0f * scale), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowBgAlpha(0.9f);

        std::string wndTitleText = S(THPRAC_LAUNCHER);
        if (LauncherIsChkingUpd()) {
            wndTitleText += " // Checking for updates...";
        }
        wndTitleText = LauncherIsChkingUpd() ? S(THPRAC_LAUNCHER_CHECKING_UPDATE) : S(THPRAC_LAUNCHER);
        wndTitleText += "###thprac_wnd";
        ImGui::Begin(wndTitleText.c_str(), &isOpen, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove, &isMinimize);
        if (!isOpen)
            break;
        if (isMinimize) {
            isMinimize = false;
            LauncherWndMinimize();
        }
        canMove = ImGui::IsItemHovered();

        if (ImGui::BeginTabBar("MenuTabBar")) {
            if (GuiTabItem(S(THPRAC_GAMES))) {
                ImGui::BeginChild("##games");
                LauncherGamesGuiUpd();
                ImGui::EndChild();
                ImGui::EndTabItem();
            }
            if (GuiTabItem(S(THPRAC_OTHERS))) {
                ImGui::BeginChild("##others");
                LauncherOthersGuiUpd();
                ImGui::EndChild();
                ImGui::EndTabItem();
            }
            if (GuiTabItem(S(THPRAC_LINKS))) {
                ImGui::BeginChild("##links");
                LauncherLinksGuiUpd();
                ImGui::EndChild();
                ImGui::EndTabItem();
            }
            if (GuiTabItem(S(THPRAC_TOOLS))) {
                ImGui::BeginChild("##tools");
                LauncherToolsGuiUpd();
                ImGui::EndChild();
                ImGui::EndTabItem();
            }
            if (GuiTabItem(S(THPRAC_KENG))) {
                ImGui::BeginChild("##keng");
                LauncherKengGuiUpd();
                ImGui::EndChild();
                ImGui::EndTabItem();
            }
            if (GuiTabItem(S(THPRAC_SETTINGS))) {
                ImGui::BeginChild("##settings");
                LauncherCfgGuiUpd();
                ImGui::EndChild();
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }
        
        LauncherChkUpdPopup();

        ImVec2 wndPos = ImGui::GetWindowPos();
        ImVec2 wndSize = ImGui::GetWindowSize();
        canMove &= !ImGui::IsAnyItemHovered() && !ImGui::IsAnyItemActive();
        ImGui::End();
        LauncherWndEndFrame(wndPos, wndSize, canMove);

        switch (gLauncherTrigger) {
        case THPrac::LAUNCHER_RESTART:
            localeSwitch = Gui::LocaleGet();
            break;
        case THPrac::LAUNCHER_CLOSE:
        case THPrac::LAUNCHER_RESET:
            isOpen = false;
            break;
        case THPrac::LAUNCHER_MINIMIZE:
            LauncherWndMinimize();
            break;
        default:
            break;
        }
        if (gLauncherTrigger != LAUNCHER_RESET) {
            gLauncherTrigger = LAUNCHER_NOTHING;
        }

        if (localeSwitch != -1) {
            MSG msg;
            Gui::LocaleSet((Gui::locale_t)localeSwitch);
            LauncherWndShutdown();
            while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE)) {
            }
            LauncherWndInit(640, 480, 1280, 960, 960, 720);
            localeSwitch = -1;
        }
    }
    LauncherOthersDestroy();
    LauncherKengDestroy();
    DisableDisableWinKey();

    LauncherCfgClose();
    LauncherWndShutdown();
    if (gLauncherTrigger == LAUNCHER_RESET) {
        LauncherCfgReset();
    }
    
    return 0;
}

}
