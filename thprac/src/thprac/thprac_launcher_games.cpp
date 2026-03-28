#include "thprac_launcher.h"

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

inline bool LargeBottomButton(const char* text, float height, float offset = 0.0f) {
    height *= g_Scale;

    auto& style = ImGui::GetStyle();
    auto wnd_dim = ImGui::GetWindowSize();

    ImGui::SetCursorPosY(wnd_dim.y - height - style.ItemSpacing.y * 2 - offset);
    return ImGui::Button(text, { wnd_dim.x, height });
}

struct LauncherInstance {
    // If type == TYPE_THCRAP this is passed as the runconfig to thcrap_loader
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

    // TODO: an std::vector would look a lot cleaner here, but I don't want the compiler
    // to generate atexit destructors for this struct. If a custom dynamic array type is
    // ever added, replace this with the custom array.
    size_t inst_count;
    LauncherInstance* instances;

    // For gamblers
    th_glossary_t shots = A0000ERROR_C;
    int shot_columns = 0;
};

constexpr unsigned int PC98_GAMES_LEN = 5;
constexpr unsigned int MAIN_GAMES_LEN = 15;
constexpr unsigned int SPINOFF_SHMUP_LEN = 7;
constexpr unsigned int SPINOFF_OTHER_LEN = 7;
constexpr unsigned int ALL_GAMES_LEN = PC98_GAMES_LEN + MAIN_GAMES_LEN + SPINOFF_SHMUP_LEN + SPINOFF_OTHER_LEN;
constexpr unsigned int GAMES_LEN = ALL_GAMES_LEN - PC98_GAMES_LEN;

static union {
LauncherGame gamesAll[ID_TH_MAX - 1] = {
    {
        .id = ID_TH01,
    },
    {
        .id = ID_TH02,
        .shots = THPRAC_GAMEROLL_TH02_SHOTTYPES,
        .shot_columns = 3,
    },
    {
        .id = ID_TH03,
        .shots = THPRAC_GAMEROLL_TH03_SHOTTYPES,
        .shot_columns = 3,
    },
    {
        .id = ID_TH04,
        .shots = THPRAC_GAMEROLL_TH04_SHOTTYPES,
        .shot_columns = 2,
    },
    {
        .id = ID_TH05,
        .shots = THPRAC_GAMEROLL_TH05_SHOTTYPES,
        .shot_columns = 4,
    },
    {
        .id = ID_TH06,
        .title = TH06_TITLE,
        .versions = gGameVersions + VER_TH06,
        .ver_count = 1,
        .shots = THPRAC_GAMEROLL_TH06_SHOTTYPES,
        .shot_columns = 2,
    },
    {
        .id = ID_TH07,
        .title = TH07_TITLE,
        .versions = gGameVersions + VER_TH07,
        .ver_count = 1,
        .shots = THPRAC_GAMEROLL_TH07_SHOTTYPES,
        .shot_columns = 2,
    },
    {
        .id = ID_TH08,
        .title = TH08_TITLE,
        .versions = gGameVersions + VER_TH08,
        .ver_count = 1,
        .shots = THPRAC_GAMEROLL_TH08_SHOTTYPES,
        .shot_columns = 4,
    },
    {
        .id = ID_TH09,
        .title = TH09_TITLE,
        .versions = gGameVersions + VER_TH09,
        .ver_count = 1,
        .shots = THPRAC_GAMEROLL_TH09_SHOTTYPES,
        .shot_columns = 4,
    },
    {
        .id = ID_TH10,
        .title = TH10_TITLE,
        .versions = gGameVersions + VER_TH10,
        .ver_count = 1,
        .shots = THPRAC_GAMEROLL_TH10_SHOTTYPES,
        .shot_columns = 3,
    },
    {
        .id = ID_TH11,
        .title = TH11_TITLE,
        .versions = gGameVersions + VER_TH11,
        .ver_count = 1,
        .shots = THPRAC_GAMEROLL_TH11_SHOTTYPES,
        .shot_columns = 3,
    },
    {
        .id = ID_TH12,
        .title = TH12_TITLE,
        .versions = gGameVersions + VER_TH12,
        .ver_count = 1,
        .shots = THPRAC_GAMEROLL_TH12_SHOTTYPES,
        .shot_columns = 2,
    },
    {
        .id = ID_TH13,
        .title = TH13_TITLE,
        .versions = gGameVersions + VER_TH13,
        .ver_count = 1,
        .appdataPath = L"%AppData%\\ShanghaiAlice\\th13",
        .shots = THPRAC_GAMEROLL_TH13_SHOTTYPES,
        .shot_columns = 4,        
    },
    {
        .id = ID_TH14,
        .title = TH14_TITLE,
        .versions = gGameVersions + VER_TH14,
        .ver_count = 1,
        .appdataPath = L"%AppData%\\ShanghaiAlice\\th14",
        .shots = THPRAC_GAMEROLL_TH14_SHOTTYPES,
        .shot_columns = 2,        
    },
    {
        .id = ID_TH15,
        .title = TH15_TITLE,
        .versions = gGameVersions + VER_TH15,
        .ver_count = 1,
        .appdataPath = L"%AppData%\\ShanghaiAlice\\th15",
        .shots = THPRAC_GAMEROLL_TH15_SHOTTYPES,
        .shot_columns = 4,        
    },
    {
        .id = ID_TH16,
        .title = TH16_TITLE,
        .versions = gGameVersions + VER_TH16,
        .ver_count = 1,
        .appdataPath = L"%AppData%\\ShanghaiAlice\\th16",
        .shots = THPRAC_GAMEROLL_TH16_SHOTTYPES,
        .shot_columns = 4,        
    },
    {
        .id = ID_TH17,
        .title = TH17_TITLE,
        .versions = gGameVersions + VER_TH17,
        .ver_count = 1,
        .appdataPath = L"%AppData%\\ShanghaiAlice\\th17",
        .shots = THPRAC_GAMEROLL_TH17_SHOTTYPES,
        .shot_columns = 3,        
    },
    {
        .id = ID_TH18,
        .title = TH18_TITLE,
        .versions = gGameVersions + VER_TH18,
        .ver_count = 1,
        .appdataPath = L"%AppData%\\ShanghaiAlice\\th18",
        .shots = THPRAC_GAMEROLL_TH18_SHOTTYPES,
        .shot_columns = 4,        
    },
    {
        .id = ID_TH19,
        .title = TH19_TITLE,
        .versions = gGameVersions + VER_TH19_V1_00A,
        .ver_count = 2,
        .appdataPath = L"%AppData%\\ShanghaiAlice\\th19",
        .shots = THPRAC_GAMEROLL_TH19_SHOTTYPES,
        .shot_columns = 4,        
    },
    {
        .id = ID_TH20,
        .title = TH20_TITLE,
        .versions = gGameVersions + VER_TH20,
        .ver_count = 1,
        .appdataPath = L"%AppData%\\ShanghaiAlice\\th20",
        .shots = THPRAC_GAMEROLL_TH20_SHOTTYPES,
        .shot_columns = 4,        
    },
    {
        .id = ID_ALCOSTG,
        .title = ALCOSTG_TITLE,
        .versions = gGameVersions + VER_ALCOSTG,
        .ver_count = 1,
    },
    {
        .id = ID_TH095,
        .title = TH095_TITLE,
        .versions = gGameVersions + VER_TH095,
        .ver_count = 1,
    },
    {
        .id = ID_TH125,
        .title = TH125_TITLE,
        .versions = gGameVersions + VER_TH125,
        .ver_count = 1,
        .shots = THPRAC_GAMEROLL_TH125_SHOTTYPES,
        .shot_columns = 2,
    },
    {
        .id = ID_TH128,
        .title = TH128_TITLE,
        .versions = gGameVersions + VER_TH128,
        .ver_count = 1,
        .appdataPath = L"%AppData%\\ShanghaiAlice\\th128",
    },
    {
        .id = ID_TH143,
        .title = TH143_TITLE,
        .versions = gGameVersions + VER_TH143,
        .ver_count = 1,
        .appdataPath = L"%AppData%\\ShanghaiAlice\\th143",
    },
    {
        .id = ID_TH165,
        .title = TH165_TITLE,
        .versions = gGameVersions + VER_TH165,
        .ver_count = 1,
        .appdataPath = L"%AppData%\\ShanghaiAlice\\th165",
    },
    {
        .id = ID_TH185,
        .title = TH185_TITLE,
        .versions = gGameVersions + VER_TH185,
        .ver_count = 1,
        .appdataPath = L"%AppData%\\ShanghaiAlice\\th185",      
    },
    {
        .id = ID_TH075,
        .title = TH075_TITLE,
        .versions = gGameVersions + VER_TH075,
        .ver_count = 1,
        .shots = THPRAC_GAMEROLL_TH075_SHOTTYPES,
        .shot_columns = 4,
    },
    {
        .id = ID_TH105,
        .title = TH105_TITLE,
        .versions = gGameVersions + VER_TH105,
        .ver_count = 1,
        .shots = THPRAC_GAMEROLL_TH105_SHOTTYPES,
        .shot_columns = 4,
    },
    {
        .id = ID_TH123,
        .title = TH123_TITLE,
        .versions = gGameVersions + VER_TH123,
        .ver_count = 1,
        .shots = THPRAC_GAMEROLL_TH123_SHOTTYPES,
        .shot_columns = 3,
    },
    {
        .id = ID_TH135,
        .title = TH135_TITLE,
        .versions = gGameVersions + VER_TH135,
        .ver_count = 1,
        .shots = THPRAC_GAMEROLL_TH135_SHOTTYPES,
        .shot_columns = 4,
    },
    {
        .id = ID_TH145,
        .title = TH145_TITLE,
        .versions = gGameVersions + VER_TH145,
        .ver_count = 1,
        .shots = THPRAC_GAMEROLL_TH145_SHOTTYPES,
        .shot_columns = 4,
    },
    {
        .id = ID_TH155,
        .title = TH155_TITLE,
        .versions = gGameVersions + VER_TH155,
        .ver_count = 1,
        .shots = THPRAC_GAMEROLL_TH155_SHOTTYPES,
        .shot_columns = 4,
    },
    {
        .id = ID_TH175,
        .title = TH175_TITLE,
        .versions = gGameVersions + VER_TH175,
        .ver_count = 1,
        .shots = THPRAC_GAMEROLL_TH175_SHOTTYPES,
        .shot_columns = 4,
    },
};
struct {
    LauncherGame pc98Games_filler[PC98_GAMES_LEN];
    LauncherGame mainGames[MAIN_GAMES_LEN];
    LauncherGame spinoffShmups[SPINOFF_SHMUP_LEN];
    LauncherGame spinoffOthers[SPINOFF_OTHER_LEN];
};
struct {
    LauncherGame pc98Games[PC98_GAMES_LEN];
    LauncherGame games[GAMES_LEN];
};
};

yyjson_doc* yyjson_read_file_report(const wchar_t* path, yyjson_read_flag flg = YYJSON_READ_JSON5, const yyjson_alc* alc_ptr = nullptr);

static bool LauncherRunGame(LauncherInstance* inst) {
    SaveSettings();

    uint32_t flags = RUN_FLAG_SKIP_IDENTIFY;
    if (inst->allow_oilp) {
        flags |= RUN_FLAG_OILP;
    }
    if (inst->allow_vpatch) {
        flags |= RUN_FLAG_VPATCH;
    }
    if (inst->apply_thprac) {
        flags |= RUN_FLAG_THPRAC;
    }

    switch (launcherSettings.after_launch) {
    case LAUNCH_MINIMIZE:
        ShowWindow(Gui::ImplWin32GetHwnd(), SW_MINIMIZE);
        break;
    case LAUNCH_CLOSE:
        PostQuitMessage(0);
        break;
    }
    
    return RunGame(utf8_to_utf16(inst->path).c_str(), nullptr, flags);
}

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
        
        
        unsigned int type = TYPE_ERROR;
        yyjson_eval_numeric(yyjson_obj_get(cur, "type"), &type);

        if (type > TYPE_UNKNOWN) {
            type = TYPE_ERROR;
        }

        const THGameVersion* ver = nullptr;
        uint8_t ver_off = 0xFF;
        if(yyjson_eval_numeric(yyjson_obj_get(cur, "ver"), &ver_off)); else {
            log_printf("Warning: instance %s has no version number, reidentifying...\r\n", path);
            goto fresh_identify;
        }
        if (game->versions + ver_off >= game->versions + VER_MAX) {
            log_printf("Warning: instance %s specifies invalid version number\r\n", path);
fresh_identify:
            ver = IdentifyExe(utf8_to_utf16(path).c_str());
            if (ver) {
                ver_off = ver - game->versions;
            } else {
                log_printf("Error: failed to identify version number for %s\r\n", path);
                continue;
            }
        }

        if (ver_off < game->ver_count) {
            ver = game->versions + ver_off;

            bool apply_thprac = false;
            if (ver->initFunc) {
                switch (launcherSettings.apply_thprac_default) {
                case APPLY_THPRAC_KEEP_STATE:
                    yyjson_eval_numeric(yyjson_obj_get(cur, "apply_thprac"), &apply_thprac);
                    break;
                case APPLY_THPRAC_DEFAULT_OPEN:
                    apply_thprac = true;
                    break;
                case APPLY_THPRAC_DEFAULT_CLOSE:
                    apply_thprac = false;
                    break;
                }
            }

            instances[valid_insts_count] = {
                .path = path,
                .name = name,
                .type = (THGameType)type,
                .ver = ver_off,
                .apply_thprac = apply_thprac,
                .allow_oilp = game->versions[ver_off].has_oilp,
                .allow_vpatch = game->versions[ver_off].has_vpatch,
            };
            valid_insts_count++;
        } else {
            log_printf("Error: version offset %d for instance %s too large\r\n", ver_off, path);
        }
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

        for (auto& game : games) {
            if (strcmp(gThGameStrs[game.id], keyReal) == 0) {
                InitLauncherGame(&game, val);
            }
        }
    }
    yyjson_doc_free(doc);
}

void SaveGamesJson() {
    yyjson_mut_doc* doc = yyjson_mut_doc_new(nullptr);
    yyjson_mut_val* root = yyjson_mut_obj(doc);
    yyjson_mut_doc_set_root(doc, root);

    for (const auto& game : games) {
        yyjson_mut_val* obj = yyjson_mut_obj_add_obj(doc, root, gThGameStrs[game.id]);
        yyjson_mut_obj_add_sint(doc, obj, "default_launch", game.default_launch);

        yyjson_mut_val* insts = yyjson_mut_obj_add_arr(doc, obj, "instances");
        for (size_t i = 0; i < game.inst_count; i++) {
            yyjson_mut_val* inst = yyjson_mut_arr_add_obj(doc, insts);
            
            yyjson_mut_obj_add_str(doc, inst, "name", game.instances[i].name);
            yyjson_mut_obj_add_int(doc, inst, "ver", game.instances[i].ver);
            yyjson_mut_obj_add_int(doc, inst, "type", game.instances[i].type);
            yyjson_mut_obj_add_str(doc, inst, "path", game.instances[i].path);
            yyjson_mut_obj_add_bool(doc, inst, "apply_thprac", game.instances[i].apply_thprac);
        }
    }
    
    size_t len = 0;
    char* buf = yyjson_mut_write(doc, YYJSON_WRITE_PRETTY, &len);

    if (buf) {
        wchar_t gamesJsonPath[MAX_PATH + 1] = {};
        memcpy(gamesJsonPath, _gConfigDir, _gConfigDirLen * sizeof(wchar_t));
        memcpy(gamesJsonPath + _gConfigDirLen, SIZED(L"games.json"));

        HANDLE hFile = CreateFileW(gamesJsonPath, GENERIC_WRITE, FILE_SHARE_READ, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        DWORD byteRet;
        WriteFile(hFile, buf, len, &byteRet, nullptr);
        CloseHandle(hFile);
        free(buf);
    }
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
        return;
    }

    ImGui::SameLine();
    Gui::TextCentered(S(game->title), ImGui::GetWindowWidth());
    ImGui::Separator();

    ImGui::TextUnformatted(S(THPRAC_GAMES_SELECT_VER));

    ImGui::BeginChild("##__game_table", { 0.0f, ImGui::GetWindowHeight() * 0.35f }, true);
    
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
            ImGui::TextUnformatted(S(TH_TYPE_SELECT[inst.type]));
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

    if (ImGui::Button(S(THPRAC_GAMES_DELETE))) {
        ImGui::OpenPopup(S(THPRAC_GAMES_DELETE_MODAL));
    }
    if (Gui::Modal(S(THPRAC_GAMES_DELETE_MODAL))) {
        ImGui::TextUnformatted(S(THPRAC_GAMES_DELETE_CONFIRM));
        switch (Gui::MultiButtonsFillWindow(0.0f, S(THPRAC_YES), S(THPRAC_NO), nullptr)) {
        case 0:
            if (game->inst_count == 1) {
                DestroyInst(game->instances + 0);
                game->instances = 0;
                game->inst_count = 0;
                selectedGame = nullptr;
                return;
            } else if (game->selected == game->inst_count - 1) {
                LauncherInstance& inst = game->instances[game->selected];
                DestroyInst(&inst);
                memset(&inst, 0, sizeof(inst));
                game->inst_count--;
                game->selected--;
            } else {
                LauncherInstance* begin = game->instances + game->selected;
                DestroyInst(begin);
                size_t elem_to_move = game->instances + game->inst_count - begin - 1;
                memmove(begin, begin + 1, elem_to_move * sizeof(LauncherInstance));
                game->inst_count--;
                game->instances[game->inst_count] = {};
            }
        case 1:
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
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

    
    auto* ver = game->versions + game->instances[game->selected].ver;

    if (ver->initFunc) {
        ImGui::Checkbox(S(THPRAC_GAMES_APPLY_THPRAC), &game->instances[game->selected].apply_thprac);
        ImGui::SameLine();
    }
    
    bool default_launch = game->default_launch == game->selected;
    if (ImGui::Checkbox(S(THPRAC_GAMES_DEFAULT_LAUNCH), &default_launch)) {
        if (default_launch) {
            game->default_launch = game->selected;
        } else {
            game->default_launch = -1;
        }
    }
    if (ver->has_oilp) {
        ImGui::Checkbox("Allow OpenInputLagPatch", &game->instances[game->selected].allow_oilp);
        ImGui::SameLine();
    }
    if (ver->has_vpatch) {
        ImGui::Checkbox("Allow Vpatch", &game->instances[game->selected].allow_vpatch);
        ImGui::SameLine();
    }
    if (ver->has_oilp && ver->has_vpatch) {
        Gui::HelpMarker("If both boxes are ticked and both Vpatch and OpenInputLagPatch are present, OpenInputLagPatch takes priority");
    } else if(ver->has_oilp || ver->has_vpatch) {
        ImGui::NewLine();
    }

    if (LargeBottomButton(S(THPRAC_GAMES_LAUNCH_GAME), 64.0f)) {
        LauncherRunGame(game->instances + game->selected);
    }

    if (Gui::Modal(S(THPRAC_GAMES_RENAME_MODAL))) {
        ImGui::InputText("", instRenameBuf, 255);
        switch (Gui::MultiButtonsFillWindow(0.0f, "OK", "Cancel", nullptr)) {
        case 0: {
            auto& inst = game->instances[game->selected];
            if (inst.name) {
                free((void*)inst.name);
            }
            inst.name = _strdup(instRenameBuf);
        }
        case 1:
            ImGui::CloseCurrentPopup();
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

    size_t exes_found = 0;
    size_t exes_scanned = 0;
    size_t text_in_progress_bar_len = 0;
    char text_in_progress_bar[MAX_PATH + 1] = {};
    
    bool abort_message = false;
    bool relative = false;

    ScanCtx() {
        InitializeCriticalSection(&found_cs);
    }
    ~ScanCtx() {
        DeleteCriticalSection(&found_cs);
    }
};

static bool GameAlreadyExists(const char* path) {
    for (const auto& game : games) {
        for (size_t i = 0; i < game.inst_count; i++) {
            if (_stricmp(game.instances[i].path, path) == 0) {
                return true;
            }
        }
    }
    return false;
}

// Pretty damn simple when you can make lots of assumptions. Not in thprac_utils.h because of those assumptions
static void PathMakeRelative(std::wstring& pathBuf, const wchar_t* relTo, size_t relTo_len) {
    size_t relTo_idx = 0;
    for (; !(relTo_idx >= relTo_len || relTo_idx >= pathBuf.length() || tolower(pathBuf[relTo_idx]) != tolower(relTo[relTo_idx])); relTo_idx++);

    // Don't modify the path if the drive letters are different.
    if (relTo_idx) {
        if ((relTo_len > relTo_idx) && (pathBuf.length() > relTo_idx)) {
            std::wstring back_chain = L"..\\";
            for (size_t i = relTo_idx; i < relTo_len; i++) {
                if (relTo[i] == L'\\') {
                    back_chain.append(L"..\\");
                }
            }
            pathBuf = back_chain + pathBuf.substr(relTo_idx - 1);
        } else if (!(relTo_len > relTo_idx) && (pathBuf.length() > relTo_idx)) {
            pathBuf.erase(0, relTo_idx + 1);
        } else {
            assert(false);
        }
    }
}

static void ScanIdentifyGame(ScanCtx* scanCtx, const wchar_t* path, size_t path_len) {
    size_t written = WideCharToMultiByte(CP_UTF8, 0, path, path_len, scanCtx->text_in_progress_bar, MAX_PATH, nullptr, nullptr);
    scanCtx->text_in_progress_bar_len = written;
    FoundGame game;
    memcpy(game.path, scanCtx->text_in_progress_bar, written);
    if (IdentifyKnownGame(game.info, game.oepCode, path)) {
        if (!GameAlreadyExists(game.path)) {
            auto& v = scanCtx->found;
            v.insert(std::upper_bound(v.begin(), v.end(), game.info.ver->gameId, [](THGameID id, FoundGame& g) -> bool {
                return id < g.info.ver->gameId;
            }), game);
        }
    }
}

static void ScanDirectory(ScanCtx* scanCtx, const wchar_t* path, std::vector<std::wstring>& found_exe_names) {
    if (scanCtx->abort_message) {
        return;
    }

    HANDLE hDir = CreateFileW(path,
        FILE_LIST_DIRECTORY | SYNCHRONIZE,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
        nullptr, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, nullptr);

    if (hDir == INVALID_HANDLE_VALUE) {
        return;
    }

    BYTE* buffer = (BYTE*)VirtualAlloc(0, 65535, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (!buffer) {
        CloseHandle(hDir);
        return;
    }

    size_t written = WideCharToMultiByte(CP_UTF8, 0, path, -1, scanCtx->text_in_progress_bar, MAX_PATH, nullptr, nullptr);
    scanCtx->text_in_progress_bar_len = written - 1;

    IO_STATUS_BLOCK iosb;
    ULONG queryFlags = SL_RESTART_SCAN;

    for (;;) {
        if (scanCtx->abort_message) {
            break;
        }
        NTSTATUS status = NtQueryDirectoryFileEx(hDir,
            nullptr, nullptr, nullptr,
            &iosb, buffer, 65535, FileDirectoryInformation, queryFlags, nullptr);
        if (status == STATUS_NO_MORE_FILES) {
            break;
        }
        if (!NT_SUCCESS(status)) {
            break;
        }

        queryFlags = 0;
        BYTE* ptr = buffer;

        for (;;) {
            auto* info = (FILE_DIRECTORY_INFORMATION*)ptr;
            std::wstring_view name(info->FileName, info->FileNameLength / sizeof(WCHAR));

            if (name != L"." && name != L"..") {
                if (scanCtx->abort_message) {
                    break;
                }

                std::wstring fullPath(path);
                if (!fullPath.ends_with(L"\\")) {
                    fullPath.push_back(L'\\');
                }
                fullPath.append(name);

                if (info->FileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                    ScanDirectory(scanCtx, fullPath.c_str(), found_exe_names);
                } else if (name.length() > 4 && (_wcsnicmp(name.data() + name.size() - 4, L".exe", 4) == 0)) {
                    if (scanCtx->relative) {
                        auto& curdir = CurrentPeb()->ProcessParameters->CurrentDirectory.DosPath;
                        PathMakeRelative(fullPath, curdir.Buffer, (curdir.Length / sizeof(WCHAR)) - 1);
                    }
                    // I originally decided to do it like this so I could have a proper progress bar
                    // but then I realized that collecting all exe names into a buffer first, then
                    // going through them all at once is twice as fast as identifying executables
                    // as they're being found.
                    // So because of this, there is a progress bar now, even though you won't get
                    // to see it for very long.
                    found_exe_names.push_back(fullPath);
                }
            }

            if (info->NextEntryOffset == 0) {
                break;
            }

            ptr += info->NextEntryOffset;
        }
    }
    VirtualFree(buffer, 0, MEM_RELEASE);
    CloseHandle(hDir);
}

static DWORD WINAPI ScanThread(LPVOID lpParam) {
    ScanCtx* scanCtx = (ScanCtx*)lpParam;

    std::vector<std::wstring> found_exe_names;
    ScanDirectory(scanCtx, scanCtx->scan_dir.c_str(), found_exe_names);

    scanCtx->exes_found = found_exe_names.size();
    for (const auto& f : found_exe_names) {
        if (scanCtx->abort_message) {
            break;
        }
        ScanIdentifyGame(scanCtx, f.data(), f.length());
        scanCtx->exes_scanned += 1;
    }

    return 0;
}

static void SteamReadLibrary(ScanCtx* scanCtx, const char* buf, size_t len, std::vector<std::wstring>& found_exe_names) {
    const char* end = buf + len;
	const char* p = buf;
start:
	p = strstr(p, "\"path\"");
	if (!p) return;

	p += strlen("\"path\"");
	while (p < end && *p != '"') p++;
	if (p >= end) return;
	p++;

	wchar_t path_buf[MAX_PATH + 1] = {};
	wchar_t* path_p = path_buf;
	wchar_t* path_end = path_buf + MAX_PATH;

	bool escape = false;
	while (p < end) {
		if (escape) {
			if (path_p >= path_end) break;
			switch (*p) {
			case 'n': *path_p++ = '\n'; break;
			case 't': *path_p++ = '\t'; break;
			case 'r': *path_p++ = '\r'; break;
			case 'b': *path_p++ = '\b'; break;
			case 'f': *path_p++ = '\f'; break;
			case '"': *path_p++ = '"';  break;
			case '\\': *path_p++ = '\\'; break;
			default:
				if (!utf8_utf16_adv(&p, end, &path_p, path_end)) {
					return;
				}
				continue;
			}
			p++;
			escape = false;
			continue;
		}
		if (*p == '\\') {
			escape = true;
			p++;
			continue;
		}
		if (*p == '"') {
			break;
		}
		if (!utf8_utf16_adv(&p, end, &path_p, path_end)) {
			return;
		}
	}

    const wchar_t suffix[] = L"\\steamapps\\common";
    size_t suffix_len = t_strlen(suffix);
    if (path_p + suffix_len < path_end) {
        memcpy(path_p, suffix, suffix_len * sizeof(wchar_t));
        ScanDirectory(scanCtx, path_buf, found_exe_names);
    }
	
	goto start;
}

static DWORD WINAPI ScanThreadSteam(LPVOID lpParam) {
    NTSTATUS err;
    
    ScanCtx* scanCtx = (ScanCtx*)lpParam;
    
    OBJECT_ATTRIBUTES keyPath;
    UNICODE_STRING keyPath_str = L"\\Registry\\Machine\\SOFTWARE\\Valve\\Steam"_wZ;
    InitializeObjectAttributes(&keyPath, &keyPath_str, 0, NULL, nullptr);

    HANDLE hKey;
    if (err = NtOpenKey(&hKey, KEY_READ, &keyPath)) {
        return 1;
    }
    // CloseHandle is just a thin wrapper around NtClose
    // TODO: Convert every call to CloseHandle to NtClose?
    defer(CloseHandle(hKey));

    KEY_VALUE_PARTIAL_INFORMATION_ALIGN64<wchar_t[MAX_PATH]> val;
    ULONG outLen;
    UNICODE_STRING InstallPath_str = L"InstallPath"_wZ;
    if (err = NtQueryValueKey(hKey, &InstallPath_str, KeyValuePartialInformationAlign64, &val, sizeof(val), &outLen)) {
        return 2;
    }

    const wchar_t lastPart[] = L"\\steamapps\\libraryfolders.vdf";
    auto lastPartSize = sizeof(lastPart);

    CHKBUF(&val, (char*)&val.Data + val.DataLength + lastPartSize, sizeof(val), 3);
    memcpy(val.Data + val.DataLength / sizeof(wchar_t) - 1, lastPart, lastPartSize);

    MappedFile f(val.Data);
    if (!f.fileMapView) {
        return 4;
    }

    std::vector<std::wstring> found_exe_names;
    SteamReadLibrary(scanCtx, (char*)f.fileMapView, f.fileSize, found_exe_names);
    scanCtx->exes_found = found_exe_names.size();
    for (const auto& f : found_exe_names) {
        if (scanCtx->abort_message) {
            break;
        }
        ScanIdentifyGame(scanCtx, f.data(), f.length());
        scanCtx->exes_scanned += 1;
    }

    return 0;
}

static LauncherGame* FindLauncherGameByID(THGameID id) {
    for (auto& game : games) {
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
        inst.ver = found[found_idx].info.ver - game->versions;
        inst.apply_thprac = found[found_idx].info.ver->initFunc && launcherSettings.apply_thprac_default == APPLY_THPRAC_DEFAULT_OPEN;

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

void FoundGamesTable(ScanCtx* scanCtx) {
    ImGui::PushID(0xF085D);
    ImGui::BeginTable("###__scan_results", 3, ImGuiTableFlags_Borders);

    ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, ImGui::GetFrameHeight());
    ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, 128.0f);

    ImGui::TableNextRow();
    ImGui::TableNextColumn();

    bool selectAll = std::all_of(scanCtx->found.begin(), scanCtx->found.end(), [](FoundGame& g) -> bool { return g.selected; });
    if (ImGui::Checkbox("###__scan_select_all", &selectAll)) {
        if (selectAll)
            for (auto& game : scanCtx->found) {
                game.selected = true;
            }
        else
            for (auto& game : scanCtx->found) {
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
        ImGui::TextUnformatted(S(TH_TYPE_SELECT[found[i].info.type]));

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
    ImGui::PopID();
}

void ProgressIndicator(float prog, const char* text, const char* textEnd = nullptr) {
    auto& style = ImGui::GetStyle();
    auto* drawList = ImGui::GetWindowDrawList();

    ImVec2 cursor = ImGui::GetCursorScreenPos();

    ImVec2 barStart = cursor;
    ImVec2 barEnd = barStart;
    barEnd.x += ImGui::GetWindowWidth();
    barEnd.y += (style.FramePadding.y * 2.0f) + ImGui::GetFontSize();

    float barWidthPx = barEnd.x - barStart.x;
    float indicatorWidthPx = barWidthPx / 4.0f;

    drawList->AddRectFilled(barStart, barEnd, ImGui::GetColorU32(ImGuiCol_FrameBg));

    ImVec2 clipStart = barStart;
    ImVec2 clipEnd = barEnd;

    ImVec2 textPos = {
        barEnd.x - (barEnd.x - barStart.x) / 2.0f,
        barStart.y + style.ItemInnerSpacing.y / 2.0f,
    };

    if (std::bit_cast<uint32_t>(prog) == 0xFFFFFFFF) {
        float barPos = fmodf((float)ImGui::GetTime() * (barWidthPx / 2), barWidthPx + indicatorWidthPx) - indicatorWidthPx;

        // barPos is negative
        if (barStart.x + barPos < barStart.x) {
            barEnd.x = barStart.x + indicatorWidthPx + barPos;
        }
        // clip indicator
        else if (barStart.x + barPos + indicatorWidthPx > barEnd.x) {
            barStart.x += barPos;
        } else {
            barStart.x += barPos;
            barEnd.x = barStart.x + indicatorWidthPx;
        }
    } else {
        barEnd.x = barStart.x + prog * barWidthPx;
    }

    drawList->AddRectFilled(barStart, barEnd, ImGui::GetColorU32(ImGuiCol_TitleBgActive));
    if (!textEnd) {
        textEnd = text + t_strlen(text);
    }

    ImVec2 textSize = ImGui::CalcTextSize(text, textEnd);
    textPos.x -= textSize.x / 2;

    drawList->PushClipRect(clipStart, clipEnd);
    drawList->AddText(textPos, ImGui::GetColorU32(ImGuiCol_Text), text, textEnd);
    drawList->PopClipRect();
}

constinit ScanCtx* scanCtx = nullptr;
static void ScanForGamesUI() {
    // WAIT_OBJECT_0: Scan thread finished.
    // WAIT_TIMEOUT: Scan thread is running
    // WAIT_FAILED: Scan thread does not exist
    DWORD scanStatus = WaitForSingleObject(scanCtx->scan_thread, 0);

    if (ImGui::Button(S(THPRAC_ABORT))) {
        scanCtx->abort_message = true;
        WaitForSingleObject(scanCtx->scan_thread, INFINITE);
        CloseHandle(scanCtx->scan_thread);
        delete scanCtx;
        scanCtx = nullptr;
        return;
    }
    ImGui::SameLine();
    Gui::TextCentered(S(THPRAC_GAMES_SCAN_FOLDER), ImGui::GetWindowWidth());

    auto& padding = ImGui::GetStyle().WindowPadding;
    float childHeight = ImGui::GetWindowHeight() - ImGui::GetCursorPosY() - ImGui::GetFrameHeight() - padding.y;

    ImGui::BeginChild(0x5CA88E6, { 0.0f, childHeight }, true);
    if (scanStatus != WAIT_FAILED) {
        if (!(scanStatus == WAIT_OBJECT_0 && scanCtx->found.size() == 0)) {
            if (scanStatus != WAIT_OBJECT_0) {
                ImGui::BeginDisabled();
            }
            FoundGamesTable(scanCtx);
            if (scanStatus != WAIT_OBJECT_0) {
                ImGui::EndDisabled();
            }
        } else {
            ImGui::SetCursorPosY(childHeight / 2);
            Gui::TextCentered("No games found", ImGui::GetWindowWidth());
        }
    }
    ImGui::EndChild();

    switch (scanStatus) {
    case WAIT_FAILED:
        ImGui::Checkbox("Use relative paths", &scanCtx->relative);
        ImGui::SameLine();
        Gui::HelpMarker("Store paths to games as paths relative to where thprac is located");
        ImGui::SameLine();
        if (Gui::ButtonRight(S(THPRAC_LINKS_EDIT_FOLDER), ImGui::GetWindowWidth()) && SelectFolder(scanCtx->scan_dir, Gui::ImplWin32GetHwnd())) {
            scanCtx->scan_thread = CreateThread(nullptr, 0, ScanThread, scanCtx, 0, nullptr);
        }
        break;
    case WAIT_TIMEOUT:
        if (scanCtx->exes_found == 0) {
            ProgressIndicator(std::bit_cast<float>(0xFFFFFFFF), scanCtx->text_in_progress_bar, scanCtx->text_in_progress_bar + scanCtx->text_in_progress_bar_len);
        } else {
            ProgressIndicator(scanCtx->exes_scanned / (float)scanCtx->exes_found, scanCtx->text_in_progress_bar, scanCtx->text_in_progress_bar + scanCtx->text_in_progress_bar_len);
        }
        break;
    case WAIT_OBJECT_0:
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
        if (Gui::ButtonRight("Finish", ImGui::GetWindowWidth())) {
            ScanResults(scanCtx->found);
            CloseHandle(scanCtx->scan_thread);
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
        scanCtx = new ScanCtx();
        scanCtx->scan_thread = CreateThread(nullptr, 0, ScanThreadSteam, scanCtx, 0, nullptr);
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
            if (game.default_launch != -1) {
                LauncherRunGame(game.instances + game.default_launch);
            } else if (launcherSettings.auto_default_launch) {
                LauncherRunGame(game.instances);
            } else {
                selectedGame = &game;
            }
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

void RandomGameSetUI(const char* id, bool* choices, unsigned int choices_len, LauncherGame* games) {
    ImGui::BeginTable(id, 6);
    ImGui::TableNextRow();
    for (size_t i = 0; i < choices_len; i++) {
        ImGui::TableNextColumn();
        ImGui::Checkbox(gThGameStrs[games[i].id], choices + i);
    }
    ImGui::EndTable();
}

static size_t ShotTypeCount(const char* shots) {
    size_t ret = 0;
    for (;;) {
        ret++;
        shots += t_strlen(shots) + 1;
        if (!*shots) {
            return ret;
        }
    }
}

static const char* ShotNameGet(const char* shots, size_t count) {
    for (size_t i = 0; i < count; i++) {
        shots += t_strlen(shots) + 1;
    }
    return shots;
}

static inline unsigned RollChoices(bool* choices, unsigned count) {
    unsigned max = 0;
    for (unsigned i = 0; i < count; i++) {
        if (choices[i]) {
            max = i + 1;
        }
    }
    if (max) for (;;) {
        unsigned roll = rand_range(max);
        if (choices[roll]) {
            return roll;
        }
    }
    return UINT_MAX;
}

static LauncherGame* randomShotGame = nullptr;
static bool shotChoices[32] = { true, true, true };
void RandomShotRollUI() {
    if (ImGui::Button("Back")) {
        toolFunc = nullptr;
    }

    ImGui::SameLine();
    Gui::TextCentered(S(THPRAC_TOOLS_RND_PLAYER), ImGui::GetWindowWidth());
    ImGui::Separator();

    if (!randomShotGame) {
        randomShotGame = gamesAll + 1;
    }

    static unsigned roll = UINT_MAX;

    // Adjust if somehow ZUN makes a game with more than 32 shot types
    // Or maybe this method of implementing random shottype choice is a bad idea
    // Or maybe this whole screen is a bad idea.
     // th02 is the default game and it only has 3 shottypes
    if (ImGui::BeginCombo(S(THPRAC_TOOLS_RND_PLAYER_GAME), gThGameStrs[randomShotGame->id])) {
        for (size_t i = 0; i < ALL_GAMES_LEN; i++) {
            if (gamesAll[i].shots == A0000ERROR_C) {
                continue;
            }

            if (ImGui::Selectable(gThGameStrs[gamesAll[i].id], randomShotGame == (gamesAll + i))) {
                randomShotGame = gamesAll + i;
                memset(shotChoices, 0, 32);
                memset(shotChoices, 1, ShotTypeCount(S(randomShotGame->shots)));
                roll = UINT_MAX;
            }
        }
        ImGui::EndCombo();
    }

    ImGui::NewLine();

    ImGui::BeginTable("###__random_shot_choice", randomShotGame->shot_columns);
    ImGui::TableNextRow();

    const char* shot_str = S(randomShotGame->shots);
    for (size_t i = 0; ; i++) {
        ImGui::TableNextColumn();
        ImGui::Checkbox(shot_str, shotChoices + i);
        
        shot_str = shot_str + t_strlen(shot_str) + 1;
        if (!*shot_str) {
            break;
        }
    }
    ImGui::EndTable();

    char buttonText[128] = {};
    if (roll != UINT_MAX) {
        snprintf(buttonText, 127, S(THPRAC_TOOLS_ROLL_RESULT), ShotNameGet(S(randomShotGame->shots), roll));
    } else {
        strcpy(buttonText, S(THPRAC_TOOLS_ROLL));
    }

    if (LargeBottomButton(buttonText, 64.0f)) {
        roll = RollChoices(shotChoices, 32);
    }

    if (randomShotGame->instances && ImGui::BeginPopupContextItem("###__roll_game_right_click")) {
        if (ImGui::Selectable(S(THPRAC_TOOLS_RND_TURNTO_GAME))) {
            selectedGame = randomShotGame;
            goToGamesPage = true;
        }
        ImGui::EndPopup();
    }
}

void RandomGameRollUI() {
    if (ImGui::Button("Back")) {
        toolFunc = nullptr;
    }

    ImGui::SameLine();
    Gui::TextCentered(S(THPRAC_TOOLS_RND_GAME), ImGui::GetWindowWidth());
    ImGui::Separator();

    static union {
        bool choices[ALL_GAMES_LEN] = {};
        struct {
            bool pc98_choice[PC98_GAMES_LEN];
            bool maingame_choice[MAIN_GAMES_LEN];
            bool spinoff_shmup_choice[SPINOFF_SHMUP_LEN];
            bool spinoff_other_choice[SPINOFF_OTHER_LEN];
        };
    };

    static unsigned roll = UINT_MAX;

    RandomGameSetUI("###__random_games_pc98", pc98_choice, PC98_GAMES_LEN, pc98Games);
    ImGui::NewLine();
    RandomGameSetUI("###__random_games_maingame", maingame_choice, MAIN_GAMES_LEN, mainGames);
    ImGui::NewLine();
    RandomGameSetUI("###__random_games_spinoff_shmup", spinoff_shmup_choice, SPINOFF_SHMUP_LEN, spinoffShmups);
    ImGui::NewLine();
    RandomGameSetUI("###__random_games_spinoff_other", spinoff_other_choice, SPINOFF_OTHER_LEN, spinoffOthers);
    ImGui::NewLine();
    
    Gui::CheckboxAll(S(THPRAC_TOOLS_RND_GAME_PC98), pc98_choice, PC98_GAMES_LEN);
    ImGui::SameLine();
    Gui::CheckboxAll(S(THPRAC_GAMES_MAIN_SERIES), maingame_choice, MAIN_GAMES_LEN);
    ImGui::SameLine();
    Gui::CheckboxAll(S(THPRAC_GAMES_SPINOFF_STG), spinoff_shmup_choice, SPINOFF_SHMUP_LEN);
    ImGui::SameLine();
    Gui::CheckboxAll(S(THPRAC_GAMES_SPINOFF_OTHERS), spinoff_other_choice, SPINOFF_OTHER_LEN);

    char buttonText[128] = {};
    if (roll != UINT_MAX) {
        snprintf(buttonText, 127, S(THPRAC_TOOLS_ROLL_RESULT), gThGameStrs[gamesAll[roll].id]);
    } else {
        strcpy(buttonText, S(THPRAC_TOOLS_ROLL));
    }

    if (LargeBottomButton(buttonText, 64.0f)) {
        roll = RollChoices(choices, ALL_GAMES_LEN);
    }

    if ((roll != UINT_MAX) && (gamesAll[roll].shots || gamesAll[roll].instances) && ImGui::BeginPopupContextItem("###__roll_game_right_click")) {
        if (gamesAll[roll].shots && ImGui::Selectable(S(THPRAC_TOOLS_RND_TURNTO_PLAYER))) {
            randomShotGame = gamesAll + roll;
            memset(shotChoices, 0, 32);
            memset(shotChoices, 1, ShotTypeCount(S(randomShotGame->shots)));
            toolFunc = RandomShotRollUI;
        }
        if (gamesAll[roll].instances && ImGui::Selectable(S(THPRAC_TOOLS_RND_TURNTO_GAME))) {
            selectedGame = gamesAll + roll;
            goToGamesPage = true;
        }
        ImGui::EndPopup();
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