#pragma once

#define NOMINMAX
#include <Windows.h>

#include <stdint.h>

#include "utils/utils.h"

#include "thprac_version.h"
#include "thprac_gui_components.h"
#include "thprac_gui_impl_dx8.h"
#include "thprac_gui_impl_dx9.h"
#include "thprac_gui_impl_win32.h"
#include "thprac_locale_def.h"
#include "thprac_gui_locale.h"

#include <charconv>
#include <concepts>
#include <imgui.h>
#include <memory>
#include <optional>
#include <random>
#include <string>
#include <utility>
#include <type_traits>
#include <vector>

#include "yyjson.h"

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

#pragma region Locale
std::string utf16_to_mb(const wchar_t* utf16, UINT encoding);
std::wstring mb_to_utf16(const char* utf8, UINT encoding);

inline std::string utf16_to_utf8(const wchar_t* utf16) { return utf16_to_mb(utf16, CP_UTF8); }
inline std::wstring utf8_to_utf16(const char* utf8) { return mb_to_utf16(utf8, CP_UTF8); }

void ingame_mb_init();
#pragma endregion

#pragma region Path
template <typename T>
constexpr bool PathsCompare(const T* a, size_t a_len, const T* b, size_t b_len) {
    size_t i = 0, j = 0;

    while (i < a_len && j < b_len) {
        bool a_is_sep = (a[i] == '/' || a[i] == '\\');
        bool b_is_sep = (b[j] == '/' || b[j] == '\\');

        if (a_is_sep && b_is_sep) {
            while (i < a_len && (a[i] == '/' || a[i] == '\\'))
                i++;
            while (j < b_len && (b[j] == '/' || b[j] == '\\'))
                j++;
            continue;
        }

        if (a_is_sep || b_is_sep)
            return 0;

        if (t_tolower(a[i]) != t_tolower(b[j])) {
            return 0;
        }

        i++;
        j++;
    }

    // Skip any trailing separators on both paths
    while (i < a_len && (a[i] == '/' || a[i] == '\\'))
        i++;
    while (j < b_len && (b[j] == '/' || b[j] == '\\'))
        j++;

    // Both must be fully consumed.
    return i == a_len && j == b_len;
}

template <typename T>
constexpr bool PathsCompare(const T* a, const T* b) {
    return PathsCompare(a, t_strlen(a), b, t_strlen(b));
}
#pragma endregion

#pragma region Algorithm
template <typename T>
unsigned int binary_search(const T* arr, size_t len, T needle) {
    size_t low = 0;
    size_t high = len - 1;

    while (low <= high) {
        // Prevents potential (low + high) overflow
        size_t mid = low + (high - low) / 2;

        if (arr[mid] == needle) {
            return (unsigned int)mid;
        }

        if (arr[mid] < needle) {
            low = mid + 1;
        } else {
            // Handle high underflow if high is 0
            if (mid == 0)
                break;
            high = mid - 1;
        }
    }

    return (unsigned int)-1;
}

// thcrap is able to use it's entire expression parser for this
// Here I'll just return true for any non empty string that doesn't say false or no
__forceinline bool str_parse_bool(const char* str) {
    if (*str) {
        return _stricmp(str, "FALSE") == 0 || _stricmp(str, "NO") == 0;
    } else {
        return false;
    }
}
#pragma endregion

#pragma region Json

// Parse numbers encoded as any type into an int of size 8/16/32/64 or float or double
// while performing all required conversions automatically
template <typename T>
requires((std::integral<T> || std::same_as<T, float> || std::same_as<T, double>) && sizeof(T) <= 8)
bool yyjson_eval_numeric(yyjson_val* val, T* out) noexcept
{
    switch (yyjson_get_type(val)) {
    case YYJSON_TYPE_BOOL:
        *out = unsafe_yyjson_get_bool(val);
        return true;
    case YYJSON_TYPE_NUM:
        switch (val->tag & 0b000111000) {
        case YYJSON_SUBTYPE_UINT:
            *out = (T)val->uni.u64;
            return true;
        case YYJSON_SUBTYPE_SINT:
            *out = (T)val->uni.i64;
            return true;
        case YYJSON_SUBTYPE_REAL:
            *out = (T)val->uni.f64;
            return true;
        default:
            return false;
        }
    case YYJSON_TYPE_STR: {
        const char* val_str = unsafe_yyjson_get_str(val);
        if constexpr (std::same_as<T, bool>) {
            *out = str_parse_bool(val_str);
            return true;
        } else {
            return std::from_chars(val_str, val_str + t_strlen(val_str), *out).ec == std::errc();
        }
    }
    default:
        return false;
    }
}

// C++ overloads for yyjson_mut_obj_add, mainly for macros
// In C, this would be the perfect use for _Generic, which
// would also guarantee inlining even in debug mode.
//
#define CPP_YYJSON_MUT_OBJ_ADD_DECL(tname_cpp, tname_yy) \
__forceinline bool yyjson_mut_obj_add(yyjson_mut_doc* doc, yyjson_mut_val* obj, const char* key, tname_cpp val) { \
    return yyjson_mut_obj_add_##tname_yy(doc, obj, key, val); \
}
CPP_YYJSON_MUT_OBJ_ADD_DECL(bool, bool)
CPP_YYJSON_MUT_OBJ_ADD_DECL(double, double)
CPP_YYJSON_MUT_OBJ_ADD_DECL(float, float)
CPP_YYJSON_MUT_OBJ_ADD_DECL(int, sint)
CPP_YYJSON_MUT_OBJ_ADD_DECL(unsigned int, uint)
CPP_YYJSON_MUT_OBJ_ADD_DECL(int64_t, sint)
CPP_YYJSON_MUT_OBJ_ADD_DECL(uint64_t, uint)
CPP_YYJSON_MUT_OBJ_ADD_DECL(const char*, str)
#undef CPP_YYJSON_MUT_OBJ_ADD_DECL
#pragma endregion

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
