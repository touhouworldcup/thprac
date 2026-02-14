#include "thprac_games_dx9.h"
#include "thprac_games_hooks.h"
#include <imgui.h>
#include <Windows.h>
#include <d3d9.h>
#include "thprac_hook.h"
#include <format>

namespace THPrac {
extern bool g_is_d3d_hooked;
extern DWORD* g_gameGuiDevice;

namespace SSS {
    extern bool g_flip_screen_y;
 }
struct Hook_Dx9 {
    HRESULT(WINAPI* real_Present9) (IDirect3DDevice9* thiz, CONST RECT* pSourceRect, CONST RECT* pDestRect, HWND hDestWindowOverride, CONST RGNDATA* pDirtyRegion);
    HRESULT(WINAPI* real_Reset9) (IDirect3DDevice9* thiz,D3DPRESENT_PARAMETERS* pPresentationParameters);

} g_hook_dx9;

extern InputLatencyTestOpt g_input_latency_test;

struct Flip_render
{
    bool resourcesInit = false;
    IDirect3DTexture9* pScreenTexture = nullptr;
    IDirect3DSurface9* pScreenSurface = nullptr;
    UINT screenWidth = 0;
    UINT screenHeight = 0;
}g_flip_render;

void FlipRender(IDirect3DDevice9* thiz)
{
    struct TLVertex {
        float x, y, z, rhw;
        float u, v;
    };
    const auto D3DFVF_TLVERTEX = (D3DFVF_XYZRHW | D3DFVF_TEX1);
    bool is_failed = false;
    if (is_failed)
        return;
    if (!g_flip_render.resourcesInit) {
        g_flip_render.resourcesInit = true;
        D3DVIEWPORT9 vp;
        thiz->GetViewport(&vp);
        g_flip_render.screenWidth = vp.Width;
        g_flip_render.screenHeight = vp.Height;

        if (FAILED(thiz->CreateTexture(g_flip_render.screenWidth, g_flip_render.screenHeight, 1,
                D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT,
                &g_flip_render.pScreenTexture, NULL))) {
            is_failed = true;
        }
        g_flip_render.pScreenTexture->GetSurfaceLevel(0, &g_flip_render.pScreenSurface);
    }
    if (g_flip_render.pScreenTexture && g_flip_render.pScreenSurface) {

        LPDIRECT3DSTATEBLOCK9 pOrigStateBlock = NULL;
        if (thiz->CreateStateBlock(D3DSBT_ALL, &pOrigStateBlock) < 0)
        {
            is_failed = true;
            return;
        }

        IDirect3DSurface9* pBackBuffer = nullptr;
        thiz->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer);

        thiz->StretchRect(pBackBuffer, NULL, g_flip_render.pScreenSurface, NULL, D3DTEXF_LINEAR);

        float w = (float)g_flip_render.screenWidth;
        float h = (float)g_flip_render.screenHeight;

        TLVertex verts[4] = {
            { -0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 1.0f },
            { w - 0.5f, -0.5f, 0.5f, 1.0f, 1.0f, 1.0f },
            { -0.5f, h - 0.5f, 0.5f, 1.0f, 0.0f, 0.0f },
            { w - 0.5f, h - 0.5f, 0.5f, 1.0f, 1.0f, 0.0f },
        };

        thiz->SetFVF(D3DFVF_TLVERTEX);
        thiz->SetTexture(0, g_flip_render.pScreenTexture);

        thiz->SetPixelShader      (NULL);
        thiz->SetVertexShader     (NULL);
        thiz->SetRenderState      (D3DRS_CULLMODE, D3DCULL_NONE);
        thiz->SetRenderState      (D3DRS_LIGHTING, false);
        thiz->SetRenderState      (D3DRS_ZENABLE, false);
        thiz->SetRenderState      (D3DRS_ALPHABLENDENABLE, false);
        thiz->SetRenderState      (D3DRS_ALPHATESTENABLE, false);
        thiz->SetRenderState      (D3DRS_BLENDOP, D3DBLENDOP_ADD);
        thiz->SetRenderState      (D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
        thiz->SetRenderState      (D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
        thiz->SetRenderState      (D3DRS_SCISSORTESTENABLE, true);
        thiz->SetRenderState      (D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
        thiz->SetRenderState      (D3DRS_FOGENABLE, false);
        thiz->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
        thiz->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
        thiz->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
        thiz->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
        thiz->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
        thiz->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
        thiz->SetSamplerState     (0, D3DSAMP_MINFILTER, D3DTEXF_NONE);
        thiz->SetSamplerState     (0, D3DSAMP_MAGFILTER, D3DTEXF_NONE);
        thiz->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);

        thiz->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, verts, sizeof(TLVertex));

        if (pBackBuffer)
            pBackBuffer->Release();
        if (pOrigStateBlock)
        {
            pOrigStateBlock->Apply();
            pOrigStateBlock->Release();
            pOrigStateBlock = nullptr;
        }
    }
}

ImVec2 GetImageInfo9(ImTextureID textureID)
{
    D3DSURFACE_DESC desc;
    ((LPDIRECT3DTEXTURE9)textureID)->GetLevelDesc(0, &desc);
    return { (float)desc.Width, (float)desc.Height };
}

HRESULT(WINAPI Reset_Changed9)(IDirect3DDevice9* thiz, D3DPRESENT_PARAMETERS* pPresentationParameters)
{
   
    g_flip_render.resourcesInit = false;
    if (g_flip_render.pScreenSurface){
        g_flip_render.pScreenSurface->Release();
        g_flip_render.pScreenSurface = nullptr;
    }
    return g_hook_dx9.real_Reset9(thiz, pPresentationParameters);
}

HRESULT WINAPI Present_Changed9(IDirect3DDevice9* thiz, CONST RECT* pSourceRect, CONST RECT* pDestRect, HWND hDestWindowOverride, CONST RGNDATA* pDirtyRegion)
{
    if (SSS::g_flip_screen_y) {
        FlipRender(thiz);
    }
    auto res = g_hook_dx9.real_Present9(thiz, pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
    g_input_latency_test.Present();
    return res;
}

void HookDx9(DWORD device)
{
    g_is_d3d_hooked = true;
    HookVTable(*(void**)device, 16, Reset_Changed9, (void**)&g_hook_dx9.real_Reset9);
    HookVTable(*(void**)device, 17, Present_Changed9, (void**)&g_hook_dx9.real_Present9);
}

ImTextureID ReadImage9(DWORD device, LPCSTR fileName, LPCSTR srcData, size_t srcSz)
{
    ImTextureID tex = nullptr;
    static auto hd3dx9 = LoadLibraryA("d3dx9_43.dll");
    if (hd3dx9) {
        auto pCreateTexture = (HRESULT(WINAPI*)(DWORD, LPCSTR, DWORD))GetProcAddress(hd3dx9, "D3DXCreateTextureFromFileA");
        if (fileName && GetFileAttributesA(fileName) != INVALID_FILE_ATTRIBUTES) {
            if (pCreateTexture && pCreateTexture(device, fileName, (DWORD)&tex) != D3D_OK)
                tex = nullptr;
        }
        if (!tex && srcData) {
            auto pCreateTextureFromMemory = (HRESULT(WINAPI*)(DWORD, LPCSTR, DWORD, DWORD))GetProcAddress(hd3dx9, "D3DXCreateTextureFromFileInMemory");
            if (pCreateTextureFromMemory && pCreateTextureFromMemory(device, srcData, srcSz, (DWORD)&tex) != D3D_OK)
                tex = nullptr;
        }
    }
    return tex;
}

int(__stdcall* g_orig_D3DXLoadSurfaceFromSurface)(
    LPDIRECT3DSURFACE9 pDestSurface,
    const PALETTEENTRY* pDestPalette,
    const RECT* pDestRect,
    LPDIRECT3DSURFACE9 pSrcSurface,
    const PALETTEENTRY* pSrcPalette,
    const RECT* pSrcRect,
    DWORD Filter,
    D3DCOLOR ColorKey);
int __stdcall D3DXLoadSurfaceFromSurface_Changed(
    LPDIRECT3DSURFACE9 pDestSurface,
    const PALETTEENTRY* pDestPalette,
    const RECT* pDestRect,
    LPDIRECT3DSURFACE9 pSrcSurface,
    const PALETTEENTRY* pSrcPalette,
    const RECT* pSrcRect,
    DWORD Filter,
    D3DCOLOR ColorKey)
{
  
    int res;
    D3DSURFACE_DESC desc;
    LPDIRECT3DSURFACE9 surface2;

    if (FAILED(pSrcSurface->GetDesc(&desc)))
        return g_orig_D3DXLoadSurfaceFromSurface(pDestSurface, pDestPalette, pDestRect, pSrcSurface, pSrcPalette, pSrcRect, Filter, ColorKey);
    surface2 = nullptr;
    IDirect3DDevice9* device = *(IDirect3DDevice9**)g_gameGuiDevice;
    if (FAILED(device->CreateOffscreenPlainSurface(desc.Width, desc.Height, desc.Format, D3DPOOL_SYSTEMMEM, &surface2, NULL))) {
        return g_orig_D3DXLoadSurfaceFromSurface(pDestSurface, pDestPalette, pDestRect, pSrcSurface, pSrcPalette, pSrcRect, Filter, ColorKey);
    }
    device->GetRenderTargetData(pSrcSurface, surface2);
    res = g_orig_D3DXLoadSurfaceFromSurface(pDestSurface, pDestPalette, pDestRect, surface2, pSrcPalette, pSrcRect, Filter, ColorKey);
    surface2->Release();
    return res;
}

void ESC_Fix()
{
    HookIAT(GetModuleHandle(NULL), "d3dx9_43.dll", "D3DXLoadSurfaceFromSurface", D3DXLoadSurfaceFromSurface_Changed, (void**)&g_orig_D3DXLoadSurfaceFromSurface);
}



}