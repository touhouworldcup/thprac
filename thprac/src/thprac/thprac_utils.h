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
    if (!seed) {
        seed = (std::mt19937::result_type)time(0);
    }
    auto dice_rand = std::bind(std::uniform_int_distribution<T>(min, max), std::mt19937(seed));
    return dice_rand;
}
DWORD WINAPI CheckDLLFunction(const wchar_t* path, const char* funcName);
}

#define w32u8_alloca(type, size) ((type*)_alloca((size) * sizeof(type)))
#define w32u8_freea(name) do; while(0) /* require a semi-colon */

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