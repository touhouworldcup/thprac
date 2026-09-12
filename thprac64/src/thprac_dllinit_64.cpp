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

extern "C" __declspec(dllexport) void CALLBACK thprac_rundll_inject_helperW(HWND hwnd, HINSTANCE hinst, LPWSTR lpszCmdLine, int nCmdShow) {
    MessageBoxW(hwnd, L"rundll helper", L"It works", MB_OK);

    STARTUPINFOW si = { .cb = sizeof(si) };
    PROCESS_INFORMATION pi = {};

    GetModuleFileNameW((HMODULE)&__ImageBase, thprac_dll_path, MAX_PATH);

    CreateProcessW(nullptr, lpszCmdLine, nullptr, nullptr, FALSE, CREATE_SUSPENDED, nullptr, nullptr, &si, &pi);
    LoadSelf(pi.hProcess);

    ResumeThread(pi.hThread);
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);
}