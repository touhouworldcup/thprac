#define NOMINMAX

#include "thprac_init.h"
#include "thprac_launcher_main.h"
#include "thprac_launcher_cfg.h"
#include "thprac_launcher_games.h"
#include "thprac_main.h"
#include "thprac_gui_locale.h"
#include "thprac_hook.h"
#include "thprac_load_exe.h"
#include "thprac_utils.h"
#include <Windows.h>
#include <psapi.h>
#include <shlwapi.h>
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

void ApplyToProcById(DWORD pid) {
    auto hProc = OpenProcess(
        // PROCESS_SUSPEND_RESUME |
        PROCESS_QUERY_INFORMATION | PROCESS_CREATE_THREAD | PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE,
        FALSE,
        pid);
    if (!hProc) {
        fprintf(stderr, "Error: failed to open process %d\n", pid);
        return;
    }

    uintptr_t base = GetGameModuleBase(hProc);
    if (!base) {
        fprintf(stderr, "Error: failed to determine image base for process %d\n", pid);
    }

    if (!WriteTHPracSig(hProc, base) || !LoadSelf(hProc)) {
        fprintf(stderr, "Error: failed to inject into %d\n", pid);
    }
}

bool DetectGame(const wchar_t* const exe, THGameSig** sigIn) {
    MappedFile f(exe);
    if(!f.fileMapView) {
        return false;
    }

    ExeSig exeSig;
    if (!GetExeInfo(f.fileMapView, f.fileSize, exeSig)) {
        return false;
    }

    for (auto& sig : gGameDefs) {
        if (sig.exeSig.textSize == exeSig.textSize && sig.exeSig.timeStamp == exeSig.timeStamp) {
            *sigIn = &sig;
            return true;
        }
    }

    return false;

}

bool doCmdLineStuff(PWSTR cmdLine) {
    int argc;
    LPWSTR* argv = CommandLineToArgvW(cmdLine, &argc);
    defer(LocalFree(argv));

    enum CURRENT_CMD {
        CMD_NONE,
        CMD_ATTACH,
    } cur_cmd = CMD_NONE;

    bool withVpatch = true;
    const wchar_t* exeFn = nullptr;

    if (argc > 0) {
        if (AttachConsole(ATTACH_PARENT_PROCESS)) {
            freopen("conin$", "r", stdin);
            freopen("conout$", "w", stdout);
            freopen("conout$", "w", stderr);
        }
    } else {
        return false;    
    }
    THGameSig* sig = nullptr;

    for (int i = 0; i < argc;) {
        if (cur_cmd == CMD_ATTACH) {
            DWORD pid = wcstoul(argv[i], nullptr, 10);
            if (!pid) {
                cur_cmd = CMD_NONE;
                fwprintf(stderr, L"Warning: failed to detect %s as process ID\n", argv[i]);
                continue;
            } else {
                ApplyToProcById(pid);
                return true;
            }
        }

        if (wcscmp(argv[i], L"--attach") == 0) {
            cur_cmd = CMD_ATTACH;
            i++;
            continue;
        }

        if (wcscmp(argv[i], L"--without-vpatch") == 0) {
            withVpatch = false;
            i++;
            continue;
        }

        // No actual command line parameter detected, so let's see if it's an exe name
        if (DetectGame(argv[i], &sig)) {
            exeFn = argv[i];
        } else {
            fwprintf(stderr, L"Warning: invalid filename %s\n", argv[i]);
        }
        i++;
    }

    if (exeFn) {
        __assume(sig);
        wchar_t exeDir[MAX_PATH + 1] = {};
        wcsncpy(exeDir, exeFn, MAX_PATH);
        PathRemoveFileSpecW(exeDir);
        SetCurrentDirectoryW(exeDir);
        RunGameWithTHPrac(*sig, exeFn, withVpatch);
        return true;
    } else {
        return false;
    }
}

int WINAPI wWinMain(
    [[maybe_unused]] HINSTANCE hInstance,
    [[maybe_unused]] HINSTANCE hPrevInstance,
    PWSTR pCmdLine,
    [[maybe_unused]] int nCmdShow
) {
    VEHHookInit();
    if (LauncherPreUpdate(pCmdLine)) {
        return 0;
    }

    RemoteInit();
    auto thpracMutex = OpenMutexW(SYNCHRONIZE, FALSE, L"thprac - Touhou Practice Tool##mutex");

    if (doCmdLineStuff(pCmdLine)) {
        return 0;
    }    

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
