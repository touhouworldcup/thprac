#define NOMINMAX
#include "utils/wininternal.h"

#include "thprac_update.h"
#include "thprac_utils.h"
#include "thprac_identify.h"
#include "thprac_load_exe.h"
#include "thprac_hook.h"
#include "thprac_cfg.h"
#include "thprac_log.h"
#include "thprac_gui_locale.h"

namespace THPrac {
constinit wchar_t old_working_dir[MAX_PATH + 1] = {};


extern int Launcher(HINSTANCE hInstance, int nCmdShow);

EXTERN_C IMAGE_DOS_HEADER __ImageBase;
void RemoteInit() {
    if ((PVOID)(&__ImageBase) == CurrentPeb()->ImageBaseAddress) {
        return;
    }

    if (const auto* ver = IdentifyExe((uint8_t*)CurrentPeb()->ImageBaseAddress, 0)) {
        log_init(false, gSettings.console);
        VEHHookInit();
        ver->initFunc();
        ExitThread(0);
    } else {
        FreeLibraryAndExitThread((HMODULE)&__ImageBase, 0);
    }
}

bool PrivilegeCheck() {
    bool ret = false;

    HANDLE hToken;
    if (!NtOpenProcessToken(CurrentProcessHandle, TOKEN_QUERY, &hToken)) {
        TOKEN_ELEVATION Elevation;
        ULONG cbSize = sizeof(Elevation);
        if (!NtQueryInformationToken(hToken, TokenElevation, &Elevation, cbSize, &cbSize)) {
            ret = Elevation.TokenIsElevated;
        }
        CloseHandle(hToken);
    }
    return ret;
}
}

using namespace THPrac;
int WINAPI wWinMain(HINSTANCE hInstance, [[maybe_unused]] HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
    GetCurrentDirectoryW(MAX_PATH, old_working_dir);

    InitConfigDir();
    LoadSettings();

    srand(Kuser_Shared_Data->SystemTime.LowPart);
    RemoteInit();
    UpdaterInit();

    {
        UNICODE_STRING exeDir = CurrentPeb()->ProcessParameters->ImagePathName;
        for (USHORT i = exeDir.Length; i > 0; i--) {
            if (((char*)exeDir.Buffer)[i] == '\\') {
                exeDir.Length = i & 0xFFFE;
                exeDir.MaximumLength = i & 0xFFFE;
                break;
            }
        }
        RtlSetCurrentDirectory_U(&exeDir);
    }

    log_init(true, gSettings.console);

    if (gSettings.thprac_admin_rights && !PrivilegeCheck()) {
        ShellExecuteW(NULL, L"runas", CurrentPeb()->ProcessParameters->ImagePathName.Buffer, pCmdLine, old_working_dir, nCmdShow);
    }

    if (gSettings.check_update == CHECK_UPDATE_ALWAYS) {
        const char* message =
            "Your update settings are configured to always check for updates when running thprac, but the updater failed to initialize."
            "\r\n\r\nDo you want to completely disable updates?";
        if (!UpdaterInitialized()) {
            if (log_mbox(NULL, MB_ICONERROR | MB_YESNO, "Update error", message) == IDYES) {
                gSettings.check_update = CHECK_UPDATE_NEVER;
            }
        } else if (PreLaunchUpdate(hInstance, pCmdLine, nCmdShow, gSettings.update_without_confirmation)) {
            return 0;
        }        
    }

    int argc = 0;
    wchar_t** argv = CommandLineToArgvW(pCmdLine, &argc);
    defer(LocalFree(argv));

    enum CurrentCmd {
        CMD_NONE,
        CMD_ATTACH
    };

    CurrentCmd curCmd = CMD_NONE;
    uint32_t flags = 0xFFFFFFFF;

    for (int i = 0; i < argc; i++) {
        if (curCmd == CMD_ATTACH) {
            curCmd = CMD_NONE;

            int pid = _wtoi(argv[i]);
            if (ApplyToProcById(pid)) {
                return 0;
            }
            else {
                fprintf(stderr, "Warning: invalid PID: %d\n", pid);
                continue;
            }
        }

        if (wcscmp(argv[i], L"--attach") == 0) {
            curCmd = CMD_ATTACH;
            continue;
        }

        if (wcscmp(argv[i], L"--without-vpatch") == 0) {
            flags &= ~RUN_FLAG_VPATCH;
            continue;
        }

        if (wcscmp(argv[i], L"--without-oilp") == 0) {
            flags &= ~RUN_FLAG_OILP;
            continue;
        }

        if (GetFileAttributesW(argv[i]) != INVALID_FILE_ATTRIBUTES) {
            auto& self_exe = CurrentPeb()->ProcessParameters->ImagePathName;
            if (memcmp(argv[i], self_exe.Buffer, self_exe.Length) == 0) {
                continue;
            }
            wchar_t* launch_cmdline = wcsstr(pCmdLine, argv[i]) + t_strlen(argv[i]);

            if (RunGame(argv[i], launch_cmdline, flags)) {
                return 0;
            }
        }
    }

    if (curCmd == CMD_ATTACH) {
        FindAndAttach(false, false);
        return 0;
    }
    else if (!gSettings.dont_search_ongoing_game && FindAndAttach(false, true)) {
        return 0;
    }

    // I already need all of this to have it's own scope.
    if (gSettings.existing_game_launch_action != LAUNCH_ACTION_OPEN_LAUNCHER) {
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

            if (gSettings.existing_game_launch_action == LAUNCH_ACTION_ALWAYS_ASK) {
                int choice = log_mboxf(0, MB_YESNOCANCEL, S(THPRAC_EXISTING_GAME_CONFIRMATION_TITLE), S(THPRAC_EXISTING_GAME_CONFIRMATION), gThGameStrs[ver->gameId]);
                if (choice == IDYES || choice == IDCANCEL) {
                    goto run_this_game;
                }
                else {
                    continue;
                }
            }
            else {
            run_this_game:
                FindClose(hFind);
                RunGame(find.cFileName, nullptr, flags);
                return 0;
            }
        } while (FindNextFileW(hFind, &find));
        FindClose(hFind);
    }
    return Launcher(hInstance, nCmdShow);
}
