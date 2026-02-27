#include "utils/wininternal.h"

#include "thprac_cfg.h"
#include "thprac_log.h"
#include "thprac_utils.h"

namespace THPrac {

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

    log_print(buf, len);
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

    int ret = MessageBoxW((HWND)hwnd, utf8_to_utf16(text).c_str(), utf8_to_utf16(caption).c_str(), type);

    VLA_FREE(caption_w);
    return ret;
}

int log_vmboxf(void* hwnd, unsigned int type, const char* caption, const char* format, va_list va) {
    va_list va2;
    va_copy(va2, va);
    int len = vsnprintf(nullptr, 0, format, va2);

    VLA(char, buf, len + 1);
    memset(buf, 0, len + 1);
    vsnprintf(buf, len, format, va);

    int ret = log_mbox(hwnd, type, caption, buf);
    va_end(va2);

    return ret;
}

int log_mboxf(void* hwnd, unsigned int type, const char* caption, const char* format, ...)
{
    va_list va;
    va_start(va, format);
    int ret = log_vmboxf(hwnd, type, caption, format, va);
    va_end(va);
    return ret;
}

void log_init(bool launcher, bool console) {
    UNICODE_STRING& cur_dir = CurrentPeb()->ProcessParameters->CurrentDirectory.DosPath;
    wchar_t cur_dir_backup[MAX_PATH + 1] = {};
    memcpy(cur_dir_backup, cur_dir.Buffer, cur_dir.Length);
    SetCurrentDirectoryW(_gConfigDir);
    CreateDirectoryW(L"logs", nullptr);
    SetCurrentDirectoryW(L"logs");

    constexpr unsigned int rot_max = 9;
    constexpr unsigned int scratch_size = 32;
    wchar_t fn_rot_temp_1[scratch_size] = {};
    wchar_t fn_rot_temp_2[scratch_size] = {};

    const wchar_t* const fn_launcher = L"thprac_launcher_log.txt";
    const wchar_t* const fn_ingame = L"thprac_log.txt";

    const wchar_t* const fn_rot_launcher = L"thprac_launcher_log.9.txt";
    const wchar_t* const fn_rot_ingame = L"thprac_log.9.txt";

    if (launcher) {
        memcpy(fn_rot_temp_1, fn_rot_launcher, t_strlen(fn_rot_launcher) * sizeof(wchar_t));
    } else {
        memcpy(fn_rot_temp_1, fn_rot_ingame, t_strlen(fn_rot_ingame) * sizeof(wchar_t)); 
    }

    const unsigned int rot_num_off = launcher ? 20 : 11;
    const wchar_t* const fn = launcher ? fn_launcher : fn_ingame;

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
    
    if (!AttachConsole(GetCurrentProcessId()) && !AttachConsole(ATTACH_PARENT_PROCESS) && console) {
        console = AllocConsole();
    }
    
    if (console) {
        SetConsoleOutputCP(CP_UTF8);
        SetConsoleCP(CP_UTF8);

        freopen("conin$", "r+b", stdin);
        freopen("conout$", "w+b", stdout);
        freopen("conerr$", "w+b", stderr);

        console_open = true;
    }

    log_print("THPrac: Logging initialized\r\n");
}
}