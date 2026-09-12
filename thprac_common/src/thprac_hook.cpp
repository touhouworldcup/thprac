#include "thprac_hook.h"

#include <unordered_map>


// If macros could define other macros, I would've used a macro
char __vehHooks[sizeof(std::unordered_map<uintptr_t, HookCtx*>)];
#define vehHooks (*(std::unordered_map<uintptr_t, HookCtx*>*)__vehHooks)

#define CALL_REL_OP_LEN 1
#define CALL_ABSPTR_OP_LEN 3
#define CALL_REL_LEN (CALL_REL_OP_LEN + sizeof(int32_t))
#define CALL_ABSPTR_LEN (CALL_ABSPTR_OP_LEN + sizeof(int32_t))
#if TH_X86
#define CALL_OP_LEN CALL_REL_OP_LEN
#define CALL_LEN    CALL_REL_LEN
#define MOV_PTR_LEN 1
#else
#define CALL_OP_LEN CALL_ABSPTR_OP_LEN
#define CALL_LEN    CALL_ABSPTR_LEN
#define MOV_PTR_LEN 2
#endif
#define x86_CALL_NEAR_REL32 0xE8
#define x86_JMP_NEAR_REL32 0xE9
#define x86_CALL_NEAR_ABSPTR TextInt(0xFF, 0x14, 0x25)
#define x86_JMP_NEAR_ABSPTR TextInt(0xFF, 0x24, 0x25)
#define x86_NOP 0x90
#define x86_INT3 0xCC

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

static inline void TH_FASTCALL cave_fix(uint8_t* sourcecave, uint8_t* bp_addr, uint32_t sourcecave_size)
{
    /// Return Jump
    /// ------------------
#if TH_X86
    const uint32_t cave_dist = bp_addr - (sourcecave + CALL_LEN);
    sourcecave[sourcecave_size] = x86_JMP_NEAR_REL32;
    *(uint32_t*)&sourcecave[sourcecave_size + CALL_OP_LEN] = cave_dist;
#else
    *(uint32_t*)&sourcecave[sourcecave_size] = x86_JMP_NEAR_ABSPTR;
#endif

    /// Fix relative stuff
    /// ------------------

    // #1: Relative near call / jump at the very beginning
    if (sourcecave[0] == x86_CALL_NEAR_REL32 || sourcecave[0] == x86_JMP_NEAR_REL32) {
#if TH_X86
        uint32_t offset_old = *(uint32_t*)(sourcecave + CALL_REL_OP_LEN);
        uint32_t offset_new = offset_old + bp_addr - sourcecave;

        *(uint32_t*)(sourcecave + CALL_REL_OP_LEN) = offset_new;

        //log_printf("fixing rel offset 0x%X to 0x%X... \n", offset_old, offset_new);
#else
        int32_t offset_old = *(int32_t*)(sourcecave + CALL_REL_OP_LEN);
        int32_t offset_new = (int32_t)(sourcecave_size - CALL_REL_OP_LEN + CALL_LEN);

        *(int32_t*)(sourcecave + CALL_REL_OP_LEN) = offset_new;

        *(uint32_t*)&sourcecave[(size_t)sourcecave_size + CALL_LEN] = x86_JMP_NEAR_ABSPTR;
        //log_printf("fixing rel offset 0x%X... \n", offset_old);
#endif
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
    auto hook = vehHooks.find(ExceptionInfo->ContextRecord->Xip);
    if (hook == vehHooks.end()) {
        return EXCEPTION_CONTINUE_SEARCH;
    }

    auto XipBak = ExceptionInfo->ContextRecord->Xip;
    hook->second->callback(ExceptionInfo->ContextRecord, hook->second);

    if (ExceptionInfo->ContextRecord->Xip == XipBak) {
        ExceptionInfo->ContextRecord->Xip = (UINT_PTR)hook->second->data.hook.codecave;
    }
    return EXCEPTION_CONTINUE_EXECUTION;
}

void VEHHookInit()
{
    new (__vehHooks) std::unordered_map<uintptr_t, HookCtx*>;

    hook_page = VirtualAlloc(nullptr, 1, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

    AddVectoredExceptionHandler(1, &VEHHandler);
}