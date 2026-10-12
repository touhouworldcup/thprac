#pragma once
#include <Windows.h>
#include <stdint.h>

#include "thprac_identify.h"
#include <string_view>

enum RunFlags {
    RUN_FLAG_THPRAC = 1 << 0,
    RUN_FLAG_SKIP_IDENTIFY = 1 << 1,
    RUN_FLAG_ALWAYS = 1 << 2,
    RUN_FLAG_OILP = 1 << 3,
    RUN_FLAG_VPATCH = 1 << 4,
};

// Pointers in both of these strucs need to explicitly be 32/64 bit
// to ensure the layout stays consistent on both architectures.
struct alignas(16) RemoteParam64 {
    uint8_t shellcode[68];
    uint32_t flags;
    uint64_t LoadLibraryW_addr;
    uint64_t GetProcAddress_addr;
    uint64_t FreeLibraryAndExitThread_addr;
    wchar_t dllPath[];
};

// Function pointers are embedded inside the shellcode directly
// Previous shellcode uses these pointers to do absolute calls
// by pulling the address from a rip relative memory location
struct alignas(16) RemoteParam32 {
    uint8_t shellcode[48];
    uint32_t flags;
    uint32_t LoadLibraryW_addr;
    uint32_t GetProcAddress_addr;
    uint32_t FreeLibraryAndExitThread_addr;
    wchar_t dllPath[];
};

#if TH_X86
#define RemoteParamNative RemoteParam32
#elif TH_X64
#define RemoteParamNative RemoteParam64
#endif

const THGameVersion* CheckOngoingGameByPID(DWORD pid, uint64_t* pOutBase, HANDLE* pOutHandle);
bool ApplyToProcById(DWORD pid);
bool FindAndAttach(bool prompt_if_no_game, bool prompt_if_yes_game, THGameID gameID = ID_UNKNOWN);
bool RunGame(const wchar_t* exeFn, wchar_t* cmdLine, uint32_t flags, uintptr_t bits);