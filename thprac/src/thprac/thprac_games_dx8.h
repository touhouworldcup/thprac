#pragma once
#include <Windows.h>
#include <imgui.h>
struct IDirect3DDevice8;

namespace THPrac {
ImTextureID ReadImage8(DWORD device, LPCSTR fileName, LPCSTR srcData, size_t srcSz);
ImVec2 GetImageInfo8(ImTextureID textureID);
void HookDx8(DWORD device);


#pragma region Snapshot
namespace THSnapshot {
    
void* GetSnapshotData(IDirect3DDevice8* d3d8);
void Snapshot(IDirect3DDevice8* d3d8);

}
#pragma endregion
}