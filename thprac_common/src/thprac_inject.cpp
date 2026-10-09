#include <wininternal.h>

#include "thprac_inject.h"
#include "thprac_identify.h"
#include "thprac_log.h"
#include "thprac_pe.h"
#include "thprac_gui_locale.h"
#include "thprac_utils.h"
#include "utils.h"

#include <wchar.h>
#include <inttypes.h>

#include <algorithm>
#include <string>

DWORD RunRemoteThread_Impl(HANDLE hProcess, uintptr_t addr) {
    SIZE_T byteRet;

    void* funcs[] = { (void*)&LoadLibraryW, (void*)&GetProcAddress, (void*)&FreeLibraryAndExitThread };
    WriteProcessMemory(hProcess, (LPVOID)(addr + offsetof(RemoteParamNative, LoadLibraryW_addr)), funcs, sizeof(funcs), &byteRet);

    DWORD rResult = 0x20000001;
    if (auto tInit = CreateRemoteThread(hProcess, nullptr, 0, (LPTHREAD_START_ROUTINE)addr, (LPVOID)addr, 0, nullptr)) {
        WaitForSingleObject(tInit, INFINITE);
        GetExitCodeThread(tInit, &rResult);
    }
    return rResult;
}

DWORD RunRemoteThread_CallDLL(HANDLE hProcess, uintptr_t addr, uintptr_t bits) {
    auto dllPath = GetThpracDllForArch(bits);

    // It doesn't really matter if the rundll32.exe matches the architecture of the DLL.
    // If the architectures mismatch, rundll32 will start another rundll32 that does have
    // the right architecture. However, handles inherited by rundll32 the first will not
    // be inherited by rundll32 the second, that's why we should get it right the first time.

    std::wstring_view rundllDir;

#if TH_X86
    BOOL isWow64Proc = FALSE;
    IsWow64Process(CurrentProcessHandle, &isWow64Proc);
    if (isWow64Proc) {
        if (bits == 64) {
            rundllDir = L"Sysnative";
        }
        else if (bits == 32) {
            rundllDir = L"System32";            
        }
    }
    else if(bits == 32) {
        rundllDir = L"System32";
    }
#elif TH_X64
    if (bits == 64) {
        rundllDir = L"System32";
    }
    else if (bits == 32) {
        rundllDir = L"SysWOW64";
    }
#endif
    
    int wrote = 0;

    std::wstring_view rundllPath_format = L"%s\\%s\\rundll32.exe";
    auto rundllPath_format_len = rundllPath_format.length() + rundllDir.length() + t_strlen(Kuser_Shared_Data->NtSystemRoot);
    VLA(wchar_t, rundllPath, rundllPath_format_len);
    wrote = _snwprintf(rundllPath, rundllPath_format_len, rundllPath_format.data(), Kuser_Shared_Data->NtSystemRoot, rundllDir);
    rundllPath[wrote] = 0;

    auto cmdFormat = L"%s %s,thprac_rundll_inject_helper_internal %" PRIXPTR " %" PRIXPTR;

    auto cmdLen = t_strlen(cmdFormat) + g_Dll32Path.length() + 64;
    VLA(wchar_t, cmd, cmdLen);
    wrote = _snwprintf(cmd, cmdLen - 1, cmdFormat, rundllPath, dllPath.data(), hProcess, addr);
    cmd[wrote] = 0;

    STARTUPINFOW si = { .cb = sizeof(si) };
    PROCESS_INFORMATION pi = {};

    CreateProcessW(rundllPath, cmd, nullptr, nullptr, TRUE, 0, nullptr, nullptr, &si, &pi);
    WaitForSingleObject(pi.hProcess, INFINITE);

    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);

    VLA_FREE(cmd);

    return 0;
}

DWORD RunRemoteThread(HANDLE hProcess, uintptr_t addr, uintptr_t bits) {
    switch (bits) {
#if TH_X64
    case 32:
        return RunRemoteThread_CallDLL(hProcess, addr, 32);
    case 64:
        return RunRemoteThread_Impl(hProcess, addr);
#elif TH_X86
    case 64:
        return RunRemoteThread_CallDLL(hProcess, addr, 64);
    case 32:
        return RunRemoteThread_Impl(hProcess, addr);
#endif
    DEFAULT_UNREACHABLE;
    }
}

static uint8_t inject_shellcode_32[] = {
    0x8B, 0x74, 0x24, 0x04,              // mov esi,dword ptr ss:[esp+4]
    0x83, 0xE4, 0xF0,                    // and esp, -0x10
    0x8D, 0x46,                          // lea eax,dword ptr ds:[esi->dllPath]
    offsetof(RemoteParam32, dllPath),
    0x50,                                // push eax                     
    0xFF, 0x56,                          // call dword ptr ds:[esi->LoadLibraryW_addr]   
    offsetof(RemoteParam32, LoadLibraryW_addr),
    0x89, 0xC7,                          // mov edi,eax                  
    0x85, 0xFF,                          // test edi,edi                 
    0x74, 0x0E,                          // je +0xE   
    0x6A, 0x01,                          // push 1                       
    0x57,                                // push edi                     
    0xFF, 0x56,                          // call dword ptr ds:[esi->GetProcAddress_addr]   
    offsetof(RemoteParam32, GetProcAddress_addr),
    0x85, 0xC0,                          // test eax,eax                 
    0x74, 0x04,                          // je +0x4
    0x89, 0xF1,                          // mov ecx, esi
    0xFF, 0xD0,                          // call eax                      
    0x64, 0xA1, 0x18, 0x00, 0x00, 0x00,  // mov eax,dword ptr fs:[18]    
    0xFF, 0x70, 0x34,                    // push dword ptr ds:[eax+34]  
    0x57,                                // push edi                     
    0xFF, 0x56,                          // call dword ptr ds:[esi->FreeLibraryAndExitThread_addr]
    offsetof(RemoteParam32, FreeLibraryAndExitThread_addr),
    0xCC
};
static uint8_t inject_shellcode_64[] = {
    0x48, 0x83, 0xE4, 0xF0,                               // and rsp, -0x10
    0x48, 0x83, 0xEC, 0x20,                               // sub rsp, 0x20
    0x48, 0x89, 0xCE,                                     // mov rsi,rcx                   
    0x48, 0x83, 0xC1,                                     // add rcx, offsetof(RemoteParam64, dllPath)                     
    offsetof(RemoteParam64, dllPath),                     
    0xFF, 0x56,                                           // call qword ptr ds:[rsi->LoadLibraryW_addr]    
    offsetof(RemoteParam64, LoadLibraryW_addr),           
    0x48, 0x89, 0xC7,                                     // mov rdi,rax                   
    0x48, 0x85, 0xFF,                                     // test rdi,rdi                  
    0x74, 0x17,                                           // je +0x17      
    0x48, 0x89, 0xF9,                                     // mov rcx,rdi                   
    0x48, 0xC7, 0xC2, 0x01, 0x00, 0x00, 0x00,             // mov rdx,1                     
    0xFF, 0x56,                                           // call qword ptr ds:[rsi->GetProcAddress_addr]    
    offsetof(RemoteParam64, GetProcAddress_addr),         
    0x48, 0x85, 0xC0,                                     // test rax,rax                  
    0x74, 0x05,                                           // je +0x5    
    0x48, 0x89, 0xF1,                                     // mov rcx,rsi
    // has to be a call because x64 functions assume that the stack pointer
    // will be aligned to an 8 byte boundary and NOT aligned to a 16 byte boundary
    0xFF, 0xD0,                                           // call rax
    0x48, 0x89, 0xF9,                                     // mov rcx,rdi                   
    0x65, 0x48, 0x8B, 0x04, 0x25, 0x30, 0x00, 0x00, 0x00, // mov rax,qword ptr gs:[30]     
    0x8B, 0x50, 0x68,                                     // mov edx,dword ptr ds:[rax+68] 
    0xFF, 0x56,                                           // call qword ptr ds:[rsi->GetProcAddress_addr]   
    offsetof(RemoteParam64, FreeLibraryAndExitThread_addr),                                          
    0xCC
};

static_assert(sizeof(inject_shellcode_32) == sizeof(RemoteParam32::shellcode));
static_assert(sizeof(inject_shellcode_64) == sizeof(RemoteParam64::shellcode));

static bool LoadThpracDll(HANDLE hProcess, uint32_t flags, size_t bits) {
    SIZE_T byteRet;

    uintptr_t rBufAddr = 0;
    SIZE_T rBufSize;

    auto dllPath = GetThpracDllForArch(bits);

    if (bits == 32) {
        rBufSize = RoundUp(sizeof(RemoteParam32) + (SIZE_T)dllPath.length() * sizeof(wchar_t), 16);
    }
    if (bits == 64) {
        rBufSize = RoundUp(sizeof(RemoteParam64) + (SIZE_T)dllPath.length() * sizeof(wchar_t), 16);
    }

    NtAllocateVirtualMemory(hProcess, (LPVOID*)&rBufAddr, 0x7FFFFFFF, &rBufSize, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

    if (bits == 32) {
        RemoteParam32 buf;
        memcpy(&buf.shellcode, inject_shellcode_32, sizeof(buf.shellcode));
        buf.flags = flags;

        WriteProcessMemory(hProcess, (LPVOID)rBufAddr, &buf, offsetof(RemoteParam32, dllPath), &byteRet);
        WriteProcessMemory(hProcess, (LPVOID)(rBufAddr + offsetof(RemoteParam32, dllPath)), dllPath.data(), dllPath.length() * sizeof(wchar_t), &byteRet);

        RunRemoteThread(hProcess, rBufAddr, 32);
    }

    if (bits == 64) {
        RemoteParam64 buf;
        memcpy(buf.shellcode, inject_shellcode_64, sizeof(buf.shellcode));
        buf.flags = flags;

        WriteProcessMemory(hProcess, (LPVOID)rBufAddr, &buf, offsetof(RemoteParam64, dllPath), &byteRet);
        WriteProcessMemory(hProcess, (LPVOID)(rBufAddr + offsetof(RemoteParam64, dllPath)), dllPath.data(), dllPath.length() * sizeof(wchar_t), &byteRet);

        RunRemoteThread(hProcess, rBufAddr, 64);
    }

    rBufSize = 0;
    NtFreeVirtualMemory(hProcess, (LPVOID*)&rBufAddr, &rBufSize, MEM_RELEASE);

    return true;
}

static bool CheckThpracAttached(DWORD pid) {
    wchar_t buf[32] = {};
    _snwprintf(buf, 31, L"Global\\thprac pid %d", pid);
    if (HANDLE hEvent = OpenEventW(SYNCHRONIZE, FALSE, buf)) {
        CloseHandle(hEvent);
        return true;
    }
    return false;
}

static bool CheckIfAnyGame() {
    constexpr const wchar_t* allMutexNames[] = {
        L"Touhou Koumakyou App",
        L"Touhou YouYouMu App",
        L"Touhou 08 App",
        L"Touhou 10 App",
        L"Touhou 11 App",
        L"Touhou 12 App",
        L"th17 App",
        L"th18 App",
        L"th185 App",
        L"th19 App",
        L"th20 App",
    };

    for (const wchar_t* mutexName : allMutexNames) {
        HANDLE hMutex = OpenMutexW(SYNCHRONIZE, FALSE, mutexName);
        if (hMutex) {
            CloseHandle(hMutex);
            return true;
        }
    }
    return false;
}

extern FUNC_T(NtWow64QueryInformationProcess64);
extern FUNC_T(NtWow64ReadVirtualMemory64);

static uint64_t GetProcessModuleBase(HANDLE hProc) {
    constexpr const int PEB32_ImageBaseAddress_offset = 0x8;
    constexpr const int PEB64_ImageBaseAddress_offset = 0x10;

    uint64_t base_ret = 0;
    union {
        SIZE_T byteRet;
        ULONG byteRet32;
        ULONG64 byteRet64;
    };

    NTSTATUS status;

#if TH_X86
    if (NtWow64QueryInformationProcess64_ptr && NtWow64ReadVirtualMemory64_ptr) {
        PROCESS_BASIC_INFORMATIONX<uint64_t> pbi64;

        if (status = NtWow64QueryInformationProcess64_ptr(hProc, ProcessBasicInformation, &pbi64, sizeof(pbi64), &byteRet32)) {
            SetLastError(RtlNtStatusToDosError(status));
            return 0;
        }
        if (status = NtWow64ReadVirtualMemory64_ptr(hProc, (PVOID64)(pbi64.PebBaseAddress + PEB64_ImageBaseAddress_offset), &base_ret, 8, &byteRet64)) {
            SetLastError(RtlNtStatusToDosError(status));
            return 0;
        }
        return base_ret;
    }
    else {
        PROCESS_BASIC_INFORMATIONX<uint32_t> pbi32;
        if (status = NtQueryInformationProcess(hProc, ProcessBasicInformation, &pbi32, sizeof(pbi32), &byteRet32)) {
            SetLastError(RtlNtStatusToDosError(status));
            return 0;
        }
        ReadProcessMemory(hProc, (LPVOID)(pbi32.PebBaseAddress + PEB32_ImageBaseAddress_offset), &base_ret, 4, &byteRet);
        return base_ret;
    }
#elif TH_X64
    PROCESS_BASIC_INFORMATION pbi;
    if (status = NtQueryInformationProcess(hProc, ProcessBasicInformation, &pbi, sizeof(pbi), nullptr)) {
        SetLastError(RtlNtStatusToDosError(status));
        return 0;
    }

    LPVOID based = (LPVOID)((uintptr_t)pbi.PebBaseAddress + PEB32_ImageBaseAddress_offset);
    uintptr_t ret = 0;

    // If this fails, it'll return 0 and GetLastError will already be set.
    ReadProcessMemory(hProc, based, &ret, sizeof(ret), &byteRet);

    return ret;
#endif
}

const THGameVersion* CheckOngoingGameByPID(DWORD pid, uint64_t* pOutBase, HANDLE* pOutHandle) {
    if (CheckThpracAttached(pid)) {
        return nullptr;
    }

    auto hProc = OpenProcess(
        PROCESS_QUERY_INFORMATION | PROCESS_CREATE_THREAD | PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE,
        TRUE, pid);
    if (!hProc)
        return nullptr;

    defer({
        if (pOutHandle) {
            *pOutHandle = hProc;
        } else {
            CloseHandle(hProc);
        }
    });

    uint64_t base = GetProcessModuleBase(hProc);
    if (!base) {
        return nullptr;
    }
    if (pOutBase) {
        *pOutBase = base;
    }

    auto exeSig = GetRemoteExeInfo(hProc, base);
    for (size_t i = 0; i < gGameVersionsCount; i++) {
        if (exeSig == gGameVersions[i].exeInfo) {
            return gGameVersions + i;
        }
    }
    return nullptr;
}

bool ApplyToProcById(DWORD pid) {
    uint64_t base;
    HANDLE hProc;
    auto* sig = CheckOngoingGameByPID(pid, &base, &hProc);
    if (sig) {
        LoadThpracDll(hProc, RUN_FLAG_THPRAC, sig->get_bits());
    }
    else {
        return false;
    }

    if (hProc) {
        CloseHandle(hProc);
    }
    return true;
}

bool FindAndAttach(bool prompt_if_no_game, bool prompt_if_yes_game, THGameID gameID) {
    bool hasPrompted = false;
    auto TryProcess = [&](SYSTEM_PROCESS_INFORMATION* proc, THGameID requiredGameID) -> bool {
        uint64_t base;
        HANDLE hProc = NULL;
        const THGameVersion* gameSig = CheckOngoingGameByPID((DWORD)proc->UniqueProcessId, &base, &hProc);
        defer(if (hProc) CloseHandle(hProc));

        if (!gameSig || !gameSig->initFunc || (requiredGameID != ID_UNKNOWN && gameSig->gameId != requiredGameID)) {
            return false;
        }
        if (prompt_if_yes_game) {
            hasPrompted = true;
            int choice = log_mboxf(0, MB_YESNO, S(THPRAC_PR_APPLY), S(THPRAC_PR_ASK_ATTACH), gThGameStrs[gameSig->gameId]);
            if (choice != IDYES) {
                return false;
            }
        }
        if (LoadThpracDll(hProc, RUN_FLAG_THPRAC, gameSig->get_bits())) {
            if (prompt_if_yes_game) {
                hasPrompted = true;
                log_mbox(0, MB_ICONASTERISK | MB_OK, S(THPRAC_PR_COMPLETE), S(THPRAC_PR_INFO_ATTACHED));
            }
            return true;
        }
        else {
            hasPrompted = true;
            log_mbox(0, MB_ICONERROR | MB_OK, S(THPRAC_PR_ERROR), S(THPRAC_PR_ERR_ATTACH));
            return true;
        }
    };

    if (CheckIfAnyGame()) {
        ULONG bufLen = 0;
        auto err = NtQuerySystemInformation(SystemProcessInformation, nullptr, 0, &bufLen);
        LPVOID buf = VirtualAlloc(nullptr, bufLen, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
        if (!buf) {
            goto no_game;
        }
        defer (if (buf) VirtualFree(buf, 0, MEM_RELEASE));
        if (!NT_SUCCESS(NtQuerySystemInformation(SystemProcessInformation, buf, bufLen, &bufLen))) {
            goto no_game;
        }

        for (auto* proc = (SYSTEM_PROCESS_INFORMATION*)buf; proc;
             proc = proc->NextEntryOffset ? (SYSTEM_PROCESS_INFORMATION*)((uintptr_t)proc + proc->NextEntryOffset) : nullptr
        ) {
            THGameID exeGameId = ParseExeName(UNICODE_STRING_PARAM(proc->ImageName));

            if (gameID == ID_UNKNOWN ? exeGameId == ID_UNKNOWN : exeGameId != gameID) {
                continue;
            }

            if (TryProcess(proc, exeGameId)) {
                return true;
            }
            proc->UniqueProcessId = 0;
        }

        for (auto* proc = (SYSTEM_PROCESS_INFORMATION*)buf; proc;
            proc = proc->NextEntryOffset ? (SYSTEM_PROCESS_INFORMATION*)((uintptr_t)proc + proc->NextEntryOffset) : nullptr
            )
        {
            if (!proc->UniqueProcessId) {
                continue;
            }

            if (TryProcess(proc, gameID)) {
                return true;
            }
        }
    }

no_game:
    if (prompt_if_no_game && !hasPrompted) {
        log_mbox(0, MB_ICONERROR | MB_OK, S(THPRAC_PR_ERROR), S(THPRAC_PR_ERR_NO_GAME));
    }

    return false;
}

bool RunGame(const wchar_t* exeFn, wchar_t* cmdLine, uint32_t flags, uintptr_t bits) {
    STARTUPINFOW si = {
        .cb = sizeof(si),
    };
    PROCESS_INFORMATION pi = {};

    const wchar_t* file_spec = std::max(wcsrchr(exeFn, L'\\'), wcsrchr(exeFn, L'/'));
    BOOL ret;

    std::wstring exeDir;
    
    SECURITY_ATTRIBUTES secAttrs = { .nLength = sizeof(secAttrs), .bInheritHandle = TRUE };
    if (file_spec) {
        exeDir = std::wstring(exeFn, file_spec);
        ret = CreateProcessW(exeFn, cmdLine, &secAttrs, nullptr, FALSE, CREATE_SUSPENDED, nullptr, exeDir.c_str(), &si, &pi);
    }
    else {
        ret = CreateProcessW(exeFn, cmdLine, &secAttrs, nullptr, FALSE, CREATE_SUSPENDED, nullptr, nullptr, &si, &pi);
    }

    if (!ret) {
        return false;
    }

    bool res = false;
    LoadThpracDll(pi.hProcess, flags, bits);

    ResumeThread(pi.hThread);

    // TODO: determine if these should be returned
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return true;
}