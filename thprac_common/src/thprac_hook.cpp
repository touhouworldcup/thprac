#include "thprac_hook.h"

#include <unordered_map>


// If macros could define other macros, I would've used a macro
char __vehHooks[sizeof(std::unordered_map<uintptr_t, HookCtx*>)];
#define vehHooks (*(std::unordered_map<uintptr_t, HookCtx*>*)__vehHooks)

LPVOID hook_page = nullptr;
uintptr_t hook_page_offset = 0;
uintptr_t ingame_image_base = 0;

void HookCtx::PatchSwap()
{
    DWORD oldProt;
    VirtualProtect((LPVOID)this->addr, this->data.buffer.size, PAGE_EXECUTE_READWRITE, &oldProt);
    memswap((void*)this->addr, this->data.buffer.ptr, this->data.buffer.size);
    VirtualProtect((LPVOID)this->addr, this->data.buffer.size, oldProt, &oldProt);
}

void HookCtx::Enable()
{
#ifdef TH_X86
    if (!this->data.buffer.ptr) {
        MessageBoxA(NULL, "No codecave or patch buffer for hook", this->name, MB_ICONERROR);
        __debugbreak();
    } else
#endif
    if (!this->setup) {
        MessageBoxA(NULL, "Hook not setup. Note that patches work without setup, unless the addresses used are RVAs", this->name, MB_ICONWARNING);
    }

    if (this->enabled) {
        return;
    }

    if (callback) {
        DWORD oldProt;
        VirtualProtect((LPVOID)this->addr, 1, PAGE_EXECUTE_READWRITE, &oldProt);
        *(uint8_t*)this->addr = 0xCC;
        VirtualProtect((LPVOID)this->addr, 1, oldProt, &oldProt);
    } else {
        this->PatchSwap();
    }
    this->enabled = true;
}

void HookCtx::Disable()
{
    if (!this->enabled) {
        return;
    }
    if (callback) {
        DWORD oldProt;
        VirtualProtect((LPVOID)this->addr, 1, PAGE_EXECUTE_READWRITE, &oldProt);
        *(uint8_t*)this->addr = this->data.hook.orig_byte;
        VirtualProtect((LPVOID)this->addr, 1, oldProt, &oldProt);
    } else {
        this->PatchSwap();
    }
    this->enabled = false;
}

#ifdef TH_X86
static void* MakeCodecave32(uint8_t* code, uintptr_t addr, size_t insLen) {
    if (!addr) {
        addr = (uintptr_t)code;
    }

    uint8_t* sourcecave = (uint8_t*)hook_page + hook_page_offset;

    memcpy(sourcecave, code, insLen);
    /// Return Jump
    /// ------------------
    const uint32_t cave_dist = code - (sourcecave + 5);
    sourcecave[insLen] = 0xE9;
    *(uint32_t*)&sourcecave[insLen + 1] = cave_dist;

    /// Fix relative stuff
    /// ------------------

    // #1: Relative near call / jump at the very beginning
    if (sourcecave[0] == 0xE8 || sourcecave[0] == 0xE9) {
        uint32_t offset_old = *(uint32_t*)(sourcecave + 1);
        uint32_t offset_new = offset_old + code - sourcecave;

        *(uint32_t*)(sourcecave + 1) = offset_new;
    }
    /// ------------------

    hook_page_offset += RoundUp(insLen + 5, 16);
    return sourcecave;
}
#endif

void HookCtx::Setup()
{
    if (!this->setup) {
        this->setup = true;
        this->addr += ingame_image_base;
        if (this->callback) {
            vehHooks[this->addr] = this;
            this->data.hook.orig_byte = *(uint8_t*)this->addr;
#ifdef TH_X86
            this->data.hook.codecave = MakeCodecave32((uint8_t*)this->addr, this->addr, this->data.hook.instr_len);
#endif
        }
    }
}

void EnableAllHooksImpl(HookCtx* hooks, size_t num)
{
    for (size_t i = 0; i < num; i++) {
        hooks[i].Setup();
        hooks[i].Enable();
    }
}

void DisableAllHooksImpl(HookCtx* hooks, size_t num)
{
    for (size_t i = 0; i < num; i++) {
        hooks[i].Disable();
    }
}

LONG NTAPI VEHHandler(EXCEPTION_POINTERS* ExceptionInfo)
{
    if (ExceptionInfo->ExceptionRecord->ExceptionCode != EXCEPTION_BREAKPOINT) {
        return EXCEPTION_CONTINUE_SEARCH;
    }
    auto hook = vehHooks.find(ExceptionInfo->ContextRecord->Xip);
    if (hook == vehHooks.end()) {
        return EXCEPTION_CONTINUE_SEARCH;
    }

    //MessageBoxW(0, 0, 0, 0);
    auto XipBak = ExceptionInfo->ContextRecord->Xip;
    hook->second->callback(ExceptionInfo->ContextRecord, hook->second);

    if (ExceptionInfo->ContextRecord->Xip == XipBak) {
        if (hook->second->data.hook.codecave) {
            ExceptionInfo->ContextRecord->Xip = (UINT_PTR)hook->second->data.hook.codecave;
        }
        else {
            ExceptionInfo->ContextRecord->Xip += hook->second->data.hook.instr_len;
        }
    }
    return EXCEPTION_CONTINUE_EXECUTION;
}

void VEHHookInit()
{
#if TH_X86
    hook_page = VirtualAlloc(nullptr, 1, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
#endif
    new (__vehHooks) std::unordered_map<uintptr_t, HookCtx*>;
    AddVectoredExceptionHandler(1, &VEHHandler);
}