#define NOMINMAX

#include "thprac_init.h"
#include "thprac_launcher_main.h"
#include "thprac_launcher_cfg.h"
#include "attach.h"
#include "gui_locale.h"
#include "thprac_hook.h"
#include <Windows.h>
#include <psapi.h>
#include <tlhelp32.h>

using namespace THPrac;

bool PrivilegeCheck()
{
    BOOL fRet = FALSE;
    HANDLE hToken = nullptr;
    if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken)) {
        TOKEN_ELEVATION Elevation = {};
        DWORD cbSize = sizeof(TOKEN_ELEVATION);
        if (GetTokenInformation(hToken, TokenElevation, &Elevation, sizeof(Elevation), &cbSize)) {
            fRet = Elevation.TokenIsElevated;
        }
    }
    if (hToken) {
        CloseHandle(hToken);
    }
    return fRet;
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    HookCtx::VEHInit();
    if (LauncherPreUpdate(pCmdLine)) {
        return 0;
    }

    auto thpracMutex = OpenMutexW(SYNCHRONIZE, FALSE, L"thprac - Touhou Practice Tool##mutex");
    RemoteInit();

    int launchBehavior = 0;
    bool dontFindOngoingGame = false;
    bool adminRights = false;
    int checkUpdateWhen = 0;
    bool autoUpdate = false;
    if (LauncherCfgInit(true)) {
        if (!Gui::LocaleInitFromCfg()) {
            Gui::LocaleAutoSet();
        }
        LauncherSettingGet("existing_game_launch_action", launchBehavior);
        LauncherSettingGet("dont_search_ongoing_game", dontFindOngoingGame);
        LauncherSettingGet("thprac_admin_rights", adminRights);
        LauncherSettingGet("check_update_timing", checkUpdateWhen);
        LauncherSettingGet("update_without_confirmation", autoUpdate);
        LauncherCfgClose();
    }

    if (adminRights && !PrivilegeCheck()) {
        wchar_t exePath[MAX_PATH];
        GetModuleFileNameW(nullptr, exePath, MAX_PATH);
        CloseHandle(thpracMutex);
        ShellExecuteW(nullptr, L"runas", exePath, nullptr, nullptr, SW_SHOW);
        return 0;
    }

    if (checkUpdateWhen == 1) {
        if (LauncherUpdDialog(autoUpdate)) {
            return 0;
        }
    }

    if (!dontFindOngoingGame && FindOngoingGame()) {
        return 0;
    }

    if (launchBehavior != 1 && FindAndRunGame(launchBehavior == 2)) {
        return 0;
    }

    if (checkUpdateWhen == 0 && autoUpdate) {
        if (LauncherUpdDialog(autoUpdate)) {
            return 0;
        }
    }

    return GuiLauncherMain();
}
