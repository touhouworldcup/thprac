#include "thprac_launcher_wnd.h"
#include "imgui.h"
#include "thprac_gui_impl_dx9.h"
#include "thprac_gui_impl_win32.h"
#include "thprac_gui_locale.h"
#include "thprac_launcher_utils.h"
#include "..\..\resource.h"
#include <d3d9.h>
#include <tchar.h>
#pragma warning(disable : 4091)
#include <Shlobj.h>
#pragma warning(default : 4091)

typedef HRESULT(WINAPI* PSetProcessDpiAwareness)(DWORD value);
typedef HRESULT(WINAPI* PGetDpiForMonitor)(HMONITOR hmonitor, DWORD dpiType, UINT* dpiX, UINT* dpiY);

namespace THPrac {
namespace Gui {
    extern LRESULT ImplWin32WndProcHandlerW(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
}

struct Image {
    int width;
    int height;
    int pitch;
    HDC hdc;
    HBITMAP hBitmap;
    BITMAPINFO info;
    BYTE* pPixels;
};

static float __thprac_lc_scale = 1.0f;
static bool __thprac_lc_hasInited = false;
static bool __thprac_lc_transparency = false;
static HWND __thprac_lc_hwnd;
static bool __thprac_lc_canMove = false;
static HANDLE __thprac_lc_mutex;
static MSG __thprac_lc_msg;
static WNDCLASSEX __thprac_lc_wc;
static Image g_image;
static LPDIRECT3D9 g_pD3D = NULL;
static LPDIRECT3DDEVICE9 g_pd3dDevice = NULL;
static D3DPRESENT_PARAMETERS g_d3dpp = {};
static IDirect3DTexture9* g_pTexture = nullptr;
static IDirect3DSurface9* g_pSurface = nullptr;
static IDirect3DSurface9* g_pRenderTargetSurface = nullptr;
static IDirect3DSurface9* g_pDepthStencilSurface = nullptr;

// D3D Functions

bool D3DCreateDevice(HWND hWnd, unsigned int width, unsigned int height)
{
    // Create D3D object using LoadLibrary
    auto d3d9Lib = LoadLibraryA("d3d9.dll");
    if (!d3d9Lib)
        return false;
    decltype(Direct3DCreate9)* d3dCreate9 = (decltype(Direct3DCreate9)*)GetProcAddress(d3d9Lib, "Direct3DCreate9");
    if (!d3dCreate9 || (g_pD3D = d3dCreate9(D3D_SDK_VERSION)) == NULL)
        return false;

    // Create the D3DDevice
    ZeroMemory(&g_d3dpp, sizeof(g_d3dpp));
    g_d3dpp.Windowed = TRUE;
    g_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    g_d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
    g_d3dpp.EnableAutoDepthStencil = TRUE;
    g_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
    g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE; // Present with vsync
    //g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;   // Present without vsync, maximum unthrottled framerate
    g_d3dpp.BackBufferWidth = width;
    g_d3dpp.BackBufferHeight = height;
    if (g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &g_d3dpp, &g_pd3dDevice) < 0)
        return false;

    return true;
}

void D3DCleanupDevice()
{
    if (g_pd3dDevice) {
        g_pd3dDevice->Release();
        g_pd3dDevice = NULL;
    }
    if (g_pD3D) {
        g_pD3D->Release();
        g_pD3D = NULL;
    }
}

void D3DResetDevice()
{
    Gui::ImplDX9InvalidateDeviceObjects();
    HRESULT hr = g_pd3dDevice->Reset(&g_d3dpp);
    if (hr == D3DERR_INVALIDCALL)
        IM_ASSERT(0);
    Gui::ImplDX9CreateDeviceObjects();
}

// Layered Window Function

void ImageDestroy(Image* pImage)
{
    if (!pImage)
        return;

    pImage->width = 0;
    pImage->height = 0;
    pImage->pitch = 0;

    if (pImage->hBitmap) {
        DeleteObject(pImage->hBitmap);
        pImage->hBitmap = 0;
    }

    if (pImage->hdc) {
        DeleteDC(pImage->hdc);
        pImage->hdc = 0;
    }

    memset(&pImage->info, 0, sizeof(pImage->info));
    pImage->pPixels = 0;
}

bool ImageCreate(Image* pImage, int width, int height)
{
    if (!pImage)
        return false;

    if (pImage->hBitmap)
        ImageDestroy(pImage);

    // All Windows DIBs are aligned to 4-byte (DWORD) memory boundaries. This
    // means that each scan line is padded with extra bytes to ensure that the
    // next scan line starts on a 4-byte memory boundary. The 'pitch' member
    // of the Image structure contains width of each scan line (in bytes).

    pImage->width = width;
    pImage->height = height;
    pImage->pitch = ((width * 32 + 31) & ~31) >> 3;
    pImage->pPixels = NULL;
    pImage->hdc = CreateCompatibleDC(NULL);

    if (!pImage->hdc)
        return false;

    memset(&pImage->info, 0, sizeof(pImage->info));

    pImage->info.bmiHeader.biSize = sizeof(pImage->info.bmiHeader);
    pImage->info.bmiHeader.biBitCount = 32;
    pImage->info.bmiHeader.biWidth = width;
    pImage->info.bmiHeader.biHeight = -height;
    pImage->info.bmiHeader.biCompression = BI_RGB;
    pImage->info.bmiHeader.biPlanes = 1;

    pImage->hBitmap = CreateDIBSection(pImage->hdc, &pImage->info,
        DIB_RGB_COLORS, (void**)&pImage->pPixels, NULL, 0);

    if (!pImage->hBitmap) {
        ImageDestroy(pImage);
        return false;
    }

    GdiFlush();
    return true;
}

BOOL InitRenderToTexture(D3DFORMAT format, D3DFORMAT depthStencil)
{
    HRESULT hr = 0;
    int width = g_image.width;
    int height = g_image.height;

    if (g_pSurface)
        g_pSurface->Release();
    if (g_pDepthStencilSurface)
        g_pDepthStencilSurface->Release();
    if (g_pRenderTargetSurface)
        g_pRenderTargetSurface->Release();
    if (g_pTexture)
        g_pTexture->Release();
    g_pTexture = nullptr;
    g_pRenderTargetSurface = nullptr;
    g_pDepthStencilSurface = nullptr;
    g_pSurface = nullptr;

    // Create the dynamic render target texture. Since we want use this texture
    // with the Win32 layered windows API to create a per pixel alpha blended
    // non-rectangular window we *must* use a D3DFORMAT that contains an ALPHA
    // channel.
    // = D3DXCreateTexture(g_pDevice, width, height, 0, D3DUSAGE_RENDERTARGET,
    //    format, D3DPOOL_DEFAULT, &g_pTexture);
    hr = g_pd3dDevice->CreateTexture(width, height, 0, D3DUSAGE_RENDERTARGET, format, D3DPOOL_DEFAULT, &g_pTexture, 0);

    if (FAILED(hr))
        return FALSE;

    // Cache the top level surface of the render target texture. This will make
    // it easier to bind the dynamic render target texture to the device.
    hr = g_pTexture->GetSurfaceLevel(0, &g_pRenderTargetSurface);

    if (FAILED(hr))
        return FALSE;

    // Create a depth-stencil surface so the scene rendered to the dynamic
    // texture will be correctly depth sorted.
    hr = g_pd3dDevice->CreateDepthStencilSurface(width, height, depthStencil,
        D3DMULTISAMPLE_NONE, 0, TRUE, &g_pDepthStencilSurface, 0);

    if (FAILED(hr))
        return FALSE;

    // Create an off-screen plain system memory surface. This system memory
    // surface will be used to fetch a copy of the pixel data rendered into the
    // render target texture. We can't directly read the render target texture
    // because textures created with D3DPOOL_DEFAULT can't be locked.
    hr = g_pd3dDevice->CreateOffscreenPlainSurface(width, height,
        format, D3DPOOL_SYSTEMMEM, &g_pSurface, 0);

    return TRUE;
}

void CleanupRenderToTexture()
{

    if (g_pSurface) {
        g_pSurface->Release();
        g_pSurface = 0;
    }

    if (g_pDepthStencilSurface) {
        g_pDepthStencilSurface->Release();
        g_pDepthStencilSurface = 0;
    }

    if (g_pRenderTargetSurface) {
        g_pRenderTargetSurface->Release();
        g_pRenderTargetSurface = 0;
    }

    if (g_pTexture) {
        g_pTexture->Release();
        g_pTexture = 0;
    }

    ImageDestroy(&g_image);
}

void CopyRenderTextureToImage()
{
    // Dynamic render target textures are created with D3DPOOL_DEFAULT and so
    // cannot be locked. To access the dynamic texture's pixel data we need to
    // get Direct3D to make a system memory copy of the texture. The system
    // memory copy can then be accessed and copies across to our Image
    // structure. This copying from video memory to system memory is slow.
    // So try not to make your dynamic textures too big if you intend to access
    // their pixel data.

    HRESULT hr = 0;
    D3DLOCKED_RECT rcLock = { 0 };

    hr = g_pd3dDevice->GetRenderTargetData(g_pRenderTargetSurface, g_pSurface);

    if (SUCCEEDED(hr)) {
        hr = g_pSurface->LockRect(&rcLock, 0, 0);

        if (SUCCEEDED(hr)) {
            const BYTE* pSrc = (const BYTE*)rcLock.pBits;
            BYTE* pDest = g_image.pPixels;
            int srcPitch = rcLock.Pitch;
            int destPitch = g_image.pitch;

            if (srcPitch == destPitch) {
                memcpy(pDest, pSrc, destPitch * g_image.height);
            } else {
                for (int i = 0; i < g_image.height; ++i)
                    memcpy(&pDest[destPitch * i], &pSrc[srcPitch * i], destPitch);
            }

            g_pSurface->UnlockRect();
        }
    }
}

void ImagePreMultAlpha(Image* pImage)
{
    // The per pixel alpha blending API for layered windows deals with
    // pre-multiplied alpha values in the RGB channels. For further details see
    // the MSDN documentation for the BLENDFUNCTION structure. It basically
    // means we have to multiply each red, green, and blue channel in our image
    // with the alpha value divided by 255.
    //
    // Notes:
    // 1. ImagePreMultAlpha() needs to be called before every call to
    //    UpdateLayeredWindow() (in the RedrawLayeredWindow() function).
    //
    // 2. Must divide by 255.0 instead of 255 to prevent alpha values in range
    //    [1, 254] from causing the pixel to become black. This will cause a
    //    conversion from 'float' to 'BYTE' possible loss of data warning which
    //    can be safely ignored.

    if (!pImage)
        return;

    BYTE* pPixel = NULL;

    if (pImage->width * 4 == pImage->pitch) {
        // This is a special case. When the image width is already a multiple
        // of 4 the image does not require any padding bytes at the end of each
        // scan line. Consequently we do not need to address each scan line
        // separately. This is much faster than the below case where the image
        // width is not a multiple of 4.

        int totalBytes = pImage->width * pImage->height * 4;

        for (int i = 0; i < totalBytes; i += 4) {
            pPixel = &pImage->pPixels[i];
            //pPixel[0] *= (BYTE)((float)pPixel[3] / 255.0f);
            //pPixel[1] *= (BYTE)((float)pPixel[3] / 255.0f);
            //pPixel[2] *= (BYTE)((float)pPixel[3] / 255.0f);
            pPixel[0] = (BYTE)((float)pPixel[0] * (float)pPixel[3] / 255.0f);
            pPixel[1] = (BYTE)((float)pPixel[1] * (float)pPixel[3] / 255.0f);
            pPixel[2] = (BYTE)((float)pPixel[2] * (float)pPixel[3] / 255.0f);
        }
    } else {
        // Width of the image is not a multiple of 4. So padding bytes have
        // been included in the DIB's pixel data. Need to address each scan
        // line separately. This is much slower than the above case where the
        // width of the image is already a multiple of 4.

        for (int y = 0; y < pImage->height; ++y) {
            for (int x = 0; x < pImage->width; ++x) {
                pPixel = &pImage->pPixels[(y * pImage->pitch) + (x * 4)];
                //pPixel[0] *= (BYTE)((float)pPixel[3] / 255.0f);
                //pPixel[1] *= (BYTE)((float)pPixel[3] / 255.0f);
                //pPixel[2] *= (BYTE)((float)pPixel[3] / 255.0f);
                pPixel[0] = (BYTE)((float)pPixel[0] * (float)pPixel[3] / 255.0f);
                pPixel[1] = (BYTE)((float)pPixel[1] * (float)pPixel[3] / 255.0f);
                pPixel[2] = (BYTE)((float)pPixel[2] * (float)pPixel[3] / 255.0f);
            }
        }
    }
}

void RedrawLayeredWindow()
{
    // The call to UpdateLayeredWindow() is what makes a non-rectangular
    // window possible. To enable per pixel alpha blending we pass in the
    // argument ULW_ALPHA, and provide a BLENDFUNCTION structure filled in
    // to do per pixel alpha blending.

    HDC hdc = GetDC(__thprac_lc_hwnd);

    if (hdc) {
        HGDIOBJ hPrevObj = 0;
        POINT ptDest = { 0, 0 };
        POINT ptSrc = { 0, 0 };
        SIZE client = { g_image.width, g_image.height };
        BLENDFUNCTION blendFunc = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };

        hPrevObj = SelectObject(g_image.hdc, g_image.hBitmap);
        ClientToScreen(__thprac_lc_hwnd, &ptDest);

        UpdateLayeredWindow(__thprac_lc_hwnd, hdc, &ptDest, &client,
            g_image.hdc, &ptSrc, 0, &blendFunc, ULW_ALPHA);

        SelectObject(g_image.hdc, hPrevObj);
        ReleaseDC(__thprac_lc_hwnd, hdc);
    }
}

// Win32 Functions

void ResizeWindow(HWND hwnd, ImVec2& wndPos, ImVec2& wndSize)
{
    //RECT rect;
    RECT wndRect;
    //::GetClientRect(hwnd, &rect);
    ::GetWindowRect(hwnd, &wndRect);
    if ((LONG)wndSize.x != wndRect.right - wndRect.left || (LONG)wndSize.y != wndRect.bottom - wndRect.top) {
        RECT rect = { 0, 0, (LONG)wndSize.x, (LONG)wndSize.y };
        ::AdjustWindowRectEx(&rect, WS_POPUP, FALSE, WS_EX_APPWINDOW); // Client to Screen
        ::SetWindowPos(hwnd, NULL,
            wndRect.left + (LONG)wndPos.x, wndRect.top + (LONG)wndPos.y,
            rect.right - rect.left, rect.bottom - rect.top,
            SWP_NOZORDER | SWP_NOACTIVATE);

        ImGuiIO& io = ImGui::GetIO();
        io.DisplaySize.x = (float)wndSize.x;
        io.DisplaySize.y = (float)wndSize.y;

        g_d3dpp.BackBufferWidth = (LONG)wndSize.x;
        g_d3dpp.BackBufferHeight = (LONG)wndSize.y;
        CleanupRenderToTexture();
        D3DResetDevice();

        ImageCreate(&g_image, (int)wndSize.x, (int)wndSize.y);
        InitRenderToTexture(D3DFMT_A8R8G8B8, D3DFMT_D24S8);
    }
}

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (Gui::ImplWin32WndProcHandlerW(hWnd, msg, wParam, lParam))
        return true;

    switch (msg) {
    case WM_GETMINMAXINFO: {
        LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;
        lpMMI->ptMinTrackSize.x = 300;
        lpMMI->ptMinTrackSize.y = 300;
    }
        return 0;
    case WM_SIZE:
        //if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
        //{
        //    g_d3dpp.BackBufferWidth = LOWORD(lParam);
        //    g_d3dpp.BackBufferHeight = HIWORD(lParam);
        //    D3DResetDevice();
        //}
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;
    case WM_NCHITTEST:
        if (__thprac_lc_canMove)
            return HTCAPTION; // allows dragging of the window
        else
            break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }
    return ::DefWindowProc(hWnd, msg, wParam, lParam);
}

bool WndMsgUpdate(HWND hWnd = NULL)
{
    MSG msg;
    while (::PeekMessage(&msg, hWnd, 0U, 0U, PM_REMOVE)) {
        ::TranslateMessage(&msg);
        ::DispatchMessage(&msg);
        if (msg.message == WM_QUIT)
            return false;
    }
    return true;
}

// Launcher Functions
void GetDesktopResolution(int& horizontal, int& vertical)
{
    RECT desktop;
    // Get a handle to the desktop window
    const HWND hDesktop = GetDesktopWindow();
    // Get the size of screen to the variable desktop
    GetWindowRect(hDesktop, &desktop);
    // The top left corner will have coordinates (0,0)
    // and the bottom right corner will have coordinates
    // (horizontal, vertical)
    horizontal = desktop.right;
    vertical = desktop.bottom;
}

int LauncherWndInit(unsigned int width, unsigned int height, unsigned int maxWidth, unsigned int maxHeight, unsigned int widthCurrent, unsigned int heightCurrent)
{
    if (__thprac_lc_hasInited)
        return 0;

    __thprac_lc_mutex = CreateMutex(NULL, TRUE, L"thprac launcher mutex");
    if (__thprac_lc_mutex == NULL || GetLastError() == ERROR_ALREADY_EXISTS) {
        for (int i = 0; i < 3; ++i) {
            auto tmpTitle = utf8_to_utf16(th_glossary_str[i][THPRAC_LAUNCHER]);
            HWND existingApp = FindWindowW(0, tmpTitle.c_str());
            if (existingApp) {
                ShowWindow(existingApp, SW_RESTORE);
                SetForegroundWindow(existingApp);
            }
        }
        return 0;
    }

    // Create application window
    auto windowTitle = utf8_to_utf16(XSTR(THPRAC_LAUNCHER));
    auto icon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1));
    __thprac_lc_wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), icon, NULL, NULL, NULL, _T("thprac launcher window"), NULL };
    ::RegisterClassEx(&__thprac_lc_wc);
    __thprac_lc_hwnd = ::CreateWindow(__thprac_lc_wc.lpszClassName, windowTitle.c_str(),
        WS_POPUP | WS_SYSMENU | WS_MINIMIZEBOX, 0, 0, width, height,
        NULL, NULL, __thprac_lc_wc.hInstance, NULL);

    // DPI handling
    DEVMODE devMod;
    EnumDisplaySettingsW(NULL, ENUM_CURRENT_SETTINGS, &devMod);
    auto displayX = devMod.dmPelsWidth;
    auto displayY = devMod.dmPelsHeight;
    //auto displayX = GetSystemMetrics(SM_CXSCREEN);
    //auto displayY = GetSystemMetrics(SM_CYSCREEN);
    //GetDesktopResolution(displayX, displayY);
    PSetProcessDpiAwareness setProcDpiAwareness = nullptr;
    PGetDpiForMonitor getDpiForMonitor = nullptr;
    setProcDpiAwareness = (PSetProcessDpiAwareness)GetProcAddress(GetModuleHandleA("shcore.dll"), "SetProcessDpiAwareness");
    getDpiForMonitor = (PGetDpiForMonitor)GetProcAddress(GetModuleHandleA("shcore.dll"), "GetDpiForMonitor");
    if (setProcDpiAwareness && getDpiForMonitor) {
        UINT dpiX;
        UINT dpiY;
        setProcDpiAwareness(1);
        getDpiForMonitor(MonitorFromWindow(__thprac_lc_hwnd, MONITOR_DEFAULTTONEAREST), 0, &dpiX, &dpiY);
        if (dpiX != 96) {
            __thprac_lc_scale = (float)dpiX / 96.0f;
            width = (unsigned int)((float)width * __thprac_lc_scale);
            height = (unsigned int)((float)height * __thprac_lc_scale);
            maxWidth = (unsigned int)((float)maxWidth * __thprac_lc_scale);
            maxHeight = (unsigned int)((float)maxHeight * __thprac_lc_scale);
            widthCurrent = (unsigned int)((float)widthCurrent * __thprac_lc_scale);
            heightCurrent = (unsigned int)((float)heightCurrent * __thprac_lc_scale);
            //displayX = (unsigned int)((float)displayX / __thprac_lc_scale);
            //displayY = (unsigned int)((float)displayY / __thprac_lc_scale);
        }
    }
    auto wndPosX = (displayX - widthCurrent) / 2;
    auto wndPosY = (displayY - heightCurrent) / 2;
    SetWindowPos(__thprac_lc_hwnd, 0, wndPosX, wndPosY, widthCurrent, heightCurrent, 0);

    // Initialize Direct3D
    if (!D3DCreateDevice(__thprac_lc_hwnd, maxWidth, maxHeight)) {
        D3DCleanupDevice();
        ::UnregisterClass(__thprac_lc_wc.lpszClassName, __thprac_lc_wc.hInstance);
        return -1;
    }

    // Show the window
    ::ShowWindow(__thprac_lc_hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(__thprac_lc_hwnd);
    MovWndToTop(__thprac_lc_hwnd);

    ImGui::CreateContext();
    Gui::ImplWin32Init(__thprac_lc_hwnd);
    Gui::ImplDX9Init(g_pd3dDevice);
    Gui::LocaleCreateMergeFont(Gui::LocaleGet(), 20.0f * __thprac_lc_scale); //30.0f LOCALE_ZH_CN LOCALE_EN_US LOCALE_JA_JP
    if (__thprac_lc_scale != 1.0f) {
        ImGui::GetStyle().ScaleAllSizes(__thprac_lc_scale);
    }

    ImGuiIO& io = ImGui::GetIO();
    ImGuiStyle& style = ImGui::GetStyle();
    ImGui::StyleColorsDark();
    io.IniFilename = nullptr;
    style.WindowRounding = 0.0f;
    style.ScrollbarRounding = 0.0f;
    style.WindowBorderSize = 0.0f;
    io.ImeWindowHandle = __thprac_lc_hwnd;
    io.DisplaySize = ImVec2((float)maxWidth, (float)(maxHeight));
    io.KeyMap[ImGuiKey_Tab] = VK_TAB;
    io.KeyMap[ImGuiKey_LeftArrow] = VK_LEFT;
    io.KeyMap[ImGuiKey_RightArrow] = VK_RIGHT;
    io.KeyMap[ImGuiKey_UpArrow] = VK_UP;
    io.KeyMap[ImGuiKey_DownArrow] = VK_DOWN;
    io.KeyMap[ImGuiKey_PageUp] = VK_PRIOR;
    io.KeyMap[ImGuiKey_PageDown] = VK_NEXT;
    io.KeyMap[ImGuiKey_Home] = VK_HOME;
    io.KeyMap[ImGuiKey_End] = VK_END;
    io.KeyMap[ImGuiKey_Insert] = VK_INSERT;
    io.KeyMap[ImGuiKey_Delete] = VK_DELETE;
    io.KeyMap[ImGuiKey_Backspace] = VK_BACK;
    io.KeyMap[ImGuiKey_Space] = VK_SPACE;
    io.KeyMap[ImGuiKey_Enter] = VK_RETURN;
    io.KeyMap[ImGuiKey_Escape] = VK_ESCAPE;
    io.KeyMap[ImGuiKey_KeyPadEnter] = VK_RETURN;
    io.KeyMap[ImGuiKey_A] = 'A';
    io.KeyMap[ImGuiKey_C] = 'C';
    io.KeyMap[ImGuiKey_V] = 'V';
    io.KeyMap[ImGuiKey_X] = 'X';
    io.KeyMap[ImGuiKey_Y] = 'Y';
    io.KeyMap[ImGuiKey_Z] = 'Z';

    return 1;
}

bool LauncherWndNewFrame()
{
    if (!WndMsgUpdate())
        return false;

    while (IsIconic(__thprac_lc_hwnd)) {
        HRESULT result = g_pd3dDevice->Present(NULL, NULL, NULL, NULL);
        // Handle loss of D3D9 device
        if (result == D3DERR_DEVICELOST && g_pd3dDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
            D3DResetDevice();
        if (!WndMsgUpdate())
            return false;
    }

    ImGuiIO& io = ImGui::GetIO();
    io.KeyCtrl = (::GetKeyState(VK_CONTROL) & 0x8000) != 0;
    io.KeyShift = (::GetKeyState(VK_SHIFT) & 0x8000) != 0;
    io.KeyAlt = (::GetKeyState(VK_MENU) & 0x8000) != 0;
    io.KeySuper = false;

    //ResizeWindow(__thprac_lc_hwnd, wndPos, wndSize);
    Gui::ImplDX9NewFrame();
    Gui::ImplWin32NewFrame(false);
    ImGui::NewFrame();
    Gui::LocalePushFont();

    return true;
}

bool LauncherWndEndFrame(ImVec2& wndPos, ImVec2& wndSize, bool canMove)
{
    __thprac_lc_canMove = canMove;
    static bool moved = false;

    RECT wndRect;
    ::GetWindowRect(__thprac_lc_hwnd, &wndRect);
    if ((LONG)wndSize.x != wndRect.right - wndRect.left || (LONG)wndSize.y != wndRect.bottom - wndRect.top) {
        moved = true;
        RECT rect = { 0, 0, (LONG)wndSize.x, (LONG)wndSize.y };
        ::AdjustWindowRectEx(&rect, WS_POPUP, FALSE, WS_EX_APPWINDOW); // Client to Screen
        ::SetWindowPos(__thprac_lc_hwnd, NULL,
            wndRect.left + (LONG)wndPos.x, wndRect.top + (LONG)wndPos.y,
            rect.right - rect.left, rect.bottom - rect.top,
            SWP_NOZORDER | SWP_NOACTIVATE);
    }
    ::GetClientRect(__thprac_lc_hwnd, &wndRect);
    RECT renderRect = { (LONG)wndPos.x, (LONG)wndPos.y,
        (LONG)(wndSize.x) + (LONG)(wndPos.x), (LONG)(wndSize.y) + (LONG)(wndPos.y) };

    //ResizeWindow(__thprac_lc_hwnd, wndPos, wndSize);
    Gui::LocalePopFont();
    ImGui::EndFrame();

    // Rendering
    g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, false);
    g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
    g_pd3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, false);

    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    D3DCOLOR clear_col_dx = D3DCOLOR_RGBA(
        (int)(clear_color.x * 255.0f), (int)(clear_color.y * 255.0f),
        (int)(clear_color.z * 255.0f), (int)(clear_color.w * 255.0f));
    g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, clear_col_dx, 1.0f, 0);

    if (g_pd3dDevice->BeginScene() >= 0) {
        ImGui::Render();
        Gui::ImplDX9RenderDrawData(ImGui::GetDrawData());
        g_pd3dDevice->EndScene();
    }

    HRESULT result = g_pd3dDevice->Present(&renderRect, NULL, NULL, NULL);
    // Handle loss of D3D9 device
    if (result == D3DERR_DEVICELOST && g_pd3dDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
        D3DResetDevice();

    if (moved) {
        if (!(GetKeyState(VK_LBUTTON) < 0)) {
            moved = false;
        }
    }
    ImGui::GetIO().DisplaySize = moved ? ImVec2(5000.0f, 5000.0f) : ImVec2(wndSize.x, wndSize.y);

    return true;
}

bool LauncherWndMinimize()
{
    return ShowWindow(__thprac_lc_hwnd, SW_MINIMIZE);
}

bool LauncherWndShutdown()
{
    Gui::ImplDX9Shutdown();
    Gui::ImplWin32Shutdown();
    ImGui::DestroyContext();

    D3DCleanupDevice();
    ::DestroyWindow(__thprac_lc_hwnd);
    ::UnregisterClass(__thprac_lc_wc.lpszClassName, __thprac_lc_wc.hInstance);
    ::CloseHandle(__thprac_lc_mutex);

    return true;
}

ImVec2 LauncherWndGetSize()
{
    RECT wndRect {};
    ::GetWindowRect(__thprac_lc_hwnd, &wndRect);
    return ImVec2((float)(wndRect.right - wndRect.left), (float)(wndRect.bottom - wndRect.top));
}

float LauncherWndGetScale()
{
    return __thprac_lc_scale;
}


int CALLBACK _BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
    if (uMsg == BFFM_INITIALIZED) {
        SendMessage(hwnd, BFFM_SETSELECTION, TRUE, lpData);
    }
    return 0;
}
std::wstring LauncherWndFolderSelect(const wchar_t* title)
{
    TCHAR path[MAX_PATH];

    BROWSEINFO bi = { 0 };
    bi.hwndOwner = __thprac_lc_hwnd;
    bi.lpszTitle = (nullptr);
    bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_USENEWUI;
    bi.lpfn = _BrowseCallbackProc;

    LPITEMIDLIST pidl = SHBrowseForFolder(&bi);

    if (pidl != 0) {
        //get the name of the folder and put it in path
        SHGetPathFromIDList(pidl, path);

        //free memory used
        IMalloc* imalloc = 0;
        if (SUCCEEDED(SHGetMalloc(&imalloc))) {
            imalloc->Free(pidl);
            imalloc->Release();
        }
    } else {
        return std::wstring(L"");
    }

    return std::wstring(path);
}

std::wstring LauncherWndFileSelect(const wchar_t* title, const wchar_t* filter) {
    OPENFILENAME ofn;
    wchar_t szFile[MAX_PATH];
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lpstrTitle = nullptr;
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = __thprac_lc_hwnd;
    ofn.lpstrFile = szFile;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = filter;
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NODEREFERENCELINKS;
    GetOpenFileName(&ofn);
    return std::wstring(szFile);
}

int LauncherWndMsgBox(const wchar_t* title, const wchar_t* text, int flag)
{
    return MessageBoxW(__thprac_lc_hwnd, text, title, flag);
}
}
