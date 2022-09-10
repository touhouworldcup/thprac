#pragma once

#define NOMINMAX

#include "thprac_version.h"
#include "thprac_gui_components.h"
#include "thprac_gui_impl_dx8.h"
#include "thprac_gui_impl_dx9.h"
#include "thprac_gui_impl_win32.h"
#include "thprac_hook.h"
#include "thprac_locale_def.h"
#include "thprac_log.h"

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
#include <tsl/robin_map.h>
#include <utility>
#include <vector>

#define MB_INFO(str) MessageBoxA(NULL, str, str, MB_OK);

namespace THPrac {

struct MappedFile {
    HANDLE fileMap = NULL;
    HANDLE hFile = INVALID_HANDLE_VALUE;
    size_t fileSize = 0;
    void* fileMapView = NULL;

    MappedFile(const wchar_t* fn, size_t max_size = -1)
    {
        hFile = CreateFileW(fn, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if (hFile == INVALID_HANDLE_VALUE) {
            return;
        }
        fileSize = GetFileSize(hFile, NULL);
        if (fileSize > max_size)
            return;
        fileMap = CreateFileMappingW(hFile, NULL, PAGE_READONLY, 0, fileSize, NULL);
        if (fileMap == NULL) {
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
std::string utf16_to_utf8(const wchar_t* utf16);
std::wstring utf8_to_utf16(const char* utf8);
inline std::wstring utf8_to_utf16(const char8_t* utf8) {
    return utf8_to_utf16((const char*)utf8);
}
std::string utf16_to_mb(const wchar_t* utf16);
std::wstring mb_to_utf16(const char* utf8);
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

#pragma region Gui Wrapper

enum game_gui_impl {
    IMPL_WIN32_DX8,
    IMPL_WIN32_DX9
};

void GameGuiInit(game_gui_impl impl, int device, int hwnd, int wndproc_addr,
    Gui::ingame_input_gen_t input_gen, int reg1, int reg2, int reg3 = 0,
    int wnd_size_flag = -1, float x = 640.0f, float y = 480.0f);
void GameGuiBegin(game_gui_impl impl, bool game_nav = true);
void GameGuiEnd(bool draw_cursor = false);
void GameGuiRender(game_gui_impl impl);
void GameToggleIME(bool toggle);
void TryKeepUpRefreshRate(void* address);
void TryKeepUpRefreshRate(void* address, void* address2);

#pragma endregion

#pragma region Advanced Options Menu

struct RecordedValue {
    enum Type {
        TYPE_INT,
        TYPE_FLOAT,
        TYPE_DOUBLE,
        TYPE_INT64,
    };

    Type type;
    std::string name;
    union {
        int i;
        float f;
        double d;
        int64_t i64;
    } value;
    std::string format;

    RecordedValue(std::string n, int i, const char* fmt = "%d")
    {
        type = TYPE_INT;
        name = n;
        value.i = i;
        format = fmt;
    }
    RecordedValue(std::string n, float f, const char* fmt = "%f")
    {
        type = TYPE_FLOAT;
        name = n;
        value.f = f;
        format = fmt;
    }
    RecordedValue(std::string n, double d, const char* fmt = "%lf")
    {
        type = TYPE_DOUBLE;
        name = n;
        value.d = d;
        format = fmt;
    }
    RecordedValue(std::string n, int64_t i64, const char* fmt = "%ll")
    {
        type = TYPE_INT64;
        name = n;
        value.i64 = i64;
        format = fmt;
    }
};

struct adv_opt_ctx {
    int fps_status = 0;
    int fps = 60;
    double fps_dbl = 1.0 / 60.0;
    int fps_replay_slow = 0;
    int fps_replay_fast = 0;
    int fps_debug_acc = 0;
    int32_t vpatch_base = 0;

    bool data_rec_toggle = false;
    std::function<void(std::vector<RecordedValue>&)> data_rec_func;
    std::wstring data_rec_dir;

    bool all_clear_bonus = false;
};

void CenteredText(const char* text, float wndX);
float GetRelWidth(float rel);
float GetRelHeight(float rel);
void CalcFileHash(const wchar_t* file_name, uint64_t hash[2]);
void HelpMarker(const char* desc);
template <th_glossary_t name>
static bool BeginOptGroup()
{
    static bool group_status = true;
    ImGui::SetNextItemOpen(group_status);
    group_status = ImGui::CollapsingHeader(XSTR(name), ImGuiTreeNodeFlags_None);
    if (group_status)
        ImGui::Indent();
    return group_status;
}
static void EndOptGroup()
{
    ImGui::Unindent();
}
typedef void __stdcall FPSHelperCallback(int32_t);
int FPSHelper(adv_opt_ctx& ctx, bool repStatus, bool vpFast, bool vpSlow, FPSHelperCallback* callback);
bool GameFPSOpt(adv_opt_ctx& ctx, bool replay = true);
bool GameplayOpt(adv_opt_ctx& ctx);
void AboutOpt(const char* thanks_text = nullptr);

#pragma endregion

#pragma region Game BGM

template <
    uintptr_t play_addr,
    uintptr_t stop_addr,
    uintptr_t pause_addr,
    uintptr_t resume_addr,
    uintptr_t caller_addr>
static bool ElBgmTest(bool hotkey_status, bool practice_status,
    uintptr_t retn_addr, int32_t bgm_cmd, int32_t bgm_param, uintptr_t caller)
{
    static bool mElStatus { false };
    static int mLockBgmId { -1 };

    bool hotkey = hotkey_status;
    bool is_practice = practice_status;

    switch (retn_addr) {
    case play_addr:
        if (caller == caller_addr) {
            if (mLockBgmId == -1)
                mLockBgmId = bgm_param;
            if (mLockBgmId != bgm_param) {
                mLockBgmId = -1;
                mElStatus = 0;
            } else if (!mElStatus && hotkey) {
                mElStatus = 1;
                return false;
            }
        }
        if (mLockBgmId >= 0 && mLockBgmId != bgm_param) {
            mLockBgmId = -1;
            mElStatus = 0;
        }
        break;
    case stop_addr:
        if (mLockBgmId >= 0) {
            mLockBgmId = -1;
            // Quitting or disabled
            if (!is_practice || !hotkey)
                mElStatus = 0;
        }
        break;
    case pause_addr:
        if (mLockBgmId >= 0) {
            if (hotkey)
                mElStatus = 1;
            else
                mElStatus = 0;
        }
        break;
    case resume_addr:
        if (mLockBgmId >= 0) {
            if (!mElStatus && hotkey) {
                mElStatus = 1;
                return false;
            }
        }
        break;
    default:
        break;
    }

    return mElStatus;
}

template <
    uintptr_t play_addr,
    uintptr_t play_addr_2,
    uintptr_t stop_addr,
    uintptr_t pause_addr,
    uintptr_t resume_addr,
    uintptr_t caller_addr>
static bool ElBgmTestTemp(bool hotkey_status, bool practice_status,
    uintptr_t retn_addr, int32_t bgm_cmd, int32_t bgm_param, uintptr_t caller)
{
    static bool mElStatus { false };
    static int mLockBgmId { -1 };

    bool hotkey = hotkey_status;
    bool is_practice = practice_status;

    switch (retn_addr) {
    case play_addr:
    case play_addr_2:
        if (caller == caller_addr) {
            if (mLockBgmId == -1)
                mLockBgmId = bgm_param;
            if (mLockBgmId != bgm_param) {
                mLockBgmId = -1;
                mElStatus = 0;
            } else if (!mElStatus && hotkey) {
                mElStatus = 1;
                return false;
            }
        }
        if (mLockBgmId >= 0 && mLockBgmId != bgm_param) {
            mLockBgmId = -1;
            mElStatus = 0;
        }
        break;
    case stop_addr:
        if (mLockBgmId >= 0) {
            mLockBgmId = -1;
            // Quitting or disabled
            if (!is_practice || !hotkey)
                mElStatus = 0;
        }
        break;
    case pause_addr:
        if (mLockBgmId >= 0) {
            if (hotkey)
                mElStatus = 1;
            else
                mElStatus = 0;
        }
        break;
    case resume_addr:
        if (mLockBgmId >= 0) {
            if (!mElStatus && hotkey) {
                mElStatus = 1;
                return false;
            }
        }
        break;
    default:
        break;
    }

    return mElStatus;
}

#pragma endregion

#pragma region Replay System

typedef void*(__cdecl* p_malloc)(size_t size);

bool ReplaySaveParam(const wchar_t* rep_path, const std::string& param);
bool ReplayLoadParam(const wchar_t* rep_path, std::string& param);

#pragma endregion

#pragma region Json

#define ParseJson()                                \
    Reset();                                       \
    rapidjson::Document param;                     \
    if (param.Parse(json.c_str()).HasParseError()) \
        return false;
#define ParseJsonNoReset()                         \
    rapidjson::Document param;                     \
    if (param.Parse(json.c_str()).HasParseError()) \
        return false;
#define CreateJson()           \
    rapidjson::Document param; \
    param.SetObject();         \
    auto& jalloc = param.GetAllocator();
#define ReturnJson()                                       \
    rapidjson::StringBuffer sb;                            \
    rapidjson::Writer<rapidjson::StringBuffer> writer(sb); \
    param.Accept(writer);                                  \
    return sb.GetString();
#define ForceJsonValue(value_name, comparator)                             \
    if (!param.HasMember(#value_name) || param[#value_name] != comparator) \
        return false;
#define GetJsonValue(value_name)                                       \
    if (param.HasMember(#value_name) && param[#value_name].IsNumber()) \
        value_name = (decltype(value_name))param[#value_name].GetDouble();
#define GetJsonValueEx(value_name, type)                               \
    if (param.HasMember(#value_name) && param[#value_name].Is##type()) \
        value_name = (decltype(value_name))param[#value_name].Get##type();
#define GetJsonValueAlt(value_name, valueVar, type)                    \
    if (param.HasMember(#value_name) && param[#value_name].Is##type()) \
        valueVar = param[#value_name].Get##type();
#define AddJsonValue(value_name)                                           \
    {                                                                      \
        rapidjson::Value __key_##value_name(#value_name, jalloc);          \
        rapidjson::Value __value_##value_name(value_name);                 \
        param.AddMember(__key_##value_name, __value_##value_name, jalloc); \
    }
#define AddJsonValueEx(value_name, ...)                                    \
    {                                                                      \
        rapidjson::Value __key_##value_name(#value_name, jalloc);          \
        rapidjson::Value __value_##value_name(__VA_ARGS__);                \
        param.AddMember(__key_##value_name, __value_##value_name, jalloc); \
    }

#pragma endregion

#pragma region Virtual File System
class VFile {
public:
    VFile() = default;

    void SetFile(void* buffer, size_t size)
    {
        mBuffer = (uint8_t*)buffer;
        mSize = size;
        mPos = 0;
    }
    void SetPos(size_t pos)
    {
        if (pos >= mSize)
            return;
        mPos = pos;
    }
    size_t GetPos()
    {
        return mPos;
    }

    void Write(const char* data);
    void Write(void* data, unsigned int length);
    void Read(void* buffer, unsigned int length);

    template <typename T>
    VFile& operator<<(T data)
    {
        *(T*)(mBuffer + mPos) = data;
        mPos += sizeof(T);
        return *this;
    }
    template <typename T>
    VFile& operator>>(T& data)
    {
        Read(&data, sizeof(T));
        return *this;
    }
    template <typename T>
    VFile& operator<<(std::pair<size_t, T> data)
    {
        SetPos(data.first);
        operator<<(data.second);
        return *this;
    }
    template <typename T>
    VFile& operator<<(std::pair<int, T> data)
    {
        SetPos(data.first);
        operator<<(data.second);
        return *this;
    }
    template <>
    VFile& operator<<<const char*>(const char* data)
    {
        Write(data);
        return *this;
    }

private:
    uint8_t* mBuffer = nullptr;
    size_t mSize = 0;
    size_t mPos = 0;
};

// Typedef
enum VFS_TYPE {
    VFS_TH11,
    VFS_TH08,
    VFS_TH07,
    VFS_TH06,
};
typedef void* (*vfs_listener)(const char* file_name, int32_t* file_size, int unk, void* buffer);

// API
void VFSHook(VFS_TYPE type, void* addr);
void VFSAddListener(const char* file_name, vfs_listener onCall, vfs_listener onLoad);
void* VFSOriginal(const char* file_name, int32_t* file_size, int32_t is_file);

#pragma endregion

#pragma region Memory Helper

template <class... Args>
inline uint32_t GetMemContent(int addr, int offset, Args... rest)
{
    return GetMemContent((int)(*((uint32_t*)addr) + (uint32_t)offset), rest...);
}
inline uint32_t GetMemContent(int addr, int offset)
{
    return *((uint32_t*)(*((uint32_t*)addr) + (uint32_t)offset));
}
inline uint32_t GetMemContent(int addr)
{
    return *((uint32_t*)addr);
}

template <class... Args>
inline uint32_t GetMemAddr(int addr, int offset, Args... rest)
{
    return GetMemAddr((int)(*((uint32_t*)addr) + (uint32_t)offset), rest...);
}
inline uint32_t GetMemAddr(int addr, int offset)
{
    return (*((uint32_t*)addr) + (uint32_t)offset);
}
inline uint32_t GetMemAddr(int addr)
{
    return (uint32_t)addr;
}

#define MDARRAY(arr, idx, size_of_subarray) (arr + idx * size_of_subarray)

#pragma endregion

#pragma region ECL Helper

class ECLHelper : public VFile {
public:
    ECLHelper() = default;

    void SetBaseAddr(void* addr)
    {
        mPtrToBuffer = (uint8_t*)addr;
        VFile::SetFile((uint8_t*)(*(uint32_t*)addr), 0x99999);
    }
    void SetFile(unsigned int ordinal)
    {
        VFile::SetFile((uint8_t*)(*(uint32_t*)(mPtrToBuffer + ordinal * 4)), 0x99999);
    }

private:
    uint8_t* mPtrToBuffer = nullptr;
};

template <typename T>
inline std::pair<size_t, T> ECLX(size_t pos, T data)
{
    return std::make_pair(pos, data);
}

typedef void (*ecl_patch_func)(ECLHelper& ecl);
#define ECLPatch(name, ...) void name(ECLHelper& ecl, __VA_ARGS__)

#pragma endregion

#pragma region Quick Config

typedef void __stdcall QuickCfgHotkeyCallback(std::string&, bool);
int QuickCfg(const char* game, QuickCfgHotkeyCallback* callback);
bool QuickCfgHintText(bool reset = false);

#pragma endregion

#pragma region Rounding

/** round n down to nearest multiple of m */
inline long RoundDown(long n, long m)
{
    return n >= 0 ? (n / m) * m : ((n - m + 1) / m) * m;
}

/** round n up to nearest multiple of m */
inline long RoundUp(long n, long m)
{
    return n >= 0 ? ((n + m - 1) / m) * m : (n / m) * m;
}

#pragma endregion

#pragma region Snapshot
namespace THSnapshot {
    void* GetSnapshotData(IDirect3DDevice8* d3d8);
    void Snapshot(IDirect3DDevice8* d3d8);
}
#pragma endregion

#pragma region ECL Warp
struct ecl_write_t {
    uint32_t off;
    std::vector<uint8_t> bytes;
    void apply(uint8_t* start)
    {
        for (unsigned int i = 0; i < bytes.size(); i++) {
            start[off + i] = bytes[i];
        }
    }
};

struct ecl_jump_t {
    uint32_t off;
    uint32_t dest;
    uint32_t at_frame;
    uint32_t ecl_time;
};

struct section_param_t;

struct stage_warps_t {
    const char* label;
    enum {
        TYPE_NONE,
        TYPE_SLIDER,
        TYPE_COMBO
    } type;
    std::vector<section_param_t> section_param;
};

struct section_param_t {
    const char* label;
    std::unordered_map<std::string, std::vector<ecl_jump_t>> jumps;
    std::unordered_map<std::string, std::vector<ecl_write_t>> writes;
    std::optional<stage_warps_t> phases;
};

typedef uint8_t* ecl_get_sub_t(const char* name, uintptr_t user_param);

uint8_t* ThModern_ECLGetSub(const char* name, uintptr_t param);

void StageWarpsRender(stage_warps_t& warps, std::vector<unsigned int>& out_warp, size_t level);
void StageWarpsApply(stage_warps_t& warps, std::vector<unsigned int>& in_warp, ecl_get_sub_t* ecl_get_sub, uintptr_t ecl_get_sub_param, size_t level);
#pragma endregion

template <typename T>
static std::function<T(void)> GetRndGenerator(T min, T max, std::mt19937::result_type seed = 0)
{
    // std::mt19937::result_type seed = time(0);
    if (!seed) {
        seed = (std::mt19937::result_type)time(0);
        // std::random_device rd;
        // seed = rd();
    }
    auto dice_rand = std::bind(std::uniform_int_distribution<T>(min, max), std::mt19937(seed));
    return dice_rand;
}
DWORD WINAPI CheckDLLFunction(const wchar_t* path, const char* funcName);
}
