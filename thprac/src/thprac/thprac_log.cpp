#include "utils/wininternal.h"

#include "thprac_cfg.h"
#include "thprac_log.h"
#include "thprac_utils.h"

namespace THPrac {
namespace Gui {
    HWND ImplWin32GetHwnd();
}

HANDLE hLog = INVALID_HANDLE_VALUE;
bool console_open = false;

// TODO: async logging
void log_print(const char* msg, size_t len) {
    DWORD byteRet;
    if (console_open) {
        WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), msg, len, &byteRet, nullptr);
    }
    if (hLog != INVALID_HANDLE_VALUE) {
        WriteFile(hLog, msg, len, &byteRet, nullptr);
    }
}

void log_vprintf(const char* format, va_list va) {
    va_list va2;
    va_copy(va2, va);
    int len = vsnprintf(nullptr, 0, format, va2);

    VLA(char, buf, len + 1);
    vsnprintf(buf, len, format, va);

    log_print(buf, len - 1);
}

void log_printf(const char* format, ...) {
    va_list va;
    va_start(va, format);
    log_vprintf(format, va);
    va_end(va);
}

int log_mbox(void* hwnd, unsigned int type, const char* caption, const char* text) {
    log_printf(
        "---------------------------\n"
        "%s\n"
        "---------------------------\n",
        text);

    if (!hwnd) {
        hwnd = Gui::ImplWin32GetHwnd();
    }

    int ret = MessageBoxW((HWND)hwnd, utf8_to_utf16(text).c_str(), utf8_to_utf16(caption).c_str(), type);

    VLA_FREE(caption_w);
    return ret;
}

int log_vmboxf(void* hwnd, unsigned int type, const char* caption, const char* format, va_list va) {
    va_list va2;
    va_copy(va2, va);
    int len = vsnprintf(nullptr, 0, format, va2) + 1;

    VLA(char, buf, len + 1);
    memset(buf, 0, len + 1);
    vsnprintf(buf, len, format, va);

    int ret = log_mbox(hwnd, type, caption, buf);
    va_end(va2);

    return ret;
}

int log_mboxf(void* hwnd, unsigned int type, const char* caption, const char* format, ...) {
    va_list va;
    va_start(va, format);
    int ret = log_vmboxf(hwnd, type, caption, format, va);
    va_end(va);
    return ret;
}

const wchar_t* ERROR_NO_DATA_DIR_MSG[] = {
    L"thprac was unable to find a suitable data directory.\n"
     "Logs will not be saved to a file.\n"
     "Do you want to open a console to see the logs?",

    L"thprac was unable to find a suitable data directory.\n"
     "Logs will not be saved to a file.\n"
     "Do you want to open a console to see the logs?",
   
    L"thprac was unable to find a suitable data directory.\n"
     "Logs will not be saved to a file.\n"
     "Do you want to open a console to see the logs?",
};

const wchar_t* ERROR_FAILED_TO_OPEN_LOG_FILE_MSG[] = {
    L"Cannot open a log file for writing. No logs will be saved to disk. Do you want to open a console to see logs anyways?",
    L"Cannot open a log file for writing. No logs will be saved to disk. Do you want to open a console to see logs anyways?",
    L"Cannot open a log file for writing. No logs will be saved to disk. Do you want to open a console to see logs anyways?",
};

void log_init(bool launcher, bool console) {
    constexpr unsigned int rot_max = 9;
    constexpr unsigned int scratch_size = 32;
    wchar_t fn_rot_temp_1[scratch_size] = {};
    wchar_t fn_rot_temp_2[scratch_size] = {};

    const wchar_t* const fn_launcher = L"thprac_launcher_log.txt";
    const wchar_t* const fn_ingame = L"thprac_log.txt";

    const wchar_t* const fn_rot_launcher = L"thprac_launcher_log.9.txt";
    const wchar_t* const fn_rot_ingame = L"thprac_log.9.txt";

    unsigned int rot_num_off;
    const wchar_t* fn;

    wchar_t cur_dir_backup[MAX_PATH + 1];
    UNICODE_STRING cur_dir;

    if (!_gConfigDirLen) {
        console = MessageBoxW(NULL, ERROR_NO_DATA_DIR_MSG[Gui::LocaleGet()], nullptr, MB_ICONERROR | MB_YESNO) != IDNO;
        goto past_log_file_open;
    }

    cur_dir = CurrentPeb()->ProcessParameters->CurrentDirectory.DosPath;
    memcpy(cur_dir_backup, cur_dir.Buffer, cur_dir.Length);
    cur_dir_backup[cur_dir.Length / sizeof(wchar_t)] = 0;

    SetCurrentDirectoryW(_gConfigDir);
    CreateDirectoryW(L"logs", nullptr);
    SetCurrentDirectoryW(L"logs");

    if (launcher) {
        memcpy(fn_rot_temp_1, fn_rot_launcher, t_strlen(fn_rot_launcher) * sizeof(wchar_t));
    } else {
        memcpy(fn_rot_temp_1, fn_rot_ingame, t_strlen(fn_rot_ingame) * sizeof(wchar_t)); 
    }

    rot_num_off = launcher ? 20 : 11;
    fn = launcher ? fn_launcher : fn_ingame;

    memcpy(fn_rot_temp_2, fn_rot_temp_1, scratch_size * sizeof(wchar_t));

    DeleteFileW(fn_rot_temp_1);

    for (size_t i = 0; i < rot_max - 1; i++) {
        fn_rot_temp_2[rot_num_off]--;
        MoveFileW(fn_rot_temp_2, fn_rot_temp_1);
        fn_rot_temp_1[rot_num_off]--;
    }

    MoveFileW(fn, fn_rot_temp_1);

    hLog = CreateFileW(fn, GENERIC_WRITE, FILE_SHARE_READ, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    SetCurrentDirectoryW(cur_dir_backup);
    
    if (hLog == INVALID_HANDLE_VALUE) {
        if (MessageBoxW(NULL, ERROR_FAILED_TO_OPEN_LOG_FILE_MSG[Gui::LocaleGet()], nullptr, MB_ICONERROR | MB_YESNO) != IDNO) {
            console = true;
        }
    }

past_log_file_open:
    if (!AttachConsole(GetCurrentProcessId()) && !AttachConsole(ATTACH_PARENT_PROCESS) && console) {
        console = AllocConsole();
    }
    
    if (console) {
        SetConsoleOutputCP(CP_UTF8);
        SetConsoleCP(CP_UTF8);

        (void)freopen("conin$", "r+b", stdin);
        (void)freopen("conout$", "w+b", stdout);
        (void)freopen("conout$", "w+b", stderr);
        setvbuf(stdout, NULL, _IONBF, 0);
        setvbuf(stderr, NULL, _IONBF, 0);

        console_open = true;
    }

    log_print("THPrac: Logging initialized\r\n");

    if (_gConfigDirLen) {
        log_print(SIZED("Data directory is: "));
        // Log data directory, because the data directory needs to be known before logging is initialized
        char dir_u8[MAX_PATH * 2 + 1];
        int wrote = WideCharToMultiByte(CP_UTF8, 0, _gConfigDir, _gConfigDirLen, dir_u8, MAX_PATH * 2, nullptr, nullptr);
        log_print(dir_u8, wrote);
        log_print("\r\n");
    } else {
        log_print(SIZED("Failed to initialize data directory\r\n"));
    }
}
}