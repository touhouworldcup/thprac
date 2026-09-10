// dear imgui: Renderer Backend for DirectX8
// This needs to be used along with a Platform Backend (e.g. Win32)

// Implemented features:
//  [X] Renderer: User texture binding. Use 'LPDIRECT3DTEXTURE8' as ImTextureID. Read the FAQ about ImTextureID!
//  [X] Renderer: Support for large meshes (64k+ vertices) with 16-bit indices.

// You can copy and use unmodified imgui_impl_* files in your project. See examples/ folder for examples of using this.
// If you are new to Dear ImGui, read documentation from the docs/ folder + read the top of imgui.cpp.
// Read online: https://github.com/ocornut/imgui/tree/master/docs

#include "imgui.h"
#include "imgui_impl_dx8.h"

// DirectX
#define CINTERFACE
#include <d3d8.h>

// DirectX data
static IDirect3DDevice8*        g_pd3dDevice = NULL;
static IDirect3DVertexBuffer8*  g_pVB = NULL;
static IDirect3DIndexBuffer8*   g_pIB = NULL;
static IDirect3DTexture8*       g_FontTexture = NULL;
static int                      g_VertexBufferSize = 5000, g_IndexBufferSize = 10000;

static DWORD					g_pOrigStateBlock = NULL;
static D3DMATRIX				g_pOrigWorld;
static D3DMATRIX				g_pOrigView;
static D3DMATRIX				g_pOrigProj;

decltype(IDirect3DDevice8Vtbl::Reset) __thimgui_dx8_reset_orig = NULL;

struct CUSTOMVERTEX
{
    float    pos[3];
    D3DCOLOR col;
    float    uv[2];
};
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1)

#ifdef IMGUI_USE_BGRA_PACKED_COLOR
#define IMGUI_COL_TO_DX8_ARGB(_COL)     (_COL)
#else
#define IMGUI_COL_TO_DX8_ARGB(_COL)     (((_COL) & 0xFF00FF00) | (((_COL) & 0xFF0000) >> 16) | (((_COL) & 0xFF) << 16))
#endif

static void ImGui_ImplDX8_SetupRenderState(ImDrawData* draw_data)
{
    // Setup viewport
    D3DVIEWPORT8 vp;
    vp.X = vp.Y = 0;
    vp.Width = (DWORD)draw_data->DisplaySize.x;
    vp.Height = (DWORD)draw_data->DisplaySize.y;
    vp.MinZ = 0.0f;
    vp.MaxZ = 1.0f;
    IDirect3DDevice8_SetViewport(g_pd3dDevice, &vp);

    // Setup render state: fixed-pipeline, alpha-blending, no face culling, no depth testing, shade mode (for gradient)
	IDirect3DDevice8_SetPixelShader(g_pd3dDevice, NULL);
	IDirect3DDevice8_SetRenderState(g_pd3dDevice, D3DRS_CULLMODE, D3DCULL_NONE);
	IDirect3DDevice8_SetRenderState(g_pd3dDevice, D3DRS_LIGHTING, FALSE);
	IDirect3DDevice8_SetRenderState(g_pd3dDevice, D3DRS_ZENABLE, FALSE);
	IDirect3DDevice8_SetRenderState(g_pd3dDevice, D3DRS_ALPHABLENDENABLE, TRUE);
	IDirect3DDevice8_SetRenderState(g_pd3dDevice, D3DRS_ALPHATESTENABLE, FALSE);
	IDirect3DDevice8_SetRenderState(g_pd3dDevice, D3DRS_BLENDOP, D3DBLENDOP_ADD);
	IDirect3DDevice8_SetRenderState(g_pd3dDevice, D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	IDirect3DDevice8_SetRenderState(g_pd3dDevice, D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	IDirect3DDevice8_SetTextureStageState(g_pd3dDevice, 0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	IDirect3DDevice8_SetTextureStageState(g_pd3dDevice, 0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	IDirect3DDevice8_SetTextureStageState(g_pd3dDevice, 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	IDirect3DDevice8_SetTextureStageState(g_pd3dDevice, 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	IDirect3DDevice8_SetTextureStageState(g_pd3dDevice, 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	IDirect3DDevice8_SetTextureStageState(g_pd3dDevice, 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
	IDirect3DDevice8_SetTextureStageState(g_pd3dDevice, 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
	IDirect3DDevice8_SetTextureStageState(g_pd3dDevice, 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);


	IDirect3DDevice8_SetRenderState(g_pd3dDevice, D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
	IDirect3DDevice8_SetRenderState(g_pd3dDevice, D3DRS_FOGENABLE, false);
	IDirect3DDevice8_SetTextureStageState(g_pd3dDevice, 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);


	D3DMATRIX mat_identity = { { { 1.0f, 0.0f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f, 0.0f,  0.0f, 0.0f, 1.0f, 0.0f,  0.0f, 0.0f, 0.0f, 1.0f } } };
	IDirect3DDevice8_SetTransform(g_pd3dDevice, D3DTS_WORLD, &mat_identity);
	IDirect3DDevice8_SetTransform(g_pd3dDevice, D3DTS_VIEW, &mat_identity);

    // Setup orthographic projection matrix
    // Our visible imgui space lies from draw_data->DisplayPos (top left) to draw_data->DisplayPos+data_data->DisplaySize (bottom right). DisplayPos is (0,0) for single viewport apps.
    // Being agnostic of whether <d3DX8.h> or <DirectXMath.h> can be used, we aren't relying on D3DXMatrixIdentity()/D3DXMatrixOrthoOffCenterLH() or DirectX::XMMatrixIdentity()/DirectX::XMMatrixOrthographicOffCenterLH()
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
        IDirect3DDevice8_SetTransform(g_pd3dDevice, D3DTS_WORLD, &mat_identity);
        IDirect3DDevice8_SetTransform(g_pd3dDevice, D3DTS_VIEW, &mat_identity);
        IDirect3DDevice8_SetTransform(g_pd3dDevice, D3DTS_PROJECTION, &mat_projection);
    }
}

static __forceinline bool ImGui_ImplDX8_StateBackup()
{
	// Backup the DX8 state
	if (IDirect3DDevice8_CreateStateBlock(g_pd3dDevice, D3DSBT_ALL, &g_pOrigStateBlock) < 0)
		return false;
	// Backup the DX8 transform
	IDirect3DDevice8_GetTransform(g_pd3dDevice, D3DTS_WORLD, &g_pOrigWorld);
	IDirect3DDevice8_GetTransform(g_pd3dDevice, D3DTS_VIEW, &g_pOrigView);
	IDirect3DDevice8_GetTransform(g_pd3dDevice, D3DTS_PROJECTION, &g_pOrigProj);
	return true;
}
static __forceinline void ImGui_ImplDX8_StateRestore()
{
	// Restore the DX8 transform
	IDirect3DDevice8_SetTransform(g_pd3dDevice, D3DTS_WORLD, &g_pOrigWorld);
	IDirect3DDevice8_SetTransform(g_pd3dDevice, D3DTS_VIEW, &g_pOrigView);
	IDirect3DDevice8_SetTransform(g_pd3dDevice, D3DTS_PROJECTION, &g_pOrigProj);
	// Restore the DX8 state
	IDirect3DDevice8_ApplyStateBlock(g_pd3dDevice, g_pOrigStateBlock);
	IDirect3DDevice8_DeleteStateBlock(g_pd3dDevice, g_pOrigStateBlock);
	g_pOrigStateBlock = NULL;
}

// Render function.
void ImGui_ImplDX8_RenderDrawData(ImDrawData* draw_data)
{
    // Avoid rendering when minimized
    if (draw_data->DisplaySize.x <= 0.0f || draw_data->DisplaySize.y <= 0.0f)
        return;

    // Create and grow buffers if needed
    if (!g_pVB || g_VertexBufferSize < draw_data->TotalVtxCount)
    {
        if (g_pVB) { IDirect3DVertexBuffer8_Release(g_pVB); g_pVB = NULL; }
        g_VertexBufferSize = draw_data->TotalVtxCount + 5000;
        if (IDirect3DDevice8_CreateVertexBuffer(g_pd3dDevice, g_VertexBufferSize * sizeof(CUSTOMVERTEX), D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, D3DFVF_CUSTOMVERTEX, D3DPOOL_DEFAULT, &g_pVB) < 0)
            return;
    }
    if (!g_pIB || g_IndexBufferSize < draw_data->TotalIdxCount)
    {
        if (g_pIB) { IDirect3DIndexBuffer8_Release(g_pIB); g_pIB = NULL; }
        g_IndexBufferSize = draw_data->TotalIdxCount + 10000;
        if (IDirect3DDevice8_CreateIndexBuffer(g_pd3dDevice, g_IndexBufferSize * sizeof(ImDrawIdx), D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, sizeof(ImDrawIdx) == 2 ? D3DFMT_INDEX16 : D3DFMT_INDEX32, D3DPOOL_DEFAULT, &g_pIB) < 0)
            return;
    }

    // Backup the DX8 state
    if(!ImGui_ImplDX8_StateBackup()) return;

    // Backup the DX8 transform (DX8 documentation suggests that it is included in the StateBlock but it doesn't appear to)
    D3DMATRIX last_world, last_view, last_projection;
    IDirect3DDevice8_GetTransform(g_pd3dDevice, D3DTS_WORLD, &last_world);
    IDirect3DDevice8_GetTransform(g_pd3dDevice, D3DTS_VIEW, &last_view);
    IDirect3DDevice8_GetTransform(g_pd3dDevice, D3DTS_PROJECTION, &last_projection);

    // Copy and convert all vertices into a single contiguous buffer, convert colors to DX8 default format.
    // FIXME-OPT: This is a minor waste of resource, the ideal is to use imconfig.h and
    //  1) to avoid repacking colors:   #define IMGUI_USE_BGRA_PACKED_COLOR
    //  2) to avoid repacking vertices: #define IMGUI_OVERRIDE_DRAWVERT_STRUCT_LAYOUT struct ImDrawVert { ImVec2 pos; float z; ImU32 col; ImVec2 uv; }
    CUSTOMVERTEX* vtx_dst;
    ImDrawIdx* idx_dst;
    if (IDirect3DVertexBuffer8_Lock(g_pVB, 0, (UINT)(draw_data->TotalVtxCount * sizeof(CUSTOMVERTEX)), (BYTE**)&vtx_dst, D3DLOCK_DISCARD) < 0)
        return;
    if (IDirect3DIndexBuffer8_Lock(g_pIB, 0, (UINT)(draw_data->TotalIdxCount * sizeof(ImDrawIdx)), (BYTE**)&idx_dst, D3DLOCK_DISCARD) < 0)
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
            vtx_dst->col = IMGUI_COL_TO_DX8_ARGB(vtx_src->col);
            vtx_dst->uv[0] = vtx_src->uv.x;
            vtx_dst->uv[1] = vtx_src->uv.y;
            vtx_dst++;
            vtx_src++;
        }
        memcpy(idx_dst, cmd_list->IdxBuffer.Data, cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx));
        idx_dst += cmd_list->IdxBuffer.Size;
    }
    IDirect3DVertexBuffer8_Unlock(g_pVB);
    IDirect3DIndexBuffer8_Unlock(g_pIB);
    IDirect3DDevice8_SetStreamSource(g_pd3dDevice, 0, g_pVB, sizeof(CUSTOMVERTEX));
    IDirect3DDevice8_SetIndices(g_pd3dDevice, g_pIB, 0);
    IDirect3DDevice8_SetVertexShader(g_pd3dDevice, D3DFVF_CUSTOMVERTEX);

    // Setup desired DX state
    ImGui_ImplDX8_SetupRenderState(draw_data);

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
                    ImGui_ImplDX8_SetupRenderState(draw_data);
                else
                    pcmd->UserCallback(cmd_list, pcmd);
            }
            else
            {
    			// Setup viewport
    			D3DVIEWPORT8 viewport;
    			viewport.X = static_cast<DWORD>((0.0f < pcmd->ClipRect.x) ? pcmd->ClipRect.x : 0.0f);
    			viewport.Y = static_cast<DWORD>((0.0f < pcmd->ClipRect.x) ? pcmd->ClipRect.y : 0.0f);
    			viewport.Width = static_cast<DWORD>((draw_data->DisplaySize.x < pcmd->ClipRect.z - pcmd->ClipRect.x) ? draw_data->DisplaySize.x : pcmd->ClipRect.z - pcmd->ClipRect.x);
    			viewport.Height = static_cast<DWORD>((draw_data->DisplaySize.y < pcmd->ClipRect.w - pcmd->ClipRect.y) ? draw_data->DisplaySize.y : pcmd->ClipRect.w - pcmd->ClipRect.y);
    			viewport.MinZ = 0.0f;
    			viewport.MaxZ = 1.0f;

    			// Setup projection matrix
    			const float L = 0.5f + viewport.X, R = viewport.Width + 0.5f + viewport.X;
    			const float T = 0.5f + viewport.Y, B = viewport.Height + 0.5f + viewport.Y;
    			D3DMATRIX matProjection =
    			{
    				2.0f / (R - L)		, 0.0f				, 0.0f, 0.0f,
    				0.0f				, 2.0f / (T - B)	, 0.0f, 0.0f,
    				0.0f				, 0.0f				, 0.5f, 0.0f,
    				(L + R) / (L - R)	, (T + B) / (B - T)	, 0.5f, 1.0f,
    			};


                IDirect3DDevice8_SetTexture(g_pd3dDevice, 0, (IDirect3DBaseTexture8*)pcmd->TextureId);
                IDirect3DDevice8_SetViewport(g_pd3dDevice, &viewport);
			    IDirect3DDevice8_SetTransform(g_pd3dDevice, D3DTS_PROJECTION, &matProjection);
			    IDirect3DDevice8_SetIndices(g_pd3dDevice, (IDirect3DIndexBuffer8*)g_pIB, global_vtx_offset);
                (g_pd3dDevice)->lpVtbl->DrawIndexedPrimitive(g_pd3dDevice, D3DPT_TRIANGLELIST, 0, (UINT)cmd_list->VtxBuffer.Size, pcmd->IdxOffset + global_idx_offset, pcmd->ElemCount / 3);
            }
        }
        global_idx_offset += cmd_list->IdxBuffer.Size;
        global_vtx_offset += cmd_list->VtxBuffer.Size;
    }

    // Restore the DX8 transform
    IDirect3DDevice8_SetTransform(g_pd3dDevice, D3DTS_WORLD, &last_world);
    IDirect3DDevice8_SetTransform(g_pd3dDevice, D3DTS_VIEW, &last_view);
    IDirect3DDevice8_SetTransform(g_pd3dDevice, D3DTS_PROJECTION, &last_projection);

    // Restore the DX8 state
    ImGui_ImplDX8_StateRestore();
}

bool ImGui_ImplDX8_Init(IDirect3DDevice8* device)
{
    // Setup backend capabilities flags
    ImGuiIO& io = ImGui::GetIO();
    io.BackendRendererName = "imgui_impl_DX8";
    io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;  // We can honor the ImDrawCmd::VtxOffset field, allowing for large meshes.

    g_pd3dDevice = device;
    IDirect3DDevice8_AddRef(device);
    return true;
}

void ImGui_ImplDX8_Shutdown()
{
    ImGui_ImplDX8_InvalidateDeviceObjects();
    if (g_pd3dDevice) { IDirect3DDevice8_Release(g_pd3dDevice); g_pd3dDevice = NULL; }
}

static bool ImGui_ImplDX8_CreateFontsTexture()
{
    // Build texture atlas
    ImGuiIO& io = ImGui::GetIO();
    unsigned char* pixels;
    int width, height, bytes_per_pixel;
    io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height, &bytes_per_pixel);

    // Convert RGBA32 to BGRA32 (because RGBA32 is not well supported by DX8 devices)
#ifndef IMGUI_USE_BGRA_PACKED_COLOR
    if (io.Fonts->TexPixelsUseColors)
    {
        ImU32* dst_start = (ImU32*)ImGui::MemAlloc(width * height * bytes_per_pixel);
        for (ImU32* src = (ImU32*)pixels, *dst = dst_start, *dst_end = dst_start + width * height; dst < dst_end; src++, dst++)
            *dst = IMGUI_COL_TO_DX8_ARGB(*src);
        pixels = (unsigned char*)dst_start;
    }
#endif

    // Upload texture to graphics system
    g_FontTexture = NULL;
    if (IDirect3DDevice8_CreateTexture(g_pd3dDevice, width, height, 1, D3DUSAGE_DYNAMIC, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &g_FontTexture) < 0)
        return false;
    D3DLOCKED_RECT tex_locked_rect;
    if (IDirect3DTexture8_LockRect(g_FontTexture, 0, &tex_locked_rect, NULL, 0) != D3D_OK)
        return false;
    for (int y = 0; y < height; y++)
        memcpy((unsigned char*)tex_locked_rect.pBits + tex_locked_rect.Pitch * y, pixels + (width * bytes_per_pixel) * y, (width * bytes_per_pixel));
    IDirect3DTexture8_UnlockRect(g_FontTexture, 0);

    // Store our identifier
    io.Fonts->SetTexID((ImTextureID)g_FontTexture);

#ifndef IMGUI_USE_BGRA_PACKED_COLOR
    if (io.Fonts->TexPixelsUseColors)
        ImGui::MemFree(pixels);
#endif

    return true;
}

bool ImGui_ImplDX8_CreateDeviceObjects()
{
    if (!g_pd3dDevice)
        return false;
    if (!ImGui_ImplDX8_CreateFontsTexture())
        return false;
    return true;
}

void ImGui_ImplDX8_InvalidateDeviceObjects()
{
    if (!g_pd3dDevice)
        return;
    if (g_pVB) { IDirect3DVertexBuffer8_Release(g_pVB); g_pVB = NULL; }
    if (g_pIB) { IDirect3DIndexBuffer8_Release(g_pIB); g_pIB = NULL; }
    if (g_FontTexture) { IDirect3DTexture8_Release(g_FontTexture); g_FontTexture = NULL; ImGui::GetIO().Fonts->SetTexID(NULL); } // We copied g_pFontTextureView to io.Fonts->TexID so let's clear that as well.
}

void ImGui_ImplDX8_NewFrame()
{
    if (!g_FontTexture)
        ImGui_ImplDX8_CreateDeviceObjects();
}

IDirect3DDevice8* ImGui_ImplDX8_GetDevice() {
    return g_pd3dDevice;
}

static HRESULT __stdcall __ThImGui_DX8_Reset_HookFunc(IDirect3DDevice8* dev, D3DPRESENT_PARAMETERS* param)
{
    ImGui_ImplDX8_InvalidateDeviceObjects();
    return __thimgui_dx8_reset_orig(dev, param);
}

IMGUI_IMPL_API void ImGui_ImplDX8_HookReset() {
    __thimgui_dx8_reset_orig = g_pd3dDevice->lpVtbl->Reset;

    DWORD oldProt;
    if (!VirtualProtect(&g_pd3dDevice->lpVtbl->Reset, sizeof(void*), PAGE_READWRITE, &oldProt)) {
        g_pd3dDevice->lpVtbl->Reset = __ThImGui_DX8_Reset_HookFunc;
        VirtualProtect(&g_pd3dDevice->lpVtbl->Reset, sizeof(void*), oldProt, &oldProt);
    }
}
IMGUI_IMPL_API void ImGui_ImplDX8_Check(IDirect3DDevice8* device) {
    if (g_pd3dDevice != device) {
        ImGui_ImplDX8_InvalidateDeviceObjects();
        IDirect3DDevice8_Release(g_pd3dDevice);
        ImGui_ImplDX8_Init(device);
        ImGui_ImplDX8_AdjustDispSize();
    }
}
IMGUI_IMPL_API void ImGui_ImplDX8_AdjustDispSize() {
    IDirect3DSurface8* backBuffer;
    D3DSURFACE_DESC backBufferDesc;
    IDirect3DDevice8_GetBackBuffer(g_pd3dDevice, 0, D3DBACKBUFFER_TYPE_MONO, &backBuffer);
    IDirect3DSurface8_GetDesc(backBuffer, &backBufferDesc);
    IDirect3DSurface8_Release(backBuffer);

    auto& io = ::ImGui::GetIO();
    io.DisplaySize.x = (float)backBufferDesc.Width;
    io.DisplaySize.y = (float)backBufferDesc.Height;
}