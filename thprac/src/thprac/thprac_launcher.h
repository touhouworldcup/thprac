#pragma once
#define NOMINMAX
#include "utils/utils.h"
#include "utils/wininternal.h"
#include <d3d9.h>

#include <limits.h>

#include <algorithm>
#include <vector>
#include <string>
#include <string_view>

#include <imgui.h>
#include <yyjson.h>

#include "thprac_cfg.h"
#include "thprac_log.h"
#include "thprac_identify.h"
#include "thprac_gui_components.h"
#include "thprac_gui_locale.h"
#include "thprac_utils.h"
#include "thprac_update.h"

namespace THPrac {

enum AFTER_LAUNCH {
    LAUNCH_MINIMIZE = 0,
    LAUNCH_CLOSE = 1,
    LAUNCH_NOTHING = 2,
};

enum APPLY_THPRAC_DEFAULT {
    APPLY_THPRAC_KEEP_STATE = 0,
    APPLY_THPRAC_DEFAULT_OPEN = 1,
    APPLY_THPRAC_DEFAULT_CLOSE = 2,
};

struct ThcrapDir {
    size_t w_len = 0;
    size_t s_len = 0;
    wchar_t w[MAX_PATH + 1] = {};
    char    s[MAX_PATH + 1] = {};

    ThcrapDir& operator=(std::wstring_view other) {
        memcpy(this->w, other.data(), other.size() * sizeof(wchar_t));
        s_len = WideCharToMultiByte(CP_UTF8, 0, other.data(), other.length(), this->s, MAX_PATH, nullptr, nullptr);
        w_len = other.length();

        return *this;
    }

    ThcrapDir& operator=(std::string_view other) {
        memcpy(this->s, other.data(), other.size() * sizeof(wchar_t));
        w_len = MultiByteToWideChar(CP_UTF8, 0, other.data(), other.length(), this->w, MAX_PATH);
        s_len = other.length();

        return *this;
    }

    operator std::wstring_view() {
        return std::wstring_view(w, w_len);
    }

    operator bool() {
        return w_len;
    }

    void clear() {
        memset(this, 0, sizeof(*this));
    }
};

struct LauncherSettings {
    AFTER_LAUNCH after_launch = LAUNCH_MINIMIZE;
    APPLY_THPRAC_DEFAULT apply_thprac_default = APPLY_THPRAC_KEEP_STATE;
    bool auto_default_launch = false;
    ThcrapDir thcrap_dir;
};

struct LauncherInstance {
    // If type == TYPE_THCRAP this is passed as the runconfig to thcrap_loader
    // TODO: these are dynamically allocated, so maybe use a type more suited for that
    const char* path;
    const char* name;
    
    THGameType type;

    // What version a game is is specific to the instance.
    uint8_t ver = 0;
    bool apply_thprac;
    bool allow_oilp = true;
    bool allow_vpatch = true;
};

struct LauncherGame {
    THGameID id;
    th_glossary_t title;
    int default_launch = -1;
    unsigned int selected = 0;

    // Multiple versions for the same game are guaranteed to be sequential in memory,
    // so this is safe.
    const THGameVersion* versions;
    size_t ver_count;

    const wchar_t* appdataPath = nullptr;
    uint32_t steamId = 0;

    // TODO: an std::vector would look a lot cleaner here, but I don't want the compiler
    // to generate atexit destructors for this struct. If a custom dynamic array type is
    // ever added, replace this with the custom array.
    // Note: these are the only dynamic fields in this struct, everything else is known
    // and provided at compile time.
    size_t inst_count;
    LauncherInstance* instances;

    // For gamblers
    th_glossary_t shots = A0000ERROR_C;
    int shot_columns = 0;
};

struct FoundGame {
    // Stored as a char array because that's what ImGui uses for text output.
    // The UTF-16 version is only needed once, to load the file to identify and hash it.
    char path[MAX_PATH + 1] = {};
    bool selected = false;
    uint16_t oepCode[10];
    THKnownGame info = {};
};

struct ScanCtx {
    std::wstring scan_dir;
    std::vector<FoundGame> found;
    HANDLE scan_thread = NULL;

    size_t exes_found = 0;
    size_t exes_scanned = 0;
    size_t text_in_progress_bar_len = 0;
    char text_in_progress_bar[MAX_PATH + 1] = {};

    bool abort_message = false;
    bool relative = false;
};

struct LinksIndexes {
    size_t linkSetIdx;
    size_t linkIdx;
};

struct LinkSet {
    bool is_open = true;
    std::string name;
    std::vector<std::pair<std::string, std::string>> links;
};

constexpr unsigned int PC98_GAMES_LEN = 5;
constexpr unsigned int MAIN_GAMES_LEN = 15;
constexpr unsigned int SPINOFF_SHMUP_LEN = 7;
constexpr unsigned int SPINOFF_OTHER_LEN = 7;
constexpr unsigned int ALL_GAMES_LEN = PC98_GAMES_LEN + MAIN_GAMES_LEN + SPINOFF_SHMUP_LEN + SPINOFF_OTHER_LEN;
constexpr unsigned int GAMES_LEN = ALL_GAMES_LEN - PC98_GAMES_LEN;

union LauncherThcrapSelection {
    bool sel[GAMES_LEN] = {};
    struct { 
        bool main_series[MAIN_GAMES_LEN];
        bool spinoff_shmups[SPINOFF_SHMUP_LEN];
        bool spinoff_others[SPINOFF_OTHER_LEN];
    };
};

// All launcher specific globals that do not contain lots of data already known at compile time go here.
// So for example, buffers used for text input fields go here, meanwhile every LauncherGame is defined
// as a constinit global variable because most of it's fields are known at compile time, so it makes more
// sense to put them into static memory in a ready to use form, rather than copying them into dynamically
// allocated memory.
// 
// TODO: move LauncherInstance into here as well
// TODO: maybe have one or more self managed memory pools for all of this state.
struct LauncherState {
    LauncherSettings settings;
    
    bool g_IsUITextureIDValid = false;
    bool g_IsOverTitleBarButton = false;
    bool goToGamesPage = false;

    void (*toolFunc)(LauncherState*) = nullptr;

    LauncherGame* selectedGame = nullptr;
    LauncherGame* hoveredGame = nullptr;
    THGameID reflectiveLaunchID = ID_UNKNOWN;
    LauncherThcrapSelection thcrapSel;

    th_glossary_t thcrapError = A0000ERROR_C;

    bool inScan = false;
    ScanCtx scanCtx;
    char instRenameBuf[256] = {};
    union {
        bool choices[ALL_GAMES_LEN] = {};
        struct {
            bool pc98_choice[PC98_GAMES_LEN];
            bool maingame_choice[MAIN_GAMES_LEN];
            bool spinoff_shmup_choice[SPINOFF_SHMUP_LEN];
            bool spinoff_other_choice[SPINOFF_OTHER_LEN];
        };
    } randomGameChoices;
    unsigned randomGameRoll = UINT_MAX;
    unsigned randomShotRoll = UINT_MAX;
    LauncherGame* randomShotGame = nullptr;
    bool shotChoices[32] = { true, true, true };

    LinksIndexes linkSelected = { SIZE_MAX, SIZE_MAX };
    char linkEditTitleBuf[1024] = {};
    char linkEditLinkBuf[1024] = {};
    bool linkNameWarn = false;
    bool linkLinkWarn = false;
    std::vector<LinkSet> linkSets;
    struct Dir {
        char data[MAX_PATH] = {};
        operator char*() {
            return data;
        }
    };
    std::vector<Dir> foundThcrapConfigs;
    // it's like vector<bool> but with none of the jank
    static_assert(sizeof(bool) == sizeof(unsigned char));
    std::vector<unsigned char> foundThcrapConfigsSel;

    // The actual downloading of the update is still contained to the launcher because the idea for triggerring
    // updates from an in-game popup is to close the game and run thprac with a `--update` command line flag,
    // which triggers the PreLaunchUpdate function regardless of settings
    HANDLE hUpdateThread = NULL;
    DownloadParams updateDownload;
    std::wstring updateUrl;
};

yyjson_doc* yyjson_read_file_report(const wchar_t* path, yyjson_read_flag flg = YYJSON_READ_JSON5, const yyjson_alc* alc_ptr = nullptr);

void LoadGamesJson(APPLY_THPRAC_DEFAULT);
void SaveGamesJson();

void LauncherGamesMain(LauncherState*);
void LauncherLinksMain(LauncherState*);

void RandomShotRollUI(LauncherState*);
void RandomGameRollUI(LauncherState*);
void ThcrapAddConfigsUI(LauncherState*);

void LoadLinksJson(std::vector<LinkSet>&);
void SaveLinksJson(std::vector<LinkSet>&);


}