#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#include <Windows.h>
#include "utils/wininternal.h"

#include "thprac_identify.h"
#include "thprac_load_exe.h"
#include "thprac_hook.h"
#include "thprac_cfg.h"
#include "thprac_log.h"

#pragma intrinsic(strcmp)

namespace THPrac {

EXTERN_C IMAGE_DOS_HEADER __ImageBase;
void RemoteInit() {
    if ((PVOID)(&__ImageBase) == CurrentPeb()->ImageBaseAddress) {
        return;
    }

    if (const auto* ver = IdentifyExe((uint8_t*)CurrentPeb()->ImageBaseAddress, 0)) {
        log_init(false, gSettings.console);
        VEHHookInit();
        ver->initFunc();
        ExitThread(0);
    } else {
        FreeLibraryAndExitThread((HMODULE)&__ImageBase, 0);
    }    
}
}
