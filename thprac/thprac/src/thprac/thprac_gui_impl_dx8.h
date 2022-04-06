#pragma once
/*
	This file is a modified copy of imgui's DirectX9 renderer implemention's header file.
	Changes made:

	1. Function "ImGui_ImplDX9_HookReset": Add declaration.
	2. Function "ImGui_ImplDX9_UnHookReset": Add declaration.
*/


#include <imgui.h>


struct IDirect3DDevice8;


namespace THPrac
{
	namespace Gui
	{
		bool ImplDX8Init(IDirect3DDevice8* device);
		void ImplDX8Check(IDirect3DDevice8* device);
		void ImplDX8Shutdown();
		void ImplDX8NewFrame();
		void ImplDX8RenderDrawData(ImDrawData* draw_data);

		void ImplDX8InvalidateDeviceObjects();
		bool ImplDX8CreateDeviceObjects();

		bool ImplDX8HookReset();
		bool ImplDX8UnHookReset();
	}
}
