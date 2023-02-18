#include "thprac_launcher_tools.h"
#include "thprac_launcher_utils.h"
#include "thprac_launcher_games.h"
#include "thprac_launcher_main.h"
#include "thprac_main.h"
#include "thprac_gui_locale.h"
#include "thprac_utils.h"
#include "utils/utils.h"
#include <functional>
#include <string>
#include <vector>

namespace THPrac {

void LauncherToolsGuiSwitch(const char* gameStr);

class THGuiRollPlayer {
public:
    THGuiRollPlayer()
    {
        mRndSeedGen = GetRndGenerator(0u, UINT_MAX);
        mRndTextGen = GetRndGenerator(1u, 20u);
        for (auto& game : gGameRoll) {
            if (game.playerSelect) {
                mGameOption.push_back(game);
            }
        }
        if (mRndTextGen() == 1) {
            mRollText = "Eeny, meeny, miny, moe!";
        } else {
            mRollText = "ROLL!";
        }
        SetPlayerOpt();
    }

    void SwitchGame(const char* gameStr)
    {
        int i = 0;
        for (auto& game : mGameOption) {
            if (!strcmp(game.name, gameStr)) {
                mGameSelected = i;
                SetPlayerOpt();
            }
            ++i;
        }
    }
    void RollPlayer()
    {
        char outputStr[256];
        std::vector<std::string> candidate;
        for (auto& player : mPlayerOption) {
            if (player.second) {
                candidate.push_back(player.first);
            }
        }
        if (candidate.size()) {
            auto rndFunc = GetRndGenerator(0u, candidate.size() - 1, mRndSeedGen());
            auto result = rndFunc();
            sprintf_s(outputStr, Gui::LocaleGetStr(THPRAC_TOOLS_ROLL_RESULT), candidate[result].c_str());
            mRollText = outputStr;
            mRollGame = mGameOption[mGameSelected].name;
        }
    }
    void SetPlayerOpt()
    {
        mPlayerOption.clear();
        mRollText = "ROLL!";
        auto playerStr = mGameOption[mGameSelected].playerSelect;
        while (playerStr[0] != '\0') {
            mPlayerOption.push_back(std::pair<std::string, bool>(playerStr, true));
            for (; playerStr[0] != '\0'; ++playerStr)
                ;
            ++playerStr;
        }
    }
    bool GuiUpdate()
    {
        bool result = true;
        if (ImGui::Button(Gui::LocaleGetStr(THPRAC_BACK))) {
            result = false;
        }
        ImGui::SameLine();
        GuiCenteredText(Gui::LocaleGetStr(THPRAC_TOOLS_RND_PLAYER));
        ImGui::Separator();

        if (ImGui::BeginCombo(Gui::LocaleGetStr(THPRAC_TOOLS_RND_PLAYER_GAME), mGameOption[mGameSelected].name, 0)) // The second parameter is the label previewed before opening the combo.
        {
            for (size_t n = 0; n < mGameOption.size(); n++) {
                bool is_selected = (mGameSelected == n);
                if (ImGui::Selectable(mGameOption[n].name, is_selected)) {
                    mGameSelected = n;
                    SetPlayerOpt();
                }
                if (is_selected) {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }
        ImGui::NewLine();

        if (mPlayerOption.size()) {
            auto& selected = mGameOption[mGameSelected];
            ImGui::Columns(selected.playerColumns, 0, false);
            for (auto& player : mPlayerOption) {
                ImGui::Checkbox(player.first.c_str(), &player.second);
                ImGui::NextColumn();
            }
            ImGui::Columns(1);
        }

        if (GuiButtonRelCentered(mRollText.c_str(), 0.9f, ImVec2(1.0f, 0.08f))) {
            RollPlayer();
        }
        if (mRollText != "ROLL!" && ImGui::BeginPopupContextItem("##roll_player_popup")) {
            if (ImGui::Selectable(Gui::LocaleGetStr(THPRAC_TOOLS_RND_TURNTO_GAME))) {
                GuiLauncherMainSwitchTab(Gui::LocaleGetStr(THPRAC_GAMES));
                LauncherGamesGuiSwitch(mRollGame.c_str());
            }
            ImGui::EndPopup();
        }

        if (!result) {
            if (mRndTextGen() == 1) {
                mRollText = "Eeny, meeny, miny, moe!";
            } else {
                mRollText = "ROLL!";
            }
        }
        return result;
    }

private:
    std::function<unsigned int(void)> mRndSeedGen;
    std::function<unsigned int(void)> mRndTextGen;
    std::vector<GameRoll> mGameOption;
    size_t mGameSelected = 0;
    std::vector<std::pair<std::string, bool>> mPlayerOption;
    std::string mRollText;
    std::string mRollGame;
};

class THGuiRollGame {
public:
    THGuiRollGame()
    {
        mRndSeedGen = GetRndGenerator(0u, UINT_MAX);
        mRndTextGen = GetRndGenerator(1u, 20u);
        for (auto& game : gGameRoll) {
            if (game.type == ROLL_MAIN) {
                game.selected = true;
            }
            mGameOption[game.type].push_back(game);
        }
        if (mRndTextGen() == 1) {
            mRollText = "Eeny, meeny, miny, moe!";
        } else {
            mRollText = "ROLL!";
        }
    }

    void RollGame()
    {
        char outputStr[256];
        std::vector<GameRoll> candidate;
        for (auto& gameType : mGameOption) {
            for (auto& game : gameType) {
                if (game.selected) {
                    candidate.push_back(game);
                }
            }
        }
        if (candidate.size()) {
            auto rndFunc = GetRndGenerator(0u, candidate.size() - 1, mRndSeedGen());
            auto result = rndFunc();
            sprintf_s(outputStr, Gui::LocaleGetStr(THPRAC_TOOLS_ROLL_RESULT), candidate[result].name);
            mRollText = outputStr;
            mRollResult = candidate[result];
        }
    }
    void GuiGameTypeChkBox(const char* text, int idx)
    {
        if (ImGui::Checkbox(text, &(mGameTypeOpt[idx]))) {
            for (auto& game : mGameOption[idx]) {
                game.selected = mGameTypeOpt[idx];
            }
        }
    }
    bool GuiUpdate()
    {
        bool result = true;
        if (ImGui::Button(Gui::LocaleGetStr(THPRAC_BACK))) {
            result = false;
        }
        ImGui::SameLine();
        GuiCenteredText(Gui::LocaleGetStr(THPRAC_TOOLS_RND_GAME));
        ImGui::Separator();

        int i = 0;
        for (auto& gameType : mGameOption) {
            bool allSelected = true;
            ImGui::Columns(6, 0, false);
            for (auto& game : gameType) {
                ImGui::Checkbox(game.name, &game.selected);
                if (!game.selected) {
                    allSelected = false;
                }
                mGameTypeOpt[i] = allSelected;
                ImGui::NextColumn();
            }
            ImGui::Columns(1);
            ImGui::NewLine();
            ++i;
        }

        ImGui::NewLine();
        GuiGameTypeChkBox(Gui::LocaleGetStr(THPRAC_TOOLS_RND_GAME_PC98), 0);
        ImGui::SameLine();
        GuiGameTypeChkBox(Gui::LocaleGetStr(THPRAC_GAMES_MAIN_SERIES), 1);
        ImGui::SameLine();
        GuiGameTypeChkBox(Gui::LocaleGetStr(THPRAC_GAMES_SPINOFF_STG), 2);
        ImGui::SameLine();
        GuiGameTypeChkBox(Gui::LocaleGetStr(THPRAC_GAMES_SPINOFF_OTHERS), 3);

        if (GuiButtonRelCentered(mRollText.c_str(), 0.9f, ImVec2(1.0f, 0.08f))) {
            RollGame();
        }
        if (mRollText != "ROLL!" && ImGui::BeginPopupContextItem("##roll_game_popup")) {
            if (mRollResult.playerSelect) {
                if (ImGui::Selectable(Gui::LocaleGetStr(THPRAC_TOOLS_RND_TURNTO_PLAYER))) {
                    mRollText = "ROLL!";
                    LauncherToolsGuiSwitch(mRollResult.name);
                }
            }
            if (ImGui::Selectable(Gui::LocaleGetStr(THPRAC_TOOLS_RND_TURNTO_GAME))) {
                GuiLauncherMainSwitchTab(Gui::LocaleGetStr(THPRAC_GAMES));
                LauncherGamesGuiSwitch(mRollResult.name);
            }
            ImGui::EndPopup();
        }

        if (!result) {
            if (mRndTextGen() == 1) {
                mRollText = "Eeny, meeny, miny, moe!";
            } else {
                mRollText = "ROLL!";
            }
        }
        return result;
    }

private:
    std::function<unsigned int(void)> mRndSeedGen;
    std::function<unsigned int(void)> mRndTextGen;
    std::vector<GameRoll> mGameOption[4];
    std::string mRollText;
    GameRoll mRollResult;
    bool mGameTypeOpt[4];
};

class THToolsGui {
private:
    THToolsGui()
    {
        mGuiUpdFunc = [&]() { return GuiContent(); };
    }
    SINGLETON(THToolsGui);

public:
    void GuiUpdate()
    {
        GuiMain();
    }
    void Switch(const char* gameStr)
    {
        mGuiRollPlayer.SwitchGame(gameStr);
        mGuiUpdFunc = [&]() { return mGuiRollPlayer.GuiUpdate(); };
    }

private:
    float GetWidthRel(const char* text, float rsv = 0.0f)
    {
        auto offset = ImGui::GetFontSize() * rsv;
        return ImGui::GetStyle().FramePadding.x * 2 + ImGui::CalcTextSize(text).x + offset;
    }
    bool CenteredButton(const char* text, float posYRel, float width)
    {
        auto columnWidth = ImGui::GetColumnWidth();
        auto columnOffset = ImGui::GetColumnOffset();

        float cursorX = (columnWidth - width) / 2.0f + columnOffset;
        ImGui::SetCursorPosX(cursorX);
        GuiSetPosYRel(posYRel);
        return ImGui::Button(text, ImVec2(width, 0.0f));
    }
    bool GuiContent()
    {
        auto width = GetWidthRel(Gui::LocaleGetStr(THPRAC_TOOLS_APPLY_THPRAC), 2.0f);
        if (CenteredButton(Gui::LocaleGetStr(THPRAC_TOOLS_APPLY_THPRAC), 0.2f, width)) {
            FindOngoingGame(true);
        }
        if (CenteredButton(Gui::LocaleGetStr(THPRAC_TOOLS_RND_GAME), 0.4f, width)) {
            mGuiUpdFunc = [&]() { return mGuiRollGame.GuiUpdate(); };
        }
        if (CenteredButton(Gui::LocaleGetStr(THPRAC_TOOLS_RND_PLAYER), 0.6f, width)) {
            mGuiUpdFunc = [&]() { return mGuiRollPlayer.GuiUpdate(); };
        }
        return true;
    }
    void GuiMain()
    {
        if (!mGuiUpdFunc()) {
            mGuiUpdFunc = [&]() { return GuiContent(); };
        }
    }

    std::function<bool(void)> mGuiUpdFunc = []() { return true; };
    THGuiRollGame mGuiRollGame;
    THGuiRollPlayer mGuiRollPlayer;
};

bool LauncherToolsGuiUpd()
{
    THToolsGui::singleton().GuiUpdate();
    return true;
}

void LauncherToolsGuiSwitch(const char* gameStr)
{
    THToolsGui::singleton().Switch(gameStr);
}
}
