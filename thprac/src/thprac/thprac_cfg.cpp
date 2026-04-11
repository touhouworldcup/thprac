#include "thprac_cfg.h"
#include "thprac_log.h"
#include "thprac_utils.h"
#include "thprac_update.h"
#include "utils/wininternal.h"

EXTERN_C IMAGE_DOS_HEADER __ImageBase;

namespace THPrac {

constexpr const wchar_t THPRAC_SETTINGS_JSON_NAME[] = L"settings.json";

wchar_t _gConfigDir[MAX_PATH + 1] = {};
unsigned int _gConfigDirLen = 0;
bool _gIsLocalConfigDir = true;
bool _gIsAppDataAvailable = true;

constinit THPracSettings gSettings;
constinit HotkeyChords hotkeys;

static bool BadDirectoryAttributes(DWORD attrs) {
    if(attrs == INVALID_FILE_ATTRIBUTES) {
        return true;
    }
    return !(attrs & FILE_ATTRIBUTE_DIRECTORY);
}

void InitConfigDir() {
    DWORD endPos = 0;
    auto& self = CurrentPeb()->ProcessParameters->ImagePathName;
    for (USHORT i = self.Length / sizeof(wchar_t); i > 0; i--) {
        if (self.Buffer[i] == L'\\') {
            endPos = i + 1;
            break;
        }
    }
    // Initialize _gConfigDir with <EXE_DIR>\\.thprac_data. Obtaining this path is needed anyways to
    // at least check if it exists. Then, if something goes wrong with obtaining the %AppData%
    // environment variable, we can fall back to using .thprac_data at no cost.
    memcpy(_gConfigDir, self.Buffer, endPos * sizeof(wchar_t));
    memcpy(_gConfigDir + endPos, SIZED(L".thprac_data\\"));
    _gConfigDirLen = endPos + t_strlen(L".thprac_data\\");

    GetEnvironmentVariableW(L"AppData", nullptr, 0);
    DWORD code = GetLastError();

    // Cannot obtain AppData environment variable at all, use .thprac_data as data directory
    if (code) {
        force_local_dir:
        if (!CreateDirectoryW(_gConfigDir, nullptr) && GetLastError() != ERROR_ALREADY_EXISTS) {
            // Complete failure to find a suitable data directory. Proceeding with no data directory
            // The user will be prompted about this in log_init
            memset(_gConfigDir, 0, sizeof(_gConfigDir));
            _gConfigDirLen = 0;
        }
        _gIsAppDataAvailable = false;
    }
    // AppData environment variable exists, and .thprac_data has undesirable attributes
    else if (BadDirectoryAttributes(GetFileAttributesW(_gConfigDir))) {
        wchar_t appdataDir[MAX_PATH + 1];
        int appdataLen = GetEnvironmentVariableW(L"AppData", appdataDir, MAX_PATH);
        if (BadDirectoryAttributes(GetFileAttributesW(appdataDir))) {
            goto force_local_dir;
        }

        if (appdataDir[appdataLen] != L'\\') {
            appdataDir[appdataLen++] = L'\\';
        }
        
        memcpy(appdataDir + appdataLen, SIZED(L"thprac\\"));
        appdataLen += t_strlen(L"thprac\\");

        if (!CreateDirectoryW(appdataDir, nullptr) && GetLastError() != ERROR_ALREADY_EXISTS) {
            goto force_local_dir;
        }

        memcpy(_gConfigDir, appdataDir, appdataLen * sizeof(wchar_t));
        _gConfigDirLen = appdataLen;
        _gConfigDir[_gConfigDirLen] = 0;
        _gIsLocalConfigDir = false;
    }
    // else case: .thprac_data exists and is a directory. If that is the case, do nothing.
    // No log statements can be made here because this runs before log_init
}

static const wchar_t* JSON_ERROR_TITLE[] = {
    L"JSON Error", L"JSON Error", L"JSON Error"
};
static const wchar_t* JSON_ERROR_FORMAT[] = {
    L"Failed to load %s: JSON error %d at position %d\nMessage: ",
    L"Failed to load %s: JSON error %d at position %d\nMessage: ",
    L"Failed to load %s: JSON error %d at position %d\nMessage: ",
};

yyjson_doc* yyjson_read_file_report(const wchar_t* path, yyjson_read_flag flg = YYJSON_READ_JSON5, const yyjson_alc* alc_ptr = nullptr) {
begin:
    yyjson_read_err err;
    yyjson_doc* doc;

    // Calls constructor and then operator bool of MappedFile
    if (MappedFile f = path) {
        doc = yyjson_read_opts((char*)f.fileMapView, f.fileSize, flg, alc_ptr, &err);
    } else {
        // This buffer is in it's own scope...
        char buf[MAX_PATH * 2] = {};
        WideCharToMultiByte(CP_UTF8, 0, path, -1, buf, MAX_PATH * 2 - 1, nullptr, nullptr);

       log_printf("JSON: could not load file %s\r\n", buf);
        return nullptr;
    }
    
    if (!doc) {
        // ...and so is this one, meaning they can occupy the space in memory on the stack
        wchar_t json_error_buffer[1024];
        memset(json_error_buffer, 0x10, sizeof(json_error_buffer));
        int wrote = _snwprintf(json_error_buffer, 1023, JSON_ERROR_FORMAT[Gui::LocaleGet()], path, err.code, err.pos);
        
        // Perfectly safe conversion to UTF-16
        // yyjson only returns error messages in English
        for (size_t i = 0; err.msg[i]; i++) {
            json_error_buffer[wrote++] = err.msg[i];
        }
        json_error_buffer[wrote] = 0;

        int choice = MessageBoxW(NULL, json_error_buffer, JSON_ERROR_TITLE[Gui::LocaleGet()], MB_ICONERROR | MB_RETRYCANCEL);
        if (choice == IDRETRY) {
            goto begin;
        } else {
            return nullptr;
        }
    } else {
        return doc;
    }
}

yyjson_doc* LoadConfigFile(const wchar_t* name) {
    if (!_gConfigDirLen) {
        return nullptr;
    }

    wchar_t path[MAX_PATH + 1] = {};
    memcpy(path, _gConfigDir, _gConfigDirLen * sizeof(wchar_t));
    memcpy(path + _gConfigDirLen, name, t_strlen(name) * sizeof(wchar_t));

    return yyjson_read_file_report(path);
}

void SaveConfigFile(const wchar_t* name, void* buf, size_t len) {
    if (!buf || !len || !_gConfigDirLen) {
        return;
    }
    wchar_t path[MAX_PATH + 1] = {};
    memcpy(path, _gConfigDir, _gConfigDirLen * sizeof(wchar_t));
    memcpy(path + _gConfigDirLen, name, t_strlen(name) * sizeof(wchar_t));

    HANDLE hFile = CreateFileW(path, GENERIC_WRITE, FILE_SHARE_READ, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    DWORD byteRet;
    WriteFile(hFile, buf, len, &byteRet, nullptr);
    CloseHandle(hFile);
}

void SetTheme(int theme) {
    switch (theme) {
    default:
    case 0:
        return ImGui::StyleColorsDark();
    case 1:
        return ImGui::StyleColorsLight();
    case 2:
        return ImGui::StyleColorsClassic();
    }
}

bool LoadSettings() {
    wchar_t settingsPath[MAX_PATH + 1] = {};
    memcpy(settingsPath, _gConfigDir, _gConfigDirLen * sizeof(wchar_t));
    memcpy(settingsPath + _gConfigDirLen, SIZED(THPRAC_SETTINGS_JSON_NAME));

    yyjson_doc* doc = yyjson_read_file_report(settingsPath);
    if (!doc) {
        Gui::LocaleSetFromSysLang();
        return false;
    }

    yyjson_val* root = yyjson_doc_get_root(doc);

    // yyjson doesn't use a hashmap, so accessing an object by key loops
    // through the whole object and running a string compare on all keys
    // until it finds a matching one. Instead, I choose to loop through
    // the object once, and checking all possible values for my keys.
    size_t idx, max;
    yyjson_val *key, *val;
    yyjson_obj_foreach(root, idx, max, key, val) {
        if (unsafe_yyjson_equals_str(key, "theme")) {
            yyjson_eval_numeric(val, &gSettings.theme);
            if (gSettings.theme > 2) {
                gSettings.theme = 0;
            }
            continue;
        }
        if (unsafe_yyjson_equals_str(key, "language")) {
            if (!yyjson_eval_numeric(val, (unsigned int*)&gSettings.language) || gSettings.language > 2) {
                Gui::LocaleSetFromSysLang();
            }
            else {
                Gui::__glocale_current = gSettings.language;
            }
            continue;
        }
        if (unsafe_yyjson_equals_str(key, "render_only_used_glyphs")) {
            yyjson_eval_numeric(val, &gSettings.render_only_used_glyphs);
            continue;
        }
        if (unsafe_yyjson_equals_str(key, "resizable_window")) {
            yyjson_eval_numeric(val, &gSettings.resizable_window);
            continue;
        }
        if (unsafe_yyjson_equals_str(key, "console")) {
            yyjson_eval_numeric(val, &gSettings.console);
            continue;
        }
        if (unsafe_yyjson_equals_str(key, "existing_game_launch_action")) {
            yyjson_eval_numeric(val, (unsigned int*)&gSettings.existing_game_launch_action);
            continue;
        }
        if (unsafe_yyjson_equals_str(key, "filename_after_update")) {
            yyjson_eval_numeric(val, (unsigned int*)&gSettings.filename_after_update);
            continue;
        }
        if (unsafe_yyjson_equals_str(key, "check_update")) {
            yyjson_eval_numeric(val, (unsigned int*)&gSettings.check_update);
            continue;
        }
        if (unsafe_yyjson_equals_str(key, "update_without_confirmation")) {
            yyjson_eval_numeric(val, &gSettings.update_without_confirmation);
            continue;
        }
        if (unsafe_yyjson_equals_str(key, "dont_search_ongoing_game")) {
            yyjson_eval_numeric(val, &gSettings.dont_search_ongoing_game);
            continue;
        }
        if (unsafe_yyjson_equals_str(key, "thprac_admin_rights")) {
            yyjson_eval_numeric(val, &gSettings.thprac_admin_rights);
            continue;
        }
        if (unsafe_yyjson_equals_str(key, "backspace_menu_chord")) {
            yyjson_eval_numeric(val, &hotkeys.backspace_menu);
            continue;
        }
        if (unsafe_yyjson_equals_str(key, "advanced_menu_chord")) {
            yyjson_eval_numeric(val, &hotkeys.advanced_menu);
            continue;
        }
        if (unsafe_yyjson_equals_str(key, "screenshot_chord")) {
            yyjson_eval_numeric(val, &hotkeys.screenshot);
            continue;
        }
        if (unsafe_yyjson_equals_str(key, "tracker_chord")) {
            yyjson_eval_numeric(val, &hotkeys.tracker);
            continue;
        }
        if (unsafe_yyjson_equals_str(key, "language_chord")) {
            yyjson_eval_numeric(val, &hotkeys.language);
            continue;
        }
    }

    yyjson_doc_free(doc);

    return true;
}

// Raw strings can't use escape sequences, but they can still be concatenated with other string literals
static const char settingsTemplate[] = 
    "{\n"
    "\t" R"("backspace_menu_chord": %d,)" "\n"
    "\t" R"("advanced_menu_chord": %d,)" "\n"
    "\t" R"("screenshot_chord": %d,)" "\n"
    "\t" R"("tracker_chord": %d,)" "\n"
    "\t" R"("language_chord": %d,)" "\n"
    "\t" R"("theme": %d,)" "\n"
    "\t" R"("language": %d,)" "\n"
    "\t" R"("existing_game_launch_action": %d,)" "\n"
    "\t" R"("filename_after_update": %d,)" "\n"
    "\t" R"("check_update": %d,)" "\n"
    "\t" R"("render_only_used_glyphs": %s,)" "\n"
    "\t" R"("resizable_window": %s,)" "\n"
    "\t" R"("console": %s,)" "\n"
    "\t" R"("update_without_confirmation": %s,)" "\n"
    "\t" R"("dont_search_ongoing_game": %s,)" "\n"
    "\t" R"("thprac_admin_rights": %s,)" "\n"
    "}";

bool SaveSettings() {
    wchar_t settingsPath[MAX_PATH + 1] = {};
    memcpy(settingsPath, _gConfigDir, _gConfigDirLen * sizeof(wchar_t));
    memcpy(settingsPath + _gConfigDirLen, SIZED(THPRAC_SETTINGS_JSON_NAME));

    HANDLE hFile = CreateFileW(settingsPath, GENERIC_WRITE, FILE_SHARE_READ, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        return false;
    }

    char buf[1024] = {};
    int len = snprintf(buf, 1023, settingsTemplate,
        hotkeys.backspace_menu,
        hotkeys.advanced_menu,
        hotkeys.screenshot,
        hotkeys.tracker,
        hotkeys.language,
        gSettings.theme,
        gSettings.language,
        gSettings.existing_game_launch_action,
        gSettings.filename_after_update,
        gSettings.check_update,
        gSettings.render_only_used_glyphs ? "true" : "false",
        gSettings.resizable_window ? "true" : "false",
        gSettings.console ? "true" : "false",
        gSettings.update_without_confirmation ? "true" : "false",
        gSettings.dont_search_ongoing_game ? "true" : "false",
        gSettings.thprac_admin_rights ? "true" : "false"
    );

    DWORD byteRet;
    WriteFile(hFile, buf, len, &byteRet, nullptr);

    CloseHandle(hFile);
    return true;
}

void GuiSettings() {
    ImGui::TextUnformatted(S(THPRAC_GLOBAL_SETTINGS));
    ImGui::Separator();

    ImGui::PushItemWidth(ImGui::GetFontSize() * 8.0f);
    if (ImGui::Combo("语言/Language/言語", (int*)&gSettings.language, "中文\0English\0日本語\0\0")) {
        ImGui::SameLine();
        ImGui::TextColored({ 1.0f, 0.0f, 0.0f, 1.0f }, "%s", S(THPRAC_LANG_SWITCH_UI_FROZEN));
    }
    if (ImGui::Combo(S(THPRAC_THEME), &gSettings.theme, S(THPRAC_THEME_OPTION))) {
        SetTheme(gSettings.theme);
    }
    ImGui::PopItemWidth();

    ImGui::Checkbox(S(THPRAC_RENDER_ONLY_USED_GLYPHS), &gSettings.render_only_used_glyphs);
    ImGui::SameLine();
    Gui::HelpMarker(S(THPRAC_RENDER_ONLY_USED_GLYPHS_DESC));
    ImGui::Checkbox(S(THPRAC_RESIZABLE_WINDOW), &gSettings.resizable_window);

    if (ImGui::Checkbox(S(THPRAC_CONSOLE), &gSettings.console) && gSettings.console && !console_open) { 
        if (!AttachConsole(GetCurrentProcessId()) && !AttachConsole(ATTACH_PARENT_PROCESS)) {
            AllocConsole();
        }
        SetConsoleOutputCP(CP_UTF8);
        SetConsoleCP(CP_UTF8);

        freopen("conin$", "r+b", stdin);
        freopen("conout$", "w+b", stdout);
        freopen("conout$", "w+b", stderr);
        console_open = true;
    }
    if (!gSettings.console && console_open) {
        ImGui::SameLine();
        ImGui::TextColored({ 1.0f, 0.0f, 0.0f, 1.0f }, "%s", S(THPRAC_DISABLE_CONSOLE_USELESS));
    }

    ImGui::NewLine();
    ImGui::TextUnformatted(S(THPRAC_LAUNCH_BEHAVIOR));
    ImGui::SameLine();
    Gui::HelpMarker(S(THPRAC_LAUNCH_BEHAVIOR_NOTICE));
    ImGui::Separator();
    ImGui::SetNextItemWidth(ImGui::GetFontSize() * 8.0f);
    ImGui::Combo(S(THPRAC_EXISTING_GAME_ACTION), (int*)&gSettings.existing_game_launch_action, S(THPRAC_EXISTING_GAME_ACTION_OPTION));
    ImGui::Checkbox(S(THPRAC_DONT_SEARCH_ONGOING), &gSettings.dont_search_ongoing_game);
    ImGui::Checkbox(S(THPRAC_ADMIN_RIGHTS), &gSettings.thprac_admin_rights);
    ImGui::NewLine();
    ImGui::TextUnformatted(S(THPRAC_UPDATE_BEHAVIOR));
    ImGui::Separator();

    if (!UpdaterInitialized()) {
        ImGui::PushStyleColor(ImGuiCol_Text, 0xFFFF0000);
        ImGui::TextUnformatted(S(THPRAC_UPDATE_INIT_FAILED));
        ImGui::PopStyleColor();
    }

    ImGui::PushItemWidth(ImGui::GetFontSize() * 16.0f);
    ImGui::Combo(S(THPRAC_FILENAME_AFTER_UPDATE), (int*)&gSettings.filename_after_update, S(THPRAC_FILENAME_AFTER_UPDATE_OPTION));
    ImGui::Combo(S(THPRAC_CHECK_UPDATE_WHEN), (int*)&gSettings.check_update, S(THPRAC_CHECK_UPDATE_WHEN_OPTION));
    ImGui::PopItemWidth();
    ImGui::Checkbox(S(THPRAC_UPDATE_WITHOUT_CONFIRMATION), &gSettings.update_without_confirmation);
    ImGui::SameLine();
    Gui::HelpMarker(S(THPRAC_UPDATE_WITHOUT_CONFIRMATION_DESC));
    
    if (!background_update_check || background_update_check->hThread) {
        ImGui::BeginDisabled();
        ImGui::Button(S(THPRAC_CHECK_UPDATE_NOW));
        ImGui::EndDisabled();
    } else if (ImGui::Button(S(THPRAC_CHECK_UPDATE_NOW))) {
        background_update_check->hThread = CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)DownloadFile, background_update_check, 0, nullptr);
    }
    
    ImGui::NewLine();
    ImGui::TextUnformatted(S(THPRAC_REBIND_HOTKEYS));
    ImGui::Separator();

    struct GuiHotkeyOption {
        const char* label;
        int& chord;
        bool has_conflict = false;

        inline bool conflicts(GuiHotkeyOption* other) {
            if (this->chord == 0 || other->chord == 0) {
                return false;
            }
            return ((this->chord & other->chord) == this->chord);
        }
    };

    GuiHotkeyOption key_backspace = {
        .label = S(THPRAC_HOTKEY_QUICK_SETTINGS),
        .chord = hotkeys.backspace_menu,
    };

    GuiHotkeyOption key_advanced = {
        .label = S(THPRAC_HOTKEY_ADVANCED_SETTINGS),
        .chord = hotkeys.advanced_menu,
    };

    GuiHotkeyOption key_screenshot = {
        .label = S(THPRAC_HOTKEY_SCREENSHOT),
        .chord = hotkeys.screenshot,
    };

    GuiHotkeyOption key_tracker = {
        .label = S(THPRAC_HOTKEY_TRACKER),
        .chord = hotkeys.tracker,
    };

    GuiHotkeyOption key_language = {
        .label = S(THPRAC_HOTKEY_LANGUAGE_SETTINGS),
        .chord = hotkeys.language,
    };
    
    GuiHotkeyOption keys[] = { key_backspace, key_advanced, key_screenshot, key_tracker };

    for (size_t i = 0; i < elementsof(keys); i++) {
        for (size_t j = 0; j < elementsof(keys); j++) {
            if (i == j)
                continue;
            if (keys[i].conflicts(&keys[j])) {
                keys[i].has_conflict = true;
                keys[j].has_conflict = true;
            }
        }
    }

    for (auto& key : keys) {
        if (key_language.conflicts(&key)) {
            key.has_conflict = true;
            key_language.has_conflict = true;
        }
    }

    for (const auto& key : keys) {
        ImGui::SetNextItemWidth(ImGui::GetFontSize() * 24.0f);
        Gui::ChordEditDropdown(key.label, key.chord);
        if (key.has_conflict) {
            ImGui::SameLine();
            Gui::CustomMarker("(!)", S(THPRAC_HOTKEY_SUBMASK_WARNING));
        }
    }
    ImGui::SetNextItemWidth(ImGui::GetFontSize() * 24.0f);
    Gui::ChordEditDropdown(key_language.label, key_language.chord);
    if (key_language.has_conflict) {
        ImGui::SameLine();
        Gui::CustomMarker("(!)", S(THPRAC_HOTKEY_SUBMASK_WARNING));
    }
    ImGui::SameLine();
    Gui::HelpMarker(S(THPRAC_HOTKEY_LANGUAGE_TOOLTIP));

    if (ImGui::Button(S(THPRAC_HOTKEY_RESET))) {
        hotkeys = {};
    }
}

}