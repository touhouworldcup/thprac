#include <wininternal.h>

#include "thprac_inject.h"
#include "thprac_identify.h"
#include "thprac_log.h"
#include "thprac_pe.h"
#include "thprac_gui_locale.h"
#include "thprac_utils.h"
#include "utils.h"

#include <wchar.h>

#include <algorithm>
#include <string>

UINT_PTR gRemoteParamAddr = 0;

uintptr_t GetProcessModuleBase(HANDLE hProc) {
    PROCESS_BASIC_INFORMATION pbi;
    if (NTSTATUS err = NtQueryInformationProcess(hProc, ProcessBasicInformation, &pbi, sizeof(pbi), nullptr)) {
        SetLastError(RtlNtStatusToDosError(err));
        return 0;
    }

    LPVOID based = (LPVOID)((uintptr_t)pbi.PebBaseAddress + offsetof(PEB, ImageBaseAddress));

    uintptr_t ret = 0;
    SIZE_T byteRet;

    // If this fails, it'll return 0 and GetLastError will already be set.
    ReadProcessMemory(hProc, based, &ret, sizeof(ret), &byteRet);

    return ret;
}

constexpr DWORD thpracSig = 'CARP'; // 🐟

bool WriteTHPracSig(HANDLE hProc, uintptr_t base) {
    UINT_PTR sigAddr = 0;
    SIZE_T bytesReadRPM;
    ReadProcessMemory(hProc, (void*)(base + 0x3c), &sigAddr, 4, &bytesReadRPM);
    if (bytesReadRPM != 4 || !sigAddr)
        return false;
    sigAddr += base;
    sigAddr -= 4;

    SIZE_T bytesWrote;
    DWORD oldProtect;
    if (!VirtualProtectEx(hProc, (void*)sigAddr, 4, PAGE_EXECUTE_READWRITE, &oldProtect)) {
        return false;
    }
    if (!WriteProcessMemory(hProc, (void*)sigAddr, &thpracSig, 4, &bytesWrote)) {
        return false;
    }
    if (!VirtualProtectEx(hProc, (void*)sigAddr, 4, oldProtect, &oldProtect)) {
        return false;
    }
    return true;
}

bool CheckTHPracSig(HANDLE hProc, uintptr_t base) {
    UINT_PTR sigAddr;
    SIZE_T bytesReadRPM;
    if (!ReadProcessMemory(hProc, (void*)(base + 0x3c), &sigAddr, 4, &bytesReadRPM)) {
        return false;
    }
    sigAddr += base;
    sigAddr -= 4;

    DWORD sig;
    if (!ReadProcessMemory(hProc, (void*)sigAddr, &sig, 4, &bytesReadRPM)) {
        return false;
    }
    return sig == thpracSig;
}

const THGameVersion* CheckOngoingGameByPID(DWORD pid, uintptr_t* pOutBase, HANDLE* pOutHandle) {
    auto hProc = OpenProcess(
        PROCESS_QUERY_INFORMATION | PROCESS_CREATE_THREAD | PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE,
        FALSE, pid);
    if (!hProc)
        return nullptr;

    defer({
        if (pOutHandle) {
            *pOutHandle = hProc;
        } else {
            CloseHandle(hProc);
        }
    });

    uintptr_t base = GetProcessModuleBase(hProc);
    if (!base) {
        return nullptr;
    }
    if (pOutBase) {
        *pOutBase = base;
    }

    // Check THPrac signature
    // If an error happens here, return early
    if (CheckTHPracSig(hProc, base)) {
        return nullptr;
    }

    auto exeSig = GetRemoteExeInfo(hProc, base);
    for (size_t i = 0; i < gGameVersionsCount; i++) {
        if (exeSig == gGameVersions[i].exeInfo) {
            return gGameVersions + i;
        }
    }
    return nullptr;
}

const wchar_t* allMutexNames[] = {
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

bool CheckIfAnyGame() {
    for (const wchar_t* mutexName : allMutexNames) {
        HANDLE hMutex = OpenMutexW(SYNCHRONIZE, FALSE, mutexName);
        if (hMutex) {
            CloseHandle(hMutex);
            return true;
        }
    }
    return false;
}

bool ApplyToProcById(DWORD pid) {
    uintptr_t base;
    HANDLE hProc;
    auto* sig = CheckOngoingGameByPID(pid, &base, &hProc);
    if (sig) {
        if (!WriteTHPracSig(hProc, base) || !LoadSelf(hProc)) {
            return false;
        }
    } else {
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
        uintptr_t base;
        HANDLE hProc = NULL;
        const THGameVersion* gameSig = CheckOngoingGameByPID(proc->UniqueProcessId, &base, &hProc);
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
        if (WriteTHPracSig(hProc, base) && LoadSelf(hProc)) {
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

uintptr_t LoadRemoteLibrary(HANDLE hProcess, const wchar_t* libName) {
    size_t l_Len = t_strlen(libName) * sizeof(wchar_t);

    LPVOID rBuf = VirtualAllocEx(hProcess, nullptr, l_Len, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (!rBuf) {
        return 0;
    }
    defer(VirtualFreeEx(hProcess, rBuf, 0, MEM_RELEASE));

    SIZE_T byteRet;
    WriteProcessMemory(hProcess, rBuf, libName, l_Len, &byteRet);

    HANDLE hrThread = CreateRemoteThread(hProcess, nullptr, 0, (LPTHREAD_START_ROUTINE)(UINT_PTR)LoadLibraryW, rBuf, 0, nullptr);
    if (!hrThread) {
        return 0;
    }
    defer(CloseHandle(hrThread));

    WaitForSingleObject(hrThread, INFINITE);

    DWORD ret = 0;
    GetExitCodeThread(hrThread, &ret);

    return ret;
}

extern wchar_t thprac_dll_path[];

bool LoadSelf(HANDLE hProcess) {
    return LoadRemoteLibrary(hProcess, thprac_dll_path);
}

bool TryLoadVpatch(HANDLE hProcess, const wchar_t* exeDir) {
    auto& c = CurrentPeb()->ProcessParameters->CurrentDirectory.DosPath;
    std::wstring curdir_bak(c.Buffer, c.Length / sizeof(wchar_t));

    SetCurrentDirectoryW(exeDir);

    WIN32_FIND_DATAW find;
    HANDLE hFind = FindFirstFileW(L"vpatch*.dll", &find);
    if (!hFind) {
        return false;
    }

    do {
        if (CheckDLLFunction(find.cFileName, "_Initialize@4") && LoadRemoteLibrary(hProcess, find.cFileName)) {
            FindClose(hFind);
            SetCurrentDirectoryW(curdir_bak.c_str());
            return true;
        }
    } while (FindNextFileW(hFind, &find));

    FindClose(hFind);
    SetCurrentDirectoryW(curdir_bak.c_str());
    return false;
}

bool RunGame(const wchar_t* exeFn, wchar_t* cmdLine, uint32_t flags) {
    STARTUPINFOW si = {
        .cb = sizeof(si),
    };
    PROCESS_INFORMATION pi = {};

    const wchar_t* file_spec = std::max(wcsrchr(exeFn, L'\\'), wcsrchr(exeFn, L'/'));
    BOOL ret;

    std::wstring exeDir;
    if (file_spec) {
        exeDir = std::wstring(exeFn, file_spec);
        ret = CreateProcessW(exeFn, cmdLine, nullptr, nullptr, FALSE, CREATE_SUSPENDED, nullptr, exeDir.c_str(), &si, &pi);
    } else {
        ret = CreateProcessW(exeFn, cmdLine, nullptr, nullptr, FALSE, CREATE_SUSPENDED, nullptr, nullptr, &si, &pi);
    }

    if (!ret) {
        return false;
    }

    bool res = false;
    if (flags & RUN_FLAG_OILP) {
        size_t pos = exeDir.length();
        exeDir.append(L"\\openinputlagpatch.dll");
        res = LoadRemoteLibrary(pi.hProcess, exeDir.c_str());
        exeDir.resize(pos);
    }
    if (!res && (flags & RUN_FLAG_VPATCH)) {
        TryLoadVpatch(pi.hProcess, exeDir.c_str());
    }

    if (flags & RUN_FLAG_THPRAC) {
        if (WriteTHPracSig(pi.hProcess, GetProcessModuleBase(pi.hProcess))) {
            LoadSelf(pi.hProcess);
        }
    }

    ResumeThread(pi.hThread);

    // TODO: determine if these should be returned
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return true;
}
