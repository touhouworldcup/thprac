#include "thprac_hook.h"

#include <unordered_map>

namespace THPrac {
// If macros could define other macros, I would've used a macro
char __vehHooks[sizeof(std::unordered_map<uintptr_t, HookCtx*>)];
#define vehHooks (*(std::unordered_map<uintptr_t, HookCtx*>*)__vehHooks)

LPVOID hook_page = nullptr;
uintptr_t hook_page_offset;

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
    if (!this->data.buffer.ptr) {
        MessageBoxA(NULL, "No codecave or patch buffer for hook", this->name, MB_ICONERROR);
        __debugbreak();
    } else if (!this->setup) {
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

static inline void cave_fix(uint8_t* sourcecave, uint8_t* bp_addr, uint32_t sourcecave_size)
{
    /// Return Jump
    /// ------------------
    const uint32_t cave_dist = bp_addr - (sourcecave + 5);
    sourcecave[sourcecave_size] = 0xE9;
    *(uint32_t*)&sourcecave[sourcecave_size + 1] = cave_dist;
    /// Fix relative stuff
    /// ------------------

    // #1: Relative near call / jump at the very beginning
    if (sourcecave[0] == 0xE8 || sourcecave[0] == 0xE9) {
        uint32_t offset_old = *(uint32_t*)(sourcecave + 1);
        uint32_t offset_new = offset_old + bp_addr - sourcecave;

        *(uint32_t*)(sourcecave + 1) = offset_new;
    }
    /// ------------------
}

void* MakeCodecave(uint8_t* code, uintptr_t addr, size_t insLen)
{
    if (!addr) {
        addr = (uintptr_t)code;
    }

    memcpy((uint8_t*)hook_page + hook_page_offset, code, insLen);
    cave_fix((uint8_t*)hook_page + hook_page_offset, code, insLen);

    return (uint8_t*)hook_page + hook_page_offset;
}


void HookCtx::Setup()
{
    if (!this->setup) {
        this->setup = true;
        this->addr += ingame_image_base;
        if (this->callback) {
            vehHooks[this->addr] = this;

            this->data.hook.codecave = MakeCodecave((uint8_t*)this->addr, this->addr, this->data.hook.instr_len);
            hook_page_offset += this->data.hook.instr_len + 5;

            this->data.hook.orig_byte = *(uint8_t*)this->addr;
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
    auto hook = vehHooks.find(ExceptionInfo->ContextRecord->Eip);
    if (hook == vehHooks.end()) {
        return EXCEPTION_CONTINUE_SEARCH;
    }

    auto EipBak = ExceptionInfo->ContextRecord->Eip;
    hook->second->callback(ExceptionInfo->ContextRecord, hook->second);

    if (ExceptionInfo->ContextRecord->Eip == EipBak) {
        ExceptionInfo->ContextRecord->Eip = (DWORD)hook->second->data.hook.codecave;
    }
    return EXCEPTION_CONTINUE_EXECUTION;
}

void VEHHookInit()
{
    new (__vehHooks) std::unordered_map<uintptr_t, HookCtx*>;

    hook_page = VirtualAlloc(nullptr, 1, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

    AddVectoredExceptionHandler(1, &VEHHandler);
}
}