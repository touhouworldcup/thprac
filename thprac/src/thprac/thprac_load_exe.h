#pragma once
#include <Windows.h>
#include <stdint.h>

#include "thprac_identify.h"

namespace THPrac
{

PIMAGE_NT_HEADERS GetNtHeader(HMODULE hMod);
void* GetNtDataDirectory(HMODULE hMod, BYTE directory);


// DO NOT CHANGE THIS STRUCT WITHOUT RECOMPILING init_shellcode.cpp
__declspec(align(16)) struct remote_param {
    UINT_PTR pRemoteParamAddr;
    decltype(LoadLibraryW)* pLoadLibraryW;
    decltype(LoadLibraryA)* pLoadLibraryA;
    decltype(VirtualProtect)* pVirtualProtect;
    decltype(GetProcAddress)* pGetProcAddress;
    wchar_t sExePath[MAX_PATH];
    char sLoadErrDllName[MAX_PATH];
};

enum RunFlags {
    RUN_FLAG_OILP = 1 << 0,
    RUN_FLAG_VPATCH = 1 << 1,
};

uintptr_t GetProcessModuleBase(HANDLE hProc);
const THGameVersion* CheckOngoingGameByPID(DWORD pid, uintptr_t* base, HANDLE* pOutHandle);
bool FindAndAttach(bool prompt_if_no_game, bool prompt_if_yes_game);
bool WriteTHPracSig(HANDLE hProc, uintptr_t base);
bool LoadSelf(HANDLE hProcess);
bool ApplyToProcById(DWORD pid);
void RunGame(const wchar_t* exeFn, wchar_t* cmdLine, uint32_t flags = 0xFFFFFFFF, bool withThprac = true);
bool CheckDLLFunction(const wchar_t* path, const char* funcName);

EXTERN_C IMAGE_DOS_HEADER __ImageBase;

}
