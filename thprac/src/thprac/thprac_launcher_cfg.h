#pragma once
#pragma warning(push)
#pragma warning(disable : 26451)
#pragma warning(disable : 26495)
#pragma warning(disable : 33010)
#include <rapidjson/document.h>
#pragma warning(pop)
#include <string>
namespace THPrac {

std::wstring LauncherGetDataDir();

bool LauncherCfgInit(bool noCreate = false);
bool LauncherCfgWrite();
rapidjson::Document& LauncherCfgGet();
void LauncherCfgClose();
void LauncherCfgReset();

bool LauncherSettingGet(const char* name, int& valueOut);
bool LauncherSettingRemove(const char* name);

bool LauncherSettingGet(const char* name, unsigned int& valueOut);
bool LauncherSettingGet(const char* name, bool& valueOut);
bool LauncherSettingGet(const char* name, float& valueOut);
bool LauncherSettingGet(const char* name, const char*& valueOut);
bool LauncherSettingGet(const char* name, std::string& valueOut);
bool LauncherSettingGet(const char* name, std::array<int, 2>& valueOut);
void LauncherSettingGet_KeyBind();

void LauncherSettingSet(const char* name, int& valueIn);
void LauncherSettingSet(const char* name, unsigned int& valueIn);
void LauncherSettingSet(const char* name, bool& valueIn);
void LauncherSettingSet(const char* name, float& valueIn);
void LauncherSettingSet(const char* name, const char* valueIn);
void LauncherSettingSet(const char* name, const std::string& valueIn);
void LauncherSettingSet_KeyBind();

void LauncherCfgGuiUpd();
void LauncherChkUpdPopup();
bool LauncherIsChkingUpd();
bool LauncherUpdDialog(bool confirmation = true);
void LauncherPeekUpd();

bool LauncherPreUpdate(wchar_t* pCmdLine);

bool SetTheme(int theme, const wchar_t* userThemeName = nullptr);

int64_t LauncherGetGameTime();
void LauncherSetGameTime(int64_t timens);
void LauncherCloseHotkeyRebindListeners();

 enum Live2D_State {
    Normal = 0,
    Miss,
    Bomb,
    Hyper,
    BorderBreak,
    Dying, // life < life_cap
    Release,
    N_Live2D_State
};

struct Live2DOption {
    int show_time_ms = 5000;
    int life_cap = 0;
    Live2D_State curr_state = Live2D_State::Normal;
    Live2D_State next_state = Live2D_State::Normal;
    uint32_t last_time_ms = 1e9;
    int32_t VKs[Live2D_State::N_Live2D_State];
};

}
