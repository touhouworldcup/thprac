#pragma once
#include "thprac_Launcher_games_def.h"

namespace THPrac {
enum LauncherTrigger {
    LAUNCHER_NOTHING = 0,
	LAUNCHER_RESTART,
	LAUNCHER_CLOSE,
    LAUNCHER_MINIMIZE,
    LAUNCHER_RESET,
};

int GuiLauncherMain();
void GuiLauncherMainSwitchTab(const char* tab);
void GuiLauncherMainTrigger(LauncherTrigger trigger);
void GuiLauncherLocaleInit();
// void DisableWinKey();
// void DisableDisableWinKey();
bool RunSteamGame(THGameSig& gameSig);
void GuiLauncherHotkeyInit();
void InitLocaleAndChore();
}
