#include "thprac_utils.h"
#include "thprac_licence.h"
#include <metrohash128.h>
#include "../3rdParties/d3d8/include/d3d8.h"

#include <shlobj.h>


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
    max_mask |= max_mask >> 32;

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

char* FormatNumberFixedPoint(int value, int dot_pos, char* buffer) {
    bool is_negative = value < 0;
    if (is_negative) {
        value = -value;
        *buffer++ = '-';
    }

    char temp[32];

    int digit_count = 0;
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