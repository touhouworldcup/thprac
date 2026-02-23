#include "thprac_cfg.h"
#include "thprac_log.h"
#include "thprac_utils.h"
#include "utils/wininternal.h"

#include <yyjson.h>

EXTERN_C IMAGE_DOS_HEADER __ImageBase;

namespace THPrac {

wchar_t _gConfigDir[MAX_PATH + 1] = {};
DWORD _gConfigDirLen = 0;

constinit THPracSettingsStartup gSettingsStartup;
constinit THPracSettingsGlobal gSettingsGlobal;
constinit HotkeyChords hotkeys;

void InitConfigDir() {
    DWORD endPos = GetModuleFileNameW((HINSTANCE)&__ImageBase, _gConfigDir, MAX_PATH);

    for (DWORD i = endPos; i > 0; i--) {
        if (_gConfigDir[i] == L'\\') {
            endPos = i + 1;
            break;
        }
    }
    wcscpy(_gConfigDir + endPos, L".thprac_data\\");
    _gConfigDirLen = endPos;

    GetEnvironmentVariableW(L"AppData", nullptr, 0);
    DWORD code = GetLastError();

    // AppData environment variable not set, force .thprac_data as config dir
    if (code == ERROR_ENVVAR_NOT_FOUND) {
        CreateDirectoryW(_gConfigDir, nullptr);
    }
    // AppData environment variable exists, and .thprac_data is not a directory
    else if (DWORD attrs = GetFileAttributesW(_gConfigDir);
        !code && (attrs == INVALID_FILE_ATTRIBUTES || !(attrs & FILE_ATTRIBUTE_DIRECTORY))) {
        endPos = GetEnvironmentVariableW(L"AppData", _gConfigDir, MAX_PATH);
        
        if (_gConfigDir[endPos - 1] == L'\\') {
            memcpy(_gConfigDir + endPos, SIZED(L"thprac\\"));
            _gConfigDirLen = endPos + constexpr_strlen(L"thprac\\");
        } else {
            memcpy(_gConfigDir + endPos, SIZED(L"\\thprac\\"));
            _gConfigDirLen = endPos + constexpr_strlen(L"\\thprac\\");
        }


    }
    // TODO(log): log data folder. Not implemented because UTF-16 logging is not implemented.
    // else case: .thprac_data exists and is a directory. If that is the case, do nothing.
}

yyjson_doc* yyjson_read_file_report(const wchar_t* path, yyjson_read_flag flg = YYJSON_READ_JSON5, const yyjson_alc* alc_ptr = nullptr) {
    for (;;) {
        yyjson_read_err err;
        yyjson_doc* doc;

        // Ensure MappedFile gets destroyed right after JSON parsing is complete
        {
            MappedFile f(path);
            if (!f.fileMapView) {
                log_printf("JSON: file %s not found\n", "TODO: pass UTF-16 filename to UTF-8 function");
                return nullptr;
            }

            doc = yyjson_read_opts((char*)f.fileMapView, f.fileSize, flg, alc_ptr, &err);
        }
        if (!doc) {
            int choice = log_mboxf(
                0,
                MB_ICONERROR | MB_RETRYCANCEL,
                "JSON Error",
                "Failed to load global.json: JSON error %d at position %d\nMessage: %s",
                err.code, err.pos, err.msg);
            if (choice == IDRETRY) {
                continue;
            } else {
                return nullptr;
            }
        } else {
            return doc;
        }
    }
}

// thcrap is able to use it's entire expression parser for this
// Here I'll just return true for any non empty string that doesn't say false or no
bool str_parse_bool(const char* str) {
    if (*str) {
        return _stricmp(str, "FALSE") == 0 || _stricmp(str, "NO") == 0;
    } else {
        return false;
    }
}

bool yyjson_parse_bool(yyjson_val* val, bool* out) {
    switch (yyjson_get_type(val)) {
    case YYJSON_TYPE_BOOL:
        *out = unsafe_yyjson_get_bool(val);
        return true;
    case YYJSON_TYPE_NUM:
        // Bools and numbers are represented differently internally
        *out = unsafe_yyjson_get_uint(val);
        return true;
    case YYJSON_TYPE_STR:
        *out = str_parse_bool(unsafe_yyjson_get_str(val));
        return true;
    // Both of these types use unsafe_yyjson_get_len for their "get size" functions internally
    case YYJSON_TYPE_OBJ:
    case YYJSON_TYPE_ARR:
        *out = unsafe_yyjson_get_len(val);
        return true;
    default:
        return false;
    }
}

bool yyjson_parse_uint(yyjson_val* val, unsigned int* out) {
    switch (yyjson_get_type(val)) {
    case YYJSON_TYPE_BOOL:
        *out = unsafe_yyjson_get_bool(val);
        return true;
    case YYJSON_TYPE_NUM:
        if (unsafe_yyjson_is_real(val)) {
            // https://www.youtube.com/watch?v=uSf5hIzHAKQ
            *out = unsafe_yyjson_get_real(val);
        } else {
            *out = unsafe_yyjson_get_uint(val);
        }
        return true;
    case YYJSON_TYPE_STR:
        *out = atoi(unsafe_yyjson_get_str(val));
        return true;
    default:
        return false;
    }
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

bool LoadSettingsGlobal() {
    wchar_t settingsGlobalPath[MAX_PATH + 1] = {};
    memcpy(settingsGlobalPath, _gConfigDir, _gConfigDirLen * sizeof(wchar_t));
    memcpy(settingsGlobalPath + _gConfigDirLen, L"global.json", sizeof(L"global.json"));

    yyjson_doc* doc = yyjson_read_file_report(settingsGlobalPath);
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
            yyjson_parse_uint(val, &gSettingsGlobal.theme);
            if (gSettingsGlobal.theme > 2) {
                gSettingsGlobal.theme = 0;
            }
            continue;
        }
        if (unsafe_yyjson_equals_str(key, "language")) {
            if (!yyjson_parse_uint(val, (unsigned int*)&gSettingsGlobal.language) || gSettingsGlobal.language > 2) {
                Gui::LocaleSetFromSysLang();
            }
            continue;
        }
        if (unsafe_yyjson_equals_str(key, "render_only_used_glyphs")) {
            yyjson_parse_bool(val, &gSettingsGlobal.render_only_used_glyphs);
            continue;
        }
        if (unsafe_yyjson_equals_str(key, "resizable_window")) {
            yyjson_parse_bool(val, &gSettingsGlobal.resizable_window);
            continue;
        }
        if (unsafe_yyjson_equals_str(key, "backspace_menu_chord")) {
            yyjson_parse_uint(val, &hotkeys.backspace_menu);
            continue;
        }
        if (unsafe_yyjson_equals_str(key, "advanced_menu_chord")) {
            yyjson_parse_uint(val, &hotkeys.advanced_menu);
            continue;
        }
        if (unsafe_yyjson_equals_str(key, "screenshot_chord")) {
            yyjson_parse_uint(val, &hotkeys.screenshot);
            continue;
        }
        if (unsafe_yyjson_equals_str(key, "tracker_chord")) {
            yyjson_parse_uint(val, &hotkeys.tracker);
            continue;
        }
        if (unsafe_yyjson_equals_str(key, "language_chord")) {
            yyjson_parse_uint(val, &hotkeys.language);
            continue;
        }
    }

    yyjson_doc_free(doc);

    return true;
}

// Raw strings can't use escape sequences, but they can still be concatenated with other string literals
static const char* settingsGlobalTemplate = 
    "{\n"
    "\t" R"("backspace_menu_chord": %d,)" "\n"
    "\t" R"("advanced_menu_chord": %d,)" "\n"
    "\t" R"("screenshot_chord": %d,)" "\n"
    "\t" R"("tracker_chord": %d,)" "\n"
    "\t" R"("language_chord": %d,)" "\n"
    "\t" R"("theme": %d,)" "\n"
    "\t" R"("language": %d,)" "\n"
    "\t" R"("render_only_used_glyphs": %s,)" "\n"
    "\t" R"("resizable_window": %s,)" "\n"
    "}";

bool SaveSettingsGlobal() {
    wchar_t settingsGlobalPath[MAX_PATH + 1] = {};
    memcpy(settingsGlobalPath, _gConfigDir, _gConfigDirLen * sizeof(wchar_t));
    memcpy(settingsGlobalPath, SIZED(L"global.json"));

    HANDLE hFile = CreateFileW(settingsGlobalPath, GENERIC_WRITE, FILE_SHARE_READ, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        return false;
    }

    char buf[256] = {};
    int len = snprintf(buf, 255, settingsGlobalTemplate,
        hotkeys.backspace_menu,
        hotkeys.advanced_menu,
        hotkeys.screenshot,
        hotkeys.tracker,
        hotkeys.language,
        gSettingsGlobal.theme,
        gSettingsGlobal.language,
        gSettingsGlobal.render_only_used_glyphs ? "true" : "false",
        gSettingsGlobal.resizable_window ? "true" : "false"
    );

    DWORD byteRet;
    WriteFile(hFile, buf, len, &byteRet, nullptr);

    CloseHandle(hFile);
    return true;
}

bool LoadSettingsStartup() {
    wchar_t settingsStartupPath[MAX_PATH + 1] = {};
    memcpy(settingsStartupPath, _gConfigDir, _gConfigDirLen * sizeof(wchar_t));
    memcpy(settingsStartupPath, SIZED(L"startup.json"));

    yyjson_doc* doc = yyjson_read_file_report(settingsStartupPath);
    if (!doc) {
        Gui::LocaleSetFromSysLang();
        return false;
    }

    yyjson_val* root = yyjson_doc_get_root(doc);

    size_t idx, max;
    yyjson_val *key, *val;
    yyjson_obj_foreach(root, idx, max, key, val) {
        if (unsafe_yyjson_equals_str(key, "existing_game_launch_action")) { 
            yyjson_parse_uint(val, (unsigned int*)&gSettingsStartup.existing_game_launch_action);
            continue;
        }
        if (unsafe_yyjson_equals_str(key, "filename_after_update")) {
            yyjson_parse_uint(val, (unsigned int*)&gSettingsStartup.filename_after_update);
            continue;
        }
        if (unsafe_yyjson_equals_str(key, "check_update")) { 
            yyjson_parse_uint(val, (unsigned int*)&gSettingsStartup.check_update);
            continue;
        }
        if (unsafe_yyjson_equals_str(key, "update_without_confirmation")) {
            yyjson_parse_bool(val, &gSettingsStartup.update_without_confirmation);
            continue;
        }
        if (unsafe_yyjson_equals_str(key, "dont_search_ongoing_game")) { 
            yyjson_parse_bool(val, &gSettingsStartup.dont_search_ongoing_game);
            continue;
        }
        if (unsafe_yyjson_equals_str(key, "thprac_admin_rights")) { 
            yyjson_parse_bool(val, &gSettingsStartup.thprac_admin_rights);
            continue;
        }
    }

    return true;
}

static const char* settingsStartupTemplate =
    "{\n"
    "\t" R"("existing_game_launch_action": %d,)" "\n"
    "\t" R"("filename_after_update": %d,)" "\n"
    "\t" R"("check_update": %d,)" "\n"
    "\t" R"("update_without_confirmation": %s,)" "\n"
    "\t" R"("dont_search_ongoing_game": %s,)" "\n"
    "\t" R"("thprac_admin_rights": %s,)" "\n"
    "}";

bool SaveSettingsStartup() {
    wchar_t settingsStartupPath[MAX_PATH + 1] = {};
    memcpy(settingsStartupPath, _gConfigDir, _gConfigDirLen * sizeof(wchar_t));
    memcpy(settingsStartupPath, SIZED(L"startup.json"));

    HANDLE hFile = CreateFileW(settingsStartupPath, GENERIC_WRITE, FILE_SHARE_READ, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        return false;
    }

    char buf[256] = {};
    int len = snprintf(buf, 255, settingsStartupTemplate,
        gSettingsStartup.existing_game_launch_action,
        gSettingsStartup.filename_after_update,
        gSettingsStartup.check_update,
        gSettingsStartup.update_without_confirmation ? "true" : "false",
        gSettingsStartup.dont_search_ongoing_game ? "true" : "false",
        gSettingsStartup.thprac_admin_rights ? "true" : "false"
    );
    
    DWORD byteRet;
    WriteFile(hFile, buf, len, &byteRet, nullptr);

    CloseHandle(hFile);
    return true;
}

}