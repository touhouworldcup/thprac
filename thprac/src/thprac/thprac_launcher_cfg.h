#pragma once
#include <rapidjson/document.h>
#include <string>

namespace THPrac {

std::wstring LauncherGetDataDir();

bool LauncherCfgInit(bool noCreate = false);
bool LauncherCfgWrite();
rapidjson::Document& LauncherCfgGet();
void LauncherCfgClose();
void LauncherCfgReset();

bool LauncherSettingGet(const char* name, int& valueOut);
bool LauncherSettingGet(const char* name, bool& valueOut);
bool LauncherSettingGet(const char* name, float& valueOut);
bool LauncherSettingGet(const char* name, std::string& valueOut);
void LauncherSettingSet(const char* name, int& valueIn);
void LauncherSettingSet(const char* name, bool& valueIn);
void LauncherSettingSet(const char* name, float& valueIn);
void LauncherSettingSet(const char* name, std::string& valueIn);

void LauncherCfgGuiUpd();
void LauncherChkUpdPopup();
bool LauncherIsChkingUpd();
bool LauncherUpdDialog(bool confirmation = true);
void LauncherPeekUpd();

bool LauncherPreUpdate(wchar_t* pCmdLine);

}