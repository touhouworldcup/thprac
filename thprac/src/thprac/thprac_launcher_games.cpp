#include "utils/utils.h"

#define NOMINMAX
#include "utils/wininternal.h"

#include "thprac_cfg.h"
#include "thprac_load_exe.h"
#include "thprac_gui_locale.h"
#include "thprac_identify.h"
#include "thprac_utils.h"

#include <stdlib.h>
#include <string.h>
#include <yyjson.h>

#include <algorithm>

namespace THPrac {
namespace Gui {
    extern HWND ImplWin32GetHwnd();
}

struct LauncherInstance {
    // If type == TYPE_THCRAP this is passed as the runconfig to thcrap_loader
    const char* path;
    const char* name;
    THGameType type;
    bool apply_thprac;
};

struct LauncherGame {
    THGameID id;
    th_glossary_t title;
    int default_launch = -1;
    unsigned int selected = 0;
    const wchar_t* appdataPath = nullptr;

    // TODO: an std::vector would look a lot cleaner here, but I don't want the compiler
    // to generate atexit destructors for this struct. If a custom dynamic array type is
    // ever added, replace this with the custom array.
    size_t inst_count;
    LauncherInstance* instances;
};

th_glossary_t gameTypeNames[] = {
    TH_TYPE_ERROR,
    TH_TYPE_ORIGINAL,
    TH_TYPE_MODDED,
    TH_TYPE_THCRAP,
    TH_TYPE_CHINESE,
    TH_TYPE_SCHINESE,
    TH_TYPE_TCHINESE,
    TH_TYPE_NYASAMA,
    TH_TYPE_STEAM,
    TH_TYPE_UNCERTAIN,
    TH_TYPE_MALICIOUS,
    TH_TYPE_UNKOWN,
};

static constinit LauncherGame mainGames[] = {
    {
        .id = ID_TH06,
        .title = TH06_TITLE,
    },
    {
        .id = ID_TH07,
        .title = TH07_TITLE,
    },
    {
        .id = ID_TH08,
        .title = TH08_TITLE,
    },
    {
        .id = ID_TH09,
        .title = TH09_TITLE,
    },
    {
        .id = ID_TH10,
        .title = TH10_TITLE,
    },
    {
        .id = ID_TH11,
        .title = TH11_TITLE,
    },
    {
        .id = ID_TH12,
        .title = TH12_TITLE,
    },
    {
        .id = ID_TH13,
        .title = TH13_TITLE,
        .appdataPath = L"%AppData%\\ShanghaiAlice\\th13",
    },
    {
        .id = ID_TH14,
        .title = TH14_TITLE,
        .appdataPath = L"%AppData%\\ShanghaiAlice\\th14",
    },
    {
        .id = ID_TH15,
        .title = TH15_TITLE,
        .appdataPath = L"%AppData%\\ShanghaiAlice\\th15",
    },
    {
        .id = ID_TH16,
        .title = TH16_TITLE,
        .appdataPath = L"%AppData%\\ShanghaiAlice\\th16",
    },
    {
        .id = ID_TH17,
        .title = TH17_TITLE,
        .appdataPath = L"%AppData%\\ShanghaiAlice\\th17",
    },
    {
        .id = ID_TH18,
        .title = TH18_TITLE,
        .appdataPath = L"%AppData%\\ShanghaiAlice\\th18",
    },
    {
        .id = ID_TH19,
        .title = TH19_TITLE,
        .appdataPath = L"%AppData%\\ShanghaiAlice\\th19",
    },
    {
        .id = ID_TH20,
        .title = TH20_TITLE,
        .appdataPath = L"%AppData%\\ShanghaiAlice\\th20",
    },
};

static constinit LauncherGame spinoffShmups[] = {
    {
        .id = ID_ALCOSTG,
        .title = ALCOSTG_TITLE,
    },
    {
        .id = ID_TH095,
        .title = TH095_TITLE,
    },
    {
        .id = ID_TH125,
        .title = TH125_TITLE,
    },
    {
        .id = ID_TH128,
        .title = TH128_TITLE,
        .appdataPath = L"%AppData%\\ShanghaiAlice\\th128",
    },
    {
        .id = ID_TH143,
        .title = TH143_TITLE,
        .appdataPath = L"%AppData%\\ShanghaiAlice\\th143",
    },
    {
        .id = ID_TH165,
        .title = TH165_TITLE,
        .appdataPath = L"%AppData%\\ShanghaiAlice\\th165",
    },
    {
        .id = ID_TH185,
        .title = TH185_TITLE,
        .appdataPath = L"%AppData%\\ShanghaiAlice\\th185",
    },
};

static constinit LauncherGame spinoffOthers[] = {
    {
        .id = ID_TH075,
        .title = TH075_TITLE,
    },
    {
        .id = ID_TH105,
        .title = TH105_TITLE,
    },
    {
        .id = ID_TH123,
        .title = TH123_TITLE,
    },
    {
        .id = ID_TH145,
        .title = TH145_TITLE,
    },
    {
        .id = ID_TH155,
        .title = TH155_TITLE,
    },
    {
        .id = ID_TH175,
        .title = TH175_TITLE,
    },
};


extern yyjson_doc* yyjson_read_file_report(const wchar_t* path, yyjson_read_flag flg = YYJSON_READ_JSON5, const yyjson_alc* alc_ptr = nullptr);

static void InitLauncherGame(LauncherGame* game, yyjson_val* json) {
    yyjson_val* insts = yyjson_obj_get(json, "instances");
    size_t insts_len = yyjson_arr_size(insts);
    if (!insts_len) {
        return;
    }
    LauncherInstance* instances = (LauncherInstance*)malloc(sizeof(LauncherInstance) * insts_len);
    size_t valid_insts_count = 0;

    yyjson_val* cur = unsafe_yyjson_get_first(insts);
    for (size_t i = 0; i < insts_len; i++, cur = unsafe_yyjson_get_next(cur)) {
        const char* path = yyjson_get_str(yyjson_obj_get(cur, "path"));
        if (!path) {
            continue;
        }
        path = _strdup(path);
        const char* name = yyjson_get_str(yyjson_obj_get(cur, "name"));
        if (name) {
            name = _strdup(name);
        }
        
        bool apply_thprac = false;
        unsigned int type = TYPE_ERROR;
        yyjson_eval_numeric(yyjson_obj_get(cur, "apply_thprac"), &apply_thprac);
        yyjson_eval_numeric(yyjson_obj_get(cur, "type"), &type);

        if (type > TYPE_UNKNOWN) {
            type = TYPE_ERROR;
        }
        
        instances[valid_insts_count] = {
            .path = path,
            .name = name,
            .type = (THGameType)type,
            .apply_thprac = apply_thprac,
        };
        valid_insts_count++;
    }

    if (!valid_insts_count) {
        free(instances);
        return;
    }

    game->inst_count = valid_insts_count;
    game->instances = instances;
    yyjson_eval_numeric(yyjson_obj_get(json, "default_launch"), &game->default_launch);
}

void LoadGamesJson() {
    wchar_t gamesJsonPath[MAX_PATH + 1] = {};
    memcpy(gamesJsonPath, _gConfigDir, _gConfigDirLen * sizeof(wchar_t));
    memcpy(gamesJsonPath + _gConfigDirLen, SIZED(L"games.json"));

    yyjson_doc* doc = yyjson_read_file_report(gamesJsonPath);
    if (!doc) {
        return;
    }

    yyjson_val* root = yyjson_doc_get_root(doc);
    if (!yyjson_is_obj(root)) {
        return;
    }

    size_t idx, max;
    yyjson_val *key, *val;
    yyjson_obj_foreach(root, idx, max, key, val) {
        const char* keyReal = unsafe_yyjson_get_str(key);

        for (auto& game : mainGames) {
            if (strcmp(gThGameStrs[game.id], keyReal) == 0) {
                InitLauncherGame(&game, val);
            }
        }
        for (auto& game : spinoffShmups) {
            if (strcmp(gThGameStrs[game.id], keyReal) == 0) {
                InitLauncherGame(&game, val);
            }
        }
        for (auto& game : spinoffOthers) {
            if (strcmp(gThGameStrs[game.id], keyReal) == 0) {
                InitLauncherGame(&game, val);
            }
        }
    }
    yyjson_doc_free(doc);
}

void SaveGamesJson() {
    
}

static constinit LauncherGame* selectedGame = nullptr;
static char instRenameBuf[256] = {};

void DestroyInst(LauncherInstance* inst) {
    if (inst->name) {
        free((void*)inst->name);
    }
    free((void*)inst->path);
}

void LaunchCustom(const wchar_t* dir, THGameType type) {
    SHELLEXECUTEINFOW see = {
        .cbSize = sizeof(see),
        .hwnd = Gui::ImplWin32GetHwnd(),
        .lpVerb = L"open",
        .lpDirectory = dir,
        .nShow = SW_SHOW,
    };

    switch (type) {
    default:
        see.lpFile = L"custom.exe";
        if (ShellExecuteExW(&see)) {
            return;
        }
        break;
    case TYPE_TCHINESE:
        see.lpFile = L"custom_cht.exe";
        if (ShellExecuteExW(&see)) {
            return;
        }
        break;
    case TYPE_CHINESE:
    case TYPE_SCHINESE:
    case TYPE_NYASAMA:
        see.lpFile = L"custom_cn.exe";
        if (ShellExecuteExW(&see)) {
            return;
        }
        see.lpFile = L"custom_c.exe";
        if (ShellExecuteExW(&see)) {
            return;
        }
        see.lpFile = L"custom_chs.exe";
        if (ShellExecuteExW(&see)) {
            return;
        }
        see.lpFile = L"custom_cht.exe";
        if (ShellExecuteExW(&see)) {
            return;
        }
        see.lpFile = L"custom.exe";
        if (ShellExecuteExW(&see)) {
            return;
        }
        break;
    }
}

static void DetailsPage(LauncherGame* game) {
    if (ImGui::Button("Back")) {
        selectedGame = nullptr;
    }

    ImGui::SameLine();
    Gui::TextCentered(S(game->title), ImGui::GetWindowWidth());
    ImGui::Separator();

    ImGui::TextUnformatted(S(THPRAC_GAMES_SELECT_VER));

    ImGui::BeginChild("##__game_table", { 0, ImGui::GetWindowHeight() * 0.35f }, true);

    ImGui::PushStyleColor(ImGuiCol_TableBorderStrong, ImGui::GetStyleColorVec4(ImGuiCol_Border));
    ImGui::PushStyleColor(ImGuiCol_TableBorderLight, ImGui::GetStyleColorVec4(ImGuiCol_Border));

    if (ImGui::BeginTable("instances_table", 3, ImGuiTableFlags_BordersInnerH | ImGuiTableFlags_BordersOuterH | ImGuiTableFlags_BordersInnerV)) {
        ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, 196.0f);
        ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, 99.0f);

        for (size_t i = 0; i < game->inst_count; ++i) {
            LauncherInstance& inst = game->instances[i];
            const char* name;
            if (inst.name) {
                name = inst.name;
            } else {
                name = "";
            }

            ImGui::TableNextRow(ImGuiTableRowFlags_None);
            ImGui::PushID((int)i);

            ImGui::TableSetColumnIndex(0);

            bool selected = game->selected == i;
            ImGui::SetCursorPosX(ImGui::GetWindowPos().x + 4.0f);
            
            if (ImGui::Selectable(name, selected, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowItemOverlap, {0.0f, 24.0f})) {
                game->selected = i;
            }

            if (ImGui::BeginDragDropSource()) {
                ImGui::SetDragDropPayload("##@__dnd_gameinst", &i, sizeof(size_t));
                ImGui::TextUnformatted(name);
                ImGui::EndDragDropSource();
            }

            if (ImGui::BeginDragDropTarget()) {
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("##@__dnd_gameinst")) {
                    size_t src = *(const size_t*)payload->Data;
                    size_t dst = i;

                    if (src != dst) {
                        LauncherInstance moved = game->instances[src];
                        if (src < dst) {
                            memmove(&game->instances[src], &game->instances[src + 1], (dst - src) * sizeof(LauncherInstance));
                        } else {
                            memmove(&game->instances[dst + 1], &game->instances[dst], (src - dst) * sizeof(LauncherInstance));
                        }
                        game->instances[dst] = moved;
                        if (game->selected == src) {
                            game->selected = dst;
                        }
                    }
                }

                ImGui::EndDragDropTarget();
            }

            ImGui::TableSetColumnIndex(1);
            ImGui::TextUnformatted(S(gameTypeNames[inst.type]));
            if (inst.type == TYPE_ERROR) {
                ImGui::SameLine();
                Gui::CustomMarker("(!)", "You tampered with the config file, didn't you?");
            }

            ImGui::TableSetColumnIndex(2);
            ImGui::TextUnformatted(inst.path);

            ImGui::PopID();
        }

        ImGui::EndTable();
    }
    ImGui::PopStyleColor(2);
    ImGui::EndChild();

    if (ImGui::Button(S(THPRAC_GAMES_RENAME))) {
        LauncherInstance& inst = game->instances[game->selected];
        if (inst.name) {
            strncpy(instRenameBuf, inst.name, 255);
        }
        else {
            memset(instRenameBuf, 0, 256);
        }
        ImGui::OpenPopup(S(THPRAC_GAMES_RENAME_MODAL));
    }
    ImGui::SameLine();
    if (Gui::ButtonYesNoConfirm(S(THPRAC_GAMES_DELETE), S(THPRAC_GAMES_DELETE_MODAL), S(THPRAC_GAMES_DELETE_CONFIRM), 6.0f, S(THPRAC_YES), S(THPRAC_NO))) {
        if (game->inst_count == 1) {
            DestroyInst(game->instances + 0);
            game->instances = 0;
            game->inst_count = 0;
            selectedGame = nullptr;
            return;
        }
        else if (game->selected == game->inst_count - 1) {
            LauncherInstance& inst = game->instances[game->selected];
            DestroyInst(&inst);
            memset(&inst, 0, sizeof(inst));
            game->inst_count--;
            game->selected--;
        }
        else {
            LauncherInstance* begin = game->instances + game->selected;
            DestroyInst(begin);
            size_t elem_to_move = game->instances + game->inst_count - begin - 1;
            memmove(begin, begin + 1, elem_to_move * sizeof(LauncherInstance));
            game->inst_count--;
            game->instances[game->inst_count] = {};
        }
    }
    ImGui::SameLine();
    if (ImGui::Button(S(THPRAC_GAMES_OPEN_FOLDER))) {
        std::wstring pathW = utf8_to_utf16(game->instances[game->selected].path);
        size_t idx = pathW.rfind(L"\\");
        if (idx != std::wstring::npos) {
            pathW.resize(idx);
            ShellExecuteW(Gui::ImplWin32GetHwnd(), L"open", pathW.c_str(), nullptr, nullptr, SW_SHOW);
        }
    }
    ImGui::SameLine();
    if (game->appdataPath && ImGui::Button(S(THPRAC_GAMES_OPEN_APPDATA))) {
        SHELLEXECUTEINFOW se = {
            .cbSize = sizeof(se),
            .fMask = SEE_MASK_DOENVSUBST,
            .hwnd = Gui::ImplWin32GetHwnd(),
            .lpVerb = L"open",
            .lpFile = game->appdataPath,
            .nShow = SW_SHOW,
        };
        ShellExecuteExW(&se);
    }
    ImGui::SameLine();
    if (ImGui::Button(S(THPRAC_GAMES_LAUNCH_CUSTOM))) {
        std::wstring pathW = utf8_to_utf16(game->instances[game->selected].path);
        size_t idx = pathW.rfind(L"\\");
        if (idx != std::wstring::npos) {
            pathW.resize(idx);
            LaunchCustom(pathW.c_str(), game->instances[game->selected].type);
        }
    }
    ImGui::NewLine();

    ImGui::Checkbox(S(THPRAC_GAMES_APPLY_THPRAC), &game->instances[game->selected].apply_thprac);

    bool default_launch = game->default_launch == game->selected;
    if(ImGui::Checkbox(S(THPRAC_GAMES_DEFAULT_LAUNCH), &default_launch)) {
        if (default_launch) {
            game->default_launch = game->selected;
        } else {
            game->default_launch = -1;
        }
    }
    ImGui::SameLine();
    Gui::HelpMarker("Not implemented");

    if (Gui::ButtonCentered(S(THPRAC_GAMES_LAUNCH_GAME), 0.85f, { 0.98f, 0.1f })) {
        std::wstring pathW = utf8_to_utf16(game->instances[game->selected].path);
        RunGame(pathW.c_str(), nullptr, game->instances[game->selected].apply_thprac);
    }

    if (Gui::Modal(S(THPRAC_GAMES_RENAME_MODAL))) {
        ImGui::InputText("", instRenameBuf, 255);
        if (Gui::YesNoChoice("OK", "Cancel")) {
            auto& inst = game->instances[game->selected];
            if (inst.name) {
                free((void*)inst.name);
            }
            inst.name = _strdup(instRenameBuf);
        }
        ImGui::EndPopup();
    }
}


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
    CRITICAL_SECTION found_cs = {};

    ScanCtx() {
        InitializeCriticalSection(&found_cs);
    }
    ~ScanCtx() {
        DeleteCriticalSection(&found_cs);
    }
};

static bool GameAlreadyExists(const char* path) {
    for (const auto& game : mainGames) {
        for (size_t i = 0; i < game.inst_count; i++) {
            if (_stricmp(game.instances[i].path, path) == 0) {
                return true;
            }
        }
    }
    for (const auto& game : spinoffShmups) {
        for (size_t i = 0; i < game.inst_count; i++) {
            if (_stricmp(game.instances[i].path, path) == 0) {
                return true;
            }
        }
    }
    for (const auto& game : spinoffOthers) {
        for (size_t i = 0; i < game.inst_count; i++) {
            if (_stricmp(game.instances[i].path, path) == 0) {
                return true;
            }
        }
    }
    return false;
}

static bool DontGoThere(const wchar_t* d) {
    if (d[0] == L'.' && d[1] == 0) {
        return true;
    }
    if (d[0] == L'.' && d[1] == L'.' && d[2] == 0) {
        return true;
    }
    return false;
}

// TODO: since we have to SetCurrentDirectoryW into every folder we scan anyways, 
// why not use NtQueryDirectoryFileEx instead? It requres a handle to a folder,
// but that's actually convenient because of RTL_USER_PROCESS_PARAMETERS -> CurrentDirectory.Handle
static void SearchFunc(ScanCtx* scanCtx, const wchar_t* path) {
    if (!(GetFileAttributesW(path) & FILE_ATTRIBUTE_DIRECTORY)) {
        FoundGame game = {};
        uint16_t oepCode[10] = {};
        if (!IdentifyKnownGame(game.info, game.oepCode, path)) {
            return;
        }
        else {
            auto& cd = CurrentPeb()->ProcessParameters->CurrentDirectory.DosPath;
            auto written = WideCharToMultiByte(CP_UTF8, 0, cd.Buffer, cd.Length / sizeof(wchar_t), game.path, MAX_PATH, nullptr, nullptr);
            WideCharToMultiByte(CP_UTF8, 0, path, -1, game.path + written, MAX_PATH - written, nullptr, nullptr);
            if (GameAlreadyExists(game.path)) {
                return;
            }

            EnterCriticalSection(&scanCtx->found_cs);

            auto& v = scanCtx->found;

            v.insert(std::upper_bound(v.begin(), v.end(), game.info.ver->gameId, [](THGameID id, FoundGame& g) -> bool {
                return id < g.info.ver->gameId;
            }), game);

            LeaveCriticalSection(&scanCtx->found_cs);
            return;
        }
    }
    if (!SetCurrentDirectoryW(path)) {
        return;
    }
    WIN32_FIND_DATAW find = {};
    HANDLE hFind = FindFirstFileW(L"*", &find);
    if (hFind) do {
        if (DontGoThere(find.cFileName)) {
            continue;
        }
        SearchFunc(scanCtx, find.cFileName);
    } while (FindNextFileW(hFind, &find));

    SetCurrentDirectoryW(L"..");
}

static DWORD WINAPI ScanThread(LPVOID lpParam) {
    ScanCtx* scanCtx = (ScanCtx*)lpParam;

    auto& c = CurrentPeb()->ProcessParameters->CurrentDirectory.DosPath;
    std::wstring curdir_bak(c.Buffer, c.Length / sizeof(wchar_t));

    SetCurrentDirectoryW(scanCtx->scan_dir.c_str());
    WIN32_FIND_DATAW find;
    HANDLE hFind = FindFirstFileW(L"*", &find);
    if (hFind) do {
        if (DontGoThere(find.cFileName)) {
            continue;
        }
        SearchFunc(scanCtx, find.cFileName);
    } while (FindNextFileW(hFind, &find));
    SetCurrentDirectoryW(curdir_bak.c_str());
    return 0;
}

static LauncherGame* FindLauncherGameByID(THGameID id) {
    for (auto& game : mainGames) {
        if (game.id == id) {
            return &game;
        }
    }
    for (auto& game : spinoffShmups) {
        if (game.id == id) {
            return &game;
        }
    }
    for (auto& game : spinoffOthers) {
        if (game.id == id) {
            return &game;
        }
    }
}

static void ScanAddInstances(LauncherGame* game, FoundGame* found, size_t found_count) {
    size_t inst_count_prev = game->inst_count;

    for (size_t i = 0; i < found_count; i++) {
        if (found[i].selected) {
            game->inst_count++;
        }
    }

    if (inst_count_prev == game->inst_count) {
        return;
    }

    game->instances = (LauncherInstance*)realloc(game->instances, game->inst_count * sizeof(LauncherInstance));

    for (size_t found_idx = 0, inst_idx = inst_count_prev; found_idx < found_count; found_idx++) {
        if (!found[found_idx].selected) {
            continue;
        }
        auto& inst = game->instances[inst_idx];

        inst.name = _strdup(gThGameStrs[found[found_idx].info.ver->gameId]);
        inst.path = _strdup(found[found_idx].path);
        inst.type = found[found_idx].info.type;
        inst_idx++;
    }
}

static void ScanResults(std::vector<FoundGame>& found) {
    if (!found.size()) {
        return;
    }

    auto cur_id = found[0].info.ver->gameId;
    size_t cur_idx = 0;

    for (size_t i = 1; i < found.size(); i++) {
        if (found[i].info.ver->gameId > cur_id) {
            ScanAddInstances(FindLauncherGameByID(cur_id), found.data() + cur_idx, i - cur_idx);
            cur_idx = i;
            cur_id = found[i].info.ver->gameId;
        }
    }
    ScanAddInstances(FindLauncherGameByID(cur_id), found.data() + cur_idx, found.size() - cur_idx);
    return;
}


constinit ScanCtx* scanCtx = nullptr;

static void ScanForGamesUI() {
    // WAIT_OBJECT_0: Scan thread finished.
    // WAIT_TIMEOUT: Scan thread is running
    // WAIT_FAILED: Scan thread does not exist
    DWORD scanStatus = WaitForSingleObject(scanCtx->scan_thread, 0);

    Gui::TextCentered(S(THPRAC_GAMES_SCAN_FOLDER), ImGui::GetWindowWidth());

    auto& padding = ImGui::GetStyle().WindowPadding;

    float childHeight = ImGui::GetWindowHeight() - ImGui::GetCursorPosY() - ImGui::GetFrameHeight() - padding.y * 2;
    float childWidth = ImGui::GetWindowWidth() - padding.x * 2;

    ImGui::BeginChild(0x5CA88E6, { childWidth, childHeight }, true);
    if (scanStatus != WAIT_FAILED) {
        if (!(scanStatus == WAIT_OBJECT_0 && scanCtx->found.size() == 0)) {
            if (scanStatus != WAIT_OBJECT_0) {
                ImGui::BeginDisabled();
            }

            ImGui::PushID(0xF085D);
            EnterCriticalSection(&scanCtx->found_cs);

            ImGui::BeginTable("###__scan_results", 3, ImGuiTableFlags_Borders);
            
            ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, ImGui::GetFrameHeight());
            ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, 128.0f);

            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            
            bool selectAll = std::all_of(scanCtx->found.begin(), scanCtx->found.end(), [](FoundGame& g) -> bool { return g.selected; });
            if (ImGui::Checkbox("###__scan_select_all", &selectAll)) {
                if (selectAll) for(auto& game : scanCtx->found) {
                    game.selected = true;
                } else for(auto& game : scanCtx->found) {
                    game.selected = false;
                }
            }

            ImGui::TableNextColumn();
            ImGui::TextUnformatted("Game Type");
            ImGui::TableNextColumn();
            ImGui::TextUnformatted("Path");

            auto& found = scanCtx->found;
            for (size_t i = 0; i < found.size(); i++) {
                ImGui::TableNextRow();

                ImGui::TableNextColumn();                
                ImGui::PushID(i);
                ImGui::Checkbox("", &found[i].selected);
                ImGui::PopID();

                ImGui::TableNextColumn();
                ImGui::TextUnformatted(S(gameTypeNames[found[i].info.type]));

                ImGui::TableNextColumn();
                ImGui::TextUnformatted(found[i].path);

                char buf[256] = {};
                snprintf(buf, 255, "MetroHash = { 0x%08x, 0x%08x, 0x%08x, 0x%08x }\noepCode = { 0x%04x, 0x%04x, 0x%04x, 0x%04x, 0x%04x, 0x%04x, 0x%04x, 0x%04x, 0x%04x, 0x%04x }",
                    found[i].info.metroHash[0], found[i].info.metroHash[1], found[i].info.metroHash[2], found[i].info.metroHash[3],
                    found[i].oepCode[0], found[i].oepCode[1], found[i].oepCode[2], found[i].oepCode[3], found[i].oepCode[4],
                    found[i].oepCode[5], found[i].oepCode[6], found[i].oepCode[7], found[i].oepCode[8], found[i].oepCode[9]);

                if (ImGui::IsItemHovered()) {
                    ImGui::BeginTooltip();
                    ImGui::TextUnformatted(buf);
                    ImGui::EndTooltip();
                }
                if (ImGui::IsItemClicked(ImGuiMouseButton_Left)) {
                    ImGui::SetClipboardText(buf);
                }
            }

            ImGui::EndTable();

            LeaveCriticalSection(&scanCtx->found_cs);
            ImGui::PopID();
            if (scanStatus != WAIT_OBJECT_0) {
                ImGui::EndDisabled();
            }
        } else {
            ImGui::SetCursorPosY(childHeight / 2);
            Gui::TextCentered("No games found", childWidth);
        }
    }
    ImGui::EndChild();

    if (scanStatus == WAIT_OBJECT_0) {
        if (ImGui::Button("Select Original")) for (auto& game : scanCtx->found) {
            if (game.info.type == TYPE_ORIGINAL) { 
                game.selected = true;
            }
        }
        ImGui::SameLine();
        if(ImGui::Button("Select Modded")) for (auto& game : scanCtx->found) {
            if (game.info.type != TYPE_ORIGINAL && game.info.type != TYPE_STEAM) { 
                game.selected = true;
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Select Steam")) for (auto& game : scanCtx->found) {
            if (game.info.type == TYPE_STEAM) { 
                game.selected = true;
            }
        }
        ImGui::SameLine();
    } else {
        if (ImGui::Button(S(THPRAC_ABORT))) {
            TerminateThread(scanCtx->scan_thread, 0);
            delete scanCtx;
            scanCtx = nullptr;
            return;
        }
    }

    ImGui::SameLine();

    switch (scanStatus) {
    case WAIT_FAILED:
        if (Gui::ButtonRight(S(THPRAC_LINKS_EDIT_FOLDER)) && SelectFolder(scanCtx->scan_dir, Gui::ImplWin32GetHwnd())) {
            scanCtx->scan_thread = CreateThread(nullptr, 0, ScanThread, scanCtx, 0, nullptr);
        }
        break;
    case WAIT_TIMEOUT:
        ImGui::TextUnformatted("TODO: add some kind of indicator");
        break;
    case WAIT_OBJECT_0:
        if (Gui::ButtonRight("Finish")) {
            ScanResults(scanCtx->found);
            delete scanCtx;
            scanCtx = nullptr;
        }
    }
}
 
static void GameRightClickMenu(LauncherGame* game) {
    if (game) {
        if (ImGui::Selectable(S(THPRAC_GAMES_DETAILS_PAGE))) {
            selectedGame = game;
            ImGui::CloseCurrentPopup();
        }
        ImGui::Separator();
    }

    if(ImGui::Selectable(S(THPRAC_GAMES_SCAN_FOLDER))) {
        scanCtx = new ScanCtx();
        ImGui::CloseCurrentPopup();
    }
    if(ImGui::Selectable(S(THPRAC_STEAM_MNG_BUTTON))) {
        ImGui::CloseCurrentPopup();
    }
    ImGui::Separator();
    if (ImGui::Selectable(S(THPRAC_GAMES_RESCAN))) {
        ImGui::CloseCurrentPopup();
    }
}

static LauncherGame* hovered = nullptr;

static inline void GamesList(LauncherGame* games, size_t count) {
    for (size_t i = 0; i < count; i++) {
        auto& game = games[i];
        ImGui::Separator();
        if (!game.instances) {
            ImGui::BeginDisabled();
        }
        if (ImGui::Selectable(S(game.title))) {
            selectedGame = &game;
        }
        if (ImGui::IsItemHovered()) {
            hovered = &game;
        }
        if (&game == hovered && ImGui::BeginPopupContextItem("##__game_context")) {
            GameRightClickMenu(hovered);
            ImGui::EndPopup();
        }
        if (!game.instances) {
            ImGui::EndDisabled();
        }
    }
}

void LauncherGamesMain() {
    if (scanCtx) {
        ScanForGamesUI();
        return;
    }

    if (selectedGame) {
        DetailsPage(selectedGame);
        return;
    }

    if (ImGui::BeginPopupContextWindow("##__no_game_context")) {
        GameRightClickMenu(nullptr);
        ImGui::EndPopup();
    }

    ImGui::TextUnformatted(S(THPRAC_GAMES_MAIN_SERIES));
    GamesList(mainGames, elementsof(mainGames));

    ImGui::Separator();
    ImGui::NewLine();

    ImGui::TextUnformatted(S(THPRAC_GAMES_SPINOFF_STG));
    GamesList(spinoffShmups, elementsof(spinoffShmups));
    
    ImGui::Separator();
    ImGui::NewLine();
    
    ImGui::TextUnformatted(S(THPRAC_GAMES_SPINOFF_OTHERS));
    GamesList(spinoffOthers, elementsof(spinoffOthers));
}


}