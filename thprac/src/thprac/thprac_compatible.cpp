#include "thprac_compatible.h"
#include <windows.h>

#if defined(MS_TOOLSET_VERSION)
#if MS_TOOLSET_VERSION >= 145 // v145
#define NEED_XTIME_PATCH 1
#endif
#endif

#if NEED_XTIME_PATCH
extern "C" {
__int64 __cdecl _Xtime_get_ticks()
{
    FILETIME ft;
    GetSystemTimeAsFileTime(&ft);
    ULARGE_INTEGER ui;
    ui.LowPart = ft.dwLowDateTime;
    ui.HighPart = ft.dwHighDateTime;
    return static_cast<__int64>(ui.QuadPart - 116444736000000000ULL);
}
}
#endif