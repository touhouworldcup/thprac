#include <wininternal.h>

#include <thprac_cfg.h>
#include <thprac_hook.h>
#include <thprac_identify.h>
#include <thprac_log.h>

extern "C" IMAGE_DOS_HEADER __ImageBase;

void thprac_init() {
    MessageBoxW(NULL, L"But there was nothing here", L"NULL", MB_ICONINFORMATION);
    abort();
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
    if (fdwReason == DLL_PROCESS_ATTACH) {
        if (const auto* ver = IdentifyExe((uint8_t*)CurrentPeb()->ImageBaseAddress, 0, nullptr)) {
            InitConfigDir();
            LoadSettings();
            log_init(false, gSettings.console);
            VEHHookInit();
            ver->initFunc();
            return TRUE;
        } else {
            return FALSE;
        }
    }
    return TRUE;
}