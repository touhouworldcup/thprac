#include "thprac_utils.h"
#include "thprac_licence.h"
#include <metrohash128.h>
#include "../3rdParties/d3d8/include/d3d8.h"


namespace THPrac {

#pragma region Locale
static void* _str_cvt_buffer(size_t size)
{
    static size_t bufferSize = 512;
    static void* bufferPtr = nullptr;
    if (!bufferPtr) {
        bufferPtr = malloc(bufferSize);
    }
    if (bufferSize < size) {
        for (; bufferSize < size; bufferSize *= 2)
            ;
        if (bufferPtr) {
            free(bufferPtr);
        }
        bufferPtr = malloc(size);
    }
    return bufferPtr;
}

RAII_CRITICAL_SECTION str_cvt_lock;

typedef int WINAPI MultiByteToWideChar_t(UINT CodePage, DWORD dwFlags, LPCCH lpMultiByteStr, int cbMultiByte, LPWSTR lpWideCharStr, int cchWideChar);
typedef int WINAPI WideCharToMultiByte_t(UINT CodePage, DWORD dwFlags, LPCWCH lpWideCharStr, int cchWideChar, LPSTR lpMultiByteStr, int cbMultiByte, LPCCH lpDefaultChar, LPBOOL lpUsedDefaultChar);

WideCharToMultiByte_t* _WideCharToMultiByte = ::WideCharToMultiByte;
MultiByteToWideChar_t* _MultiByteToWideChar = ::MultiByteToWideChar;

std::string utf16_to_mb(const wchar_t* utf16, UINT encoding)
{
    int utf8Length = _WideCharToMultiByte(encoding, 0, utf16, -1, nullptr, 0, NULL, NULL);
    cs_lock lock(*str_cvt_lock);
    char* utf8 = (char*)_str_cvt_buffer(utf8Length);
    _WideCharToMultiByte(encoding, 0, utf16, -1, utf8, utf8Length, NULL, NULL);
    return std::string(utf8);
}
std::wstring mb_to_utf16(const char* utf8, UINT encoding)
{
    int utf16Length = _MultiByteToWideChar(encoding, 0, utf8, -1, nullptr, 0);
    cs_lock lock(*str_cvt_lock);
    wchar_t* utf16 = (wchar_t*)_str_cvt_buffer(utf16Length);
    _MultiByteToWideChar(encoding, 0, utf8, -1, utf16, utf16Length);
    return std::wstring(utf16);
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

#pragma region Path
std::string GetSuffixFromPath(const char* pathC)
{
    std::string path = pathC;
    auto pos = path.rfind('.');
    if (pos != std::string::npos) {
        return path.substr(pos + 1);
    }
    return std::string("");
}

std::string GetSuffixFromPath(const std::string& path)
{
    auto pos = path.rfind('.');
    if (pos != std::string::npos) {
        return path.substr(pos + 1);
    }
    return std::string("");
}

std::string GetDirFromFullPath(const std::string& dir)
{
    auto slashPos = dir.rfind('\\');
    if (slashPos == std::string::npos) {
        slashPos = dir.rfind('/');
    }
    if (slashPos == std::string::npos) {
        return dir;
    }
    return dir.substr(0, slashPos + 1);
}

std::wstring GetDirFromFullPath(const std::wstring& dir)
{
    auto slashPos = dir.rfind(L'\\');
    if (slashPos == std::wstring::npos) {
        slashPos = dir.rfind(L'/');
    }
    if (slashPos == std::wstring::npos) {
        return dir;
    }
    return dir.substr(0, slashPos + 1);
}

std::string GetNameFromFullPath(const std::string& dir)
{
    auto slashPos = dir.rfind('\\');
    if (slashPos == std::string::npos) {
        slashPos = dir.rfind('/');
    }
    if (slashPos == std::string::npos) {
        return dir;
    }
    return dir.substr(slashPos + 1);
}

std::wstring GetNameFromFullPath(const std::wstring& dir)
{
    auto slashPos = dir.rfind(L'\\');
    if (slashPos == std::wstring::npos) {
        slashPos = dir.rfind(L'/');
    }
    if (slashPos == std::wstring::npos) {
        return dir;
    }
    return dir.substr(slashPos + 1);
}

std::string GetCleanedPath(const std::string& path)
{
    std::string result;
    wchar_t lastChar = '\0';
    for (auto& c : path) {
        if (c == '/' || c == '\\') {
            if (lastChar == '\\') {
                continue;
            } else {
                result.push_back('\\');
                lastChar = '\\';
            }
        } else {
            result.push_back(c);
            lastChar = c;
        }
    }
    return result;
}

std::wstring GetCleanedPath(const std::wstring& path)
{
    std::wstring result;
    wchar_t lastChar = '\0';
    for (auto& c : path) {
        if (c == L'/' || c == L'\\') {
            if (lastChar == L'\\') {
                continue;
            } else {
                result.push_back(L'\\');
                lastChar = L'\\';
            }
        } else {
            result.push_back(c);
            lastChar = c;
        }
    }
    return result;
}

std::string GetUnifiedPath(const std::string& path)
{
    std::string result;
    wchar_t lastChar = '\0';
    for (auto& c : path) {
        if (c == '/' || c == '\\') {
            if (lastChar == '\\') {
                continue;
            } else {
                result.push_back('\\');
                lastChar = '\\';
            }
        } else {
            char lower = static_cast<char>(tolower(c));
            result.push_back(lower);
            lastChar = lower;
        }
    }
    return result;
}

std::wstring GetUnifiedPath(const std::wstring& path)
{
    std::wstring result;
    wchar_t lastChar = '\0';
    for (auto& c : path) {
        if (c == L'/' || c == L'\\') {
            if (lastChar == L'\\') {
                continue;
            } else {
                result.push_back(L'\\');
                lastChar = L'\\';
            }
        } else {
            auto lower = towlower(c);
            result.push_back(lower);
            lastChar = lower;
        }
    }
    return result;
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
