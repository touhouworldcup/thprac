// Based on https://github.com/ocornut/imgui/discussions/3925
// Fun fact: the author of that is also a Touhou fan

#define NOMINMAX
#include <Windows.h>
#include <d3d9.h>
#include <algorithm>

#include <ImGui.h>

#include "thprac_cfg.h"
#include "thprac_log.h"
#include "thprac_gui_impl_win32.h"
#include "thprac_gui_impl_dx9.h"
#include "thprac_gui_locale.h"
#include "thprac_gui_input.h"

#include "../../resource.h"

namespace THPrac {
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Constinit structs where all the required data is already here
// so that it doesn't need to be initialized
static constinit WNDCLASSEXW g_WndCls = {
    .cbSize = sizeof(WNDCLASSEXW),
    .style = CS_HREDRAW | CS_VREDRAW,
    .lpfnWndProc = &WndProc,
    .lpszClassName = L"thprac launcher window",
};

static constinit D3DPRESENT_PARAMETERS g_d3dpp = {
    .BackBufferFormat = D3DFMT_UNKNOWN,
    .SwapEffect = D3DSWAPEFFECT_DISCARD,
    .Windowed = TRUE,
    .EnableAutoDepthStencil = TRUE,
    .AutoDepthStencilFormat = D3DFMT_D16,
    .PresentationInterval = D3DPRESENT_INTERVAL_ONE,
};

static constinit IDirect3DDevice9* g_pd3dDevice = NULL;
static constinit bool g_IsUITextureIDValid = false;
static constinit bool g_IsInitialized = false;

void ResetDevice();
bool UpdateUIScaling(float scale = 1.0f);

void UiUpdate()
{
    if (!g_IsInitialized)
        return;

    // Start the Dear ImGui frame
    Gui::ImplDX9NewFrame();
    Gui::ImplWin32NewFrame();
    g_IsUITextureIDValid = true;
    ImGui::NewFrame();

    auto& io = ImGui::GetIO();
    auto& style = ImGui::GetStyle();
    // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
    ImGui::SetNextWindowSize(io.DisplaySize);
    ImGui::SetNextWindowPos({ 0.0f, 0.0f });
    style.WindowBorderSize = 0;
    ImGui::Begin("###_main_window", nullptr,
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize);

    ImGui::BeginTabBar("__launcher_tab_bar");
   
    if (ImGui::BeginTabItem(S(THPRAC_GAMES))) {
        ImGui::TextUnformatted("Games");
        ImGui::EndTabItem();
    }
    if (ImGui::BeginTabItem(S(THPRAC_LINKS))) {
        ImGui::TextUnformatted("Links");
        ImGui::EndTabItem();
    }
    if (ImGui::BeginTabItem(S(THPRAC_TOOLS))) {
        ImGui::TextUnformatted("Tools");
        ImGui::EndTabItem();
    }
    if (ImGui::BeginTabItem(S(THPRAC_SETTINGS))) {
        ImGui::TextUnformatted("Settings");
        ImGui::EndTabItem();
    }


    ImGui::EndTabBar();

    ImGui::End();
    ImGui::EndFrame();
    ImGui::Render();

    g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
    g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    g_pd3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
    g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_RGBA(0, 0, 0, 255), 1.0f, 0);
    if (g_pd3dDevice->BeginScene() >= 0) {
        if (g_IsUITextureIDValid) {
            Gui::ImplDX9RenderDrawData(ImGui::GetDrawData());
        }
        g_pd3dDevice->EndScene();
    }
    HRESULT result = g_pd3dDevice->Present(NULL, NULL, NULL, NULL);

    // Handle loss of D3D9 device
    if (result == D3DERR_DEVICELOST && g_pd3dDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
        ResetDevice();
}

int Launcher(HINSTANCE hInstance, int nCmdShow) {
    // There shall only be one
    if (HWND existing = FindWindowW(g_WndCls.lpszClassName, nullptr)) {
        SetForegroundWindow(existing);
        return 0;
    }

    auto* d3d9 = LoadLibraryW(L"d3d9.dll");
    if (!d3d9) {
        return 1;
    }

    auto* _Direct3DCreate9 = (decltype(Direct3DCreate9)*)GetProcAddress(d3d9, "Direct3DCreate9");
    if (!_Direct3DCreate9) {
        return 1;
    }

    IDirect3D9* d3d = nullptr;
    if ((d3d = _Direct3DCreate9(D3D_SDK_VERSION)) == NULL) {
        return 1;
    }
    defer(d3d->Release());
    g_WndCls.hIcon = LoadIconW(hInstance, MAKEINTRESOURCEW(IDI_ICON1));
    g_WndCls.hInstance = hInstance;
    if (!RegisterClassExW(&g_WndCls)) {
        return 1;
    }
    defer(UnregisterClassW(g_WndCls.lpszClassName, g_WndCls.hInstance));

    HWND hwnd = CreateWindowExW(
        0, g_WndCls.lpszClassName, L"thprac - Touhou Game Launcher", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 960, 720,
        NULL, NULL, g_WndCls.hInstance, NULL);

    if (!hwnd) {
        return 1;
    }
    defer(DestroyWindow(hwnd));

    if (d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &g_d3dpp, &g_pd3dDevice) < 0) {
        return 1;
    }
    defer(d3d->Release());

    if (!ImGui::CreateContext()) {
        return 1;
    }
    defer(ImGui::DestroyContext());

    // Setup Dear ImGui flags
    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = nullptr;
    SetTheme(gSettingsGlobal.theme);

    // Setup Platform/Renderer backends
    if (!Gui::ImplWin32Init(hwnd)) {
        return 1;
    }
    defer(Gui::ImplWin32Shutdown());

    if (!Gui::ImplDX9Init(g_pd3dDevice)) {
        return 1;
    }
    defer(Gui::ImplDX9Shutdown());

    float dpiscale = 1.0f;
    if (auto shcore = GetModuleHandleW(L"shcore.dll")) {

        typedef HRESULT(WINAPI * PSetProcessDpiAwareness)(DWORD value);
        typedef HRESULT(WINAPI * PGetDpiForMonitor)(HMONITOR hmonitor, DWORD dpiType, UINT * dpiX, UINT * dpiY);

        auto setProcDpiAwareness = (PSetProcessDpiAwareness)GetProcAddress(shcore, "SetProcessDpiAwareness");
        auto getDpiForMonitor = (PGetDpiForMonitor)GetProcAddress(shcore, "GetDpiForMonitor");
        if (setProcDpiAwareness && getDpiForMonitor) {
            setProcDpiAwareness(1);
            UINT dpiX;
            UINT dpiY;
            getDpiForMonitor(MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST), 0, &dpiX, &dpiY);
            dpiscale = dpiX / (float)USER_DEFAULT_SCREEN_DPI;
        }
    }

    // In vanilla ImGui, ImGui_ImplWin32_NewFrame sets DisplaySize
    // THPrac's Gui::ImplWin32NewFrame removed that line.
    RECT cr;
    GetClientRect(hwnd, &cr);
    io.DisplaySize = {
        (float)cr.right - (float)cr.left,
        (float)cr.bottom - (float)cr.top
    };

    Gui::LocaleCreateFont(20.0f);
    if (!UpdateUIScaling(dpiscale > 1.0f ? dpiscale : 1.0f)) {
        return 1;
    }

    // Show the window
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    // Main loop
    MSG msg = {};
    g_IsInitialized = true;
    while (msg.message != WM_QUIT) {
        // Poll and handle messages (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        if (PeekMessageW(&msg, NULL, 0U, 0U, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }
    g_IsInitialized = false;

    return 0;
}

#ifndef USER_DEFAULT_SCREEN_DPI
#define USER_DEFAULT_SCREEN_DPI 96
#endif
#ifndef WM_DPICHANGED
#define WM_DPICHANGED 0x02E0
#endif

namespace Gui {
    extern LRESULT ImplWin32WndProcHandlerW(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
}
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (Gui::ImplWin32WndProcHandlerW(hWnd, msg, wParam, lParam))
        return true;

    switch (msg) {
    case WM_GETMINMAXINFO: {
        LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;
        lpMMI->ptMinTrackSize.x = 640;
        lpMMI->ptMinTrackSize.y = 480;
        break;
    }
    case WM_SIZE:
        if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED) {
            g_d3dpp.BackBufferWidth = LOWORD(lParam);
            g_d3dpp.BackBufferHeight = HIWORD(lParam);
            ImGui::GetIO().DisplaySize = {
                (float)g_d3dpp.BackBufferWidth,
                (float)g_d3dpp.BackBufferHeight
            };
            ResetDevice();
        }
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;
    case WM_PAINT:
        UiUpdate();
        return 0;
    case WM_DPICHANGED: {
        RECT* rect = (RECT*)lParam;
        IM_ASSERT(LOWORD(wParam) == HIWORD(wParam));
        SetWindowPos(hWnd, NULL, rect->left, rect->top, rect->right - rect->left, rect->bottom - rect->top, SWP_NOZORDER);
        UpdateUIScaling((float)LOWORD(wParam) / (float)USER_DEFAULT_SCREEN_DPI);
    }
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcW(hWnd, msg, wParam, lParam);
}

void ResetDevice() {
    g_IsUITextureIDValid = false;
    Gui::ImplDX9InvalidateDeviceObjects();
    HRESULT hr = g_pd3dDevice->Reset(&g_d3dpp);
    if (hr == D3DERR_INVALIDCALL)
        IM_ASSERT(0);
    Gui::ImplDX9CreateDeviceObjects();
}

bool UpdateUIScaling(float scale) {
    // Not rebuilding the font atlas here because it doesn't seem to do anything
    ImGuiIO& io = ImGui::GetIO();

    g_IsUITextureIDValid = false;
    Gui::ImplDX9InvalidateDeviceObjects();
    
    // Setup Dear ImGui style
    ImGuiStyle& style = ImGui::GetStyle();
    ImGuiStyle styleold = style; // Backup colors
    style = ImGuiStyle(); // IMPORTANT: ScaleAllSizes will change the original size, so we should reset all style config
    ImGui::GetStyle().ScaleAllSizes(scale);
    memcpy(style.Colors, styleold.Colors, sizeof(style.Colors)); // Restore colors
    return Gui::ImplDX9CreateDeviceObjects();
}
}