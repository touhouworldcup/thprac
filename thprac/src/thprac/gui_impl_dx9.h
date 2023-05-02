#pragma once
/*
	This file is a modified copy of imgui's DirectX9 renderer implemention's  header file.
	Changes made:

	1. Function "ImGui_ImplDX9_HookReset": Add declaration.
	2. Function "ImGui_ImplDX9_UnHookReset": Add declaration.
*/

#include <imgui.h>

struct IDirect3DDevice9;

namespace THPrac {
namespace Gui {
    bool ImplDX9Init(IDirect3DDevice9* device);
    void ImplDX9Check(IDirect3DDevice9* device);
    void ImplDX9Shutdown();
    void ImplDX9NewFrame();
    void ImplDX9RenderDrawData(ImDrawData* draw_data);
    void ImplDX9AdjustDispSize();

    void ImplDX9InvalidateDeviceObjects();
    bool ImplDX9CreateDeviceObjects();

    bool ImplDX9HookReset();
    bool ImplDX9UnHookReset();
}
}
