#pragma once
#include "utils/utils.h"
#include "thprac_gui_locale.h"
#include "thprac_launcher_games_def.h"
#include <cstdint>
#pragma warning(push)
#pragma warning(disable : 26451)
#pragma warning(disable : 26495)
#pragma warning(disable : 33010)
#include <rapidjson/document.h>
#pragma warning(pop)
#include <string>
#include <vector>

namespace THPrac {

bool GetExeInfo(void* exeBuffer, size_t exeSize, ExeSig& exeSigOut);
bool GetExeInfoEx(uintptr_t hProcess, uintptr_t base, ExeSig& exeSigOut);

bool LauncherGamesGuiUpd();
bool LauncherGamesDestroy();
void LauncherGamesGuiSwitch(const char* idStr);
void LauncherGamesForceReload();
bool LauncherGamesThcrapTest(std::wstring& dir);
bool LauncherGamesThcrapSetup();
void LauncherGamesThcrapCfgGet(std::vector<std::pair<std::string, bool>>& cfgVec, std::vector<GameRoll> gameVec[4]);
void LauncherGamesThcrapAdd(const char* gameId, const std::string& cfg, bool use_thprac, bool flush = false);
bool LauncherGamesThcrapLaunch();
std::vector<std::wstring> GetAllGamePaths();
}
