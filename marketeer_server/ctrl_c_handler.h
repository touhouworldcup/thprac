#pragma once

static volatile bool g_CtrlCInt = false;

#if _WIN32 || _WIN64
#define NOMINMAX
#include <Windows.h>

static BOOL WINAPI CtrlCHandler(_In_ DWORD dwCtrlType)
{
    if (dwCtrlType == CTRL_C_EVENT) {
        g_CtrlCInt = true;
        return true;
    }
    return false;
}

static void CtrlCHandling()
{
    SetConsoleCtrlHandler(CtrlCHandler, true);
}
#elif __GNUC__
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

static void CtrlCHandler(int s)
{
    g_CtrlCInt = true;
}

static void CtrlCHandling()
{
    struct sigaction sigIntHandler;
    sigIntHandler.sa_handler = CtrlCHandler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    sigaction(SIGINT, &sigIntHandler, NULL);
}
#else
#error Unsupported compiler
#endif