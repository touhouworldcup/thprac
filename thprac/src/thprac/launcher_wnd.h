#pragma once
#include <imgui.h>
#include <string>

namespace THPrac {
int LauncherWndInit(unsigned int width, unsigned int height, unsigned int maxWidth, unsigned int maxHeight, unsigned int widthCurrent, unsigned int heightCurrent);
bool LauncherWndNewFrame();
bool LauncherWndEndFrame(ImVec2& wndPos, ImVec2& wndSize, bool canMove = false);
bool LauncherWndMinimize();
bool LauncherWndShutdown();
ImVec2 LauncherWndGetSize();
float LauncherWndGetScale();
std::wstring LauncherWndFolderSelect(const wchar_t* title = L"Browse for folder...");
std::wstring LauncherWndFileSelect(const wchar_t* title = L"Browse for file...", const wchar_t* filter = nullptr);
int LauncherWndMsgBox(const wchar_t* title, const wchar_t* text, int flag);
}
