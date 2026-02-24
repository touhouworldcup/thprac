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

__declspec(align(16)) struct remote_init_config {
    bool newProcess = true;
    bool forbidVpatch = false;
    bool forbidOILP = false;
};

uintptr_t GetProcessModuleBase(HANDLE hProc);
const THGameVersion* CheckOngoingGameByPID(DWORD pid, uintptr_t* base, HANDLE* pOutHandle);
bool FindAndAttach(bool prompt_if_no_game, bool prompt_if_yes_game);
bool WriteTHPracSig(HANDLE hProc, uintptr_t base);
bool LoadSelf(HANDLE hProcess, remote_init_config* conf = nullptr);
bool ApplyToProcById(DWORD pid);
void RunGameWithTHPrac(const wchar_t* exeFn, wchar_t* cmdLine, remote_init_config* conf = nullptr);
remote_init_config* RemoteGetConfig();
bool CheckDLLFunction(const wchar_t* path, const char* funcName);

EXTERN_C IMAGE_DOS_HEADER __ImageBase;

}
