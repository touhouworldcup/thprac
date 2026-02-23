#pragma once

namespace THPrac {

enum Locale : unsigned {
    LOCALE_NONE = (unsigned int)-1, // Only here because of how GameGuiWnd works
    LOCALE_ZH_CN = 0,
    LOCALE_EN_US = 1,
    LOCALE_JA_JP = 2,
};

enum ChordKeys {
    ChordKey_Ctrl,
    ChordKey_Shift,
    ChordKey_Alt,
    ChordKey_Caps,
    ChordKey_Tab,
    ChordKey_Space,
    ChordKey_Backspace,
    ChordKey_F11,
    ChordKey_F12,
    ChordKey_Insert,
    ChordKey_Home,
    ChordKey_PgUp,
    ChordKey_Delete,
    ChordKey_End,
    ChordKey_PgDn,
    ChordKey_KEYBOARD_COUNT,
    ChordKey_DPad_Up = ChordKey_KEYBOARD_COUNT,
    ChordKey_DPad_Down,
    ChordKey_DPad_Left,
    ChordKey_DPad_Right,
    ChordKey_A,
    ChordKey_B,
    ChordKey_X,
    ChordKey_Y,
    ChordKey_L1,
    ChordKey_L2,
    ChordKey_R1,
    ChordKey_R2,
    ChordKey_Start,
    ChordKey_Select,
    ChordKey_HomeMenu,
    ChordKey_COUNT
};

enum ExistingGameLaunchAction {
    LAUNCH_ACTION_LAUNCH_GAME = 0,
    LAUNCH_ACTION_OPEN_LAUNCHER = 1,
    LAUNCH_ACTION_ALWAYS_ASK = 2,
};

enum CheckUpdateWhen {
    CHECK_UPDATE_LAUNCHER = 0,
    CHECK_UPDATE_ALWAYS = 1,
    CHECK_UPDATE_NEVER = 2,
};

enum FilenameAfterUpdate {
    FN_UPDATE_KEEP_DOWNLOADED = 0,
    FN_UPDATE_USE_PREVIOUS = 1,
    FN_UPDATE_THPRAC_EXE = 2,
};

struct THPracSettingsGlobal {
    unsigned int theme = 0;
    Locale language = LOCALE_EN_US;
    bool render_only_used_glyphs = false;
    bool resizable_window = false;
};

struct HotkeyChords {
    unsigned int backspace_menu = 1 << ChordKey_Backspace;
    unsigned int advanced_menu = 1 << ChordKey_F12;
    unsigned int screenshot = 1 << ChordKey_Home;
    unsigned int tracker = 1 << ChordKey_End;
    unsigned int language = 1 << ChordKey_Alt;
};

struct THPracSettingsStartup {
    ExistingGameLaunchAction existing_game_launch_action = LAUNCH_ACTION_LAUNCH_GAME;    
    FilenameAfterUpdate      filename_after_update = FN_UPDATE_KEEP_DOWNLOADED;
    CheckUpdateWhen          check_update = CHECK_UPDATE_LAUNCHER;
    bool                     update_without_confirmation = false;
    bool                     dont_search_ongoing_game = false;
    bool                     thprac_admin_rights = false;   
};

extern constinit THPracSettingsGlobal gSettingsGlobal;
extern constinit THPracSettingsStartup gSettingsStartup;
extern constinit HotkeyChords hotkeys;
bool LoadSettingsGlobal();
bool SaveSettingsGlobal();

bool LoadSettingsStartup();
bool SaveSettingsStartup();

void InitConfigDir();

void SetTheme(int theme);

}