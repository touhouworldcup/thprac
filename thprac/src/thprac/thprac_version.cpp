#include "thprac_version.h"


namespace THPrac {

#define THPRAC_VERSION_STR STR(THPRAC_VERSION_0) "." STR(THPRAC_VERSION_1) "." STR(THPRAC_VERSION_2) "." STR(THPRAC_VERSION_3)
const char* g_thpracVersionStr = THPRAC_VERSION_STR;
const wchar_t* g_thpracVersionWcs = L"" THPRAC_VERSION_STR;

const char* GetVersionStr()
{
    return g_thpracVersionStr;
}

const wchar_t* GetVersionWcs()
{
    return g_thpracVersionWcs;
}

ThpracVersion ParseVersion(const char* str)
{
    ThpracVersion ver = { 0, 0, 0, 0 };

    unsigned char* fields[4] = { &ver.meta, &ver.major, &ver.minor, &ver.patch };
    unsigned char curVal = 0;
    size_t fieldIndex = 0;

    for (const char* p = str;; ++p) {
        if (*p >= '0' && *p <= '9') {
            curVal = curVal * 10 + (*p - '0');

        } else if (*p == '.' || *p == '\0') {
            if (fieldIndex < 4)
                *fields[fieldIndex++] = curVal;
            curVal = 0;

            if (*p == '\0')
                break;
        }
    }

    return ver;
}

}