#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#include "thprac_init.h"
#include "thprac_games.h"
#include "thprac_launcher_games.h"
#include <Windows.h>
#include <imgui.h>
#include <metrohash128.h>
#include <string>
#include <utility>

#pragma intrinsic(strcmp)

EXTERN_C IMAGE_DOS_HEADER __ImageBase;

namespace THPrac {
void RemoteInit()
{
    if (((int)&__ImageBase) != ((int)GetModuleHandleW(nullptr))) {
        ExeSig exeSig;
        if (GetExeInfoEx((size_t)GetCurrentProcess(), (uintptr_t)GetModuleHandleW(NULL), exeSig)) {
            for (auto& gameDef : gGameDefs) {
                if (gameDef.catagory != CAT_MAIN && gameDef.catagory != CAT_SPINOFF_STG) {
                    continue;
                }
                if (gameDef.exeSig.textSize != exeSig.textSize || gameDef.exeSig.timeStamp != exeSig.timeStamp) {
                    continue;
                }
                if (gameDef.initFunc) {
                    gameDef.initFunc();
                }
                break;
            }
        }
        // This is an InjectResult { .error = Ok, .lastError = 0 }
        // See thprac_load_exe.cpp, thprac_load_exe.h and inject_shellcode.cpp for more info
        ExitThread(0);
    }
}
}
