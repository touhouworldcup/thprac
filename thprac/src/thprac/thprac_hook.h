#pragma once
#define NOMINMAX
#include <Windows.h>
#include <memory>
#include <vector>

namespace THPrac {
typedef void __stdcall CallbackFunc(PCONTEXT);
class HookCtx {
public:
    HookCtx() = default;
    ~HookCtx();
    HookCtx(uintptr_t target, CallbackFunc* inject)
        : mTarget((void*)target)
        , mDetour(inject)
    {
    }
    HookCtx(uintptr_t target, const char* patch, size_t size)
        : mTarget((void*)target)
        , mPatch(patch)
        , mPatchSize(size)
        , mIsPatch(true)
    {
    }
    HookCtx(std::vector<HookCtx*>& vec, uintptr_t target, CallbackFunc* inject)
        : mTarget((void*)target)
        , mDetour(inject)
    {
        vec.push_back(this);
    }
    HookCtx(std::vector<HookCtx*>& vec, uintptr_t target, const char* patch, size_t size)
        : mTarget((void*)target)
        , mPatch(patch)
        , mPatchSize(size)
        , mIsPatch(true)
    {
        vec.push_back(this);
    }

    bool Setup(void* target, CallbackFunc* detour);
    bool Setup(void* target, const char* patch, size_t patch_size);
    bool Setup();
    bool Reset();

    bool Enable();
    bool Disable();
    bool Toggle(bool status);

    static void VEHInit();

private:
    void* mTarget = nullptr;
    CallbackFunc* mDetour = nullptr;
    void* mTrampoline = nullptr;
    const char* mPatch = nullptr;
    size_t mPatchSize = 0;
    bool mIsPatch = false;
    bool mIsHookReady = false;
    bool mIsHookEnabled = false;
};

template <uintptr_t target, CallbackFunc* inject>
class EHookSingleton {
    EHookSingleton(const EHookSingleton&) = delete;
    EHookSingleton& operator=(EHookSingleton&) = delete;
    EHookSingleton(EHookSingleton&&) = delete;
    EHookSingleton& operator=(EHookSingleton&&) = delete;
public:
    __declspec(noinline) static auto& GetHook()
    {
        static HookCtx* hook_singleton = nullptr;
        if (!hook_singleton) {
            hook_singleton = new HookCtx(target, inject);
            hook_singleton->Setup();
        }
        return *hook_singleton;
    }
    __declspec(noinline) static auto& GetData()
    {
        static int data = 0;
        return data;
    }
};

template <uintptr_t target, const char* patch, size_t size>
class PatchSingleton {
    PatchSingleton(const PatchSingleton&) = delete;
    PatchSingleton& operator=(PatchSingleton&) = delete;
    PatchSingleton(PatchSingleton&&) = delete;
    PatchSingleton& operator=(PatchSingleton&&) = delete;

public:
    __declspec(noinline) static auto& GetPatch()
    {
        static HookCtx* hook_singleton = nullptr;
        if (!hook_singleton) {
            hook_singleton = new HookCtx(target, patch, size);
            hook_singleton->Setup();
        }
        return *hook_singleton;
    }
};

struct HookSetBase {
public:
    void EnableAllHooks()
    {
        for (auto& hookCtxDyn : mHooks) {
            hookCtxDyn->Setup();
            hookCtxDyn->Enable();
        }
    }
    void DisableAllHooks()
    {
        for (auto& hookCtxDyn : mHooks)
            hookCtxDyn->Disable();
    }
    void ToggleAllHooks(bool status)
    {
        if (status)
            EnableAllHooks();
        else
            DisableAllHooks();
    }


protected:
    std::vector<HookCtx*> mHooks;
};

enum CallType {
    Cdecl, // cdecl and CDECL are both predefined. :P
    Stdcall,
    Fastcall,
    Thiscall,
    Vectorcall
};
template <uintptr_t addr, CallType type, typename R = void, typename... Args>
static inline R asm_call(Args... args) {
    if constexpr (type == Cdecl) {
        auto* func = (R(__cdecl*)(Args...))addr;
        return func(args...);
    }
    else if constexpr (type == Stdcall) {
        auto* func = (R(__stdcall*)(Args...))addr;
        return func(args...);
    }
    else if constexpr (type == Fastcall) {
        auto* func = (R(__fastcall*)(Args...))addr;
        return func(args...);
    }
    else if constexpr (type == Vectorcall) {
        auto* func = (R(__vectorcall*)(Args...))addr;
        return func(args...);
    }
    else if constexpr (type == Thiscall) {
        auto* func = (R(__thiscall*)(Args...))addr;
        return func(args...);
    }
}

inline void PushHelper32(CONTEXT* pCtx, DWORD value)
{
    pCtx->Esp -= 4;
    *(DWORD*)pCtx->Esp = value;
}
inline DWORD PopHelper32(CONTEXT* pCtx)
{
    // The compiler will optimize this to just use eax
    DWORD ret = *(DWORD*)pCtx->Esp;
    pCtx->Esp += 4;
    return ret;
}

#define EHOOK_G1(name, target) \
    __declspec(noinline) void __stdcall __vehf_##name([[maybe_unused]] PCONTEXT pCtx); \
    typedef EHookSingleton <target, __vehf_##name> name; \
    __declspec(noinline) void __stdcall __vehf_##name([[maybe_unused]] PCONTEXT pCtx)

#define EHOOK_ST(name, target)                                \
    HookCtx name { target, __vehf_##name }; \
    static __declspec(noinline) void __stdcall __vehf_##name([[maybe_unused]] PCONTEXT pCtx)

#define EHOOK_DY(name, target)                                \
    HookCtx name { mHooks, target, __vehf_##name }; \
    static __declspec(noinline) void __stdcall __vehf_##name([[maybe_unused]] PCONTEXT pCtx)

#define PATCH_S1(name, target, patch, size)           \
    static constexpr char __s1patch_##name[] = patch; \
    typedef PatchSingleton<target, __s1patch_##name, size> name;

#define PATCH_ST(name, target, patch, size) \
    HookCtx name { target, patch, size };

#define PATCH_DY(name, target, patch, size) \
    HookCtx name { mHooks, target, patch, size };

#define HOOKSET_DEFINE(name)           \
    struct name : public HookSetBase { \
        name() = default;              \
        SINGLETON(name);

#define HOOKSET_ENDDEF() };
}
