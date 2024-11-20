#pragma once
#include <Windows.h>

namespace THPrac
{

// DO NOT CHANGE THIS STRUCT WITHOUT RECOMPILING init_shellcode.cpp
struct remote_param {
    LPVOID pUserData;
    PUINT8 pAddrOfUserData;
    decltype(LoadLibraryW)* pLoadLibraryW;
    decltype(LoadLibraryA)* pLoadLibraryA;
    decltype(VirtualProtect)* pVirtualProtect;
    decltype(GetProcAddress)* pGetProcAddress;
    decltype(GetLastError)* pGetLastError;
    wchar_t sExePath[MAX_PATH];
    char sLoadErrDllName[MAX_PATH];
};

// GetLastError return value + an additional error code packed in a 32 bit integer
// Return value of InjectShellcode in inject_shellcode.cpp
struct InjectResult {
    // Appending to this enum is fine, but if you reorder any of the fields, recompile inject_shellcode.cpp
    enum : WORD {
        Ok = 0,
        LoadError,
        RelocationError,
    } error;
    WORD lastError;
};
uintptr_t GetGameModuleBase(HANDLE hProc);
bool WriteTHPracSig(HANDLE hProc, uintptr_t base);
bool LoadSelf(HANDLE hProcess, void* userdata = nullptr, size_t userdataSize = 0);
void** GetUserData();
}
