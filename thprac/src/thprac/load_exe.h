#pragma once
#include <Windows.h>

namespace THPrac
{
bool LoadSelf(HANDLE hProcess, void* userdata = nullptr, size_t userdataSize = 0);
void** GetUserData();
}
