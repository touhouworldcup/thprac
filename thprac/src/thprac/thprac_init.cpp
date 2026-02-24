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

bool TryLoadOILP() {
    return LoadLibraryW(L"openinputlagpatch.dll");
}

bool TryLoadVpatch() {
    WIN32_FIND_DATAW find = {};
    HANDLE hFind = FindFirstFileW(L"vpatch*.dll", &find);
    if (!hFind) {
        return false;
    }
    do {
        if (LoadLibraryW(find.cFileName)) {
            goto vpatch_loaded;
        }   
    } while (FindNextFileW(hFind, &find));
    FindClose(hFind);
    return false;
vpatch_loaded:
    FindClose(hFind);
    return true;
}

inline void LoadVpatchOrOILP(remote_init_config* conf) {
    if (!conf->newProcess) {
        return;
    }

    if (!conf->forbidOILP && TryLoadOILP()) {
        return;
    }

    if (!conf->forbidVpatch && TryLoadVpatch()) {
        return;
    }
}


void RemoteInit() {
    auto* conf = RemoteGetConfig();
    // No remote shellcode buffer, no remote init config, we are not being injected
    if (!conf) {
        return;
    }
    if (const auto* ver = IdentifyExe((uint8_t*)CurrentPeb()->ImageBaseAddress)) {
        log_init(false, gSettingsGlobal.console);

        LoadVpatchOrOILP(conf);

        VEHHookInit();
        ver->initFunc();
        ExitThread(0);
    } else {
        FreeLibraryAndExitThread((HMODULE)&__ImageBase, 0);
    }    
}
}
