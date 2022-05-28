#pragma once
/*
	This file is a modified copy of imgui's Win32 platform implemention's header file.
	Changes made:

	1. Function "ImGui_ImplWin32_WndProcHandler": Uncommented declaration.
	2. Function "ImGui_ImplWin32_HookWndProc": Add declaration.
	3. Function "ImGui_ImplWin32_UnHookWndProc": Add declaration.
	4. Function "ImGui_ImplWin32_CheckFullScreen": Add declaration.
*/
#include <string>

namespace THPrac
{
	namespace Gui
	{
		bool ImplWin32Init(void* hwnd);
		void ImplWin32Check(void* hwnd);
		void ImplWin32Shutdown();
		void ImplWin32NewFrame(bool mouseMapping = true);

		bool ImplWin32HookWndProc(void* wndproc_addr = nullptr);
		bool ImplWin32UnHookWndProc();
		bool ImplWin32CheckFullScreen();
		bool ImplWin32CheckForeground();

		int ImplWin32GetKey(int vk);
		int ImplWin32GetKeyFrame(int vk);
		int ImplWin32ScanForUserHotkey(std::string* hotkeyStr = nullptr);
		int ImplWin32CheckHotkey(int hotkey);

		void ImplWin32SetNoClose(bool noClose);
	}
}