#include "thprac_version.h"

namespace THPrac {
const char* g_thpracVersionStr { "2.1.3.0" };
const wchar_t* g_thpracVersionWcs { L"2.1.3.0" };
const int g_thpracVersionInt[] { 2, 1, 3, 0 };


const char* GetVersionStr()
{
    return g_thpracVersionStr;
}
const wchar_t* GetVersionWcs()
{
    return g_thpracVersionWcs;
}
const int* GetVersionInt()
{
    return g_thpracVersionInt;
}


}
