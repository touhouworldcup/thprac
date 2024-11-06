#include "thprac_utils.h"
#include "thprac_licence.h"
#include "thprac_launcher_main.h"
#include "thprac_launcher_cfg.h"
#include <metrohash128.h>
#include "../3rdParties/d3d8/include/d3d8.h"
#include <dinput.h>
#include <set>
#include <format>

namespace THPrac {
#pragma region Key

const KeyDefine keyBindDefine[109] = {
{   DIK_ESCAPE      ,VK_ESCAPE,         "esc"},
{   DIK_1           ,'1',               "key_1"},
{   DIK_2           ,'2',               "key_2"},
{   DIK_3           ,'3',               "key_3"},
{   DIK_4           ,'4',               "key_4"},
{   DIK_5           ,'5',               "key_5"},
{   DIK_6           ,'6',               "key_6"},
{   DIK_7           ,'7',               "key_7"},
{   DIK_8           ,'8',               "key_8"},
{   DIK_9           ,'9',               "key_9"},
{   DIK_0           ,'0',               "key_0"},
{   DIK_MINUS       ,VK_OEM_MINUS,      "minus"},
{   DIK_EQUALS      ,VK_OEM_PLUS,       "equals"},
{   DIK_BACK        ,VK_BACK,           "backspace"},
{   DIK_TAB         ,VK_TAB,            "tab"},
{   DIK_Q           ,'Q',               "key_Q"},
{   DIK_W           ,'W',               "key_W"},
{   DIK_E           ,'E',               "key_E"},
{   DIK_R           ,'R',               "key_R"},
{   DIK_T           ,'T',               "key_T"},
{   DIK_Y           ,'Y',               "key_Y"},
{   DIK_U           ,'U',               "key_U"},
{   DIK_I           ,'I',               "key_I"},
{   DIK_O           ,'O',               "key_O"},
{   DIK_P           ,'P',               "key_P"},
{   DIK_LBRACKET    ,VK_OEM_4,          "lbracket"},
{   DIK_RBRACKET    ,VK_OEM_6,          "rbracket"},
{   DIK_RETURN      ,VK_RETURN,         "enter"},
{   DIK_LCONTROL    ,VK_LCONTROL,       "lcontrol"},
{   DIK_A           ,'A',               "key_A"},
{   DIK_S           ,'S',               "key_S"},
{   DIK_D           ,'D',               "key_D"},
{   DIK_F           ,'F',               "key_F"},
{   DIK_G           ,'G',               "key_G"},
{   DIK_H           ,'H',               "key_H"},
{   DIK_J           ,'J',               "key_J"},
{   DIK_K           ,'K',               "key_K"},
{   DIK_L           ,'L',               "key_L"},
{   DIK_SEMICOLON   ,VK_OEM_1,          "semicolon"},
{   DIK_APOSTROPHE  ,VK_OEM_7,          "apostrophe"},
{   DIK_GRAVE       ,VK_OEM_3,          "grave"},
{   DIK_LSHIFT      ,VK_LSHIFT,         "lshift"},
{   DIK_BACKSLASH   ,VK_OEM_5,          "backslash"},
{   DIK_Z           ,'Z',               "key_Z"},
{   DIK_X           ,'X',               "key_X"},
{   DIK_C           ,'C',               "key_C"},
{   DIK_V           ,'V',               "key_V"},
{   DIK_B           ,'B',               "key_B"},
{   DIK_N           ,'N',               "key_N"},
{   DIK_M           ,'M',               "key_M"},
{   DIK_COMMA       ,VK_OEM_COMMA,      "comma"},
{   DIK_PERIOD      ,VK_OEM_PERIOD,     "period"},
{   DIK_SLASH       ,VK_OEM_2,          "slash"},
{   DIK_RSHIFT      ,VK_RSHIFT,         "rshift"},
{   DIK_MULTIPLY    ,VK_MULTIPLY,       "multiply"},
{   DIK_LMENU       ,VK_LMENU,          "lmenu"},
{   DIK_SPACE       ,VK_SPACE,          "space"},
{   DIK_CAPITAL     ,VK_CAPITAL,        "capital"},
{   DIK_F1          ,VK_F1,             "key_F1"},
{   DIK_F2          ,VK_F2,             "key_F2"},
{   DIK_F3          ,VK_F3,             "key_F3"},
{   DIK_F4          ,VK_F4,             "key_F4"},
{   DIK_F5          ,VK_F5,             "key_F5"},
{   DIK_F6          ,VK_F6,             "key_F6"},
{   DIK_F7          ,VK_F7,             "key_F7"},
{   DIK_F8          ,VK_F8,             "key_F8"},
{   DIK_F9          ,VK_F9,             "key_F9"},
{   DIK_F10         ,VK_F10,            "key_F10"},
{   DIK_NUMLOCK     ,VK_NUMLOCK,        "numlock"},
{   DIK_SCROLL      ,VK_SCROLL,         "scroll"},
{   DIK_NUMPAD7     ,VK_NUMPAD7,        "numpad_7"},
{   DIK_NUMPAD8     ,VK_NUMPAD8,        "numpad_8"},
{   DIK_NUMPAD9     ,VK_NUMPAD9,        "numpad_9"},
{   DIK_SUBTRACT    ,VK_SUBTRACT,       "subtract"},
{   DIK_NUMPAD4     ,VK_NUMPAD4,        "numpad_4"},
{   DIK_NUMPAD5     ,VK_NUMPAD5,        "numpad_5"},
{   DIK_NUMPAD6     ,VK_NUMPAD6,        "numpad_6"},
{   DIK_ADD         ,VK_ADD,            "add"},
{   DIK_NUMPAD1     ,VK_NUMPAD1,        "numpad_1"},
{   DIK_NUMPAD2     ,VK_NUMPAD2,        "numpad_2"},
{   DIK_NUMPAD3     ,VK_NUMPAD3,        "numpad_3"},
{   DIK_NUMPAD0     ,VK_NUMPAD0,        "numpad_0"},
{   DIK_DECIMAL     ,VK_DECIMAL,        "decimal"},
{   DIK_OEM_102     ,VK_OEM_102,        "oem_102"},
{   DIK_F11         ,VK_F11,            "key_F11"},
{   DIK_F12         ,VK_F12,            "key_F12"},
{   DIK_F13         ,VK_F13,            "key_F13"},
{   DIK_F14         ,VK_F14,            "key_F14"},
{   DIK_F15         ,VK_F15,            "key_F15"},
{   DIK_KANA        ,VK_KANA,           "kana"},
{   DIK_CONVERT     ,VK_CONVERT,        "convert"},
{   DIK_NOCONVERT   ,VK_NONCONVERT,     "nonconvert"},
{   DIK_NUMPADENTER ,VK_RETURN,         "numpad_enter"},
{   DIK_RCONTROL    ,VK_RCONTROL,       "rcontrol"},
{   DIK_DIVIDE      ,VK_DIVIDE,         "divide"},
{   DIK_RMENU       ,VK_RMENU,          "rmenu"},
{   DIK_HOME        ,VK_HOME,           "home"},
{   DIK_UP          ,VK_UP,             "up"},
{   DIK_PRIOR       ,VK_PRIOR,          "prior"},
{   DIK_LEFT        ,VK_LEFT,           "left"},
{   DIK_RIGHT       ,VK_RIGHT,          "right"},
{   DIK_END         ,VK_END,            "end"},
{   DIK_DOWN        ,VK_DOWN,           "down"},
{   DIK_NEXT        ,VK_NEXT,           "next"},
{   DIK_INSERT      ,VK_INSERT,         "insert"},
{   DIK_DELETE      ,VK_DELETE,         "delete"},
{   DIK_LWIN        ,VK_LWIN,           "lwin"},
{   DIK_RWIN        ,VK_RWIN,           "rwin"},
{   DIK_APPS        ,VK_APPS,           "apps"},
};

    #pragma endregion
#pragma region TimeCvt
std::string GetTime_HHMMSS(int64_t ns)
{

    int64_t t_us = ns / 1000ll;
    int64_t t_ms = t_us / 1000ll;
    int64_t t_s = t_ms / 1000ll;
    int t_min = t_s / 60ll; // t_min = ns/60e9 < INT_MAX
    int t_h = t_min / 60ll;

    int t_ns = ns % 1000ll;
    t_us = t_us % 1000ll;
    t_ms = t_ms % 1000ll;
    t_s = t_s % 60ll;
    t_min = t_min % 60ll;
    return std::format("{}:{:0>2}:{:0>2}.{:0>3}.{:0>3}.{:0>3}", t_h, t_min, t_s, t_ms, t_us, t_ns);
}
std::string GetTime_YYMMDD_HHMMSS(int64_t ns)
{
    int64_t t_us = ns / 1000ll;
    int64_t t_ms = t_us / 1000ll;
    int64_t t_s = t_ms / 1000ll;
    int t_min = t_s / 60ll;
    int t_h = t_min / 60ll;
    
    int t_day = t_h / 24;
    int t_month = t_day / 30;
    int t_year = t_month / 12;
    int t_centry = t_year / 100;

    int t_ns = ns % 1000ll;
    t_us = t_us % 1000ll;
    t_ms = t_ms % 1000ll;
    t_s = t_s % 60ll;
    t_min = t_min % 60ll;
    t_h = t_h % 24ll;
    t_day = t_day % 30ll;
    t_month = t_month % 12ll;
    t_year = t_year % 100ll;

    if (t_centry > 0) {
        return std::format("{}{}{:0>2}{}{:0>2}{}{:0>2}{}{:0>2}{}{:0>2}{}{:0>2}{}{}{}{}{}{}{}",
            t_centry, S(THPRAC_INGAMEINFO_06_GAMTIME_CENTRY),
            t_year, S(THPRAC_INGAMEINFO_06_GAMTIME_YEAR),
            t_month, S(THPRAC_INGAMEINFO_06_GAMTIME_MONTH),
            t_day, S(THPRAC_INGAMEINFO_06_GAMTIME_DAY),
            t_h, S(THPRAC_INGAMEINFO_06_GAMTIME_HOUR),
            t_min, S(THPRAC_INGAMEINFO_06_GAMTIME_MINUTE),
            t_s, S(THPRAC_INGAMEINFO_06_GAMTIME_SECOND),
            t_ms, S(THPRAC_INGAMEINFO_06_GAMTIME_MILLISECOND),
            t_us, S(THPRAC_INGAMEINFO_06_GAMTIME_MICROSECOND),
            t_ns, S(THPRAC_INGAMEINFO_06_GAMTIME_NANOSECOND));
    }else if (t_year > 0) {
        return std::format("{}{}{:0>2}{}{:0>2}{}{:0>2}{}{:0>2}{}{:0>2}{}{}{}{}{}{}{}",
            t_year, S(THPRAC_INGAMEINFO_06_GAMTIME_YEAR),
            t_month, S(THPRAC_INGAMEINFO_06_GAMTIME_MONTH),
            t_day, S(THPRAC_INGAMEINFO_06_GAMTIME_DAY),
            t_h, S(THPRAC_INGAMEINFO_06_GAMTIME_HOUR),
            t_min, S(THPRAC_INGAMEINFO_06_GAMTIME_MINUTE),
            t_s, S(THPRAC_INGAMEINFO_06_GAMTIME_SECOND),
            t_ms, S(THPRAC_INGAMEINFO_06_GAMTIME_MILLISECOND),
            t_us, S(THPRAC_INGAMEINFO_06_GAMTIME_MICROSECOND),
            t_ns, S(THPRAC_INGAMEINFO_06_GAMTIME_NANOSECOND));
    } else if (t_month > 0) {
        return std::format("{}{}{:0>2}{}{:0>2}{}{:0>2}{}{:0>2}{}{}{}{}{}{}{}",
            t_month, S(THPRAC_INGAMEINFO_06_GAMTIME_MONTH),
            t_day, S(THPRAC_INGAMEINFO_06_GAMTIME_DAY),
            t_h, S(THPRAC_INGAMEINFO_06_GAMTIME_HOUR),
            t_min, S(THPRAC_INGAMEINFO_06_GAMTIME_MINUTE),
            t_s, S(THPRAC_INGAMEINFO_06_GAMTIME_SECOND),
            t_ms, S(THPRAC_INGAMEINFO_06_GAMTIME_MILLISECOND),
            t_us, S(THPRAC_INGAMEINFO_06_GAMTIME_MICROSECOND),
            t_ns, S(THPRAC_INGAMEINFO_06_GAMTIME_NANOSECOND));
    } else if (t_day > 0) {
        return std::format("{}{}{:0>2}{}{:0>2}{}{:0>2}{}{}{}{}{}{}{}",
            t_day, S(THPRAC_INGAMEINFO_06_GAMTIME_DAY),
            t_h, S(THPRAC_INGAMEINFO_06_GAMTIME_HOUR),
            t_min, S(THPRAC_INGAMEINFO_06_GAMTIME_MINUTE),
            t_s, S(THPRAC_INGAMEINFO_06_GAMTIME_SECOND),
            t_ms, S(THPRAC_INGAMEINFO_06_GAMTIME_MILLISECOND),
            t_us, S(THPRAC_INGAMEINFO_06_GAMTIME_MICROSECOND),
            t_ns, S(THPRAC_INGAMEINFO_06_GAMTIME_NANOSECOND));
    } else if (t_h > 0) {
        return std::format("{}{}{:0>2}{}{:0>2}{}{}{}{}{}{}{}",
            t_h, S(THPRAC_INGAMEINFO_06_GAMTIME_HOUR),
            t_min, S(THPRAC_INGAMEINFO_06_GAMTIME_MINUTE),
            t_s, S(THPRAC_INGAMEINFO_06_GAMTIME_SECOND),
            t_ms, S(THPRAC_INGAMEINFO_06_GAMTIME_MILLISECOND),
            t_us, S(THPRAC_INGAMEINFO_06_GAMTIME_MICROSECOND),
            t_ns, S(THPRAC_INGAMEINFO_06_GAMTIME_NANOSECOND));
    } else if (t_min > 0) {
        return std::format("{}{}{:0>2}{}{}{}{}{}{}{}",
            t_min, S(THPRAC_INGAMEINFO_06_GAMTIME_MINUTE),
            t_s, S(THPRAC_INGAMEINFO_06_GAMTIME_SECOND),
            t_ms, S(THPRAC_INGAMEINFO_06_GAMTIME_MILLISECOND),
            t_us, S(THPRAC_INGAMEINFO_06_GAMTIME_MICROSECOND),
            t_ns, S(THPRAC_INGAMEINFO_06_GAMTIME_NANOSECOND));
    } else if (t_s > 0) {
        return std::format("{}{}{}{}{}{}{}{}",
            t_s, S(THPRAC_INGAMEINFO_06_GAMTIME_SECOND),
            t_ms, S(THPRAC_INGAMEINFO_06_GAMTIME_MILLISECOND),
            t_us, S(THPRAC_INGAMEINFO_06_GAMTIME_MICROSECOND),
            t_ns, S(THPRAC_INGAMEINFO_06_GAMTIME_NANOSECOND));
    } else if (t_ms > 0) {
        return std::format("{}{}{}{}{}{}",
            t_ms, S(THPRAC_INGAMEINFO_06_GAMTIME_MILLISECOND),
            t_us, S(THPRAC_INGAMEINFO_06_GAMTIME_MICROSECOND),
            t_ns, S(THPRAC_INGAMEINFO_06_GAMTIME_NANOSECOND));
    } else if (t_us > 0) {
        return std::format("{}{}{}{}",
            t_us, S(THPRAC_INGAMEINFO_06_GAMTIME_MICROSECOND),
            t_ns, S(THPRAC_INGAMEINFO_06_GAMTIME_NANOSECOND));
    }
    return std::format("{}{}",
        t_ns, S(THPRAC_INGAMEINFO_06_GAMTIME_NANOSECOND));
}
#pragma endregion

#pragma region Directory

std::vector<std::wstring> g_directory;
int g_count_directory = 0;
void PushCurrentDirectory(LPCWSTR new_dictionary)
{
    WCHAR buffer[MAX_PATH] = { 0 };
    GetCurrentDirectoryW(MAX_PATH, buffer);
    if (g_directory.size() <= g_count_directory)
        g_directory.push_back(std::wstring(buffer));
    else
        g_directory[g_count_directory] = std::wstring(buffer);
    g_count_directory++;

    ExpandEnvironmentStringsW(new_dictionary, buffer, MAX_PATH);
    if (GetFileAttributesW(buffer) == INVALID_FILE_ATTRIBUTES) {
        CreateDirectoryW(buffer, NULL);
    }
    SetCurrentDirectoryW(buffer);
}
void PopCurrentDirectory()
{
    if (!g_directory.empty()) {
        std::wstring last_dicg_dict = g_directory[g_count_directory - 1];
        SetCurrentDirectoryW(last_dicg_dict.c_str());
        g_count_directory--;
    }
}
#pragma endregion

#pragma region FontsEnum
std::set<std::string> g_fonts; //make sure fonts are arranged
int CALLBACK EnumFontFamExProc(ENUMLOGFONTEXA* lpelfe, const TEXTMETRICA* lpntme, DWORD FontType, LPARAM lParam)
{
    if (lpelfe == NULL)
        return 0;
    if (lpelfe->elfFullName[0] != '@')
        g_fonts.insert(std::string((char*)lpelfe->elfFullName));
    return 1;
}
std::vector<std::string>& EnumAllFonts()
{
    static std::vector<std::string> res;
    if (res.size() != 0)
        return res;
    LOGFONTA logFont = { 0 };
    logFont.lfFaceName[0] = '\0';
    logFont.lfCharSet = SHIFTJIS_CHARSET;
    EnumFontFamiliesExA(GetDC(NULL), &logFont, (FONTENUMPROCA)EnumFontFamExProc, 0, 0);
    for (auto& i : g_fonts)
        res.push_back(i);
    return res;
}
std::string GetComboStr(std::vector<std::string>& str_lst)
{
    std::string res;
    for (auto& i : g_fonts) {
        res += utf16_to_mb(mb_to_utf16(i.c_str(), GetACP()).c_str(), CP_UTF8);
        res += '\0';
    }
    res += '\0';
    return res;
}

#pragma endregion

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

DWORD WINAPI CheckDLLFunction(const wchar_t* path, const char* funcName)
{
    MappedFile file(path);

    auto exeSize = file.fileSize;
    auto exeBuffer = file.fileMapView;
    if (exeSize < 128)
        return 0;
    PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)exeBuffer;
    if (!pDosHeader || pDosHeader->e_magic != 0x5a4d || (size_t)pDosHeader->e_lfanew + 512 >= exeSize)
        return 0;
    PIMAGE_NT_HEADERS pNtHeader = (PIMAGE_NT_HEADERS)((DWORD)exeBuffer + pDosHeader->e_lfanew);
    if (!pNtHeader || pNtHeader->Signature != 0x00004550)
        return 0;
    PIMAGE_SECTION_HEADER pSection = IMAGE_FIRST_SECTION(pNtHeader);
    if (!pSection)
        return 0;

    if (pNtHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress != 0 && pNtHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size != 0) {
        auto pExportSectionVA = pNtHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;
        for (DWORD i = 0; i < pNtHeader->FileHeader.NumberOfSections; i++, pSection++) {
            if (pSection->VirtualAddress <= pExportSectionVA && pSection->VirtualAddress + pSection->SizeOfRawData > pExportSectionVA) {
                auto pSectionBase = (DWORD)exeBuffer - pSection->VirtualAddress + pSection->PointerToRawData;
                PIMAGE_EXPORT_DIRECTORY pExportDirectory = (PIMAGE_EXPORT_DIRECTORY)(pSectionBase + pExportSectionVA);
                char** pExportNames = (char**)(pSectionBase + pExportDirectory->AddressOfNames);
                for (DWORD j = 0; j < pExportDirectory->NumberOfNames; ++j) {
                    auto pFunctionName = (char*)(pSectionBase + pExportNames[j]);
                    if (!strcmp(pFunctionName, funcName)) {
                        return true;
                    }
                }
            }
        }
    }

    return true;
}

}
