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
        ImGui::SameLine();
    }
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

void LauncherGamesMain() {
    if (selectedGame) {
        DetailsPage(selectedGame);
        return;
    }

    ImGui::TextUnformatted(S(THPRAC_GAMES_MAIN_SERIES));
    for (auto& game : mainGames) {
        ImGui::Separator();
        if (!game.instances) {
            ImGui::BeginDisabled();
        }
        if (ImGui::Selectable(S(game.title))) {
            selectedGame = &game;
        }
        if (!game.instances) {
            ImGui::EndDisabled();
        }
    }
    ImGui::Separator();
    ImGui::NewLine();

    ImGui::TextUnformatted(S(THPRAC_GAMES_SPINOFF_STG));
    for (auto& game : spinoffShmups) {
        ImGui::Separator();
        if (ImGui::Selectable(S(game.title))) {
            selectedGame = &game;
        }
    }
    ImGui::Separator();
    ImGui::NewLine();
    
    ImGui::TextUnformatted(S(THPRAC_GAMES_SPINOFF_OTHERS));
    for (auto& game : spinoffOthers) {
        ImGui::Separator();
        if (ImGui::Selectable(S(game.title))) {
            selectedGame = &game;
        }
    }
}


}