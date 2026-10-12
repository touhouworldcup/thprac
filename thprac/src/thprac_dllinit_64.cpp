#include <wininternal.h>

#include <thprac_cfg.h>
#include <thprac_hook.h>
#include <thprac_identify.h>
#include <thprac_inject.h>
#include <thprac_log.h>

extern "C" IMAGE_DOS_HEADER __ImageBase;

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, [[maybe_unused]] LPVOID lpvReserved) {
    if (fdwReason == DLL_PROCESS_ATTACH) {
        wchar_t mod_fn[MAX_PATH + 1] = {};
        auto cch = GetModuleFileNameW(hinstDLL, mod_fn, MAX_PATH);
        InitPaths({ mod_fn, cch });
    }
    return TRUE;
}

DWORD RunRemoteThread_Impl(HANDLE hProcess, uintptr_t addr);
void CALLBACK thprac_rundll_inject_helper_internalW(HWND hwnd, HINSTANCE hinst, LPWSTR lpszCmdLine, int nCmdShow) {
    (void)hwnd;
    (void)hinst;
    (void)nCmdShow;

    wchar_t* next = nullptr;
    HANDLE hProcess = (HANDLE)_wcstoui64(lpszCmdLine, &next, 16);
    uint32_t addr = wcstoul(next + 1, nullptr, 16);

    RunRemoteThread_Impl(hProcess, addr);
}

[[noreturn]] void __fastcall thprac_init(RemoteParam64* param) {
    if (const auto* ver = IdentifyExe((uint8_t*)CurrentPeb()->ImageBaseAddress, 0, nullptr)) {
        auto flags = param->flags;
    
        // NOTE: none of thprac's known wrapper patches support any x64 game
        // If one pops up, functionality to load it is to be added here


        if (flags & RUN_FLAG_THPRAC) {
            wchar_t buf[32] = {};
            _snwprintf(buf, 31, L"Global\\thprac pid %d", GetCurrentProcessId());
            CreateEventW(nullptr, TRUE, FALSE, buf);

            LoadSettings();
            log_init(false, gSettings.console);
            VEHHookInit();
            ver->initFunc();
            ExitThread(0);
        }
    }
    FreeLibraryAndExitThread((HMODULE)&__ImageBase, 0x20000000);
}