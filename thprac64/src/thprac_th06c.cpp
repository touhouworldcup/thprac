#include <wininternal.h>

#include <thprac_games.h>

namespace TH06C {
enum rel_addrs {
	D3D11_DEVICE_CONTEXT_PTR = 0x84D270,
	D3D11_DEVICE_PTR = 0x84D268,
	HWND_ADDR = 0x3F9B28,
	INPUT_ADDR = 0x90A110,
	INPUT_PREV_ADDR = 0x90A114,
	IS_SEVENTH_FRAME_OF_HELD_INPUT_ADDR = 0x90A118,
};

HOOKSET_DEFINE(THMainHook)

EHOOK_DY(th06c_present, 0x58F2D, 5, {
	ImGui::GetIO().DisplaySize = { 640.0f, 480.0f };
	GameGuiBegin(IMPL_WIN32_DX11);
	ImGui::TextUnformatted("It works");
	GameGuiEnd();
	GameGuiRender(IMPL_WIN32_DX11);
	pCtx->Rcx |= 0xFFFFFFFF;
	pCtx->Rcx &= 0xFFFFFFFE;
})
HOOKSET_ENDDEF();

void THGuiCreate() {
	GameGuiInit(
		IMPL_WIN32_DX11, 
		RVA(D3D11_DEVICE_PTR), 
		RVA(HWND_ADDR), 
		Gui::INGAGME_INPUT_GEN1, 
		RVA(INPUT_ADDR), 
		RVA(INPUT_PREV_ADDR), 
		RVA(IS_SEVENTH_FRAME_OF_HELD_INPUT_ADDR), 
		1.0f, 
		RVA(D3D11_DEVICE_CONTEXT_PTR)
	);
	EnableAllHooks(THMainHook);
}

HOOKSET_DEFINE(THInitHook)

EHOOK_DY(th06c_init_d3d11, 0x250745, 1, {
	self->Disable();
	THGuiCreate();

	// Original code: ret
	pCtx->Rip = PopHelper(pCtx);
})

HOOKSET_ENDDEF()
}

void TH06CInitReal() {
	ingame_image_base = (uintptr_t)CurrentPeb()->ImageBaseAddress;
	EnableAllHooks(TH06C::THInitHook);
}