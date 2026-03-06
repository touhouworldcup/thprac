#pragma once

namespace THPrac {

#define THPRAC_VERSION_0 2
#define THPRAC_VERSION_1 2
#define THPRAC_VERSION_2 2
#define THPRAC_VERSION_3 6

#define STRR(X) #X
#define STR(X) STRR(X) 

const char* GetVersionStr();
const wchar_t* GetVersionWcs();
const int* GetVersionInt();
}
