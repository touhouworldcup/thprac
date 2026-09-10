#include <wininternal.h>

#include <thprac_cfg.h>
#include <thprac_hook.h>
#include <thprac_identify.h>
#include <thprac_log.h>

extern "C" IMAGE_DOS_HEADER __ImageBase;

TH_NORETURN void thprac_init() {
    if (const auto* ver = IdentifyExe((uint8_t*)CurrentPeb()->ImageBaseAddress, 0, nullptr)) {
        InitConfigDir();
        LoadSettings();
        log_init(false, gSettings.console);
        VEHHookInit();
        ver->initFunc();
        ExitThread(0);
    } else {
        FreeLibraryAndExitThread((HMODULE)&__ImageBase, 0);
    }
}