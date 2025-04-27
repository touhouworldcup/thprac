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
bool CheckIfAnyGame();
bool FindOngoingGame(bool prompt_if_no_game = false, bool prompt_if_yes_game = true);
bool FindAndRunGame(bool prompt = false);
bool RunGameWithTHPrac(THGameSig& gameSig, const wchar_t* const name, bool withVpatch = true);
}
