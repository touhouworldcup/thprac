#include "thprac_hook.h"

#include <unordered_map>

namespace THPrac {

void** FindImportAddress(HANDLE hookModule, LPCSTR moduleName, LPCSTR functionName)
{
    uintptr_t hookModuleBase = (uintptr_t)hookModule;
    PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)hookModuleBase;
    PIMAGE_NT_HEADERS ntHeader = (PIMAGE_NT_HEADERS)(hookModuleBase + dosHeader->e_lfanew);
    PIMAGE_IMPORT_DESCRIPTOR importTable = (PIMAGE_IMPORT_DESCRIPTOR)(hookModuleBase
        + ntHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);
    for (; importTable->Characteristics != 0; importTable++) {
        if (_stricmp((LPCSTR)(hookModuleBase + importTable->Name), moduleName) != 0)
            continue;
        PIMAGE_THUNK_DATA info = (PIMAGE_THUNK_DATA)(hookModuleBase + importTable->OriginalFirstThunk);
        void** iat = (void**)(hookModuleBase + importTable->FirstThunk);
        for (; info->u1.AddressOfData != 0; info++, iat++) {
            if ((info->u1.Ordinal & IMAGE_ORDINAL_FLAG) == 0) {
                PIMAGE_IMPORT_BY_NAME name = (PIMAGE_IMPORT_BY_NAME)(hookModuleBase + info->u1.AddressOfData);
                if (strcmp((LPCSTR)name->Name, functionName) == 0)
                    return iat;
            }
        }
        return NULL;
    }
    return NULL;
}
BOOL HookIAT(HANDLE hookModule, LPCSTR moduleName, LPCSTR functionName, void* hookFunction, void** oldAddress)
{
    void** address = FindImportAddress(hookModule, moduleName, functionName);
    if (address == NULL)
        return FALSE;
    if (oldAddress != NULL)
        *oldAddress = *address;
    DWORD oldProtect, oldProtect2;
    VirtualProtect(address, sizeof(DWORD), PAGE_READWRITE, &oldProtect);
    *address = hookFunction;
    VirtualProtect(address, sizeof(DWORD), oldProtect, &oldProtect2);
    return TRUE;
}

BOOL UnhookIAT(HANDLE hookModule, LPCSTR moduleName, LPCSTR functionName)
{
    auto x = GetModuleHandleA(moduleName);
    if (x == NULL)
        return FALSE;
    void* oldAddress = GetProcAddress(x, functionName);
    if (oldAddress == NULL)
        return FALSE;
    return HookIAT(hookModule, moduleName, functionName, oldAddress, NULL);
}

BOOL HookVTable(void* pInterface, int index, void* hookFunction, void** oldAddress)
{
    void** address = &(*(void***)pInterface)[index];
    if (address == NULL)
        return FALSE;
    if (oldAddress != NULL && *oldAddress==NULL)
        *oldAddress = *address;
    DWORD oldProtect, oldProtect2;
    VirtualProtect(address, sizeof(DWORD), PAGE_READWRITE, &oldProtect);
    *address = hookFunction;
    VirtualProtect(address, sizeof(DWORD), oldProtect, &oldProtect2);
    return TRUE;
}

BOOL UnhookVTable(void* pInterface, int index, void* oldAddress)
{
    return HookVTable(pInterface, index, oldAddress, NULL);
}


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
    // 70 short jo
    // 71 short jno
    // 72 short jb
    // 73 short jae
    // 74 short je
    // 75 short jne
    // 76 short jna
    // 77 short ja
    // 78 short js
    // 79 short jns
    // 7A short jp
    // 7B short jnp
    // 7C short jl
    // 7D short jnl
    // 7E short jle
    // 7F short jg
    // EB short jmp
    
    // E8 call
    // E9 jmp
    // 0F80 jo
    // 0F81 jno
    // ...

    
    if (sourcecave[0] == 0xE8 || sourcecave[0] == 0xE9) {
        // call/jmp
        uint32_t offset_old = *(uint32_t*)(sourcecave + 1);
        uint32_t offset_new = offset_old + bp_addr - sourcecave;

        *(uint32_t*)(sourcecave + 1) = offset_new;
    } else if (sourcecave[0] == 0x0F && sourcecave[1] >= 0x80 && sourcecave[1] <= 0x8F){
        // long jo~jg
        uint32_t offset_old = *(uint32_t*)(sourcecave + 2);
        uint32_t offset_new = offset_old + bp_addr - sourcecave;

        *(uint32_t*)(sourcecave + 2) = offset_new;
    } else if (sourcecave[0] >= 0x70 && sourcecave[0] <= 0x7F) {
        // short jo~jg
        uint32_t offset_old = *(uint8_t*)(sourcecave + 1);
        uint32_t offset_new = offset_old + bp_addr - sourcecave - 4;
        // turn into long jmp, notice that it will break the jmp after the code
        // init +1 first
        *(uint8_t*)(sourcecave + 1) = sourcecave[0] - 0x70 + 0x80;
        *(uint8_t*)(sourcecave + 0) = 0x0F;
        *(uint32_t*)(sourcecave + 2) = offset_new;
        // long jmp is constant 2 + 4 = 6 bytes
        *(uint32_t*)(sourcecave + 6) = 0xE9;
        *(uint32_t*)(sourcecave + 7) = bp_addr - (sourcecave + 5 + 4);
        // short jmp is 1 + 2 = 3 bytes, so total bytes used more in cave is (6 - 2) + 5 = 9 bytes
    } else if (sourcecave[0] == 0xEB) {
        // short jmp
        uint32_t offset_old = *(uint8_t*)(sourcecave + 1);
        uint32_t offset_new = offset_old + bp_addr - sourcecave - 3;
        // turn into long jmp
        *(uint8_t*)(sourcecave + 0) = 0xE9;
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
            // in case that short jmp turns to long jmps
            hook_page_offset += this->data.hook.instr_len + 9;

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