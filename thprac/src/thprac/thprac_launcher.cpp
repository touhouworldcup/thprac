// Based on https://github.com/ocornut/imgui/discussions/3925
// Fun fact: the author of that is also a Touhou fan

#include "thprac_launcher.h"
#include "thprac_load_exe.h"

#include <psapi.h>

#define GET_X_LPARAM(lp) ((int)(short)LOWORD(lp))
#define GET_Y_LPARAM(lp) ((int)(short)HIWORD(lp))

namespace THPrac {
namespace Gui {
    extern LRESULT ImplWin32WndProcHandlerW(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
    extern HWND ImplWin32GetHwnd();
    extern IDirect3DDevice9* ImplDX9GetDevice();
}

static const wchar_t* LAUNCHER_TITLE_W[] = { L"thprac - 东方游戏启动器", L"thprac - Touhou Game Launcher", L"thprac - 東方ゲームランチャー" };

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

void ResetDevice();
bool UpdateUIScaling(float scale = 1.0f);

#define TITLE_BAR_HEIGHT() (ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2)

// This title bar is inspriried by the title bar from the old launcher provided by a modified ImGui::Begin,
// but it's remade from scratch and reponds to minimize and close button presses by sending messages to 'hwnd' directly
// The old launcher enabled the title bar when calling ImGui::Begin and used the buttons provided by that.
// However, I haven't found a way to check if the title bar is selected, or if any of the title bar's buttons are selected
// It also seems like the title bar code was modified by thprac's previous behaviour, ACK, in order to add a real minimize button.
// I want to cut down on non-standard Dear ImGui as much as possible to make it easier to upgrade Dear ImGui in the future.
inline float DrawTitleBar(HWND hwnd, bool* overBtn, const char* title) {
    auto& io = ImGui::GetIO();
    const float tbH = TITLE_BAR_HEIGHT();

    ImGui::GetWindowDrawList()->AddRectFilled(
        ImVec2(0.0f, 0.0f),
        ImVec2(io.DisplaySize.x, tbH),
        ImGui::GetColorU32(ImGuiCol_TitleBgActive));

    ImGui::SetCursorPos(ImVec2(4.0f, (tbH - ImGui::GetTextLineHeight()) * 0.5f));
    ImGui::TextUnformatted(title);

    float cross_extent = g_Scale * 7.071f - 1.0f;

    // Minimize Button
    {
        ImVec2 btnPos = { io.DisplaySize.x - tbH * 2.0f, 0.0f };
        ImVec2 btnCenter = { btnPos.x + (tbH / 2), btnPos.y + (tbH / 2) };

        ImGui::SetCursorPos(btnPos);
        if (ImGui::InvisibleButton("##MINIMIZE", { tbH, tbH })) {
            ShowWindow(hwnd, SW_MINIMIZE);
        }

        if (ImGui::IsItemHovered()) {
            ImGui::GetWindowDrawList()->AddCircleFilled(btnCenter, (tbH / 2) - 2.0f, ImGui::GetColorU32(ImGuiCol_ButtonHovered));
        }

        ImVec2 lineLeft = btnCenter;
        ImVec2 lineRight = btnCenter;

        lineLeft.x -= cross_extent;
        lineRight.x += cross_extent;

        ImGui::GetWindowDrawList()->AddLine(lineLeft, lineRight, ImGui::GetColorU32(ImGuiCol_Text), 1.5f * g_Scale);
    }

    // Close button
    {
        ImVec2 btnPos = { io.DisplaySize.x - tbH, 0.0f };
        ImVec2 btnCenter = { btnPos.x + (tbH / 2), btnPos.y + (tbH / 2) };

        ImGui::SetCursorPos(btnPos);
        if (ImGui::InvisibleButton("##CLOSE", { tbH, tbH })) {
            SendMessageW(hwnd, WM_CLOSE, 0, 0);
        }

        if (ImGui::IsItemHovered()) {
            ImGui::GetWindowDrawList()->AddCircleFilled(btnCenter, (tbH / 2) - 2.0f, ImGui::GetColorU32(ImGuiCol_ButtonHovered));
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

        ImGui::GetWindowDrawList()->AddLine(lineBottomRight, lineTopLeft, ImGui::GetColorU32(ImGuiCol_Text), g_Scale);
        ImGui::GetWindowDrawList()->AddLine(lineTopRight, lineBottomLeft, ImGui::GetColorU32(ImGuiCol_Text), g_Scale);
    }

    *overBtn = ImGui::IsAnyItemHovered();
    ImGui::SetCursorPos({ 0.0f, tbH });
    return tbH;
}

void LauncherToolsMain(LauncherState* state) {
    if (state->toolFunc) {
        return state->toolFunc(state);
    }

    ImVec2 buttonSize = { (ImGui::GetWindowWidth() / 4.0f), 0.0f };
    float btnPosX = (ImGui::GetWindowWidth() / 2) - (buttonSize.x / 2);

    float step = ImGui::GetWindowHeight() / 4.0f;
    float height = 135.0f;

    ImGui::SetCursorPos({ btnPosX,  height + step * 0 });
    if (ImGui::Button(S(THPRAC_TOOLS_APPLY_THPRAC), buttonSize)) {
        FindAndAttach(true, true);
    }
    ImGui::SetCursorPos({ btnPosX,  height + step * 1 });
    if (ImGui::Button(S(THPRAC_TOOLS_RND_GAME), buttonSize)) {
        state->toolFunc = RandomGameRollUI;
    }
    ImGui::SetCursorPos({ btnPosX,  height + step * 2 });
    if (ImGui::Button(S(THPRAC_TOOLS_RND_PLAYER), buttonSize)) {
        state->toolFunc = RandomShotRollUI;
    }
}

static bool LauncherSettingsCheckThcrapDir(std::wstring_view dir) {
    std::wstring_view dll_release = L"\\bin\\thcrap.dll";
    std::wstring_view dll_debug = L"\\bin\\thcrap_d.dll";

    wchar_t dll_path[MAX_PATH + 1] = {};

    if (dir.length() + dll_debug.length() > MAX_PATH) {
        return false;
    }
    
    memcpy(dll_path, dir.data(), dir.length() * sizeof(wchar_t));
    memcpy(dll_path + dir.length(), dll_release.data(), dll_release.length() * sizeof(wchar_t));
    if (GetFileAttributesW(dll_path) != INVALID_FILE_ATTRIBUTES) {
        return true;
    }

    memcpy(dll_path + dir.length(), dll_debug.data(), dll_debug.length() * sizeof(wchar_t));
    return GetFileAttributesW(dll_path) != INVALID_FILE_ATTRIBUTES;
}

static void LauncherSettingsMain(LauncherState* state) {
    if (state->foundThcrapConfigs.size()) {
        return ThcrapAddConfigsUI(state);
    }

    ImGui::TextUnformatted(S(THPRAC_LAUNCHER_SETTINGS));
    ImGui::Separator();
    ImGui::Combo(   S(THPRAC_AFTER_LAUNCH),         (int*)&state->settings.after_launch,         S(THPRAC_AFTER_LAUNCH_OPTION));
    ImGui::Combo(   S(THPRAC_APPLY_THPRAC_DEFAULT), (int*)&state->settings.apply_thprac_default, S(THPRAC_APPLY_THPRAC_DEFAULT_OPTION));
    ImGui::Checkbox(S(THPRAC_AUTO_DEFAULT_LAUNCH),        &state->settings.auto_default_launch);
    ImGui::SameLine();
    Gui::HelpMarker(S(THPRAC_AUTO_DEFAULT_LAUNCH_DESC));
    ImGui::NewLine();
    ImGui::TextUnformatted("thcrap"); // Doesn't need translation
    ImGui::Separator();

    auto& t = state->settings.thcrap_dir;
    if (t) {
        ImGui::Text(S(THPRAC_THCRAP_LOCATION), t.s);
        if (ImGui::Button(S(THPRAC_THCRAP_UNSET))) {
            state->thcrapError = A0000ERROR_C;
            t.clear();
        }
        ImGui::SameLine();
        if (ImGui::Button(S(THPRAC_THCRAP_ADDCFG))) {
            state->thcrapError = A0000ERROR_C;

            wchar_t thcrapConfigDir[MAX_PATH + 1] = {};
            memcpy(thcrapConfigDir, t.w, t.w_len * sizeof(wchar_t));
            memcpy(thcrapConfigDir + t.w_len, SIZED(L"\\config\\*.js"));

            WIN32_FIND_DATAW find = {};
            HANDLE hFind = FindFirstFileW(thcrapConfigDir, &find);

            if (hFind) do {
                if (wcscmp(find.cFileName, L"games.js") == 0 || wcscmp(find.cFileName, L"config.js") == 0) {
                    continue;
                }

                auto& name = state->foundThcrapConfigs.emplace_back();
                WideCharToMultiByte(CP_UTF8, 0, find.cFileName, -1, name, MAX_PATH, nullptr, nullptr);
            } while (FindNextFileW(hFind, &find));
            if (state->foundThcrapConfigs.size()) {
                auto& v = state->foundThcrapConfigsSel;
                v.resize(state->foundThcrapConfigs.size());
                memset(v.data(), 0, v.size());
            } else {
                state->thcrapError = THPRAC_THCRAP_ADDCFG_404;
            }
        }
        ImGui::SameLine();
        if (ImGui::Button(S(THPRAC_THCRAP_LAUNCH_CONFIGURE))) {
            wchar_t conf_path[MAX_PATH + 1] = {};
            std::wstring_view conf_exe = L"\\bin\\thcrap_configure_v3.exe";
            
            if (t.w_len + conf_exe.length() < MAX_PATH) {
                memcpy(conf_path, t.w, t.w_len * sizeof(wchar_t));
                memcpy(conf_path + t.w_len, conf_exe.data(), conf_exe.length() * sizeof(wchar_t));
                
                if ((UINT_PTR)ShellExecuteW(Gui::ImplWin32GetHwnd(), L"open", conf_path, nullptr, nullptr, SW_SHOW) > 32) {
                    state->thcrapError = A0000ERROR_C;
                } else {
                    state->thcrapError = THPRAC_THCRAP_LAUNCH_FAILED;
                }
            }
        }
    } else {
        ImGui::TextUnformatted(S(THPRAC_THCRAP_NOTYET));
        if (ImGui::Button(S(THPRAC_THCRAP_GET))) {
            ShellExecuteW(Gui::ImplWin32GetHwnd(), L"open", L"https://thpatch.net", nullptr, nullptr, SW_SHOW);
            state->thcrapError = A0000ERROR_C;
        }
        ImGui::SameLine();
        if (ImGui::Button(S(THPRAC_THCRAP_SET))) {
            std::wstring new_thcrap_dir;
            if (SelectFolder(new_thcrap_dir, Gui::ImplWin32GetHwnd())) {
                if (LauncherSettingsCheckThcrapDir(new_thcrap_dir)) {
                    t = new_thcrap_dir;
                    state->thcrapError = A0000ERROR_C;
                } else {
                    state->thcrapError = THPRAC_THCRAP_INVALID;
                }
            }
        }
    }
    if (state->thcrapError != A0000ERROR_C) {
        ImGui::SameLine();
        ImGui::TextUnformatted(S(state->thcrapError));
    }

    ImGui::NewLine();

    // The rest of the settings, which will hopefully be displayed in-game too some day
    GuiSettings();
}

void UiUpdate(HWND hwnd, LauncherState* state) {
    if (Gui::LocaleGet() != gSettings.language) {
        Gui::LocaleSet(gSettings.language);
        SetWindowTextW(hwnd, LAUNCHER_TITLE_W[Gui::LocaleGet()]);
        UpdateUIScaling(g_Scale);
    }

    // Start the Dear ImGui frame
    Gui::ImplDX9NewFrame();
    Gui::ImplWin32NewFrame();
    state->g_IsUITextureIDValid = true;
    ImGui::NewFrame();

    auto& io = ImGui::GetIO();
    auto& style = ImGui::GetStyle();
    style.WindowBorderSize = 0.0f;
    
    ImGui::SetNextWindowPos({ 0.0f, 0.0f });
    ImGui::SetNextWindowSize(io.DisplaySize);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0.0f, 0.0f });
    ImGui::Begin("###_main_window", nullptr,
        ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::PopStyleVar();

    const char* title = background_update_check->hThread ? S(THPRAC_LAUNCHER_CHECKING_UPDATE) : S(THPRAC_LAUNCHER);
    float begin_height = DrawTitleBar(hwnd, &state->g_IsOverTitleBarButton, title);
    ImGui::BeginChild("###__content", { io.DisplaySize.x, io.DisplaySize.y - begin_height }, false, ImGuiWindowFlags_AlwaysUseWindowPadding);
    ImGui::BeginTabBar("__launcher_tab_bar");

    ImGuiTabItemFlags gameTabFlags = 0;
    if (state->goToGamesPage) {
        state->goToGamesPage = false;
        gameTabFlags = ImGuiTabItemFlags_SetSelected;
    }
    if (ImGui::BeginTabItem(S(THPRAC_LAUNCHER_TAB_GAMES), nullptr, gameTabFlags)) {
        ImGui::BeginChild(0x6A8E5);
        LauncherGamesMain(state);
        ImGui::EndChild();
        ImGui::EndTabItem();
    }
    if (ImGui::BeginTabItem(S(THPRAC_LAUNCHER_TAB_LINKS))) {
        // Links page has it's own child window for all it's content
        LauncherLinksMain(state);
        ImGui::EndTabItem();
    }
    if (ImGui::BeginTabItem(S(THPRAC_LAUNCHER_TAB_TOOLS))) {
        ImGui::BeginChild(0x70015);
        LauncherToolsMain(state);
        ImGui::EndChild();
        ImGui::EndTabItem();
    }
    if (ImGui::BeginTabItem(S(THPRAC_LAUNCHER_TAB_CONFG))) {
        ImGui::BeginChild(0xC02F16);
        LauncherSettingsMain(state);
        ImGui::EndChild();
        ImGui::EndTabItem();
    }
    ImGui::EndTabBar();

    if (state->reflectiveLaunchID && FindAndAttach(false, false, state->reflectiveLaunchID)) {
        state->reflectiveLaunchID = ID_UNKNOWN;
    }

    ImGui::EndChild();

    if (background_update_check && background_update_check->hThread) {
        DWORD waitStatus = WaitForSingleObject(background_update_check->hThread, 0);

        if (waitStatus == WAIT_OBJECT_0) {
            DWORD exitCode = 0xFFFFFFFF;
            GetExitCodeThread(background_update_check->hThread, &exitCode);

            if (exitCode == 0) {
                if (background_update_check->updateJson.ver > gVersion) {
                    ImGui::OpenPopup(S(THPRAC_UPDATE_MODAL));
                }
            }
            else {
                ImGui::OpenPopup(S(THPRAC_UPDATE_ERROR_MODAL));
            }
            CloseHandle(background_update_check->hThread);
            background_update_check->hThread = NULL;
        }
    }

    ImGui::SetNextWindowSize({ 355.0f, 128.0f });
    if (Gui::Modal(S(THPRAC_UPDATE_MODAL))) {
        if (state->hUpdateThread) {
            ImGui::TextUnformatted(S(THPRAC_UPDATE_DOWNLOADING));
            
            if (state->updateDownload.filesize) {
                float prog = (float)state->updateDownload.out.size() / (float)state->updateDownload.filesize;

                char txt[16] = {};
                char* txt_end = txt + snprintf(txt, 15, "%.2f%%", prog * 100);
                Gui::ProgressBar(prog, txt, txt_end);
            } else {
                Gui::ProgressBar(0.0f, "0.0%");
            }
            ImGui::SetCursorPosY(ImGui::GetWindowHeight() - (ImGui::GetFontSize() + style.FramePadding.y * 2 + style.ItemSpacing.y * 2));

            if (ImGui::Button(S(TH_CANCEL))) {
                state->updateDownload.abort_signal = true;
            }

            DWORD waitStatus = WaitForSingleObject(state->hUpdateThread, 0);
            if (waitStatus == WAIT_OBJECT_0) {
                DWORD exitCode = 0xFFFFFFFF;
                GetExitCodeThread(state->hUpdateThread, &exitCode);
                CloseHandle(state->hUpdateThread);
                state->hUpdateThread = NULL;
                if (exitCode == 0) {
                    CompleteUpdate(state->updateDownload.out.data(), state->updateDownload.out.size(), nullptr, SW_SHOW, &background_update_check->updateJson);
                    PostQuitMessage(0);
                } else {
                    state->updateDownload.out.clear();
                    ImGui::CloseCurrentPopup();
                }
            }
            else if (waitStatus != WAIT_TIMEOUT) {
                ImGui::CloseCurrentPopup();

            }
        } else {
            ImGui::Text(S(THPRAC_UPDATE_PROMPT), VER_PARAMS(background_update_check->updateJson.ver));
            switch (Gui::MultiButtonsFillWindow(0.0f, S(THPRAC_UPDATE_AUTO_UPDATE), S(THPRAC_UPDATE_DOWNLOAD_MANUALLY), nullptr)) {
            case 0:
                state->updateUrl = utf8_to_utf16(background_update_check->updateJson.url);
                state->updateDownload.url = state->updateUrl.c_str();
                state->hUpdateThread = CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)DownloadFile, &state->updateDownload, 0, nullptr);
                break;
            case 1:
                ShellExecuteA(Gui::ImplWin32GetHwnd(), "open", background_update_check->updateJson.url, nullptr, nullptr, SW_SHOW);
            }
            switch (Gui::MultiButtonsFillWindow(0.0f, S(THPRAC_UPDATE_VIEW_CHANGELOG), S(TH_CLOSE), nullptr)) {
            case 0:
                ShellExecuteA(Gui::ImplWin32GetHwnd(), "open", background_update_check->updateJson.changelog, nullptr, nullptr, SW_SHOW);
                break;
            case 1:
                ImGui::CloseCurrentPopup();
            }
        }
        ImGui::EndPopup();
    }
    if (Gui::Modal(S(THPRAC_UPDATE_ERROR_MODAL))) {
        ImGui::TextUnformatted(S(THPRAC_UPDATE_ERROR_ASK_DISABLE));
        switch (Gui::MultiButtonsFillWindow(0.0f, S(TH_YES), S(TH_NO), nullptr)) {
        case 0:
            gSettings.check_update = CHECK_UPDATE_NEVER;
        case 1:
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    ImGui::End();
    ImGui::EndFrame();
    ImGui::Render();

    auto* dev = Gui::ImplDX9GetDevice();

    dev->SetRenderState(D3DRS_ZENABLE, FALSE);
    dev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    dev->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
    dev->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_RGBA(0, 0, 0, 255), 1.0f, 0);
    if (dev->BeginScene() >= 0) {
        if (state->g_IsUITextureIDValid) {
            Gui::ImplDX9RenderDrawData(ImGui::GetDrawData());
        }
        dev->EndScene();
    }
    HRESULT result = dev->Present(NULL, NULL, NULL, NULL);

    // Handle loss of D3D9 device
    if (result == D3DERR_DEVICELOST && dev->TestCooperativeLevel() == D3DERR_DEVICENOTRESET) {
        state->g_IsUITextureIDValid = false;
        ResetDevice();
    }
}

void DwmTweaksForCustomTitlebar(HWND hwnd) {
    struct MARGINS {
        int cxLeftWidth;
        int cxRightWidth;
        int cyTopHeight;
        int cyBottomHeight;
    };

    typedef HRESULT WINAPI T_DwmIsCompositionEnabled(BOOL* pfEnabled);
    typedef HRESULT WINAPI T_DwmExtendFrameIntoClientArea(HWND hWnd, const MARGINS* pMarInset);

    auto* dwmapi = LoadLibraryW(L"dwmapi.dll");
    if (!dwmapi) {
        return;
    }

    auto* pDwmIsCompositionEnabled = (T_DwmIsCompositionEnabled*)GetProcAddress(dwmapi, "DwmIsCompositionEnabled");
    auto* pDwmExtendFrameIntoClientArea = (T_DwmExtendFrameIntoClientArea*)GetProcAddress(dwmapi, "DwmExtendFrameIntoClientArea");

    if (!pDwmIsCompositionEnabled || !pDwmExtendFrameIntoClientArea) {
        return;
    }

    BOOL compositon = FALSE;
    if (SUCCEEDED(pDwmIsCompositionEnabled(&compositon)) && compositon) {
        MARGINS margins = { -1, -1, -1, -1 };
        pDwmExtendFrameIntoClientArea(hwnd, &margins);
    }
}

void LoadLauncherSettings(LauncherSettings* launcherSettings) {
    wchar_t launcherSettingsPath[MAX_PATH + 1] = {};
    memcpy(launcherSettingsPath, _gConfigDir, _gConfigDirLen * sizeof(wchar_t));
    memcpy(launcherSettingsPath + _gConfigDirLen, SIZED(L"launcher.json"));

    yyjson_doc* doc = yyjson_read_file_report(launcherSettingsPath);
    if (!doc) {
        return;
    }

    yyjson_val* root = yyjson_doc_get_root(doc);

    size_t idx, max;
    yyjson_val *key, *val;
    yyjson_obj_foreach(root, idx, max, key, val) {
        if (unsafe_yyjson_equals_str(key, "after_launch")) {
            yyjson_eval_numeric(val, (int*)&launcherSettings->after_launch);
        }
        if (unsafe_yyjson_equals_str(key, "apply_thprac_default")) {
            yyjson_eval_numeric(val, (int*)&launcherSettings->apply_thprac_default);
        }
        if (unsafe_yyjson_equals_str(key, "auto_default_launch")) {
            yyjson_eval_numeric(val, &launcherSettings->auto_default_launch);
        }
        if (unsafe_yyjson_equals_str(key, "thcrap")) {
            if (const char* str = yyjson_get_str(val)) {
                if (size_t len = unsafe_yyjson_get_len(val)) {
                    launcherSettings->thcrap_dir = std::string_view(str, len);
                }
            }
        }
    }

    yyjson_doc_free(doc);
}

static const char launcherSettingsTemplate[] = 
    "{\n"
    "\t" R"("after_launch": %d,)" "\n"
    "\t" R"("apply_thprac_default": %d,)" "\n"
    "\t" R"("auto_default_launch": %s,)" "\n"
    "\t" R"("thcrap": "%.*s",)" "\n"
    "}";

void SaveLauncherSettings(LauncherSettings* launcherSettings) {

    char thcrap_dir_safe[MAX_PATH * 2] = {};
    
    char* thcrap_dir = launcherSettings->thcrap_dir.s;
    size_t thcrap_dir_len = 0;
    
    // If more characters need to be escaped, I'll use yyjson's write API
    for (size_t i = 0; i < launcherSettings->thcrap_dir.s_len; i++) {
        if (thcrap_dir[i] == '\\') {
            thcrap_dir_safe[thcrap_dir_len++] = '\\';
        }
        thcrap_dir_safe[thcrap_dir_len++] = thcrap_dir[i];
    }

    char buf[1024];
    int len = snprintf(buf, sizeof(buf) - 1, launcherSettingsTemplate
        , launcherSettings->after_launch
        , launcherSettings->apply_thprac_default
        , launcherSettings->auto_default_launch ? "true" : "false"
        , thcrap_dir_len, thcrap_dir_safe
    );

    wchar_t launcherSettingsPath[MAX_PATH + 1] = {};
    memcpy(launcherSettingsPath, _gConfigDir, _gConfigDirLen * sizeof(wchar_t));
    memcpy(launcherSettingsPath + _gConfigDirLen, SIZED(L"launcher.json"));

    HANDLE hFile = CreateFileW(launcherSettingsPath, GENERIC_WRITE, FILE_SHARE_READ, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    DWORD byteRet;
    WriteFile(hFile, buf, len, &byteRet, nullptr);
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
    g_WndCls.hIcon = LoadIconW(hInstance, (LPCWSTR)1);
    g_WndCls.hInstance = hInstance;
    if (!RegisterClassExW(&g_WndCls)) {
        return 1;
    }
    defer(UnregisterClassW(g_WndCls.lpszClassName, g_WndCls.hInstance));

    LauncherState* state = new LauncherState;

    HWND hwnd = CreateWindowExW(
        0, g_WndCls.lpszClassName, LAUNCHER_TITLE_W[Gui::LocaleGet()], WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 960, 720,
        NULL, NULL, g_WndCls.hInstance, state);

    if (!hwnd) {
        return 1;
    }
    defer(DestroyWindow(hwnd));
    DwmTweaksForCustomTitlebar(hwnd);
    
    IDirect3DDevice9* dev;
    if (d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &g_d3dpp, &dev) < 0) {
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
    SetTheme(gSettings.theme);

    // Setup Platform/Renderer backends
    if (!Gui::ImplWin32Init(hwnd)) {
        return 1;
    }
    defer(Gui::ImplWin32Shutdown());

    if (!Gui::ImplDX9Init(dev)) {
        return 1;
    }
    defer(Gui::ImplDX9Shutdown());

    HMONITOR hMonitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);

    float dpiscale = 1.0f;
    if (auto shcore = GetModuleHandleW(L"shcore.dll")) {
        typedef HRESULT(WINAPI * T_SetProcessDpiAwareness)(DWORD value);
        typedef HRESULT(WINAPI * T_GetDpiForMonitor)(HMONITOR hmonitor, DWORD dpiType, UINT * dpiX, UINT * dpiY);

        auto SetProcessDpiAwareness = (T_SetProcessDpiAwareness)GetProcAddress(shcore, "SetProcessDpiAwareness");
        auto GetDpiForMonitor = (T_GetDpiForMonitor)GetProcAddress(shcore, "GetDpiForMonitor");
        if (SetProcessDpiAwareness && GetDpiForMonitor) {
            SetProcessDpiAwareness(2);
            UINT dpiX;
            UINT dpiY;
            GetDpiForMonitor(hMonitor, 0, &dpiX, &dpiY);
            dpiscale = dpiX / (float)USER_DEFAULT_SCREEN_DPI;
        }
    }

    // In vanilla ImGui, ImGui_ImplWin32_NewFrame sets DisplaySize
    // THPrac's Gui::ImplWin32NewFrame removed that line because
    // it's common for Touhou games to have their window client
    // area and D3D9 backbuffer size differ.
    RECT cr;
    GetClientRect(hwnd, &cr);
    io.DisplaySize = {
        (float)(cr.right - cr.left),
        (float)(cr.bottom - cr.top)
    };

    state->g_IsUITextureIDValid = false;
    if (!UpdateUIScaling(dpiscale > 1.0f ? dpiscale : 1.0f)) {
        return 1;
    }

    int width = (int)(960.0f * dpiscale), height = (int)(720.0f * dpiscale);
    MONITORINFO mi = {
        .cbSize = sizeof(mi)
    };
    if (GetMonitorInfoW(hMonitor, &mi)) {
        int x = (mi.rcMonitor.right - mi.rcMonitor.left) / 2 - width / 2;
        int y = (mi.rcMonitor.bottom - mi.rcMonitor.top) / 2 - height / 2;
        SetWindowPos(hwnd, NULL, x, y, width, height, SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
    } else {
        SetWindowPos(hwnd, NULL, 0, 0, width, height, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
    }
    
    LoadLauncherSettings(&state->settings);
    LoadGamesJson(state->settings.apply_thprac_default);
    LoadLinksJson(state->linkSets);

    // Show the window
    ShowWindow(hwnd, nCmdShow);
    RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE);
    UpdateWindow(hwnd);

    // Main loop
    MSG msg = {};
    while (msg.message != WM_QUIT) {
        if (PeekMessageW(&msg, NULL, 0U, 0U, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }
    SaveLauncherSettings(&state->settings);
    SaveGamesJson();
    SaveLinksJson(state->linkSets);
    SaveSettings();

    delete state;

    return 0;
}

#ifndef USER_DEFAULT_SCREEN_DPI
#define USER_DEFAULT_SCREEN_DPI 96
#endif
#ifndef WM_DPICHANGED
#define WM_DPICHANGED 0x02E0
#endif

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (Gui::ImplWin32WndProcHandlerW(hWnd, msg, wParam, lParam))
        return true;

    LauncherState* state = nullptr;
    if (msg == WM_NCCREATE) {
        auto* create = (CREATESTRUCT*)lParam;
        state = (LauncherState*)create->lpCreateParams;
        SetWindowLongPtrW(hWnd, GWLP_USERDATA, (LONG_PTR)state);
    }
    else {
        state = (LauncherState*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
    }

    switch (msg) {
    case WM_NCPAINT:
        return 0;
    case WM_NCACTIVATE:
        return TRUE;
    case WM_NCCALCSIZE:
        if (wParam) {
            if (IsZoomed(hWnd)) {
                NCCALCSIZE_PARAMS* params = (NCCALCSIZE_PARAMS*)lParam;

                HMONITOR monitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
                MONITORINFO mi = { 
                    .cbSize = sizeof(mi)
                };
                GetMonitorInfoW(monitor, &mi);

                params->rgrc[0] = mi.rcWork;
            }
            return 0;
        }
        break;
    case WM_NCHITTEST: {
        // Let DefWindowProc handle any residual non-client hits first
        LRESULT hit = DefWindowProcW(hWnd, msg, wParam, lParam);
        if (hit == HTLEFT || hit == HTRIGHT || hit == HTTOP || hit == HTBOTTOM || hit == HTTOPLEFT || hit == HTTOPRIGHT || hit == HTBOTTOMLEFT || hit == HTBOTTOMRIGHT) {
            return hit;
        }
        POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
        ScreenToClient(hWnd, &pt);

        RECT rc;
        GetClientRect(hWnd, &rc);

        int border = 4; // resize-grip thickness in pixels
        int titleH = (int)TITLE_BAR_HEIGHT();
        if (!state->g_IsOverTitleBarButton && !IsZoomed(hWnd)) {
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

        if (pt.y < titleH && !state->g_IsOverTitleBarButton) {
            // Enables drag and double-click maximize.
            // Don't even think about handling window movement yourself
            // through ImGui itself, it will be an epic disaster.
            return HTCAPTION; 
        }
        return HTCLIENT;    
    }
    case WM_GETMINMAXINFO: {
        LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;
        lpMMI->ptMinTrackSize.x = 640;
        lpMMI->ptMinTrackSize.y = 480;
        break;
    }
    case WM_SIZE:
        if (Gui::ImplDX9GetDevice() != NULL && wParam != SIZE_MINIMIZED) {
            g_d3dpp.BackBufferWidth = LOWORD(lParam);
            g_d3dpp.BackBufferHeight = HIWORD(lParam);
            ImGui::GetIO().DisplaySize = {
                (float)g_d3dpp.BackBufferWidth,
                (float)g_d3dpp.BackBufferHeight
            };
            state->g_IsUITextureIDValid = false;
            ResetDevice();
        }
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;
    case WM_PAINT: {
        PAINTSTRUCT ps;
        BeginPaint(hWnd, &ps);
        EndPaint(hWnd, &ps);
        UiUpdate(hWnd, state);
        RedrawWindow(hWnd, NULL, NULL, RDW_INTERNALPAINT | RDW_INVALIDATE);
        return 0;
    }
    case WM_DPICHANGED: {
        RECT* rect = (RECT*)lParam;
        IM_ASSERT(LOWORD(wParam) == HIWORD(wParam));
        SetWindowPos(hWnd, NULL, rect->left, rect->top, rect->right - rect->left, rect->bottom - rect->top, SWP_NOZORDER);
        state->g_IsUITextureIDValid = false;
        UpdateUIScaling((float)LOWORD(wParam) / (float)USER_DEFAULT_SCREEN_DPI);
        return 0;
    }
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcW(hWnd, msg, wParam, lParam);
}

void ResetDevice() {
    Gui::ImplDX9InvalidateDeviceObjects();
    HRESULT hr = Gui::ImplDX9GetDevice()->Reset(&g_d3dpp);
    if (hr == D3DERR_INVALIDCALL)
        IM_ASSERT(0);
    Gui::ImplDX9CreateDeviceObjects();
}

bool UpdateUIScaling(float scale) {
    Gui::ImplDX9InvalidateDeviceObjects();

    // Setup Dear ImGui style
    ImGuiStyle& style = ImGui::GetStyle();
    ImGuiStyle styleold = style; // Backup colors
    style = ImGuiStyle(); // IMPORTANT: ScaleAllSizes will change the original size, so we should reset all style config
    ImGui::GetStyle().ScaleAllSizes(scale);
    style.WindowBorderSize = 0;
    memcpy(style.Colors, styleold.Colors, sizeof(style.Colors)); // Restore colors

    Gui::LocaleCreateMergeFont(20.0f * scale);
    g_Scale = scale;

    return Gui::ImplDX9CreateDeviceObjects();
}
}