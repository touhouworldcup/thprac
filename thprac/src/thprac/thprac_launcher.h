#pragma once
#define NOMINMAX
#include "utils/utils.h"
#include "utils/wininternal.h"
#include <d3d9.h>
#include <algorithm>

#include <ImGui.h>

#include "thprac_cfg.h"
#include "thprac_log.h"
#include "thprac_gui_impl_win32.h"
#include "thprac_gui_impl_dx9.h"
#include "thprac_gui_locale.h"
#include "thprac_gui_input.h"

namespace THPrac {

typedef HRESULT(WINAPI* PSetProcessDpiAwareness)(DWORD value);
typedef HRESULT(WINAPI* PGetDpiForMonitor)(HMONITOR hmonitor, DWORD dpiType, UINT* dpiX, UINT* dpiY);

void LoadGamesJson();
void SaveGamesJson();
void LauncherGamesMain();

void RandomShotRollUI();
void RandomGameRollUI();

extern void (*toolFunc)();
extern bool goToGamesPage;

enum AFTER_LAUNCH {
    LAUNCH_MINIMIZE = 0,
    LAUNCH_CLOSE = 1,
    LAUNCH_NOTHING = 2,
};

enum APPLY_THPRAC_DEFAULT {
    APPLY_THPRAC_KEEP_STATE = 0,
    APPLY_THPRAC_DEFAULT_OPEN = 1,
    APPLY_THPRAC_DEFAULT_CLOSE = 2,
};

struct LauncherSettings {
    AFTER_LAUNCH after_launch = LAUNCH_MINIMIZE;
    APPLY_THPRAC_DEFAULT apply_thprac_default = APPLY_THPRAC_KEEP_STATE;
    int filter_default = 0; // unimplemented
    bool auto_default_launch = false;
};

extern LauncherSettings launcherSettings;

}