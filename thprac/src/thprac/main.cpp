#define NOMINMAX

#include "thprac_init.h"
#include "thprac_gui_locale.h"
#include "thprac_hook.h"
#include "thprac_load_exe.h"
#include "thprac_log.h"
#include "thprac_utils.h"

#include "utils/wininternal.h"

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

int WINAPI wWinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ PWSTR pCmdLine,
    _In_ int nCmdShow)
{
    InitConfigDir();
    LoadSettingsGlobal();

    RemoteInit();
    LoadSettingsStartup();

    log_init(false, true);

    if (gSettingsStartup.thprac_admin_rights && !PrivilegeCheck()) {
        ShellExecuteW(NULL, L"runas", CurrentPeb()->ProcessParameters->ImagePathName.Buffer, nullptr, nullptr, nCmdShow);        
    }

    int argc = 0;
    wchar_t** argv = CommandLineToArgvW(pCmdLine, &argc);
    defer(LocalFree(argv));

    enum CurrentCmd {
        CMD_NONE,
        CMD_ATTACH
    };

    CurrentCmd curCmd = CMD_NONE;

    remote_init_config rInitConf = {};

    for (int i = 0; i < argc; i++) {
        if (curCmd == CMD_ATTACH) {
            curCmd = CMD_NONE;

            int pid = _wtoi(argv[i]);
            if (ApplyToProcById(pid)) {
                return 0;
            } else {
                fprintf(stderr, "Warning: invalid PID: %d\n", pid);
                continue;
            }
        }

        if (wcscmp(argv[i], L"--attach") == 0) {
            curCmd = CMD_ATTACH;
            continue;
        }

        if (wcscmp(argv[i], L"--without-vpatch") == 0) {
            rInitConf.forbidVpatch = true;
            continue;
        }

        if (wcscmp(argv[i], L"--without-oilp") == 0) {
            rInitConf.forbidOILP = true;
            continue;
        }

        if (GetFileAttributesW(argv[i]) != INVALID_FILE_ATTRIBUTES) {
            if (!IdentifyExe(argv[i])) {
                continue;
            }
            
            wchar_t* launch_cmdline = wcsstr(pCmdLine, argv[i]) + wcslen(argv[i]);

            RunGameWithTHPrac(argv[i], launch_cmdline, &rInitConf);
            return 0;
        }
    }

    if (curCmd == CMD_ATTACH) {
        FindAndAttach(false, false);
        return 0;
    } else if (!gSettingsStartup.dont_search_ongoing_game && FindAndAttach(false, true)) {
        return 0;        
    }

    // I already need all of this to have it's own scope.
    if (gSettingsStartup.existing_game_launch_action != LAUNCH_ACTION_OPEN_LAUNCHER) {
        WIN32_FIND_DATAW find = {};
        HANDLE hFind = FindFirstFileW(L"*.exe", &find);
        if (!hFind) {
            return false;
        }
        do {
            const auto* ver = IdentifyExe(find.cFileName);
            if (!ver) {
                continue;
            }

            if (gSettingsStartup.existing_game_launch_action == LAUNCH_ACTION_ALWAYS_ASK) {
                int choice = log_mboxf(0, MB_YESNOCANCEL, S(THPRAC_EXISTING_GAME_CONFIRMATION_TITLE), S(THPRAC_EXISTING_GAME_CONFIRMATION), gThGameStrs[ver->gameId]);
                if (choice == IDYES || choice == IDCANCEL) {
                    goto run_this_game;
                } else {
                    continue;
                }
            } else {
            run_this_game:
                FindClose(hFind);
                RunGameWithTHPrac(find.cFileName, nullptr);
                return 0;
            }
        } while (FindNextFileW(hFind, &find));
        FindClose(hFind);
    }

    MessageBoxW(NULL, L"TODO: implement launcher", L"", MB_ICONINFORMATION | MB_OK);
    return 0;
}
