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

EXTERN_C IMAGE_DOS_HEADER __ImageBase;

namespace THPrac {

constinit wchar_t old_working_dir[MAX_PATH + 1] = {};
extern int Launcher(HINSTANCE hInstance, int nCmdShow);

void RemoteInit() {
    if ((PVOID)(&__ImageBase) == CurrentPeb()->ImageBaseAddress) {
        return;
    }
    if (const auto* ver = IdentifyExe((uint8_t*)CurrentPeb()->ImageBaseAddress, 0, nullptr)) {
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

enum RUN_GAME_STATUS {
    RUN_GAME_SUCCESS,
    RUN_GAME_FAILURE,
    RUN_GAME_LAUNCHER,
    RUN_GAME_MALICIOUS,
};

RUN_GAME_STATUS TryRunGame(const wchar_t* exeFn, wchar_t* cmdLine, uint32_t flags, bool prompt, bool warn_unknown_exe) {
    size_t exeLen = t_strlen(exeFn);
    if (_wcsnicmp(exeFn + exeLen - 4, L".exe", 4) != 0) {
        return RUN_GAME_FAILURE;
    }

    ExeInfo exeInfo = {};
    THKnownGame knownGame;
    uint16_t oepCode[10];
    if (!IdentifyKnownGame(knownGame, oepCode, exeFn, &exeInfo)) {
        if (warn_unknown_exe && exeInfo) {
            log_printf("Warning: completely unknown executable %s\r\n", utf16_to_utf8(exeFn).c_str());
            log_printf(" -> timestamp = %d, text size = %d", exeInfo.timeStamp, exeInfo.textSize);
        }
        return RUN_GAME_FAILURE;
    } else if (prompt) {
        int choice = log_mboxf(0, MB_YESNOCANCEL, S(THPRAC_EXISTING_GAME_CONFIRMATION_TITLE), S(THPRAC_EXISTING_GAME_CONFIRMATION), gThGameStrs[knownGame.ver->gameId]);
        switch (choice) {
        case IDYES:
            break;
        case IDNO:
            return RUN_GAME_LAUNCHER;
        default:
            return RUN_GAME_FAILURE;
        }
    }

    switch (knownGame.type) {
    case TYPE_MODDED:
        log_printf("Warning: unknown variant of %s\n", gThGameStrs[knownGame.ver->gameId]);
        log_printf("-> MetroHash: " METRO_HASH_FORMAT "\n", METRO_HASH_ARGS(knownGame.metroHash));
        break;
    case TYPE_MALICIOUS:
        log_printf("Warning: potentially malicious variant of %s\n", gThGameStrs[knownGame.ver->gameId]);
        log_printf("-> MetroHash: " METRO_HASH_FORMAT "\n", METRO_HASH_ARGS(knownGame.metroHash));
        log_printf("-> oepCode: " OEP_CODE_FORMAT "\n", OEP_CODE_ARGS(oepCode));
        if (log_mbox(NULL, MB_ICONWARNING | MB_YESNO, "Potentially malicious game", "The game you are attempting to run is detected to be potentially malicious. Are you sure you want to run it?") != IDYES) {
            return RUN_GAME_MALICIOUS;
        }
        break;
    case TYPE_STEAM:
        ShellExecuteW(NULL, L"open", exeFn, cmdLine, old_working_dir, SW_SHOW);
        for (;;) {
            Sleep(100);
            if (FindAndAttach(false, false, knownGame.ver->gameId)) {
                return RUN_GAME_SUCCESS;
            }
        }
        break;
    default:
        break;
    }

    if (RunGame(exeFn, cmdLine, flags)) {
        return RUN_GAME_SUCCESS;
    }
    else {
        return RUN_GAME_FAILURE;
    }
}
}

wchar_t* NextCommandLineParam(wchar_t* cmdline, wchar_t* out) {
    while (*cmdline == L' ' || *cmdline == L'\t') {
        ++cmdline;
    }

    if (*cmdline == L'\0') {
        return nullptr;
    }

    wchar_t* dst = out;
    bool in_quotes = 0;

    while (*cmdline != L'\0') {
        if (*cmdline == L'\\') {
            unsigned num_backslashes = 0;
            while (*cmdline == L'\\') {
                ++num_backslashes;
                ++cmdline;
            }

            if (*cmdline == L'"') {
                for (unsigned i = 0; i < num_backslashes / 2; ++i) {
                    *dst++ = L'\\';
                }
                if (num_backslashes % 2 == 1) {
                    *dst++ = L'"';
                }
                else {
                    in_quotes = !in_quotes;
                }
                ++cmdline;
            }
            else for (unsigned i = 0; i < num_backslashes; ++i) {
                *dst++ = L'\\';
            }
        }
        else if (*cmdline == L'"') {
            in_quotes = !in_quotes;
            ++cmdline;
        }
        else if (!in_quotes && (*cmdline == L' ' || *cmdline == L'\t')) {
            break;
        }
        else {
            *dst++ = *cmdline++;
        }
    }

    *dst = L'\0';

    while (*cmdline == L' ' || *cmdline == L'\t') {
        ++cmdline;
    }

    return cmdline;
}

using namespace THPrac;
int WINAPI wWinMain(HINSTANCE hInstance, [[maybe_unused]] HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
    InitConfigDir();
    LoadSettings();

    srand(Kuser_Shared_Data->SystemTime.LowPart);

    RemoteInit();
    log_init(true, gSettings.console);

    // Exit because we rerun ourselves with LegacyPostUpdate with no parameters
    if (LegacyPostUpdate(pCmdLine, nCmdShow)) {
        return 0;
    }

    if (gSettings.thprac_admin_rights && !PrivilegeCheck()) {
        ShellExecuteW(NULL, L"runas", CurrentPeb()->ProcessParameters->ImagePathName.Buffer, pCmdLine, old_working_dir, nCmdShow);
    }
    
    GetCurrentDirectoryW(MAX_PATH, old_working_dir);
    {
        UNICODE_STRING exeDir = CurrentPeb()->ProcessParameters->ImagePathName;
        for (USHORT i = exeDir.Length / sizeof(wchar_t); i > 0; i--) {
            if (exeDir.Buffer[i] == '\\') {
                exeDir.Length = i * sizeof(wchar_t);
                exeDir.MaximumLength = i * sizeof(wchar_t);
                break;
            }
        }
        RtlSetCurrentDirectory_U(&exeDir);
    }

    UpdaterInit();
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

    enum CurrentCmd {
        CMD_NONE,
        CMD_ATTACH,
    };

    CurrentCmd curCmdId = CMD_NONE;
    uint32_t flags = 0xFFFFFFFF;

    wchar_t curCmdStr[512];
    for (wchar_t* p = NextCommandLineParam(GetCommandLineW(), curCmdStr); p; p = NextCommandLineParam(p, curCmdStr)) {
        if (curCmdId == CMD_ATTACH) {
            curCmdId = CMD_NONE;

            int pid = _wtoi(curCmdStr);
            if (ApplyToProcById(pid)) {
                return 0;
            }
            else {
                log_printf("Warning: invalid PID: %d\n", pid);
                continue;
            }
        }
        if (wcscmp(curCmdStr, L"--attach") == 0) {
            curCmdId = CMD_ATTACH;
            continue;
        }
        if (wcscmp(curCmdStr, L"--without-vpatch") == 0) {
            flags &= ~RUN_FLAG_VPATCH;
            continue;
        }
        if (wcscmp(curCmdStr, L"--without-oilp") == 0) {
            flags &= ~RUN_FLAG_OILP;
            continue;
        }

        if (PathsCompare(curCmdStr, t_strlen(curCmdStr), CurrentPeb()->ProcessParameters->ImagePathName.Buffer, CurrentPeb()->ProcessParameters->ImagePathName.Length / 2)) {
            continue;
        }

        switch (TryRunGame(curCmdStr, p, flags, false, true)) {
        case RUN_GAME_SUCCESS:
            return 0;
        case RUN_GAME_MALICIOUS:
            return 1;
        default:
            continue;
        }
    }

    if (curCmdId == CMD_ATTACH) {
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
            switch (TryRunGame(find.cFileName, nullptr, flags, gSettings.existing_game_launch_action == LAUNCH_ACTION_ALWAYS_ASK, false)) {
            case RUN_GAME_SUCCESS:
                return 0;
            case RUN_GAME_LAUNCHER:
                goto breakout;
            default:
                break;
            }
        } while (FindNextFileW(hFind, &find));
    breakout:
        FindClose(hFind);
    }

    return Launcher(hInstance, nCmdShow);
}
