// dear imgui: Renderer Backend for DirectX9
// This needs to be used along with a Platform Backend (e.g. Win32)

// Implemented features:
//  [X] Renderer: User texture binding. Use 'LPDIRECT3DTEXTURE9' as ImTextureID. Read the FAQ about ImTextureID!
//  [X] Renderer: Support for large meshes (64k+ vertices) with 16-bit indices.

// You can copy and use unmodified imgui_impl_* files in your project. See examples/ folder for examples of using this.
// If you are new to Dear ImGui, read documentation from the docs/ folder + read the top of imgui.cpp.
// Read online: https://github.com/ocornut/imgui/tree/master/docs

// CHANGELOG
// (minor and older changes stripped away, please see git history for details)
//  2021-03-03: DirectX9: Added support for IMGUI_USE_BGRA_PACKED_COLOR in user's imconfig file.
//  2021-02-18: DirectX9: Change blending equation to preserve alpha in output buffer.
//  2019-05-29: DirectX9: Added support for large mesh (64K+ vertices), enable ImGuiBackendFlags_RendererHasVtxOffset flag.
//  2019-04-30: DirectX9: Added support for special ImDrawCallback_ResetRenderState callback to reset render state.
//  2019-03-29: Misc: Fixed erroneous assert in ImGui_ImplDX9_InvalidateDeviceObjects().
//  2019-01-16: Misc: Disabled fog before drawing UI's. Fixes issue #2288.
//  2018-11-30: Misc: Setting up io.BackendRendererName so it can be displayed in the About Window.
//  2018-06-08: Misc: Extracted imgui_impl_dx9.cpp/.h away from the old combined DX9+Win32 example.
//  2018-06-08: DirectX9: Use draw_data->DisplayPos and draw_data->DisplaySize to setup projection matrix and clipping rectangle.
//  2018-05-07: Render: Saving/restoring Transform because they don't seem to be included in the StateBlock. Setting shading mode to Gouraud.
//  2018-02-16: Misc: Obsoleted the io.RenderDrawListsFn callback and exposed ImGui_ImplDX9_RenderDrawData() in the .h file so you can call it yourself.
//  2018-02-06: Misc: Removed call to ImGui::Shutdown() which is not available from 1.60 WIP, user needs to call CreateContext/DestroyContext themselves.

#include "imgui.h"
#include "imgui_impl_dx9.h"

// DirectX
#define CINTERFACE
#include <d3d9.h>

// DirectX data
static IDirect3DDevice9*       g_pd3dDevice = NULL;
static IDirect3DVertexBuffer9* g_pVB = NULL;
static IDirect3DIndexBuffer9*  g_pIB = NULL;
static IDirect3DTexture9*      g_FontTexture = NULL;
static int                     g_VertexBufferSize = 5000, g_IndexBufferSize = 10000;

decltype(IDirect3DDevice9Vtbl::Reset) __thimgui_dx9_reset_orig = NULL;

struct CUSTOMVERTEX
{
    float    pos[3];
    D3DCOLOR col;
    float    uv[2];
};
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1)

#ifdef IMGUI_USE_BGRA_PACKED_COLOR
#define IMGUI_COL_TO_DX9_ARGB(_COL)     (_COL)
#else
#define IMGUI_COL_TO_DX9_ARGB(_COL)     (((_COL) & 0xFF00FF00) | (((_COL) & 0xFF0000) >> 16) | (((_COL) & 0xFF) << 16))
#endif

static void ImGui_ImplDX9_SetupRenderState(ImDrawData* draw_data)
{
    // Setup viewport
    D3DVIEWPORT9 vp;
    vp.X = vp.Y = 0;
    vp.Width = (DWORD)draw_data->DisplaySize.x;
    vp.Height = (DWORD)draw_data->DisplaySize.y;
    vp.MinZ = 0.0f;
    vp.MaxZ = 1.0f;
    IDirect3DDevice9_SetViewport(g_pd3dDevice, &vp);

    // Setup render state: fixed-pipeline, alpha-blending, no face culling, no depth testing, shade mode (for gradient)
    IDirect3DDevice9_SetPixelShader(g_pd3dDevice, NULL);
    IDirect3DDevice9_SetVertexShader(g_pd3dDevice, NULL);
    IDirect3DDevice9_SetRenderState(g_pd3dDevice, D3DRS_CULLMODE, D3DCULL_NONE);
    IDirect3DDevice9_SetRenderState(g_pd3dDevice, D3DRS_LIGHTING, FALSE);
    IDirect3DDevice9_SetRenderState(g_pd3dDevice, D3DRS_ZENABLE, FALSE);
    IDirect3DDevice9_SetRenderState(g_pd3dDevice, D3DRS_ALPHABLENDENABLE, TRUE);
    IDirect3DDevice9_SetRenderState(g_pd3dDevice, D3DRS_ALPHATESTENABLE, FALSE);
    IDirect3DDevice9_SetRenderState(g_pd3dDevice, D3DRS_BLENDOP, D3DBLENDOP_ADD);
    IDirect3DDevice9_SetRenderState(g_pd3dDevice, D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    IDirect3DDevice9_SetRenderState(g_pd3dDevice, D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
    IDirect3DDevice9_SetRenderState(g_pd3dDevice, D3DRS_SEPARATEALPHABLENDENABLE, TRUE);
    IDirect3DDevice9_SetRenderState(g_pd3dDevice, D3DRS_SRCBLENDALPHA, D3DBLEND_ONE);
    IDirect3DDevice9_SetRenderState(g_pd3dDevice, D3DRS_DESTBLENDALPHA, D3DBLEND_INVSRCALPHA);
    IDirect3DDevice9_SetRenderState(g_pd3dDevice, D3DRS_SCISSORTESTENABLE, TRUE);
    IDirect3DDevice9_SetRenderState(g_pd3dDevice, D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
    IDirect3DDevice9_SetRenderState(g_pd3dDevice, D3DRS_FOGENABLE, FALSE);
    IDirect3DDevice9_SetTextureStageState(g_pd3dDevice, 0, D3DTSS_COLOROP, D3DTOP_MODULATE);
    IDirect3DDevice9_SetTextureStageState(g_pd3dDevice, 0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    IDirect3DDevice9_SetTextureStageState(g_pd3dDevice, 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
    IDirect3DDevice9_SetTextureStageState(g_pd3dDevice, 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
    IDirect3DDevice9_SetTextureStageState(g_pd3dDevice, 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    IDirect3DDevice9_SetTextureStageState(g_pd3dDevice, 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
    IDirect3DDevice9_SetSamplerState(g_pd3dDevice, 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
    IDirect3DDevice9_SetSamplerState(g_pd3dDevice, 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);

    // Setup orthographic projection matrix
    // Our visible imgui space lies from draw_data->DisplayPos (top left) to draw_data->DisplayPos+data_data->DisplaySize (bottom right). DisplayPos is (0,0) for single viewport apps.
    // Being agnostic of whether <d3dx9.h> or <DirectXMath.h> can be used, we aren't relying on D3DXMatrixIdentity()/D3DXMatrixOrthoOffCenterLH() or DirectX::XMMatrixIdentity()/DirectX::XMMatrixOrthographicOffCenterLH()
    {
        float L = draw_data->DisplayPos.x + 0.5f;
        float R = draw_data->DisplayPos.x + draw_data->DisplaySize.x + 0.5f;
        float T = draw_data->DisplayPos.y + 0.5f;
        float B = draw_data->DisplayPos.y + draw_data->DisplaySize.y + 0.5f;
        D3DMATRIX mat_identity = { { { 1.0f, 0.0f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f, 0.0f,  0.0f, 0.0f, 1.0f, 0.0f,  0.0f, 0.0f, 0.0f, 1.0f } } };
        D3DMATRIX mat_projection =
        { { {
            2.0f/(R-L),   0.0f,         0.0f,  0.0f,
            0.0f,         2.0f/(T-B),   0.0f,  0.0f,
            0.0f,         0.0f,         0.5f,  0.0f,
            (L+R)/(L-R),  (T+B)/(B-T),  0.5f,  1.0f
        } } };
        IDirect3DDevice9_SetTransform(g_pd3dDevice, D3DTS_WORLD, &mat_identity);
        IDirect3DDevice9_SetTransform(g_pd3dDevice, D3DTS_VIEW, &mat_identity);
        IDirect3DDevice9_SetTransform(g_pd3dDevice, D3DTS_PROJECTION, &mat_projection);
    }
}

// Render function.
void ImGui_ImplDX9_RenderDrawData(ImDrawData* draw_data)
{
    // Avoid rendering when minimized
    if (draw_data->DisplaySize.x <= 0.0f || draw_data->DisplaySize.y <= 0.0f)
        return;

    // Create and grow buffers if needed
    if (!g_pVB || g_VertexBufferSize < draw_data->TotalVtxCount)
    {
        if (g_pVB) { IDirect3DVertexBuffer9_Release(g_pVB); g_pVB = NULL; }
        g_VertexBufferSize = draw_data->TotalVtxCount + 5000;
        if (IDirect3DDevice9_CreateVertexBuffer(g_pd3dDevice, g_VertexBufferSize * sizeof(CUSTOMVERTEX), D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, D3DFVF_CUSTOMVERTEX, D3DPOOL_DEFAULT, &g_pVB, NULL) < 0)
            return;
    }
    if (!g_pIB || g_IndexBufferSize < draw_data->TotalIdxCount)
    {
        if (g_pIB) { IDirect3DIndexBuffer9_Release(g_pIB); g_pIB = NULL; }
        g_IndexBufferSize = draw_data->TotalIdxCount + 10000;
        if (IDirect3DDevice9_CreateIndexBuffer(g_pd3dDevice, g_IndexBufferSize * sizeof(ImDrawIdx), D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, sizeof(ImDrawIdx) == 2 ? D3DFMT_INDEX16 : D3DFMT_INDEX32, D3DPOOL_DEFAULT, &g_pIB, NULL) < 0)
            return;
    }

    // Backup the DX9 state
    IDirect3DStateBlock9* d3d9_state_block = NULL;
    if (IDirect3DDevice9_CreateStateBlock(g_pd3dDevice, D3DSBT_ALL, &d3d9_state_block) < 0)
        return;

    // Backup the DX9 transform (DX9 documentation suggests that it is included in the StateBlock but it doesn't appear to)
    D3DMATRIX last_world, last_view, last_projection;
    IDirect3DDevice9_GetTransform(g_pd3dDevice, D3DTS_WORLD, &last_world);
    IDirect3DDevice9_GetTransform(g_pd3dDevice, D3DTS_VIEW, &last_view);
    IDirect3DDevice9_GetTransform(g_pd3dDevice, D3DTS_PROJECTION, &last_projection);

    // Copy and convert all vertices into a single contiguous buffer, convert colors to DX9 default format.
    // FIXME-OPT: This is a minor waste of resource, the ideal is to use imconfig.h and
    //  1) to avoid repacking colors:   #define IMGUI_USE_BGRA_PACKED_COLOR
    //  2) to avoid repacking vertices: #define IMGUI_OVERRIDE_DRAWVERT_STRUCT_LAYOUT struct ImDrawVert { ImVec2 pos; float z; ImU32 col; ImVec2 uv; }
    CUSTOMVERTEX* vtx_dst;
    ImDrawIdx* idx_dst;
    if (IDirect3DVertexBuffer9_Lock(g_pVB, 0, (UINT)(draw_data->TotalVtxCount * sizeof(CUSTOMVERTEX)), (void**)&vtx_dst, D3DLOCK_DISCARD) < 0)
        return;
    if (IDirect3DIndexBuffer9_Lock(g_pIB, 0, (UINT)(draw_data->TotalIdxCount * sizeof(ImDrawIdx)), (void**)&idx_dst, D3DLOCK_DISCARD) < 0)
        return;
    for (int n = 0; n < draw_data->CmdListsCount; n++)
    {
        const ImDrawList* cmd_list = draw_data->CmdLists[n];
        const ImDrawVert* vtx_src = cmd_list->VtxBuffer.Data;
        for (int i = 0; i < cmd_list->VtxBuffer.Size; i++)
        {
            vtx_dst->pos[0] = vtx_src->pos.x;
            vtx_dst->pos[1] = vtx_src->pos.y;
            vtx_dst->pos[2] = 0.0f;
            vtx_dst->col = IMGUI_COL_TO_DX9_ARGB(vtx_src->col);
            vtx_dst->uv[0] = vtx_src->uv.x;
            vtx_dst->uv[1] = vtx_src->uv.y;
            vtx_dst++;
            vtx_src++;
        }
        memcpy(idx_dst, cmd_list->IdxBuffer.Data, cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx));
        idx_dst += cmd_list->IdxBuffer.Size;
    }
    IDirect3DVertexBuffer9_Unlock(g_pVB);
    IDirect3DIndexBuffer9_Unlock(g_pIB);
    IDirect3DDevice9_SetStreamSource(g_pd3dDevice, 0, g_pVB, 0, sizeof(CUSTOMVERTEX));
    IDirect3DDevice9_SetIndices(g_pd3dDevice, g_pIB);
    IDirect3DDevice9_SetFVF(g_pd3dDevice, D3DFVF_CUSTOMVERTEX);

    // Setup desired DX state
    ImGui_ImplDX9_SetupRenderState(draw_data);

    // Render command lists
    // (Because we merged all buffers into a single one, we maintain our own offset into them)
    int global_vtx_offset = 0;
    int global_idx_offset = 0;
    ImVec2 clip_off = draw_data->DisplayPos;
    for (int n = 0; n < draw_data->CmdListsCount; n++)
    {
        const ImDrawList* cmd_list = draw_data->CmdLists[n];
        for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
        {
            const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
            if (pcmd->UserCallback != NULL)
            {
                // User callback, registered via ImDrawList::AddCallback()
                // (ImDrawCallback_ResetRenderState is a special callback value used by the user to request the renderer to reset render state.)
                if (pcmd->UserCallback == ImDrawCallback_ResetRenderState)
                    ImGui_ImplDX9_SetupRenderState(draw_data);
                else
                    pcmd->UserCallback(cmd_list, pcmd);
            }
            else
            {
                const RECT r = { (LONG)(pcmd->ClipRect.x - clip_off.x), (LONG)(pcmd->ClipRect.y - clip_off.y), (LONG)(pcmd->ClipRect.z - clip_off.x), (LONG)(pcmd->ClipRect.w - clip_off.y) };
                IDirect3DDevice9_SetTexture(g_pd3dDevice, 0, (IDirect3DBaseTexture9*)pcmd->TextureId);
                IDirect3DDevice9_SetScissorRect(g_pd3dDevice, &r);
                IDirect3DDevice9_DrawIndexedPrimitive(g_pd3dDevice, D3DPT_TRIANGLELIST, pcmd->VtxOffset + global_vtx_offset, 0, (UINT)cmd_list->VtxBuffer.Size, pcmd->IdxOffset + global_idx_offset, pcmd->ElemCount / 3);
            }
        }
        global_idx_offset += cmd_list->IdxBuffer.Size;
        global_vtx_offset += cmd_list->VtxBuffer.Size;
    }

    // Restore the DX9 transform
    IDirect3DDevice9_SetTransform(g_pd3dDevice, D3DTS_WORLD, &last_world);
    IDirect3DDevice9_SetTransform(g_pd3dDevice, D3DTS_VIEW, &last_view);
    IDirect3DDevice9_SetTransform(g_pd3dDevice, D3DTS_PROJECTION, &last_projection);

    // Restore the DX9 state
    IDirect3DStateBlock9_Apply(d3d9_state_block);
    IDirect3DStateBlock9_Release(d3d9_state_block);
}

bool ImGui_ImplDX9_Init(IDirect3DDevice9* device)
{
    // Setup backend capabilities flags
    ImGuiIO& io = ImGui::GetIO();
    io.BackendRendererName = "imgui_impl_dx9";
    io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;  // We can honor the ImDrawCmd::VtxOffset field, allowing for large meshes.

    g_pd3dDevice = device;
    IDirect3DDevice9_AddRef(g_pd3dDevice);
    return true;
}

void ImGui_ImplDX9_Shutdown()
{
    ImGui_ImplDX9_InvalidateDeviceObjects();
    if (g_pd3dDevice) { IDirect3DDevice9_Release(g_pd3dDevice); g_pd3dDevice = NULL; }
}

static bool ImGui_ImplDX9_CreateFontsTexture()
{
    // Build texture atlas
    ImGuiIO& io = ImGui::GetIO();
    unsigned char* pixels;
    int width, height, bytes_per_pixel;
    io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height, &bytes_per_pixel);

    // Convert RGBA32 to BGRA32 (because RGBA32 is not well supported by DX9 devices)
#ifndef IMGUI_USE_BGRA_PACKED_COLOR
    if (io.Fonts->TexPixelsUseColors)
    {
        ImU32* dst_start = (ImU32*)ImGui::MemAlloc(width * height * bytes_per_pixel);
        for (ImU32* src = (ImU32*)pixels, *dst = dst_start, *dst_end = dst_start + width * height; dst < dst_end; src++, dst++)
            *dst = IMGUI_COL_TO_DX9_ARGB(*src);
        pixels = (unsigned char*)dst_start;
    }
#endif

    // Upload texture to graphics system
    g_FontTexture = NULL;
    if (IDirect3DDevice9_CreateTexture(g_pd3dDevice, width, height, 1, D3DUSAGE_DYNAMIC, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &g_FontTexture, NULL) < 0)
        return false;
    D3DLOCKED_RECT tex_locked_rect;
    if (IDirect3DTexture9_LockRect(g_FontTexture, 0, &tex_locked_rect, NULL, 0) != D3D_OK)
        return false;
    for (int y = 0; y < height; y++)
        memcpy((unsigned char*)tex_locked_rect.pBits + tex_locked_rect.Pitch * y, pixels + (width * bytes_per_pixel) * y, (width * bytes_per_pixel));
    IDirect3DTexture9_UnlockRect(g_FontTexture, 0);

    // Store our identifier
    io.Fonts->SetTexID((ImTextureID)g_FontTexture);

#ifndef IMGUI_USE_BGRA_PACKED_COLOR
    if (io.Fonts->TexPixelsUseColors)
        ImGui::MemFree(pixels);
#endif

    return true;
}

bool ImGui_ImplDX9_CreateDeviceObjects()
{
    if (!g_pd3dDevice)
        return false;
    if (!ImGui_ImplDX9_CreateFontsTexture())
        return false;
    return true;
}

void ImGui_ImplDX9_InvalidateDeviceObjects()
{
    if (!g_pd3dDevice)
        return;
    if (g_pVB) { IDirect3DVertexBuffer9_Release(g_pVB); g_pVB = NULL; }
    if (g_pIB) { IDirect3DIndexBuffer9_Release(g_pIB); g_pIB = NULL; }
    if (g_FontTexture) { IDirect3DTexture9_Release(g_FontTexture); g_FontTexture = NULL; ImGui::GetIO().Fonts->SetTexID(NULL); } // We copied g_pFontTextureView to io.Fonts->TexID so let's clear that as well.
}

void ImGui_ImplDX9_NewFrame()
{
    if (!g_FontTexture)
        ImGui_ImplDX9_CreateDeviceObjects();
}

IDirect3DDevice9* ImGui_ImplDX9_GetDevice() {
    return g_pd3dDevice;
}
static HRESULT __stdcall __ThImGui_DX9_Reset_HookFunc(IDirect3DDevice9* dev, D3DPRESENT_PARAMETERS* param)
{
    ImGui_ImplDX9_InvalidateDeviceObjects();
    ImGui::GetIO().DisplaySize = { (float)param->BackBufferWidth, (float)param->BackBufferHeight };
    return __thimgui_dx9_reset_orig(dev, param);
}
IMGUI_IMPL_API void ImGui_ImplDX9_HookReset() {
    __thimgui_dx9_reset_orig = g_pd3dDevice->lpVtbl->Reset;

    DWORD oldProt;
    if (VirtualProtect(&g_pd3dDevice->lpVtbl->Reset, sizeof(void*), PAGE_READWRITE, &oldProt)) {
        g_pd3dDevice->lpVtbl->Reset = __ThImGui_DX9_Reset_HookFunc;
        VirtualProtect(&g_pd3dDevice->lpVtbl->Reset, sizeof(void*), oldProt, &oldProt);
    }
}

IMGUI_IMPL_API void ImGui_ImplDX9_Check(IDirect3DDevice9* device)
{
    if (g_pd3dDevice != device) {
        ImGui_ImplDX9_InvalidateDeviceObjects();
        IDirect3DDevice9_Release(g_pd3dDevice);
        ImGui_ImplDX9_Init(device);
        ImGui_ImplDX9_AdjustDispSize();
    }
}
IMGUI_IMPL_API void ImGui_ImplDX9_AdjustDispSize()
{
    IDirect3DSurface9* backBuffer;
    D3DSURFACE_DESC backBufferDesc;
    IDirect3DDevice9_GetBackBuffer(g_pd3dDevice, 0, 0, D3DBACKBUFFER_TYPE_MONO, &backBuffer);
    IDirect3DSurface9_GetDesc(backBuffer, &backBufferDesc);
    IDirect3DSurface9_Release(backBuffer);

    auto& io = ::ImGui::GetIO();
    io.DisplaySize.x = (float)backBufferDesc.Width;
    io.DisplaySize.y = (float)backBufferDesc.Height;
}