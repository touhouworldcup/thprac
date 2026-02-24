#include "thprac_utils.h"
#include "thprac_licence.h"
#include <metrohash128.h>
#include "../3rdParties/d3d8/include/d3d8.h"


namespace THPrac {

#pragma region Locale
typedef int WINAPI MultiByteToWideChar_t(UINT CodePage, DWORD dwFlags, LPCCH lpMultiByteStr, int cbMultiByte, LPWSTR lpWideCharStr, int cchWideChar);
typedef int WINAPI WideCharToMultiByte_t(UINT CodePage, DWORD dwFlags, LPCWCH lpWideCharStr, int cchWideChar, LPSTR lpMultiByteStr, int cbMultiByte, LPCCH lpDefaultChar, LPBOOL lpUsedDefaultChar);

WideCharToMultiByte_t* _WideCharToMultiByte = ::WideCharToMultiByte;
MultiByteToWideChar_t* _MultiByteToWideChar = ::MultiByteToWideChar;

std::string utf16_to_mb(const wchar_t* utf16, UINT encoding) {
    int utf8Length = _WideCharToMultiByte(encoding, 0, utf16, -1, nullptr, 0, NULL, NULL);
    std::string utf8;
    utf8.resize(utf8Length);
    _WideCharToMultiByte(encoding, 0, utf16, -1, utf8.data(), utf8Length, NULL, NULL);    
    return utf8;
}

std::wstring mb_to_utf16(const char* utf8, UINT encoding) {
    int utf16Length = _MultiByteToWideChar(encoding, 0, utf8, -1, nullptr, 0);
    std::wstring utf16;
    utf16.resize(utf16Length);
    _MultiByteToWideChar(encoding, 0, utf8, -1, utf16.data(), utf16Length);
    return utf16;
}

void ingame_mb_init()
{
    HMODULE win32_utf8 = GetModuleHandleW(L"win32_utf8.dll");
    if (!win32_utf8) {
        win32_utf8 = GetModuleHandleW(L"win32_utf8_d.dll");
        if (!win32_utf8) {
            return;
        }
    }
    WideCharToMultiByte_t* WideCharToMultiByteU = (WideCharToMultiByte_t*)GetProcAddress(win32_utf8, "WideCharToMultiByteU");
    MultiByteToWideChar_t* MultiByteToWideCharU = (MultiByteToWideChar_t*)GetProcAddress(win32_utf8, "MultiByteToWideCharU");

    if (WideCharToMultiByteU && MultiByteToWideCharU) {
        _WideCharToMultiByte = WideCharToMultiByteU;
        _MultiByteToWideChar = MultiByteToWideCharU;
    }
}

#pragma endregion
}

void memswap(void* buf1_, void* buf2_, unsigned int len)
{
    unsigned char* buf1 = (unsigned char*)buf1_;
    unsigned char* buf2 = (unsigned char*)buf2_;

    for (unsigned i = 0; i < len; i++) {
        unsigned char temp = buf1[i];
        buf1[i] = buf2[i];
        buf2[i] = temp;
    }
}
