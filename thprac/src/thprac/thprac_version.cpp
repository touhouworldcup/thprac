#include "thprac_version.h"

namespace THPrac {

#define THPRAC_VERSION_STR STR(THPRAC_VERSION_0) "." STR(THPRAC_VERSION_1) "." STR(THPRAC_VERSION_2) "." STR(THPRAC_VERSION_3);

const char* g_thpracVersionStr = THPRAC_VERSION_STR;
const wchar_t* g_thpracVersionWcs = L"" THPRAC_VERSION_STR;
const int g_thpracVersionInt[] { THPRAC_VERSION_0, THPRAC_VERSION_1, THPRAC_VERSION_2, THPRAC_VERSION_3 };


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
