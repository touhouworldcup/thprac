#pragma once
#define NOMINMAX
#include <Windows.h>
#include <memory>
#include <vector>

namespace THPrac {
#define NAKED __declspec(naked)

class HookCtx {
public:
    HookCtx() = default;
    ~HookCtx();
    HookCtx(void* target, void* inject)
        : mTarget(target)
        , mDetour(inject)
    {
    }
    HookCtx(void* target, const char* patch, size_t size)
        : mTarget(target)
        , mPatch(patch)
        , mPatchSize(size)
        , mIsPatch(true)
    {
    }
    HookCtx(std::vector<HookCtx*>& vec, void* target, void* inject)
        : mTarget(target)
        , mDetour(inject)
    {
        vec.push_back(this);
    }
    HookCtx(std::vector<HookCtx*>& vec, void* target, const char* patch, size_t size)
        : mTarget(target)
        , mPatch(patch)
        , mPatchSize(size)
        , mIsPatch(true)
    {
        vec.push_back(this);
    }

    bool Setup(void* target, void* detour);
    bool Setup(void* target, const char* patch, size_t patch_size);
    bool Setup();
    bool Reset();

    bool Enable();
    bool Disable();
    bool Toggle(bool status);

    static void VEHInit();

private:
    void* mTarget = nullptr;
    void* mDetour = nullptr;
    void* mTrampoline = nullptr;
    const char* mPatch = nullptr;
    size_t mPatchSize = 0;
    bool mIsPatch = false;
    bool mIsHookReady = false;
    bool mIsHookEnabled = false;
};

template<void* target, void* inject>
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

template <void* target, const char* patch, size_t size>
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

static void PushHelper32(CONTEXT* pCtx, DWORD value)
{
    pCtx->Esp -= 4;
    *(DWORD*)pCtx->Esp = value;
}
static DWORD PopHelper32(CONTEXT* pCtx)
{
    // The compiler will optimize this to just use eax
    DWORD ret = *(DWORD*)pCtx->Esp;
    pCtx->Esp += 4;
    return ret;
}

#define EHOOK_G1(name, target) \
    __declspec(noinline) void __stdcall __vehf_##name(PCONTEXT pCtx); \
    typedef EHookSingleton <target, (void*)__vehf_##name> name; \
    __declspec(noinline) void __stdcall __vehf_##name(PCONTEXT pCtx)

#define EHOOK_ST(name, target)                                \
    HookCtx name { target, (void*)&__vehf_##name }; \
    static __declspec(noinline) void __stdcall __vehf_##name(PCONTEXT pCtx)

#define EHOOK_DY(name, target)                                \
    HookCtx name { mHooks, target, (void*)&__vehf_##name }; \
    static __declspec(noinline) void __stdcall __vehf_##name(PCONTEXT pCtx)

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


#define ASM \
    __asm
#define ASM_JMP(addr) \
    __asm push addr __asm retn
#define ASM_START() \
    __asm { push ebp __asm mov ebp,esp __asm sub esp,__LOCAL_SIZE __asm pushad }
#define ASM_END() \
    __asm { popad __asm mov esp,ebp __asm pop ebp }
#define ASM_RETN(name) \
    __asm { push eax __asm push eax __asm lea eax,name __asm mov eax,[eax]name.trampoline __asm mov [esp +4], eax __asm pop eax __asm retn }

}
