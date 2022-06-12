#include "thprac_launcher_cfg.h"
#include "thprac_launcher_utils.h"
#include "thprac_launcher_wnd.h"
#include "thprac_gui_locale.h"
#include "utils/utils.h"
#include <functional>
#include <imgui.h>
#pragma warning(push)
#pragma warning(disable : 26451)
#pragma warning(disable : 26495)
#pragma warning(disable : 33010)
#include <rapidjson/document.h>
#pragma warning(pop)
#include <string>
#include <vector>

namespace THPrac {

struct LinkSelectable {
};

struct LinkLeaf : public LinkSelectable {
    std::string name;
    std::string link;
};

struct LinkNode : public LinkSelectable {
    std::string name = "";
    std::vector<LinkLeaf> leaves;
    bool isOpen = false;
};


class THLinksGui {
private:
    THLinksGui()
    {
        mGuiUpdFunc = [&]() { GuiMain(); };
        LoadLinksCfg();
    }
    SINGLETON(THLinksGui);

public:
    void GuiUpdate()
    {
        mGuiUpdFunc();
    }

private:
    enum LinkGuiTrigger {
        TRIGGER_ERROR,
        TRIGGER_ADD_LINK,
        TRIGGER_EDIT_LINK,
        TRIGGER_DELETE_LINK,
        TRIGGER_ADD_FILTER,
        TRIGGER_DELETE_FILTER,
        TRIGGER_ERR_REPETED,
        TRIGGER_ERR_EXEC,
    };
    void WriteLinksCfgDefault()
    {
        const char* debugJsonStr = u8R"123({
    "Default":{
        "__is_open__" : true,
        "Royalflare Archive":"https://maribelhearn.com/royalflare",
        "Lunarcast":"http://replay.lunarcast.net/",
        "PND":"https://thscore.pndsng.com/index.php",
        "Maribel Hearn's Touhou Portal":"https://maribelhearn.com/",
        "Eientei Forums":"https://eientei.boards.net/",
        "甜品站 (isndes)":"https://www.isndes.com/",
        "Touhou Patch":"https://www.thpatch.net/",
        "THBWiki":"https://thwiki.cc/"
    }
})123";
        rapidjson::Document linksJson;
        linksJson.Parse(debugJsonStr);

        auto& cfg = LauncherCfgGet();
        if (cfg.HasMember("links")) {
            cfg.RemoveMember("links");
        }
        JsonAddMemberA(cfg, "links", linksJson, cfg.GetAllocator());
        LauncherCfgWrite();
    }
    void WriteLinksCfg()
    {
        rapidjson::Document linksJson;
        auto& alloc = linksJson.GetAllocator();
        linksJson.SetObject();

        for (auto& node : mLinks) {
            rapidjson::Value nodeJson;
            nodeJson.SetObject();
            JsonAddMember(nodeJson, "__is_open__", node.isOpen, alloc);
            for (auto& leaf : node.leaves) {
                JsonAddMemberA(nodeJson, leaf.name.c_str(), leaf.link.c_str(), alloc);
            }
            JsonAddMemberA(linksJson, node.name.c_str(), nodeJson, alloc);
        }

        auto& cfg = LauncherCfgGet();
        if (cfg.HasMember("links")) {
            cfg.RemoveMember("links");
        }
        JsonAddMemberA(cfg, "links", linksJson, cfg.GetAllocator());
        LauncherCfgWrite();
    }
    bool LoadLinksCfg()
    {
        bool result = true;
        int filterState = 0;
        LauncherSettingGet("filter_default", filterState);

        auto& cfg = LauncherCfgGet();
        if (!cfg.HasMember("links")) {
            WriteLinksCfgDefault();
        }
        if (cfg.HasMember("links") && cfg["links"].IsObject()) {
            auto& linkRoot = cfg["links"];
            for (auto it = linkRoot.MemberBegin(); it != linkRoot.MemberEnd(); ++it) {
                if (it->value.IsObject()) {
                    LinkNode node;
                    //node.type = LINK_NODE;
                    node.name = it->name.GetString();
                    for (auto linkIt = it->value.MemberBegin(); linkIt != it->value.MemberEnd(); ++linkIt) {
                        auto isOpenFlag = !strcmp(linkIt->name.GetString(), "__is_open__");
                        if (!isOpenFlag && linkIt->value.IsString()) {
                            LinkLeaf leaf;
                            //leaf.type = LINK_LEAF;
                            leaf.name = linkIt->name.GetString();
                            leaf.link = linkIt->value.GetString();
                            node.leaves.push_back(leaf);
                        } else if (isOpenFlag && linkIt->value.IsBool()) {
                            node.isOpen = linkIt->value.GetBool();
                        }
                    }
                    if (filterState == 1) {
                        node.isOpen = true;
                    } else if (filterState == 2) {
                        node.isOpen = false;
                    }
                    mLinks.push_back(node);
                }
            }
        } else {
            if (cfg.HasMember("links")) {
                result = false;
                cfg["links"].SetObject();
            } 
        }

        WriteLinksCfg();
        return result;
    }
    std::string WrapLink(const char* link, const char* param, int type)
    {
        std::string finalLink;
        if (type == 1 && GetSuffixFromPath(link) == "exe") {
            finalLink = "\"";
            finalLink += link;
            finalLink += "\" ";
            finalLink += param;
        } else {
            finalLink = link;
        }
        return finalLink;
    }
    int ResolveLink(std::string& link, std::string& linkOut, std::string& paramOut)
    {
        auto frontPos = link.find('\"');
        auto backPos = link.rfind('\"');
        if (frontPos != std::string::npos && backPos != std::string::npos && frontPos != backPos) {
            linkOut = link.substr(frontPos + 1, backPos - frontPos - 1);
            paramOut = link.substr(backPos + 1);
            while (paramOut.size() && isblank(paramOut[0])) {
                paramOut.erase(0, 1);
            }
            return 1;
        } else {
            linkOut = link;
            paramOut = "";
            auto resStr = GetUnifiedPath(link);
            if (isalpha(resStr[0]) && resStr[1] == ':' && resStr[2] == '\\') {
                return 2;
            } else {
                return 0;
            }
        }
    }
    bool ExecLink(std::string& link)
    {
        std::string linkExec;
        std::string linkParam;
        std::wstring linkDirectoryW;
        HINSTANCE execResult;
        auto linkType = ResolveLink(link, linkExec, linkParam);
        auto linkExecW = utf8_to_utf16(linkExec);
        auto linkParamW = utf8_to_utf16(linkParam);

        switch (linkType) {
        case 1:
            linkDirectoryW = GetDirFromFullPath(linkExecW);
            execResult = ShellExecuteW(NULL, L"open", linkExecW.c_str(), linkParamW.c_str(), linkDirectoryW.c_str(), SW_SHOW);
            break;
        case 0:
        case 2:
            execResult = ShellExecuteW(NULL, NULL, linkExecW.c_str(), NULL, NULL, SW_SHOW);
            break;
        default:
            return false;
        }

        return ((DWORD)execResult > 32);
    }
    void GuiLinkEditPopupClr()
    {
        mLinkInput[0] = mLinkNameInput[0] = mLinkParamInput[0] = '\0';
        mLinkInputType = mLinkInputErr = 0;
    }
    void GuiLinkEditPopupErrTxt(int err)
    {
        switch (err) {
        case 1:
            ImGui::TextColored(ImVec4(255.0f, 0.0f, 0.0f, 255.0f), XSTR(THPRAC_LINKS_EDIT_ERR_NAME));
            break;
        case 2:
            ImGui::TextColored(ImVec4(255.0f, 0.0f, 0.0f, 255.0f), XSTR(THPRAC_LINKS_EDIT_ERR_LINK));
            break;
        case 3:
            ImGui::TextColored(ImVec4(255.0f, 0.0f, 0.0f, 255.0f), XSTR(THPRAC_LINKS_EDIT_ERR_REPEATED));
            break;
        case 4:
            ImGui::TextColored(ImVec4(255.0f, 0.0f, 0.0f, 255.0f), XSTR(THPRAC_LINKS_EDIT_ERR_RSV));
            break;
        default:
            break;
        }
    }
    int GuiLinkEditPopup()
    {
        ImGui::Text(XSTR(THPRAC_LINKS_EDIT_NAME));
        ImGui::SameLine();
        ImGui::SetNextItemWidth(-1.0f);
        if (ImGui::InputText("##__linkname_input", mLinkNameInput, sizeof(mLinkInput))) {
            if (mLinkInputErr == 1 || mLinkInputErr == 3) {
                mLinkInputErr = 0;
            }
        }
        ImGui::Text(XSTR(THPRAC_LINKS_EDIT_LINK));
        ImGui::SameLine();
        ImGui::SetNextItemWidth(-1.0f);
        if (mLinkInputType == 0) {
            if (ImGui::InputText("##__link_input", mLinkInput, sizeof(mLinkInput))) {
                if (mLinkInputErr == 2) {
                    mLinkInputErr = 0;
                }
            }
        } else {
            ImGui::Text(mLinkInput);
        }
        if (mLinkInputType == 1 && GetSuffixFromPath(mLinkInput) == "exe") {
            ImGui::Text(XSTR(THPRAC_LINKS_EDIT_PARAM));
            ImGui::SameLine();
            ImGui::SetNextItemWidth(-1.0f);
            ImGui::InputText("##__linkparam_input", mLinkParamInput, sizeof(mLinkParamInput));
        }

        GuiLinkEditPopupErrTxt(mLinkInputErr);

        if (ImGui::Button(XSTR(THPRAC_LINKS_EDIT_FILE))) {
            auto fileStr = LauncherWndFileSelect();
            if (fileStr != L"") {
                mLinkInputErr = 0;
                mLinkInputType = 1;
                mLinkParamInput[0] = '\0';
                sprintf_s(mLinkInput, "%s", utf16_to_utf8(fileStr).c_str());
            }
        }
        ImGui::SameLine();
        if (ImGui::Button(XSTR(THPRAC_LINKS_EDIT_FOLDER))) {
            auto folderStr = LauncherWndFolderSelect();
            if (folderStr != L"") {
                mLinkInputErr = 0;
                mLinkInputType = 2;
                sprintf_s(mLinkInput, "%s", utf16_to_utf8(folderStr).c_str());
            }
        }
        ImGui::SameLine();
        if (ImGui::Button(XSTR(THPRAC_LINKS_EDIT_INPUT))) {
            mLinkInputErr = 0;
            mLinkInput[0] = '\0';
            mLinkInputType = 0;
        }
        ImGui::SameLine();
        
        auto result = GuiCornerButton(XSTR(THPRAC_OK), XSTR(THPRAC_CANCEL), ImVec2(1.0f, 0.0f), true);
        if (result == 1) {
            if (mLinkNameInput[0] == '\0') {
                mLinkInputErr = 1;
            } else if (mLinkInput[0] == '\0') {
                mLinkInputErr = 2;
            } else if (!strcmp(mLinkNameInput, "__is_open__")) {
                mLinkInputErr = 4;
            } else {
                for (auto& leaf : mLinks[mCurrentNode].leaves) {
                    if (leaf.name == mLinkNameInput) {
                        mLinkInputErr = 3;
                        break;
                    }
                }
            }
            if (mLinkInputErr) {
                result = 0;
            }
        } 
        return result;
    }
    void GuiCtxMenuUpdate()
    {
        switch (mTrigger) {
        case THPrac::THLinksGui::TRIGGER_ADD_LINK:
            GuiLinkEditPopupClr();
            ImGui::OpenPopup(XSTR(THPRAC_LINKS_ADD));
            break;
        case THPrac::THLinksGui::TRIGGER_EDIT_LINK: {
            GuiLinkEditPopupClr();
            std::string linkOut;
            std::string paramOut;
            auto& leaf = mLinks[mCurrentNode].leaves[mCurrentLeaf];
            mLinkInputType = ResolveLink(leaf.link, linkOut, paramOut);
            strcpy_s(mLinkNameInput, leaf.name.c_str());
            strcpy_s(mLinkInput, linkOut.c_str());
            strcpy_s(mLinkParamInput, paramOut.c_str());
            ImGui::OpenPopup(XSTR(THPRAC_LINKS_EDIT));
        }
            break;
        case THPrac::THLinksGui::TRIGGER_DELETE_LINK:
            ImGui::OpenPopup(XSTR(THPRAC_LINKS_DELETE_MODAL));
            break;
        case THPrac::THLinksGui::TRIGGER_ADD_FILTER:
            GuiLinkEditPopupClr();
            ImGui::OpenPopup(XSTR(THPRAC_LINKS_FILTER_ADD_MODAL));
            break;
        case THPrac::THLinksGui::TRIGGER_DELETE_FILTER:
            ImGui::OpenPopup(XSTR(THPRAC_LINKS_FILTER_DEL_MODAL));
            break;
        case THPrac::THLinksGui::TRIGGER_ERR_REPETED:
            ImGui::OpenPopup(XSTR(THPRAC_LINKS_ERR_MOVE_MODAL));
            break;
        case THPrac::THLinksGui::TRIGGER_ERR_EXEC:
            ImGui::OpenPopup(XSTR(THPRAC_LINKS_ERR_EXEC_MODAL));
            break;
        default:
            break;
        }
        mTrigger = TRIGGER_ERROR;

        if (GuiModal(XSTR(THPRAC_LINKS_ADD), ImVec2(ImGui::GetIO().DisplaySize.x * 0.9f, 0.0f))) {
            auto result = GuiLinkEditPopup();
            if (result) {
                if (result == 1) {
                    auto finalLink = WrapLink(mLinkInput, mLinkParamInput, mLinkInputType);
                    LinkLeaf linkToInsert;
                    linkToInsert.name = mLinkNameInput;
                    linkToInsert.link = finalLink;
                    auto insertIdx = mLinkSelected ? mCurrentLeaf : 0;
                    mLinks[mCurrentNode].leaves.insert(mLinks[mCurrentNode].leaves.begin() + insertIdx, linkToInsert);
                    mCurrentLeaf = insertIdx;
                    mLinkSelected = nullptr;
                    WriteLinksCfg();
                }
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }

        if (GuiModal(XSTR(THPRAC_LINKS_EDIT), ImVec2(ImGui::GetIO().DisplaySize.x * 0.9f, 0.0f))) {
            auto result = GuiLinkEditPopup();
            if (result) {
                if (result == 1) {
                    auto finalLink = WrapLink(mLinkInput, mLinkParamInput, mLinkInputType);
                    auto& linkToEdit = mLinks[mCurrentNode].leaves[mCurrentLeaf];
                    linkToEdit.name = mLinkNameInput;
                    linkToEdit.link = finalLink;
                    WriteLinksCfg();
                }
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }

        if (GuiModal(XSTR(THPRAC_LINKS_DELETE_MODAL))) {
            ImGui::Text(XSTR(THPRAC_LINKS_DELETE_WARNING));
            if (GuiButtonYesNo(XSTR(THPRAC_YES), XSTR(THPRAC_NO), 6.0f)) {
                mLinks[mCurrentNode].leaves.erase(mLinks[mCurrentNode].leaves.begin() + mCurrentLeaf);
                mLinkSelected = nullptr;
                WriteLinksCfg();
            }
            ImGui::EndPopup();
        }

        if (GuiModal(XSTR(THPRAC_LINKS_FILTER_ADD_MODAL), ImVec2(ImGui::GetIO().DisplaySize.x * 0.5f, 0.0f))) {
            ImGui::Text(XSTR(THPRAC_LINKS_EDIT_NAME));
            ImGui::SameLine();
            ImGui::SetNextItemWidth(-1.0f);
            if (ImGui::InputText("##__linkname_input", mLinkNameInput, sizeof(mLinkInput))) {
                if (mLinkInputErr == 1 || mLinkInputErr == 3) {
                    mLinkInputErr = 0;
                }
            }
            GuiLinkEditPopupErrTxt(mLinkInputErr);
            auto result = GuiCornerButton(XSTR(THPRAC_OK), XSTR(THPRAC_CANCEL), ImVec2(1.0f, 0.0f), true);
            if (result == 1) {
                if (mLinkNameInput[0] == '\0') {
                    mLinkInputErr = 1;
                } else {
                    for (auto& node : mLinks) {
                        if (node.name == mLinkNameInput) {
                            mLinkInputErr = 3;
                            break;
                        }
                    }
                }
                if (!mLinkInputErr) {
                    LinkNode tmpNode;
                    tmpNode.name = mLinkNameInput;
                    tmpNode.isOpen = true;
                    mLinks.insert(mLinks.begin() + mCurrentNode, tmpNode);
                    WriteLinksCfg();
                    ImGui::CloseCurrentPopup();
                }
            } else if (result == 2) {
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }

        if (GuiModal(XSTR(THPRAC_LINKS_FILTER_DEL_MODAL))) {
            ImGui::Text(XSTR(THPRAC_LINKS_FILTER_DELETE_WARNING));
            if (GuiButtonYesNo(XSTR(THPRAC_YES), XSTR(THPRAC_NO), 6.0f)) {
                mLinks.erase(mLinks.begin() + mCurrentNode);
                mLinkSelected = nullptr;
                WriteLinksCfg();
            }
            ImGui::EndPopup();
        }

        if (GuiModal(XSTR(THPRAC_LINKS_ERR_MOVE_MODAL))) {
            ImGui::Text(XSTR(THPRAC_LINKS_ERR_MOVE));
            if (GuiCornerButton(XSTR(THPRAC_OK), nullptr, ImVec2(1.0f, 0.0f), true)) {
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }

        if (GuiModal(XSTR(THPRAC_LINKS_ERR_EXEC_MODAL))) {
            ImGui::Text(XSTR(THPRAC_LINKS_ERR_EXEC));
            if (GuiCornerButton(XSTR(THPRAC_OK), nullptr, ImVec2(1.0f, 0.0f), true)) {
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
    }
    bool GuiPopupCtxMenu(int type)
    {
        if (type) {
            if (ImGui::BeginPopupContextItem()) {
                if (type == 2) {
                    if (ImGui::Selectable(XSTR(THPRAC_LINKS_EDIT))) {
                        mTrigger = TRIGGER_EDIT_LINK;
                    }
                    if (ImGui::Selectable(XSTR(THPRAC_LINKS_DELETE))) {
                        mTrigger = TRIGGER_DELETE_LINK;
                    }
                    ImGui::Separator();
                } else {
                    if (ImGui::Selectable(XSTR(THPRAC_LINKS_FILTER_DEL))) {
                        mTrigger = TRIGGER_DELETE_FILTER;
                    }
                }
                if (ImGui::Selectable(XSTR(THPRAC_LINKS_ADD))) {
                    mTrigger = TRIGGER_ADD_LINK;
                }
                ImGui::Separator();
                if (ImGui::Selectable(XSTR(THPRAC_LINKS_FILTER_ADD))) {
                    mTrigger = TRIGGER_ADD_FILTER;
                }
                ImGui::EndPopup();
                return true;
            }
        } else {
            if (ImGui::BeginPopupContextWindow()) {
                if (ImGui::Selectable(XSTR(THPRAC_LINKS_FILTER_ADD))) {
                    mTrigger = TRIGGER_ADD_FILTER;
                }
                if (!mLinks.size()) {
                    if (ImGui::Selectable(XSTR(THPRAC_LINKS_RESET))) {
                        WriteLinksCfgDefault();
                        LoadLinksCfg();
                    }
                }
                ImGui::EndPopup();
                return true;
            }
        }
        return false;
    }
    void GuiMain()
    {
        static int moveIdx[2] = { -1, -1 };
        int destIdx[2] = { -1, -1 };
        static int filterMoveIdx = -1;
        int filterDestIdx = -1;
        if (GuiPopupCtxMenu(0)) {
            mLinkSelected = nullptr;
            mCurrentNode = mLinks.size();
        }
        if (!mLinks.size()) {
            GuiSetPosYRel(0.5f);
            GuiCenteredText(XSTR(THPRAC_GAMES_MISSING));
            return;
        }
        ImGui::Columns(2, "##@__col_links", true, true);

        int i = 0;
        for (auto& node : mLinks) {
            ImGui::SetNextItemOpen(node.isOpen, ImGuiCond_FirstUseEver);
            auto isNodeOpen = ImGui::TreeNodeEx(node.name.c_str(), mLinkSelected == &node ? ImGuiTreeNodeFlags_Selected : 0);
            if (ImGui::BeginDragDropSource()) {
                filterMoveIdx = i;
                ImGui::SetDragDropPayload("##@__dnd_linkfilter", &(moveIdx), sizeof(moveIdx));
                ImGui::EndDragDropSource();
            }
            if (ImGui::BeginDragDropTarget()) {
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("##@__dnd_linkfilter")) {
                    filterDestIdx = i;
                }
                ImGui::EndDragDropTarget();
            }
            if (ImGui::BeginDragDropTarget()) {
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("##@__dnd_linkleaf")) {
                    destIdx[0] = i;
                    destIdx[1] = 0;
                }
                ImGui::EndDragDropTarget();
            }
            if (GuiPopupCtxMenu(1)) {
                mLinkSelected = &node;
                mCurrentNode = i;
            } else if (mLinkSelected == &node) {
                mLinkSelected = nullptr;
            }
            ImGui::NextColumn();
            ImGui::NextColumn();

            if (isNodeOpen) {
                int j = 0;
                for (auto& leaf : node.leaves) {
                    auto nodeFlag = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanAvailWidth;
                    if (mLinkSelected == &leaf) {
                        nodeFlag |= ImGuiTreeNodeFlags_Selected;
                    }
                    ImGui::TreeNodeEx((void*)(intptr_t)j, nodeFlag, leaf.name.c_str());
                    if (ImGui::IsItemHovered()) {
                        if (ImGui::IsMouseDoubleClicked(0)) {
                            mLinkSelected = &leaf;
                            mCurrentNode = i;
                            mCurrentLeaf = j;
                            if (!ExecLink(leaf.link)) {
                                mTrigger = TRIGGER_ERR_EXEC;
                            }
                        } else if (ImGui::IsMouseClicked(0)) {
                            mLinkSelected = &leaf;
                            mCurrentNode = i;
                            mCurrentLeaf = j;
                        }
                    }

                    if (ImGui::BeginDragDropSource()) {
                        moveIdx[0] = i;
                        moveIdx[1] = j;
                        ImGui::SetDragDropPayload("##@__dnd_linkleaf", &(moveIdx), sizeof(moveIdx));
                        ImGui::EndDragDropSource();
                    }
                    if (ImGui::BeginDragDropTarget()) {
                        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("##@__dnd_linkleaf")) {
                            destIdx[0] = i;
                            destIdx[1] = j;
                        }
                        ImGui::EndDragDropTarget();
                    }

                    if (GuiPopupCtxMenu(2)) {
                        mLinkSelected = &leaf;
                        mCurrentNode = i;
                        mCurrentLeaf = j;
                    }

                    ImGui::NextColumn();
                    GuiColumnText(leaf.link.c_str());
                    ImGui::NextColumn();
                    j++;
                }
                ImGui::TreePop();
            }

            if (node.isOpen != isNodeOpen) {
                node.isOpen = isNodeOpen;
                WriteLinksCfg();
                mLinkSelected = nullptr;
            }
            i++;
        }
        ImGui::Columns(1);

        if (filterDestIdx != -1) {
            auto filterTmp = mLinks[filterMoveIdx];
            if (filterDestIdx > filterMoveIdx) {
                mLinks.insert(mLinks.begin() + filterDestIdx + 1, filterTmp);
                mLinks.erase(mLinks.begin() + filterMoveIdx);
            } else if (filterDestIdx < filterMoveIdx) {
                mLinks.insert(mLinks.begin() + filterDestIdx, filterTmp);
                mLinks.erase(mLinks.begin() + filterMoveIdx + 1);
            }
            mCurrentNode = filterDestIdx;
            mLinkSelected = nullptr;
            WriteLinksCfg();
        }

        if (destIdx[0] != -1 && destIdx[1] != -1) {
            auto linkTmp = mLinks[moveIdx[0]].leaves[moveIdx[1]];
            auto& srcNode = mLinks[moveIdx[0]].leaves;
            auto& destNode = mLinks[destIdx[0]].leaves;

            if (moveIdx[0] != destIdx[0]) {
                for (auto& leaf : destNode) {
                    if (leaf.name == linkTmp.name) {
                        mTrigger = TRIGGER_ERR_REPETED;
                        break;
                    }
                }
            }

            if (mTrigger != TRIGGER_ERR_REPETED) {
                if (moveIdx[0] == destIdx[0]) {
                    if (destIdx[1] > moveIdx[1]) {
                        destNode.insert(destNode.begin() + destIdx[1] + 1, linkTmp);
                        srcNode.erase(srcNode.begin() + moveIdx[1]);
                    } else {
                        destNode.insert(destNode.begin() + destIdx[1], linkTmp);
                        srcNode.erase(srcNode.begin() + moveIdx[1] + 1);
                    }
                } else {
                    destNode.insert(destNode.begin() + destIdx[1], linkTmp);
                    srcNode.erase(srcNode.begin() + moveIdx[1]);
                }
                mCurrentNode = destIdx[0];
                mCurrentLeaf = destIdx[1];
                mLinkSelected = &destNode[destIdx[1]];
                WriteLinksCfg();
            }
        }

        GuiCtxMenuUpdate();
    }

    std::function<void(void)> mGuiUpdFunc = []() {};
    std::vector<LinkNode> mLinks;

    LinkGuiTrigger mTrigger = TRIGGER_ERROR;
    LinkSelectable* mLinkSelected = nullptr;
    int mCurrentNode = 0;
    int mCurrentLeaf = 0;

    char mLinkNameInput[1024];
    char mLinkInput[1024];
    char mLinkParamInput[1024];
    int mLinkInputType = 0;
    int mLinkInputErr = 0;
};

bool LauncherLinksGuiUpd()
{
    THLinksGui::singleton().GuiUpdate();
    return true;
}
}