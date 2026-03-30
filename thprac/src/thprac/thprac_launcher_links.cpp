#define NOMINMAX
#include <Windows.h>

#include "utils/utils.h"
#include "thprac_launcher.h"
#include "thprac_gui_components.h"
#include "thprac_utils.h"

#include <string>
#include <vector>
#include <utility>

#include <yyjson.h>

namespace THPrac {
namespace Gui {
    extern HWND ImplWin32GetHwnd();
}

extern yyjson_doc* yyjson_read_file_report(const wchar_t* path, yyjson_read_flag flg = YYJSON_READ_JSON5, const yyjson_alc* alc_ptr = nullptr);

void LinksDefault(LinkSet& out) {
    out = { true, "Default", {
        { "Royalflare Archive", "https://maribelhearn.com/royalflare/" },
        { "PND's Scoreboard", "https://thscore.pndsng.com/index.php" },
        { "甜品站 (isndes)", "https://www.isndes.com/" },
        { "Lunarcast", "http://replay.lunarcast.net/" },
        { "Silent Selene", "https://www.silentselene.net/" },
        { "Maribel Hearn's Touhou Portal", "https://maribelhearn.com/" },
        { "Touhou Patch Center", "https://www.thpatch.net/" },
        { "Touhou Replay Showcase", "https://twitch.tv/touhou_replay_showcase/" },
        { "Touhou World Cup", "https://touhouworldcup.com/" },
        { "THBWiki", "https://thwiki.cc/" },
        { "Touhou Wiki (EN)", "https://en.touhouwiki.net/" }
    } };
}

void LoadLinksJson(std::vector<LinkSet>& linkSets) {
    wchar_t linksJsonPath[MAX_PATH + 1] = {};
    memcpy(linksJsonPath, _gConfigDir, _gConfigDirLen * sizeof(wchar_t));
    memcpy(linksJsonPath + _gConfigDirLen, SIZED(L"links.json"));

    yyjson_doc* doc = yyjson_read_file_report(linksJsonPath);
    if (!doc) {
        LinksDefault(linkSets.emplace_back());
        return;
    }

    yyjson_val* root = yyjson_doc_get_root(doc);
    if (!yyjson_is_obj(root)) {
        LinksDefault(linkSets.emplace_back());
        return;
    }

    size_t idx_outer, max_outer;
    yyjson_val *key_outer, *val_outer;
    yyjson_obj_foreach(root, idx_outer, max_outer, key_outer, val_outer) {
        auto& linksList = linkSets.emplace_back();
        linksList.name = unsafe_yyjson_get_str(key_outer);

        size_t idx_inner, max_inner;
        yyjson_val *key_inner, *val_inner;
        yyjson_obj_foreach(val_outer, idx_inner, max_inner, key_inner, val_inner) {
            if (!unsafe_yyjson_equals_str(key_inner, "__is_open__")) {
                if (yyjson_is_str(val_inner)) {
                    auto& linkEntry = linksList.links.emplace_back();
                    linkEntry.first = unsafe_yyjson_get_str(key_inner);
                    linkEntry.second = unsafe_yyjson_get_str(val_inner);
                }
            } else {
                yyjson_eval_numeric(val_inner, &linksList.is_open);
            }
        }
    }
    yyjson_doc_free(doc);
}

void SaveLinksJson(std::vector<LinkSet>& linkSets) {
    yyjson_mut_doc* doc = yyjson_mut_doc_new(nullptr);
    yyjson_mut_val* root = yyjson_mut_obj(doc);
    yyjson_mut_doc_set_root(doc, root);

    for (const auto& linkSet : linkSets) {
        yyjson_mut_val* linkSet_json = yyjson_mut_obj_add_obj(doc, root, linkSet.name.c_str());
        yyjson_mut_obj_add_bool(doc, linkSet_json, "__is_open__", linkSet.is_open);

        for (const auto& link : linkSet.links) {
            yyjson_mut_obj_add_str(doc, linkSet_json, link.first.c_str(), link.second.c_str());
        }
    }

    size_t len;
    char* buf = yyjson_mut_write(doc, YYJSON_WRITE_PRETTY, &len);
    if (buf) {
        wchar_t linksJsonPath[MAX_PATH + 1] = {};
        memcpy(linksJsonPath, _gConfigDir, _gConfigDirLen * sizeof(wchar_t));
        memcpy(linksJsonPath + _gConfigDirLen, SIZED(L"links.json"));

        HANDLE hFile = CreateFileW(linksJsonPath, GENERIC_WRITE, FILE_SHARE_READ, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        DWORD byteRet;
        WriteFile(hFile, buf, len, &byteRet, nullptr);
    }
}

void LinksAddSet(std::vector<LinkSet>& linkSets, size_t pos, const char* name) {
    if (pos == -1) {
        linkSets.emplace_back().name = name;
    } else {
        linkSets.emplace(linkSets.begin() + pos)->name = name;
    }
}

void LinkSetAddLink(LinkSet& set, size_t pos, const char* name, const char* link) {
    std::string_view sv = name;
    for (auto& i : set.links) {
        if (i.first == sv) {
            i.second = link;
            return;
        }
    }
    if (pos == -1) {
        set.links.emplace_back() = { name, link };
    } else {
        *set.links.emplace(set.links.begin() + pos) = { name, link };
    }
}

enum OpenWhichPopup {
    OPEN_NONE,
    OPEN_LINK_ERROR,
    OPEN_DELETE_FILTER,
    OPEN_DELETE_LINK,
    OPEN_EDIT_LINK,
    OPEN_ADD_LINK,
    OPEN_ADD_FILTER,
};

int EditLinkUI(char* linkEditTitleBuf, char* linkEditLinkBuf, bool& linkNameWarn, bool& linkLinkWarn) {
    ImGui::TextUnformatted(S(THPRAC_LINKS_EDIT_NAME));
    ImGui::SameLine();
    ImGui::InputText("##__linkname_input", linkEditTitleBuf, 1023);
    ImGui::TextUnformatted(S(THPRAC_LINKS_EDIT_LINK));
    ImGui::SameLine();
    ImGui::InputText("##__link_input", linkEditLinkBuf, 1023);

    ImGui::PushStyleColor(ImGuiCol_Text, { 1.0f, 0.0f, 0.0f, 1.0f });
    if (linkNameWarn) {
        ImGui::TextUnformatted(S(THPRAC_LINKS_EDIT_ERR_NAME));
    }
    if (linkLinkWarn) {
        ImGui::TextUnformatted(S(THPRAC_LINKS_EDIT_ERR_LINK));
    }
    ImGui::PopStyleColor();
    
    if (ImGui::Button(S(THPRAC_LINKS_EDIT_FILE))) {
        wchar_t szFile[MAX_PATH + 1] = {};

        OPENFILENAMEW ofn = {
            .lStructSize = sizeof(ofn),
            .hwndOwner = Gui::ImplWin32GetHwnd(),
            .lpstrFile = szFile,
            .nMaxFile = MAX_PATH + 1,
            .Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NODEREFERENCELINKS,
        };
        if (GetOpenFileNameW(&ofn)) {
            std::string file_u8 = utf16_to_utf8(szFile);
            strncat(linkEditLinkBuf, file_u8.c_str(), 1023);
        }
    }
    ImGui::SameLine();
    if (ImGui::Button(S(THPRAC_LINKS_EDIT_FOLDER))) {
        std::wstring folder;
        if (SelectFolder(folder, Gui::ImplWin32GetHwnd())) {
            std::string folder_u8 = utf16_to_utf8(folder.c_str());
            strncat(linkEditLinkBuf, folder_u8.c_str(), 1023);
        }
    }
    ImGui::SameLine();

    int ret = Gui::MultiButtonsRight(0.0f, S(THPRAC_CANCEL), S(THPRAC_OK), nullptr);
    if (ImGui::IsKeyPressed(ImGuiKey_Enter)) {
        ret = 1;
    }
    if (ret == 1) {
        linkNameWarn = !*linkEditTitleBuf;
        linkLinkWarn = !*linkEditLinkBuf;
        if (linkNameWarn || linkLinkWarn) {
            ret = -1;
        }
    }
    return ret;
}

void LauncherLinksMain(LauncherState* state) {
    auto& style = ImGui::GetStyle();
    ImGui::BeginChild(0x21945, { 0.0f, ImGui::GetWindowHeight() - ImGui::GetCursorPosY() - ImGui::GetFontSize() - style.WindowPadding.y - style.ItemSpacing.y - style.FramePadding.y * 2 });

    OpenWhichPopup openWhich = OPEN_NONE;

    if (ImGui::BeginPopupContextWindow()) {
        state->linkSelected = { (size_t)-1, (size_t)-1 };
        if (ImGui::Selectable(S(THPRAC_LINKS_FILTER_ADD))) {
            openWhich = OPEN_ADD_FILTER;
        }
        ImGui::EndPopup();
    }

    ImGui::BeginTable("###__links_table", 2, ImGuiTableFlags_Resizable | ImGuiTableFlags_BordersInnerV);
    for (size_t linkSetIdx = 0; linkSetIdx < state->linkSets.size(); linkSetIdx++) {
        ImGui::TableNextRow();
        ImGui::TableNextColumn();

        ImGui::SetNextItemOpen(state->linkSets[linkSetIdx].is_open);
        bool nodeOpen = ImGui::TreeNode(state->linkSets[linkSetIdx].name.c_str());
        if (ImGui::BeginPopupContextItem()) {
            state->linkSelected = { linkSetIdx, (size_t)-1 };
            if (ImGui::Selectable(S(THPRAC_LINKS_FILTER_DEL))) {
                openWhich = OPEN_DELETE_FILTER;
            }
            if (ImGui::Selectable(S(THPRAC_LINKS_ADD))) {
                openWhich = OPEN_ADD_LINK;
            }
            ImGui::Separator();
            if (ImGui::Selectable(S(THPRAC_LINKS_FILTER_ADD))) {
                openWhich = OPEN_ADD_FILTER;
            }
            ImGui::EndPopup();
        }
        if (ImGui::BeginDragDropTarget()) {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("##@__dnd_link")) {
                auto src = *(const LinksIndexes*)payload->Data;
                auto val = state->linkSets[src.linkSetIdx].links[src.linkIdx];

                if (src.linkSetIdx == linkSetIdx) {
                    auto& v = state->linkSets[linkSetIdx].links;
                    v.erase(v.begin() + src.linkIdx);
                    v.insert(v.begin(), std::move(val));
                } else {
                    for (auto& link : state->linkSets[linkSetIdx].links) {
                        if (link.first == val.first) {
                            link.second = val.second;
                            goto link_on_filter_dnd_finished;
                        }
                    }
                    state->linkSets[src.linkSetIdx].links.erase(state->linkSets[src.linkSetIdx].links.begin() + src.linkIdx);
                    state->linkSets[linkSetIdx].links.insert(state->linkSets[linkSetIdx].links.begin(), std::move(val));
                }
            }
        link_on_filter_dnd_finished:
            ImGui::EndDragDropTarget();
        }

        if (nodeOpen) {
            state->linkSets[linkSetIdx].is_open = true;
            ImGui::TableNextRow();

            for (size_t linkIdx = 0; linkIdx < state->linkSets[linkSetIdx].links.size(); linkIdx++) {
                ImGui::TableNextColumn();
                ImGui::Indent();
                ImGui::Selectable(state->linkSets[linkSetIdx].links[linkIdx].first.c_str());
                if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
                    std::wstring u16_cmd = utf8_to_utf16(state->linkSets[linkSetIdx].links[linkIdx].second.c_str());
                    if ((UINT_PTR)ShellExecuteW(Gui::ImplWin32GetHwnd(), L"open", u16_cmd.c_str(), nullptr, nullptr, SW_SHOW) < 32) {
                        openWhich = OPEN_LINK_ERROR;
                    }
                }
                ImGui::Unindent();
                if (ImGui::BeginPopupContextItem()) {
                    state->linkSelected = { linkSetIdx, linkIdx };
                    if (ImGui::Selectable(S(THPRAC_LINKS_EDIT))) {
                        openWhich = OPEN_EDIT_LINK;
                    }
                    if (ImGui::Selectable(S(THPRAC_LINKS_DELETE))) {
                        openWhich = OPEN_DELETE_LINK;
                    }
                    ImGui::Separator();
                    if (ImGui::Selectable(S(THPRAC_LINKS_ADD))) {
                        openWhich = OPEN_ADD_LINK;
                    }
                    ImGui::Separator();
                    if (ImGui::Selectable(S(THPRAC_LINKS_FILTER_ADD))) {
                        openWhich = OPEN_ADD_FILTER;
                    }
                    ImGui::EndPopup();
                }
                if (ImGui::BeginDragDropSource()) {
                    LinksIndexes payload { linkSetIdx, linkIdx };
                    ImGui::SetDragDropPayload("##@__dnd_link", &payload, sizeof(payload));
                    ImGui::TextUnformatted(state->linkSets[linkSetIdx].links[linkIdx].first.c_str());
                    ImGui::EndDragDropSource();
                }

                if (ImGui::BeginDragDropTarget()) {
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("##@__dnd_link")) {
                        auto src = *(const LinksIndexes*)payload->Data;
                        auto val = std::move(state->linkSets[src.linkSetIdx].links[src.linkIdx]);

                        if (src.linkSetIdx == linkSetIdx) {
                            auto& v = state->linkSets[linkSetIdx].links;
                            v.erase(v.begin() + src.linkIdx);
                            v.insert(v.begin() + linkIdx, val);
                        } else {
                            for (auto& link : state->linkSets[linkSetIdx].links) {
                                if (link.first == val.first) {
                                    link.second = val.second;
                                    goto link_on_link_dnd_finished;
                                }
                            }
                            state->linkSets[src.linkSetIdx].links.erase(state->linkSets[src.linkSetIdx].links.begin() + src.linkIdx);                            
                            state->linkSets[linkSetIdx].links.insert(state->linkSets[linkSetIdx].links.begin() + linkIdx + 1, std::move(val));
                        }
                    }
                    link_on_link_dnd_finished:
                    ImGui::EndDragDropTarget();
                }
                ImGui::TableNextColumn();
                ImGui::TextUnformatted(state->linkSets[linkSetIdx].links[linkIdx].second.c_str());
            }
            ImGui::TreePop();
        } else {
            state->linkSets[linkSetIdx].is_open = false;
        }
    }
    ImGui::EndTable();
    ImGui::EndChild();

    if (ImGui::Button("Expand all")) {
        for (auto& i : state->linkSets) {
            i.is_open = true;
        }
    }
    ImGui::SameLine();
    if (ImGui::Button("Collapse all")) {
        for (auto& i : state->linkSets) {
            i.is_open = false;
        }
    }

    if (openWhich != OPEN_NONE) {
        memset(state->linkEditTitleBuf, 0, 1024);
        memset(state->linkEditLinkBuf, 0, 1024);
        state->linkNameWarn = false;
        state->linkLinkWarn = false;
    }

    switch (openWhich) {
    case OPEN_LINK_ERROR:
        ImGui::OpenPopup(S(THPRAC_LINKS_ERR_EXEC_MODAL));
        break;
    case OPEN_DELETE_FILTER:
        ImGui::OpenPopup(S(THPRAC_LINKS_FILTER_DEL_MODAL));
        break;
    case OPEN_DELETE_LINK:
        ImGui::OpenPopup(S(THPRAC_LINKS_DELETE_MODAL));
        break;
    case OPEN_EDIT_LINK: {
        auto& l = state->linkSets[state->linkSelected.linkSetIdx].links[state->linkSelected.linkIdx];
        strncpy(state->linkEditTitleBuf, l.first.c_str(), 1023);
        strncpy(state->linkEditLinkBuf, l.second.c_str(), 1023);
        ImGui::OpenPopup(S(THPRAC_LINKS_EDIT_MODAL));
        } break;
    case OPEN_ADD_LINK:
        ImGui::OpenPopup(S(THPRAC_LINKS_ADD_MODAL));
        break;
    case OPEN_ADD_FILTER:
        ImGui::OpenPopup(S(THPRAC_LINKS_FILTER_ADD_MODAL));
        break;
    case OPEN_NONE:
        break;
    }

    if (Gui::Modal(S(THPRAC_LINKS_ERR_EXEC_MODAL))) {
        ImGui::TextUnformatted(S(THPRAC_LINKS_ERR_EXEC));
        if (ImGui::Button(S(THPRAC_OK), { ImGui::GetWindowWidth() - style.WindowPadding.x * 2, 0.0f })) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
    if (Gui::Modal(S(THPRAC_LINKS_FILTER_DEL_MODAL))) {
        ImGui::TextUnformatted(S(THPRAC_LINKS_FILTER_DELETE_WARNING));
        switch (Gui::MultiButtonsFillWindow(0.0f, S(THPRAC_YES), S(THPRAC_NO), nullptr)) {
        case 0:
            state->linkSets.erase(state->linkSets.begin() + state->linkSelected.linkSetIdx);
        case 1:
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
    if (Gui::Modal(S(THPRAC_LINKS_DELETE_MODAL))) {
        ImGui::TextUnformatted(S(THPRAC_LINKS_DELETE_WARNING));
        auto& v = state->linkSets[state->linkSelected.linkSetIdx].links;
        switch (Gui::MultiButtonsFillWindow(0.0f, S(THPRAC_YES), S(THPRAC_NO), nullptr)) {
        case 0:
            v.erase(v.begin() + state->linkSelected.linkIdx);
        case 1:
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }

    if(Gui::Modal(S(THPRAC_LINKS_EDIT_MODAL))) {
        switch (EditLinkUI(state->linkEditTitleBuf, state->linkEditLinkBuf, state->linkNameWarn, state->linkLinkWarn)) {
        case 1:
            state->linkSets[state->linkSelected.linkSetIdx].links[state->linkSelected.linkIdx] = { state->linkEditTitleBuf, state->linkEditLinkBuf };
        case 0:
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
    if (Gui::Modal(S(THPRAC_LINKS_ADD_MODAL))) {
        switch (EditLinkUI(state->linkEditTitleBuf, state->linkEditLinkBuf, state->linkNameWarn, state->linkLinkWarn)) {
        case 1: {
            LinkSetAddLink(state->linkSets[state->linkSelected.linkSetIdx], state->linkSelected.linkIdx, state->linkEditTitleBuf, state->linkEditLinkBuf);
        }
        case 0:
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
    if(Gui::Modal(S(THPRAC_LINKS_FILTER_ADD_MODAL))) {
        ImGui::TextUnformatted(S(THPRAC_LINKS_EDIT_NAME));
        ImGui::SameLine();
        ImGui::InputText("##__linkname_input", state->linkEditTitleBuf, 1023);

        std::string_view sv = state->linkEditTitleBuf;
        int sel;
        for (const auto& i : state->linkSets) {
            if (i.name == sv) {
                goto filter_name_collision;
            }
        }
        sel = Gui::MultiButtonsFillWindow(0.0f, S(THPRAC_OK), S(THPRAC_CANCEL), nullptr);
        if (ImGui::IsKeyPressed(ImGuiKey_Enter)) {
            sel = 1;
        }
        switch (sel) {
        case 1:
            LinksAddSet(state->linkSets, state->linkSelected.linkSetIdx, state->linkEditTitleBuf);
        case 0:
            ImGui::CloseCurrentPopup();
        }
        goto filter_add_end_popup;
    filter_name_collision:
        ImGui::TextColored({ 1.0f, 0.0f, 0.0f, 1.0f }, "%s", S(THPRAC_LINKS_EDIT_ERR_REPEATED));
    filter_add_end_popup:
        ImGui::EndPopup();
    }
}
}