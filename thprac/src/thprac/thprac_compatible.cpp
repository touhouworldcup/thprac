#include "thprac_compatible.h"
#include <windows.h>
#include <time.h>

// it's a stupid patch...
// now just change import table


// #if defined(MS_TOOLSET_VERSION)
// #if MS_TOOLSET_VERSION >= 145 // v145
// #define NEED_XTIME_PATCH 1
// #endif
// #endif
// 
// #if NEED_XTIME_PATCH
// extern "C" {
// __int64 __cdecl _Xtime_get_ticks()
// {
//     FILETIME ft;
//     GetSystemTimeAsFileTime(&ft);
//     ULARGE_INTEGER ui;
//     ui.LowPart = ft.dwLowDateTime;
//     ui.HighPart = ft.dwHighDateTime;
//     return static_cast<__int64>(ui.QuadPart - 116444736000000000ULL);
// }
// void __cdecl _Timespec64_get_sys(_timespec64 * xt)
// {
//     FILETIME ft;
//     GetSystemTimeAsFileTime(&ft);
//     unsigned long long t = ((unsigned long long)ft.dwHighDateTime << 32) | ft.dwLowDateTime;
//     t -= 116444736000000000ULL;
//     xt->tv_sec = t / 10000000ULL;
//     xt->tv_nsec = (t % 10000000ULL) * 100;
// }
// 
// }
// #endif