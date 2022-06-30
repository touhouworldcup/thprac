#include "thprac_hook.h"

#define NOMINMAX

#include "thprac_log.h"
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

//#include <string>
#define MB_INFO(str) MessageBoxA(NULL, str, str, MB_OK);
EXTERN_C IMAGE_DOS_HEADER __ImageBase;

// #define VEHHOOK_MULTITHREAD

#ifdef VEHHOOK_MULTITHREAD
#include <mutex>
#endif

#pragma intrinsic(memcpy)

namespace THPrac {

struct DmpFileDesc {
    std::wstring path;
    uint64_t time = 0;
    bool operator()(DmpFileDesc& a, DmpFileDesc& b) const { return a.time < b.time; }
};

bool IsBadStackPtr(void* p)
{
    MEMORY_BASIC_INFORMATION mbi = { 0 };
    if (::VirtualQuery(p, &mbi, sizeof(mbi))) {
        DWORD mask = (PAGE_READONLY | PAGE_READWRITE | PAGE_WRITECOPY | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY);
        bool b = !(mbi.Protect & mask);
        // check the page is not a guard page
        if (mbi.Protect & (PAGE_GUARD | PAGE_NOACCESS))
            b = true;

        return b;
    }
    return true;
}

void CleanDumpDir(std::wstring& dmpDir)
{
    WIN32_FIND_DATAW findData;
    std::vector<DmpFileDesc> dmpFiles;
    std::wstring searchDir = dmpDir + L"\\*.dmp";
    HANDLE searchHnd = FindFirstFileW(searchDir.c_str(), &findData);

    if (searchHnd == INVALID_HANDLE_VALUE) {
        return;
    }

    do {
        if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            continue;
        }
        searchDir = dmpDir + L"\\" + std::wstring(findData.cFileName);
        DmpFileDesc desc;
        desc.path = searchDir;
        desc.time = ((uint64_t)findData.ftCreationTime.dwHighDateTime << 32) + (uint64_t)findData.ftCreationTime.dwLowDateTime;
        dmpFiles.push_back(desc);
    } while (FindNextFileW(searchHnd, &findData));
    FindClose(searchHnd);

    if (dmpFiles.size() >= 50) {
        DmpFileDesc dummy;
        std::sort(dmpFiles.begin(), dmpFiles.end(), dummy);
        for (size_t i = 0; dmpFiles.size() - i >= 50; ++i) {
            DeleteFileW(dmpFiles[i].path.c_str());
        }
    }

}

bool MakeMiniDump(EXCEPTION_POINTERS* e, std::wstring* dmpName = nullptr)
{
    auto hDbgHelp = LoadLibraryW(L"dbghelp.dll");
    if (hDbgHelp == nullptr) {
        return false;
    }
    auto pMiniDumpWriteDump = (decltype(&MiniDumpWriteDump))GetProcAddress(hDbgHelp, "MiniDumpWriteDump");
    if (pMiniDumpWriteDump == nullptr) {
        return false;
    }

    auto logFilePath = LauncherGetDataDir();
    CreateDirectoryW(logFilePath.c_str(), NULL);
    logFilePath += L"crashdump";
    CreateDirectoryW(logFilePath.c_str(), NULL);
    CleanDumpDir(logFilePath);
    SYSTEMTIME time;
    GetSystemTime(&time);
    wchar_t logFileName[MAX_PATH];
    swprintf_s(logFileName, L"thprac-%04d%02d%02d-%02d%02d%02d-%04d.dmp", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond, time.wMilliseconds);
    logFilePath += L"\\";
    logFilePath += logFileName;

    auto hFile = CreateFileW(logFilePath.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        return false;
    }

    MINIDUMP_EXCEPTION_INFORMATION exceptionInfo;
    exceptionInfo.ThreadId = GetCurrentThreadId();
    exceptionInfo.ExceptionPointers = e;
    exceptionInfo.ClientPointers = FALSE;

    auto dumped = pMiniDumpWriteDump(
        GetCurrentProcess(),
        GetCurrentProcessId(),
        hFile,
        MINIDUMP_TYPE(MiniDumpNormal | MiniDumpWithProcessThreadData | MiniDumpWithThreadInfo | MiniDumpWithUnloadedModules | MiniDumpWithFullMemoryInfo | MiniDumpScanMemory),
        e ? &exceptionInfo : nullptr,
        nullptr,
        nullptr);

    CloseHandle(hFile);

    if (dmpName) {
        *dmpName = logFileName;
    }

    return dumped;
}

__declspec(noinline) LONG WINAPI UEHandler(__in struct _EXCEPTION_POINTERS* ExceptionInfo)
{
    MessageBoxW(NULL, utf8_to_utf16(XSTR(THPRAC_UNHANDLED_EXCEPTION)).c_str(), utf8_to_utf16(XSTR(THPRAC_PR_ERROR)).c_str(), MB_OK | MB_ICONERROR | MB_SYSTEMMODAL);
    return EXCEPTION_CONTINUE_SEARCH;
}

#ifdef _WIN64
#define XIP Rip
#define XIP_TYPE DWORD64
#else
#define XIP Eip
#define XIP_TYPE DWORD
#endif

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
        auto ctx = VEHHookLookUp(ExceptionInfo->ContextRecord->XIP);
        if (ctx != g_VEHHookVector->end()) {
            InvokeCallback(*ctx, ExceptionInfo->ContextRecord);
            return EXCEPTION_CONTINUE_EXECUTION;
        }
    }

#if 0
    auto c = ExceptionInfo->ContextRecord;
    LogA(0, "[ERROR] Unknown exception encountered: %08x at %08x", ExceptionInfo->ExceptionRecord->ExceptionCode, c->Eip);
    LogA(0, "thprac base: %08x", (int)&__ImageBase);
    LogA(0, "Register: Eax: %08x, Ebx: %08x, Ecx: %08x, Edx: %08x, \nEsi: %08x, Edi: %08x, Ebp: %08x, Esp: %08x, ",
        c->Eax, c->Ebx, c->Ecx, c->Edx, c->Esi, c->Edi, c->Ebp, c->Esp);
    LogA(0, "Stack dump:");
    for (int i = 0; i < 16; ++i) {
        DWORD* stackPtr = (DWORD*)(c->Esp + i * 16 * sizeof(DWORD));
        DWORD stackValue[16];
        memset(stackValue, 0, 16 * sizeof(DWORD));
        for (int j = 0; j < 16; ++j) {
            if (!IsBadStackPtr(stackPtr)) {
                stackValue[j] = *stackPtr;
            }
            stackPtr++;
        }
        LogA(0, "%08x|%08x|%08x|%08x|%08x|%08x|%08x|%08x|%08x|%08x|%08x|%08x|%08x|%08x|%08x|%08x",
            stackValue[0], stackValue[1], stackValue[2], stackValue[3], stackValue[4], stackValue[5], stackValue[6], stackValue[7],
            stackValue[8], stackValue[9], stackValue[10], stackValue[11], stackValue[12], stackValue[13], stackValue[14], stackValue[15]);
    }
#endif

    MakeMiniDump(ExceptionInfo);
    SetUnhandledExceptionFilter(&UEHandler);

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
    SetUnhandledExceptionFilter(&UEHandler);
}
void VEHHookEnable(void* target)
{
    VEHHookMutexGuard();
    auto ctx = VEHHookLookUp(target);
    if (ctx->isActivate)
        return;

    VEHWriteByte(ctx->m_Src, 0xCC, &ctx->m_StorageByte);
    FlushInstructionCache(GetCurrentProcess(), ctx->m_Src, 1);

    ctx->isActivate = true;
}
void VEHHookDisable(void* target)
{
    VEHHookMutexGuard();
    auto ctx = VEHHookLookUp(target);
    if (!ctx->isActivate)
        return;

    VEHWriteByte(ctx->m_Src, ctx->m_StorageByte);
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

    LPVOID buffer = AllocateBuffer(target);
    if (buffer) {
        TRAMPOLINE ct;
        ct.pTarget = target;
        ct.pDetour = detour;
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
    memcpy((void*)((size_t)buffer + patch_size), target, patch_size);

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
        VirtualProtect(mTarget, mPatchSize, PAGE_EXECUTE_READWRITE, &oldProtect);
        memcpy(mTarget, mPatch, mPatchSize);
        FlushInstructionCache(GetCurrentProcess(), mTarget, mPatchSize);
        VirtualProtect(mTarget, mPatchSize, oldProtect, &oldProtect);
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
        VirtualProtect(mTarget, mPatchSize, PAGE_EXECUTE_READWRITE, &oldProtect);
        memcpy(mTarget, (void*)((size_t)mPatch + mPatchSize), mPatchSize);
        FlushInstructionCache(GetCurrentProcess(), mTarget, mPatchSize);
        VirtualProtect(mTarget, mPatchSize, oldProtect, &oldProtect);
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
