#include "thprac_hook.h"

#define NOMINMAX

#include "thprac_launcher_cfg.h"
#include "thprac_utils.h"
#include "thprac_gui_locale.h"

#include "..\MinHook\src\buffer.h"
#include "..\MinHook\src\trampoline.h"
#include <MinHook.h>

#include <Windows.h>
#include <cstdint>
#include <vector>
#include <algorithm>
#pragma warning(disable : 4091)
#include <DbgHelp.h>
#pragma warning(default : 4091)

EXTERN_C IMAGE_DOS_HEADER __ImageBase;

#ifdef VEHHOOK_MULTITHREAD
#include <mutex>
#endif

#pragma intrinsic(memcpy)

namespace THPrac {
#ifdef _WIN64
#define XIP Rip
#define XIP_TYPE DWORD64
#else
#define XIP Eip
#define XIP_TYPE DWORD
#endif

uintptr_t ingame_image_base = 0;



#pragma region IAT_hook_VTable_hook
BOOL HookVTable(void* pInterface, int index, void* hookFunction, void** oldAddress)
{
    void** address = &(*(void***)pInterface)[index];
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

BOOL UnhookVTable(void* pInterface, int index, void* oldAddress)
{
    return HookVTable(pInterface, index, oldAddress, NULL);
}

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

        // 遍历导入的函数
        for (; info->u1.AddressOfData != 0; info++, iat++) {
            if ((info->u1.Ordinal & IMAGE_ORDINAL_FLAG) == 0) // 是用函数名导入的
            {
                PIMAGE_IMPORT_BY_NAME name = (PIMAGE_IMPORT_BY_NAME)(hookModuleBase + info->u1.AddressOfData);
                if (strcmp((LPCSTR)name->Name, functionName) == 0)
                    return iat;
            }
        }
        return nullptr;
    }
    return nullptr;
}

bool HookIAT(HANDLE hookModule, LPCSTR moduleName, LPCSTR functionName, void* hookFunction, void** oldAddress)
{
    void** address = FindImportAddress(hookModule, moduleName, functionName);
    if (address == NULL)
        return false;
    if (oldAddress != nullptr)
        *oldAddress = *address;
    DWORD oldProtect, oldProtect2;
    VirtualProtect(address, sizeof(DWORD), PAGE_READWRITE, &oldProtect);
    *address = hookFunction;
    VirtualProtect(address, sizeof(DWORD), oldProtect, &oldProtect2);
    return true;
}

bool UnhookIAT(HANDLE hookModule, LPCSTR moduleName, LPCSTR functionName)
{
    if (GetModuleHandleA(moduleName)) {
        void* oldAddress = GetProcAddress(GetModuleHandleA(moduleName), functionName);
        if (oldAddress == NULL)
            return false;
        return HookIAT(hookModule, moduleName, functionName, oldAddress, NULL);
    }
    return false;
}
#pragma endregion



struct VEHHookCtx {
    uint8_t* m_Src = nullptr;
    CallbackFunc* m_Dest = nullptr;
    uint8_t* m_Trampoline = nullptr;
    uint8_t m_StorageByte = 0;
    bool isActivate = false;

    VEHHookCtx(uint8_t* Src, CallbackFunc* Dest)
    {
        m_Dest = Dest;
        m_Src = Src;
    }
    VEHHookCtx() = default;

    void Reset()
    {
        memset(this, 0, sizeof(VEHHookCtx));
    }
    friend bool operator==(const VEHHookCtx& Ctx1, const VEHHookCtx& Ctx2)
    {
        if (Ctx1.m_Dest == Ctx2.m_Dest && Ctx1.m_Src == Ctx2.m_Src)
            return true;
        return false;
    }
};

std::vector<VEHHookCtx>* g_VEHHookVector = nullptr;
std::vector<VEHHookCtx>::iterator VEHHookLookUp(void* addr)
{
    for (auto it = g_VEHHookVector->begin(); it != g_VEHHookVector->end(); ++it) {
        if ((DWORD_PTR)it->m_Src == (DWORD_PTR)addr) {
            return it;
        }
    }
    return g_VEHHookVector->end();
}
std::vector<VEHHookCtx>::iterator VEHHookLookUp(DWORD_PTR addr)
{
    for (auto it = g_VEHHookVector->begin(); it != g_VEHHookVector->end(); ++it) {
        if ((DWORD_PTR)it->m_Src == addr) {
            return it;
        }
    }
    return g_VEHHookVector->end();
}

#ifdef VEHHOOK_MULTITHREAD
static std::mutex m_TargetMutex;
#define VEHHookMutexGuard() std::lock_guard<std::mutex> m_Lock(m_TargetMutex);
#else
#define VEHHookMutexGuard()
#endif

__declspec(noinline) void InvokeCallback(const VEHHookCtx vehHook, PCONTEXT pCtx)
{
    auto xipBackup = pCtx->XIP;
    vehHook.m_Dest(pCtx);
    if (xipBackup == pCtx->XIP) {
        pCtx->XIP = (DWORD_PTR)vehHook.m_Trampoline;
    }
}
__declspec(noinline) LONG CALLBACK VEHHandler(EXCEPTION_POINTERS* ExceptionInfo)
{
    VEHHookMutexGuard();

    DWORD ExceptionCode = ExceptionInfo->ExceptionRecord->ExceptionCode;
    if (ExceptionCode == EXCEPTION_BREAKPOINT) {
        auto ctx = VEHHookLookUp(ExceptionInfo->ContextRecord->XIP - ingame_image_base);
        if (ctx != g_VEHHookVector->end()) {
            InvokeCallback(*ctx, ExceptionInfo->ContextRecord);
            return EXCEPTION_CONTINUE_EXECUTION;
        }
    }

    return EXCEPTION_CONTINUE_SEARCH;
}

static bool __hook_gctx_status = false;
bool VEHWriteByte(void* addr, uint8_t byte, uint8_t* backup = nullptr)
{
    DWORD oldProtect;
    if (VirtualProtect(addr, 1, PAGE_EXECUTE_READWRITE, &oldProtect)) {
        if (backup) {
            *backup = *(uint8_t*)addr;
        }
        *(uint8_t*)addr = byte;
        return VirtualProtect(addr, 1, oldProtect, &oldProtect);
    }
    return false;
}
void VEHHookInit()
{
    if (!g_VEHHookVector) {
        g_VEHHookVector = new std::vector<VEHHookCtx>();
        AddVectoredExceptionHandler(1, &VEHHandler);
    }
}
void VEHHookEnable(void* target)
{
    VEHHookMutexGuard();
    auto ctx = VEHHookLookUp(target);
    if (ctx->isActivate)
        return;

    VEHWriteByte((void*)(ingame_image_base + (uintptr_t)ctx->m_Src), 0xCC, &ctx->m_StorageByte);

    ctx->isActivate = true;
}
void VEHHookDisable(void* target)
{
    VEHHookMutexGuard();
    auto ctx = VEHHookLookUp(target);
    if (!ctx->isActivate)
        return;

    VEHWriteByte((void*)(ingame_image_base + (uintptr_t)ctx->m_Src), ctx->m_StorageByte);
    FlushInstructionCache(GetCurrentProcess(), ctx->m_Src, 1);

    ctx->isActivate = false;
}
bool VEHHookAdd(void* target, CallbackFunc* detour, void* trampoline)
{
    VEHHookInit();
    if (VEHHookLookUp(target) != g_VEHHookVector->end()) {
        return false;
    }

    VEHHookCtx ctx((uint8_t*)target, detour);
    ctx.m_Trampoline = (uint8_t*)trampoline;
    g_VEHHookVector->push_back(ctx);

    return true;
}
void VEHHookDelete(void* target)
{
    auto ctx = VEHHookLookUp(target);
    if (ctx != g_VEHHookVector->end()) {
        if (ctx->isActivate)
            VEHHookDisable(target);
        g_VEHHookVector->erase(ctx);
    }
}

HookCtx::~HookCtx()
{
    Reset();
}
bool HookCtx::Reset()
{
    if (!mIsHookReady) {
        return false;
    }
    if (mIsHookEnabled) {
        Disable();
    }

    if (mIsPatch) {
        free((void*)mPatch);
    } else {
        VEHHookDelete(mTarget);
        if (mTrampoline) {
            FreeBuffer(mTrampoline);
        }
    }

    mTarget = nullptr;
    mDetour = nullptr;
    mTrampoline = nullptr;
    mPatch = nullptr;
    mPatchSize = 0;
    mIsPatch = false;
    mIsHookReady = false;
    mIsHookEnabled = false;
    return true;
}
bool HookCtx::Setup()
{
    if (mIsHookReady) {
        return false;
    }

    if (mIsPatch) {
        return Setup(mTarget, mPatch, mPatchSize);
    } else {
        return Setup(mTarget, mDetour);
    }
}
bool HookCtx::Setup(void* target, CallbackFunc* detour)
{
    if (mIsHookReady) {
        return false;
    }

    LPVOID buffer = AllocateBuffer((LPVOID)((uintptr_t)target + ingame_image_base));
    if (buffer) {
        TRAMPOLINE ct;
        ct.pTarget = (LPVOID)((uintptr_t)target + ingame_image_base);
        ct.pDetour = (void*)detour;
        ct.pTrampoline = buffer;
        if (CreateTrampolineFunctionEx(&ct, 1, FALSE)) {
            mTrampoline = buffer;
        } else {
            return false;
        }
    }

    if (!VEHHookAdd(target, detour, buffer)) {
        return false;
    }

    mTarget = target;
    mDetour = detour;
    mIsPatch = false;
    mIsHookReady = true;
    return true;
}
bool HookCtx::Setup(void* target, const char* patch, size_t patch_size)
{
    if (mIsHookReady) {
        return false;
    }

    auto buffer = malloc(patch_size * 2);
    if (!buffer)
        return false;
    memcpy(buffer, patch, patch_size);
    memcpy((void*)((size_t)buffer + patch_size), (void*)((uintptr_t)target + ingame_image_base), patch_size);

    mTarget = target;
    mIsPatch = true;
    mPatch = (char*)buffer;
    mPatchSize = patch_size;
    mIsHookReady = true;
    return true;
}
bool HookCtx::Enable()
{
    if (!mIsHookReady) {
        return false;
    }

    if (mIsPatch) {
        DWORD oldProtect;
        VirtualProtect((LPVOID)((uintptr_t)mTarget + ingame_image_base), mPatchSize, PAGE_EXECUTE_READWRITE, &oldProtect);
        memcpy((void*)((uintptr_t)mTarget + ingame_image_base), mPatch, mPatchSize);
        VirtualProtect((LPVOID)((uintptr_t)mTarget + ingame_image_base), mPatchSize, oldProtect, &oldProtect);
    } else {
        VEHHookEnable(mTarget);
    }

    return true;
}
bool HookCtx::Disable()
{
    if (!mIsHookReady) {
        return false;
    }

    if (mIsPatch) {
        DWORD oldProtect;
        VirtualProtect((LPVOID)((uintptr_t)mTarget + ingame_image_base), mPatchSize, PAGE_EXECUTE_READWRITE, &oldProtect);
        memcpy((void*)((uintptr_t)mTarget + ingame_image_base), (void*)((size_t)mPatch + mPatchSize), mPatchSize);
        VirtualProtect((LPVOID)((uintptr_t)mTarget + ingame_image_base), mPatchSize, oldProtect, &oldProtect);
    } else {
        VEHHookDisable(mTarget);
    }

    return true;
}
bool HookCtx::Toggle(bool status)
{
    if (status) {
        return Enable();
    } else {
        return Disable();
    }
}

void HookCtx::VEHInit()
{
    VEHHookInit();
}

}
