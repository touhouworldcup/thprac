#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#include <Windows.h>
#include "utils/wininternal.h"

#include "thprac_identify.h"
#include "thprac_hook.h"
#include "thprac_log.h"

#pragma intrinsic(strcmp)

EXTERN_C IMAGE_DOS_HEADER __ImageBase;

namespace THPrac {
void RemoteInit() {
    uintptr_t image_base = (uintptr_t)CurrentPeb()->ImageBaseAddress;

    // thprac is running in it's own process, proceed to WinMain
    if ((uintptr_t)&__ImageBase == image_base) {
        return;
    }
    
    if (const auto* ver = IdentifyExe((uint8_t*)image_base)) {
        log_init(false, true);

        VEHHookInit();
        ver->initFunc();
        ExitThread(0);
    } else {
        FreeLibraryAndExitThread((HMODULE)&__ImageBase, 0);
    }    
}
}
