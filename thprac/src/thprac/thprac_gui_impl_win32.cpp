/*
	This file is a modified copy of imgui's Win32 platform implemention's  source file.
	Major changes made:

	1. Includes: Renamed "imgui_impl_win32.h".
	2. Includes: Added "thprac_hook.h".
	2. Function "ImGui_ImplWin32_UpdateMousePos": Added extra code to fix unmatch window/render size.
	3. Function "ImGui_ImplWin32_UpdateGamepads": Emptied function.
	4. Function "ImGui_ImplWin32_NewFrame": Removed sections: Setup display size; Read keyboard modifiers inputs.
	5. Function "ImGui_ImplWin32_HookWndProc": Add definition.
	6. Function "ImGui_ImplWin32_UnHookWndProc": Add definition.
	4. Function "ImGui_ImplWin32_CheckFullScreen": Add definition.

	Last official change: 2019-01-17.
*/

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include "thprac_gui_impl_win32.h"
#include "thprac_hook.h"
#include <imgui.h>
#include <string>
#include <tchar.h>
#include <windows.h>

namespace THPrac {
LRESULT CALLBACK GameExternWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
namespace Gui {
    // Macros
    // Allow compilation with old Windows SDK. MinGW doesn't have default _WIN32_WINNT/WINVER versions.
#ifndef WM_MOUSEHWHEEL
#define WM_MOUSEHWHEEL 0x020E
#endif
#ifndef DBT_DEVNODES_CHANGED
#define DBT_DEVNODES_CHANGED 0x0007
#endif

    // Win32 Data
    static int g_wndNoClose_ = 0;
    static HWND g_hWnd = 0;
    static INT64 g_Time = 0;
    static INT64 g_TicksPerSecond = 0;
    static ImGuiMouseCursor g_LastMouseCursor = ImGuiMouseCursor_COUNT;
    static bool g_HasGamepad = false;
    static bool g_WantUpdateHasGamepad = true;
    struct WndKeyStatus {
        unsigned int hold = 0;
        int frame = 0;
        char name[32] = {};
    } g_wndKeyStatus[256] = {};
    unsigned int g_wndKeyFrameStatus[256] = {};
    bool g_isFullscreenCache = false;

    // Key
    void ImplWin32UpdKeyStatus(UINT msg, WPARAM wParam, LPARAM lParam)
    {
        if (wParam < 256) {
            if (msg == WM_KEYDOWN || msg == WM_SYSKEYDOWN) {
                g_wndKeyStatus[wParam].hold += (lParam & 0x000000ff);
                g_wndKeyStatus[wParam].frame = 1;
                if (!g_wndKeyStatus[wParam].name[0]) {
                    GetKeyNameTextA(lParam, g_wndKeyStatus[wParam].name, 32);
                }
            } else if (msg == WM_KEYUP || msg == WM_SYSKEYUP) {
                g_wndKeyStatus[wParam].hold = 0;
            }
        }
    }
    void ImplWin32UpdKeyFrame()
    {
        for (int i = 0; i < 256; ++i) {
            if (g_wndKeyStatus[i].hold || g_wndKeyStatus[i].frame) {
                g_wndKeyFrameStatus[i]++;
            } else {
                g_wndKeyFrameStatus[i] = 0;
            }
            g_wndKeyStatus[i].frame = 0;
        }
    }
    int ImplWin32GetKey(int vk)
    {
        return g_wndKeyStatus[vk].hold;
    }
    int ImplWin32GetKeyFrame(int vk)
    {
        return g_wndKeyFrameStatus[vk];
    }
    int ImplWin32ScanForUserHotkey(std::string* hotkeyStr)
    {
        int key = 0;
        for (int i = 0; i < 256; ++i) {
            if (g_wndKeyFrameStatus[i]) {
                if ((i >= 0x30 && i <= 0x5A) || (i >= 0x60 && i <= 0x87)) {
                    key = i;
                } else {
                    switch (i) {
                    case VK_BACK:
                    case VK_TAB:
                    case VK_RETURN:
                    case VK_ESCAPE:
                    case VK_SPACE:
                    case VK_PRIOR:
                    case VK_NEXT:
                    case VK_END:
                    case VK_HOME:
                    case VK_LEFT:
                    case VK_UP:
                    case VK_RIGHT:
                    case VK_DOWN:
                    case VK_SNAPSHOT:
                    case VK_INSERT:
                    case VK_DELETE:
                    case VK_OEM_1:
                    case VK_OEM_PLUS:
                    case VK_OEM_COMMA:
                    case VK_OEM_MINUS:
                    case VK_OEM_PERIOD:
                    case VK_OEM_2:
                    case VK_OEM_3:
                    case VK_OEM_102:
                        key = i;
                    }
                }
                if (key) {
                    break;
                }
            }
        }

        if (key) {
            if (g_wndKeyFrameStatus[VK_CONTROL]) {
                key |= 0x00020000;
            }
            if (g_wndKeyFrameStatus[VK_SHIFT]) {
                key |= 0x00040000;
            }
            if (g_wndKeyFrameStatus[VK_MENU]) {
                key |= 0x00010000;
            }

            if (hotkeyStr) {
                *hotkeyStr = "";
                if (key & 0x00020000) {
                    *hotkeyStr += "Ctrl ";
                }
                if (key & 0x00040000) {
                    *hotkeyStr += "Shift ";
                }
                if (key & 0x00010000) {
                    *hotkeyStr += "Alt ";
                }
                *hotkeyStr += g_wndKeyStatus[LOWORD(key)].name;
            }
        }

        return key;
    }
    const char* ImplWin32GetHotkeyText(int hotkey)
    {
        return g_wndKeyStatus[LOWORD(hotkey)].name;
    }
    int ImplWin32CheckHotkey(int hotkey)
    {
        auto modifier = HIWORD(hotkey);
        auto key = LOWORD(hotkey);
        int result;
        if (g_wndKeyFrameStatus[key] != 1) {
            return 0;
        }

        if (modifier == 0xFFFF) {
            result = key;
            if (g_wndKeyFrameStatus[VK_MENU]) {
                result |= 0x00010000;
            }
            if (g_wndKeyFrameStatus[VK_CONTROL]) {
                result |= 0x00020000;
            }
            if (g_wndKeyFrameStatus[VK_SHIFT]) {
                result |= 0x00040000;
            }
        } else {
            if ((bool)(modifier & MOD_ALT) != (bool)g_wndKeyFrameStatus[VK_MENU]) {
                return 0;
            }
            if ((bool)(modifier & MOD_CONTROL) != (bool)g_wndKeyFrameStatus[VK_CONTROL]) {
                return 0;
            }
            if ((bool)(modifier & MOD_SHIFT) != (bool)g_wndKeyFrameStatus[VK_SHIFT]) {
                return 0;
            }
            result = hotkey;
        }

        return result;
    }

    // Helper functions
    static bool ImplWin32UpdateMouseCursor()
    {
        ImGuiIO& io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange)
            return false;

        ImGuiMouseCursor imgui_cursor = ImGui::GetMouseCursor();
        if (imgui_cursor == ImGuiMouseCursor_None || io.MouseDrawCursor) {
            // Hide OS mouse cursor if imgui is drawing it or if it wants no cursor
            ::SetCursor(NULL);
        } else {
            // Show OS mouse cursor
            LPTSTR win32_cursor = IDC_ARROW;
            switch (imgui_cursor) {
            case ImGuiMouseCursor_Arrow:
                win32_cursor = IDC_ARROW;
                break;
            case ImGuiMouseCursor_TextInput:
                win32_cursor = IDC_IBEAM;
                break;
            case ImGuiMouseCursor_ResizeAll:
                win32_cursor = IDC_SIZEALL;
                break;
            case ImGuiMouseCursor_ResizeEW:
                win32_cursor = IDC_SIZEWE;
                break;
            case ImGuiMouseCursor_ResizeNS:
                win32_cursor = IDC_SIZENS;
                break;
            case ImGuiMouseCursor_ResizeNESW:
                win32_cursor = IDC_SIZENESW;
                break;
            case ImGuiMouseCursor_ResizeNWSE:
                win32_cursor = IDC_SIZENWSE;
                break;
            case ImGuiMouseCursor_Hand:
                win32_cursor = IDC_HAND;
                break;
            }
            ::SetCursor(::LoadCursor(NULL, win32_cursor));
        }
        return true;
    }
    static void ImplWin32UpdateMousePos(bool mouseMapping)
    {
        ImGuiIO& io = ImGui::GetIO();

        // Set OS mouse position if requested (rarely used, only when ImGuiConfigFlags_NavEnableSetMousePos is enabled by user)
        if (io.WantSetMousePos) {
            POINT pos = { (int)io.MousePos.x, (int)io.MousePos.y };
            ::ClientToScreen(g_hWnd, &pos);
            ::SetCursorPos(pos.x, pos.y);
        }

        // Set mouse position
        io.MousePos = ImVec2(-FLT_MAX, -FLT_MAX);
        POINT pos;
        if (HWND active_window = ::GetForegroundWindow())
            if (active_window == g_hWnd || ::IsChild(active_window, g_hWnd))
                if (::GetCursorPos(&pos) && ::ScreenToClient(g_hWnd, &pos)) {
                    if (mouseMapping) {
                        RECT rect = {};
                        SIZE wndSize = {};
                        GetClientRect(g_hWnd, &rect);
                        wndSize.cx = rect.right - rect.left;
                        wndSize.cy = rect.bottom - rect.top;
                        io.MousePos.x = (float)pos.x / (float)wndSize.cx * io.DisplaySize.x;
                        io.MousePos.y = (float)pos.y / (float)wndSize.cy * io.DisplaySize.y;
                    } else {
                        io.MousePos.x = (float)pos.x;
                        io.MousePos.y = (float)pos.y;
                    }
                }
    }
    LRESULT ImplWin32WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        // Process Win32 mouse/keyboard inputs.
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        // PS: In this Win32 handler, we use the capture API (GetCapture/SetCapture/ReleaseCapture) to be able to read mouse coordinates when dragging mouse outside of our window bounds.
        // PS: We treat DBLCLK messages as regular mouse down messages, so this code will work on windows classes that have the CS_DBLCLKS flag set. Our own example app code doesn't set this flag.

        if (ImGui::GetCurrentContext() == NULL)
            return 0;

        static char doubleByte[2] {};
        static bool isDoubleByte = false;

        ImGuiIO& io = ImGui::GetIO();
        switch (msg) {
        case WM_LBUTTONDOWN:
        case WM_LBUTTONDBLCLK:
        case WM_RBUTTONDOWN:
        case WM_RBUTTONDBLCLK:
        case WM_MBUTTONDOWN:
        case WM_MBUTTONDBLCLK:
        case WM_XBUTTONDOWN:
        case WM_XBUTTONDBLCLK: {
            int button = 0;
            if (msg == WM_LBUTTONDOWN || msg == WM_LBUTTONDBLCLK) {
                button = 0;
            }
            if (msg == WM_RBUTTONDOWN || msg == WM_RBUTTONDBLCLK) {
                button = 1;
            }
            if (msg == WM_MBUTTONDOWN || msg == WM_MBUTTONDBLCLK) {
                button = 2;
            }
            if (msg == WM_XBUTTONDOWN || msg == WM_XBUTTONDBLCLK) {
                button = (GET_XBUTTON_WPARAM(wParam) == XBUTTON1) ? 3 : 4;
            }
            if (!ImGui::IsAnyMouseDown() && ::GetCapture() == NULL)
                ::SetCapture(hwnd);
            io.MouseDown[button] = true;
            return 0;
        }
        case WM_LBUTTONUP:
        case WM_RBUTTONUP:
        case WM_MBUTTONUP:
        case WM_XBUTTONUP: {
            int button = 0;
            if (msg == WM_LBUTTONUP) {
                button = 0;
            }
            if (msg == WM_RBUTTONUP) {
                button = 1;
            }
            if (msg == WM_MBUTTONUP) {
                button = 2;
            }
            if (msg == WM_XBUTTONUP) {
                button = (GET_XBUTTON_WPARAM(wParam) == XBUTTON1) ? 3 : 4;
            }
            io.MouseDown[button] = false;
            if (!ImGui::IsAnyMouseDown() && ::GetCapture() == hwnd)
                ::ReleaseCapture();
            return 0;
        }
        case WM_MOUSEWHEEL:
            io.MouseWheel += (float)GET_WHEEL_DELTA_WPARAM(wParam) / (float)WHEEL_DELTA;
            return 0;
        case WM_MOUSEHWHEEL:
            io.MouseWheelH += (float)GET_WHEEL_DELTA_WPARAM(wParam) / (float)WHEEL_DELTA;
            return 0;
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
            ImplWin32UpdKeyStatus(msg, wParam, lParam);
            if (wParam < 256)
                io.KeysDown[wParam] = 1;
            return 0;
        case WM_KEYUP:
        case WM_SYSKEYUP:
            ImplWin32UpdKeyStatus(msg, wParam, lParam);
            if (wParam < 256)
                io.KeysDown[wParam] = 0;
            return 0;
        // ZUN use ANSI charset in all his games so WM_UNICHAR isn't a option.
        // Simple workaround here: user both WM_CHAR and WM_IME_CHAR.
        case WM_CHAR:
            // You can also use ToAscii()+GetKeyboardState() to retrieve characters.
            if (isDoubleByte) {
                isDoubleByte = false;
                doubleByte[1] = (BYTE)(wParam & 0x00FF);
                wchar_t ch[6];
                MultiByteToWideChar(CP_OEMCP, 0, (LPCSTR)&doubleByte, 2, ch, 3);
                io.AddInputCharacter(ch[0]);
            } else if (wParam > 0x7f) {
                isDoubleByte = true;
                doubleByte[0] = (BYTE)(wParam & 0x00FF);
            } else {
                io.AddInputCharacter((unsigned short)wParam);
            }

            return 0;
        case WM_IME_CHAR:
            return 0;
        case WM_SETCURSOR:
            if (!g_isFullscreenCache) {
                DefWindowProcA(hwnd, msg, wParam, lParam);
                return 1;
            }
            return 0;
        case WM_DEVICECHANGE:
            if ((UINT)wParam == DBT_DEVNODES_CHANGED)
                g_WantUpdateHasGamepad = true;
            return 0;
        case WM_SIZING: {
            float aspectRatio = 640.0f / 480.0f;
            float wi;
            float he;
            auto frameSize = GetSystemMetrics(SM_CXSIZEFRAME) * 2;
            auto captionSize = GetSystemMetrics(SM_CYCAPTION);
            RECT* re = ((RECT*)(lParam));
            switch (wParam) {
            case WMSZ_BOTTOMRIGHT:
            case WMSZ_RIGHT:
            case WMSZ_BOTTOMLEFT:
            case WMSZ_LEFT:
                wi = (float)(re->right - re->left - frameSize);
                he = wi / aspectRatio;
                re->bottom = re->top + (LONG)he + captionSize + frameSize;
                break;
            case WMSZ_BOTTOM:
            case WMSZ_TOP:
                he = (float)(re->bottom - re->top - captionSize - frameSize);
                wi = he * aspectRatio;
                re->right = re->left + (LONG)wi + frameSize;
                break;
                break;
            case WMSZ_TOPLEFT:
            case WMSZ_TOPRIGHT:
                wi = (float)(re->right - re->left) - frameSize;
                he = wi / aspectRatio;
                re->top = re->bottom - (LONG)he - captionSize - frameSize;
                break;
            default:
                break;
            }
            }
            return 0;
        }
        return 0;
    }
    LRESULT ImplWin32WndProcHandlerW(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        // Process Win32 mouse/keyboard inputs.
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        // PS: In this Win32 handler, we use the capture API (GetCapture/SetCapture/ReleaseCapture) to be able to read mouse coordinates when dragging mouse outside of our window bounds.
        // PS: We treat DBLCLK messages as regular mouse down messages, so this code will work on windows classes that have the CS_DBLCLKS flag set. Our own example app code doesn't set this flag.

        if (ImGui::GetCurrentContext() == NULL)
            return 0;

        static char doubleByte[2] {};
        static bool isDoubleByte = false;

        ImGuiIO& io = ImGui::GetIO();
        switch (msg) {
        case WM_LBUTTONDOWN:
        case WM_LBUTTONDBLCLK:
        case WM_RBUTTONDOWN:
        case WM_RBUTTONDBLCLK:
        case WM_MBUTTONDOWN:
        case WM_MBUTTONDBLCLK:
        case WM_XBUTTONDOWN:
        case WM_XBUTTONDBLCLK: {
            int button = 0;
            if (msg == WM_LBUTTONDOWN || msg == WM_LBUTTONDBLCLK) {
                button = 0;
            }
            if (msg == WM_RBUTTONDOWN || msg == WM_RBUTTONDBLCLK) {
                button = 1;
            }
            if (msg == WM_MBUTTONDOWN || msg == WM_MBUTTONDBLCLK) {
                button = 2;
            }
            if (msg == WM_XBUTTONDOWN || msg == WM_XBUTTONDBLCLK) {
                button = (GET_XBUTTON_WPARAM(wParam) == XBUTTON1) ? 3 : 4;
            }
            if (!ImGui::IsAnyMouseDown() && ::GetCapture() == NULL)
                ::SetCapture(hwnd);
            io.MouseDown[button] = true;
            return 0;
        }
        case WM_LBUTTONUP:
        case WM_RBUTTONUP:
        case WM_MBUTTONUP:
        case WM_XBUTTONUP: {
            int button = 0;
            if (msg == WM_LBUTTONUP) {
                button = 0;
            }
            if (msg == WM_RBUTTONUP) {
                button = 1;
            }
            if (msg == WM_MBUTTONUP) {
                button = 2;
            }
            if (msg == WM_XBUTTONUP) {
                button = (GET_XBUTTON_WPARAM(wParam) == XBUTTON1) ? 3 : 4;
            }
            io.MouseDown[button] = false;
            if (!ImGui::IsAnyMouseDown() && ::GetCapture() == hwnd)
                ::ReleaseCapture();
            return 0;
        }
        case WM_MOUSEWHEEL:
            io.MouseWheel += (float)GET_WHEEL_DELTA_WPARAM(wParam) / (float)WHEEL_DELTA;
            return 0;
        case WM_MOUSEHWHEEL:
            io.MouseWheelH += (float)GET_WHEEL_DELTA_WPARAM(wParam) / (float)WHEEL_DELTA;
            return 0;
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
            if (wParam < 256)
                io.KeysDown[wParam] = 1;
            return 0;
        case WM_KEYUP:
        case WM_SYSKEYUP:
            if (wParam < 256)
                io.KeysDown[wParam] = 0;
            return 0;
        case WM_CHAR:
            io.AddInputCharacter(wParam);
            return 0;
        case WM_SETCURSOR:
            if (LOWORD(lParam) == HTCLIENT && ImplWin32UpdateMouseCursor())
                return 1;
            return 0;
        case WM_DEVICECHANGE:
            if ((UINT)wParam == DBT_DEVNODES_CHANGED)
                g_WantUpdateHasGamepad = true;
            return 0;
        }
        return 0;
    }

    // API Functions
    bool ImplWin32Init(void* hwnd)
    {
        if (!::QueryPerformanceFrequency((LARGE_INTEGER*)&g_TicksPerSecond))
            return false;
        if (!::QueryPerformanceCounter((LARGE_INTEGER*)&g_Time))
            return false;

        // Setup back-end capabilities flags
        g_hWnd = (HWND)hwnd;
        ImGuiIO& io = ImGui::GetIO();
        io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors; // We can honor GetMouseCursor() values (optional)
        io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos; // We can honor io.WantSetMousePos requests (optional, rarely used)
        io.BackendPlatformName = "thprac_gui_impl_win32";
        //io.ImeWindowHandle = hwnd;

        // Keyboard mapping. ImGui will use those indices to peek into the io.KeysDown[] array that we will update during the application lifetime.
        /*
			io.KeyMap[ImGuiKey_Tab] = VK_TAB;
			io.KeyMap[ImGuiKey_UpArrow] = VK_UP;
			io.KeyMap[ImGuiKey_DownArrow] = VK_DOWN;
			io.KeyMap[ImGuiKey_PageUp] = VK_PRIOR;
			io.KeyMap[ImGuiKey_PageDown] = VK_NEXT;
			io.KeyMap[ImGuiKey_Home] = VK_HOME;
			io.KeyMap[ImGuiKey_Escape] = VK_ESCAPE;
			io.KeyMap[ImGuiKey_End] = VK_END;
			io.KeyMap[ImGuiKey_Insert] = VK_INSERT;
			io.KeyMap[ImGuiKey_Space] = VK_SPACE;
			io.KeyMap[ImGuiKey_A] = 'A';
			io.KeyMap[ImGuiKey_C] = 'C';
			io.KeyMap[ImGuiKey_V] = 'V';
			io.KeyMap[ImGuiKey_X] = 'X';
			io.KeyMap[ImGuiKey_Y] = 'Y';
			io.KeyMap[ImGuiKey_Z] = 'Z';
			*/
        io.KeyMap[ImGuiKey_LeftArrow] = VK_LEFT;
        io.KeyMap[ImGuiKey_RightArrow] = VK_RIGHT;
        io.KeyMap[ImGuiKey_Backspace] = VK_BACK;
        io.KeyMap[ImGuiKey_Delete] = VK_DELETE;
        io.KeyMap[ImGuiKey_Enter] = VK_RETURN;
        return true;
    }
    void ImplWin32Check(void* hwnd)
    {
        if (g_hWnd != (HWND)hwnd) {
            g_hWnd = (HWND)hwnd;
            ImplWin32HookWndProc();
        }
    }
    void ImplWin32Shutdown()
    {
        g_hWnd = (HWND)0;
    }
    void ImplWin32NewFrame(bool mouseMapping)
    {
        ImGuiIO& io = ImGui::GetIO();
        IM_ASSERT(io.Fonts->IsBuilt() && "Font atlas not built! It is generally built by the renderer back-end. Missing call to renderer _NewFrame() function? e.g. ImGui_ImplOpenGL3_NewFrame().");

        // Setup time step
        INT64 current_time = 0;
        ::QueryPerformanceCounter((LARGE_INTEGER*)&current_time);
        io.DeltaTime = (float)(current_time - g_Time) / g_TicksPerSecond;
        g_Time = current_time;

        // Update OS mouse position
        ImplWin32UpdateMousePos(mouseMapping);

        // Update OS mouse cursor with the cursor requested by imgui
        ImGuiMouseCursor mouse_cursor = io.MouseDrawCursor ? ImGuiMouseCursor_None : ImGui::GetMouseCursor();
        if (g_LastMouseCursor != mouse_cursor) {
            g_LastMouseCursor = mouse_cursor;
            ImplWin32UpdateMouseCursor();
        }

        ImplWin32UpdKeyFrame();
        g_isFullscreenCache = ImplWin32CheckFullScreen();
    }

    // Added functions used by thprac
    static WNDPROC __thimgui_wp_original = nullptr;
    void ImplWin32SetNoClose(bool noClose)
    {
        g_wndNoClose_ = noClose ? 1 : 0;
    }
    static LRESULT CALLBACK __ThImGui_WndProc_HookFunc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
        if (g_wndNoClose_ && msg == WM_CLOSE) {
            return 1;
        }
        if (GameExternWndProc(hwnd, msg, wParam, lParam)) {
            return 1;
        }
        if (ImplWin32WndProcHandler(hwnd, msg, wParam, lParam)) {
            return 1;
        }
        return CallWindowProcW(__thimgui_wp_original, hwnd, msg, wParam, lParam);
    }
    bool ImplWin32HookWndProc()
    {
        __thimgui_wp_original = (WNDPROC)GetWindowLongW(g_hWnd, GWLP_WNDPROC);
        if (!__thimgui_wp_original) {
            return false;
        }
        if (!SetWindowLongW(g_hWnd, GWLP_WNDPROC, (LONG)__ThImGui_WndProc_HookFunc)) {
            return false;
        }
        return true;
    }
    bool ImplWin32UnHookWndProc()
    {
        return SetWindowLongW(g_hWnd, GWLP_WNDPROC, (LONG)__thimgui_wp_original) != 0;
    }
    bool ImplWin32CheckFullScreen()
    {
        RECT a, b;
        GetWindowRect(g_hWnd, &a);
        GetWindowRect(GetDesktopWindow(), &b);
        return (a.left == b.left && a.top == b.top && a.right == b.right && a.bottom == b.bottom);
    }
    bool ImplWin32CheckForeground()
    {
        if (HWND active_window = ::GetForegroundWindow())
            if (active_window == g_hWnd || ::IsChild(active_window, g_hWnd))
                return true;
        return false;
    }
}
}
