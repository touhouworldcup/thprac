#include <wininternal.h>

#include <thprac_cfg.h>
#include <thprac_hook.h>
#include <thprac_identify.h>
#include <thprac_inject.h>
#include <thprac_log.h>

extern "C" IMAGE_DOS_HEADER __ImageBase;

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
    if (fdwReason == DLL_PROCESS_ATTACH) {
        wchar_t mod_fn[MAX_PATH + 1] = {};
        auto cch = GetModuleFileNameW(hinstDLL, mod_fn, MAX_PATH);
        InitPaths({ mod_fn, cch });
    }
    return TRUE;
}

//DWORD RunRemoteThread_Impl(HANDLE hProcess, uintptr_t addr);
void CALLBACK thprac_rundll_inject_helper_internalW(HWND hwnd, HINSTANCE hinst, LPWSTR lpszCmdLine, int nCmdShow) {
    MessageBoxW(hwnd, L"Not implemented (thprac_rundll_inject_helper_internalW)", g_Dll32Path.data(), MB_ICONERROR);
    abort();
}

bool TryLoadVpatch();
[[noreturn]] void __fastcall thprac_init(RemoteParam32* param) {
    if (const auto* ver = IdentifyExe((uint8_t*)CurrentPeb()->ImageBaseAddress, 0, nullptr)) {
        auto flags = param->flags;
        
        // TODO: addresses for OILP and vpatch functions will be obtained in here
        if (flags & RUN_FLAG_OILP && ver->has_oilp) {
            LoadLibraryW(L"openinputlagpatch.dll");
        }
        else if (flags & RUN_FLAG_VPATCH) {
            TryLoadVpatch();
        }

        if (!(flags & RUN_FLAG_THPRAC)) {
            FreeLibraryAndExitThread((HMODULE)&__ImageBase, 0x20000003);
        }
        
        wchar_t buf[32] = {};
        _snwprintf(buf, 31, L"Global\\thprac pid %d", GetCurrentProcessId());
        CreateEventW(nullptr, TRUE, FALSE, buf);

        LoadSettings();
        log_init(false, gSettings.console);
        VEHHookInit();
        ver->initFunc();
        ExitThread(0);
    }
    else {
        FreeLibraryAndExitThread((HMODULE)&__ImageBase, 0x20000000);
    }
}