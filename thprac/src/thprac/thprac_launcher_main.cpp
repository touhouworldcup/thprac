#include "thprac_launcher_main.h"
#include "thprac_gui_locale.h"
#include "thprac_launcher_cfg.h"
#include "thprac_launcher_games.h"
#include "thprac_launcher_links.h"
#include "thprac_launcher_tools.h"
#include "thprac_launcher_utils.h"
#include "thprac_launcher_wnd.h"
#include "thprac_utils.h"
#include <ShlObj.h>
#include <Windows.h>
#include <Shlwapi.h>
#include <cstdio>
#include <sstream>

#include <thread>
#include <numbers>


namespace THPrac {
#pragma region MutexTest
int64_t g_game_time_ns=-1;
int64_t g_game_time_ns_cur=-1;
int64_t g_game_time_delta_ns=-1;
int64_t g_game_time_toolong_SE_ns=-1;
bool g_update_game_time=true;
LARGE_INTEGER g_last_time;
LARGE_INTEGER g_freq;
float g_toolong_hour = 3.0f;
float g_toolong_SE_sec = 10.0f;
bool g_enable_too_long_SE = false;

bool IsAnyGameOpen()
{
    static const char* mutexStrA = nullptr;
    static const WCHAR* mutexStrW = nullptr;
    if (mutexStrA) {// less CPU
        auto resultA = OpenMutexA(SYNCHRONIZE, FALSE, mutexStrA);
        if (resultA) {
            CloseHandle(resultA);
            return true;
        }
    }
    if(mutexStrW){
        auto resultW = OpenMutexW(SYNCHRONIZE, FALSE, mutexStrW);
        if (resultW) {
            CloseHandle(resultW);
            return true;
        }
    }
    for (auto& gamesig : gGameDefs){
        if (gamesig.mutexStr)
        {
            auto result = OpenMutexA(SYNCHRONIZE, FALSE, gamesig.mutexStr);
            if (result) {
                mutexStrA = gamesig.mutexStr;
                CloseHandle(result);
                return true;
            }
        }else if (gamesig.mutexWStr){
            auto result = OpenMutexW(SYNCHRONIZE, FALSE, gamesig.mutexWStr);
            if (result) {
                mutexStrW = gamesig.mutexWStr;
                CloseHandle(result);
                return true;
            }
        }
    }
    mutexStrA = nullptr;
    mutexStrW = nullptr;
    return false;
}
#pragma endregion

const char* gTabToOpen = nullptr;
LauncherTrigger gLauncherTrigger = LAUNCHER_NOTHING;
void GuiLauncherMainSwitchTab(const char* tab)
{
    gTabToOpen = tab;
}
bool GuiTabItem(const char* tabText)
{
    auto flag = 0;
    if (gTabToOpen && !strcmp(gTabToOpen, tabText)) {
        flag = ImGuiTabItemFlags_SetSelected;
        gTabToOpen = nullptr;
    }
    return ImGui::BeginTabItem(tabText, 0, flag);
}
void ErrorMsgBox(th_glossary_t textRef)
{
    auto title = utf8_to_utf16(S(THPRAC_PR_ERROR));
    auto text = utf8_to_utf16(S(textRef));
    MessageBoxW(nullptr, text.c_str(), title.c_str(), MB_OK | MB_ICONERROR | MB_SYSTEMMODAL);
}

void GuiLauncherLocaleInit()
{
    if (LauncherCfgInit(true)) {
        if (!Gui::LocaleInitFromCfg()) {
            Gui::LocaleAutoSet();
        }
        LauncherCfgClose();
    }
}
void GuiLauncherMainTrigger(LauncherTrigger trigger)
{
    gLauncherTrigger = trigger;
}

void UpdateGameTime()
{
    while (g_update_game_time){
        if (g_game_time_ns_cur >= 1000000000ll * 3600 * (double)g_toolong_hour) {
            if (g_enable_too_long_SE && 
                g_game_time_toolong_SE_ns >= g_toolong_SE_sec * 1000000000ll && 
                g_game_time_toolong_SE_ns >= 5*1000000000ll &&// mintime: 5sec
                IsAnyGameOpen()
                ) {
                PlaySoundW(L"SE.wav", NULL, SND_FILENAME | SND_ASYNC);
                g_game_time_toolong_SE_ns = 0;
            }
        }
        static bool is_game_open = false;
        Sleep(16);
        if (g_game_time_delta_ns > 1000000000) { //test every second
            is_game_open = IsAnyGameOpen();
            g_game_time_delta_ns = 0;
        }
        LARGE_INTEGER cur_time;
        QueryPerformanceCounter(&cur_time);
        int64_t passed_time = (((double)(cur_time.QuadPart - g_last_time.QuadPart)) / (double)(g_freq.QuadPart)) * 1e9;
        if (is_game_open) {
            g_game_time_ns += passed_time;
            g_game_time_ns_cur += passed_time;
        }
        g_game_time_delta_ns += passed_time;
        g_game_time_toolong_SE_ns += passed_time;
        g_last_time = cur_time;
    }
    LauncherSetGameTime(g_game_time_ns);
}
int GuiLauncherMain()
{
    int localeSwitch = -1;

    auto initResult = LauncherWndInit(640, 480, 1280, 960, 960, 720);
    if (initResult <= 0) {
        if (initResult) {
            ErrorMsgBox(THPRAC_PR_ERR_LAUNCHER_INIT);
        }
        return -1;
    }
    if (!LauncherCfgInit()) {
        ErrorMsgBox(THPRAC_PR_ERR_LAUNCHER_CFG);
        return -1;
    }

    int theme;
    if (LauncherSettingGet("theme", theme)) {
        // LauncherSettingSet doesn't take int, only int&.
        // Passing 0 will call the overload with const char*
        int Sus = 0;
        const char* theme_user = nullptr;
        // LauncherSettingGet only accepts signed ints but I want to do an unsigned comparison
        if ((unsigned int)theme > 2) {
            if (LauncherSettingGet("theme_user", theme_user) && theme_user) {
                std::wstring theme_path = LauncherGetDataDir() + L"themes\\" + utf8_to_utf16(theme_user);
                if (!PathFileExistsW(theme_path.c_str())) {
                    LauncherSettingSet("theme", Sus);
                    theme = Sus;
                }
            } else {
                LauncherSettingSet("theme", Sus);
                theme = Sus;
            }
        }

        if (theme_user) {
            SetTheme(theme, utf8_to_utf16(theme_user).c_str());
        } else {
            SetTheme(theme);
        }
    }
    LauncherPeekUpd();
    auto scale = LauncherWndGetScale();

    bool enableRecordGameTime=false;
    LauncherSettingGet("recordGameTime",enableRecordGameTime);
    std::thread gametime_thread;
    if(enableRecordGameTime){ // game counter
        g_game_time_ns = LauncherGetGameTime();
        if (g_game_time_ns < 0)
            g_game_time_ns = 0;
        g_game_time_ns_cur = 0;
        g_game_time_delta_ns = 0;
        g_game_time_toolong_SE_ns = 0;
        g_update_game_time = true;
        QueryPerformanceFrequency(&g_freq);
        QueryPerformanceCounter(&g_last_time);
        gametime_thread = std::thread(UpdateGameTime);
    }



    while (LauncherWndNewFrame()) {
        LauncherSettingGet("gameTimeTooLong_Time", g_toolong_hour);
        LauncherSettingGet("gameTimeTooLong_SE", g_enable_too_long_SE);
        LauncherSettingGet("gameTimeTooLong_SE_repeat", g_toolong_SE_sec);

        static bool isOpen = true;
        static bool isMinimize = false;
        bool canMove = false;

        ImGui::SetNextWindowSizeConstraints(ImVec2(640.0f * scale, 480.0f * scale), ImVec2(1280.0f * scale, 960.0f * scale));
        ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(960.0f * scale, 720.0f * scale), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowBgAlpha(0.9f);

        std::string wndTitleText = S(THPRAC_LAUNCHER);
        if (LauncherIsChkingUpd()) {
            wndTitleText += " // Checking for updates...";
        }
        wndTitleText = LauncherIsChkingUpd() ? S(THPRAC_LAUNCHER_CHECKING_UPDATE) : S(THPRAC_LAUNCHER);
        wndTitleText += "###thprac_wnd";
        ImGui::Begin(wndTitleText.c_str(), &isOpen, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove, &isMinimize);
        if (!isOpen)
            break;
        if (isMinimize) {
            isMinimize = false;
            LauncherWndMinimize();
        }
        canMove = ImGui::IsItemHovered();

        if (ImGui::BeginTabBar("MenuTabBar")) {
            if (GuiTabItem(S(THPRAC_GAMES))) {
                ImGui::BeginChild("##games");
                LauncherGamesGuiUpd();
                ImGui::EndChild();
                ImGui::EndTabItem();
            }
            if (GuiTabItem(S(THPRAC_OTHERS))) {
                ImGui::BeginChild("##others");
                if (enableRecordGameTime)
                {
                    ImGui::Text(S(THPRAC_GAME_TIME_TOTAL));
                    ImGui::TextWrapped(GetTime_HHMMSS(g_game_time_ns > 0 ? g_game_time_ns : 0).c_str());
                    ImGui::TextWrapped(GetTime_YYMMDD_HHMMSS(g_game_time_ns > 0 ? g_game_time_ns : 0).c_str());
                    ImGui::TextWrapped("(%lld ns)", g_game_time_ns > 0 ? g_game_time_ns : 0);

                    ImGui::NewLine();
                    ImGui::Separator();

                    float r, g, b;
                    int gametimecur_hour = (g_game_time_ns_cur / (1000000000ll * 3600ll)) % 24;
                    ImGui::ColorConvertHSVtoRGB(gametimecur_hour / 24.0f, 1, 1, r, g, b);
                    ImGui::PushStyleColor(ImGuiCol_Text, { r, g, b, 1 });
                    ImGui::Text(S(THPRAC_GAME_TIME_CURRENT));
                    ImGui::PopStyleColor();

                    int gametimecur_minute = (g_game_time_ns_cur / (1000000000ll * 60ll)) % 60;
                    ImGui::ColorConvertHSVtoRGB(gametimecur_minute / 60.0f, 1, 1, r, g, b);
                    ImGui::PushStyleColor(ImGuiCol_Text, { r, g, b, 1 });
                    ImGui::TextWrapped(GetTime_HHMMSS(g_game_time_ns_cur > 0 ? g_game_time_ns_cur : 0).c_str());
                    ImGui::PopStyleColor();

                    int gametimecur_sec = (g_game_time_ns_cur / (1000000000ll)) % 60;
                    ImGui::ColorConvertHSVtoRGB(gametimecur_sec / 60.0f, 1, 1, r, g, b);
                    ImGui::PushStyleColor(ImGuiCol_Text, { r, g, b, 1 });
                    ImGui::TextWrapped(GetTime_YYMMDD_HHMMSS(g_game_time_ns_cur > 0 ? g_game_time_ns_cur : 0).c_str());
                    ImGui::PopStyleColor();

                    int gametimecur_frame = (g_game_time_ns_cur / (16666666ll)) % 60;
                    ImGui::ColorConvertHSVtoRGB(gametimecur_frame / 60.0f, 1, 1, r, g, b);
                    ImGui::PushStyleColor(ImGuiCol_Text, { r, g, b, 1 });
                    ImGui::TextWrapped("(%lld ns)", g_game_time_ns_cur);
                    ImGui::PopStyleColor();
                    ImGui::NewLine();

                    ImGui::Separator();

                    if (g_game_time_ns_cur >= 1000000000ll * 3600 * (double)g_toolong_hour)
                    {
                        static float h = 0.0f,vt=0.0f, angle=0.0f;
                        h += 0.02f;
                        if (h >= 1.0f)
                            h = 0.0f;
                        vt += 0.01f;
                        angle += 0.1f;
                        ImScaleStart();
                        ImRotateStart();
                        ImGui::NewLine();
                        ImGui::ColorConvertHSVtoRGB(h, sinf(vt)*0.3f+0.5f, 1.0f, r, g, b);
                        ImGui::PushStyleColor(ImGuiCol_Text, { r, g, b, 1 });
                        auto wndSize = ImGui::GetWindowSize();
                        auto textSz = ImGui::CalcTextSize(S(THPRAC_GAME_TIME_LONG));
                        ImGui::SetCursorPosX(wndSize.x * 0.5f - textSz.x*0.5f);
                        ImGui::TextUnformatted(S(THPRAC_GAME_TIME_LONG));
                        ImGui::PopStyleColor();
                        constexpr auto HALF_PI = static_cast<float>(std::numbers::pi / 2.0);
                        ImRotateEnd(HALF_PI - sinf(-1.23f*angle + HALF_PI/2.0f) * 0.12f);
                        ImScaleEnd(2.3f + sinf(angle) * 0.8f, 2.3f + cosf(angle) * 0.8f);
                    }
                }else{
                    ImGui::Text(S(THPRAC_ENABLE_GAMETIME_RECORD));
                }
                ImGui::EndChild();
                ImGui::EndTabItem();
            }
            if (GuiTabItem(S(THPRAC_LINKS))) {
                ImGui::BeginChild("##links");
                LauncherLinksGuiUpd();
                ImGui::EndChild();
                ImGui::EndTabItem();
            }
            if (GuiTabItem(S(THPRAC_TOOLS))) {
                ImGui::BeginChild("##tools");
                LauncherToolsGuiUpd();
                ImGui::EndChild();
                ImGui::EndTabItem();
            }
            if (GuiTabItem(S(THPRAC_SETTINGS))) {
                ImGui::BeginChild("##settings");
                LauncherCfgGuiUpd();
                ImGui::EndChild();
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }

        LauncherChkUpdPopup();

        ImVec2 wndPos = ImGui::GetWindowPos();
        ImVec2 wndSize = ImGui::GetWindowSize();
        canMove &= !ImGui::IsAnyItemHovered() && !ImGui::IsAnyItemActive();
        ImGui::End();
        LauncherWndEndFrame(wndPos, wndSize, canMove);

        switch (gLauncherTrigger) {
        case THPrac::LAUNCHER_RESTART:
            localeSwitch = Gui::LocaleGet();
            break;
        case THPrac::LAUNCHER_CLOSE:
        case THPrac::LAUNCHER_RESET:
            isOpen = false;
            break;
        case THPrac::LAUNCHER_MINIMIZE:
            LauncherWndMinimize();
            break;
        default:
            break;
        }
        if (gLauncherTrigger != LAUNCHER_RESET) {
            gLauncherTrigger = LAUNCHER_NOTHING;
        }

        if (localeSwitch != -1) {
            MSG msg;
            Gui::LocaleSet((Gui::locale_t)localeSwitch);
            LauncherWndShutdown();
            while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE)) {
            }
            LauncherWndInit(640, 480, 1280, 960, 960, 720);
            localeSwitch = -1;
        }
    }
    if (enableRecordGameTime) {
        g_update_game_time = false;
        gametime_thread.join();
    }
    

    LauncherCfgClose();
    LauncherWndShutdown();
    if (gLauncherTrigger == LAUNCHER_RESET) {
        LauncherCfgReset();
    }

    return 0;
}

}
