#include "thprac_games.h"
#include "thprac_launcher_cfg.h"
#include "thprac_launcher_main.h"
#include "thprac_licence.h"
#include "thprac_load_exe.h"
#include "thprac_gui_impl_dx8.h"
#include "thprac_gui_impl_dx9.h"
#include "thprac_utils.h"
#include "thprac_hook.h"
#include <dinput.h>

#include "thprac_games_dx8.h"
#include "thprac_games_dx9.h"
#include "thprac_games_hooks.h"

#include <metrohash128.h>
#include <dinput.h>

#include "utils/wininternal.h"
#include <array>

#include <queue>
#include <format>
#include <vector>

#include <dwmapi.h>
#include "thprac_launcher_tools.h"
#include <ShlObj.h>

#pragma comment(lib, "dwmapi.lib")

namespace THPrac {
#pragma region Gui Wrapper

int g_gameGuiImpl = -1;
DWORD* g_gameGuiDevice = nullptr;
DWORD* g_gameGuiHwnd = nullptr;
HIMC g_gameIMCCtx = 0;

FastRetryOpt g_fast_re_opt;
InputOpt g_input_opt;


struct CursorOpt
{
    ImTextureID customCursor;
    ImVec2 textureSize;
    bool forceRenderCursor = false;
    bool alwaysRenderCursor = false;
}g_cursor_opt;

bool g_disable_max_btn = true;

bool g_record_key_aps = false;
AdvancedGameOptions g_adv_igi_options;


void GameUpdateInner(int gamever)
{
    if (g_fast_re_opt.fast_retry_count_down)
        g_fast_re_opt.fast_retry_count_down--;
}

void GameUpdateOuter(ImDrawList* p, int ver)
{
    if (g_input_opt.enable_auto_shoot && g_input_opt.is_auto_shooting) {
        ImGuiIO& io = ImGui::GetIO();
        float sz = 32.0f * io.DisplaySize.x / 1280.0f;
        p->AddRectFilled({ 0, 0 }, { sz, sz }, 0xFFFFDDDD); // pink
        p->PushClipRect({ 0, 0 }, { sz, sz });
        auto textSz = ImGui::CalcTextSize("A");
        p->AddText({ sz * 0.5f - textSz.x * 0.5f, sz * 0.5f - textSz.y * 0.5f }, 0xFFCC2222, "A");
        p->PopClipRect();
    }
}

void FastRetry(int thprac_mode)
{
    if (thprac_mode && g_fast_re_opt.enable_fast_retry) {
        g_fast_re_opt.fast_retry_count_down = g_fast_re_opt.fast_retry_cout_down_max;
    }
}

LRESULT CALLBACK GameExternWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg) {
    default:
        break;
    case WM_ACTIVATEAPP:
    case WM_ACTIVATE:
        ClearInputData(LOWORD(wParam) == WA_INACTIVE);
        break;
    case WM_INPUT:
        GetRawInput(lParam);
        break;
    }
    return 0; // return 0: pass to other proc
}


HANDLE thcrap_dll;
HANDLE thcrap_tsa_dll;

#pragma endregion

ImTextureID ReadImage(DWORD dxVer, DWORD device, LPCSTR fileName, LPCSTR srcData, size_t srcSz)
{
    if (dxVer == 8)
        return ReadImage8(device, fileName, srcData, srcSz);
    else
        return ReadImage9(device, fileName, srcData, srcSz);
}

void SetDpadHook(uintptr_t addr, size_t instr_len)
{
    // static constinit HookCtx dpad_hook = {
    //     .callback = IDirectInputDevice8_GetDeviceState_VEHHook,
    //     .data = PatchData()
    // };
    //
    // dpad_hook.addr = addr;
    // dpad_hook.data.hook.instr_len = static_cast<uint8_t>(instr_len);
    // dpad_hook.Setup();
    // dpad_hook.Enable();
}

void GameGuiInit(game_gui_impl impl, int device, int hwnd_addr,
    Gui::ingame_input_gen_t input_gen, int reg1, int reg2, int reg3,
    int wnd_size_flag, float x, float y)
{
    //MH_Initialize();
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
    // Set Hotkeys
    GuiLauncherHotkeyInit();

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

    ::ImGui::StyleColorsDark();

    LauncherSettingGet("force_render_cursor", g_cursor_opt.forceRenderCursor);
    g_cursor_opt.alwaysRenderCursor = false;
    if (g_cursor_opt.forceRenderCursor) {
        LauncherSettingGet("always_render_cursor", g_cursor_opt.alwaysRenderCursor);
        g_cursor_opt.customCursor = ReadImage(impl==IMPL_WIN32_DX8?8:9, *(DWORD*)g_gameGuiDevice, "cursor.png", NULL, 0);
        if (!g_cursor_opt.customCursor){
            char appDataPath[MAX_PATH];
            std::string path;
            if (SHGetFolderPathA(nullptr, CSIDL_APPDATA, nullptr, SHGFP_TYPE_CURRENT, appDataPath) == S_OK) {
                path = appDataPath;
                path += "\\thprac\\cursor.png";
                g_cursor_opt.customCursor = ReadImage(impl == IMPL_WIN32_DX8 ? 8 : 9, *(DWORD*)g_gameGuiDevice, path.c_str(), NULL, 0);
            }
        }
        if (g_cursor_opt.customCursor){
            g_cursor_opt.textureSize = (impl == IMPL_WIN32_DX8) ? GetImageInfo8(g_cursor_opt.customCursor) : GetImageInfo9(g_cursor_opt.customCursor);
        } else {
            g_cursor_opt.textureSize = { 32.0f, 32.0f };
        }

    }

    if (g_cursor_opt.forceRenderCursor && g_cursor_opt.alwaysRenderCursor) {
        io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
        ShowCursor(FALSE);
    }

    g_adv_igi_options.keyboard_style.separated = true;
    g_adv_igi_options.keyboard_style.border_color_press = 0xFFFFFFFF;
    g_adv_igi_options.keyboard_style.border_color_release = 0xFFFFFFFF;
    g_adv_igi_options.keyboard_style.fill_color_press = 0xFFFF4444;
    g_adv_igi_options.keyboard_style.fill_color_release = 0xFFFFCCCC;
    g_adv_igi_options.keyboard_style.text_color_press = 0xFFFFFFFF;
    g_adv_igi_options.keyboard_style.text_color_release = 0xFFFFFFFF;
    g_adv_igi_options.keyboard_style.type = 2;
    g_adv_igi_options.keyboard_style.padding = { 0.0, 0.0 };

    if (LauncherCfgInit(true)) {
        if (!Gui::ImplWin32CheckFullScreen())
        {
            bool resizable_window = false, change_window_when_open = false,init_window_pos=true;
            LauncherSettingGet("disableMax_btn", g_disable_max_btn);
            LauncherSettingGet("resizable_window", resizable_window);
            LauncherSettingGet("change_window_size_when_open", change_window_when_open);
            LauncherSettingGet("init_window_pos", init_window_pos);

            if (g_disable_max_btn || resizable_window)
            {
                auto longPtr = GetWindowLongW(*(HWND*)hwnd_addr, GWL_STYLE);
                if (resizable_window)
                    longPtr |= WS_SIZEBOX;
                if (g_disable_max_btn)
                    longPtr = longPtr & (~WS_MAXIMIZEBOX);
                SetWindowLongW(*(HWND*)hwnd_addr, GWL_STYLE, longPtr);
                if (resizable_window)
                {
                    RECT wndRect;
                    GetClientRect(*(HWND*)hwnd_addr, &wndRect);
                    auto frameSize = GetSystemMetrics(SM_CXSIZEFRAME) * 2;
                    auto captionSize = GetSystemMetrics(SM_CYCAPTION);
                    SetWindowPos(*(HWND*)hwnd_addr, HWND_NOTOPMOST,
                        0, 0, wndRect.right + frameSize, wndRect.bottom + frameSize + captionSize,
                        SWP_NOMOVE | SWP_NOZORDER | SWP_FRAMECHANGED);
                }
            }
            if (init_window_pos) {
                RECT wndRect;
                GetWindowRect(*(HWND*)hwnd_addr, &wndRect);
                if (wndRect.left < 0 || wndRect.top < 0 || (wndRect.bottom + wndRect.top) / 2 >= GetSystemMetrics(SM_CYSCREEN) || (wndRect.right + wndRect.left) / 2 >= GetSystemMetrics(SM_CXSCREEN))
                    SetWindowPos(*(HWND*)hwnd_addr, HWND_NOTOPMOST, 0, 0, wndRect.right - wndRect.left, wndRect.bottom - wndRect.top, SWP_NOZORDER | SWP_FRAMECHANGED);
            }
            if (change_window_when_open) {
                std::array<int, 2> windowsz = { 0, 0 };
                if (LauncherSettingGet("changed_window_size", windowsz) && windowsz[0] > 0 && windowsz[1] > 0) {
                        RECT wndRect;
                        RECT clientRect;
                        GetClientRect(*(HWND*)hwnd_addr, &clientRect);
                        GetWindowRect(*(HWND*)hwnd_addr, &wndRect);
                        int szx = (wndRect.right - wndRect.left)-(clientRect.right-clientRect.left);
                        int szy = (wndRect.bottom - wndRect.top) - (clientRect.bottom - clientRect.top);
                        auto frameSize = GetSystemMetrics(SM_CXSIZEFRAME) * 2;
                        auto captionSize = GetSystemMetrics(SM_CYCAPTION);
                        SetWindowPos(*(HWND*)hwnd_addr, HWND_NOTOPMOST,
                            0, 0, windowsz[0] + szx, windowsz[1] + szy,
                            SWP_NOMOVE | SWP_NOZORDER | SWP_FRAMECHANGED);
                }
            }
           
        }

        LauncherSettingGet("pauseBGM_06", g_adv_igi_options.th06_pauseBGM);
        LauncherSettingGet("autoInputName_06", g_adv_igi_options.th06_autoname);
        std::string name06 = "";
        LauncherSettingGet("autoName_06", name06);
        if (name06.size() >= 1 && name06.size() <= 9) {
            strcpy_s(g_adv_igi_options.th06_autoname_name, name06.c_str());
            static std::string allowed = ".,:;~@+-/*=%(){}[]<>#!?'\"$ ";
            for (int i = 0; i < name06.size(); i++)
            {
                auto ch = g_adv_igi_options.th06_autoname_name[i];
                if (ch == '\0')
                    break;
                if ((ch >= '0' && ch <= '9') || (ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <='z'))
                    continue;
                bool is_allowed = false;
                for (auto allowdch : allowed)
                {
                    if (allowdch == ch) {
                        is_allowed = true;
                        break;
                    }
                }
                if (is_allowed)
                    continue;
                g_adv_igi_options.th06_autoname_name[i] = '?';
            }
        }

        LauncherSettingGet("always_save_score_hist_07", g_adv_igi_options.th07_save_score_always);

        LauncherSettingGet("forceLS_08", g_adv_igi_options.th08_forceLS);
        LauncherSettingGet("th10_ud_Replay", g_adv_igi_options.th10_ud_Replay);
        LauncherSettingGet("chromatic_UFO_info", g_adv_igi_options.th12_chromatic_ufo);

        LauncherSettingGet("th18_force_card", g_adv_igi_options.th18_force_card);
        LauncherSettingGet("th18_card_st1", g_adv_igi_options.th18_cards[0]);
        LauncherSettingGet("th18_card_st2", g_adv_igi_options.th18_cards[1]);
        LauncherSettingGet("th18_card_st3", g_adv_igi_options.th18_cards[2]);
        LauncherSettingGet("th18_card_st4", g_adv_igi_options.th18_cards[3]);
        LauncherSettingGet("th18_card_st5", g_adv_igi_options.th18_cards[4]);
        LauncherSettingGet("th18_card_st7", g_adv_igi_options.th18_cards[5]);

        LauncherSettingGet("auto_disable_master", g_adv_igi_options.disable_master_autoly);
        LauncherSettingGet("auto_lock_timer", g_adv_igi_options.enable_lock_timer_autoly);
        
        LauncherSettingGet("auto_map_inf_life_to_no_continue", g_adv_igi_options.map_inf_life_to_no_continue);
        LauncherSettingGet("auto_th06_bg_fix", g_adv_igi_options.th06_bg_fix);
        LauncherSettingGet("auto_th06_fix_seed", g_adv_igi_options.th06_fix_seed);
        LauncherSettingGet("th06_seed", g_adv_igi_options.th06_seed);
        g_adv_igi_options.th06_seed = std::clamp(g_adv_igi_options.th06_seed, 0, 65535);

        LauncherSettingGet("auto_th06_rep_marker", g_adv_igi_options.th06_showRepMarker);
        LauncherSettingGet("auto_th06_show_rank", g_adv_igi_options.th06_showRank);
        LauncherSettingGet("auto_th06_disable_rank_drop", g_adv_igi_options.th06_disable_drop_rank);
        LauncherSettingGet("auto_th06_show_hitbox", g_adv_igi_options.th06_show_hitbox);
        LauncherSettingGet("auto_th11_show_hint", g_adv_igi_options.th11_showHint);
        LauncherSettingGet("auto_th13_show_hits", g_adv_igi_options.th13_showHits);
        LauncherSettingGet("auto_th13_show_hitbar", g_adv_igi_options.th13_showHitBar);
        LauncherSettingGet("auto_th14_show_bonus", g_adv_igi_options.th14_showBonus);
        LauncherSettingGet("auto_th14_show_item_cnt", g_adv_igi_options.th14_showItemsCount);
        LauncherSettingGet("auto_th14_show_drop_bar", g_adv_igi_options.th14_showDropBar);
        LauncherSettingGet("auto_th14_laser_rep_repair", g_adv_igi_options.th14_laserRepRepair);
        LauncherSettingGet("auto_th15_show_rate", g_adv_igi_options.th15_showShootingDownRate);

        LauncherSettingGet("auto_keyboard_monitor", g_adv_igi_options.show_keyboard_monitor);

        LauncherSettingGet("auto_th20_PIV_overflow_fix", g_adv_igi_options.th20_piv_overflow_fix);
        LauncherSettingGet("auto_th20_PIV_uncap", g_adv_igi_options.th20_piv_uncap);
        LauncherSettingGet("auto_th20_score_uncap", g_adv_igi_options.th20_score_uncap);
        LauncherSettingGet("auto_th20_fake_unlock_stones", g_adv_igi_options.th20_fake_unlock_stone);
        LauncherSettingGet("auto_th20_fix_bullet_hitbox", g_adv_igi_options.th20_fix_bullet_hitbox);
        LauncherSettingGet("auto_th20_decrease_graze_effect", g_adv_igi_options.th20_decrease_graze_effect);

        if (!LauncherSettingGet("kb_separated",             g_adv_igi_options.keyboard_style.separated))                g_adv_igi_options.keyboard_style.separated = true;
        if (!LauncherSettingGet("kb_aps",                   g_adv_igi_options.keyboard_style.show_aps))                 g_adv_igi_options.keyboard_style.show_aps = true;
        if (!LauncherSettingGet("kb_border_color_press",    g_adv_igi_options.keyboard_style.border_color_press))       g_adv_igi_options.keyboard_style.border_color_press = 0xFFFFFFFF;
        if (!LauncherSettingGet("kb_border_color_release",  g_adv_igi_options.keyboard_style.border_color_release))     g_adv_igi_options.keyboard_style.border_color_release = 0xFFFFFFFF;
        if (!LauncherSettingGet("kb_fill_color_press",      g_adv_igi_options.keyboard_style.fill_color_press))         g_adv_igi_options.keyboard_style.fill_color_press = 0xFFFF4444;
        if (!LauncherSettingGet("kb_fill_color_release",    g_adv_igi_options.keyboard_style.fill_color_release))       g_adv_igi_options.keyboard_style.fill_color_release = 0xFFFFCCCC;
        if (!LauncherSettingGet("kb_text_color_press",      g_adv_igi_options.keyboard_style.text_color_press))         g_adv_igi_options.keyboard_style.text_color_press = 0xFFFFFFFF;
        if (!LauncherSettingGet("kb_text_color_release",    g_adv_igi_options.keyboard_style.text_color_release))       g_adv_igi_options.keyboard_style.text_color_release = 0xFFFFFFFF;
        if (!LauncherSettingGet("kb_text_color_style",      g_adv_igi_options.keyboard_style.type))                     g_adv_igi_options.keyboard_style.type = 2;
        float kb_padding = 0.05f;
        LauncherSettingGet("kb_padding", kb_padding);
        g_adv_igi_options.keyboard_style.padding = { kb_padding, kb_padding };
        g_adv_igi_options.keyboard_style.size = { 34.0f, 34.0f };
        LauncherSettingGet("kb_type", g_adv_igi_options.keyboard_style.type);

        bool use_custom_font = false, use_correct_ja_font = false;
        int custom_font_idx = 0;
        LauncherSettingGet("use_custom_font", use_custom_font);
        LauncherSettingGet("use_correct_ja_fonts", use_correct_ja_font);
        if (use_custom_font) {
            LauncherSettingGet("custom_font", custom_font_idx);
        }
        HookCreateFont(use_correct_ja_font, use_custom_font, custom_font_idx);

        int theme;
        if (LauncherSettingGet("theme", theme)) {
            const char* userThemeName;
            if (LauncherSettingGet("theme_user", userThemeName))
                SetTheme(theme, utf8_to_utf16(userThemeName).c_str());
            else
                SetTheme(theme);
        } else
            ImGui::StyleColorsDark();

        bool is_hook_bgm = false, no_change_pitch = false;
        LauncherSettingGet("fast_BGM_when_spdup", is_hook_bgm);
        LauncherSettingGet("fast_BGM_when_spdup_no_pitch", no_change_pitch);
        HookBGMSpeed(is_hook_bgm, no_change_pitch);

        bool enable_present_hook = false;
        if (LauncherSettingGet("enable_present_hook", enable_present_hook) && enable_present_hook) {
            if (impl == IMPL_WIN32_DX8)
            {
                HookDx8(device);
            }else {
                HookDx9(device);
            }
        }

        if (g_input_opt.g_enable_keyhook){
            InitInput();
            if (enable_present_hook) {
                bool test_input_latency = false;
                LauncherSettingGet("test_input_latency", test_input_latency);
                EnableInputLatencyTest(test_input_latency);
            }
        }

        bool fix_esc_lag = false;
        if (impl == IMPL_WIN32_DX9 && LauncherSettingGet("fixEscLag", fix_esc_lag) && fix_esc_lag) {
            ESC_Fix();
        }
    }else{
        
    }
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

bool RenderCustomCursor()
{
    if (g_cursor_opt.customCursor == 0)
        return false;
   
    auto& io = ImGui::GetIO();
    io.MouseDrawCursor = false;

    float cursorSize = g_cursor_opt.textureSize.y/128.0f*32.0f;
    float eps = 0.5f / g_cursor_opt.textureSize.y;
    std::pair<ImVec2,ImVec2> uvs[8] = {
        { { 0.00f + eps, 0.0f + eps }, { 0.25f,       0.5f } },
        { { 0.25f,       0.0f + eps }, { 0.50f,       0.5f } },
        { { 0.50f,       0.0f + eps }, { 0.75f,       0.5f } },
        { { 0.75f,       0.0f + eps }, { 1.00f - eps, 0.5f } },
        { { 0 + eps,     0.5f },       { 0.25f,       1.0f - eps } },
        { { 0.25f,       0.5f },       { 0.50f,       1.0f - eps} },
        { { 0.50f,       0.5f },       { 0.75f,       1.0f - eps} },
        { { 0.75f,       0.5f },       { 1.00f,       1.0f - eps} },
    };
    //ImGuiMouseCursor_NotAllowed is not included
    ImGuiMouseCursor cursor = ImGui::GetMouseCursor();
    if (cursor >= 8 || cursor < 0) {
        return false;
    }
    ImDrawList* foreground = ImGui::GetForegroundDrawList();
    foreground->AddImage(g_cursor_opt.customCursor, io.MousePos, ImVec2(io.MousePos.x + cursorSize, io.MousePos.y + cursorSize), uvs[cursor].first, uvs[cursor].second);
    return true;
}

void GameGuiEnd(bool draw_cursor)
{
    if (GameGuiProgress != 1)
        return;
    // try to re-hook wnd proc
    // now it's no need since win32 hook method is changed
    
    // Draw cursor if needed
    if ((draw_cursor && (g_cursor_opt.forceRenderCursor || Gui::ImplWin32CheckFullScreen())) || g_cursor_opt.alwaysRenderCursor) {
        if (!RenderCustomCursor()) {
            auto& io = ::ImGui::GetIO();
            io.MouseDrawCursor = true;
        }
    }

    // Locale Change
    if (!ImGui::IsAnyItemActive()) {
        if (!g_input_opt.disable_locale_change_hotkey) {
            if (Gui::GetChordPressedDuration(Gui::GetLanguageChord()) > 0) {
                if (Gui::KeyboardInputUpdate('1') == 1) {
                    Gui::LocaleSet(Gui::LOCALE_JA_JP);
                } else if (Gui::KeyboardInputUpdate('2') == 1) {
                    Gui::LocaleSet(Gui::LOCALE_ZH_CN);
                } else if (Gui::KeyboardInputUpdate('3') == 1) {
                    Gui::LocaleSet(Gui::LOCALE_EN_US);
                }
            }
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

HRESULT (WINAPI *real_MessageBoxA)(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType);
HRESULT WINAPI MessageBoxA_Changed(HWND hWnd,LPCSTR lpText,LPCSTR lpCaption,UINT uType)
{
    auto a = mb_to_utf16(lpText, 932);
    auto b = mb_to_utf16(lpCaption, 932);
    return MessageBoxW(hWnd, a.c_str(), b.c_str(), uType);
}

void InitHook(int ver,void* addr1, void* addr2)
{
    {
        if (addr2 != nullptr) {
            TryKeepUpRefreshRate(addr1, addr2);
        } else if (addr1 != nullptr) {
            TryKeepUpRefreshRate(addr1);
        }
    }
    static bool is_inited = false;
    if (is_inited)
        return;
    is_inited = true;
    if (LauncherCfgInit(true)) {
        bool msg_box_a2w = false;
        LauncherSettingGet("tryInnerPatch", g_adv_igi_options.try_inner_patch);
        
        if (LauncherSettingGet("msg_box_a2w", msg_box_a2w) && msg_box_a2w)
        {
            const WCHAR* name_vp_module = NULL;
            switch (ver)
            {
            case -10: name_vp_module = L"vpatch_alcostg.dll";break;
            case 6: name_vp_module = L"vpatch_th06.dll";break;
            case 7: name_vp_module = L"vpatch_th07.dll";break;
            case 8: name_vp_module = L"vpatch_th08.dll";break;
            case 9: name_vp_module = L"vpatch_th09.dll";break;
            case 95: name_vp_module = L"vpatch_th095.dll";break;
            case 10: name_vp_module = L"vpatch_th10.dll";break;
            case 11: name_vp_module = L"vpatch_th11.dll";break;
            case 12: name_vp_module = L"vpatch_th12.dll";break;
            case 125: name_vp_module = L"vpatch_th125.dll";break;
            case 128: name_vp_module = L"vpatch_th128.dll";break;
            case 13: name_vp_module = L"vpatch_th13.dll";break;
            case 14: name_vp_module = L"vpatch_th14.dll";break;
            case 15: name_vp_module = L"vpatch_th15.dll";break;
            default:
                break;
            }
            real_MessageBoxA = (decltype(real_MessageBoxA))GetProcAddress(GetModuleHandleW(L"user32.dll"), "MessageBoxA");
            HookIAT(GetModuleHandleW(NULL), "user32.dll", "MessageBoxA", MessageBoxA_Changed, nullptr);
            if (name_vp_module != NULL)
            {
                auto module_vp = GetModuleHandleW(name_vp_module);
                if (module_vp)
                    HookIAT(module_vp, "user32.dll", "MessageBoxA", MessageBoxA_Changed, nullptr);
            }
            if (ver == 6)
            {
                auto module_vp = GetModuleHandleW(L"vpatch_th06_unicode.dll");
                if (module_vp)
                    HookIAT(module_vp, "user32.dll", "MessageBoxA", MessageBoxA_Changed, nullptr);
            }
        }


        LauncherSettingGet("auto_disable_C", g_input_opt.disable_Ckey_at_same_time);
        LauncherSettingGet("disable_locale_change_hotkey", g_input_opt.disable_locale_change_hotkey);
        LauncherSettingGet("disableWinDinput8", g_input_opt.disable_win_key);

        LauncherSettingGet("keyboard_SOCDv2", (int&)g_input_opt.g_socd_setting);
        LauncherSettingGet("keyboard_API", (int&)g_input_opt.g_keyboardAPI);

        bool disable_f10 = false;
        if (LauncherSettingGet("disable_F10_11_13", disable_f10) && disable_f10) {
            if (ver == 11 // 11
                || ver == 12 // 12
                || ver == 125 // 125
                || ver == 128 // 128
                || ver == 13 // 13
            ) {
                g_input_opt.disable_f10_11_13 = true;
            } else {
                g_input_opt.disable_f10_11_13 = false;
            }

        } else {
            g_input_opt.disable_f10_11_13 = false;
        }
        LauncherSettingGet_KeyBind();
        LauncherSettingGet("disableJoy", g_input_opt.g_disable_joy);
        // if (LauncherSettingGet("disableJoy", g_disable_joy) && g_disable_joy)
       
        HookJoyInput();

        g_fast_re_opt.enable_fast_retry = false;
        g_input_opt.enable_auto_shoot = false;
        g_input_opt.shoot_key_DIK = -1;
        if (LauncherSettingGet("enable_keyboard_hook", g_input_opt.g_enable_keyhook) && g_input_opt.g_enable_keyhook) { // hook keyboard to enable SOCD and X-disable
            LPVOID pTarget;
            HookKeyboardInput();
            
            if (g_input_opt.g_keyboardAPI == InputOpt::KeyboardAPI::Force_RawInput || g_input_opt.g_keyboardAPI == InputOpt::KeyboardAPI::Force_dinput8KeyAPI) {
                LauncherSettingGet("auto_fast_retry", g_fast_re_opt.enable_fast_retry);
                LauncherSettingGet("auto_auto_shoot", g_input_opt.enable_auto_shoot);
                g_input_opt.is_th128 = (ver == 128);
                LauncherSettingGet("auto_shoot_key", g_input_opt.shoot_key_DIK);
                bool has_this_key = false;
                for (int i = 0; i < ARRAYSIZE(keyBindDefine); i++) {
                    if (keyBindDefine[i].dik == g_input_opt.shoot_key_DIK) {
                        has_this_key = true;
                        break;
                    }
                }
                if (!has_this_key)
                    g_input_opt.shoot_key_DIK = -1;
            }
            if (g_input_opt.g_keyboardAPI == InputOpt::KeyboardAPI::Force_dinput8KeyAPI) {
                // keyboard hook + force dinput8
                LauncherSettingGet("use_get_device_data", g_input_opt.use_get_device_data);
            }
        }
    }
}

#pragma endregion

#pragma region Advanced Options Menu

void MsgBox(UINT type, const char* title, const char* msg, const char* msg2, HWND owner)
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
    MessageBoxW(owner, _msg, _title, type);
}

void OILPInit(adv_opt_ctx& ctx)
{
    ctx.fps_status = 3;
    ctx.oilp_set_game_fps = (adv_opt_ctx::oilp_set_fps_t*)GetProcAddress((HMODULE)ctx.vpatch_base, "oilp_set_game_fps");
    auto oilp_get_game_fps = (int(__stdcall*)())GetProcAddress((HMODULE)ctx.vpatch_base, "oilp_get_game_fps");
    if (oilp_get_game_fps)
        ctx.fps = oilp_get_game_fps();
    else
        ctx.fps = 60;
    ctx.oilp_set_replay_skip_fps = (adv_opt_ctx::oilp_set_fps_t*)GetProcAddress((HMODULE)ctx.vpatch_base, "oilp_set_replay_skip_fps");
    ctx.oilp_set_replay_slow_fps = (adv_opt_ctx::oilp_set_fps_t*)GetProcAddress((HMODULE)ctx.vpatch_base, "oilp_set_replay_slow_fps");
    auto oilp_get_replay_skip_fps = (int(__stdcall*)())GetProcAddress((HMODULE)ctx.vpatch_base, "oilp_get_replay_skip_fps");
    auto oilp_get_replay_slow_fps = (int(__stdcall*)())GetProcAddress((HMODULE)ctx.vpatch_base, "oilp_get_replay_slow_fps");
    if (oilp_get_replay_skip_fps)
        ctx.fps_replay_fast = oilp_get_replay_skip_fps();
    if (oilp_get_replay_slow_fps)
        ctx.fps_replay_slow = oilp_get_replay_slow_fps();
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

void CustomMarker(const char* text, const char* desc)
{
    ImGui::TextDisabled(text);
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

    

    if (clickedApply)  {
        ChangeBGMSpeed(ctx.fps / 60.0f, -1.0f);
    }
    BGMPitchChanger();

    return clickedApply;
}

void DisableKeyOpt()
{
    if (g_input_opt.g_enable_keyhook) {
        ImGui::Checkbox(S(TH_ADV_DISABLE_X_KEY), &g_input_opt.disable_xkey);
        ImGui::SameLine();
        HelpMarker(S(TH_ADV_DISABLE_X_KEY_DESC));
        ImGui::SameLine();
        ImGui::Checkbox(S(TH_ADV_DISABLE_SHIFT_KEY), &g_input_opt.disable_shiftkey);
        ImGui::SameLine();
        HelpMarker(S(TH_ADV_DISABLE_SHIFT_KEY_DESC));
        ImGui::SameLine();
        ImGui::Checkbox(S(TH_ADV_DISABLE_Z_KEY), &g_input_opt.disable_zkey);
        ImGui::SameLine();
        HelpMarker(S(TH_ADV_DISABLE_Z_KEY_DESC));
        ImGui::SameLine();
        ImGui::Checkbox(S(TH_ADV_DISABLE_C_KEY_SAMETIME), &g_input_opt.disable_Ckey_at_same_time);

        static bool test_opt = false;
        ImGui::SameLine();
        ImGui::Checkbox("Hook Opt.(test)", &test_opt);

        if (g_input_opt.g_keyboardAPI == InputOpt::KeyboardAPI::Force_dinput8KeyAPI || g_input_opt.g_keyboardAPI == InputOpt::KeyboardAPI::Force_RawInput)
        {
            ImGui::Checkbox(S(THPRAC_FAST_RETRY), &g_fast_re_opt.enable_fast_retry);
            ImGui::SameLine();
            HelpMarker(S(THPRAC_FAST_RETRY_DESC2));
            if (g_input_opt.shoot_key_DIK != -1) {
                ImGui::SameLine();
                ImGui::Checkbox(S(THPRAC_AUTO_SHOOT), &g_input_opt.enable_auto_shoot);
            }
        }
        if (test_opt)
        {
            std::string str;
            switch (g_input_opt.g_keyboardAPI)
            {
            default: str = "unknown";break;
            case InputOpt::KeyboardAPI::Default_API:str = "default";break;
            case InputOpt::KeyboardAPI::Force_dinput8KeyAPI:str = "force dinput8"; break;
            case InputOpt::KeyboardAPI::Force_win32KeyAPI:str = "force win32";break;
            case InputOpt::KeyboardAPI::Force_RawInput:str = "force raw input";break;
            }
            ImGui::Text("current API: %s", str.c_str());
            ImGui::Text("dinput_init: %s, ri_init: %s, gamepad disable: %s", (g_input_opt.ddevice == nullptr) ? "F" : "T", (g_input_opt.is_ri_inited) ? "T" : "F", (g_input_opt.g_disable_joy) ? "T" : "F");

            if (ImGui::Button("switch to Dinput")){
                g_input_opt.g_keyboardAPI = InputOpt::KeyboardAPI::Force_dinput8KeyAPI;
                InitInput();
            }
            ImGui::SameLine();
            if (ImGui::Button("switch to RawInput")) {
                g_input_opt.g_keyboardAPI = InputOpt::KeyboardAPI::Force_RawInput;
                InitInput();
            }
            ImGui::SameLine();
            if (ImGui::Button("switch to Win32")) {
                g_input_opt.g_keyboardAPI = InputOpt::KeyboardAPI::Force_win32KeyAPI;
                InitInput();
            }
            ImGui::SameLine();
            if (ImGui::Button("switch to Default")) {
                g_input_opt.g_keyboardAPI = InputOpt::KeyboardAPI::Default_API;
                InitInput();
            }
            if (g_input_opt.g_keyboardAPI == InputOpt::KeyboardAPI::Force_dinput8KeyAPI ){
                if (ImGui::Checkbox("use GetDeviceData", &g_input_opt.use_get_device_data)){
                    SetDinput8DeviceData();
                    ClearInputData(false);
                }
            }
        }
        double cur_latency;
        if (GetLatency(&cur_latency))
        {
            static std::deque<double> times;
            times.push_back(cur_latency);
            double avg = 0;
            for (auto& i : times)
                avg += i;
            avg /= times.size();
            while (times.size() > 60)
                times.pop_front();
            ImGui::Text("InputA->Present: %lf(%lf)", avg * 1000.0, cur_latency * 1000.0);
        }

        if (ImGui::IsKeyDown(0x10)) // shift
        {
            if (g_input_opt.g_keyboardAPI == InputOpt::KeyboardAPI::Force_dinput8KeyAPI || g_input_opt.g_keyboardAPI == InputOpt::KeyboardAPI::Force_RawInput) {
                if (ImGui::IsKeyPressed('F'))
                    g_fast_re_opt.enable_fast_retry = !g_fast_re_opt.enable_fast_retry;
            }
            if (ImGui::IsKeyPressed('D'))
                g_input_opt.disable_xkey = !g_input_opt.disable_xkey;
            if (ImGui::IsKeyPressed('S'))
                g_input_opt.disable_zkey = !g_input_opt.disable_zkey;
            if (ImGui::IsKeyPressed('A'))
                g_input_opt.disable_shiftkey = !g_input_opt.disable_shiftkey;
        }
    }
    return;
}

void KeyHUDOpt()
{
    ImGui::Checkbox(S(THPRAC_KB_OPEN), &(g_adv_igi_options.show_keyboard_monitor));
    if (g_adv_igi_options.show_keyboard_monitor){
        
        if (!g_record_key_aps){
            if (ImGui::Button(S(THPRAC_KB_RECORD_START))){
                ClearKeyRecord();
                g_record_key_aps = true;
            }
        }else if (ImGui::Button(S(THPRAC_KB_RECORD_STOP)))
            g_record_key_aps = false;
        ImGui::SameLine();
        if (ImGui::Button(S(THPRAC_KB_OUTPUT))){
            SaveKeyRecorded();
        }
        auto& recorded_aps = GetKeyAPS();
        if (recorded_aps.size() >= 2) {
            ImGui::PlotLines("##APS", 
                [](void* data, int idx) -> float { 
                    std::vector<uint8_t> &recorded_aps = *(std::vector<uint8_t>*)data;
                    if (recorded_aps.size() > 600) 
                        return recorded_aps[recorded_aps.size() - 600 + idx];
                    else return  recorded_aps[idx]; }
                ,&recorded_aps, recorded_aps.size() > 600 ? 600 : recorded_aps.size(), 0, 0, FLT_MAX, FLT_MAX, { 0, ImGui::GetFrameHeight() * 3.0f });
        }
    }
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

void InfLifeOpt()
{
    ImGui::Checkbox(S(THPRAC_INFLIVES_MAP), &g_adv_igi_options.map_inf_life_to_no_continue);
    return;
}

void InGameReactionTestOpt()
{
    // this tool is used for reaction test, but also can test the game lag, since players' reaction remains almost the same when in game or out of game
    static THGuiTestReactionTest test;
    if (ImGui::CollapsingHeader(S(THPRAC_TOOLS_REACTION_TEST))) {
        test.GuiUpdate(true);
    } else {
        test.Reset();
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

                if (bytesRead == 4 && paramLength > 0 && paramLength < repSize) {
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

ReplayClearResult ReplayClearParam(const wchar_t* rep_path)
{
    DWORD repMagic = 0, bytesRead = 0;

    HANDLE repFile = CreateFileW(rep_path, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (repFile == INVALID_HANDLE_VALUE)
        return ReplayClearResult::Error;
    defer(CloseHandle(repFile));

    // read the magic
    SetFilePointer(repFile, 0, nullptr, FILE_BEGIN);
    if (!ReadFile(repFile, &repMagic, 4, &bytesRead, nullptr) || bytesRead != 4)
        return ReplayClearResult::Error;

    // find thprac param headers based off magic
    if (repMagic == 'PR6T' || repMagic == 'PR7T') {
        DWORD magic = 0, paramLength = 0;
        DWORD repSize = GetFileSize(repFile, nullptr);

        SetFilePointer(repFile, -4, nullptr, FILE_END);
        if (ReadFile(repFile, &magic, 4, &bytesRead, nullptr) && bytesRead == 4 && magic == 'CARP') {
            // read param length
            SetFilePointer(repFile, -8, nullptr, FILE_CURRENT);
            if (!ReadFile(repFile, &paramLength, 4, &bytesRead, nullptr) || bytesRead != 4)
                return ReplayClearResult::Error;

            // truncate file before thprac data
            if (repSize < paramLength + 8)
                return ReplayClearResult::Error;
            SetFilePointer(repFile, repSize - (paramLength + 8), nullptr, FILE_BEGIN);
            return SetEndOfFile(repFile) ? ReplayClearResult::Cleared : ReplayClearResult::Error;
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
                    // truncate before this user param
                    SetFilePointer(repFile, -12, nullptr, FILE_CURRENT);
                    return SetEndOfFile(repFile) ? ReplayClearResult::Cleared : ReplayClearResult::Error;
                } else {
                    SetFilePointer(repFile, userLength - 12, nullptr, FILE_CURRENT);
                }
            }
        }
    }

    return ReplayClearResult::NoParams;
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