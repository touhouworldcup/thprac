#include "thprac_version.h"

namespace THPrac {
ThpracVersion ParseVersion(const char* str) {
    ThpracVersion ver = { 0, 0, 0, 0 };
    if(!str) {
        return ver;
    }

    unsigned char curVal = 0;
    size_t fieldIndex = 0;

    for (const char* p = str;; ++p) {
        if (*p >= '0' && *p <= '9') {
            curVal = curVal * 10 + (*p - '0');

        } else if (*p == '.' || *p == '\0') {
            if (fieldIndex < 4)
                ver[fieldIndex++] = curVal;
            curVal = 0;

            if (*p == '\0')
                break;
        }
    }

    return ver;
}

const ThpracVersion gVersion = { THPRAC_VERSION_0, THPRAC_VERSION_1, THPRAC_VERSION_2, THPRAC_VERSION_3 };
}
