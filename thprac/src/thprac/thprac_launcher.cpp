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
// so that it doesn't need to be initialized.

// (HINSTANCE)&__ImageBase is considered constinit in MSVC but not Clang,
// so I can't use that to initiialize hInstance right here.
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

static constinit float g_TitleBarHeight;
static constinit bool g_IsOverTitleBarButton = false;

static constinit IDirect3DDevice9* g_pd3dDevice = NULL;
static constinit bool g_IsUITextureIDValid = false;
static constinit bool g_IsInitialized = false;

void ResetDevice();
bool UpdateUIScaling(float scale = 1.0f);

// This title bar is inspriried by the title bar from the old launcher provided by a modified ImGui::Begin,
// but it's remade from scratch and reponds to minimize and close button presses by sending messages to 'hwnd' directly
void DrawTitleBar(HWND hwnd, const char* title) {

    // The old launcher enabled the title bar when calling ImGui::Begin and used the buttons provided by that for it's title bar
    // However, I haven't found a way to check if the title bar is selected, or if any of the title bar's buttons are selected
    // It also seems like the title bar code was modified by thprac's previous behaviour, ACK. I want to cut down on non-standard
    // Dear ImGui as much as possible to make it easier to upgrade Dear ImGui in the future.

    auto& io = ImGui::GetIO();
    const float btnW = g_TitleBarHeight;

    ImGui::GetWindowDrawList()->AddRectFilled(
        ImVec2(0.0f, 0.0f),
        ImVec2(io.DisplaySize.x, btnW),
        ImGui::GetColorU32(ImGuiCol_TitleBgActive));


    ImGui::SetCursorPos(ImVec2(4.0f, (btnW - ImGui::GetTextLineHeight()) * 0.5f));
    ImGui::TextUnformatted(title);

    bool overBtn = false;
    constexpr float cross_extent = 20.0f * 0.5f * 0.7071f - 1.0f;

    // Minimize Button
    {
        ImVec2 btnPos = { io.DisplaySize.x - btnW * 2.0f, 0.0f };
        ImVec2 btnCenter = { btnPos.x + (btnW / 2), btnPos.y + (btnW / 2) };

        ImGui::SetCursorPos(btnPos);
        if (ImGui::InvisibleButton("##MINIMIZE", { btnW, btnW })) {
            ShowWindow(hwnd, SW_MINIMIZE);
        }
        bool hovered = ImGui::IsItemHovered();

        if (hovered) {
            ImGui::GetWindowDrawList()->AddCircleFilled(btnCenter, (btnW / 2) - 2.0f, ImGui::GetColorU32(ImGuiCol_ButtonHovered));
        }    

        ImVec2 lineLeft = btnCenter;
        ImVec2 lineRight = btnCenter;

        lineLeft.x -= cross_extent;
        lineRight.x += cross_extent;

        ImGui::GetWindowDrawList()->AddLine(lineLeft, lineRight, ImGui::GetColorU32(ImGuiCol_Text), 1.5f);         

        overBtn |= hovered;
    }

    // Close button
    {
        ImVec2 btnPos = { io.DisplaySize.x - btnW, 0.0f };
        ImVec2 btnCenter = { btnPos.x + (btnW / 2), btnPos.y + (btnW / 2) };

        ImGui::SetCursorPos(btnPos);
        if (ImGui::InvisibleButton("##CLOSE", { btnW, btnW })) {
            PostQuitMessage(0);
        }
        bool hovered = ImGui::IsItemHovered();
        if (hovered) {
            ImGui::GetWindowDrawList()->AddCircleFilled(btnCenter, (btnW / 2) - 2.0f, ImGui::GetColorU32(ImGuiCol_ButtonHovered));
        }

        ImVec2 lineTopLeft = btnCenter;
        ImVec2 lineTopRight = btnCenter;
        ImVec2 lineBottomLeft = btnCenter;
        ImVec2 lineBottomRight = btnCenter;

        lineTopLeft.x -= cross_extent;
        lineTopLeft.y -= cross_extent;
        
        lineTopRight.x += cross_extent;
        lineTopRight.y -= cross_extent;      

        lineBottomLeft.x -= cross_extent;
        lineBottomLeft.y += cross_extent;

        lineBottomRight.x += cross_extent;
        lineBottomRight.y += cross_extent;

        ImGui::GetWindowDrawList()->AddLine(lineBottomRight, lineTopLeft, ImGui::GetColorU32(ImGuiCol_Text));
        ImGui::GetWindowDrawList()->AddLine(lineTopRight, lineBottomLeft, ImGui::GetColorU32(ImGuiCol_Text));     

        overBtn |= hovered;
    }

    // Record whether the mouse is over a button so WndProc can skip HTCAPTION
    g_IsOverTitleBarButton = overBtn;
}


void UiUpdate(HWND hwnd) {
    if (!g_IsInitialized)
        return;

    // Start the Dear ImGui frame
    Gui::ImplDX9NewFrame();
    Gui::ImplWin32NewFrame();
    g_IsUITextureIDValid = true;
    ImGui::NewFrame();

    auto& io = ImGui::GetIO();
    auto& style = ImGui::GetStyle();
    style.WindowBorderSize = 0.0f;
    
    ImGui::SetNextWindowPos({ 0, 0 });
    ImGui::SetNextWindowSize(io.DisplaySize);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0.0f, 0.0f });
    
    // The title bar cannot have inner padding, but I want to have inner padding for the rest of the UI
    ImGui::Begin("###_outer_window", nullptr,
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize);
    DrawTitleBar(hwnd, "thprac - Touhou Game Launcher");
    ImGui::PopStyleVar(1);
    ImGui::SetNextWindowPos({ 0.0f, g_TitleBarHeight });
    ImGui::SetNextWindowSize({ io.DisplaySize.x, io.DisplaySize.y - g_TitleBarHeight });

    // So I put all of my actual content into a sub-window with padding re-enabled
    ImGui::Begin("###_main_window", nullptr,
         ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::BeginTabBar("__launcher_tab_bar");
   
    if (ImGui::BeginTabItem(S(THPRAC_LAUNCHER_TAB_GAMES))) {
        ImGui::TextUnformatted("Games");
        ImGui::EndTabItem();
    }
    if (ImGui::BeginTabItem(S(THPRAC_LAUNCHER_TAB_LINKS))) {
        ImGui::TextUnformatted("Links");
        ImGui::EndTabItem();
    }
    if (ImGui::BeginTabItem(S(THPRAC_LAUNCHER_TAB_TOOLS))) {
        ImGui::TextUnformatted("Tools");
        ImGui::EndTabItem();
    }
    if (ImGui::BeginTabItem(S(THPRAC_LAUNCHER_TAB_CONFG))) {
        GuiSettings();
        ImGui::EndTabItem();
    }

    ImGui::EndTabBar();

    ImGui::End();
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
    SetTheme(gSettings.theme);

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

    Gui::LocaleCreateMergeFont(20.0f);
    if (!UpdateUIScaling(dpiscale > 1.0f ? dpiscale : 1.0f)) {
        return 1;
    }

    // Send WM_NCCALCSIZE message immediately
    SetWindowPos(hwnd, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);

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
    SaveSettings();
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
    case WM_NCCALCSIZE: 
        if (wParam == TRUE) {
            if (IsZoomed(hWnd)) {
                auto* params = (NCCALCSIZE_PARAMS*)lParam;
                const int cx = GetSystemMetrics(SM_CXSIZEFRAME) + GetSystemMetrics(SM_CXPADDEDBORDER);
                const int cy = GetSystemMetrics(SM_CYSIZEFRAME) + GetSystemMetrics(SM_CXPADDEDBORDER);
                params->rgrc[0].left += cx;
                params->rgrc[0].top += cy;
                params->rgrc[0].right -= cx;
                params->rgrc[0].bottom -= cy;
            }
            return 0; // use the (adjusted) rect as-is → no non-client area
        }
        break;
    case WM_NCHITTEST: {
        // Let DefWindowProc handle any residual non-client hits first
        LRESULT hit = DefWindowProcW(hWnd, msg, wParam, lParam);
        if (hit != HTCLIENT)
            return hit;

        POINT pt = { LOWORD(lParam), HIWORD(lParam) };
        ScreenToClient(hWnd, &pt);

        RECT rc;
        GetClientRect(hWnd, &rc);

        int border = 4; // resize-grip thickness in pixels
        int titleH = (int)g_TitleBarHeight;

        if (!g_IsOverTitleBarButton && !IsZoomed(hWnd)) {
            // Corners (tested before edges to take priority)
            if (pt.x < border && pt.y >= rc.bottom - border)
                return HTBOTTOMLEFT;
            if (pt.x >= rc.right - border && pt.y >= rc.bottom - border)
                return HTBOTTOMRIGHT;
            // Edges
            if (pt.y >= rc.bottom - border)
                return HTBOTTOM;
            if (pt.x < border)
                return HTLEFT;
            if (pt.x >= rc.right - border)
                return HTRIGHT;
        }

        if (pt.y < titleH && !g_IsOverTitleBarButton)
            return HTCAPTION; // enables drag, double-click maximise, system menu on right-click

        return HTCLIENT;    
    }
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
        UiUpdate(hWnd);
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
    g_TitleBarHeight = 26.0f * scale;
    ImGuiStyle& style = ImGui::GetStyle();
    ImGuiStyle styleold = style; // Backup colors
    style = ImGuiStyle(); // IMPORTANT: ScaleAllSizes will change the original size, so we should reset all style config
    ImGui::GetStyle().ScaleAllSizes(scale);
    style.WindowBorderSize = 0;
    memcpy(style.Colors, styleold.Colors, sizeof(style.Colors)); // Restore colors
    return Gui::ImplDX9CreateDeviceObjects();
}
}