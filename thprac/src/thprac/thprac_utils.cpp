#include "thprac_utils.h"
#include "thprac_licence.h"
#include "thprac_launcher_main.h"
#include "thprac_launcher_cfg.h"
#include <metrohash128.h>
#include "thprac_data_anly.h"

namespace THPrac {

#pragma region Gui Wrapper

int g_gameGuiImpl = -1;
DWORD* g_gameGuiDevice = nullptr;
DWORD* g_gameGuiHwnd = nullptr;
HIMC g_gameIMCCtx = 0;

void VoidJoystickAPI()
{
    auto pJoyGetPosEx = GetProcAddress(GetModuleHandleA("winmm.dll"), "joyGetPosEx");

    if (pJoyGetPosEx) {
        DWORD oldProtect;
        VirtualProtect(pJoyGetPosEx, 8, PAGE_EXECUTE_READWRITE, &oldProtect);
        ((uint32_t*)pJoyGetPosEx)[0] = 0xCC0008C2;
        ((uint32_t*)pJoyGetPosEx)[1] = 0xCC0008C2;
        ((uint32_t*)pJoyGetPosEx)[0] = 0xA7B0C031;
        FlushInstructionCache(GetCurrentProcess(), pJoyGetPosEx, 8);
        VirtualProtect(pJoyGetPosEx, 8, oldProtect, &oldProtect);
    }
}

void GameGuiInit(game_gui_impl impl, int device, int hwnd, int wndproc_addr,
    Gui::ingame_input_gen_t input_gen, int reg1, int reg2, int reg3,
    int wnd_size_flag, float x, float y)
{
    ::ImGui::CreateContext();
    ::ImPlot::CreateContext();
    g_gameGuiImpl = impl;
    g_gameGuiDevice = (DWORD*)device;
    g_gameGuiHwnd = (DWORD*)hwnd;
    g_gameIMCCtx = ImmAssociateContext(*(HWND*)hwnd, 0);
    VoidJoystickAPI();

    // Set Locale
    GuiLauncherLocaleInit();

    switch (impl) {
    case THPrac::IMPL_WIN32_DX8:
        // Impl
        Gui::ImplDX8Init((IDirect3DDevice8*)*g_gameGuiDevice);
        Gui::ImplWin32Init((HWND)*g_gameGuiHwnd);

        // Hooks
        Gui::ImplDX8HookReset();
        Gui::ImplWin32HookWndProc((void*)wndproc_addr);
        break;
    case THPrac::IMPL_WIN32_DX9:
        // Impl
        Gui::ImplDX9Init((IDirect3DDevice9*)*g_gameGuiDevice);
        Gui::ImplWin32Init((HWND)*g_gameGuiHwnd);

        // Hooks
        Gui::ImplDX9HookReset();
        Gui::ImplWin32HookWndProc((void*)wndproc_addr);
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
    //LocaleCreateMergeFont(Gui::LocaleGet(), io.DisplaySize.x * 0.025f);

    if (!Gui::ImplWin32CheckFullScreen()) {
        RECT wndRect;
        GetClientRect(*(HWND*)hwnd, &wndRect);
        auto frameSize = GetSystemMetrics(SM_CXSIZEFRAME) * 2;
        auto captionSize = GetSystemMetrics(SM_CYCAPTION);
        auto longPtr = GetWindowLongPtrA(*(HWND*)hwnd, GWL_STYLE);
        SetWindowLongPtr(*(HWND*)hwnd, GWL_STYLE, longPtr | WS_SIZEBOX);
        SetWindowPos(*(HWND*)hwnd, HWND_NOTOPMOST,
            0, 0, wndRect.right + frameSize, wndRect.bottom + frameSize + captionSize,
            SWP_NOMOVE | SWP_NOZORDER | SWP_FRAMECHANGED);
    }

    // Imgui settings
    io.IniFilename = nullptr;
    ::ImGui::StyleColorsDark();
}

void GameGuiBegin(game_gui_impl impl, bool game_nav)
{
    // Locale Rotate
    /**
		if (Gui::KeyboardInputUpdate('1') == 1)
		{
			Gui::LocaleSet(Gui::LOCALE_JA_JP);
		}
		else if (Gui::KeyboardInputUpdate('2') == 1)
		{
			Gui::LocaleSet(Gui::LOCALE_ZH_CN);
		}
		else if (Gui::KeyboardInputUpdate('3') == 1)
		{
			Gui::LocaleSet(Gui::LOCALE_EN_US);
		}
		*/
    /*
		if (Gui::KeyboardInputUpdate(VK_OEM_3) == 1)
			Gui::LocaleRotate();
		*/

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
    default:
        break;
    }
}

void GameGuiEnd(game_gui_impl impl, bool draw_cursor)
{
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

    Gui::ImplWin32Check((void*)*g_gameGuiHwnd);
    switch (impl) {
    case THPrac::IMPL_WIN32_DX8:
        // End frame and render
        Gui::ImplDX8Check((IDirect3DDevice8*)*g_gameGuiDevice);
        ::ImGui::EndFrame();
        ::ImGui::Render();
        Gui::ImplDX8RenderDrawData(::ImGui::GetDrawData());
        break;
    case THPrac::IMPL_WIN32_DX9:
        // End frame and render
        Gui::ImplDX9Check((IDirect3DDevice9*)*g_gameGuiDevice);
        ::ImGui::EndFrame();
        ::ImGui::Render();
        Gui::ImplDX9RenderDrawData(::ImGui::GetDrawData());
        break;
    default:
        break;
    }    
}

void GameFreeze()
{


}

void GameToggleIME(bool toggle)
{
    ImmAssociateContext(*(HWND*)g_gameGuiHwnd, toggle ? g_gameIMCCtx : 0);
}

void TryKeepUpRefreshRate(void* address, void* address2)
{
    if (LauncherCfgInit(true)) {
        bool tryRefreshRateChange = false;
        if (LauncherSettingGet("unlock_refresh_rate", tryRefreshRateChange) && tryRefreshRateChange) {
            DWORD oldProtect;

            VirtualProtect(address, 1, PAGE_EXECUTE_READWRITE, &oldProtect);
            *(uint8_t*)address = 0;
            VirtualProtect(address, 1, oldProtect, &oldProtect);

            VirtualProtect(address2, 1, PAGE_EXECUTE_READWRITE, &oldProtect);
            *(uint8_t*)address2 = (uint8_t)0xeb;
            VirtualProtect(address2, 1, oldProtect, &oldProtect);
        }
    }
}

void TryKeepUpRefreshRate(void* address)
{
    if (LauncherCfgInit(true)) {
        bool tryRefreshRateChange = false;
        if (LauncherSettingGet("unlock_refresh_rate", tryRefreshRateChange) && tryRefreshRateChange) {
            DWORD oldProtect;

            VirtualProtect(address, 1, PAGE_EXECUTE_READWRITE, &oldProtect);
            *(uint8_t*)address = 0;
            VirtualProtect(address, 1, oldProtect, &oldProtect);
        }
    }
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
    MessageBoxW(NULL, _msg, _title, type);
}

void CenteredText(const char* text, float wndX)
{
    ImGui::SetCursorPosX((wndX - ImGui::CalcTextSize(text).x) / 2.0f);
    ImGui::Text(text);
}

float GetRelWidth(float rel)
{
    return ImGui::GetIO().DisplaySize.x * rel;
}

float GetRelHeight(float rel)
{
    return ImGui::GetIO().DisplaySize.y * rel;
}

void CalcFileHash(const char* file_name, uint64_t hash[2])
{
    hash[0] = 0ll;
    hash[1] = 0ll;

    auto hFile = CreateFileA(file_name, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    auto fileSize = GetFileSize(hFile, NULL);
    auto fileMap = CreateFileMappingA(hFile, NULL, PAGE_READONLY, 0, fileSize, NULL);
    auto fileMapView = MapViewOfFile(fileMap, FILE_MAP_READ, 0, 0, fileSize);

    MetroHash128 metro;
    metro.Hash((uint8_t*)fileMapView, fileSize, (uint8_t*)hash);

    UnmapViewOfFile(fileMapView);
    CloseHandle(fileMap);
    CloseHandle(hFile);
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

bool GameFPSOpt(adv_opt_ctx& ctx)
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
    char* fpsMultiplierStr;
    auto fontSize = ImGui::GetFontSize();

    if (fps == 0) {
        if (ctx.fps_status == 1) {
            fpsStatic = fps = 60;
            ctx.fps_replay_slow = fpsSlowStatic = 15;
            ctx.fps_replay_fast = 60;
            fpsFastStatic = 1;
            fpsDebugAcc = ctx.fps_debug_acc;
        }
        else if (ctx.fps_status == 2) {
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
            ImGui::TextColored(ImColor(255, 0, 0), XSTR(TH_FPS_ERR));
        else if (ctx.fps_status == -1)
            ImGui::TextColored(ImColor(255, 0, 0), XSTR(TH_FPS_UNSUPPORTED));
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
        ImGui::DragInt(XSTR(TH_FPS_ADJ), &fps, 1.0f, 60, 6000);
        if (!ImGui::IsItemActive())
            if (fps < 60)
                fps = 60;
            else if (fps > 6000)
                fps = 6000;
    } else {
        ImGui::SliderInt(XSTR(TH_FPS_ADJ), &fpsMultiplier, 0, 8, fpsMultiplierStr);
        fps = fpsMultiplier * 15 + 60;
    }
    ImGui::PopItemWidth();
    ImGui::SameLine();
    if (ImGui::Checkbox(XSTR(TH_FPS_FREE_ADJ), &canFpsChangeFreely)) {
        if (!canFpsChangeFreely) {
            int i = 0;
            for (; (i * 15 + 60) <= fps && i <= 8; ++i)
                ;
            fpsMultiplier = --i;
            fps = fpsMultiplier * 15 + 60;
        }
    }

    ImGui::PushItemWidth(GetRelWidth(0.23f));
    if (fpsFastStatic > 20) {
        sprintf(tmpStr, "infinite");
    } else {
        sprintf(tmpStr, "x%d.0 (%dfps)", fpsFastStatic, fpsFastStatic * 60);
    }
    ImGui::SliderInt("Replay slow FPS", &fpsSlowStatic, 1, 60);
    ImGui::SliderInt("Replay fast FPS", &fpsFastStatic, 1, 21, tmpStr);
    ImGui::PopItemWidth();
    ImGui::Checkbox("Debug acc.", (bool*)&fpsDebugAcc);
    ImGui::SameLine();
    HelpMarker("Blah");

    if (fpsStatic != fps
        || fpsSlowStatic != ctx.fps_replay_slow 
        || fpsFastStatic != ctx.fps_replay_fast / 60 
        || fpsDebugAcc != ctx.fps_debug_acc) {
        ImGui::SameLine();
        if (ImGui::Button(XSTR(TH_ADV_OPT_APPLY))) {
            clickedApply = true;
            if (fpsStatic > fps && ctx.fps_status != 1)
                ImGui::Text(XSTR(TH_FPS_LOWERING));
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

    hasChanged |= ImGui::Checkbox(XSTR(TH_FACTOR_ACB), &ctx.all_clear_bonus);
    ImGui::SameLine();
    HelpMarker(XSTR(TH_FACTOR_ACB_DESC));

    return hasChanged;
}

bool DataRecOpt(adv_opt_ctx& ctx, bool preUpd, bool isInGame)
{
    enum TestDataType {
        PERIODIC_STAT,
        PLAYER_MISS,
        PLAYER_BOMB,
        PLAYER_ACTIVE_TRANCE,
        PLAYER_PASSIVE_TRANCE,

    };

    char tmpStr[256];
    if (preUpd) {
        return false;
    }
    static bool showDataTree = false;
    static bool showPlot = true;
    static bool showDemo = false;
    static bool useDerivative = false;

    // Title
    bool backButton = false;
    if (ImGui::Button(XSTR(TH_BACK))) {
        backButton = true;
    }
    ImGui::SameLine();
    CenteredText(XSTR(TH_DATANLY), ImGui::GetWindowSize().x);
    ImGui::Separator();
    
    //ImGui::Separator();
    //ImGui::Separator();

    // Anlysis manage
    ImGui::Checkbox("Demo", &showDemo);
    if (showDemo) {
        ImPlot::ShowDemoWindow();
        ImGui::Separator();
        ImGui::Separator();
    }

    
    if (ImGui::Button("Load")) {
        AnlyLoadTest();
    }
    ImGui::Checkbox("Show Plot", &showPlot);
    ImGui::Checkbox("Show Tree", &showDataTree);
    auto& stageData = AnlyDataGet().stageData;
    if (ImGui::Button("Clear")) {
        stageData.clear();
    }
    ImGui::SameLine();
    if (ImGui::Button("Save")) {
        AnlyWriteTest();
    }
    for (size_t asd = 0; asd < stageData.size(); ++asd) {
        auto& frameData = stageData[asd].frameData;
        sprintf_s(tmpStr, "Stage %d", asd + 1);
        ImGui::SetNextTreeNodeOpen(true, ImGuiCond_FirstUseEver);
        if (ImGui::TreeNode(tmpStr)) {
            if (showPlot) {
                ImGui::Checkbox("Derivative", &useDerivative);
                std::vector<uint32_t> x;
                std::vector<uint32_t> y;
                std::vector<uint32_t> z;
                std::vector<std::pair<uint32_t, uint32_t>> chpater;
                EventRecord::OmniData32 prev;
                prev.i = 0;

                for (size_t i = 0; i < frameData.size(); ++i) {
                    for (auto& event : frameData[i].eventData) {
                        if (event.id == EVENT_DATA_COLLECT) {
                            x.push_back(frameData[i].frame);
                            if (useDerivative) {
                                y.push_back(i ? event.valueData[0].i - prev.i : 0);
                            } else {
                                y.push_back(event.valueData[0].i);
                            }
                            prev = event.valueData[0];
                        } else if (event.id == EVENT_SECTION) {
                            chpater.push_back(std::pair<uint32_t, uint32_t>(frameData[i].frame, event.valueData[0].i));
                        }
                    }
                }

                ImPlot::SetNextAxesToFit();
                if (ImPlot::BeginPlot("Line Plot")) {
                    ImPlot::SetupAxes("Time", "Value");
                    ImPlot::PlotStairs("Score", x.data(), y.data(), x.size());
                    //ImPlot::PlotStairs("Value", x.data(), z.data(), x.size());
                    for (auto& ch : chpater) {
                        ImPlot::TagX((double)ch.first, ImVec4(0, 1, 1, 1), "%s", std::to_string(ch.second).c_str());
                    }
                    ImPlot::EndPlot();
                }
                ImGui::Text("%d, %d, %d", x.size(), y.size(), z.size());
            }

            if (showDataTree) {
                for (auto& frame : frameData) {
                    sprintf_s(tmpStr, "Frame %d", frame.frame);
                    if (ImGui::TreeNode(tmpStr)) {
                        for (auto& record : frame.eventData) {
                            sprintf_s(tmpStr, "Data %u", record.id);
                            if (ImGui::TreeNode(tmpStr)) {
                                for (auto& value : record.valueData) {
                                    sprintf_s(tmpStr, "%u", value.i);
                                    ImGui::Selectable(tmpStr);
                                }
                                ImGui::TreePop();
                            }
                        }
                        ImGui::TreePop();
                    }
                }
            }
            ImGui::TreePop();
        }
    }
    return backButton;
}

void AboutOpt(const char* thanks_text)
{
    static bool showLicense = false;
    if (BeginOptGroup<TH_ABOUT_THPRAC>()) {
        ImGui::Text(XSTR(TH_ABOUT_VERSION), GetVersionStr());
        ImGui::Text(XSTR(TH_ABOUT_AUTHOR));
        ImGui::Text(XSTR(TH_ABOUT_WEBSITE));

        ImGui::NewLine();
        ImGui::Text(XSTR(TH_ABOUT_THANKS), thanks_text ? thanks_text : "You!");

        ImGui::NewLine();
        if (showLicense) {
            if (ImGui::Button(XSTR(TH_ABOUT_HIDE_LICENCE)))
                showLicense = false;
        } else {
            if (ImGui::Button(XSTR(TH_ABOUT_SHOW_LICENCE)))
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

bool ReplaySaveParam(const char* rep_path, std::string& param)
{
    auto repFile = CreateFileA(rep_path, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (repFile == INVALID_HANDLE_VALUE)
        return false;
    DWORD repMagic, bytesRead;
    if ((SetFilePointer(repFile, 0, NULL, FILE_BEGIN) != INVALID_SET_FILE_POINTER) && (ReadFile(repFile, &repMagic, sizeof(LONG), &bytesRead, NULL))) {
        if (repMagic == 'PR6T' || repMagic == 'PR7T') {
            auto paramSize = param.size();
            for (paramSize++; paramSize % 4; paramSize++)
                ;
            auto paramBuf = malloc(paramSize + 8);
            memset(paramBuf, 0, paramSize);
            memcpy(paramBuf, param.data(), param.size());
            *(int32_t*)((int)paramBuf + paramSize) = paramSize;
            *(int32_t*)((int)paramBuf + paramSize + 4) = 'CARP';

            SetFilePointer(repFile, 0, NULL, FILE_END);
            WriteFile(repFile, paramBuf, paramSize + 8, &bytesRead, NULL);
            free(paramBuf);

            // Recalculate checksum
            auto repSize = GetFileSize(repFile, NULL);
            uint8_t* repBuf = (uint8_t*)malloc(repSize - (repMagic == 'PR6T' ? 14 : 13));
            SetFilePointer(repFile, repMagic == 'PR6T' ? 14 : 13, NULL, FILE_BEGIN);
            ReadFile(repFile, repBuf, repSize - (repMagic == 'PR6T' ? 14 : 13), &bytesRead, NULL);

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

            SetFilePointer(repFile, 8, NULL, FILE_BEGIN);
            WriteFile(repFile, &checksum, 4, &bytesRead, NULL);

            free(repBuf);
        } else {
            auto paramSize = param.size() + 12;
            for (paramSize++; paramSize % 4; paramSize++)
                ;
            auto paramBuf = malloc(paramSize);
            memset(paramBuf, 0, paramSize);
            *(int32_t*)((int)paramBuf) = 'RESU';
            *(int32_t*)((int)paramBuf + 4) = paramSize;
            *(int32_t*)((int)paramBuf + 8) = 'CARP';
            memcpy((void*)((int)paramBuf + 12), param.data(), param.size());

            SetFilePointer(repFile, 0, NULL, FILE_END);
            WriteFile(repFile, paramBuf, paramSize, &bytesRead, NULL);
            free(paramBuf);
        }
    }

    CloseHandle(repFile);
    return false;
}

bool ReplayLoadParam(const char* rep_path, std::string& param)
{
    DWORD repMagic, bytesRead;
    param = "";

    auto repFile = CreateFileA(rep_path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (repFile == INVALID_HANDLE_VALUE)
        return false;

    SetFilePointer(repFile, 0, NULL, FILE_BEGIN);
    ReadFile(repFile, &repMagic, 4, &bytesRead, NULL);
    if (bytesRead == 4) {
        if (repMagic == 'PR6T' || repMagic == 'PR7T') {
            DWORD magic, paramLength;
            DWORD repSize = GetFileSize(repFile, NULL);

            SetFilePointer(repFile, -4, NULL, FILE_END);
            ReadFile(repFile, &magic, 4, &bytesRead, NULL);
            if (bytesRead == 4 && magic == 'CARP') {
                SetFilePointer(repFile, -8, NULL, FILE_CURRENT);
                ReadFile(repFile, &paramLength, 4, &bytesRead, NULL);
                if (bytesRead == 4 && paramLength > 0 && paramLength < repSize && paramLength < 512) {
                    void* buf;

                    SetFilePointer(repFile, ~paramLength - 3, NULL, FILE_CURRENT);
                    buf = malloc(paramLength + 1);
                    memset(buf, 0, paramLength + 1);
                    ReadFile(repFile, buf, paramLength, &bytesRead, NULL);
                    if (bytesRead == paramLength)
                        param = std::string((char*)buf, paramLength + 1);
                    free(buf);
                    CloseHandle(repFile);

                    return (bytesRead == paramLength);
                }
            }
        } else {
            DWORD userPtr, userMagic, userLength, userNo;

            SetFilePointer(repFile, 12, NULL, FILE_BEGIN);
            ReadFile(repFile, &userPtr, 4, &bytesRead, NULL);
            if (bytesRead == 4) {
                SetFilePointer(repFile, userPtr, NULL, FILE_BEGIN);
                while (true) {
                    ReadFile(repFile, &userMagic, 4, &bytesRead, NULL);
                    if (bytesRead != 4 || userMagic != 'RESU')
                        break;
                    ReadFile(repFile, &userLength, 4, &bytesRead, NULL);
                    if (bytesRead != 4)
                        break;
                    ReadFile(repFile, &userNo, 4, &bytesRead, NULL);
                    if (bytesRead != 4)
                        break;

                    if (userNo == 'CARP') {
                        void* buf;

                        buf = malloc(userLength - 12 + 1);
                        memset(buf, 0, userLength - 12 + 1);
                        ReadFile(repFile, buf, userLength - 12, &bytesRead, NULL);
                        if (bytesRead == userLength - 12)
                            param = std::string((char*)buf, userLength - 12 + 1);
                        free(buf);
                        CloseHandle(repFile);

                        return (bytesRead == userLength - 12);
                    } else {
                        SetFilePointer(repFile, userLength - 12, NULL, FILE_CURRENT);
                    }
                }
            }
        }
    }

    CloseHandle(repFile);
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
#if 0
struct _listener {
    vfs_listener onCall;
    vfs_listener onLoad;
};
static tsl::robin_map<std::string, _listener> __vfs_listener;
static _listener __vfs_g_listener { nullptr, nullptr };

typedef void*(__stdcall* p_vfs_main)(const char* file_name, int32_t* file_size, int32_t is_file);
static p_vfs_main __vfs_original;

static hook_ctx __vfs_hook;

static void* __stdcall __VFSMain(const char* file_name, int32_t* file_size, int32_t is_file)
{
    // Disable Hook, initalize variables.
    std::string mFileName(file_name);
    auto l = __vfs_listener.find(mFileName);
    void* fileBuffer;

    // Load File
    if (l != __vfs_listener.end() && l->second.onCall) {
        fileBuffer = l->second.onCall(file_name, file_size, is_file, nullptr);
        if (!fileBuffer)
            fileBuffer = __vfs_original(file_name, file_size, is_file);

    } else if (__vfs_g_listener.onCall) {
        fileBuffer = __vfs_g_listener.onCall(file_name, file_size, is_file, nullptr);
        if (!fileBuffer)
            fileBuffer = __vfs_original(file_name, file_size, is_file);
    } else {
        fileBuffer = __vfs_original(file_name, file_size, is_file);
    }

    // Post Process
    if (l != __vfs_listener.end() && l->second.onLoad) {
        l->second.onLoad(file_name, file_size, is_file, fileBuffer);
    } else if (__vfs_g_listener.onLoad) {
        __vfs_g_listener.onLoad(file_name, file_size, is_file, fileBuffer);
    }
    return fileBuffer;
}

// TH11: File name at eax, others in stack
__declspec(naked) static void __vfs_th11_trampoline()
{
    __asm
    {
			push [esp + 8]
			push [esp + 8]
			push eax
			call __VFSMain
			retn 8
    }
}
static void* __stdcall __vfs_th11_original(const char* file_name, int32_t* file_size, int32_t is_file)
{
    char* ret_var;
    void* func = __vfs_hook.trampoline;
    __asm
    {
			push is_file
			push file_size
			mov eax, file_name
			call func
			mov ret_var, eax
    }
    return ret_var;
}

// TH08: File name at ecx, pBytesRead at edx, others in stack
__declspec(naked) static void __vfs_th08_trampoline()
{
    __asm
    {
			push [esp + 4]
			push edx
			push ecx
			call __VFSMain
			retn 4
    }
}
static void* __stdcall __vfs_th08_original(const char* file_name, int32_t* file_size, int32_t is_file)
{
    char* ret_var;
    void* func = __vfs_hook.trampoline;
    __asm
    {
			push is_file
			mov edx, file_size
			mov ecx, file_name
			call func
			mov ret_var, eax
    }
    return ret_var;
}

// TH07: File name at ecx, isFile at edx, pBytesRead is a global variable
__declspec(naked) static void __vfs_th07_trampoline()
{
    __asm
    {
			push edx
			push 0x4b9e64
			push ecx
			call __VFSMain
			retn
    }
}
static void* __stdcall __vfs_th07_original(const char* file_name, int32_t* file_size, int32_t is_file)
{
    char* ret_var;
    void* func = __vfs_hook.trampoline;
    __asm
    {
			mov edx, is_file
			mov ecx, file_name
			call func
			mov ret_var, eax
    }
    return ret_var;
}

// TH06: All in stack, pBytesRead is a global variable
__declspec(naked) static void __vfs_th06_trampoline()
{
    __asm
    {
			push [esp + 8]
			push 0x69d914
			push [esp + 12]
			call __VFSMain
			retn
    }
}
static void* __stdcall __vfs_th06_original(const char* file_name, int32_t* file_size, int32_t is_file)
{
    char* ret_var;
    void* func = __vfs_hook.trampoline;
    __asm
    {
			push is_file
			push file_name
			call func
			add esp, 8
			mov ret_var, eax
    }
    return ret_var;
}

// API
void VFSHook(VFS_TYPE type, void* addr)
{
    if (__vfs_hook.hook)
        delete __vfs_hook.hook;

    __vfs_hook.target = addr;
    __vfs_hook.method = HOOK_INLINE;

    // Type Switch
    switch (type) {
    case VFS_TH06:
        __vfs_hook.detour = __vfs_th06_trampoline;
        __vfs_original = __vfs_th06_original;
        break;
    case VFS_TH07:
        __vfs_hook.detour = __vfs_th07_trampoline;
        __vfs_original = __vfs_th07_original;
        break;
    case VFS_TH08:
        __vfs_hook.detour = __vfs_th08_trampoline;
        __vfs_original = __vfs_th08_original;
        break;
    case VFS_TH11:
        __vfs_hook.detour = __vfs_th11_trampoline;
        __vfs_original = __vfs_th11_original;
        break;
    default:
        break;
    }

    Hook::CreateHook(__vfs_hook);
}
void VFSAddListener(const char* file_name, vfs_listener onCall, vfs_listener onLoad)
{
    if (!file_name) {
        __vfs_g_listener.onCall = onCall;
        __vfs_g_listener.onLoad = onLoad;
        return;
    }
    std::string file(file_name);
    _listener l;
    l.onCall = onCall;
    l.onLoad = onLoad;
    __vfs_listener[file] = l;
}
void* VFSOriginal(const char* file_name, int32_t* file_size, int32_t is_file)
{
    if (!__vfs_original)
        return nullptr;
    return __vfs_original(file_name, file_size, is_file);
}
#endif
#pragma endregion

}