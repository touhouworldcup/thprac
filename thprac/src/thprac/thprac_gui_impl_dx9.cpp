/*
	This file is a modified copy of imgui's DirectX9 renderer implemention's  source file.
	Changes made:

	1. Includes: Renamed "imgui_impl_dx9.h".
	2. Includes: Added "thprac_hook.h".
	3. Function "ImGui_ImplDX9_HookReset": Add definition.
	4. Function "ImGui_ImplDX9_UnHookReset": Add definition.
	// Structure: Replaced "CUSTOMVERTEX" with "ImDrawVert". Removed "CUSTOMVERTEX".
	// Method "ImGui_ImplDX9_RenderDrawData": Simplified vertices repack to only zeroing the z-coordinate.

	Last official change: 2019-04-30.
*/

#include "thprac_gui_impl_dx9.h"
#include "thprac_hook.h"
#include <MinHook.h>
#include <d3d9.h>
#include <imgui.h>

namespace THPrac {
namespace Gui {
    struct CUSTOMVERTEX {
        float pos[3];
        D3DCOLOR col;
        float uv[2];
    };
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1)

    // DirectX data
    static LPDIRECT3DDEVICE9 g_pd3dDevice = NULL;
    static LPDIRECT3DVERTEXBUFFER9 g_pVB = NULL;
    static LPDIRECT3DINDEXBUFFER9 g_pIB = NULL;
    static LPDIRECT3DTEXTURE9 g_FontTexture = NULL;
    static int g_VertexBufferSize = 5000, g_IndexBufferSize = 10000;

    // Helper functions
    static LPDIRECT3DSTATEBLOCK9 g_pOrigStateBlock = NULL;
    static D3DMATRIX g_pOrigWorld;
    static D3DMATRIX g_pOrigView;
    static D3DMATRIX g_pOrigProj;
    static bool ImplDX9CreateFontsTexture()
    {
        // Build texture atlas
        ImGuiIO& io = ImGui::GetIO();
        unsigned char* pixels;
        int width, height, bytes_per_pixel;
        io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height, &bytes_per_pixel);

        // Upload texture to graphics system
        g_FontTexture = NULL;
        if (g_pd3dDevice->CreateTexture(width, height, 1, D3DUSAGE_DYNAMIC, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &g_FontTexture, NULL) < 0)
            return false;
        D3DLOCKED_RECT tex_locked_rect;
        if (g_FontTexture->LockRect(0, &tex_locked_rect, NULL, 0) != D3D_OK)
            return false;
        for (int y = 0; y < height; y++)
            memcpy((unsigned char*)tex_locked_rect.pBits + tex_locked_rect.Pitch * y, pixels + (width * bytes_per_pixel) * y, (width * bytes_per_pixel));
        g_FontTexture->UnlockRect(0);

        // Store our identifier
        io.Fonts->TexID = (ImTextureID)g_FontTexture;

        return true;
    }
    static void ImplDX9SetupRenderState(ImDrawData* draw_data)
    {
        // Setup viewport
        D3DVIEWPORT9 vp;
        vp.X = vp.Y = 0;
        vp.Width = (DWORD)draw_data->DisplaySize.x;
        vp.Height = (DWORD)draw_data->DisplaySize.y;
        vp.MinZ = 0.0f;
        vp.MaxZ = 1.0f;
        g_pd3dDevice->SetViewport(&vp);

        // Setup render state: fixed-pipeline, alpha-blending, no face culling, no depth testing, shade mode (for gradient)
        g_pd3dDevice->SetPixelShader(NULL);
        g_pd3dDevice->SetVertexShader(NULL);
        g_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
        g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, false);
        g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, false);
        g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
        g_pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, false);
        g_pd3dDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
        g_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
        g_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
        g_pd3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, true);
        g_pd3dDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
        g_pd3dDevice->SetRenderState(D3DRS_FOGENABLE, false);
        g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
        g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
        g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
        g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
        g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
        g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
        g_pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
        g_pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);

        g_pd3dDevice->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);

        // Setup orthographic projection matrix
        // Our visible imgui space lies from draw_data->DisplayPos (top left) to draw_data->DisplayPos+data_data->DisplaySize (bottom right).
        // Being agnostic of whether <d3dx9.h> or <DirectXMath.h> can be used, we aren't relying on D3DXMatrixIdentity()/D3DXMatrixOrthoOffCenterLH() or DirectX::XMMatrixIdentity()/DirectX::XMMatrixOrthographicOffCenterLH()
        {
            float L = draw_data->DisplayPos.x + 0.5f;
            float R = draw_data->DisplayPos.x + draw_data->DisplaySize.x + 0.5f;
            float T = draw_data->DisplayPos.y + 0.5f;
            float B = draw_data->DisplayPos.y + draw_data->DisplaySize.y + 0.5f;
            D3DMATRIX mat_identity = { { { 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f } } };
            D3DMATRIX mat_projection = { { { 2.0f / (R - L), 0.0f, 0.0f, 0.0f,
                0.0f, 2.0f / (T - B), 0.0f, 0.0f,
                0.0f, 0.0f, 0.5f, 0.0f,
                (L + R) / (L - R), (T + B) / (B - T), 0.5f, 1.0f } } };
            g_pd3dDevice->SetTransform(D3DTS_WORLD, &mat_identity);
            g_pd3dDevice->SetTransform(D3DTS_VIEW, &mat_identity);
            g_pd3dDevice->SetTransform(D3DTS_PROJECTION, &mat_projection);
        }
    }
    static __forceinline bool ImplDX9StateBackup()
    {
        // Backup the DX9 state
        if (g_pd3dDevice->CreateStateBlock(D3DSBT_ALL, &g_pOrigStateBlock) < 0)
            return false;

        // Backup the DX9 transform (DX9 documentation suggests that it is included in the StateBlock but it doesn't appear to)
        g_pd3dDevice->GetTransform(D3DTS_WORLD, &g_pOrigWorld);
        g_pd3dDevice->GetTransform(D3DTS_VIEW, &g_pOrigView);
        g_pd3dDevice->GetTransform(D3DTS_PROJECTION, &g_pOrigProj);

        return true;
    }
    static __forceinline void ImplDX9StateRestore()
    {
        // Restore the DX9 transform
        g_pd3dDevice->SetTransform(D3DTS_WORLD, &g_pOrigWorld);
        g_pd3dDevice->SetTransform(D3DTS_VIEW, &g_pOrigView);
        g_pd3dDevice->SetTransform(D3DTS_PROJECTION, &g_pOrigProj);

        // Restore the DX9 state
        g_pOrigStateBlock->Apply();
        g_pOrigStateBlock->Release();
        g_pOrigStateBlock = NULL;
    }

    // Api functions
    void ImplDX9AdjustDispSize()
    {
        LPDIRECT3DSURFACE9 backBuffer;
        D3DSURFACE_DESC backBufferDesc;
        g_pd3dDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &backBuffer);
        backBuffer->GetDesc(&backBufferDesc);
        backBuffer->Release();

        auto& io = ::ImGui::GetIO();
        io.DisplaySize.x = (float)backBufferDesc.Width;
        io.DisplaySize.y = (float)backBufferDesc.Height;
    }
    bool ImplDX9Init(IDirect3DDevice9* device)
    {
        ImGuiIO& io = ImGui::GetIO();
        io.BackendRendererName = "thprac_gui_impl_dx9";

        g_pd3dDevice = device;
        g_pd3dDevice->AddRef();
        return true;
    }
    void ImplDX9Check(IDirect3DDevice9* device)
    {
        if (g_pd3dDevice != device) {
            ImplDX9InvalidateDeviceObjects();
            g_pd3dDevice->Release();
            ImplDX9Init(device);
            ImplDX9AdjustDispSize();
        }
    }
    void ImplDX9Shutdown()
    {
        ImplDX9InvalidateDeviceObjects();
        if (g_pd3dDevice) {
            g_pd3dDevice->Release();
            g_pd3dDevice = NULL;
        }
    }
    void ImplDX9NewFrame()
    {
        if (!g_FontTexture)
            ImplDX9CreateDeviceObjects();
    }
    void ImplDX9RenderDrawData(ImDrawData* draw_data)
    {
        // Avoid rendering when minimized
        if (draw_data->DisplaySize.x <= 0.0f || draw_data->DisplaySize.y <= 0.0f)
            return;

        // Create and grow buffers if needed
        if (!g_pVB || g_VertexBufferSize < draw_data->TotalVtxCount) {
            if (g_pVB) {
                g_pVB->Release();
                g_pVB = NULL;
            }
            g_VertexBufferSize = draw_data->TotalVtxCount + 5000;
            if (g_pd3dDevice->CreateVertexBuffer(g_VertexBufferSize * sizeof(CUSTOMVERTEX), D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, D3DFVF_CUSTOMVERTEX, D3DPOOL_DEFAULT, &g_pVB, NULL) < 0)
                return;
        }
        if (!g_pIB || g_IndexBufferSize < draw_data->TotalIdxCount) {
            if (g_pIB) {
                g_pIB->Release();
                g_pIB = NULL;
            }
            g_IndexBufferSize = draw_data->TotalIdxCount + 10000;
            if (g_pd3dDevice->CreateIndexBuffer(g_IndexBufferSize * sizeof(ImDrawIdx), D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, sizeof(ImDrawIdx) == 2 ? D3DFMT_INDEX16 : D3DFMT_INDEX32, D3DPOOL_DEFAULT, &g_pIB, NULL) < 0)
                return;
        }

        // Backup DX9 state
        if (!ImplDX9StateBackup())
            return;

        // Copy and convert all vertices into a single contiguous buffer, convert colors to DX9 default format.
        // FIXME-OPT: This is a waste of resource, the ideal is to use imconfig.h and
        //  1) to avoid repacking colors:   #define IMGUI_USE_BGRA_PACKED_COLOR
        //  2) to avoid repacking vertices: #define IMGUI_OVERRIDE_DRAWVERT_STRUCT_LAYOUT struct ImDrawVert { ImVec2 pos; float z; ImU32 col; ImVec2 uv; }
        CUSTOMVERTEX* vtx_dst;
        ImDrawIdx* idx_dst;
        if (g_pVB->Lock(0, (UINT)(draw_data->TotalVtxCount * sizeof(CUSTOMVERTEX)), (void**)&vtx_dst, D3DLOCK_DISCARD) < 0)
            return;
        if (g_pIB->Lock(0, (UINT)(draw_data->TotalIdxCount * sizeof(ImDrawIdx)), (void**)&idx_dst, D3DLOCK_DISCARD) < 0)
            return;
        for (int n = 0; n < draw_data->CmdListsCount; n++) {
            const ImDrawList* cmd_list = draw_data->CmdLists[n];
            const ImDrawVert* vtx_src = cmd_list->VtxBuffer.Data;
            for (int i = 0; i < cmd_list->VtxBuffer.Size; i++) {
                vtx_dst->pos[0] = vtx_src->pos.x;
                vtx_dst->pos[1] = vtx_src->pos.y;
                vtx_dst->pos[2] = 0.0f;
                vtx_dst->col = vtx_src->col;
                vtx_dst->uv[0] = vtx_src->uv.x;
                vtx_dst->uv[1] = vtx_src->uv.y;
                vtx_dst++;
                vtx_src++;
            }
            memcpy(idx_dst, cmd_list->IdxBuffer.Data, cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx));
            idx_dst += cmd_list->IdxBuffer.Size;
        }
        g_pVB->Unlock();
        g_pIB->Unlock();
        g_pd3dDevice->SetStreamSource(0, g_pVB, 0, sizeof(CUSTOMVERTEX));
        g_pd3dDevice->SetIndices(g_pIB);
        g_pd3dDevice->SetFVF(D3DFVF_CUSTOMVERTEX);

        // Setup desired DX state
        ImplDX9SetupRenderState(draw_data);

        // Render command lists
        int vtx_offset = 0;
        int idx_offset = 0;
        ImVec2 clip_off = draw_data->DisplayPos;
        for (int n = 0; n < draw_data->CmdListsCount; n++) {
            const ImDrawList* cmd_list = draw_data->CmdLists[n];
            for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++) {
                const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
                if (pcmd->UserCallback != NULL) {
                    // User callback, registered via ImDrawList::AddCallback()
                    // (ImDrawCallback_ResetRenderState is a special callback value used by the user to request the renderer to reset render state.)
                    if (pcmd->UserCallback == ImDrawCallback_ResetRenderState)
                        ImplDX9SetupRenderState(draw_data);
                    else
                        pcmd->UserCallback(cmd_list, pcmd);
                } else {
                    const RECT r = { (LONG)(pcmd->ClipRect.x - clip_off.x), (LONG)(pcmd->ClipRect.y - clip_off.y), (LONG)(pcmd->ClipRect.z - clip_off.x), (LONG)(pcmd->ClipRect.w - clip_off.y) };
                    const LPDIRECT3DTEXTURE9 texture = (LPDIRECT3DTEXTURE9)pcmd->TextureId;
                    g_pd3dDevice->SetTexture(0, texture);
                    g_pd3dDevice->SetScissorRect(&r);
                    g_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, vtx_offset, 0, (UINT)cmd_list->VtxBuffer.Size, idx_offset, pcmd->ElemCount / 3);
                }
                idx_offset += pcmd->ElemCount;
            }
            vtx_offset += cmd_list->VtxBuffer.Size;
        }

        // Restore DX9 state
        ImplDX9StateRestore();
    }
    bool ImplDX9CreateDeviceObjects()
    {
        if (!g_pd3dDevice)
            return false;
        if (!ImplDX9CreateFontsTexture())
            return false;
        return true;
    }
    void ImplDX9InvalidateDeviceObjects()
    {
        if (!g_pd3dDevice)
            return;
        if (g_pVB) {
            g_pVB->Release();
            g_pVB = NULL;
        }
        if (g_pIB) {
            g_pIB->Release();
            g_pIB = NULL;
        }
        if (g_FontTexture) {
            g_FontTexture->Release();
            g_FontTexture = NULL;
            ImGui::GetIO().Fonts->TexID = NULL;
        } // We copied g_pFontTextureView to io.Fonts->TexID so let's clear that as well.
    }

    // Added functions used by thprac
    static void* __thimgui_dx9_reset_hook = nullptr;
    static HRESULT __stdcall __ThImGui_DX9_Reset_HookFunc(void* dev, D3DPRESENT_PARAMETERS* param)
    {
        typedef decltype(__ThImGui_DX9_Reset_HookFunc)* PReset;

        ImplDX9InvalidateDeviceObjects();
        ImGui::GetIO().DisplaySize = { (float)param->BackBufferWidth, (float)param->BackBufferHeight };

        return (*(PReset)__thimgui_dx9_reset_hook)(dev, param);
    }
    bool ImplDX9HookReset()
    {
        if (!__thimgui_dx9_reset_hook) {
            MH_Initialize();
            MH_CreateHook((void*)(*(int32_t*)(*(int32_t*)g_pd3dDevice + 0x40)), (void*)__ThImGui_DX9_Reset_HookFunc, &__thimgui_dx9_reset_hook);
            MH_EnableHook((void*)(*(int32_t*)(*(int32_t*)g_pd3dDevice + 0x40)));
            return true;
        }
        return false;
    }
    bool ImplDX9UnHookReset()
    {
        if (__thimgui_dx9_reset_hook) {
            MH_RemoveHook((void*)(*(int32_t*)(*(int32_t*)g_pd3dDevice + 0x40)));
            __thimgui_dx9_reset_hook = nullptr;
            return true;
        }
        return false;
    }
}
}
