#include "thprac_games.h"
#include "thprac_launcher_cfg.h"
#include "thprac_launcher_main.h"
#include "thprac_licence.h"
#include "thprac_load_exe.h"
#include "thprac_gui_impl_dx8.h"
#include "thprac_gui_impl_dx9.h"
#include "thprac_utils.h"
#include "../3rdParties/d3d8/include/d3d8.h"
#include <metrohash128.h>
#include <dinput.h>

#include "utils/wininternal.h"

namespace THPrac {
#pragma region Gui Wrapper

int g_gameGuiImpl = -1;
DWORD* g_gameGuiDevice = nullptr;
DWORD* g_gameGuiHwnd = nullptr;
HIMC g_gameIMCCtx = 0;

HANDLE thcrap_dll;
HANDLE thcrap_tsa_dll;

// Code from thcrap
template <typename T>
struct Ranges {
    T x_min, x_max;
    T y_min, y_max;
};

#define DEG_TO_RAD(x) ((x) * 0.0174532925199432957692369076848f)

template <typename T>
bool pov_to_xy(T& x, T& y, Ranges<T>& ranges, DWORD pov)
{
    // According to MSDN, some DirectInput drivers report the centered
    // position of the POV indicator as 65,535. This matches both that
    // behavior and JOY_POVCENTERED for WinMM.
    if (LOWORD(pov) == 0xFFFF) {
        return false;
    }
    T x_center = (ranges.x_max - ranges.x_min) / 2;
    T y_center = (ranges.y_max - ranges.y_min) / 2;

    float angle_deg = pov / 100.0f;
    float angle_rad = DEG_TO_RAD(angle_deg);
    // POV values ≠ unit circle angles, so...
    float angle_sin = 1.0f - cosf(angle_rad);
    float angle_cos = sinf(angle_rad) + 1.0f;
    x = (T)(ranges.x_min + (angle_cos * x_center));
    y = (T)(ranges.y_min + (angle_sin * y_center));
    return true;
}

HRESULT IDirectInputDevice8_GetDeviceState_Hook(IDirectInputDevice8A* This, DWORD cbData, void* lpvData)
{
    HRESULT res = This->GetDeviceState(cbData, lpvData);
    if (FAILED(res) || !(cbData == sizeof(DIJOYSTATE) || cbData == sizeof(DIJOYSTATE2))) {
        return res;
    }

    auto* js = (DIJOYSTATE*)lpvData;

    Ranges<long> di_range = { -1000, 1000, -1000, 1000 };

    bool ret_map = false;
    for (int i = 0; i < elementsof(js->rgdwPOV) && !ret_map; i++) {
        ret_map = pov_to_xy(js->lX, js->lY, di_range, js->rgdwPOV[i]);
    }
    return res;
}

bool __fastcall IDirectInputDevice8_GetDeviceState_VEHHook(PCONTEXT pCtx, [[maybe_unused]] HookCtx* self)
{
    pCtx->Eax = IDirectInputDevice8_GetDeviceState_Hook(
        GetMemContent<IDirectInputDevice8A*>(pCtx->Esp + 0),
        GetMemContent<DWORD>(pCtx->Esp + 4),
        GetMemContent<void*>(pCtx->Esp + 8));
    pCtx->Esp += 12;
    return false;
}

decltype(joyGetPosEx)* orig_joyGetPosEx = nullptr;

// joyGetDevCaps() will necessarily be slower
struct winmm_joy_caps_t {
    // joyGetPosEx() will return bogus values on joysticks without a POV, so
    // we must check if we even have one.
    bool initialized = false;
    bool has_pov;
    Ranges<DWORD> range;
};

std::vector<winmm_joy_caps_t> joy_info;

MMRESULT WINAPI hook_joyGetPosEx(UINT uJoyID, JOYINFOEX* pji)
{
    if (!pji) {
        return MMSYSERR_INVALPARAM;
    }
    pji->dwFlags |= JOY_RETURNPOV;

    auto ret_pos = orig_joyGetPosEx(uJoyID, pji);
    if (ret_pos != JOYERR_NOERROR) {
        return ret_pos;
    }

    if (uJoyID >= joy_info.size()) {
        joy_info.resize(uJoyID + 1);
    }

    auto& jc = joy_info[uJoyID];

    if (!jc.initialized) {
        JOYCAPSW caps;
        auto ret_caps = joyGetDevCapsW(uJoyID, &caps, sizeof(caps));
        assert(ret_caps == JOYERR_NOERROR);

        jc.initialized = true;
        jc.has_pov = (caps.wCaps & JOYCAPS_HASPOV) != 0;
        jc.range.x_min = caps.wXmin;
        jc.range.x_max = caps.wXmax;
        jc.range.y_min = caps.wYmin;
        jc.range.y_max = caps.wYmax;
    } else if (!jc.has_pov) {
        return ret_pos;
    }
    pov_to_xy(pji->dwXpos, pji->dwYpos, jc.range, pji->dwPOV);
    return ret_pos;
}

// Replace with centralized IAT hooking once there's a need for that
void iat_hook_joyGetPosEx()
{
    uintptr_t base = CurrentImageBase;

    auto* pImpDesc = (PIMAGE_IMPORT_DESCRIPTOR)GetNtDataDirectory((HMODULE)base, IMAGE_DIRECTORY_ENTRY_IMPORT);

    for (; pImpDesc->Name; pImpDesc++) {
        if (_stricmp((char*)(base + pImpDesc->Name), "winmm.dll")) {
            continue;
        }
        auto pOT = (PIMAGE_THUNK_DATA)(base + pImpDesc->OriginalFirstThunk);
        auto pIT = (PIMAGE_THUNK_DATA)(base + pImpDesc->FirstThunk);

        if (pImpDesc->OriginalFirstThunk) {
            for (; pOT->u1.Function; pOT++, pIT++) {
                PIMAGE_IMPORT_BY_NAME pByName;
                if (!(pOT->u1.Ordinal & IMAGE_ORDINAL_FLAG)) {
                    pByName = (PIMAGE_IMPORT_BY_NAME)(base + pOT->u1.AddressOfData);
                    if (pByName->Name[0] == '\0') {
                        continue;
                    }
                    if (!strcmp("joyGetPosEx", (char*)pByName->Name)) {
                        orig_joyGetPosEx = (decltype(joyGetPosEx)*)pIT->u1.Function;

                        DWORD oldProt;
                        VirtualProtect(&pIT->u1.Function, 4, PAGE_READWRITE, &oldProt);
                        pIT->u1.Function = (DWORD)hook_joyGetPosEx;
                        VirtualProtect(&pIT->u1.Function, 4, oldProt, &oldProt);
                    }
                }
            }
        }
    }
}

void SetDpadHook(uintptr_t addr, size_t instr_len) {
    static constinit HookCtx dpad_hook = {
        .callback = IDirectInputDevice8_GetDeviceState_VEHHook,
        .data = PatchData()
    };

    dpad_hook.addr = addr;
    dpad_hook.data.hook.instr_len = static_cast<uint8_t>(instr_len);
    dpad_hook.Setup();
    dpad_hook.Enable();

    iat_hook_joyGetPosEx();
}

void GameGuiInit(game_gui_impl impl, int device, int hwnd_addr,
    Gui::ingame_input_gen_t input_gen, int reg1, int reg2, int reg3,
    int wnd_size_flag, float x, float y)
{
    thcrap_dll = GetModuleHandleW(L"thcrap.dll");
    if (!thcrap_dll) {
        thcrap_dll = GetModuleHandleW(L"thcrap_d.dll");
    }

    thcrap_tsa_dll = GetModuleHandleW(L"thcrap_tsa.dll");
    if (!thcrap_tsa_dll) {
        thcrap_tsa_dll = GetModuleHandleW(L"thcrap_tsa_d.dll");
    }

    ingame_mb_init();
    ::ImGui::CreateContext();
    g_gameGuiImpl = impl;
    g_gameGuiDevice = (DWORD*)device;
    g_gameGuiHwnd = (DWORD*)hwnd_addr;
    g_gameIMCCtx = ImmAssociateContext(*(HWND*)hwnd_addr, 0);

    // Set Locale
    GuiLauncherLocaleInit();

    switch (impl) {
    case THPrac::IMPL_WIN32_DX8:
        // Impl
        Gui::ImplDX8Init((IDirect3DDevice8*)*g_gameGuiDevice);
        Gui::ImplWin32Init((HWND)*g_gameGuiHwnd);

        // Hooks
        Gui::ImplDX8HookReset();
        Gui::ImplWin32HookWndProc();
        break;
    case THPrac::IMPL_WIN32_DX9:
        // Impl
        Gui::ImplDX9Init((IDirect3DDevice9*)*g_gameGuiDevice);
        Gui::ImplWin32Init((HWND)*g_gameGuiHwnd);

        // Hooks
        Gui::ImplDX9HookReset();
        Gui::ImplWin32HookWndProc();
        break;
    default:
        break;
    }

    // Inputs
    Gui::InGameInputInit(input_gen, reg1, reg2, reg3);

    // Display size setup
    auto& io = ::ImGui::GetIO();
    if (wnd_size_flag == -1) {
        io.DisplaySize = { x, y };
        Gui::LocaleCreateFont(io.DisplaySize.x * 0.025f);
    } else if (wnd_size_flag == -2) {
        float dispX, dispY;
        if (x > 1.6) {
            dispX = 1280.0f;
            dispY = 960.0f;
        } else if (x > 1.1) {
            dispX = 960.0f;
            dispY = 720.0f;
        } else {
            dispX = 640.0f;
            dispY = 480.0f;
        }
        Gui::ImplDX9AdjustDispSize();
        Gui::LocaleCreateFont(dispX * 0.025f);
    } else {
        switch (wnd_size_flag) {
        case 2:
        case 5:
            io.DisplaySize = { 1280.0f, 960.0f };
            break;
        case 1:
        case 4:
            io.DisplaySize = { 960.0f, 720.0f };
            break;
        default:
            io.DisplaySize = { 640.0f, 480.0f };
            break;
        }
        Gui::LocaleCreateFont(io.DisplaySize.x * 0.025f);
    }

    if (LauncherCfgInit(true)) {
        bool resizable_window;
        if (LauncherSettingGet("resizable_window", resizable_window) && resizable_window && !Gui::ImplWin32CheckFullScreen()) {
            RECT wndRect;
            GetClientRect(*(HWND*)hwnd_addr, &wndRect);
            auto frameSize = GetSystemMetrics(SM_CXSIZEFRAME) * 2;
            auto captionSize = GetSystemMetrics(SM_CYCAPTION);
            auto longPtr = GetWindowLongW(*(HWND*)hwnd_addr, GWL_STYLE);
            SetWindowLongW(*(HWND*)hwnd_addr, GWL_STYLE, longPtr | WS_SIZEBOX);
            SetWindowPos(*(HWND*)hwnd_addr, HWND_NOTOPMOST,
                0, 0, wndRect.right + frameSize, wndRect.bottom + frameSize + captionSize,
                SWP_NOMOVE | SWP_NOZORDER | SWP_FRAMECHANGED);
        }
        int theme;
        if (LauncherSettingGet("theme", theme)) {
            const char* userThemeName;
            if (LauncherSettingGet("theme_user", userThemeName))
                SetTheme(theme, utf8_to_utf16(userThemeName).c_str());
            else
                SetTheme(theme);
        } else
            ImGui::StyleColorsDark();
    } else
        ::ImGui::StyleColorsDark();
    // Imgui settings
    io.IniFilename = nullptr;
}

int GameGuiProgress = 0;

void GameGuiBegin(game_gui_impl impl, bool game_nav)
{
    // Acquire game input
    ImGuiIO& io = ImGui::GetIO();
    if (game_nav) {
        Gui::GuiNavFocus::GlobalDisable(false);
        io.NavInputs[ImGuiNavInput_DpadUp] = Gui::InGameInputGet(VK_UP);
        io.NavInputs[ImGuiNavInput_DpadDown] = Gui::InGameInputGet(VK_DOWN);
        io.NavInputs[ImGuiNavInput_DpadLeft] = Gui::InGameInputGet(VK_LEFT);
        io.NavInputs[ImGuiNavInput_DpadRight] = Gui::InGameInputGet(VK_RIGHT);
    } else {
        Gui::GuiNavFocus::GlobalDisable(true);
    }

    switch (impl) {
    case THPrac::IMPL_WIN32_DX8:
        // New frame
        Gui::ImplDX8NewFrame();
        Gui::ImplWin32NewFrame();
        ::ImGui::NewFrame();
        break;
    case THPrac::IMPL_WIN32_DX9:
        // New frame
        Gui::ImplDX9NewFrame();
        Gui::ImplWin32NewFrame();
        ::ImGui::NewFrame();
        break;
    }
    GameGuiProgress = 1;
}

void GameGuiEnd(bool draw_cursor)
{
    if (GameGuiProgress != 1)
        return;
    // Draw cursor if needed
    if (draw_cursor && Gui::ImplWin32CheckFullScreen()) {
        auto& io = ::ImGui::GetIO();
        io.MouseDrawCursor = true;
    }

    // Locale Change
    if (!ImGui::IsAnyItemActive()) {
        if (Gui::ImplWin32CheckHotkey(0x00010031)) {
            Gui::LocaleSet(Gui::LOCALE_JA_JP);
        } else if (Gui::ImplWin32CheckHotkey(0x00010032)) {
            Gui::LocaleSet(Gui::LOCALE_ZH_CN);
        } else if (Gui::ImplWin32CheckHotkey(0x00010033)) {
            Gui::LocaleSet(Gui::LOCALE_EN_US);
        }
    }
    ::ImGui::EndFrame();
    GameGuiProgress = 2;
}

void GameGuiRender(game_gui_impl impl)
{
    if (GameGuiProgress != 2)
        return;
    Gui::ImplWin32Check((void*)*g_gameGuiHwnd);
    switch (impl) {
    case THPrac::IMPL_WIN32_DX8:
        // End frame and render
        Gui::ImplDX8Check((IDirect3DDevice8*)*g_gameGuiDevice);
        ::ImGui::Render();
        Gui::ImplDX8RenderDrawData(::ImGui::GetDrawData());
        break;
    case THPrac::IMPL_WIN32_DX9:
        // End frame and render
        Gui::ImplDX9Check((IDirect3DDevice9*)*g_gameGuiDevice);
        ::ImGui::Render();
        Gui::ImplDX9RenderDrawData(::ImGui::GetDrawData());
        break;
    default:
        break;
    }
    GameGuiProgress = 0;
}

#pragma endregion

#pragma region Advanced Options Menu

void MsgBox(UINT type, const char* title, const char* msg, const char* msg2 = nullptr)
{
    static wchar_t _title[256];
    static wchar_t _msg[256];
    static wchar_t _msg2[256];
    MultiByteToWideChar(CP_UTF8, 0, title, -1, _title, 256);
    MultiByteToWideChar(CP_UTF8, 0, msg, -1, _msg, 256);
    if (msg2) {
        MultiByteToWideChar(CP_UTF8, 0, msg2, -1, _msg2, 256);
        wcscat_s(_msg, _msg2);
    }
    MessageBoxW(nullptr, _msg, _title, type);
}

void CenteredText(const char* text, float wndX)
{
    ImGui::SetCursorPosX((wndX - ImGui::CalcTextSize(text).x) / 2.0f);
    ImGui::TextUnformatted(text);
}

float GetRelWidth(float rel)
{
    return ImGui::GetIO().DisplaySize.x * rel;
}

float GetRelHeight(float rel)
{
    return ImGui::GetIO().DisplaySize.y * rel;
}

void CalcFileHash(const wchar_t* file_name, uint64_t hash[2])
{
    hash[0] = 0ll;
    hash[1] = 0ll;

    MappedFile file(file_name);
    if (file.fileMapView)
        MetroHash128::Hash((uint8_t*)file.fileMapView, file.fileSize, (uint8_t*)hash);
}

void HelpMarker(const char* desc)
{
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered()) {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

int FPSHelper(adv_opt_ctx& ctx, bool repStatus, bool vpFast, bool vpSlow, FPSHelperCallback* callback)
{
    static bool isDebugAccActive = false;
    int res = 0;
    if (ctx.fps_status <= 0) {
        return 0;
    }
    if (ctx.fps_debug_acc && Gui::ImplWin32GetKeyFrame(VK_SPACE)) {
        if (!isDebugAccActive) {
            isDebugAccActive = true;
            res = 1;
        }
        if (ctx.fps_status == 2) {
            callback(repStatus ? 9999 : (ctx.fps_replay_fast > 1200 ? 9999 : ctx.fps_replay_fast));
        } else if (ctx.fps_status == 1) {
            ctx.fps_dbl = 1.0 / (repStatus ? 9999.0 : (ctx.fps_replay_fast > 1200 ? 9999.0 : (double)ctx.fps_replay_fast));
        }
    } else if (ctx.fps_status == 2) {
        if (isDebugAccActive) {
            isDebugAccActive = false;
            res = -1;
        }
        if (repStatus) {
            if (!vpFast && Gui::ImplWin32GetKeyFrame(VK_CONTROL)) {
                callback(ctx.fps_replay_fast > 1200 ? 9999 : ctx.fps_replay_fast);
            } else if (!vpSlow && Gui::ImplWin32GetKeyFrame(VK_SHIFT)) {
                callback(ctx.fps_replay_slow);
            } else {
                callback(ctx.fps);
            }
        } else {
            callback(ctx.fps);
        }
    } else if (ctx.fps_status == 1) {
        if (isDebugAccActive) {
            isDebugAccActive = false;
            res = -1;
        }
        if (repStatus) {
            if (Gui::ImplWin32GetKeyFrame(VK_CONTROL)) {
                ctx.fps_dbl = 1.0 / (ctx.fps_replay_fast > 1200 ? 9999.0 : (double)ctx.fps_replay_fast);
            } else if (Gui::ImplWin32GetKeyFrame(VK_SHIFT)) {
                ctx.fps_dbl = 1.0 / (double)ctx.fps_replay_slow;
            } else {
                ctx.fps_dbl = 1.0 / (double)ctx.fps;
            }
        } else {
            ctx.fps_dbl = 1.0 / (double)ctx.fps;
        }
    }
    return res;
}

bool GameFPSOpt(adv_opt_ctx& ctx, bool replay)
{
    static char tmpStr[32] {};
    static int fps = 0;
    static int fpsStatic = 60;
    static int fpsSlowStatic = 0;
    static int fpsFastStatic = 0;
    static int fpsDebugAcc = 0;
    static int fpsMultiplier = 0;
    static bool canFpsChangeFreely = false;
    bool clickedApply = false;
    const char* fpsMultiplierStr;

    if (fps == 0) {
        if (ctx.fps_status == 1) {
            fpsStatic = fps = 60;
            ctx.fps_replay_slow = fpsSlowStatic = 15;
            ctx.fps_replay_fast = 60;
            fpsFastStatic = 1;
            fpsDebugAcc = ctx.fps_debug_acc;
        } else if (ctx.fps_status == 2) {
            fpsStatic = fps = ctx.fps;

            if (fps <= 60)
                fpsStatic = fps = 60;
            else if (fps % 15)
                canFpsChangeFreely = true;
            else
                fpsMultiplier = (fps - 60) / 15;

            if (ctx.fps_replay_slow > 60) {
                ctx.fps_replay_slow = 60;
            }
            fpsSlowStatic = ctx.fps_replay_slow;

            if (ctx.fps_replay_fast < 60) {
                ctx.fps_replay_fast = 60;
            }
            ctx.fps_replay_fast = ctx.fps_replay_fast - ctx.fps_replay_fast % 60;
            fpsFastStatic = ctx.fps_replay_fast / 60;

            fpsDebugAcc = ctx.fps_debug_acc;
        }
    }

    if (ctx.fps_status <= 0) {
        ImGui::PushTextWrapPos();
        if (ctx.fps_status == 0)
            ImGui::TextColored(ImColor(255, 0, 0), "%s", S(TH_FPS_ERR));
        else if (ctx.fps_status == -1)
            ImGui::TextColored(ImColor(255, 0, 0), "%s", S(TH_FPS_UNSUPPORTED));
        ImGui::PopTextWrapPos();
        ImGui::BeginDisabled();
    }

    switch (fpsMultiplier) {
    case 1:
        fpsMultiplierStr = "x1.25 (75fps)";
        break;
    case 2:
        fpsMultiplierStr = "x1.5 (90fps)";
        break;
    case 3:
        fpsMultiplierStr = "x1.75 (105fps)";
        break;
    case 4:
        fpsMultiplierStr = "x2.0 (120fps)";
        break;
    case 5:
        fpsMultiplierStr = "x2.25 (135fps)";
        break;
    case 6:
        fpsMultiplierStr = "x2.5 (150fps)";
        break;
    case 7:
        fpsMultiplierStr = "x2.75 (165fps)";
        break;
    case 8:
        fpsMultiplierStr = "x3.0 (180fps)";
        break;
    default:
        fpsMultiplierStr = "x1.0 (60fps)";
        break;
    }

    ImGui::PushItemWidth(GetRelWidth(0.23f));
    if (canFpsChangeFreely) {
        ImGui::DragInt(S(TH_FPS_ADJ), &fps, 1.0f, 60, 6000);
        if (!ImGui::IsItemActive())
            fps = std::clamp(fps, 60, 6000);
    } else {
        ImGui::SliderInt(S(TH_FPS_ADJ), &fpsMultiplier, 0, 8, fpsMultiplierStr);
        fps = fpsMultiplier * 15 + 60;
    }
    ImGui::PopItemWidth();
    ImGui::SameLine();
    if (ImGui::Checkbox(S(TH_FPS_FREE_ADJ), &canFpsChangeFreely)) {
        if (!canFpsChangeFreely) {
            int i = 0;
            for (; (i * 15 + 60) <= fps && i <= 8; ++i)
                ;
            fpsMultiplier = --i;
            fps = fpsMultiplier * 15 + 60;
        }
    }

    if (replay) {
        ImGui::PushItemWidth(GetRelWidth(0.23f));
        if (fpsFastStatic > 20) {
            sprintf(tmpStr, "infinite");
        } else {
            sprintf(tmpStr, "x%d.0 (%dfps)", fpsFastStatic, fpsFastStatic * 60);
        }

        ImGui::SliderInt("Replay slow FPS", &fpsSlowStatic, 1, 60);
        ImGui::SliderInt("Replay fast FPS", &fpsFastStatic, 1, 21, tmpStr);
        ImGui::PopItemWidth();
    }
    ImGui::Checkbox("Debug acc.", (bool*)&fpsDebugAcc);
    ImGui::SameLine();
    HelpMarker("Blah");

    if (fpsStatic != fps
        || fpsSlowStatic != ctx.fps_replay_slow
        || fpsFastStatic != ctx.fps_replay_fast / 60
        || fpsDebugAcc != ctx.fps_debug_acc) {
        ImGui::SameLine();
        if (ImGui::Button(S(TH_ADV_OPT_APPLY))) {
            clickedApply = true;
            if (fpsStatic > fps && ctx.fps_status != 1)
                ImGui::TextUnformatted(S(TH_FPS_LOWERING));
            fpsStatic = fps;
            ctx.fps_replay_slow = fpsSlowStatic;
            ctx.fps_replay_fast = fpsFastStatic * 60;
            ctx.fps_debug_acc = fpsDebugAcc;
        }
    }

    if (ctx.fps_status <= 0)
        ImGui::EndDisabled();

    ctx.fps = fpsStatic;
    return clickedApply;
}

bool GameplayOpt(adv_opt_ctx& ctx)
{
    bool hasChanged = false;

    hasChanged |= ImGui::Checkbox(S(TH_FACTOR_ACB), &ctx.all_clear_bonus);
    ImGui::SameLine();
    HelpMarker(S(TH_FACTOR_ACB_DESC));

    return hasChanged;
}

void AboutOpt(const char* thanks_text)
{
    static bool showLicense = false;
    if (BeginOptGroup<TH_ABOUT_THPRAC>()) {
        ImGui::Text(S(TH_ABOUT_VERSION), GetVersionStr());
        ImGui::TextUnformatted(S(TH_ABOUT_AUTHOR));
        ImGui::TextUnformatted(S(TH_ABOUT_WEBSITE));

        ImGui::NewLine();
        ImGui::Text(S(TH_ABOUT_THANKS), thanks_text ? thanks_text : "You!");

        ImGui::NewLine();
        if (showLicense) {
            if (ImGui::Button(S(TH_ABOUT_HIDE_LICENCE)))
                showLicense = false;
        } else {
            if (ImGui::Button(S(TH_ABOUT_SHOW_LICENCE)))
                showLicense = true;
        }
        if (showLicense) {
            ImGui::BeginChild("COPYING", ImVec2(0.0f, GetRelHeight(0.8f)), true);

            Gui::ShowLicenceInfo();

            ImGui::EndChild();
        }

        EndOptGroup();
    }
}

#pragma endregion

#pragma region Replay System

bool ReplaySaveParam(const wchar_t* rep_path, const std::string& param)
{
    auto repFile = CreateFileW(rep_path, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (repFile == INVALID_HANDLE_VALUE)
        return false;
    defer(CloseHandle(repFile));
    DWORD repMagic = 0, bytesRead = 0;
    if ((SetFilePointer(repFile, 0, nullptr, FILE_BEGIN) != INVALID_SET_FILE_POINTER) && (ReadFile(repFile, &repMagic, sizeof(LONG), &bytesRead, nullptr))) {
        if (repMagic == 'PR6T' || repMagic == 'PR7T') {
            auto paramSize = param.size();
            for (paramSize++; paramSize % 4; paramSize++)
                ;
            auto paramBuf = malloc(paramSize + 8);
            if (!paramBuf)
                return false;
            defer(free(paramBuf));
            memset(paramBuf, 0, paramSize);
            memcpy(paramBuf, param.data(), param.size());
            *(int32_t*)((int)paramBuf + paramSize) = paramSize;
            *(int32_t*)((int)paramBuf + paramSize + 4) = 'CARP';

            SetFilePointer(repFile, 0, nullptr, FILE_END);
            WriteFile(repFile, paramBuf, paramSize + 8, &bytesRead, nullptr);

            // Recalculate checksum
            auto repSize = GetFileSize(repFile, nullptr);
            uint8_t* repBuf = (uint8_t*)malloc(repSize - (repMagic == 'PR6T' ? 14 : 13));
            if (!repBuf)
                return false;
            defer(free(repBuf));
            SetFilePointer(repFile, repMagic == 'PR6T' ? 14 : 13, nullptr, FILE_BEGIN);
            if (!ReadFile(repFile, repBuf, repSize - (repMagic == 'PR6T' ? 14 : 13), &bytesRead, nullptr))
                return false;

            uint8_t key = *repBuf;
            auto decBuf = repBuf + (repMagic == 'PR6T' ? 1 : 3);
            for (DWORD i = 0; i < repSize - (repMagic == 'PR6T' ? 15 : 16); i++, decBuf++) {
                *decBuf -= key;
                key += 7;
            }

            DWORD checksum = 0x3F000318;
            decBuf = repBuf;
            for (DWORD i = 0; i < repSize - (repMagic == 'PR6T' ? 14 : 13); i++, decBuf++)
                checksum += *decBuf;

            SetFilePointer(repFile, 8, nullptr, FILE_BEGIN);
            WriteFile(repFile, &checksum, 4, &bytesRead, nullptr);
        } else {
            auto paramSize = param.size() + 12;
            for (paramSize++; paramSize % 4; paramSize++)
                ;
            auto paramBuf = malloc(paramSize);
            if (!paramBuf)
                return false;
            defer(free(paramBuf));
            memset(paramBuf, 0, paramSize);
            *(int32_t*)((int)paramBuf) = 'RESU';
            *(int32_t*)((int)paramBuf + 4) = paramSize;
            *(int32_t*)((int)paramBuf + 8) = 'CARP';
            memcpy((void*)((int)paramBuf + 12), param.data(), param.size());

            SetFilePointer(repFile, 0, nullptr, FILE_END);
            WriteFile(repFile, paramBuf, paramSize, &bytesRead, nullptr);
        }
    }
    return false;
}

bool ReplayLoadParam(const wchar_t* rep_path, std::string& param)
{
    DWORD repMagic = 0, bytesRead = 0;

    auto repFile = CreateFileW(rep_path, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (repFile == INVALID_HANDLE_VALUE)
        return false;
    defer(CloseHandle(repFile));

    SetFilePointer(repFile, 0, nullptr, FILE_BEGIN);
    if (ReadFile(repFile, &repMagic, 4, &bytesRead, nullptr) && bytesRead == 4) {
        if (repMagic == 'PR6T' || repMagic == 'PR7T') {
            DWORD magic = 0, paramLength = 0;
            DWORD repSize = GetFileSize(repFile, nullptr);

            SetFilePointer(repFile, -4, nullptr, FILE_END);
            if (ReadFile(repFile, &magic, 4, &bytesRead, nullptr) && bytesRead == 4 && magic == 'CARP') {
                SetFilePointer(repFile, -8, nullptr, FILE_CURRENT);
                if (!ReadFile(repFile, &paramLength, 4, &bytesRead, nullptr))
                    return false;

                if (bytesRead == 4 && paramLength > 0 && paramLength < repSize && paramLength < 512) {
                    SetFilePointer(repFile, ~paramLength - 3, nullptr, FILE_CURRENT);
                    char* buf = (char*)malloc(paramLength + 1);
                    if (!buf)
                        return false;
                    defer(free(buf));
                    memset(buf, 0, paramLength + 1);

                    if (ReadFile(repFile, buf, paramLength, &bytesRead, nullptr) && bytesRead == paramLength)
                        param = std::string(buf, paramLength);

                    return (bytesRead == paramLength);
                }
            }
        } else {
            DWORD userPtr = 0, userMagic = 0, userLength = 0, userNo = 0;

            SetFilePointer(repFile, 12, nullptr, FILE_BEGIN);
            if (ReadFile(repFile, &userPtr, 4, &bytesRead, nullptr) && bytesRead == 4) {
                SetFilePointer(repFile, userPtr, nullptr, FILE_BEGIN);
                while (true) {
                    if (!ReadFile(repFile, &userMagic, 4, &bytesRead, nullptr) || bytesRead != 4 || userMagic != 'RESU')
                        break;
                    if (!ReadFile(repFile, &userLength, 4, &bytesRead, nullptr) || bytesRead != 4)
                        break;
                    if (!ReadFile(repFile, &userNo, 4, &bytesRead, nullptr) || bytesRead != 4)
                        break;

                    if (userNo == 'CARP') {
                        char* buf = (char*)malloc(userLength - 12 + 1);
                        if (!buf)
                            break;
                        defer(free(buf));
                        memset(buf, 0, userLength - 12 + 1);
                        if (ReadFile(repFile, buf, userLength - 12, &bytesRead, nullptr) && bytesRead == userLength - 12)
                            param = std::string((char*)buf, userLength - 12 + 1);

                        return (bytesRead == userLength - 12);
                    } else {
                        SetFilePointer(repFile, userLength - 12, nullptr, FILE_CURRENT);
                    }
                }
            }
        }
    }
    return false;
}

#pragma endregion

#pragma region Virtual File System

void VFile::Write(const char* data)
{
    unsigned int i = 0;
    for (; data[i] != '\0'; i++)
        *(mBuffer + mPos + i) = ((uint8_t*)data)[i];
    mPos += i;
}
void VFile::Write(void* data, unsigned int length)
{
    if (!mBuffer)
        return;
    for (unsigned int i = 0; i < length; i++) {
        if ((mPos + i) >= mSize)
            break;
        *(mBuffer + mPos + i) = ((uint8_t*)data)[i];
    }
    mPos += length;
}
void VFile::Read(void* buffer, unsigned int length)
{
    if (!mBuffer)
        return;
    for (unsigned int i = 0; i < length; i++) {
        if ((mPos + i) >= mSize)
            break;
        ((uint8_t*)buffer)[i] = *(mBuffer + mPos + i);
    }
    mPos += length;
}
#pragma endregion

#pragma region Snapshot
namespace THSnapshot {
    void* GetSnapshotData(IDirect3DDevice8* d3d8)
    {
        IDirect3DSurface8* surface = nullptr;
        d3d8->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &surface);
        D3DLOCKED_RECT rect = {};
        surface->LockRect(&rect, nullptr, 0);

        void* bmp = malloc(0xE2000);
        uint8_t* bmp_write = (uint8_t*)bmp;
        for (int32_t i = 0x1DF; i >= 0; --i) {
            uint8_t* bmp_bits = ((uint8_t*)rect.pBits) + i * rect.Pitch;
            for (size_t j = 0; j < 0x280; ++j) {
                memcpy(bmp_write, bmp_bits, 3); // This *should* get optimized to byte/word MOVs
                bmp_bits += 4;
                bmp_write += 3;
            }
        }

        surface->UnlockRect();
        surface->Release();

        return bmp;
    }
    void Snapshot(IDirect3DDevice8* d3d8)
    {
        wchar_t dir[] = L"snapshot/th000.bmp";
        HANDLE hFile;
        CreateDirectoryW(L"snapshot", nullptr);
        for (int i = 0; i < 1000; i++) {
            dir[13] = static_cast<wchar_t>(i % 10) + L'0';
            dir[12] = static_cast<wchar_t>((i % 100 - i % 10) / 10) + L'0';
            dir[11] = static_cast<wchar_t>((i - i % 100) / 100) + L'0';
            hFile = CreateFileW(dir, GENERIC_READ | GENERIC_WRITE, 0, nullptr, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, nullptr);
            if (hFile != INVALID_HANDLE_VALUE)
                break;
        }
        if (hFile == INVALID_HANDLE_VALUE)
            return;

        auto header = "\x42\x4d\x36\x10\x0e\x00\x00\x00\x00\x00\x36\x00\x00\x00\x28\x00\x00\x00\x80\x02\x00\x00\xe0\x01\x00\x00\x01\x00\x18\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";
        void* bmp = GetSnapshotData(d3d8);
        DWORD bytesRead;
        WriteFile(hFile, header, 0x36, &bytesRead, nullptr);
        WriteFile(hFile, bmp, 0xE2000, &bytesRead, nullptr);
        free(bmp);

        CloseHandle(hFile);
    }
};
#pragma endregion

#pragma region ECL Warp
void StageWarpsRender(stage_warps_t& warps, std::vector<unsigned int>& out_warp, size_t level)
{
    if (warps.section_param.size() == 0)
        return;

    if (out_warp.size() <= level)
        out_warp.resize(level + 1);

    if (warps.section_param.size() <= out_warp[level]) {
        out_warp[level] = warps.section_param.size() - 1;
    }

    switch (warps.type) {
    case stage_warps_t::TYPE_SLIDER:
        ImGui::SliderInt(warps.label, (int*)&out_warp[level], 0, warps.section_param.size() - 1, warps.section_param[out_warp[level]].label);
        break;
    case stage_warps_t::TYPE_COMBO:
        if (ImGui::BeginCombo(warps.label, warps.section_param[out_warp[level]].label)) {
            for (unsigned int i = 0; i < warps.section_param.size(); i++) {
                ImGui::PushID(i);

                bool item_selected = (out_warp[level] == i);

                if (ImGui::Selectable(warps.section_param[i].label, &item_selected)) {
                    out_warp[level] = i;
                }

                if (item_selected)
                    ImGui::SetItemDefaultFocus();

                ImGui::PopID();
            }
            ImGui::EndCombo();
        }
    default:
        break;
    }

    if (ImGui::IsItemFocused()) {
        if (Gui::InGameInputGet(VK_LEFT) && out_warp[level] > 0) {
            out_warp[level]--;
        }
        if (Gui::InGameInputGet(VK_RIGHT) && out_warp[level] + 1 < warps.section_param.size()) {
            out_warp[level]++;
        }
    }

    if (warps.section_param[out_warp[level]].phases) {
        ImGui::PushID(level + 1);
        StageWarpsRender(*warps.section_param[out_warp[level]].phases, out_warp, level + 1);
        ImGui::PopID();
    }
}

uint8_t* ThModern_ECLGetSub(const char* name, uintptr_t param)
{
    struct ecl_sub_t {
        const char* name;
        uint8_t* data;
    };
    auto subs = (ecl_sub_t*)param;

    while (strcmp(subs->name, name))
        subs++;
    return subs->data;
};

void StageWarpsApply(stage_warps_t& warps, std::vector<unsigned int>& in_warp, ecl_get_sub_t* ECLGetSub, uintptr_t ecl_get_sub_param, size_t level)
{
    if (!in_warp.size())
        return;
    auto& param = warps.section_param[in_warp[level]];

    // This entire block gives me the idea to convert everything to writes once there's a JSON
    // file. But for readability, as long as there is no JSON file, this block will have to stay
    for (auto& jumps : param.jumps) {
        uint8_t* ecl = ECLGetSub(jumps.first.c_str(), ecl_get_sub_param);
        for (auto& jmp : jumps.second) {
            ecl_write_t real_write;
            real_write.off = jmp.off;
            union i32b {
                uint32_t i;
                uint8_t b[4];
                i32b(uint32_t a)
                    : i(a)
                {
                }
            };

            i32b ecl_time = jmp.ecl_time;
            uint8_t instr[] = { 0x0c, 0x00, 0x18, 0x00, 0x00, 0x00, 0xff, 0x2c, 0x00, 0x00, 0x00, 0x00 };
            i32b dest = jmp.dest - jmp.off;
            i32b at_frame = jmp.at_frame;

#define BYTES_APPEND(a)                    \
    for (size_t j = 0; j < sizeof(a); j++) \
        real_write.bytes.push_back(a[j]);

            BYTES_APPEND(ecl_time.b);
            BYTES_APPEND(instr);
            BYTES_APPEND(dest.b);
            BYTES_APPEND(at_frame.b);
#undef BYTES_APPEND

            real_write.apply(ecl);
        }
    }

    for (auto& writes : param.writes) {
        uint8_t* ecl = ECLGetSub(writes.first.c_str(), ecl_get_sub_param);
        for (auto& write : writes.second) {
            write.apply(ecl);
        }
    }

    if (param.phases)
        StageWarpsApply(*param.phases, in_warp, ECLGetSub, ecl_get_sub_param, level + 1);
}
#pragma endregion

#pragma region Game State
bool GameState_Assert(bool cond)
{
    if (cond)
        return true;

    int res = MessageBoxW(NULL,
        L"CORRUPT GAME STATE DETECTED!!!\n\n"
        L"Your game will likely crash very soon\n"
        L"Would you like to proceed anyways?",
        L"FATAL ERROR", MB_ICONERROR | MB_YESNO);
    if (res == IDYES)
        return false;
    else
        ExitProcess(UINT_MAX);
}
#pragma endregion
}