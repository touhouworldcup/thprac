#include "thprac_games_dx8.h"
#include "../3rdParties/d3d8/include/d3d8.h"
#include <cstdint>
#include <imgui.h>
#include "thprac_games_hooks.h"
#include "../3rdParties/d3d8/include/d3dx8tex.h"
#include "thprac_hook.h"
#pragma comment(lib, "d3dx8.lib")


namespace THPrac {
extern bool g_is_d3d_hooked;

struct Hook_Dx8
{
    HRESULT(WINAPI* real_Present8)(IDirect3DDevice8* thiz, CONST RECT* pSourceRect, CONST RECT* pDestRect, HWND hDestWindowOverride, CONST RGNDATA* pDirtyRegion);

}g_hook_dx8;

extern InputLatencyTestOpt g_input_latency_test;

HRESULT WINAPI Present_Changed8(IDirect3DDevice8* thiz, CONST RECT* pSourceRect, CONST RECT* pDestRect, HWND hDestWindowOverride, CONST RGNDATA* pDirtyRegion)
{
    auto res = g_hook_dx8.real_Present8(thiz, pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
    g_input_latency_test.Present();
    return res;
}

void HookDx8(DWORD device)
{
    g_is_d3d_hooked = true;
    HookVTable(*(void**)device, 15, Present_Changed8, (void**)&g_hook_dx8.real_Present8);
}

ImVec2 GetImageInfo8(ImTextureID textureID)
{
    D3DSURFACE_DESC desc;
    ((LPDIRECT3DTEXTURE8)textureID)->GetLevelDesc(0, &desc);
    return { (float)desc.Width, (float)desc.Height };
}

ImTextureID ReadImage8(DWORD device, LPCSTR fileName, LPCSTR srcData, size_t srcSz)
{
    ImTextureID tex = nullptr;
    if (fileName && GetFileAttributesA(fileName) != INVALID_FILE_ATTRIBUTES) {
        if (D3DXCreateTextureFromFileA((IDirect3DDevice8*)device, fileName, (LPDIRECT3DTEXTURE8*)&tex) != D3D_OK)
            tex = nullptr;
    }
    if (!tex && srcData) {
        if (D3DXCreateTextureFromFileInMemory((IDirect3DDevice8*)device, srcData, srcSz, (LPDIRECT3DTEXTURE8*)&tex) != D3D_OK)
            tex = nullptr;
    }
    return tex;
}


#pragma region Snapshot
namespace THSnapshot {
void* GetSnapshotData(IDirect3DDevice8* d3d8)
{
    IDirect3DSurface8* surface = nullptr;
    d3d8->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &surface);
    D3DLOCKED_RECT rect = {};
    surface->LockRect(&rect, nullptr, 0);

    void* bmp = malloc(0xE2000);
    uint8_t* bmp_write = (uint8_t*)bmp;
    for (int32_t i = 0x1DF; i >= 0; --i) {
        uint8_t* bmp_bits = ((uint8_t*)rect.pBits) + i * rect.Pitch;
        for (size_t j = 0; j < 0x280; ++j) {
            memcpy(bmp_write, bmp_bits, 3); // This *should* get optimized to byte/word MOVs
            bmp_bits += 4;
            bmp_write += 3;
        }
    }

    surface->UnlockRect();
    surface->Release();

    return bmp;
}
void Snapshot(IDirect3DDevice8* d3d8)
{
    wchar_t dir[] = L"snapshot/th000.bmp";
    HANDLE hFile;
    CreateDirectoryW(L"snapshot", nullptr);
    for (int i = 0; i < 1000; i++) {
        dir[13] = static_cast<wchar_t>(i % 10) + L'0';
        dir[12] = static_cast<wchar_t>((i % 100 - i % 10) / 10) + L'0';
        dir[11] = static_cast<wchar_t>((i - i % 100) / 100) + L'0';
        hFile = CreateFileW(dir, GENERIC_READ | GENERIC_WRITE, 0, nullptr, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (hFile != INVALID_HANDLE_VALUE)
            break;
    }
    if (hFile == INVALID_HANDLE_VALUE)
        return;

    auto header = "\x42\x4d\x36\x10\x0e\x00\x00\x00\x00\x00\x36\x00\x00\x00\x28\x00\x00\x00\x80\x02\x00\x00\xe0\x01\x00\x00\x01\x00\x18\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";
    void* bmp = GetSnapshotData(d3d8);
    DWORD bytesRead;
    WriteFile(hFile, header, 0x36, &bytesRead, nullptr);
    WriteFile(hFile, bmp, 0xE2000, &bytesRead, nullptr);
    free(bmp);

    CloseHandle(hFile);
}

}
#pragma endregion
}
