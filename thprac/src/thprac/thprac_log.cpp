#include <Windows.h>
#include <Shlwapi.h>
#include "thprac_log.h"
#include "thprac_launcher_cfg.h"

namespace THPrac {

HANDLE hLog = INVALID_HANDLE_VALUE;
bool console_open = false;

void log_print(const char* msg, size_t len)
{
    DWORD byteRet;
    if (console_open) {
        WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), msg, len, &byteRet, nullptr);
    }
    if (hLog != INVALID_HANDLE_VALUE) {
        WriteFile(hLog, msg, len, &byteRet, nullptr);
    }
}

void log_init(bool launcher, bool console)
{
    // The lengths I'll go to not allocate on the heap

    constexpr unsigned int rot_max = 9;
    constexpr unsigned int scratch_size = 32;
    wchar_t fn_rot_temp_1[scratch_size] = {};
    wchar_t fn_rot_temp_2[scratch_size] = {};

    const wchar_t* const fn_launcher = L"thprac_launcher_log.txt";
    const wchar_t* const fn_ingame = L"thprac_log.txt";

    const wchar_t* const fn_rot_launcher = L"thprac_launcher_log.9.txt";
    const wchar_t* const fn_rot_ingame = L"thprac_log.9.txt";

    if (launcher) {
        memcpy(fn_rot_temp_1, fn_rot_launcher, wcslen(fn_rot_launcher) * sizeof(wchar_t));
    } else {
        memcpy(fn_rot_temp_1, fn_rot_ingame, wcslen(fn_rot_ingame) * sizeof(wchar_t)); 
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
    
    // Attach to existing console if it exists, open new console if no console exists but console is set to true
    // I wonder if this is gonna make command prompt appear for a millisecond for some people
    BOOL console_success = AllocConsole();
    
    if (console_success && !console) {
        FreeConsole();
    } else if (!console_success) {
        AttachConsole(GetCurrentProcessId());
        console_open = true;
    } else {
        console_open = true;
    }

    log_print("THPrac: Logging initialized\r\n");
}
}