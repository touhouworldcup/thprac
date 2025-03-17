﻿#pragma once

#include "thprac_gui_components.h"
#include "thprac_igi_key_render.h"

#include <vector>
#include <unordered_map>
#include <optional>
#define BOSS_MOVE_DOWN_RANGE_INIT 0.5

struct IDirect3DDevice8;

namespace THPrac {
    struct AdvancedIGI_Options
    {
        int th18_cards[6] = {0}; // card for st1,2,3,4,5,7
        bool th18_force_card = false;
        bool disable_master_autoly = false;
        bool map_inf_life_to_no_continue = true;
        bool th06_bg_fix = false;
        bool th06_fix_seed = false;
        int32_t th06_seed = 0; // to match imgui
        bool th06_disable_drop_rank = false;
        bool th06_showRank = false;
        bool th06_show_hitbox = false;
        bool th06_showRepMarker = false;
        bool th06_autoname = false;
        char th06_autoname_name[12] = {0};
        bool th11_showHint = false;
        bool th13_showHits = false;
        bool th13_showHitBar = false;
        bool th14_showBonus = false;
        bool th14_showItemsCount = false;
        bool th14_showDropBar = false;
        bool th15_showShootingDownRate = false;
        bool show_keyboard_monitor = false;
        KeyRectStyle keyboard_style;
    };
    extern AdvancedIGI_Options g_adv_igi_options;

struct Float2 {
    float x;
    float y;
};
    
struct Float3 {
    float x;
    float y;
    float z;
};

struct Timer {
    int32_t previous;
    int32_t current;
    float current_f;
    float* __game_speed__disused;
    uint32_t control;
};

struct Timer19 {
    int32_t previous;
    int32_t current;
    float current_f;
    uint32_t scale_table_index;
    uint32_t flags;
};

template<typename T>
struct ThList {
    T* entry;
    ThList<T>* next;
    ThList<T>* prev;
    ThList<T>* __seldom_used;
};

#pragma region Gui Wrapper

enum game_gui_impl {
    IMPL_WIN32_DX8,
    IMPL_WIN32_DX9
};

void GameGuiInit(game_gui_impl impl, int device, int hwnd, int wndproc_addr,
    Gui::ingame_input_gen_t input_gen, int reg1, int reg2, int reg3 = 0,
    int wnd_size_flag = -1, float x = 640.0f, float y = 480.0f);
extern int GameGuiProgress;
void GameGuiBegin(game_gui_impl impl, bool game_nav = true);
void GameGuiEnd(bool draw_cursor = false);
void GameGuiRender(game_gui_impl impl);
#pragma endregion

#pragma region Advanced Options Menu

struct adv_opt_ctx {
    int fps_status = 0;
    int fps = 60;
    double fps_dbl = 1.0 / 60.0;
    int fps_replay_slow = 0;
    int fps_replay_fast = 0;
    int fps_debug_acc = 0;
    int32_t vpatch_base = 0;

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
    group_status = ImGui::CollapsingHeader(Gui::LocaleGetStr(name), ImGuiTreeNodeFlags_None);
    if (group_status)
        ImGui::Indent();
    return group_status;
}
inline void EndOptGroup()
{
    ImGui::Unindent();
}
typedef void __stdcall FPSHelperCallback(int32_t);
int FPSHelper(adv_opt_ctx& ctx, bool repStatus, bool vpFast, bool vpSlow, FPSHelperCallback* callback);
bool GameFPSOpt(adv_opt_ctx& ctx, bool replay = true);
bool GameplayOpt(adv_opt_ctx& ctx);
void AboutOpt(const char* thanks_text = nullptr);
void InfLifeOpt();
void DisableKeyOpt();
void KeyHUDOpt();

#pragma endregion

#pragma region Game BGM

// TODO: Should bgm_cmd be removed?
template <
    uintptr_t play_addr,
    uintptr_t stop_addr,
    uintptr_t pause_addr,
    uintptr_t resume_addr,
    uintptr_t caller_addr>
static bool ElBgmTest(bool hotkey_status, bool practice_status,
    uintptr_t retn_addr, [[maybe_unused]] int32_t bgm_cmd, int32_t bgm_param, uintptr_t caller)
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

// TODO: Should bgm_cmd be removed?
template <
    uintptr_t play_addr,
    uintptr_t play_addr_2,
    uintptr_t stop_addr,
    uintptr_t pause_addr,
    uintptr_t resume_addr,
    uintptr_t caller_addr>
static bool ElBgmTestTemp(bool hotkey_status, bool practice_status,
    uintptr_t retn_addr, [[maybe_unused]] int32_t bgm_cmd, int32_t bgm_param, uintptr_t caller)
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
    VFile& operator<< <const char*>(const char* data)
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

template <typename R = size_t>
inline R GetMemContent(uintptr_t addr)
{
    return *(R*)addr;
}
template <typename R = size_t, typename... OffsetArgs>
inline R GetMemContent(uintptr_t addr, size_t offset, OffsetArgs... remaining_offsets)
{
    return GetMemContent<R>(((uintptr_t) * (R*)addr) + offset, remaining_offsets...);
}

template <typename R = uintptr_t>
inline R GetMemAddr(uintptr_t addr)
{
    return (R)addr;
}
template <typename R = uintptr_t, typename... OffsetArgs>
inline R GetMemAddr(uintptr_t addr, size_t offset, OffsetArgs... remaining_offsets)
{
    return GetMemAddr<R>(((uintptr_t) * (R*)addr) + offset, remaining_offsets...);
}

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
    uint32_t off = 0;
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

#pragma region Game State
// GameState_Assert:
// ---
// Check if the game's own state is in order. Returns true if it is.
// Otherwise it will display a message box. If the user clicks "No",
// the function will terminate the current process. If the user
// clicks "Yes", the function will return false to signal to the caller
// that the assertion failed, but that the user wishes to proceed anyways
bool GameState_Assert(bool cond);

#pragma endregion
}