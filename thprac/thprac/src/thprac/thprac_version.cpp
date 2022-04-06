#include "thprac_version.h"

namespace THPrac {
const char* g_thpracVersionStr { "2.1" };
const int g_thpracVersionInt[] { 2, 1, 0, 0 };


const char* GetVersionStr()
{
    return g_thpracVersionStr;
}
const int* GetVersionInt()
{
    return g_thpracVersionInt;
}


}