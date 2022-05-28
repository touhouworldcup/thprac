#include "thprac_launcher_main.h"
#include "thprac_av_evation.h"
#include "thprac_gui_locale.h"
#include "thprac_launcher_cfg.h"
#include "thprac_launcher_games.h"
#include "thprac_launcher_links.h"
#include "thprac_launcher_tools.h"
#include "thprac_launcher_utils.h"
#include "thprac_launcher_wnd.h"
#include <ShlObj.h>
#include <Windows.h>
#include <cstdio>
#include <sstream>
#include <xxh3.h>

namespace THPrac {
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
int DebugTabBar()
{
    int localeSwitch = -1;
    static char text[1024 * 16] {};
    static float f = 0.0f;
    static std::string selectable { "" };
    if (GuiTabItem("Debug")) {
        ImGui::BeginChild("##debug");
        /*
                HICON hIcon = static_cast<HICON>(::LoadImage(hInstance,
                MAKEINTRESOURCE(MYICON1),
                IMAGE_ICON,
                48, 48,    // or whatever size icon you want to load
                LR_DEFAULTCOLOR);
                */

        if (ImGui::Button("zh_CN")) {
            localeSwitch = Gui::LOCALE_ZH_CN;
        }
        if (ImGui::Button("en_US")) {
            localeSwitch = Gui::LOCALE_EN_US;
        }
        if (ImGui::Button("ja_JP")) {
            localeSwitch = Gui::LOCALE_JA_JP;
        }

        if (ImGui::Button("testDialog")) {
            LauncherUpdDialog();
        }

        if (ImGui::IsWindowResizing())
            ImGui::Text("Resizing");
        else
            ImGui::Text("Not resizing");

        auto dispSize = ImGui::GetIO().DisplaySize;
        ImGui::Text("%d, %d", (int)ImGui::GetWindowSize().x, (int)ImGui::GetWindowSize().y);
        ImGui::Text("%d, %d", (int)dispSize.x, (int)dispSize.y);

        ImGui::Selectable("TestSelectable");
        if (ImGui::IsItemHovered()) {
            if (ImGui::IsMouseDoubleClicked(0)) {
                selectable = "DOUBLE_CLICKED";
            } else if (ImGui::IsMouseClicked(0)) {
                selectable = "SINGLE_CLICKED";
            }
        }
        ImGui::Text(selectable.c_str());

        ImGui::InputTextMultiline("Test", text, IM_ARRAYSIZE(text), ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 16));

        static float percentage = 0.0f;
        static uint32_t rsaRandom = 0;
        static bool rsaStatus = false;
        if (!rsaStatus) {
            rsaStatus = true;
        }
        ImGui::SliderFloat("%", &percentage, 0.0f, 1.0f);
        if (ImGui::Button("RSA Test")) {
        }
        ImGui::Text("%08x", rsaRandom);
        if (ImGui::Button("File Select")) {
            OPENFILENAME ofn;
            // a another memory buffer to contain the file name
            wchar_t szFile[MAX_PATH];
            ZeroMemory(&ofn, sizeof(ofn));
            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = NULL;
            ofn.lpstrFile = szFile;
            ofn.lpstrFile[0] = '\0';
            ofn.nMaxFile = sizeof(szFile);
            ofn.lpstrFilter = nullptr;
            ofn.nFilterIndex = 1;
            ofn.lpstrFileTitle = NULL;
            ofn.nMaxFileTitle = 0;
            ofn.lpstrInitialDir = NULL;
            ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
            GetOpenFileName(&ofn);

            
                    HANDLE hFile = INVALID_HANDLE_VALUE;
                    DWORD fileSize = 0;
                    HANDLE hFileMap = NULL;
                    void* pFileMapView = nullptr;
                    hFile = CreateFileW(ofn.lpstrFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
                    fileSize = GetFileSize(hFile, NULL);
                    hFileMap = CreateFileMappingA(hFile, NULL, PAGE_READONLY, 0, fileSize, NULL);
                    pFileMapView = MapViewOfFile(hFileMap, FILE_MAP_READ, 0, 0, fileSize);

                    ExeSig exeSig;
                    GetExeInfo(pFileMapView, fileSize, exeSig);
                    auto xxH = XXH3_64bits(pFileMapView, fileSize);
                    
                    auto fmtStr = "%llullu\n {\n\"thXX\",\nTHXX_TITLE,\nCAT_MAIN,\n{\n%d, %d,\n{\n0x%04x, 0x%04x, 0x%04x, 0x%04x, 0x%04x,\n0x%04x, 0x%04x, 0x%04x, 0x%04x, 0x%04x\n},\n{\n0x%08x, 0x%08x,\n0x%08x, 0x%08x\n} } }";
                    sprintf_s(text, fmtStr, xxH,
                        exeSig.timeStamp, exeSig.textSize,
                        exeSig.oepCode[0], exeSig.oepCode[1], exeSig.oepCode[2], exeSig.oepCode[3], exeSig.oepCode[4],
                        exeSig.oepCode[5], exeSig.oepCode[6], exeSig.oepCode[7], exeSig.oepCode[8], exeSig.oepCode[9],
                        exeSig.metroHash[0], exeSig.metroHash[1], exeSig.metroHash[2], exeSig.metroHash[3]);
                    
                    //auto fmtStr = "{ \"thXX\",\nnullptr,\nA0000ERROR_C,\n{ 0x%08x, 0x%08x,\n0x%08x, 0x%08x} },";
                    //sprintf_s(text, fmtStr, exeSig.metroHash[0], exeSig.metroHash[1], exeSig.metroHash[2], exeSig.metroHash[3]);

                    if (pFileMapView)
                        UnmapViewOfFile(pFileMapView);
                    if (hFileMap)
                        CloseHandle(hFileMap);
                    if (hFile != INVALID_HANDLE_VALUE)
                        CloseHandle(hFile);
                    

        }

        if (ImGui::Button("Folder Select")) {
            //auto s = LauncherCfgGuiDbg();
            //sprintf_s(text, s.c_str());
        }
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

        ImGui::EndChild();
        ImGui::EndTabItem();
    }

    return localeSwitch;
}
void ErrorMsgBox(th_glossary_t textRef)
{
    auto title = utf8_to_utf16(XSTR(THPRAC_PR_ERROR));
    auto text = utf8_to_utf16(XSTR(textRef));
    MessageBoxW(NULL, text.c_str(), title.c_str(), MB_OK | MB_ICONERROR | MB_SYSTEMMODAL);
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
    LauncherPeekUpd();
    auto scale = LauncherWndGetScale();

    while (LauncherWndNewFrame()) {
        static bool isOpen = true;
        static bool isMinimize = false;
        bool canMove = false;

        ImGui::SetNextWindowSizeConstraints(ImVec2(640.0f * scale, 480.0f * scale), ImVec2(1280.0f * scale, 960.0f * scale));
        ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(960.0f * scale, 720.0f * scale), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowBgAlpha(0.9f);

        std::string wndTitleText = XSTR(THPRAC_LAUNCHER);
        if (LauncherIsChkingUpd()) {
            wndTitleText += " // Checking for updates...";
        }
        wndTitleText = LauncherIsChkingUpd() ? XSTR(THPRAC_LAUNCHER_CHECKING_UPDATE) : XSTR(THPRAC_LAUNCHER);
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
            if (GuiTabItem(XSTR(THPRAC_GAMES))) {
                ImGui::BeginChild("##games");
                LauncherGamesGuiUpd();
                ImGui::EndChild();
                ImGui::EndTabItem();
            }
            if (GuiTabItem(XSTR(THPRAC_LINKS))) {
                ImGui::BeginChild("##links");
                LauncherLinksGuiUpd();
                ImGui::EndChild();
                ImGui::EndTabItem();
            }
            if (GuiTabItem(XSTR(THPRAC_TOOLS))) {
                ImGui::BeginChild("##tools");
                LauncherToolsGuiUpd();
                ImGui::EndChild();
                ImGui::EndTabItem();
            }
            if (GuiTabItem(XSTR(THPRAC_SETTINGS))) {
                ImGui::BeginChild("##settings");
                LauncherCfgGuiUpd();
                ImGui::EndChild();
                ImGui::EndTabItem();
            }

            //localeSwitch = DebugTabBar();

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
            while (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE)) {
            }
            LauncherWndInit(640, 480, 1280, 960, 960, 720);
            localeSwitch = -1;
        }
    }

    LauncherCfgClose();
    LauncherWndShutdown();
    if (gLauncherTrigger == LAUNCHER_RESET) {
        LauncherCfgReset();
    }

    return 0;
}

}
