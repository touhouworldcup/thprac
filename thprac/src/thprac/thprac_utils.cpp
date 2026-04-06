#include "utils/wininternal.h"
#include "thprac_utils.h"
#include "thprac_licence.h"
#include <metrohash128.h>
#include "../3rdParties/d3d8/include/d3d8.h"

#include <shlobj.h>


namespace THPrac {

#pragma region Windows Version
wchar_t* windows_version_str() {
    static constinit wchar_t version[64] = {};
    if (version[0] != 0) {
        return version;
    }

    auto* wine_get_version = (const char*(*)())GetProcAddress(GetModuleHandleW(L"ntdll.dll"), "wine_get_version");
    if (wine_get_version) {
        // wine_get_version never returns a null pointer
        // https://gitlab.winehq.org/wine/wine/-/blob/wine-11.0/dlls/ntdll/version.c?ref_type=tags#L221
        const char* p = wine_get_version();

        std::wstring_view sv = L"Wine ";
        memcpy(version, sv.data(), sv.length() * sizeof(wchar_t));

        for (size_t i = sv.length(); *p; i++, p++) {
            version[i] = *p;
        }
        return version;
    }

    int wrote = 0;
#define snprintf_cat(fmt, ...) wrote += _snwprintf(version + wrote, 64 - wrote, fmt, __VA_ARGS__)

    auto major = Kuser_Shared_Data->NtMajorVersion;
    auto minor = Kuser_Shared_Data->NtMinorVersion;
    auto product = Kuser_Shared_Data->NtProductType;

    // As per https://msdn.microsoft.com/en-us/library/windows/hardware/ff563620(v=vs.85).aspx
    const wchar_t* winver = nullptr;
    if (major == 10) {
        // Windows 11 also use major = 10 and minor = 0
        if (Kuser_Shared_Data->NtBuildNumber >= 22000) {
            winver = L"11";
        } else {
            winver = L"10";
        }
    } else if (major == 6 && minor == 3) {
        winver = L"8.1";
    } else if (major == 6 && minor == 2 && product == VER_NT_WORKSTATION) {
        winver = L"8";
    } else if (major == 6 && minor == 2 && product != VER_NT_WORKSTATION) {
        winver = L"Server 2012";
    } else if (major == 6 && minor == 1 && product == VER_NT_WORKSTATION) {
        winver = L"7";
    } else if (major == 6 && minor == 1 && product != VER_NT_WORKSTATION) {
        winver = L"Server 2008 R2";
    } else if (major == 6 && minor == 0 && product == VER_NT_WORKSTATION) {
        winver = L"Vista";
    } else if (major == 6 && minor == 0 && product != VER_NT_WORKSTATION) {
        winver = L"Server 2008";
    } else if (major == 5 && minor == 2 && Kuser_Shared_Data->SuiteMask == VER_SUITE_WH_SERVER) {
        winver = L"Home Server";
    } else if (major == 5 && minor == 2) {
        winver = L"Server 2003";
    } else if (major == 5 && minor == 1) {
        winver = L"XP";
    } else if (major == 5 && minor == 0) {
        winver = L"2000";
    }

    if (winver) {
        snprintf_cat(L"Windows %s", winver);
    } else {
        snprintf_cat(L"Windows %u.%u", major, minor);
    }

    auto* peb = CurrentPeb();
    if (peb->OSCSDMajorVersion != 0) {
        snprintf_cat(L", Service Pack %hu", peb->OSCSDMajorVersion);
        if (peb->OSCSDMinorVersion != 0) {
            snprintf_cat(L".%hu", peb->OSCSDMajorVersion);
        }
    }
    if (Kuser_Shared_Data->NtBuildNumber != 0) {
        snprintf_cat(L", Build %u", Kuser_Shared_Data->NtBuildNumber);
    }
#undef snprintf_cat
    return version;
}
#pragma endregion

#pragma region Error

std::wstring ErrorToString(DWORD err) {
    std::wstring ret(32767, 0);
    DWORD wrote = FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM, nullptr, err, 0, ret.data(), 32767, nullptr);
    ret.resize(wrote);
    return ret;
}

#pragma endregion

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

bool utf8_adv(const char** utf8_param, const char* end, uint32_t* out) {
    const char* s = *utf8_param;
    unsigned char byte = *(unsigned char*)s;

    if (byte < 0x80) {
        if (out) {
            *out = byte;
        }
        (*utf8_param)++;
        return true;
    }
    auto len = utf8_len[(byte >> 4) & 7];
    // If the first 4 bits from the left are 1,
    // the one after MUST be 0
    if (len == 4 && byte & 8) {
        return false;
    }
    if (len == 0) {
        return false;
    }
    if (s + len > end) {
        return false;
    }

    uint32_t cp = byte & (0x7F >> len);
    for (uint32_t i = 1; i < len; ++i) {
        unsigned char c = (unsigned char)s[i];
        // continuation byte must be 10xxxxxx
        if ((c & 0xC0) != 0x80) {
            return false;
        }
        cp = (cp << 6) | (c & 0x3F);
    }

    // YOUR      LONG
    static const uint32_t min_cp[5] = { 0, 0, 0x80, 0x800, 0x10000 };
    if (cp < min_cp[len]) {
        return false;
    }
    if (out) {
        *out = cp;
    }
    *utf8_param += len;
    return true;
}

bool utf8_utf16_adv(const char** utf8_param, const char* end, wchar_t** utf16_param, wchar_t* utf16_end) {
    uint32_t cp;
    if (!utf8_adv(utf8_param, end, &cp)) {
        return false;
    }
    wchar_t* out = *utf16_param;

    if (cp <= 0xFFFF) {
        if (out >= utf16_end) {
            return false;
        }
        *out++ = (uint16_t)cp;
    } else {
        if (out + 1 >= utf16_end) {
            return false;
        }
        cp -= 0x10000;
        out[0] = (wchar_t)(0xD800 | (cp >> 10));
        out[1] = (wchar_t)(0xDC00 | (cp & 0x3FF));
        out += 2;
    }

    *utf16_param = out;
    return true;
}

#pragma endregion

#pragma region Select Folder

// Needs 3 possible return values. Anything that tries to call SelectFolder will only need to know if a folder has been selected or not
// The SelectFolder function itself though needs to know if the user clicked "Cancel", or if the function actually just failed, to determine
// if the Windows XP folder picker should be called as a backup.
enum SelectFolderVista_Status {
    SF_SUCCESS,
    SF_CANCEL,
    SF_FAILED,
};

static SelectFolderVista_Status SelectFolderVista(std::wstring& out, HWND hwnd) {
    IFileDialog* pfd;

    CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pfd));

    if (!pfd) {
        return SF_FAILED;
    }

    defer(pfd->Release());

    pfd->SetOptions(FOS_NOCHANGEDIR | FOS_PICKFOLDERS | FOS_FORCEFILESYSTEM | FOS_PATHMUSTEXIST | FOS_FILEMUSTEXIST | FOS_DONTADDTORECENT);
    pfd->SetTitle(L"Deez");

    HRESULT hr = pfd->Show(hwnd);
    if (hr == HRESULT_FROM_WIN32(ERROR_CANCELLED)) {
        return SF_CANCEL;
    }

    IShellItem* result;
    pfd->GetResult(&result);
    if (!result) {
        return SF_FAILED;
    }
    defer(result->Release());

    wchar_t* path;
    if (FAILED(result->GetDisplayName(SIGDN_FILESYSPATH, &path))) {
        return SF_FAILED;
    }
    out = path;
    CoTaskMemFree(path);
    return SF_SUCCESS;
}

static bool SelectFolderXP(std::wstring& out, HWND hwnd) {
    BROWSEINFOW bi = {
        .hwndOwner = hwnd,
        .ulFlags = BIF_EDITBOX | BIF_NONEWFOLDERBUTTON
    };
    LPITEMIDLIST res = SHBrowseForFolderW(&bi);
    out.resize(MAX_PATH + 1);

    bool ret = SHGetPathFromIDListW(res, out.data());
    if (auto pos = out.find(L'\0'); pos != std::wstring::npos) {
        out.resize(pos);
    }
    return ret;
}

bool SelectFolder(std::wstring& out, HWND hwnd) {
    auto res = SelectFolderVista(out, hwnd);
    if (res == SF_FAILED) {
        return SelectFolderXP(out, hwnd);
    } else {
        return res == SF_SUCCESS;
    }
}
#pragma endregion

}

#pragma region They have to go somewhere

unsigned rand_range(unsigned max) {
    unsigned max_mask = max;
    max_mask |= max_mask >> 1;
    max_mask |= max_mask >> 2;
    max_mask |= max_mask >> 4;
    max_mask |= max_mask >> 8;
    max_mask |= max_mask >> 16;

    for (;;) {
        unsigned val = std::bit_cast<unsigned>(rand()) & max_mask;
        if (val < max) {
            return val;
        }
    }
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

char* FormatNumberWithCommas(int64_t val, char* buffer) {
    char temp[32];
    sprintf(temp, "%lld", val);
    int len = strlen(temp);
    int commas = (len - (val < 0 ? 2 : 1)) / 3;

    char* dst = buffer + len + commas;
    *dst-- = '\0';

    int group = 0;

    for (int i = len - 1; i >= 0; --i) {
        if (temp[i] == '-') {
            *dst-- = '-';
            break;
        }

        if (group == 3) {
            *dst-- = ',';
            group = 0;
        }

        *dst-- = temp[i];
        ++group;
    }

    return buffer;
}

char* FormatNumberFixedPoint(int value, unsigned int dot_pos, char* buffer) {
    bool is_negative = value < 0;
    if (is_negative) {
        value = -value;
        *buffer++ = '-';
    }

    char temp[32];

    size_t digit_count = 0;
    do {
        temp[digit_count++] = (value % 10) + '0';
        value /= 10;
    } while (value > 0);

    if (dot_pos >= digit_count) {
        buffer[0] = '0';
        buffer[1] = '.';

        size_t i = 0;
        while (i < dot_pos - digit_count) {
            buffer[2 + i++] = '0';
        }

        for (size_t j = 0; j < digit_count; j++) {
            buffer[i + j + 2] = temp[digit_count - j - 1];
        }
        buffer[digit_count + i + 2] = 0;
    } else {
        for (size_t i = 0, buf_pos = 0; i < digit_count; i++) {
            if (i == digit_count - dot_pos) {
                buffer[buf_pos++] = '.';
            }
            buffer[buf_pos++] = temp[digit_count - i - 1];
        }
        buffer[digit_count + 1] = 0;
    }
    return buffer - (unsigned int)is_negative;
}
#pragma endregion