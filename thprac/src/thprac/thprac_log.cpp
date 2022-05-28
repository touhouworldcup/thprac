#include "thprac_log.h"
#include "thprac_launcher_cfg.h"
#include "thprac_launcher_utils.h"
#define NOMINMAX
#include <Windows.h>
#include <cstdio>
#include <ctime>

namespace THPrac {
HANDLE g_logFileHnd = INVALID_HANDLE_VALUE;
unsigned int g_priority = 0;
bool g_dbgLogInited = false;

enum LogOutSelect {
    OUT_FILE = 1 << 0,
    OUT_CONSOLE = 1 << 1
};

void LogWrite(const char* txt) 
{
    //auto newline = "\r\n";
    DWORD bytesWritten;
    WriteFile(g_logFileHnd, txt, strlen(txt), &bytesWritten, NULL);
    //WriteFile(g_logFileHnd, newline, 2, &bytesWritten, NULL);
    FlushFileBuffers(g_logFileHnd);
}
bool LogInit()
{
    if (g_logFileHnd == INVALID_HANDLE_VALUE) {


        auto logFilePath = LauncherGetDataDir();
        logFilePath += L"log";
        CreateDirectoryW(logFilePath.c_str(), NULL);

        SYSTEMTIME time;
        GetSystemTime(&time);

        wchar_t logFileName[MAX_PATH];
        swprintf_s(logFileName, L"thprac-%04d%02d%02d-%02d%02d%02d-%04d.log", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond, time.wMilliseconds);
        logFilePath += L"\\";
        logFilePath += logFileName;

        g_logFileHnd = CreateFileW(logFilePath.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if (g_logFileHnd != INVALID_HANDLE_VALUE) {
            std::function<unsigned int(void)> random = GetRndGenerator(0u, 19u);
            wchar_t moduleFileNameRaw[MAX_PATH];
            GetModuleFileNameW(NULL, moduleFileNameRaw, MAX_PATH);
            auto moduleFileName = GetNameFromFullPath(utf16_to_utf8(moduleFileNameRaw));
            LogA(0, "[INFO] Log initialized in module \"%s\".%s", moduleFileName.c_str(), random() ? "" : " |0M0)");
            return true;
        }
        return false;
    }
    return true;
}
void LogClose()
{
    if (g_logFileHnd != INVALID_HANDLE_VALUE) {
        CloseHandle(g_logFileHnd);
        g_logFileHnd = INVALID_HANDLE_VALUE;
    }
}
void DbgPrtInit()
{
    if (!g_dbgLogInited) {
        FILE* fp;

        AllocConsole();
        freopen_s(&fp, "CONIN$", "r", stdin);
        freopen_s(&fp, "CONOUT$", "w", stdout);
        freopen_s(&fp, "CONOUT$", "w", stderr);

        g_dbgLogInited = true;
    }
}

void OmniLogA(unsigned int priority, int out, const char* fmt, va_list argSrc)
{
    if (priority <= g_priority) {
        char txt[512];
        char txtFinal[512];
        char timeStr[64];
        std::tm tm;

        auto time = std::time(nullptr);
        localtime_s(&tm, &time);
        strftime(timeStr, 64, "%c", &tm);
        va_list args;
        va_copy(args, argSrc);
        vsprintf_s(txt, fmt, args);
        sprintf_s(txtFinal, "[%s] %s\r\n", timeStr, txt);
        va_end(args);

        if (out & OUT_FILE) {
            LogInit();
            LogWrite(txtFinal);
        }
        if (out & OUT_CONSOLE) {
            DbgPrtInit();
            printf_s(txtFinal);
        }
    }
}
void OmniLogW(unsigned int priority, int out, const wchar_t* fmt, va_list argSrc)
{
    if (priority <= g_priority) {
        wchar_t txt[512];
        wchar_t txtFinal[512];
        char timeStr[64];
        std::tm tm;

        auto time = std::time(nullptr);
        localtime_s(&tm, &time);
        strftime(timeStr, 64, "%c", &tm);
        va_list args;
        va_copy(args, argSrc);
        vswprintf_s(txt, fmt, args);
        swprintf_s(txtFinal, L"[%hs] %s\r\n", timeStr, txt);
        va_end(args);

        if (out & OUT_FILE) {
            LogInit();
            LogWrite(utf16_to_utf8(txtFinal).c_str());
        }
        if (out & OUT_CONSOLE) {
            DbgPrtInit();
            wprintf_s(txtFinal);
        }
    }
}

void LogA(unsigned int priority, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    OmniLogA(priority, OUT_FILE, fmt, args);
    va_end(args);
}
void LogW(unsigned int priority, const wchar_t* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    OmniLogW(priority, OUT_FILE, fmt, args);
    va_end(args);
}
void DbgPrtA(unsigned int priority, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    OmniLogA(priority, OUT_CONSOLE, fmt, args);
    va_end(args);
}
void DbgPrtW(unsigned int priority, const wchar_t* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    OmniLogW(priority, OUT_CONSOLE, fmt, args);
    va_end(args);
}
void DbgLogA(unsigned int priority, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    OmniLogA(priority, OUT_FILE | OUT_CONSOLE, fmt, args);
    va_end(args);
}
void DbgLogW(unsigned int priority, const wchar_t* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    OmniLogW(priority, OUT_FILE | OUT_CONSOLE, fmt, args);
    va_end(args);
}

}