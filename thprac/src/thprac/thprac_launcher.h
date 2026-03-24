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

}