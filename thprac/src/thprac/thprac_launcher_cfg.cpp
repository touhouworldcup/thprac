#include "thprac_launcher_cfg.h"
#include "thprac_gui_locale.h"
#include "thprac_launcher_main.h"
#include "thprac_launcher_games.h"
#include "thprac_launcher_utils.h"
#include "thprac_licence.h"
#include "thprac_log.h"
#include "thprac_version.h"
#include "thprac_utils.h"
#include "utils/utils.h"
#include "..\..\resource.h"
#include <Windows.h>
#include <functional>
#include <string>
#include <vector>
#include <wininet.h>
#include <ShlObj.h>
#pragma warning(push)
#pragma warning(disable : 26819)
#include <rapidjson/prettywriter.h>
#pragma warning(pop)
#pragma comment(lib, "wininet.lib")
#include <shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")
#pragma comment(linker, "\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

EXTERN_C IMAGE_DOS_HEADER __ImageBase;

namespace THPrac {
std::wstring* gCfgPath = nullptr;
int gCfgIsLocalDir = 0;
bool gCfgLocalBackupAvaliable = false;
bool gCfgReadOnly = false;
HANDLE gCfgHnd = INVALID_HANDLE_VALUE;
rapidjson::Document gCfgJson;

void LauncherAquireDataDirVar()
{
    if (gCfgPath) {
        return;
    }
    gCfgPath = new std::wstring();

    std::wstring path;
    wchar_t pathBuffer[MAX_PATH];
    GetModuleFileNameW((HMODULE)&__ImageBase, pathBuffer, MAX_PATH);
    path = GetDirFromFullPath(std::wstring(pathBuffer));

    auto backupPath = path + L".thprac_data_backup";
    auto backupPathAttr = GetFileAttributesW(backupPath.c_str());
    if (backupPathAttr != INVALID_FILE_ATTRIBUTES && backupPathAttr & FILE_ATTRIBUTE_DIRECTORY) {
        gCfgLocalBackupAvaliable = true;
    }

    path += L".thprac_data";
    auto fileAttr = GetFileAttributesW(path.c_str());
    if (fileAttr != INVALID_FILE_ATTRIBUTES && fileAttr & FILE_ATTRIBUTE_DIRECTORY) {
        gCfgIsLocalDir = 1;
        path += L"\\";
    } else {
        gCfgIsLocalDir = 0;
        wchar_t appDataPath[MAX_PATH];
        if (SHGetFolderPathW(NULL, CSIDL_APPDATA, NULL, SHGFP_TYPE_CURRENT, appDataPath) == S_OK) {
            path = appDataPath;
            path += L"\\thprac\\";
        } else {
            path = L"";
        }
    }

    *gCfgPath = path;
}
std::wstring LauncherGetDataDir()
{
    LauncherAquireDataDirVar();

    return *gCfgPath;
}

bool LauncherCfgWrite()
{
    if (gCfgHnd == INVALID_HANDLE_VALUE) {
        return false;
    }

    rapidjson::StringBuffer sb;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(sb);
    gCfgJson.Accept(writer);

    auto jsonStr = sb.GetString();
    auto jsonStrLen = strlen(jsonStr);
    DWORD bytesProcessed;
    SetFilePointer(gCfgHnd, 0, NULL, FILE_BEGIN);
    SetEndOfFile(gCfgHnd);
    return WriteFile(gCfgHnd, jsonStr, jsonStrLen, &bytesProcessed, NULL);
}
rapidjson::Document& LauncherCfgGet()
{
    return gCfgJson;
}
bool LauncherCfgInit(bool noCreate)
{
    if (gCfgHnd != INVALID_HANDLE_VALUE) {
        return true;
    }

    std::wstring jsonPath = LauncherGetDataDir();
    CreateDirectoryW(jsonPath.c_str(), NULL);
    jsonPath += L"thprac.json";

    DWORD openFlag = noCreate ? OPEN_EXISTING : OPEN_ALWAYS;
    DWORD openAccess = noCreate ? GENERIC_READ : GENERIC_READ | GENERIC_WRITE;
    gCfgHnd = CreateFileW(jsonPath.c_str(), openAccess, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, openFlag, FILE_ATTRIBUTE_NORMAL, NULL);
    if (gCfgHnd == INVALID_HANDLE_VALUE) {
        return false;
    }

    DWORD bytesProcessed;
    auto fileSize = GetFileSize(gCfgHnd, NULL);
    auto fileBuffer = malloc(fileSize + 1);
    memset(fileBuffer, 0, fileSize + 1);
    if (!ReadFile(gCfgHnd, fileBuffer, fileSize, &bytesProcessed, NULL)) {
        return false;
    }

    if (gCfgJson.Parse((const char*)fileBuffer, fileSize + 1).HasParseError()) {
        gCfgJson.SetObject();
        return LauncherCfgWrite();
    }

    gCfgReadOnly = noCreate;
    return true;
}
void LauncherCfgClose()
{
    if (gCfgHnd != INVALID_HANDLE_VALUE) {
        if (!gCfgReadOnly) {
            LauncherCfgWrite();
        }
        CloseHandle(gCfgHnd);
        auto err = GetLastError();
        gCfgReadOnly = false;
        gCfgHnd = INVALID_HANDLE_VALUE;
        delete gCfgPath;
        gCfgPath = nullptr;
    }
}

rapidjson::Value& GetCfgSettingsJson()
{
    if (gCfgJson.HasMember("settings")) {
        if (!gCfgJson["settings"].IsObject()) {
            gCfgJson["settings"].SetObject();
        }
    } else {
        rapidjson::Document settingsJson;
        settingsJson.SetObject();
        JsonAddMemberA(gCfgJson, "settings", settingsJson, gCfgJson.GetAllocator());
    }
    return gCfgJson["settings"];
}
bool LauncherSettingGet(const char* name, int& valueOut)
{
    if (gCfgJson.HasMember("settings") && gCfgJson["settings"].IsObject()) {
        auto& settingsJson = gCfgJson["settings"];
        if (settingsJson.HasMember(name) && settingsJson[name].IsInt()) {
            valueOut = settingsJson[name].GetInt();
            return true;
        }
    }
    return false;
}
bool LauncherSettingGet(const char* name, unsigned int& valueOut)
{
    if (gCfgJson.HasMember("settings") && gCfgJson["settings"].IsObject()) {
        auto& settingsJson = gCfgJson["settings"];
        if (settingsJson.HasMember(name) && settingsJson[name].IsUint()) {
            valueOut = settingsJson[name].GetUint();
            return true;
        }
    }
    return false;
}
bool LauncherSettingGet(const char* name, bool& valueOut)
{
    if (gCfgJson.HasMember("settings") && gCfgJson["settings"].IsObject()) {
        auto& settingsJson = gCfgJson["settings"];
        if (settingsJson.HasMember(name) && settingsJson[name].IsBool()) {
            valueOut = settingsJson[name].GetBool();
            return true;
        }
    }
    return false;
}
bool LauncherSettingGet(const char* name, float& valueOut)
{
    if (gCfgJson.HasMember("settings") && gCfgJson["settings"].IsObject()) {
        auto& settingsJson = gCfgJson["settings"];
        if (settingsJson.HasMember(name) && settingsJson[name].IsFloat()) {
            valueOut = settingsJson[name].GetFloat();
            return true;
        }
    }
    return false;
}
bool LauncherSettingGet(const char* name, std::string& valueOut)
{
    if (gCfgJson.HasMember("settings") && gCfgJson["settings"].IsObject()) {
        auto& settingsJson = gCfgJson["settings"];
        if (settingsJson.HasMember(name) && settingsJson[name].IsString()) {
            valueOut = settingsJson[name].GetString();
            return true;
        }
    }
    return false;
}
void LauncherSettingSet(const char* name, int& valueIn)
{
    auto& settingsJson = GetCfgSettingsJson();
    if (settingsJson.HasMember(name)) {
        settingsJson.RemoveMember(name);
    }
    JsonAddMember(settingsJson, name, valueIn, gCfgJson.GetAllocator());
    LauncherCfgWrite();
}
void LauncherSettingSet(const char* name, unsigned int& valueIn)
{
    auto& settingsJson = GetCfgSettingsJson();
    if (settingsJson.HasMember(name)) {
        settingsJson.RemoveMember(name);
    }
    JsonAddMember(settingsJson, name, valueIn, gCfgJson.GetAllocator());
    LauncherCfgWrite();
}
void LauncherSettingSet(const char* name, bool& valueIn)
{
    auto& settingsJson = GetCfgSettingsJson();
    if (settingsJson.HasMember(name)) {
        settingsJson.RemoveMember(name);
    }
    JsonAddMember(settingsJson, name, valueIn, gCfgJson.GetAllocator());
    LauncherCfgWrite();
}
void LauncherSettingSet(const char* name, float& valueIn)
{
    auto& settingsJson = GetCfgSettingsJson();
    if (settingsJson.HasMember(name)) {
        settingsJson.RemoveMember(name);
    }
    JsonAddMember(settingsJson, name, valueIn, gCfgJson.GetAllocator());
    LauncherCfgWrite();
}
void LauncherSettingSet(const char* name, std::string& valueIn)
{
    auto& settingsJson = GetCfgSettingsJson();
    if (settingsJson.HasMember(name)) {
        settingsJson.RemoveMember(name);
    }
    JsonAddMemberA(settingsJson, name, valueIn.c_str(), gCfgJson.GetAllocator());
    LauncherCfgWrite();
}

template <typename T>
class THSetting {
public:
    THSetting() = delete;
    THSetting(const char* _name, T _value)
    {
        name = _name;
        value = _value;
        Init();
    }
    virtual void Init() final
    {
        if (!LauncherSettingGet(name.c_str(), value)) {
            LauncherSettingSet(name.c_str(), value);
        }
    }
    virtual T& Get()
    {
        return value;
    }
    virtual void Set(T& _value)
    {
        value = _value;
        LauncherSettingSet(name.c_str(), value);
    }
    virtual void Set()
    {
        LauncherSettingSet(name.c_str(), value);
    }

protected:
    std::string name;
    T value;
};

template <>
class THSetting<std::string> {
public:
    THSetting() = delete;
    THSetting(const char* _name, const char* _value)
    {
        name = _name;
        value = _value;
        Init();
    }
    THSetting(const char* _name, std::string _value)
    {
        name = _name;
        value = _value;
        Init();
    }
    virtual void Init() final
    {
        if (!LauncherSettingGet(name.c_str(), value)) {
            LauncherSettingSet(name.c_str(), value);
        }
    }
    virtual std::string& Get()
    {
        return value;
    }
    virtual void Set(std::string& _value)
    {
        value = _value;
        LauncherSettingSet(name.c_str(), value);
    }
    virtual void Set()
    {
        LauncherSettingSet(name.c_str(), value);
    }

protected:
    std::string name;
    std::string value;
};

class THCfgCheckboxEx : public THSetting<bool> {
public:
    THCfgCheckboxEx(const char* _name, bool _value)
        : THSetting(_name, _value)
    {
    }
    bool Gui(const char* guiTxt, const char* helpTxt = nullptr)
    {
        bool result = false;
        if (ImGui::Checkbox(guiTxt, &value)) {
            value = !value;
            result = true;
        }
        if (helpTxt) {
            ImGui::SameLine();
            GuiHelpMarker(helpTxt);
        }
        return result;
    }
    void Toggle()
    {
        value = !value;
        LauncherSettingSet(name.c_str(), value);
    }
};

class THCfgCheckbox : public THSetting<bool> {
public:
    THCfgCheckbox(const char* _name, bool _value)
        : THSetting(_name, _value)
    {
    }
    void Gui(const char* guiTxt, const char* helpTxt = nullptr)
    {
        if (ImGui::Checkbox(guiTxt, &value)) {
            LauncherSettingSet(name.c_str(), value);
        }
        if (helpTxt) {
            ImGui::SameLine();
            GuiHelpMarker(helpTxt);
        }
    }
};

class THCfgCombo : public THSetting<int> {
public:
    THCfgCombo(const char* _name, int _value, int _counts)
        : THSetting(_name, _value)
    {
        counts = _counts;
        Check(_value);
    }
    void Check(int def = 0)
    {
        if (value < 0 || value >= counts) {
            value = def;
        }
    }
    virtual void Set(int& _value) override
    {
        value = _value;
        LauncherSettingSet(name.c_str(), value);
        Check();
    }
    bool Gui(const char* guiTxt, const char* guiCombo, const char* helpTxt = nullptr)
    {
        ImGui::Text(guiTxt);
        ImGui::SameLine();
        std::string txtTmp = "##";
        txtTmp += guiTxt;
        ImGui::PushItemWidth(620.0f - ImGui::GetWindowSize().x);
        auto result = ImGui::Combo(txtTmp.c_str(), &value, guiCombo);
        ImGui::PopItemWidth();
        if (result) {
            LauncherSettingSet(name.c_str(), value);
        }
        if (helpTxt) {
            ImGui::SameLine();
            GuiHelpMarker(helpTxt);
        }
        return result;
    }

private:
    int counts;
};

class THUpdate {
private:
    THUpdate()
    {

    }
    SINGLETON(THUpdate);

public:
    enum ChkUpdateStatus {
        STATUS_RSV,
        STATUS_CHKING_OR_UPDATING,
        STATUS_UPDATE_PROMPT,
        STATUS_UPD_ABLE_OR_FINISHED,
        STATUS_NO_UPDATE,
        STATUS_INTERNET_ERROR,
    };

    void PeekUpdate()
    {
        int chkUpdate = 0;
        LauncherSettingGet("check_update_timing", chkUpdate);
        if (!chkUpdate && mChkUpdStatus == STATUS_RSV) {
            mUpdateThread.Stop();
            mUpdateThread.Start();
        }
    }
    void CheckUpdate()
    {
        mUpdateThread.Stop();
        mUpdateThread.Start();
    }
    bool IsCheckingUpdate()
    {
        return mChkUpdStatus == STATUS_CHKING_OR_UPDATING;
    }
    ChkUpdateStatus GetUpdateStatus()
    {
        return mChkUpdStatus;
    }

    void ChkUpdPopup()
    {
        if (mChkUpdStatus == STATUS_UPDATE_PROMPT) {
            mChkUpdStatus = STATUS_UPD_ABLE_OR_FINISHED;
            ImGui::OpenPopup(XSTR(THPRAC_UPDATE_MODAL));
        }
        if (GuiModal(XSTR(THPRAC_UPDATE_MODAL))) {
            ImGui::Text(XSTR(THPRAC_UPDATE_PROMPT), mUpdVerStr.c_str());
            ImGui::NewLine();
            //ImGui::Text(UpdateJsonDbg().c_str());

            if (mUpdDesc[Gui::LocaleGet()] != "") {
                ImGui::Text(mUpdDesc[Gui::LocaleGet()].c_str());
                ImGui::NewLine();
            }

            bool disabled = mAutoUpdStatus == STATUS_CHKING_OR_UPDATING;
            if (mUpdDirectLink != "") {
                ImGui::BeginDisabled(disabled);
                if (ImGui::Button(XSTR(THPRAC_UPDATE_AUTO_UPDATE))) {
                    mAutoUpdateThread.Stop();
                    mUpdPercentage = 0.0f;
                    mAutoUpdateThread.Start();
                }
                ImGui::EndDisabled(disabled);
                if (mAutoUpdStatus == STATUS_UPD_ABLE_OR_FINISHED) {
                    GuiLauncherMainTrigger(LAUNCHER_CLOSE);
                }
            }
            if (mUpdDownloads.size()) {
                if (mUpdDirectLink != "") {
                    ImGui::SameLine();
                    ImGui::Text(XSTR(THPRAC_UPDATE_OR_DOWNLOAD));
                } else {
                    ImGui::Text(XSTR(THPRAC_UPDATE_DOWNLOADS));
                }

                for (auto& download : mUpdDownloads) {
                    if (CenteredButton(download.first.c_str())) {
                        ShellExecuteW(NULL, NULL, utf8_to_utf16(download.second.c_str()).c_str(), NULL, NULL, SW_SHOW);
                    }
                }
            }

            if (mAutoUpdStatus == STATUS_CHKING_OR_UPDATING) {
                ImGui::Text(XSTR(THPRAC_UPDATE_DOWNLOADING), mUpdPercentage);
                ImGui::SameLine();
            } else if (mAutoUpdStatus == STATUS_INTERNET_ERROR) {
                ImGui::Text(XSTR(THPRAC_UPDATE_AUTO_UPDATE_FALIED));
                ImGui::SameLine();
            }

            ImGui::BeginDisabled(disabled);
            if (GuiCornerButton(XSTR(THPRAC_CLOSE), nullptr, ImVec2(1.0f, 0.0f), true)) {
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndDisabled(disabled);
            ImGui::EndPopup();
        }
    }
    __declspec(noinline) bool UpdateDialog(bool confirmation)
    {
        bool returnValue = false;
        HWND hDialog;
        mUpdDialogThread.Start();
        while (!mUpdDialogHnd) {
            Sleep(0);
        }
        hDialog = mUpdDialogHnd;

        auto titleStr = utf8_to_utf16(XSTR(THPRAC_UPDATE_DIALOG_CHECKING));
#pragma warning(push) // TODO: make this less hacky
#pragma warning(disable: 28183)
        SetWindowText(hDialog, titleStr.c_str());
#pragma warning(pop)
        LONG_PTR style = GetWindowLongPtr(GetDlgItem(hDialog, IDC_PROGRESS1), GWL_STYLE);
        SetWindowLongPtr(GetDlgItem(hDialog, IDC_PROGRESS1), GWL_STYLE, style | PBS_MARQUEE);
        SendMessage(GetDlgItem(hDialog, IDC_PROGRESS1), PBM_SETMARQUEE, 1, 0);

        mUpdateThread.Stop();
        mUpdateThread.Start();
        for (int i = 0; i < 20; ++i) {
            if (!mUpdateThread.IsActive()) {
                break;
            }
            Sleep(100);
        }
        if (mUpdateThread.IsActive()) {
            ShowWindow(hDialog, SW_SHOW);
            MovWndToTop(hDialog);
            Sleep(1000);
            while (mUpdDialogHnd && mUpdateThread.IsActive()) {
                Sleep(1);
            }
        }

        ShowWindow(hDialog, SW_HIDE);
        mInterruptSignal = true;
        mUpdateThread.Wait();
        mInterruptSignal = false;
        mUpdateThread.Stop();
        auto isUpdateAvailable = mChkUpdStatus == STATUS_UPDATE_PROMPT || mChkUpdStatus == STATUS_UPD_ABLE_OR_FINISHED;
        if (mChkUpdStatus == STATUS_UPDATE_PROMPT) {
            mChkUpdStatus = STATUS_UPD_ABLE_OR_FINISHED;
        } else if (mChkUpdStatus == STATUS_CHKING_OR_UPDATING) {
            mChkUpdStatus = STATUS_INTERNET_ERROR;
        }

        if (mUpdDialogHnd && isUpdateAvailable) {
            auto titleStr = utf8_to_utf16(XSTR(THPRAC_UPDATE_DIALOG_TITLE));
            auto textStr = utf8_to_utf16(XSTR(THPRAC_UPDATE_DIALOG_TEXT));
            if (confirmation || MessageBoxW(NULL, textStr.c_str(), titleStr.c_str(), MB_YESNO | MB_SETFOREGROUND) == IDYES) {
                mAutoUpdateThread.Stop();
                mUpdPercentage = 0.0f;
                mAutoUpdateThread.Start();

                titleStr = utf8_to_utf16(XSTR(THPRAC_UPDATE_DIALOG_UPDATING));
                SetWindowText(hDialog, titleStr.c_str());
                LONG_PTR style = GetWindowLongPtr(GetDlgItem(hDialog, IDC_PROGRESS1), GWL_STYLE);
                SetWindowLongPtr(GetDlgItem(hDialog, IDC_PROGRESS1), GWL_STYLE, style & (~PBS_MARQUEE));
                ShowWindow(hDialog, SW_SHOW);
                MovWndToTop(hDialog);

                while (mUpdPercentage < 100.0f) {
                    if (!mUpdDialogHnd) {
                        break;
                    }
                    SendMessage(GetDlgItem(hDialog, IDC_PROGRESS1), PBM_SETPOS, (int)mUpdPercentage, 0);
                    Sleep(30);
                }
                if (mUpdDialogHnd) {
                    SendMessage(GetDlgItem(hDialog, IDC_PROGRESS1), PBM_SETPOS, 100, 0);
                    Sleep(500);
                }

                ShowWindow(hDialog, SW_HIDE);
                mInterruptSignal = true;
                mAutoUpdateThread.Wait();
                mInterruptSignal = false;
                mAutoUpdateThread.Stop();
                if (mAutoUpdStatus == STATUS_UPD_ABLE_OR_FINISHED) {
                    returnValue = true;
                } else {
                    mAutoUpdStatus = STATUS_RSV;
                }
            } 
        }

        mUpdDialogHnd = NULL;
        DestroyWindow(hDialog);
        mUpdDialogThread.Wait();
        mUpdDialogThread.Stop();
        return returnValue;
    }
    static DWORD WINAPI UpdateDialogCtrlFunc(_In_ LPVOID lpParameter)
    {
        auto hDialog = CreateDialog(NULL, MAKEINTRESOURCE(IDD_DIALOG1), NULL, UpdateDialogProc);
        THUpdate::singleton().mUpdDialogHnd = hDialog;

        MSG msg;
        while (true) {
            if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE)) {
                if (!IsDialogMessage(hDialog, &msg)) {
                    TranslateMessage(&msg);
                    DispatchMessage(&msg);
                }
            }
            if (THUpdate::singleton().mUpdDialogHnd == NULL) {
                break;
            }
        }

        return 0;
    }
    static INT_PTR __stdcall UpdateDialogProc(HWND hDialog, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        switch (msg) {
        case WM_INITDIALOG:
            return TRUE;
        case WM_COMMAND:
            switch (LOWORD(wParam)) {
            case IDOK:
                break;
            case IDCANCEL:
                THUpdate::singleton().mUpdDialogHnd = NULL;
                break;
            }
            break;
        default:
            return FALSE;
        }
        return TRUE;
    }

private:
    static bool CenteredButton(const char* buttonText)
    {
        auto wndSize = ImGui::GetWindowSize();
        auto buttonWidth = ImGui::CalcTextSize(buttonText).x + ImGui::GetStyle().FramePadding.x * 2;
        ImGui::SetCursorPosX((wndSize.x - buttonWidth) / 2.0f);
        return ImGui::Button(buttonText, ImVec2(buttonWidth, 0));
    }
    static void InitUpdatePopup(rapidjson::Document& versionJson)
    {
        auto& cfgGui = THUpdate::singleton();

        cfgGui.mUpdVerStr = "";
        auto& version = versionJson["version"];
        for (int i = 0; i < 4; ++i) {
            auto versionNum = version[i].GetInt();
            if (i < 2 || versionNum) {
                cfgGui.mUpdVerStr += i ? '.' : 'v';
                cfgGui.mUpdVerStr += std::to_string(version[i].GetInt());
            }
        }

        cfgGui.mUpdDirectLink = "";
        if (versionJson.HasMember("direct_link") && versionJson["direct_link"].IsString()) {
            cfgGui.mUpdDirectLink = versionJson["direct_link"].GetString();
        }
        if (versionJson.HasMember("file_size") && versionJson["file_size"].IsUint()) {
            cfgGui.mUpdFileSize = versionJson["file_size"].GetUint();
        }

        cfgGui.mUpdDownloads.clear();
        if (versionJson.HasMember("downloads") && versionJson["downloads"].IsObject()) {
            auto& downloadsJson = versionJson["downloads"];
            for (auto it = downloadsJson.MemberBegin(); it != downloadsJson.MemberEnd(); ++it) {
                if (it->value.IsString()) {
                    cfgGui.mUpdDownloads.emplace_back(it->name.GetString(), it->value.GetString());
                }
            }
        }

        for (auto& desc : cfgGui.mUpdDesc) {
            desc = "";
        }
        if (versionJson.HasMember("description") && versionJson["description"].IsArray() && versionJson["description"].Size() == 3) {
            auto& descJson = versionJson["description"];
            for (int i = 0; i < 3; ++i) {
                if (descJson[i].IsString()) {
                    cfgGui.mUpdDesc[i] = descJson[i].GetString();
                }
            }
        }

        THUpdate::singleton().mChkUpdStatus = STATUS_UPDATE_PROMPT;
    }
    static void CheckUpdateJson(const char* jsonStr, size_t jsonSize = 0)
    {
        rapidjson::Document versionJson;
        if (!versionJson.Parse(jsonStr, jsonSize ? jsonSize : strlen(jsonStr) + 1).HasParseError()) {
            if (versionJson.HasMember("version")) {
                auto& version = versionJson["version"];
                if (version.IsArray() && version.Size() == 4 && version[0].IsInt() && version[1].IsInt() && version[2].IsInt() && version[3].IsInt()) {
                    for (int i = 0; i < 4; ++i) {
                        int versionNum = version[i].GetInt();
                        if (versionNum > GetVersionInt()[i]) {
                            InitUpdatePopup(versionJson);
                            return;
                        } else if (versionNum < GetVersionInt()[i]) {
                            break;
                        }
                    }
                }
            }
        }
        THUpdate::singleton().mChkUpdStatus = STATUS_NO_UPDATE;
        return;
    }
    static bool ParseURL_X(std::string url, std::wstring& serverNameOut, std::wstring& objectNameOut, std::wstring& fileNameOut, bool& isHttpsOut)
    {
        bool isHttps = true;
        if (url.find("https://") == 0) {
            url = url.substr(8);
            isHttps = true;

        } else if (url.find("http://") == 0) {
            url = url.substr(7);
            isHttps = false;
        } else {
            return false;
        }

        auto slashPos = url.find('/');
        if (slashPos == std::string::npos) {
            return false;
        }

        auto slashRPos = url.rfind('/');
        if (slashRPos != std::string::npos) {
            fileNameOut = utf8_to_utf16(url.substr(slashRPos + 1).c_str());
        }
        serverNameOut = utf8_to_utf16(url.substr(0, slashPos).c_str());
        objectNameOut = utf8_to_utf16(url.substr(slashPos).c_str());
        isHttpsOut = isHttps;
        return true;
    }

    static DWORD DownloadSingleFile(
        const wchar_t* url, std::vector<uint8_t>& out, std::function<void(DWORD, DWORD)> progressCallback = [](DWORD, DWORD) {})
    {
        static HINTERNET hInternet = NULL;
        DWORD byteRet = sizeof(DWORD);
        if (!hInternet) {
            hInternet = InternetOpenW(L"thprac", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
            if (!hInternet)
                return -1;
            DWORD ignore = 1;
            if (!InternetSetOptionW(hInternet, INTERNET_OPTION_IGNORE_OFFLINE, &ignore, sizeof(DWORD)))
                return -2;
        }
        HINTERNET hFile = InternetOpenUrlW(hInternet, url, NULL, 0, INTERNET_FLAG_RELOAD | INTERNET_FLAG_KEEP_CONNECTION, 0);
        if (!hFile)
            return -3;
        defer(InternetCloseHandle(hFile));
        
        DWORD fileSize = 0;
        if (!HttpQueryInfoW(hFile, HTTP_QUERY_FLAG_NUMBER | HTTP_QUERY_CONTENT_LENGTH, &fileSize, &byteRet, 0))
            return -4;

        std::vector<uint8_t> buffer;
        auto remSize = fileSize;
        progressCallback(0, fileSize);
        while (remSize) {
            DWORD readSize = 0;
            if (!InternetQueryDataAvailable(hFile, &readSize, 0, 0)) {
                readSize = remSize;
            }
            if (readSize == 0) {
                return -5;
            }
            buffer.resize(readSize);
            if (InternetReadFile(hFile, buffer.data(), readSize, &byteRet) == FALSE) {
                return -6;
            }
            remSize -= byteRet;
            progressCallback(remSize, fileSize);
            out.insert(out.end(), buffer.begin(), buffer.end());
        }
        return 0;
    }

    static DWORD WINAPI CheckUpdateFunc(_In_ LPVOID lpParameter)
    {
        auto& updObj = THUpdate::singleton();
        updObj.mChkUpdStatus = STATUS_CHKING_OR_UPDATING;

        std::vector<uint8_t> updateJson;
        DWORD status = DownloadSingleFile(L"https://raw.githubusercontent.com/touhouworldcup/thprac/master/thprac_version.json", updateJson);
        if (status)
            updObj.mChkUpdStatus = STATUS_INTERNET_ERROR;
        else
            CheckUpdateJson((char*)updateJson.data(), updateJson.size());
        return 0;
    }
    static DWORD WINAPI AutoUpdateFunc(_In_ LPVOID lpParameter)
    {
        auto& updObj = THUpdate::singleton();
        updObj.mAutoUpdStatus = STATUS_CHKING_OR_UPDATING;

        std::vector<uint8_t> newFile;
        DWORD status = DownloadSingleFile(utf8_to_utf16(updObj.mUpdDirectLink.c_str()).c_str(), newFile, [&](DWORD remSize, DWORD fileSize) {
            updObj.mUpdPercentage = (fileSize - remSize) * 100 / (float)fileSize;
        });
        if (status) {
            updObj.mAutoUpdStatus = STATUS_INTERNET_ERROR;
            return status;
        }

        HANDLE localeFileHnd;
        wchar_t* exePathCstr;
        wchar_t tmpPath[MAX_PATH];
        std::wstring localFileName;
        std::wstring remoteFileName;
        std::wstring serverName;
        std::wstring objectName;
        bool isHttps;
        ParseURL_X(updObj.mUpdDirectLink, serverName, objectName, remoteFileName, isHttps);
        _get_wpgmptr(&exePathCstr);
        GetDirFromFullPath(std::wstring(exePathCstr));
        GetTempPath(MAX_PATH, tmpPath);
        localFileName = tmpPath;
        localFileName += remoteFileName;
        localeFileHnd = CreateFileW(localFileName.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if (localeFileHnd == INVALID_HANDLE_VALUE) {
            return 1;
        }
        SetFilePointer(localeFileHnd, 0, NULL, FILE_BEGIN);
        SetEndOfFile(localeFileHnd);

        DWORD byteRet;
        WriteFile(localeFileHnd, newFile.data(), newFile.size(), &byteRet, NULL);

        CloseHandle(localeFileHnd);
        ShellExecuteW(NULL, NULL, localFileName.c_str(),
            (std::wstring(L"--update-launcher-1 ") + exePathCstr).c_str(), tmpPath, SW_SHOW);
        updObj.mAutoUpdStatus = STATUS_UPD_ABLE_OR_FINISHED;
        return 0;
    }

    GuiThread mUpdateThread { THUpdate::CheckUpdateFunc };
    GuiThread mAutoUpdateThread { THUpdate::AutoUpdateFunc };
    ChkUpdateStatus mChkUpdStatus = STATUS_RSV;
    ChkUpdateStatus mAutoUpdStatus = STATUS_RSV;
    float mChkUpdHintTime = 0.0f;
    std::vector<std::pair<std::string, std::string>> mUpdDownloads;
    std::string mUpdVerStr;
    std::string mUpdDesc[3];
    std::string mUpdDirectLink;
    size_t mUpdFileSize = 0;
    float mUpdPercentage = 0.0f;
    GuiThread mUpdDialogThread { THUpdate::UpdateDialogCtrlFunc };
    HWND mUpdDialogHnd = NULL;
    bool mInterruptSignal = false;
};

class THCfgGui {
private:
    THCfgGui()
    {
        mGuiUpdFunc = [&]() { GuiMain(); };
        CfgInit();
        for (auto& game : gGameRoll) {
            mThcrapGames[game.type].push_back(game);
        }
    }
    SINGLETON(THCfgGui);

public:
    void GuiUpdate()
    {
        mGuiUpdFunc();
    }

    void FileOperateWrapper(FILEOP_FLAGS op, const wchar_t* from, const wchar_t* to)
    {
        SHFILEOPSTRUCTW fileStruct = {};
        fileStruct.hwnd = 0;
        fileStruct.wFunc = op;
        fileStruct.pFrom = from;
        fileStruct.pTo = to;
        fileStruct.fFlags = FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_SILENT | FOF_NOCONFIRMMKDIR;
        SHFileOperationW(&fileStruct);
    }
    void LauncherCfgReset()
    {
        LauncherCfgClose();
        LogClose();
        if (mCfgResetFlag == 1) {
            wchar_t appDataPath[MAX_PATH];
            if (SHGetFolderPathW(NULL, CSIDL_APPDATA, NULL, SHGFP_TYPE_CURRENT, appDataPath) == S_OK) {
                std::wstring jsonPath = appDataPath;
                jsonPath += L"\\thprac";
                jsonPath += L"\\thprac.json";
                DeleteFileW(jsonPath.c_str());
            }
        } else if (mCfgResetFlag == 2) {
            if (mCfgUseLocalDir != gCfgIsLocalDir) {
                wchar_t pathBuffer[MAX_PATH];
                wchar_t appDataPath[MAX_PATH];
                SHGetFolderPathW(NULL, CSIDL_APPDATA, NULL, SHGFP_TYPE_CURRENT, appDataPath);
                GetModuleFileNameW((HMODULE)&__ImageBase, pathBuffer, MAX_PATH);
                std::wstring globalPath = appDataPath;
                globalPath += L"\\thprac";
                globalPath.push_back('\0');
                globalPath.push_back('\0');
                std::wstring localPath = GetDirFromFullPath(std::wstring(pathBuffer));

                if (!gCfgIsLocalDir) {
                    auto dest = localPath + L".thprac_data";
                    dest.push_back('\0');
                    dest.push_back('\0');
                    if (mCfgDirChgGlobalParam == 1 || mCfgDirChgGlobalParam == 2) {
                        FileOperateWrapper(FO_DELETE, dest.c_str(), nullptr);
                        FileOperateWrapper(mCfgDirChgGlobalParam == 1 ? FO_MOVE : FO_COPY, globalPath.c_str(), dest.c_str());
                    } else if (mCfgDirChgLocalParam == 1) {
                        auto src = localPath + L".thprac_data_backup";
                        MoveFileW(src.c_str(), dest.c_str());
                    } else if (mCfgDirChgLocalParam == 0) {
                        CreateDirectoryW(dest.c_str(), NULL);
                    }
                } else {
                    auto src = localPath + L".thprac_data";
                    src.push_back('\0');
                    src.push_back('\0');
                    if (mCfgDirChgLocalParam == 0) {
                        auto dest = localPath + L".thprac_data_backup";
                        MoveFileW(src.c_str(), dest.c_str());
                    } else if (mCfgDirChgLocalParam == 1 || mCfgDirChgLocalParam == 2) {
                        if (mCfgDirChgLocalParam == 1) {
                            FileOperateWrapper(FO_DELETE, globalPath.c_str(), nullptr);
                        }
                        FileOperateWrapper(mCfgDirChgLocalParam == 1 ? FO_MOVE : FO_DELETE, src.c_str(), mCfgDirChgLocalParam == 1 ? globalPath.c_str() : nullptr);
                    }
                }
            }

            if (LauncherCfgInit()) {
                bool useRelPath = mCfgRelativePath && gCfgIsLocalDir;
                LauncherSettingSet("use_relative_path", useRelPath);
                LauncherGamesForceReload();
                LauncherCfgClose();
            }
        }
        mCfgResetFlag = 0;
    }

private:
    void CfgInit()
    {
        int language = Gui::LocaleGet();
        mCfgLanguage.Set(language);
        mOriginalLanguage = mCfgLanguage.Get();
    }

    void PathAndDirSettings()
    {
        bool dirSettingModalFlag = false;
        bool confirmModalFlag = false;

        if (ImGui::Button(XSTR(THPRAC_DIRECTORY_SETTING))) {
            ImGui::OpenPopup(XSTR(THPRAC_DIRECTORY_SETTING_MODAL));
            mCfgUseLocalDir = gCfgIsLocalDir;
            mCfgDirChgGlobalParam = 0;
            mCfgDirChgLocalParam = 0;
            mCfgRelativePath = mUseRelativePath.Get();
        }

        if (GuiModal(XSTR(THPRAC_DIRECTORY_SETTING_MODAL))) {
            ImGui::PushTextWrapPos(LauncherWndGetSize().x * 0.9f);
            ImGui::Text(XSTR(THPRAC_DIRECTORY_SETTING_DESC));
            ImGui::NewLine();

            ImGui::RadioButton(XSTR(THPRAC_DIR_GLOBAL), &mCfgUseLocalDir, 0);
            ImGui::SameLine();
            ImGui::RadioButton((XSTR(THPRAC_DIR_LOCAL)), &mCfgUseLocalDir, 1);
            ImGui::SameLine();
            if (mCfgUseLocalDir == 1) {
                ImGui::Checkbox(XSTR(THPRAC_USE_REL_PATH), &mCfgRelativePath);
            } else {
                ImGui::BeginDisabled();
                ImGui::Checkbox(XSTR(THPRAC_USE_REL_PATH), &mCfgRelativePath);
                ImGui::EndDisabled();
            }

            if (gCfgIsLocalDir == mCfgUseLocalDir) {
                ImGui::BeginDisabled();
            }
            if (!gCfgIsLocalDir) {
                ImGui::Text(XSTR(THPRAC_EXISTING_GLOBAL_DATA));
                ImGui::SameLine();
                ImGui::RadioButton(XSTR(THPRAC_IGNORE_GLOBAL_DATA), &mCfgDirChgGlobalParam, 0);
                ImGui::SameLine();
                ImGui::RadioButton(XSTR(THPRAC_MOVE_TO_LOCAL), &mCfgDirChgGlobalParam, 1);
                ImGui::SameLine();
                ImGui::RadioButton(XSTR(THPRAC_COPY_TO_LOCAL), &mCfgDirChgGlobalParam, 2);

                ImGui::Text(XSTR(THPRAC_EXISTING_LOCAL_DATA));
                ImGui::SameLine();
                ImGui::RadioButton(XSTR(THPRAC_IGNORE_LOCAL_DATA), &mCfgDirChgLocalParam, 0);
                ImGui::SameLine();
                if (gCfgIsLocalDir != mCfgUseLocalDir && !gCfgLocalBackupAvaliable) {
                    ImGui::BeginDisabled();
                }
                ImGui::RadioButton(XSTR(THPRAC_USE_BACKUP_DATA), &mCfgDirChgLocalParam, 1);
                if (gCfgIsLocalDir != mCfgUseLocalDir && !gCfgLocalBackupAvaliable) {
                    ImGui::EndDisabled();
                }
                ImGui::SameLine();
                ImGui::BeginDisabled();
                ImGui::RadioButton(XSTR(THPRAC_OVERWRTITE_DATA), &mCfgDirChgLocalParam, 2);
                ImGui::EndDisabled();
                if (mCfgDirChgGlobalParam == 1 || mCfgDirChgGlobalParam == 2) {
                    mCfgDirChgLocalParam = 2;
                } else if (mCfgDirChgLocalParam == 2) {
                    mCfgDirChgLocalParam = 0;
                }
            } else {

                ImGui::Text(XSTR(THPRAC_EXISTING_GLOBAL_DATA));
                ImGui::SameLine();
                ImGui::RadioButton(XSTR(THPRAC_IGNORE_GLOBAL_DATA), &mCfgDirChgGlobalParam, 0);
                ImGui::SameLine();
                if (gCfgIsLocalDir != mCfgUseLocalDir) {
                    ImGui::BeginDisabled();
                }
                ImGui::RadioButton(XSTR(THPRAC_OVERWRTITE_DATA), &mCfgDirChgGlobalParam, 1);
                if (gCfgIsLocalDir != mCfgUseLocalDir) {
                    ImGui::EndDisabled();
                }
                if (mCfgDirChgLocalParam == 1) {
                    mCfgDirChgGlobalParam = 1;
                } else if (mCfgDirChgGlobalParam == 1) {
                    mCfgDirChgGlobalParam = 0;
                }

                ImGui::Text(XSTR(THPRAC_EXISTING_LOCAL_DATA));
                ImGui::SameLine();
                ImGui::RadioButton(XSTR(THPRAC_CREATE_BACKUP_DATA), &mCfgDirChgLocalParam, 0);
                ImGui::SameLine();
                ImGui::RadioButton(XSTR(THPRAC_MOVE_TO_GLOBAL), &mCfgDirChgLocalParam, 1);
                ImGui::SameLine();
                ImGui::RadioButton(XSTR(THPRAC_PURGE_LOCAL), &mCfgDirChgLocalParam, 2);
            }
            if (gCfgIsLocalDir == mCfgUseLocalDir) {
                ImGui::EndDisabled();
            }

            if (gCfgIsLocalDir == mCfgUseLocalDir && mCfgRelativePath == mUseRelativePath.Get()) {
                if (GuiCornerButton(XSTR(THPRAC_CANCEL), nullptr, ImVec2(1.0f, 0.0f), true)) {
                    ImGui::CloseCurrentPopup();
                }
            } else {
                auto retnValue = GuiCornerButton(XSTR(THPRAC_APPLY), XSTR(THPRAC_CANCEL), ImVec2(1.0f, 0.0f), true);
                if (retnValue == 1) {
                    ImGui::CloseCurrentPopup();
                    confirmModalFlag = true;
                } else if (retnValue == 2) {
                    ImGui::CloseCurrentPopup();
                }
            }

            ImGui::PopTextWrapPos();
            ImGui::EndPopup();
        }

        if (confirmModalFlag) {
            ImGui::OpenPopup(XSTR(THPRAC_DIRECTORY_CONFIRM));
        }
        if (GuiModal(XSTR(THPRAC_DIRECTORY_CONFIRM))) {
            ImGui::Text(XSTR(THPRAC_DIRECTORY_CONFIRM_DESC));
            auto buttonSize = ImGui::GetItemRectSize().x / 2.05f;
            if (ImGui::Button(XSTR(THPRAC_YES), ImVec2(buttonSize, 0))) {
                GuiLauncherMainTrigger(LAUNCHER_RESET);
                mCfgResetFlag = 2;
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();
            if (ImGui::Button(XSTR(THPRAC_NO), ImVec2(buttonSize, 0))) {
                dirSettingModalFlag = true;
                //ImGui::OpenPopup("Data directory##modal");
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
        if (dirSettingModalFlag) {
            ImGui::OpenPopup(XSTR(THPRAC_DIRECTORY_SETTING_MODAL));
        }

        ImGui::SameLine();
        if (ImGui::Button(XSTR(THPRAC_DATADIR_OPEN))) {
            ShellExecuteW(NULL, L"open", LauncherGetDataDir().c_str(), NULL, NULL, SW_SHOW);
        }

        ImGui::SameLine();
        if (GuiButtonAndModalYesNo(XSTR(THPRAC_RESET_LAUNCHER), XSTR(THPRAC_RESET_LAUNCHER_MODAL), XSTR(THPRAC_RESET_LAUNCHER_WARNING), -1.0f, XSTR(THPRAC_YES), XSTR(THPRAC_NO))) {
            GuiLauncherMainTrigger(LAUNCHER_RESET);
            mCfgResetFlag = 1;
        }
    }

    bool GuiGameTypeChkBox(const char* text, int idx)
    {
        bool result = ImGui::Checkbox(text, &(mGameTypeOpt[idx]));
        if (result) {
            if (!idx) {
                for (auto& cfg : mThcrapCfg) {
                    cfg.second = mGameTypeOpt[idx];
                }
            } else {
                for (auto& game : mThcrapGames[idx]) {
                    if (game.playerSelect) {
                        game.selected = mGameTypeOpt[idx];
                    }
                }
            }
        }
        return result;
    }
    void GuiThcrapBatchAddWnd()
    {
        if (ImGui::Button(XSTR(THPRAC_BACK))) {
            mGuiUpdFunc = [&]() { GuiMain(); };
        }
        ImGui::SameLine();
        GuiCenteredText(XSTR(THPRAC_THCRAP_ADDCFG_CHILD));
        ImGui::Separator();

        ImGui::BeginChild("BATCH_ADD");

        bool itemChanged = false;
        auto childHeight = std::max(ImGui::GetWindowHeight() * 0.37f, ImGui::GetFontSize() * 10.5f);

        ImGui::Text(XSTR(THPRAC_THCRAP_ADDCFG_CONFIGS));
        ImGui::BeginChild("BATCH_ADD_CFG", ImVec2(0, childHeight), true);
        ImGui::Columns(3, 0, false);
        bool allCfgSelected = true;
        for (auto& cfg : mThcrapCfg) {
            auto txtSize = ImGui::CalcTextSize(cfg.first.c_str());
            itemChanged |= ImGui::Checkbox(cfg.first.c_str(), &cfg.second);
            if (ImGui::IsItemHovered() && txtSize.x > ImGui::GetWindowWidth() * 0.3f) {
                ImGui::BeginTooltip();
                ImGui::PushTextWrapPos(ImGui::GetFontSize() * 25.0f);
                ImGui::TextUnformatted(cfg.first.c_str());
                ImGui::PopTextWrapPos();
                ImGui::EndTooltip();
            }
            if (!cfg.second) {
                allCfgSelected = false;
            }
            mGameTypeOpt[0] = allCfgSelected;
            ImGui::NextColumn();
        }
        ImGui::Columns(1);
        ImGui::EndChild();

        ImGui::Text(XSTR(THPRAC_THCRAP_ADDCFG_GAMES));
        ImGui::BeginChild("BATCH_ADD_GAMES", ImVec2(0, childHeight), true);
        int i = 0;
        for (auto& gameType : mThcrapGames) {
            if (i) {
                bool allSelected = true;
                bool allUnavailable = true;
                ImGui::Columns(i == 1 ? 7 : 6, 0, false);
                for (auto& game : gameType) {
                    if (game.playerSelect) {
                        allUnavailable = false;
                        itemChanged |= ImGui::Checkbox(game.name, &game.selected);
                        if (!game.selected) {
                            allSelected = false;
                        }
                    } else {
                        ImGui::BeginDisabled();
                        ImGui::Checkbox(game.name, &game.selected);
                        ImGui::EndDisabled();
                    }
                    mGameTypeOpt[i] = allSelected;
                    ImGui::NextColumn();
                }
                if (allUnavailable) {
                    mGameTypeOpt[i] = false;
                }
                ImGui::Columns(1);
                if (i != 3) {
                    ImGui::NewLine();
                }
            }
            ++i;
        }
        ImGui::EndChild();
        ImGui::Indent(ImGui::GetStyle().ItemSpacing.x);
        itemChanged |= GuiGameTypeChkBox(XSTR(THPRAC_THCRAP_ADDCFG_ALLCFG), 0);
        ImGui::SameLine();
        itemChanged |= GuiGameTypeChkBox(XSTR(THPRAC_GAMES_MAIN_SERIES), 1);
        ImGui::SameLine();
        itemChanged |= GuiGameTypeChkBox(XSTR(THPRAC_GAMES_SPINOFF_STG), 2);
        ImGui::SameLine();
        itemChanged |= GuiGameTypeChkBox(XSTR(THPRAC_GAMES_SPINOFF_OTHERS), 3);
        ImGui::Unindent(ImGui::GetStyle().ItemSpacing.x);

        if (itemChanged) {
            mThcrapAddPromptTime = 0.0f;
        }
        if (mThcrapAddPromptTime > 0.0f) {
            ImGui::PushStyleColor(ImGuiCol_Button, mThcrapAddPromptCol);
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, mThcrapAddPromptCol);
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, mThcrapAddPromptCol);
            mThcrapAddPromptTime -= ImGui::GetIO().DeltaTime;
            GuiCornerButton(mThcrapAddPrompt.c_str(), nullptr, ImVec2(1.0f, 0.0f), true);
            ImGui::PopStyleColor();
            ImGui::PopStyleColor();
            ImGui::PopStyleColor();
        } else {
            auto retnValue = GuiCornerButton(XSTR(THPRAC_APPLY), nullptr, ImVec2(1.0f, 0.0f), true);
            if (retnValue == 1) {
                size_t cfgSize = 0;
                size_t gameSize = 0;
                for (auto& cfg : mThcrapCfg) {
                    if (cfg.second) {
                        cfgSize++;
                        for (auto& gameType : mThcrapGames) {
                            for (auto& game : gameType) {
                                if (game.playerSelect && game.selected) {
                                    gameSize++;
                                    LauncherGamesThcrapAdd(game.name, cfg.first, mCfgThpracDefault.Get());
                                }
                            }
                        }
                    }
                }
                LauncherGamesThcrapAdd("", std::string(), false, true);
                mThcrapAddPromptTime = 2.0f;
                if (!cfgSize) {
                    mThcrapAddPrompt = XSTR(THPRAC_THCRAP_ADDCFG_NOCFG);
                    mThcrapAddPromptCol = { 0.8f, 0.0f, 0.0f, 1.0f };
                } else if (!gameSize) {
                    mThcrapAddPrompt = XSTR(THPRAC_THCRAP_ADDCFG_NOGAME);
                    mThcrapAddPromptCol = { 0.8f, 0.0f, 0.0f, 1.0f };
                } else {
                    mThcrapAddPrompt = XSTR(THPRAC_THCRAP_ADDCFG_SUCCESS);
                    mThcrapAddPromptCol = { 0.0f, 0.75f, 0.0f, 1.0f };
                }
            } 
        }
        

        ImGui::EndChild();
    }
    void GuiThcrapSettings()
    {
        ImGui::Text(XSTR(THPRAC_THCRAP));
        ImGui::Separator();
        if (mThcrap.Get() == "") {
            ImGui::Text(XSTR(THPRAC_THCRAP_NOTYET));
            if (ImGui::Button("Get thcrap")) {
                ShellExecuteW(NULL, L"open", L"https://www.thpatch.net/", NULL, NULL, SW_SHOW);
            }
            ImGui::SameLine();
            if (ImGui::Button(XSTR(THPRAC_THCRAP_SET))) {
                auto path = LauncherWndFolderSelect();
                if (path != L"") {
                    if (!LauncherGamesThcrapTest(path)) {
                        mThcrapHintTimeBuffer = 2;
                        mThcrapHintTime = 5.0f;
                        mThcrapHintStr = XSTR(THPRAC_THCRAP_INVALID);
                    } else {
                        mThcrap.Set(utf16_to_utf8(path.c_str()));
                        LauncherGamesThcrapSetup();
                        mThcrapHintTime = 0.0f;
                        //mThcrapHintTimeBuffer = 2;
                        //mThcrapHintTime = 15.0f;
                        //mThcrapHintStr = "Hint: Use the \"Batch add\" button to add thcrap configs to your game lists.";
                    }
                }
            }
        } else {
            ImGui::Text(XSTR(THPRAC_THCRAP_LOCATION), mThcrap.Get().c_str());
            if (GuiButtonAndModalYesNo(XSTR(THPRAC_THCRAP_UNSET), XSTR(THPRAC_THCRAP_UNSET_MODAL), XSTR(THPRAC_THCRAP_UNSET_TXT), -1.0f, XSTR(THPRAC_YES), XSTR(THPRAC_NO))) {
                mThcrap.Set(std::string(""));
                mThcrapHintTime = 0.0f;
            }
            ImGui::SameLine();
            if (ImGui::Button(XSTR(THPRAC_THCRAP_ADDCFG))) {
                LauncherGamesThcrapSetup();
                LauncherGamesThcrapCfgGet(mThcrapCfg, mThcrapGames);
                if (mThcrapCfg.size()) {
                    mGuiUpdFunc = [&]() { GuiThcrapBatchAddWnd(); };
                } else {
                    mThcrapHintTimeBuffer = 2;
                    mThcrapHintTime = 5.0f;
                    mThcrapHintStr = XSTR(THPRAC_THCRAP_ADDCFG_404);
                }
            }
            ImGui::SameLine();
            if (ImGui::Button(XSTR(THPRAC_THCRAP_LAUNCH_CONFIGURE))) {
                if (!LauncherGamesThcrapLaunch()) {
                    mThcrapHintTimeBuffer = 2;
                    mThcrapHintTime = 5.0f;
                    mThcrapHintStr = XSTR(THPRAC_THCRAP_LAUNCH_FAILED);
                }
            }
        }
        if (mThcrapHintTimeBuffer) {
            mThcrapHintTimeBuffer--;
        } else if (mThcrapHintTime > 0.0f) {
            mThcrapHintTime -= ImGui::GetIO().DeltaTime;
            ImGui::SameLine();
            ImGui::Text(mThcrapHintStr.c_str());
        }
    }

    void TextLink(const char* text, const wchar_t* link)
    {
        ImGui::Text(text);
        if (ImGui::IsItemHovered()) {
            ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
            if (ImGui::IsMouseClicked(0)) {
                ShellExecuteW(NULL, NULL, link, NULL, NULL, SW_SHOW);
            }
        }
    }
    void GuiLicenceWnd()
    {
        if (ImGui::Button(XSTR(THPRAC_BACK))) {
            mGuiUpdFunc = [&]() { GuiMain(); };
        }
        ImGui::SameLine();
        GuiCenteredText("COPYING");
        ImGui::Separator();
        ImGui::BeginChild("COPYING_INFO");
        Gui::ShowLicenceInfo();
        ImGui::EndChild();
    }
    void GuiMain()
    {
        ImGui::Text(XSTR(THPRAC_LAUNCH_BEHAVIOR));
        ImGui::Separator();
        mAdminRights.Gui(XSTR(THPRAC_ADMIN_RIGHTS));
        mExistingGameAction.Gui(XSTR(THPRAC_EXISTING_GAME_ACTION), XSTR(THPRAC_EXISTING_GAME_ACTION_OPTION));
        mDontSearchOngoingGame.Gui(XSTR(THPRAC_DONT_SEARCH_ONGOING));
        ImGui::BeginDisabled();
        mReflectiveLaunch.Gui(XSTR(THPRAC_REFLECTIVE_LAUNCH));
        ImGui::EndDisabled();
        ImGui::SameLine();
        GuiHelpMarker(XSTR(THPRAC_REFLECTIVE_LAUNCH_DESC));
        ImGui::NewLine();

        ImGui::Text(XSTR(THPRAC_SETTING_LAUNCHER));
        ImGui::Separator();
        //mCfgAlwaysOpen.Gui(XSTR(THPRAC_ALWAYS_OPEN_LAUNCHER), XSTR(THPRAC_ALWAYS_OPEN_LAUNCHER_DESC));
        if (mCfgTheme.Gui("Theme:", "Dark\0Light\0Classic\0\0")) {
            SetTheme(mCfgTheme.Get());
        }
        mCfgAfterLaunch.Gui(XSTR(THPRAC_AFTER_LAUNCH), XSTR(THPRAC_AFTER_LAUNCH_OPTION));
        mAutoDefLaunch.Gui(XSTR(THPRAC_AUTO_DEFAULT_LAUNCH), XSTR(THPRAC_AUTO_DEFAULT_LAUNCH_DESC));
        mCfgThpracDefault.Gui(XSTR(THPRAC_APPLY_THPRAC_DEFAULT), XSTR(THPRAC_APPLY_THPRAC_DEFAULT_OPTION));
        mCfgFilterDefault.Gui(XSTR(THPRAC_FILTER_DEFAULT), XSTR(THPRAC_FILTER_DEFAULT_OPTION), XSTR(THPRAC_FILTER_DEFAULT_DESC));
        PathAndDirSettings();
        ImGui::NewLine();

        GuiThcrapSettings();
        ImGui::NewLine();

        ImGui::Text(XSTR(THPRAC_GAME_ADJUSTMENTS));
        ImGui::Separator();
        mCfgUnlockRefreshRate.Gui(XSTR(THPRAC_UNLOCK_REFRESH_RATE), XSTR(THPRAC_UNLOCK_REFRESH_RATE_DESC));
        mResizableWindow.Gui(XSTR(THPRAC_RESIZABLE_WINDOW));
        ImGui::NewLine();

        ImGui::Text(XSTR(THPRAC_SETTING_LANGUAGE));
        ImGui::Separator();
        mCfgLanguage.Gui(XSTR(THPRAC_LANGUAGE), u8"中文\0English\0日本語\0\0");
        if (mOriginalLanguage != mCfgLanguage.Get()) {
            ImGui::Text(th_glossary_str[mCfgLanguage.Get()][THPRAC_LANGUAGE_HINT]);
        }
        ImGui::NewLine();

        ImGui::Text(XSTR(THPRAC_SETTING_UPDATE));
        ImGui::Separator();
        //mCfgCheckUpdate.Gui(XSTR(THPRAC_CHK_UPDATE_ON_STARTUP));
        mCheckUpdateTiming.Gui(XSTR(THPRAC_CHECK_UPDATE_WHEN), XSTR(THPRAC_CHECK_UPDATE_WHEN_OPTION));
        if (mCheckUpdateTiming.Get() == 2) {
            ImGui::BeginDisabled();
            mUpdateWithoutConfirm.Gui(XSTR(THPRAC_UPDATE_WITHOUT_CONFIRMATION));
            ImGui::EndDisabled();
            ImGui::SameLine();
            ImGui::Text(XSTR(THPRAC_UPDATE_WITHOUT_CONFIRMATION_DESC));
        } else {
            mUpdateWithoutConfirm.Gui(XSTR(THPRAC_UPDATE_WITHOUT_CONFIRMATION));
        }
        mFilenameAfterUpdate.Gui(XSTR(THPRAC_FILENAME_AFTER_UPDATE), XSTR(THPRAC_FILENAME_AFTER_UPDATE_OPTION));
        if (THUpdate::singleton().IsCheckingUpdate()) {
            ImGui::BeginDisabled();
            ImGui::Button(XSTR(THPRAC_CHECK_UPDATE_NOW));
            ImGui::EndDisabled();
        } else {
            if (ImGui::Button(XSTR(THPRAC_CHECK_UPDATE_NOW))) {
                if (!THUpdate::singleton().IsCheckingUpdate()) {
                    THUpdate::singleton().CheckUpdate();
                }
            }
        }

        switch (THUpdate::singleton().GetUpdateStatus()) {
        case THPrac::THUpdate::STATUS_CHKING_OR_UPDATING:
            ImGui::SameLine();
            ImGui::Text(XSTR(THPRAC_UPDATE_CHECKING));
            break;
        case THPrac::THUpdate::STATUS_UPD_ABLE_OR_FINISHED:
            ImGui::SameLine();
            ImGui::Text(XSTR(THPRAC_UPDATE_AVALIABLE));
            break;
        case THPrac::THUpdate::STATUS_NO_UPDATE:
            ImGui::SameLine();
            ImGui::Text(XSTR(THPRAC_UPDATE_NO_UPDATE));
            break;
        case THPrac::THUpdate::STATUS_INTERNET_ERROR:
            ImGui::SameLine();
            ImGui::Text(XSTR(THPRAC_UPDATE_ERROR));
            break;
        default:
            break;
        }
        ImGui::NewLine();

        ImGui::Text(XSTR(THPRAC_SETTING_ABOUT));
        ImGui::Separator();
        ImGui::Text(XSTR(TH_ABOUT_VERSION), GetVersionStr());
        ImGui::SameLine();
        if (ImGui::Button(XSTR(TH_ABOUT_SHOW_LICENCE))) {
            mGuiUpdFunc = [&]() { GuiLicenceWnd(); };
        }
        ImGui::NewLine();
        ImGui::Text(XSTR(TH_ABOUT_AUTHOR));
        TextLink(XSTR(TH_ABOUT_WEBSITE), L"https://github.com/touhouworldcup/thprac");
        ImGui::NewLine();
        ImGui::Text(XSTR(TH_ABOUT_THANKS), "You!");
    }

    THCfgCombo mCfgLanguage { "language", 0, 3 };
    THCfgCheckbox mCfgAlwaysOpen { "always_open_launcher", false };
    THCfgCombo mCfgAfterLaunch { "after_launch", 0, 3 };
    THCfgCheckbox mAutoDefLaunch { "auto_default_launch", false };
    THCfgCombo mCfgThpracDefault { "apply_thprac_default", 0, 3 };
    THCfgCombo mCfgFilterDefault { "filter_default", 0, 3 };
    THCfgCombo mCfgTheme { "theme", 0, 3 };
    THSetting<bool> mUseRelativePath { "use_relative_path", false };
    THSetting<std::string> mThcrap { "thcrap", "" };
    THCfgCheckbox mCfgUnlockRefreshRate { "unlock_refresh_rate", false };
    THCfgCheckbox mCfgCheckUpdate { "check_update", true };

    THCfgCheckbox mResizableWindow { "resizable_window", false };
    THCfgCheckbox mReflectiveLaunch { "reflective_launch", false };
    THCfgCombo mExistingGameAction { "existing_game_launch_action", 0, 3 };
    THCfgCheckbox mDontSearchOngoingGame { "dont_search_ongoing_game", false };
    THCfgCheckbox mAdminRights { "thprac_admin_rights", false };
    THCfgCombo mCheckUpdateTiming { "check_update_timing", 0, 3 };
    THCfgCheckbox mUpdateWithoutConfirm { "update_without_confirmation", false };
    THCfgCombo mFilenameAfterUpdate { "filename_after_update", 0, 3 };
    int mOriginalLanguage;

    std::string mThcrapHintStr;
    float mThcrapHintTime = 0.0f;
    int mThcrapHintTimeBuffer = false;
    std::string mThcrapAddPrompt;
    float mThcrapAddPromptTime = 0.0f;
    ImVec4 mThcrapAddPromptCol;
    std::vector<GameRoll> mThcrapGames[4];
    bool mGameTypeOpt[4];
    std::vector<std::pair<std::string, bool>> mThcrapCfg;

    unsigned int mCfgResetFlag = 0;
    int mCfgUseLocalDir = 0;
    int mCfgDirChgGlobalParam = 0;
    int mCfgDirChgLocalParam = 0;
    bool mCfgRelativePath = false;

    std::function<void(void)> mGuiUpdFunc = []() {};
};

void LauncherCfgReset()
{
    THCfgGui::singleton().LauncherCfgReset();
}
void LauncherCfgGuiUpd()
{
    THCfgGui::singleton().GuiUpdate();
}
void LauncherChkUpdPopup()
{
    return THUpdate::singleton().ChkUpdPopup();
}
bool LauncherIsChkingUpd()
{
    return THUpdate::singleton().IsCheckingUpdate();
}
bool LauncherUpdDialog(bool confirmation)
{
    return THUpdate::singleton().UpdateDialog(confirmation);
}
void LauncherPeekUpd()
{
    THUpdate::singleton().PeekUpdate();
}

bool DeleteFileLoop(const wchar_t* fileName, size_t timeout = 0)
{
    auto attr = GetFileAttributesW(fileName);
    if (attr == INVALID_FILE_ATTRIBUTES || attr & FILE_ATTRIBUTE_DIRECTORY || attr & FILE_ATTRIBUTE_READONLY) {
        return false;
    }

    size_t localTimeout = 0;
    while (true) {
        if (DeleteFileW(fileName)) {
            return true;
        }
        Sleep(1000);
        localTimeout += 1000;
        if (localTimeout > timeout) {
            return false;
        }
    }
}
bool CheckMutexLoop(const wchar_t* mutex_name, size_t timeout = 0)
{
    size_t localTimeout = 0;
    while (true) {
        auto result = OpenMutexW(SYNCHRONIZE, FALSE, mutex_name);
        if (result) {
            CloseHandle(result);
            return true;
        }
        Sleep(500);
        localTimeout += 500;
        if (localTimeout > timeout) {
            return false;
        }
    }
}
bool LauncherPreUpdate(wchar_t* pCmdLine)
{
    std::wstring cmd = pCmdLine;
    int stage = 0;
    if (cmd.find(L"--update-launcher-1 ") == 0) {
        stage = 1;
        cmd = cmd.substr(20);
    } else if (cmd.find(L"--update-launcher-2 ") == 0) {
        stage = 2;
        cmd = cmd.substr(20);
    } else if (cmd.find(L"--update-launcher ") == 0) {
        stage = 3;
        cmd = cmd.substr(18);
    }

    if (stage) {
        DeleteFileLoop(cmd.c_str(), 20000);

        if (stage == 1) {
            wchar_t* exePathCstr;
            _get_wpgmptr(&exePathCstr);

            std::wstring finalDir = GetDirFromFullPath(cmd);
            std::wstring finalPath;
            int filenameAfterUpdate = 0;
            if (LauncherCfgInit(true)) {
                LauncherSettingGet("filename_after_update", filenameAfterUpdate);
                LauncherCfgClose();
            }
            switch (filenameAfterUpdate) {
            case 1:
                finalPath = cmd;
                break;
            case 2:
                finalPath = GetDirFromFullPath(cmd) + L"thprac.exe";
                break;
            default:
                finalPath = GetDirFromFullPath(cmd) + GetNameFromFullPath(std::wstring(exePathCstr));
                break;
            }

            CopyFile(exePathCstr, finalPath.c_str(), FALSE);
            ShellExecuteW(NULL, NULL, finalPath.c_str(),
                (std::wstring(L"--update-launcher-2 ") + exePathCstr).c_str(), finalDir.c_str(), SW_SHOW);

            return true;
        } 
    }

    return false;
}

}
