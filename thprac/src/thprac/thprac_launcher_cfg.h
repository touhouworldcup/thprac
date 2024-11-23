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

}
