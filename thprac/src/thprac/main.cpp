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

HMODULE hWininet;

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

    uintptr_t base;
    HANDLE hProc;
    auto* sig = THPrac::CheckOngoingGameByPID(pid, &base, &hProc);
    if (sig) {
        if (!WriteTHPracSig(hProc, base) || !LoadSelf(hProc)) {
            fprintf(stderr, "Error: failed to inject into PID %d\n", pid);
        }
    } else {
        fprintf(stderr, "Warning: PID %d is invalid. Is it a Touhou game? Is thprac already applied\n", pid);
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

struct CmdlineRet {
    bool proceed;
    bool prompt_yes_game;
};

inline CmdlineRet doCmdLineStuff(PWSTR cmdLine) {
    CmdlineRet ret = {
        .proceed = false,
        .prompt_yes_game = true,
    };

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
        ret.proceed = true;
        return ret;    
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
                return ret;
            }
        }

        if (wcscmp(argv[i], L"--attach") == 0) {
            if (argc == 1) {
                ret.prompt_yes_game = false;
                ret.proceed = true;
                return ret;
            }

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
        ret.proceed = false;
        return ret;
    } else {
        ret.proceed = true;
        return ret;
    }
}

int WINAPI wWinMain(
    [[maybe_unused]] HINSTANCE hInstance,
    [[maybe_unused]] HINSTANCE hPrevInstance,
    PWSTR pCmdLine,
    [[maybe_unused]] int nCmdShow) 
{
    VEHHookInit();
    if (LauncherPreUpdate(pCmdLine)) {
        return 0;
    }

    RemoteInit();
    hWininet = LoadLibraryW(L"wininet.dll");
    auto thpracMutex = OpenMutexW(SYNCHRONIZE, FALSE, L"thprac - Touhou Practice Tool##mutex");

    CmdlineRet cmd = doCmdLineStuff(pCmdLine);
    if (!cmd.proceed) {
        return 0;
    }

    int launchBehavior = 0;
    bool dontFindOngoingGame = false;
    bool adminRights = false;
    int checkUpdateWhen = 0;
    bool autoUpdate = false;
    if (LauncherCfgInit(true)) {
         InitLocaleAndChore();
         if (!hWininet) {
            int oh_my_god_bruh = 2;
            bool oh_my_god_bruh_2 = false;
            LauncherSettingSet("check_update_timing", oh_my_god_bruh);
            LauncherSettingSet("update_without_confirmation", oh_my_god_bruh_2);
        }
        LauncherSettingGet("existing_game_launch_action", launchBehavior);
        LauncherSettingGet("dont_search_ongoing_game", dontFindOngoingGame);
        LauncherSettingGet("thprac_admin_rights", adminRights);
        LauncherSettingGet("check_update_timing", checkUpdateWhen);
        LauncherSettingGet("update_without_confirmation", autoUpdate);
        LauncherCfgClose();
        // Done after loading language as entries rely on it.
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

    if (!dontFindOngoingGame && FindOngoingGame(false, cmd.prompt_yes_game)) {
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
    return 0;
}
