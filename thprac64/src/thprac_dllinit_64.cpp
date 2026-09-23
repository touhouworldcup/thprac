#include <wininternal.h>

#include <thprac_cfg.h>
#include <thprac_hook.h>
#include <thprac_identify.h>
#include <thprac_inject.h>
#include <thprac_log.h>

extern "C" IMAGE_DOS_HEADER __ImageBase;

// temp but probably not temp workaround for linking
constinit wchar_t thprac_dll_path[MAX_PATH + 1] = {};

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
    if (fdwReason == DLL_PROCESS_ATTACH) {
        GetModuleFileNameW((HMODULE)&__ImageBase, thprac_dll_path, MAX_PATH);
        if (const auto* ver = IdentifyExe((uint8_t*)CurrentPeb()->ImageBaseAddress, 0, nullptr)) {
            InitConfigDir();
            LoadSettings();
            log_init(false, gSettings.console);
            VEHHookInit();
            ver->initFunc();
        }
    }
    return TRUE;
}

extern "C" {
    __declspec(dllexport) void CALLBACK thprac_rundll_inject_exeW(HWND hwnd, HINSTANCE hinst, LPWSTR lpszCmdLine, int nCmdShow) {
        STARTUPINFOW si = { .cb = sizeof(si) };
        PROCESS_INFORMATION pi = {};

        CreateProcessW(nullptr, lpszCmdLine, nullptr, nullptr, FALSE, CREATE_SUSPENDED, nullptr, nullptr, &si, &pi);
        LoadSelf(pi.hProcess);

        ResumeThread(pi.hThread);
        CloseHandle(pi.hThread);
        CloseHandle(pi.hProcess);
    }

    __declspec(dllexport) void CALLBACK thprac_rundll_inject_pidW(HWND hwnd, HINSTANCE hinst, LPWSTR lpszCmdLine, int nCmdShow) {
        auto pid = _wtoi(lpszCmdLine);
        ApplyToProcById(pid);
    }
}