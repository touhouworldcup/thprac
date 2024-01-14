#pragma once
#define NOMINMAX
#include "thprac_games.h"
#include "thprac_launcher_games_def.h"
#include <Windows.h>
#include <tlhelp32.h>

#ifdef UNICODE
#undef Process32First
#undef Process32Next
#undef PROCESSENTRY32
#undef PPROCESSENTRY32
#undef LPPROCESSENTRY32
#endif

namespace THPrac {
uintptr_t GetGameModuleBase(HANDLE hProc);
bool CheckIfAnyGame();
bool WriteTHPracSig(HANDLE hProc, uintptr_t base);
bool FindOngoingGame(bool prompt = false);
bool FindAndRunGame(bool prompt = false);
}
