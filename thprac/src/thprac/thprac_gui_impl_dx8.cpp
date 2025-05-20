#pragma warning (disable : 4100)
/*
	This file is a modified copy of imgui's DirectX9 renderer implemention's  source file.
	Changes made:

	1. Includes: Renamed "imgui_impl_dx9.h".
	2. Includes: Added "thprac_hook.h".
	3. Use C versions of all interfaces for...
	4. ImGui_ImplDX9_HookReset and ImGui_ImplDX9_UnhookReset
	// Structure: Replaced "CUSTOMVERTEX" with "ImDrawVert". Removed "CUSTOMVERTEX".
	// Method "ImGui_ImplDX9_RenderDrawData": Simplified vertices repack to only zeroing the z-coordinate.

	Last official change: 2025-04-13.
*/

#define CINTERFACE

#include "thprac_gui_impl_dx8.h"
#include <imgui.h>
#include "..\3rdParties\d3d8\include\d3d8.h"


namespace THPrac
{
	namespace Gui
	{
		struct CUSTOMVERTEX
		{
			float    pos[3];
			D3DCOLOR col;
			float    uv[2];
		};
		static constexpr DWORD D3DFVF_CUSTOMVERTEX =
			D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1;

		// DirectX data
		static LPDIRECT3DDEVICE8        g_pd3dDevice = NULL;
		static LPDIRECT3DVERTEXBUFFER8  g_pVB = NULL;
		static LPDIRECT3DINDEXBUFFER8   g_pIB = NULL;
		static LPDIRECT3DTEXTURE8       g_FontTexture = NULL;
		static int                      g_VertexBufferSize = 5000, g_IndexBufferSize = 10000;


		// Helper functions
		static DWORD					g_pOrigStateBlock = NULL;
		static D3DMATRIX				g_pOrigWorld;
		static D3DMATRIX				g_pOrigView;
		static D3DMATRIX				g_pOrigProj;
		static bool ImplDX8CreateFontsTexture()
		{
			// Build texture atlas
			ImGuiIO& io = ImGui::GetIO();
			unsigned char* pixels;
			int width, height, bytes_per_pixel;
			io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height, &bytes_per_pixel);

			// Upload texture to graphics system
			g_FontTexture = NULL;
            if (g_pd3dDevice->lpVtbl->CreateTexture(g_pd3dDevice, width, height, 1, D3DUSAGE_DYNAMIC, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &g_FontTexture) < 0)
				return false;
			D3DLOCKED_RECT tex_locked_rect;
            if (g_FontTexture->lpVtbl->LockRect(g_FontTexture, 0, &tex_locked_rect, NULL, 0) != D3D_OK)
				return false;
			for (int y = 0; y < height; y++)
				memcpy((unsigned char*)tex_locked_rect.pBits + tex_locked_rect.Pitch * y, pixels + (width * bytes_per_pixel) * y, (width * bytes_per_pixel));
			g_FontTexture->lpVtbl->UnlockRect(g_FontTexture, 0);

			// Store our identifier
			io.Fonts->TexID = (ImTextureID)g_FontTexture;

			return true;
		}
		static void ImplDX8SetupRenderState(ImDrawData* draw_data)
		{
			// Setup render state: fixed-pipeline, alpha-blending, no face culling, no depth testing, shade mode (for gradient)
			g_pd3dDevice->lpVtbl->SetPixelShader(g_pd3dDevice, NULL);
			g_pd3dDevice->lpVtbl->SetRenderState(g_pd3dDevice, D3DRS_CULLMODE, D3DCULL_NONE);
			g_pd3dDevice->lpVtbl->SetRenderState(g_pd3dDevice, D3DRS_LIGHTING, FALSE);
			g_pd3dDevice->lpVtbl->SetRenderState(g_pd3dDevice, D3DRS_ZENABLE, FALSE);
			g_pd3dDevice->lpVtbl->SetRenderState(g_pd3dDevice, D3DRS_ALPHABLENDENABLE, TRUE);
			g_pd3dDevice->lpVtbl->SetRenderState(g_pd3dDevice, D3DRS_ALPHATESTENABLE, FALSE);
			g_pd3dDevice->lpVtbl->SetRenderState(g_pd3dDevice, D3DRS_BLENDOP, D3DBLENDOP_ADD);
			g_pd3dDevice->lpVtbl->SetRenderState(g_pd3dDevice, D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
			g_pd3dDevice->lpVtbl->SetRenderState(g_pd3dDevice, D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
			g_pd3dDevice->lpVtbl->SetTextureStageState(g_pd3dDevice, 0, D3DTSS_COLOROP, D3DTOP_MODULATE);
			g_pd3dDevice->lpVtbl->SetTextureStageState(g_pd3dDevice, 0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
			g_pd3dDevice->lpVtbl->SetTextureStageState(g_pd3dDevice, 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
			g_pd3dDevice->lpVtbl->SetTextureStageState(g_pd3dDevice, 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
			g_pd3dDevice->lpVtbl->SetTextureStageState(g_pd3dDevice, 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
			g_pd3dDevice->lpVtbl->SetTextureStageState(g_pd3dDevice, 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
			g_pd3dDevice->lpVtbl->SetTextureStageState(g_pd3dDevice, 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
			g_pd3dDevice->lpVtbl->SetTextureStageState(g_pd3dDevice, 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);


			g_pd3dDevice->lpVtbl->SetRenderState(g_pd3dDevice, D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
			g_pd3dDevice->lpVtbl->SetRenderState(g_pd3dDevice, D3DRS_FOGENABLE, false);
			g_pd3dDevice->lpVtbl->SetTextureStageState(g_pd3dDevice, 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);


			// Setup world & view matrix
			D3DMATRIX mat_identity = { { { 1.0f, 0.0f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f, 0.0f,  0.0f, 0.0f, 1.0f, 0.0f,  0.0f, 0.0f, 0.0f, 1.0f } } };
			g_pd3dDevice->lpVtbl->SetTransform(g_pd3dDevice, D3DTS_WORLD, &mat_identity);
			g_pd3dDevice->lpVtbl->SetTransform(g_pd3dDevice, D3DTS_VIEW, &mat_identity);
		}
		static __forceinline bool ImplDX8StateBackup()
		{
			// Backup the DX8 state
			if (g_pd3dDevice->lpVtbl->CreateStateBlock(g_pd3dDevice, D3DSBT_ALL, &g_pOrigStateBlock) < 0)
				return false;

			// Backup the DX8 transform
			g_pd3dDevice->lpVtbl->GetTransform(g_pd3dDevice, D3DTS_WORLD, &g_pOrigWorld);
			g_pd3dDevice->lpVtbl->GetTransform(g_pd3dDevice, D3DTS_VIEW, &g_pOrigView);
			g_pd3dDevice->lpVtbl->GetTransform(g_pd3dDevice, D3DTS_PROJECTION, &g_pOrigProj);

			return true;
		}
		static __forceinline void ImplDX8StateRestore()
		{
			// Restore the DX8 transform
			g_pd3dDevice->lpVtbl->SetTransform(g_pd3dDevice, D3DTS_WORLD, &g_pOrigWorld);
			g_pd3dDevice->lpVtbl->SetTransform(g_pd3dDevice, D3DTS_VIEW, &g_pOrigView);
			g_pd3dDevice->lpVtbl->SetTransform(g_pd3dDevice, D3DTS_PROJECTION, &g_pOrigProj);

			// Restore the DX8 state
			g_pd3dDevice->lpVtbl->ApplyStateBlock(g_pd3dDevice, g_pOrigStateBlock);
			g_pd3dDevice->lpVtbl->DeleteStateBlock(g_pd3dDevice, g_pOrigStateBlock);
			g_pOrigStateBlock = NULL;
		}


		// Api functions
		bool ImplDX8Init(IDirect3DDevice8* device)
		{
			ImGuiIO& io = ImGui::GetIO();
			io.BackendRendererName = "thprac_gui_impl_dx8";

			g_pd3dDevice = device;
			g_pd3dDevice->lpVtbl->AddRef(g_pd3dDevice);

			return true;
		}
        void ImplDX8Check(IDirect3DDevice8* device)
        {
			if (g_pd3dDevice != device) {
                g_pd3dDevice->lpVtbl->Release(g_pd3dDevice);
                ImplDX8Init(device);
			}
        }
		void ImplDX8Shutdown()
		{
			ImplDX8InvalidateDeviceObjects();
            if (g_pd3dDevice) {
                g_pd3dDevice->lpVtbl->Release(g_pd3dDevice);
                g_pd3dDevice = NULL;
            }
		}
		void ImplDX8NewFrame()
		{
			if (!g_FontTexture)
				ImplDX8CreateDeviceObjects();
		}
		void ImplDX8RenderDrawData(ImDrawData* draw_data)
		{
			// Avoid rendering when minimized
			if (draw_data->DisplaySize.x <= 0.0f || draw_data->DisplaySize.y <= 0.0f)
				return;

			// Create and grow buffers if needed
			if (!g_pVB || g_VertexBufferSize < draw_data->TotalVtxCount)
			{
				if (g_pVB) { g_pVB->lpVtbl->Release(g_pVB); g_pVB = NULL; }
				g_VertexBufferSize = draw_data->TotalVtxCount + 5000;
				if (g_pd3dDevice->lpVtbl->CreateVertexBuffer(g_pd3dDevice, g_VertexBufferSize * sizeof(CUSTOMVERTEX),
					D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, D3DFVF_CUSTOMVERTEX, D3DPOOL_DEFAULT, &g_pVB) < 0)
					return;
			}
			if (!g_pIB || g_IndexBufferSize < draw_data->TotalIdxCount)
			{
				if (g_pIB) { g_pIB->lpVtbl->Release(g_pIB); g_pIB = NULL; }
				g_IndexBufferSize = draw_data->TotalIdxCount + 10000;
				if (g_pd3dDevice->lpVtbl->CreateIndexBuffer(g_pd3dDevice, g_IndexBufferSize * sizeof(ImDrawIdx),
					D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, sizeof(ImDrawIdx) == 2 ? D3DFMT_INDEX16 : D3DFMT_INDEX32,
					D3DPOOL_DEFAULT, &g_pIB) < 0)
					return;
			}

			// Backup DX8 state
			if (!ImplDX8StateBackup()) return;

			// Copy and convert all vertices into a single contiguous buffer, convert colors to DX8 default format.
			// FIXME-OPT: This is a waste of resource, the ideal is to use imconfig.h and
			//  1) to avoid repacking colors:   #define IMGUI_USE_BGRA_PACKED_COLOR
			//  2) to avoid repacking vertices: #define IMGUI_OVERRIDE_DRAWVERT_STRUCT_LAYOUT struct ImDrawVert { ImVec2 pos; float z; ImU32 col; ImVec2 uv; }
			CUSTOMVERTEX* vtx_dst;
			ImDrawIdx* idx_dst;
			if (g_pVB->lpVtbl->Lock(g_pVB, 0, (UINT)(draw_data->TotalVtxCount * sizeof(CUSTOMVERTEX)), (BYTE * *)& vtx_dst, D3DLOCK_DISCARD) < 0)
				return;
			if (g_pIB->lpVtbl->Lock(g_pIB, 0, (UINT)(draw_data->TotalIdxCount * sizeof(ImDrawIdx)), (BYTE * *)& idx_dst, D3DLOCK_DISCARD) < 0)
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
					vtx_dst->col = vtx_src->col;
					vtx_dst->uv[0] = vtx_src->uv.x;
					vtx_dst->uv[1] = vtx_src->uv.y;
					vtx_dst++;
					vtx_src++;
				}
				memcpy(idx_dst, cmd_list->IdxBuffer.Data, cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx));
				idx_dst += cmd_list->IdxBuffer.Size;
			}
			g_pVB->lpVtbl->Unlock(g_pVB);
			g_pIB->lpVtbl->Unlock(g_pIB);
			g_pd3dDevice->lpVtbl->SetStreamSource(g_pd3dDevice, 0, g_pVB, sizeof(CUSTOMVERTEX));
			g_pd3dDevice->lpVtbl->SetVertexShader(g_pd3dDevice, D3DFVF_CUSTOMVERTEX);

			// Setup desired DX state
			ImplDX8SetupRenderState(draw_data);

			// Render command lists
			int vtx_offset = 0;
			int idx_offset = 0;
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
							ImplDX8SetupRenderState(draw_data);
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

						// Render
						g_pd3dDevice->lpVtbl->SetTexture(g_pd3dDevice, 0, (LPDIRECT3DBASETEXTURE8)pcmd->TextureId);
						g_pd3dDevice->lpVtbl->SetViewport(g_pd3dDevice, &viewport);
						g_pd3dDevice->lpVtbl->SetTransform(g_pd3dDevice, D3DTS_PROJECTION, &matProjection);
						g_pd3dDevice->lpVtbl->SetIndices(g_pd3dDevice, (IDirect3DIndexBuffer8*)g_pIB, vtx_offset);
						g_pd3dDevice->lpVtbl->DrawIndexedPrimitive(g_pd3dDevice, D3DPT_TRIANGLELIST, 0, (UINT)cmd_list->VtxBuffer.Size, idx_offset, pcmd->ElemCount / 3);
					}
					idx_offset += pcmd->ElemCount;
				}
				vtx_offset += cmd_list->VtxBuffer.Size;
			}

			// Restore DX8 state
			ImplDX8StateRestore();

		}
		bool ImplDX8CreateDeviceObjects()
		{
			if (!g_pd3dDevice)
				return false;
			if (!ImplDX8CreateFontsTexture())
				return false;
			return true;
		}
		void ImplDX8InvalidateDeviceObjects()
		{
			if (!g_pd3dDevice)
				return;
			if (g_pVB) { g_pVB->lpVtbl->Release(g_pVB); g_pVB = NULL; }
			if (g_pIB) { g_pIB->lpVtbl->Release(g_pIB); g_pIB = NULL; }
            if (g_FontTexture) {
               g_FontTexture->lpVtbl->Release(g_FontTexture);
               g_FontTexture = NULL;
               ImGui::GetIO().Fonts->TexID = NULL;
            } // We copied g_pFontTextureView to io.Fonts->TexID so let's clear that as well.
		}


		// Added functions used by thprac
        static decltype(IDirect3DDevice8Vtbl::Reset) __thimgui_dx8_reset_orig = nullptr;
		static HRESULT __stdcall __ThImGui_DX8_Reset_HookFunc(IDirect3DDevice8* dev, D3DPRESENT_PARAMETERS* param)
        {
			ImplDX8InvalidateDeviceObjects();
			return __thimgui_dx8_reset_orig(dev, param);
		}
		bool ImplDX8HookReset()
		{
			__thimgui_dx8_reset_orig = g_pd3dDevice->lpVtbl->Reset;

			DWORD oldProt;
			if (!VirtualProtect(&g_pd3dDevice->lpVtbl->Reset, sizeof(void*), PAGE_READWRITE, &oldProt)) {
				return false;
			}
			g_pd3dDevice->lpVtbl->Reset = __ThImGui_DX8_Reset_HookFunc;
			VirtualProtect(&g_pd3dDevice->lpVtbl->Reset, sizeof(void*), oldProt, &oldProt);
			return true;
		}
		bool ImplDX8UnHookReset()
		{
			__thimgui_dx8_reset_orig = g_pd3dDevice->lpVtbl->Reset;

			DWORD oldProt;
			if (!VirtualProtect(&g_pd3dDevice->lpVtbl->Reset, sizeof(void*), PAGE_READWRITE, &oldProt)) {
				return false;
			}
			g_pd3dDevice->lpVtbl->Reset = __thimgui_dx8_reset_orig;
			__thimgui_dx8_reset_orig = nullptr;
			VirtualProtect(&g_pd3dDevice->lpVtbl->Reset, sizeof(void*), oldProt, &oldProt);
			return true;
		}
	}
}
