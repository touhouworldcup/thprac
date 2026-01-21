#pragma once
#include <Windows.h>
#include <imgui.h>

struct IDirect3DDevice9;
namespace THPrac {
ImTextureID ReadImage9(DWORD device, LPCSTR fileName, LPCSTR srcData, size_t srcSz);
ImVec2 GetImageInfo9(ImTextureID textureID);
void HookDx9(DWORD device);
void FlipRender(IDirect3DDevice9* thiz);

}