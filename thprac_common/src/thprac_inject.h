#pragma once
#include <Windows.h>

#include <stdint.h>
#include "thprac_identify.h"

enum RunFlags {
    RUN_FLAG_THPRAC = 1 << 0,
    RUN_FLAG_SKIP_IDENTIFY = 1 << 1,
    RUN_FLAG_ALWAYS = 1 << 2,
    RUN_FLAG_OILP = 1 << 3,
    RUN_FLAG_VPATCH = 1 << 4,
};

uintptr_t GetProcessModuleBase(HANDLE hProc);
const THGameVersion* CheckOngoingGameByPID(DWORD pid, uintptr_t* base, HANDLE* pOutHandle);
bool CheckIfAnyGame();
const wchar_t* GetGameMutexName(THGameID game);
bool CheckTHPracSig(HANDLE hProc, uintptr_t base);
bool FindAndAttach(bool prompt_if_no_game, bool prompt_if_yes_game, THGameID id = ID_UNKNOWN);
bool WriteTHPracSig(HANDLE hProc, uintptr_t base);
bool LoadSelf(HANDLE hProcess);
bool ApplyToProcById(DWORD pid);
bool RunGame(const wchar_t* exeFn, wchar_t* cmdLine, uint32_t flags = 0xFFFFFFFF);
bool LoadRemoteLibrary(HANDLE hProcess, const wchar_t* libName);
