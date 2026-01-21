#pragma once

#define NOMINMAX

#include "thprac_version.h"
#include "thprac_gui_components.h"
#include "thprac_gui_impl_dx8.h"
#include "thprac_gui_impl_dx9.h"
#include "thprac_gui_impl_win32.h"
#include "thprac_hook.h"
#include "thprac_locale_def.h"
#include "thprac_gui_locale.h"

#include <Windows.h>
#include <cstdint>
#include <imgui.h>
#include <memory>
#include <optional>
#pragma warning(push)
#pragma warning(disable : 26451)
#pragma warning(disable : 26495)
#pragma warning(disable : 33010)
#pragma warning(disable : 26819)
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#pragma warning(pop)
#include <random>
#include <string>
#include <utility>
#include <vector>

namespace THPrac {
void InitHook(int ver,void* refreshRateAddr1 = nullptr, void* refreshRateAddr2 = nullptr);

int SetUpClock();
double ResetClock(int id);
double CheckTimePassed(int id);

struct KeyDefine {
    uint16_t dik;
    uint16_t vk;
    std::string keyname;
    bool operator==(const KeyDefine& k2)const
    {
        return dik == k2.dik && vk == k2.vk && keyname == k2.keyname;
    }
};
struct KeyDefineHashFunction {
    size_t operator()(const KeyDefine& key) const
    {
        std::hash<uint16_t> h;
        std::hash<std::string> h2;
        std::hash<size_t> h3;
        return h3(h(key.dik) + h(key.vk) + h2(key.keyname));
    }
};
extern const KeyDefine keyBindDefine[109];

struct MappedFile {
    HANDLE fileMap = nullptr;
    HANDLE hFile = INVALID_HANDLE_VALUE;
    size_t fileSize = 0;
    void* fileMapView = nullptr;

    MappedFile(const wchar_t* fn, size_t max_size = -1)
    {
        hFile = CreateFileW(fn, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (hFile == INVALID_HANDLE_VALUE) {
            return;
        }
        fileSize = GetFileSize(hFile, nullptr);
        if (fileSize > max_size)
            return;
        fileMap = CreateFileMappingW(hFile, nullptr, PAGE_READONLY, 0, fileSize, nullptr);
        if (fileMap == nullptr) {
            CloseHandle(hFile);
            return;
        }
        fileMapView = MapViewOfFile(fileMap, FILE_MAP_READ, 0, 0, fileSize);
        if (!fileMapView) {
            CloseHandle(hFile);
            CloseHandle(fileMap);
            return;
        }
    }
    ~MappedFile()
    {
        if (fileMapView)
            UnmapViewOfFile(fileMapView);
        if (fileMap)
            CloseHandle(fileMap);
        if (hFile != INVALID_HANDLE_VALUE)
            CloseHandle(hFile);
    }
};

struct cs_lock {
    CRITICAL_SECTION* cs;
    cs_lock(CRITICAL_SECTION* cs)
    {
        EnterCriticalSection(cs);
        this->cs = cs;
    }
    ~cs_lock()
    {
        LeaveCriticalSection(cs);
    }
};

struct RAII_CRITICAL_SECTION {
    CRITICAL_SECTION cs;
    RAII_CRITICAL_SECTION()
    {
        InitializeCriticalSection(&cs);
    }

    ~RAII_CRITICAL_SECTION()
    {
        DeleteCriticalSection(&cs);
    }

    CRITICAL_SECTION* operator*() {
        return &cs;
    }
};
#pragma region FontsEnum
std::vector<std::string>& EnumAllFonts();
std::string GetComboStr(std::vector<std::string>& str_lst);
#pragma endregion

#pragma region TimeCvt
std::string GetTime_HHMMSS(int64_t ns);
std::string GetTime_YYMMDD_HHMMSS(int64_t ns);
#pragma endregion

#pragma region Directory
void PushCurrentDirectory(LPCWSTR new_dictionary);
void PopCurrentDirectory();
#pragma endregion

#pragma region Locale
std::string utf16_to_mb(const wchar_t* utf16, UINT encoding);
std::wstring mb_to_utf16(const char* utf8, UINT encoding);

inline std::string utf16_to_utf8(const wchar_t* utf16) { return utf16_to_mb(utf16, CP_UTF8); }
inline std::wstring utf8_to_utf16(const char* utf8) { return mb_to_utf16(utf8, CP_UTF8); }

void ingame_mb_init();
#pragma endregion

#pragma region Path
std::string GetSuffixFromPath(const char* pathC);
std::string GetSuffixFromPath(const std::string& path);
std::string GetDirFromFullPath(const std::string& dir);
std::wstring GetDirFromFullPath(const std::wstring& dir);
std::string GetNameFromFullPath(const std::string& dir);
std::wstring GetNameFromFullPath(const std::wstring& dir);
std::string GetCleanedPath(const std::string& path);
std::wstring GetCleanedPath(const std::wstring& path);
std::string GetUnifiedPath(const std::string& path);
std::wstring GetUnifiedPath(const std::wstring& path);
#pragma endregion

template <typename T>
static std::function<T(void)> GetRndGenerator(T min, T max, std::mt19937::result_type seed = 0)
{
    if (!seed)
        seed = (std::mt19937::result_type)time(0);
    std::mt19937 engine(seed);
    if constexpr (std::is_floating_point_v<T>) {
        return std::bind(std::uniform_real_distribution<T>(min, max), engine);
    } else {
        return std::bind(std::uniform_int_distribution<T>(min, max), engine);
    }
}
template <typename T>
static std::function<T(void)> GetRndGeneratorNormal(T mean, T sigma, std::mt19937::result_type seed = 0)
{
    if (!seed) {
        seed = (std::mt19937::result_type)time(0);
    }
    return std::bind(std::normal_distribution<T>(mean, sigma), std::mt19937(seed));
}
DWORD WINAPI CheckDLLFunction(const wchar_t* path, const char* funcName);
}


#define w32u8_alloca(type, size) ((type*)_alloca((size) * sizeof(type)))
#define w32u8_freea(name) \
    do                    \
        ;                 \
    while (0) /* require a semi-colon */

#if VLA_SUPPORT
#define VLA(type, name, size) \
    type name##_vla[(size)];  \
    type* name = name##_vla /* to ensure that [name] is a modifiable lvalue */
#define VLA_FREE(name) \
    do                 \
        ;              \
    while (0) /* require a semi-colon */
#else
#define VLA(type, name, size) \
    type* name = w32u8_alloca(type, size)
#define VLA_FREE(name) \
    w32u8_freea(name)
#endif


#define _MACRO_CAT(arg1, arg2) arg1##arg2
#define MACRO_CAT(arg1, arg2) _MACRO_CAT(arg1, arg2)

#define BASE_NOP0
#define BASE_NOP1 "90"
#define BASE_NOP2 "6690"
#define BASE_NOP3 "0F1F00"
#define BASE_NOP4 "0F1F4000"
#define BASE_NOP5 "0F1F440000"
#define BASE_NOP6 "660F1F440000"
#define BASE_NOP7 "0F1F8000000000"
#define BASE_NOP8 "0F1F840000000000"
#define BASE_NOP9 "660F1F840000000000"
#define BASE_NOP10 "662E0F1F840000000000"
#define BASE_NOP11 BASE_NOP5 BASE_NOP6
#define BASE_NOP12 BASE_NOP6 BASE_NOP6
#define BASE_NOP13 BASE_NOP6 BASE_NOP7
#define BASE_NOP14 BASE_NOP7 BASE_NOP7
#define BASE_NOP15 BASE_NOP7 BASE_NOP8
#define BASE_NOP16 BASE_NOP8 BASE_NOP8
#define BASE_NOP17 BASE_NOP8 BASE_NOP9
#define BASE_NOP18 BASE_NOP9 BASE_NOP9
#define BASE_NOP19 BASE_NOP9 BASE_NOP10
#define BASE_NOP20 BASE_NOP10 BASE_NOP10
#define BASE_NOP21 BASE_NOP7 BASE_NOP7 BASE_NOP7
#define BASE_NOP22 BASE_NOP7 BASE_NOP7 BASE_NOP8
#define BASE_NOP23 BASE_NOP7 BASE_NOP8 BASE_NOP8
#define BASE_NOP24 BASE_NOP8 BASE_NOP8 BASE_NOP8
#define BASE_NOP25 BASE_NOP8 BASE_NOP8 BASE_NOP9
#define BASE_NOP26 BASE_NOP8 BASE_NOP9 BASE_NOP9
#define BASE_NOP27 BASE_NOP9 BASE_NOP9 BASE_NOP9
#define BASE_NOP28 BASE_NOP9 BASE_NOP9 BASE_NOP10
#define BASE_NOP29 BASE_NOP9 BASE_NOP10 BASE_NOP10
#define BASE_NOP30 BASE_NOP10 BASE_NOP10 BASE_NOP10
#define BASE_NOP31 BASE_NOP7 BASE_NOP8 BASE_NOP8 BASE_NOP8
#define BASE_NOP32 BASE_NOP8 BASE_NOP8 BASE_NOP8 BASE_NOP8
#define BASE_NOP33 BASE_NOP8 BASE_NOP8 BASE_NOP8 BASE_NOP9
#define BASE_NOP34 BASE_NOP8 BASE_NOP8 BASE_NOP9 BASE_NOP9
#define BASE_NOP35 BASE_NOP8 BASE_NOP9 BASE_NOP9 BASE_NOP9
#define BASE_NOP36 BASE_NOP9 BASE_NOP9 BASE_NOP9 BASE_NOP9
#define BASE_NOP37 BASE_NOP9 BASE_NOP9 BASE_NOP9 BASE_NOP10
#define BASE_NOP38 BASE_NOP9 BASE_NOP9 BASE_NOP10 BASE_NOP10
#define BASE_NOP39 BASE_NOP9 BASE_NOP10 BASE_NOP10 BASE_NOP10
#define BASE_NOP40 BASE_NOP10 BASE_NOP10 BASE_NOP10 BASE_NOP10
#define BASE_NOP41 BASE_NOP8 BASE_NOP8 BASE_NOP8 BASE_NOP8 BASE_NOP9
#define BASE_NOP42 BASE_NOP8 BASE_NOP8 BASE_NOP8 BASE_NOP9 BASE_NOP9
#define BASE_NOP43 BASE_NOP8 BASE_NOP8 BASE_NOP9 BASE_NOP9 BASE_NOP9
#define BASE_NOP44 BASE_NOP8 BASE_NOP9 BASE_NOP9 BASE_NOP9 BASE_NOP9
#define BASE_NOP45 BASE_NOP9 BASE_NOP9 BASE_NOP9 BASE_NOP9 BASE_NOP9
#define BASE_NOP46 BASE_NOP9 BASE_NOP9 BASE_NOP9 BASE_NOP9 BASE_NOP10
#define BASE_NOP47 BASE_NOP9 BASE_NOP9 BASE_NOP9 BASE_NOP10 BASE_NOP10
#define BASE_NOP48 BASE_NOP9 BASE_NOP9 BASE_NOP10 BASE_NOP10 BASE_NOP10
#define BASE_NOP49 BASE_NOP9 BASE_NOP10 BASE_NOP10 BASE_NOP10 BASE_NOP10
#define BASE_NOP50 BASE_NOP10 BASE_NOP10 BASE_NOP10 BASE_NOP10 BASE_NOP10
#define BASE_NOP51 BASE_NOP8 BASE_NOP8 BASE_NOP8 BASE_NOP9 BASE_NOP9 BASE_NOP9
#define BASE_NOP52 BASE_NOP8 BASE_NOP8 BASE_NOP9 BASE_NOP9 BASE_NOP9 BASE_NOP9
#define BASE_NOP53 BASE_NOP8 BASE_NOP9 BASE_NOP9 BASE_NOP9 BASE_NOP9 BASE_NOP9
#define BASE_NOP54 BASE_NOP9 BASE_NOP9 BASE_NOP9 BASE_NOP9 BASE_NOP9 BASE_NOP9
#define BASE_NOP55 BASE_NOP9 BASE_NOP9 BASE_NOP9 BASE_NOP9 BASE_NOP9 BASE_NOP10
#define BASE_NOP56 BASE_NOP9 BASE_NOP9 BASE_NOP9 BASE_NOP9 BASE_NOP10 BASE_NOP10
#define BASE_NOP57 BASE_NOP9 BASE_NOP9 BASE_NOP9 BASE_NOP10 BASE_NOP10 BASE_NOP10
#define BASE_NOP58 BASE_NOP9 BASE_NOP9 BASE_NOP10 BASE_NOP10 BASE_NOP10 BASE_NOP10
#define BASE_NOP59 BASE_NOP9 BASE_NOP10 BASE_NOP10 BASE_NOP10 BASE_NOP10 BASE_NOP10
#define BASE_NOP60 BASE_NOP10 BASE_NOP10 BASE_NOP10 BASE_NOP10 BASE_NOP10 BASE_NOP10
#define BASE_NOP61 BASE_NOP8 BASE_NOP8 BASE_NOP9 BASE_NOP9 BASE_NOP9 BASE_NOP9 BASE_NOP9
#define BASE_NOP62 BASE_NOP8 BASE_NOP9 BASE_NOP9 BASE_NOP9 BASE_NOP9 BASE_NOP9 BASE_NOP9
#define BASE_NOP63 BASE_NOP9 BASE_NOP9 BASE_NOP9 BASE_NOP9 BASE_NOP9 BASE_NOP9 BASE_NOP9
#define BASE_NOP64 BASE_NOP9 BASE_NOP9 BASE_NOP9 BASE_NOP9 BASE_NOP9 BASE_NOP9 BASE_NOP10
#define BASE_NOP65 BASE_NOP9 BASE_NOP9 BASE_NOP9 BASE_NOP9 BASE_NOP9 BASE_NOP10 BASE_NOP10
#define BASE_NOP66 BASE_NOP9 BASE_NOP9 BASE_NOP9 BASE_NOP9 BASE_NOP10 BASE_NOP10 BASE_NOP10
#define BASE_NOP67 BASE_NOP9 BASE_NOP9 BASE_NOP9 BASE_NOP10 BASE_NOP10 BASE_NOP10 BASE_NOP10
#define BASE_NOP68 BASE_NOP9 BASE_NOP9 BASE_NOP10 BASE_NOP10 BASE_NOP10 BASE_NOP10 BASE_NOP10
#define BASE_NOP69 BASE_NOP9 BASE_NOP10 BASE_NOP10 BASE_NOP10 BASE_NOP10 BASE_NOP10 BASE_NOP10
#define BASE_NOP70 BASE_NOP10 BASE_NOP10 BASE_NOP10 BASE_NOP10 BASE_NOP10 BASE_NOP10 BASE_NOP10
#define BASE_NOP71 BASE_NOP8 BASE_NOP9 BASE_NOP9 BASE_NOP9 BASE_NOP9 BASE_NOP9 BASE_NOP9 BASE_NOP9
#define BASE_NOP72 BASE_NOP9 BASE_NOP9 BASE_NOP9 BASE_NOP9 BASE_NOP9 BASE_NOP9 BASE_NOP9 BASE_NOP9
#define BASE_NOP73 BASE_NOP9 BASE_NOP9 BASE_NOP9 BASE_NOP9 BASE_NOP9 BASE_NOP9 BASE_NOP9 BASE_NOP10
#define BASE_NOP74 BASE_NOP9 BASE_NOP9 BASE_NOP9 BASE_NOP9 BASE_NOP9 BASE_NOP9 BASE_NOP10 BASE_NOP10
#define BASE_NOP75 BASE_NOP9 BASE_NOP9 BASE_NOP9 BASE_NOP9 BASE_NOP9 BASE_NOP10 BASE_NOP10 BASE_NOP10
#define BASE_NOP76 BASE_NOP9 BASE_NOP9 BASE_NOP9 BASE_NOP9 BASE_NOP10 BASE_NOP10 BASE_NOP10 BASE_NOP10
#define BASE_NOP77 BASE_NOP9 BASE_NOP9 BASE_NOP9 BASE_NOP10 BASE_NOP10 BASE_NOP10 BASE_NOP10 BASE_NOP10
#define BASE_NOP78 BASE_NOP9 BASE_NOP9 BASE_NOP10 BASE_NOP10 BASE_NOP10 BASE_NOP10 BASE_NOP10 BASE_NOP10
#define BASE_NOP79 BASE_NOP9 BASE_NOP10 BASE_NOP10 BASE_NOP10 BASE_NOP10 BASE_NOP10 BASE_NOP10 BASE_NOP10
#define BASE_NOP80 BASE_NOP10 BASE_NOP10 BASE_NOP10 BASE_NOP10 BASE_NOP10 BASE_NOP10 BASE_NOP10 BASE_NOP10

#define NOP(val) MACRO_CAT(BASE_NOP, val)
