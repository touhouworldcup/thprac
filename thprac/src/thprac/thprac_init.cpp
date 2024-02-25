#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#include "thprac_init.h"
#include "thprac_games.h"
#include "thprac_launcher_games.h"
#include "thprac_load_exe.h"
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
#define THREAD_RETURN(err, lasterr) ExitThread(err << 16 | lasterr);
    uintptr_t base = (uintptr_t)GetModuleHandleW(NULL);
    if (reinterpret_cast<uintptr_t>(&__ImageBase) == base)
        return;

    ExeSig exeSig;
    if (!GetExeInfoEx((uintptr_t)GetCurrentProcess(), base, exeSig))
        THREAD_RETURN(InjectResult::Ok, 0);

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

    // This has to be ExitThread, because making wWinMain return will destruct any static classes 
    // and uninitialize a lot of C runtime library stuff that we'll need when the game hits our hooks.
    // Using ExitThread here, directly, prevents all that uninitialization code from running.
    // 
    // This also means that every resource allocated by thprac gets leaked when the game exits, but
    // Windows knows how to clean that up on it's own
    // 
    // The value passed to ExitThread is interpreted as an InjectResult { .error = Ok, .lastError = 0 }
    // See thprac_load_exe.cpp, thprac_load_exe.h and inject_shellcode.cpp for more info
    THREAD_RETURN(InjectResult::Ok, 0);
#undef THREAD_RETURN
}
}
