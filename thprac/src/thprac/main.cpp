#define NOMINMAX

#include "thprac_init.h"
#include "thprac_gui_locale.h"
#include "thprac_hook.h"
#include "thprac_load_exe.h"
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


enum ExistingGameLaunchAction {
    LAUNCH_ACTION_LAUNCH_GAME = 0,
    LAUNCH_ACTION_OPEN_LAUNCHER = 1,
    LAUNCH_ACTION_ALWAYS_ASK = 2,
};

int WINAPI wWinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ PWSTR pCmdLine,
    _In_ int nCmdShow)
{
    // TODO: get rid of these and reimplement the settings system

    ExistingGameLaunchAction existing_game_launch_action = LAUNCH_ACTION_LAUNCH_GAME;
    bool dont_search_ongoing_game = false;
    bool thprac_admin_rights = false;


    RemoteInit();

    // TODO: read config file
#if 0
    if (thprac_admin_rights && !PrivilegeCheck()) {
        ShellExecuteW(NULL, L"runas", CurrentPeb()->ProcessParameters->ImagePathName.Buffer, nullptr, nullptr, nCmdShow);        
    }
#endif

    int argc = 0;
    wchar_t** argv = CommandLineToArgvW(pCmdLine, &argc);
    defer(LocalFree(argv));

    enum CurrentCmd {
        CMD_NONE,
        CMD_ATTACH
    };

    CurrentCmd curCmd = CMD_NONE;

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

        if (GetFileAttributesW(argv[i]) != INVALID_FILE_ATTRIBUTES) {
            if (!IdentifyExe(argv[i])) {
                continue;
            }
            
            wchar_t* launch_cmdline = wcsstr(pCmdLine, argv[i]) + wcslen(argv[i]);

            RunGameWithTHPrac(argv[i], launch_cmdline);
            return 0;
        }
    }

    if (curCmd == CMD_ATTACH) {
        FindAndAttach(false, false);
        return 0;
    } else if (!dont_search_ongoing_game && FindAndAttach(false, true)) {
        return 0;        
    }

    // I already need all of this to have it's own scope.
    if (existing_game_launch_action != LAUNCH_ACTION_OPEN_LAUNCHER) {
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

            if (existing_game_launch_action == LAUNCH_ACTION_ALWAYS_ASK) {
                const char* gameStr = gThGameStrs[ver->gameId];
                wchar_t gameWcs[8] = {};
                for (size_t i = 0; gameStr[i]; i++) {
                    gameWcs[i] = gameStr[i];
                }

                wchar_t buf[64] = {};
                swprintf(buf, 63, L"An existing game was found: %s.\nClick \"Yes\" to launch the game.\nClick \"No\" to open the launcher.", gameWcs);
                int choice = MessageBoxW(NULL, buf, L"TODO: add translation support", MB_YESNOCANCEL);

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
