#include "thprac_launcher_others.h"
#include "thprac_launcher_utils.h"
#include "thprac_launcher_games.h"
#include "thprac_launcher_main.h"
#include "thprac_launcher_cfg.h"
#include "thprac_launcher_games_def.h"
#include "thprac_main.h"
#include "thprac_gui_locale.h"
#include "thprac_utils.h"
#include "utils/utils.h"
#include <functional>
#include <string>
#include <vector>
#include <numbers>
#include <fstream>
#include <iostream>
#include <format>
#include <chrono>
#include <map>
#include "..\3rdParties\rapidcsv\rapidcsv.h"

namespace THPrac {
    void KengSave();
    void GuiDateSelector(const char* id, std::chrono::year_month_day* time)
    {
        ImGui::PushID(id);
        
        int y = (int32_t)time->year();
        int m = (unsigned)time->month();
        int d = (unsigned)time->day();
        
        ImGui::SetNextItemWidth(200.0f);
        ImGui::InputInt("##Y", &y);
        ImGui::SameLine();
        ImGui::SetNextItemWidth(180.0f);
        ImGui::InputInt("##M", &m);
        ImGui::SameLine();
        ImGui::SetNextItemWidth(180.0f);
        ImGui::InputInt("##D", &d);
        
        std::chrono::year_month_day time_new = std::chrono::year_month_day(std::chrono::year(y), std::chrono::month(m), std::chrono::day(d));
        if (time_new.ok())
            *time = time_new;

        ImGui::PopID();
    }

    typedef int DiffIndex;

    struct KengDifficulty {
        DiffIndex id;
        char name[256];

        KengDifficulty(int iid = 0, const char* nname = "")
            : id(iid)
        {
            strcpy_s(name, nname);
        }
        KengDifficulty(std::istream& is, int ver)
        {
            is.read((char*)&id, sizeof(id));
            is.read((char*)name, sizeof(name));
        }
        std::ostream& Write(std::ostream& os)
        {
            os.write((char*)&id, sizeof(id));
            os.write((char*)name, sizeof(name));
            return os;
        }
    };

    
    void static GuiSwapDiff(int idx_u, int idx_d, std::vector<KengDifficulty>& diffs, std::vector<char>& selects)
    {
        if (idx_u != -1 && idx_u >= 1) {
            std::swap(selects[idx_u], selects[idx_u - 1]);
            std::swap(diffs[idx_u], diffs[idx_u - 1]);
        }
        if (idx_d != -1 && diffs.size() >= 2 && idx_d <= diffs.size() - 2) {
            std::swap(selects[idx_d], selects[idx_d + 1]);
            std::swap(diffs[idx_d], diffs[idx_d + 1]);
        }
    }

    bool static GuiInsertDiff(int& idx_add, std::vector<KengDifficulty>& diffs, std::vector<char>& selects,int& id_tot)
    {
        static bool isopen = false;
        bool focus = (isopen == false);
        bool is_changed = false;
        if (idx_add != -1) {
            isopen = true;
            ImGui::OpenPopup(S(THPRAC_KENG_ADD_DIFF_POPUP));
        }
        if (GuiModal(S(THPRAC_KENG_ADD_DIFF_POPUP), { LauncherWndGetSize().x * 0.5f, LauncherWndGetSize().y * 0.3f }, &isopen)) {
            ImGui::PushTextWrapPos(LauncherWndGetSize().x * 0.9f);
            {
                static char diff_name[256] = { 0 };
                ImGui::Columns(2, 0, false);
                ImGui::SetColumnWidth(0, 300.0f);
                ImGui::Text(S(THPRAC_KENG_DIFF_NAME));
                ImGui::NextColumn();
                if(focus)
                    ImGui::SetKeyboardFocusHere();
                ImGui::InputText("##diff name", diff_name, sizeof(diff_name), ImGuiInputTextFlags_::ImGuiInputTextFlags_CharsNoBlank);

                ImGui::Columns(1);
                ImGui::SetCursorPosY(std::fmaxf(ImGui::GetCursorPosY(), ImGui::GetWindowHeight() - ImGui::GetTextLineHeightWithSpacing() * 1.5f));
                auto retnValue = GuiCornerButton(S(THPRAC_APPLY), S(THPRAC_CANCEL), ImVec2(1.0f, 0.0f), true);
                if (retnValue == 1 || ImGui::IsKeyDown(0xD)) {//enter
                    std::string name_input = std::string(diff_name);
                    bool find=false;
                    for (int i = 0; i < diffs.size(); i++)
                        if (!strcmp(diffs[i].name,diff_name)){
                            find = true;
                            break;
                        }
                    if (!find)
                    {
                        if (diffs.size() == 0) {
                            diffs.insert(diffs.begin() + idx_add, KengDifficulty { id_tot, diff_name });
                            id_tot++;
                            selects.insert(selects.begin() + idx_add, true);
                        } else {
                            diffs.insert(diffs.begin() + idx_add + 1, KengDifficulty { id_tot, diff_name });
                            id_tot++;
                            selects.insert(selects.begin() + idx_add + 1, true);
                        }
                        is_changed = true;
                    }
                    idx_add = -1;
                    KengSave();
                    isopen = false;
                } else if (retnValue == 2) {
                    idx_add = -1;
                    isopen = false;
                }
                if (!isopen)
                {
                    memset(diff_name, 0, sizeof(diff_name));
                    ImGui::CloseCurrentPopup();
                }
                ImGui::PopTextWrapPos();
                ImGui::EndPopup();
            }
        }
        return is_changed;
    }

    class SingleGamePlay
    {
        char mPlayName[256];
        char mPlayComment[2048];
        std::chrono::year_month_day mTimeCreate;
        std::vector<DiffIndex> mDiffsDied;
    public:
        SingleGamePlay(const char* name, const char* cmt, std::vector<DiffIndex> diffs, std::chrono::year_month_day timeCreate):
            mDiffsDied(diffs)
            , mTimeCreate(timeCreate)
        {
            strcpy_s(mPlayName, name);
            strcpy_s(mPlayComment, cmt);
        }
        SingleGamePlay(std::istream& is, int ver)
        {
            switch (ver)
            {
            case 1:
            default:
            {
                int nsz;
                is.read((char*)mPlayName, sizeof(mPlayName));
                is.read((char*)mPlayComment, sizeof(mPlayComment));
        
                int y, m, d;
                is.read((char*)&y, sizeof(y));
                is.read((char*)&m, sizeof(m));
                is.read((char*)&d, sizeof(d));
                mTimeCreate = std::chrono::year_month_day(std::chrono::year(y), std::chrono::month(m), std::chrono::day(d));
        
                is.read((char*)&nsz, sizeof(nsz));
                mDiffsDied.resize(nsz, 0);
                for (int i = 0; i < nsz; i++) {
                    is.read((char*)&(mDiffsDied[i]), sizeof(mDiffsDied[i]));
                }
            }
            }
        }

        char* GetTimeDesc(){
            static char chs[256] = { 0 };
            sprintf_s(chs, S(THPRAC_OTHER_TODAY), mTimeCreate.year(),mTimeCreate.month(), mTimeCreate.day());
            return chs;
        }

        
        const char* GetDescription_Line()
        {
            static char mCmtLine[64];
            memcpy_s(mCmtLine, sizeof(mCmtLine), mPlayComment, sizeof(mCmtLine));
            bool add_dots = false;
            for (int i = 0; i < 25; i++) {
                if (mCmtLine[i] == '\r' || mCmtLine[i] == '\n') {
                    mCmtLine[i] = '.';
                    mCmtLine[i + 1] = '.';
                    mCmtLine[i + 2] = '.';
                    mCmtLine[i + 3] = '\0';
                    add_dots = true;
                    break;
                } else if (mCmtLine[i] == '\0') {
                    add_dots = true;
                    break;
                }
            }
            if (!add_dots) {
                mCmtLine[24] = '.';
                mCmtLine[25] = '.';
                mCmtLine[26] = '.';
                mCmtLine[27] = '\0';
            }
            return mCmtLine;
        }

        void DrawPlay(bool* is_del, bool* is_open, std::vector<KengDifficulty>& diffs, int& diffs_id_tot, std::function<void(int)> remove_diff)
        {
            // keng draw
            ImGui::Columns(3, 0, false);
            ImGui::SetColumnWidth(0, LauncherWndGetSize().x * 0.05f);
            ImGui::SetColumnWidth(1, 300.0f);

            ImGui::NextColumn();
            ImGui::Text(S(THPRAC_KENG_PLAY_NAME));
            ImGui::NextColumn();
            ImGui::InputText("##playname", mPlayName,sizeof(mPlayName));
            ImGui::NextColumn();

            ImGui::NextColumn();
            ImGui::Text(S(THPRAC_KENG_PLAY_DATE));
            ImGui::NextColumn();
            GuiDateSelector("##date", &mTimeCreate);
            ImGui::NextColumn();

            ImGui::NextColumn();
            ImGui::Text(S(THPRAC_KENG_PLAY_CMT));
            ImGui::NextColumn();
            ImGui::InputTextMultiline("##playcmt", mPlayComment,sizeof(mPlayComment));
            ImGui::Columns(1);

            ImGui::NewLine();
            ImGui::Separator();

            // diffs draw
            static std::vector<char> diffs_select;
            diffs_select.resize(diffs.size(),false);
            for (auto& i : diffs_select)
                i = false;
            for (auto& id : mDiffsDied){
                for (int j = 0; j < diffs.size(); j++){
                    if (diffs[j].id == id){
                        diffs_select[j] = true;
                        break;
                    }
                }
            }
            
            ImGui::SetCursorPosX(LauncherWndGetSize().x * 0.05f);
            static int idx_add = -1;
            static int idx_changename = -1;
            int idx_u = -1, idx_d = -1, idx_rem = -1;
            bool is_changed = false;
            if (ImGui::BeginTable("##diffTable", 2, ImGuiTableFlags_::ImGuiTableFlags_Borders | ImGuiTableFlags_::ImGuiTableFlags_Resizable, ImVec2(LauncherWndGetSize().x * 0.9f, 0.0f)))
            {
                ImGui::TableSetupColumn(S(THPRAC_KENG_DIFF_TABLE_CK), 0, ImGui::GetTextLineHeight() * 10);
                ImGui::TableSetupColumn(S(THPRAC_KENG_DIFF_TABLE_NAME), 0, LauncherWndGetSize().x * 0.9f - ImGui::GetTextLineHeight() * 10);
                ImGui::TableHeadersRow();
                if (diffs.size() == 0) {
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    if (ImGui::Button(S(THPRAC_KENG_DIFF_TABLE_ADD)))
                        idx_add = 0;
                }
                for (int idx = 0; idx < diffs.size(); idx++) {
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    if (ImGui::Button(std::format("{}##diff_add_{}", S(THPRAC_KENG_DIFF_TABLE_ADD), idx).c_str()))
                        idx_add = idx;
                    ImGui::SameLine();
                    if (ImGui::Button(std::format("{}##diff_rem_{}", S(THPRAC_KENG_DIFF_TABLE_REM), idx).c_str()))
                        idx_rem = idx;
                    ImGui::SameLine();
                    // u
                    if (ImGui::Button(std::format("{}##diff_u_{}", S(THPRAC_KENG_DIFF_TABLE_U), idx).c_str()))
                        idx_u = idx;
                    ImGui::SameLine();
                    // d
                    if (ImGui::Button(std::format("{}##diff_d_{}", S(THPRAC_KENG_DIFF_TABLE_D), idx).c_str()))
                        idx_d = idx;
                    ImGui::SameLine();
                    is_changed |= ImGui::Checkbox(std::format("##diff{}", idx).c_str(), (bool*)(&diffs_select[idx]));
                    ImGui::TableNextColumn();
                    if(ImGui::Selectable(std::format("{}##sel{}", diffs[idx].name, idx).c_str()))
                        idx_changename=idx;
                }
                ImGui::EndTable();
            }else{
                idx_add = -1;
            }
            {
                static bool isopen_changename = false;
                static char diffname[256];
                bool focus = (isopen_changename == false);
                if (idx_changename != -1) {
                    if (!isopen_changename) {
                        strcpy_s(diffname, diffs[idx_changename].name);
                    }
                    isopen_changename = true;
                    ImGui::OpenPopup(std::format("{}##diffname", S(THPRAC_KENG_DIFF_NAME)).c_str());
                }
                if (GuiModal(std::format("{}##diffname", S(THPRAC_KENG_DIFF_NAME)).c_str(), { LauncherWndGetSize().x * 0.5f, LauncherWndGetSize().y * 0.3f }, &isopen_changename)) {
                    ImGui::PushTextWrapPos(LauncherWndGetSize().x * 0.9f);
                    {
                        ImGui::Columns(2, 0, false);
                        ImGui::SetColumnWidth(0, 300.0f);
                        ImGui::Text(S(THPRAC_KENG_DIFF_NAME));
                        ImGui::NextColumn();
                        if (focus)
                            ImGui::SetKeyboardFocusHere();
                        ImGui::InputText("##diffnameip", diffname, sizeof(diffname), ImGuiInputTextFlags_::ImGuiInputTextFlags_CharsNoBlank);

                        ImGui::Columns(1);
                        ImGui::SetCursorPosY(std::fmaxf(ImGui::GetCursorPosY(), ImGui::GetWindowHeight() - ImGui::GetTextLineHeightWithSpacing() * 1.5f));
                        if (ImGui::Button(S(THPRAC_APPLY)) || ImGui::IsKeyDown(0xD)) { // enter
                            bool find = false;
                            for (int i = 0; i < diffs.size(); i++)
                                if (!strcmp(diffs[i].name, diffname)) {
                                    find = true;
                                    break;
                                }
                            if (!find) {
                                strcpy_s(diffs[idx_changename].name, diffname);
                            }
                            KengSave();
                            isopen_changename = false;
                        }
                        if (!isopen_changename) {
                            ImGui::CloseCurrentPopup();
                            idx_changename = -1;
                        }
                        ImGui::PopTextWrapPos();
                        ImGui::EndPopup();
                    }
                }
                if (!isopen_changename)
                    idx_changename = -1;
            }
            GuiSwapDiff(idx_u, idx_d, diffs, diffs_select);
            is_changed |= GuiInsertDiff(idx_add, diffs, diffs_select, diffs_id_tot);
            if (idx_rem != -1){
                remove_diff(idx_rem);
            }
            if (is_changed){
                std::vector<int> ids;
                for (int i = 0; i < diffs.size(); i++) {
                    if (diffs_select[i])
                        ids.push_back(diffs[i].id);
                }
                this->mDiffsDied = ids;
            }

            ImGui::SetCursorPosX(LauncherWndGetSize().x * 0.05f);
            if (ImGui::Button(S(THPRAC_KENG_RETURN))) {
                *is_open = false;
            }
            ImGui::SameLine();
            if (ImGui::Button(S(THPRAC_KENG_DEL))) {
                ImGui::OpenPopup(S(THPRAC_KENG_DEL_POPUP));
            }
            if (GuiModal(S(THPRAC_KENG_DEL_POPUP), { LauncherWndGetSize().x * 0.3f, LauncherWndGetSize().y * 0.3f })) {
                ImGui::PushTextWrapPos(LauncherWndGetSize().x * 0.9f);
                {
                    ImGui::Text(S(THPRAC_KENG_DEL_YES_OR_NO));
                    ImGui::SetCursorPosY(std::fmaxf(ImGui::GetCursorPosY(), ImGui::GetWindowHeight() - ImGui::GetTextLineHeightWithSpacing() * 1.5f));
                    auto retnValue = GuiCornerButton(S(THPRAC_APPLY), S(THPRAC_CANCEL), ImVec2(1.0f, 0.0f), true);
                    if (retnValue == 1) {
                        *is_del = true;
                        *is_open = false;
                        ImGui::CloseCurrentPopup();
                    } else if (retnValue == 2) {
                        ImGui::CloseCurrentPopup();
                    }
                    ImGui::PopTextWrapPos();
                    ImGui::EndPopup();
                }
            }
        }
        std::ostream& Write(std::ostream& os)
        {
            os.write((char*)mPlayName, sizeof(mPlayName));
            os.write((char*)mPlayComment, sizeof(mPlayComment));

            int y = (int32_t)mTimeCreate.year();
            int m = (unsigned)mTimeCreate.month();
            int d = (unsigned)mTimeCreate.day();
            os.write((char*)&y, sizeof(y));
            os.write((char*)&m, sizeof(m));
            os.write((char*)&d, sizeof(d));

            int nsz = mDiffsDied.size();
            os.write((char*)&nsz, sizeof(nsz));
            for (auto& id : mDiffsDied)
                os.write((char*)&id, sizeof(id));
            return os;
        }
        friend class Keng;
    };


    class Keng
    {
        char mKengName[256];
        char mKengDescription[2048];
        std::chrono::year_month_day mTimeCreate;

        std::vector<KengDifficulty> mKengDifficulties;
        int mDifficulties_idtot=0;
        std::vector<SingleGamePlay> mPlays;
    public:
        char* GetTimeDesc()
        {
            static char chs[256] = { 0 };
            sprintf_s(chs, S(THPRAC_OTHER_TODAY), mTimeCreate.year(), mTimeCreate.month(), mTimeCreate.day());
            return chs;
        }

        void GuiGetDetails()
        {
            static std::map<int, int> diffs_die_count;
            static std::vector<double> probs_pass_vec;
            static std::vector<float> pass_rate;
            static bool isopen = false;
            if (ImGui::Button(S(THPRAC_KENG_DETAILS))) {
                isopen = true;

                diffs_die_count = {};
                probs_pass_vec = {};
                pass_rate = {};
                for (auto& play : mPlays) {
                    for (auto& diff : play.mDiffsDied) {
                        if (diffs_die_count.contains(diff))
                            diffs_die_count[diff]++;
                        else
                            diffs_die_count[diff] = 1;
                    }
                }
                double play_time = mPlays.size();
                probs_pass_vec.resize(mKengDifficulties.size(), 0.0);
                for (int i = 0; i < mKengDifficulties.size(); i++) {
                    if (diffs_die_count.contains(mKengDifficulties[i].id)) {
                        probs_pass_vec[i] = 1.0f - (double)diffs_die_count[mKengDifficulties[i].id] / std::max(1.0, play_time);
                    }
                }
                auto CalProbForN = [](std::vector<double>& probs_single) -> std::vector<double> {
                    // p[n][m]:  for first n singles, catchs m
                    // p[n][0]
                    // p[n][m] = p[n-1][m-1]*prob_single[n] + p[n-1][m]
                    // n: 1..N, m: 0..n
                    std::vector<std::vector<double>> p;
                    p.push_back({ 1.0 }); // init  p[0][n]
                    for (int i = 1; i <= probs_single.size(); i++) {
                        p.push_back(std::vector<double>());
                        p[i].resize(static_cast<size_t>(i) + 1, -1); // init p[i][m]
                    }
                    for (int i = 1; i <= probs_single.size(); i++) {
                        // cal p[n][0]
                        p[i][0] = 1.0;
                        for (int j = 1; j <= i; j++) {
                            p[i][0] *= (1 - probs_single[j - 1]);
                        }
                    }
                    p[0][0] = 1.0; // init p[0][0]
                    for (int i = 1; i <= probs_single.size(); i++) {
                        for (int j = 1; j <= i; j++) {
                            p[i][j] = p[i - 1][j - 1] * probs_single[i - 1] + (j <= i - 1 ? p[i - 1][j] * (1 - probs_single[i - 1]) : 0.0);
                        }
                    }
                    return p[p.size() - 1];
                };
                auto probs = CalProbForN(probs_pass_vec);
                std::reverse(probs.begin(), probs.end());
                pass_rate.resize(probs.size());
                int miss = 0;
                double tot = 0.0;
                for (double u : probs) {
                    tot = tot + u;
                    pass_rate[miss] = tot;
                    miss++;
                }
                ImGui::OpenPopup(S(THPRAC_KENG_DETAILS_POPUP));
            }
            
            if (GuiModal(S(THPRAC_KENG_DETAILS_POPUP), { LauncherWndGetSize().x * 0.95f, LauncherWndGetSize().y * 0.8f }, &isopen)) {
                ImGui::PushTextWrapPos(LauncherWndGetSize().x * 0.9f);
                ImGui::SetCursorPosX(ImGui::GetWindowWidth() * 0.05f);
                if (ImGui::Button(S(THPRAC_KENG_RETURN)))
                    isopen = false;
                ImGui::SetCursorPosX(ImGui::GetWindowWidth() * 0.05f);
                if (ImGui::BeginTable(S(THPRAC_KENG_DETAILS_DIFF_TABLE), 4, ImGuiTableFlags_::ImGuiTableFlags_Borders | ImGuiTableFlags_::ImGuiTableFlags_Resizable, ImVec2(ImGui::GetWindowWidth() * 0.9f, 0.0f))) {
                    ImGui::TableSetupColumn(S(THPRAC_KENG_DETAILS_DIFF_INDEX), 0, ImGui::GetWindowWidth() * 0.15f);
                    ImGui::TableSetupColumn(S(THPRAC_KENG_DETAILS_DIFF_NAME), 0, ImGui::GetWindowWidth() * 0.4f);
                    ImGui::TableSetupColumn(S(THPRAC_KENG_DETAILS_DIFF_CNT), 0, ImGui::GetWindowWidth() * 0.2f);
                    ImGui::TableSetupColumn(S(THPRAC_KENG_DETAILS_DIFF_PASSRATE), 0, ImGui::GetWindowWidth() * 0.25f);
                    ImGui::TableHeadersRow();
                    int n = 1;
                    for (auto& diff : mKengDifficulties) {
                        ImGui::TableNextRow();
                        ImGui::TableNextColumn();
                        ImGui::Text("%d", n);
                        ImGui::TableNextColumn();
                        ImGui::Text("%s", diff.name);
                        ImGui::TableNextColumn();
                        ImGui::Text("%6d", diffs_die_count[diff.id]);
                        ImGui::TableNextColumn();
                        float r, g, b;
                        double passrate = (1.0 - (double)diffs_die_count[diff.id] / (std::max(1.0, (double)mPlays.size())));
                        passrate = std::clamp(passrate, 0.0, 1.0);
                        ImGui::ColorConvertHSVtoRGB(passrate * passrate * passrate * passrate * 0.33f, 0.75f, 1.0f, r, g, b);
                        ImGui::TextColored({r,g,b,1.0f}, "%6.2lf%%", passrate * 100.0);
                        n++;
                    }
                    ImGui::EndTable();

                    ImGui::NewLine();
                    ImGui::Separator();
                    ImGui::SetCursorPosX(LauncherWndGetSize().x * 0.05f);
                    if (ImGui::BeginTable(S(THPRAC_KENG_DETAILS_PASS_TABLE), 2, ImGuiTableFlags_::ImGuiTableFlags_Borders | ImGuiTableFlags_::ImGuiTableFlags_Resizable, ImVec2(ImGui::GetWindowWidth() * 0.9f, 0.0f)))
                    {
                        ImGui::TableSetupColumn(S(THPRAC_KENG_DETAILS_PASS_MISS), 0, ImGui::GetWindowWidth() * 0.15f);
                        ImGui::TableSetupColumn(S(THPRAC_KENG_DETAILS_PASS_PROB), 0, ImGui::GetWindowWidth() * 0.85f);
                        ImGui::TableHeadersRow();

                        for (int miss = 0; miss < pass_rate.size(); miss++) {
                            ImGui::TableNextRow();
                            ImGui::TableNextColumn();
                            ImGui::Text("%6d", miss);
                            ImGui::TableNextColumn();
                            ImGui::Text("%10.6lf%%", pass_rate[miss] * 100.0);
                        }
                        ImGui::EndTable();
                    }
                }
                ImGui::SetCursorPosX(LauncherWndGetSize().x * 0.05f);
                if (ImGui::Button(std::format("{}##2",S(THPRAC_KENG_RETURN)).c_str()))
                    isopen = false;
                if (!isopen)
                    ImGui::CloseCurrentPopup();
                ImGui::PopTextWrapPos();
                ImGui::EndPopup();
            }
        }

        const char* GetDescription_Line()
        {
            static char mKengDescription_line[64];
            memcpy_s(mKengDescription_line, sizeof(mKengDescription_line), mKengDescription, sizeof(mKengDescription_line));
            bool add_dots=false;
            for (int i = 0; i < 25; i++)
            {
                if (mKengDescription_line[i] == '\r' || mKengDescription_line[i] == '\n'){
                    mKengDescription_line[i] = '.';
                    mKengDescription_line[i+1] = '.';
                    mKengDescription_line[i+2] = '.';
                    mKengDescription_line[i+3] = '\0';
                    add_dots = true;
                    break;
                } else if (mKengDescription_line[i] == '\0') {
                    add_dots = true;
                    break;
                }
            }
            if (!add_dots)
            {
                mKengDescription_line[24] = '.';
                mKengDescription_line[25] = '.';
                mKengDescription_line[26] = '.';
                mKengDescription_line[27] = '\0';
            }
            return mKengDescription_line;
        }

        void WriteToCsv(std::wstring csv_filename)
        {
            mTimeCreate = std::chrono::year_month_day(std::chrono::floor<std::chrono::days>(std::chrono::system_clock::now()));
            rapidcsv::Document doc;
            doc.SetColumnName(0, "name");
            doc.SetColumnName(1, "date");
            doc.SetColumnName(2, "comment");
            //std::vector<std::string> header;
            //header.push_back("name");
            //header.push_back("date");
            //header.push_back("comment");
            for (int i = 0; i < mKengDifficulties.size(); i++)
                doc.SetColumnName(i + 3, mKengDifficulties[i].name);
                //header.push_back(mKengDifficulties[i].name);
            //doc.SetRow(0, header);
            for (int j = 0; j < mPlays.size(); j++){
                auto &play = mPlays[j];
                std::vector<std::string> row;
                row.push_back(play.mPlayName);
                row.push_back(std::format("{}/{}/{}", (int32_t)play.mTimeCreate.year(), (unsigned)(play.mTimeCreate.month()), (unsigned)(play.mTimeCreate.day())));
                row.push_back(play.mPlayComment);
                for (int df = 0; df < mKengDifficulties.size(); df++){
                    bool is_died = false;
                    for (int k = 0; k < play.mDiffsDied.size(); k++){
                        if (mKengDifficulties[df].id == play.mDiffsDied[k]){
                            row.push_back("1");
                            is_died = true;
                            break;
                        }
                    }
                    if (!is_died)
                        row.push_back("0");
                }
                doc.SetRow(j, row);
            }
            doc.Save(utf16_to_mb(csv_filename.c_str(), CP_ACP));
        }

        Keng(std::wstring csv_filename)
        {
            std::string name = utf16_to_mb(GetNameFromFullPath(csv_filename).c_str(), CP_UTF8);
            strcpy_s(mKengName, name.c_str());
            mTimeCreate = std::chrono::year_month_day(std::chrono::floor<std::chrono::days>(std::chrono::system_clock::now()));
            memset(mKengDescription, 0, sizeof(mKengDescription));

            rapidcsv::Document doc(utf16_to_mb(csv_filename.c_str(), CP_ACP), rapidcsv::LabelParams(-1, -1));
            int idx_row = 0;
            std::vector<std::string> header = doc.GetRow<std::string>(idx_row);
            
            int diff_count = header.size() - 3;
            for (int i = 0; i < diff_count; i++){
                mKengDifficulties.emplace_back(i, header[i+3].c_str());
            }
            mDifficulties_idtot = diff_count;
            for (int i = 1; i < doc.GetRowCount();i++){
                std::vector<std::string> rw = doc.GetRow<std::string>(i);
                std::string name = rw[0];
                std::string date = rw[1];
                std::string cmt = rw[2];
                std::vector<int> ids;
                for (int i = 0; i < std::min((int)(rw.size()) - 3, diff_count); i++){
                    if (rw[i + 3] == "1")
                        ids.push_back(i);
                }
                int y, m, d;
                std::chrono::year_month_day time_play;
                if (sscanf_s(date.c_str(), "%d/%d/%d", &y, &m, &d) == 3){
                    time_play = std::chrono::year_month_day(std::chrono::year(y), std::chrono::month(m), std::chrono::day(d));
                }else{
                    time_play = std::chrono::year_month_day(std::chrono::floor<std::chrono::days>(std::chrono::system_clock::now()));
                }
                mPlays.emplace_back(name.c_str(), cmt.c_str(), ids, time_play);
            }
        }
        Keng(const char* name, const char* desc, std::chrono::year_month_day t):mTimeCreate(t)
        {
            strcpy_s(mKengName, name);
            strcpy_s(mKengDescription, desc);
        }
        Keng(std::istream& is, int ver)
        {
            switch (ver)
            {
            case 1:
            default:
            {
                is.read((char*)mKengName, sizeof(mKengName));
                is.read((char*)mKengDescription, sizeof(mKengDescription));
        
                int y,m,d;
                is.read((char*)&y, sizeof(y));
                is.read((char*)&m, sizeof(m));
                is.read((char*)&d, sizeof(d));
                mTimeCreate = std::chrono::year_month_day(std::chrono::year(y), std::chrono::month(m), std::chrono::day(d));
        
                int nsz = mKengDifficulties.size();
                is.read((char*)&nsz, sizeof(nsz));
                for (int i = 0; i < nsz;i++) {
                   mKengDifficulties.emplace_back(is, ver);
                }
                is.read((char*)&mDifficulties_idtot, sizeof(mDifficulties_idtot));
        
                is.read((char*)&nsz, sizeof(nsz));
                for (int i = 0; i < nsz; i++) {
                    mPlays.emplace_back(is,ver);
                }
            }
            }
        }
        std::ostream& Write(std::ostream& os)
        {
            os.write((char*)mKengName, sizeof(mKengName));
            os.write((char*)mKengDescription, sizeof(mKengDescription));

            int y = (int32_t)mTimeCreate.year();
            int m = (unsigned)mTimeCreate.month();
            int d = (unsigned)mTimeCreate.day();
            os.write((char*)&y, sizeof(y));
            os.write((char*)&m, sizeof(m));
            os.write((char*)&d, sizeof(d));

            int nsz = mKengDifficulties.size();
            os.write((char*)&nsz, sizeof(nsz));
            for (auto& i : mKengDifficulties){
                i.Write(os);
            }
            os.write((char*)&mDifficulties_idtot, sizeof(mDifficulties_idtot));

            nsz = mPlays.size();
            os.write((char*)&nsz, sizeof(nsz));
            for (auto& i : mPlays) {
                i.Write(os);
            }
            return os;
        }

        void RemoveDiffs(int idx)
        {
            for (auto& i : mPlays)
            {
                for (auto& diff_id : i.mDiffsDied)
                {
                    if (diff_id == mKengDifficulties[idx].id){
                        return;//can not remove
                    }
                }
            }
            mKengDifficulties.erase(mKengDifficulties.begin() + idx);
            return;
        }

        void DrawKeng(bool* is_del,bool* is_open)
        {
            //keng draw
            static int play_index_draw = -1;
            if (play_index_draw != -1)
            {
                bool is_del2 = false, is_open2 = true;
                mPlays[play_index_draw].DrawPlay(&is_del2, &is_open2, mKengDifficulties, mDifficulties_idtot, [=](int idx) { RemoveDiffs(idx); });
                if (is_del2) {
                    mPlays.erase(mPlays.begin() + play_index_draw);
                    is_open2 = false;
                    play_index_draw = -1;
                }
                if (is_open2 == false)
                {
                    play_index_draw = -1;
                }
            }else {
                ImGui::SetCursorPosX(ImGui::GetWindowWidth() * 0.05f);
                if (ImGui::Button(S(THPRAC_KENG_RETURN)))
                    *is_open = false;
                ImGui::Columns(3, 0, false);
                ImGui::SetColumnWidth(0, LauncherWndGetSize().x * 0.05f);
                ImGui::SetColumnWidth(1, 300.0f);

                ImGui::NextColumn();
                ImGui::Text(S(THPRAC_KENG_NAME));
                ImGui::NextColumn();
                ImGui::InputText("##keng name", mKengName, sizeof(mKengName));
                ImGui::NextColumn();

                ImGui::NextColumn();
                ImGui::Text(S(THPRAC_KENG_DATE));
                ImGui::NextColumn();
                GuiDateSelector("##keng date", &mTimeCreate);
                ImGui::NextColumn();

                ImGui::NextColumn();
                ImGui::Text(S(THPRAC_KENG_DESC));
                ImGui::NextColumn();
                ImGui::InputTextMultiline("##keng desc", mKengDescription, sizeof(mKengDescription));
                //ImGui::TextWrapped(mKengDescription.c_str());
                ImGui::Columns(1);

                ImGui::NewLine();
                ImGui::Separator();
                // play draw
                ImGui::SetCursorPosX(LauncherWndGetSize().x * 0.05f);
                if (ImGui::BeginTable(S(THPRAC_KENG_PLAY_TABLE), 5, ImGuiTableFlags_::ImGuiTableFlags_Borders | ImGuiTableFlags_::ImGuiTableFlags_Resizable, ImVec2(LauncherWndGetSize().x * 0.9f, 0.0f)))
                {
                    ImGui::TableSetupColumn(S(THPRAC_KENG_PLAY_INDEX), 0, LauncherWndGetSize().x * 0.9f * 0.1f);
                    ImGui::TableSetupColumn(S(THPRAC_KENG_PLAY_NAME), 0, LauncherWndGetSize().x * 0.9f * 0.15f);
                    ImGui::TableSetupColumn(S(THPRAC_KENG_PLAY_DATE), 0, LauncherWndGetSize().x * 0.9f * 0.2f);
                    ImGui::TableSetupColumn(S(THPRAC_KENG_PLAY_CMT), 0, LauncherWndGetSize().x * 0.9f * 0.4f);
                    ImGui::TableSetupColumn(S(THPRAC_KENG_PLAY_CNT), 0, LauncherWndGetSize().x * 0.9f * 0.15f);
                    ImGui::TableHeadersRow();
                    int idx = 0;
                    for (auto it = mPlays.begin(); it != mPlays.end(); it++) {

                        ImGui::TableNextRow();
                        ImGui::TableNextColumn();
                        ImGui::Text("%2d", idx+1);
                        ImGui::TableNextColumn();
                        if (ImGui::Selectable(std::format("{}##{}", it->mPlayName, idx).c_str(), false, ImGuiSelectableFlags_::ImGuiSelectableFlags_SpanAllColumns)) {
                            play_index_draw = idx;
                        }
                        ImGui::TableNextColumn();
                        ImGui::Text(it->GetTimeDesc());
                        ImGui::TableNextColumn();
                        ImGui::Text(it->GetDescription_Line());
                        ImGui::TableNextColumn();
                        ImGui::Text("%6d", it->mDiffsDied.size());
                        idx++;
                    }
                    ImGui::EndTable();
                }

                // add play
                ImGui::NewLine();
                ImGui::Separator();
                ImGui::SetCursorPosX(LauncherWndGetSize().x * 0.05f);

                static char playName[256] = { 0 };
                static char playCmt[2048] = { 0 };
                static std::chrono::year_month_day time;
                static std::vector<char> diffs_select;
                static bool isopen = false;
                if (ImGui::Button(S(THPRAC_KENG_PLAY_ADD))) {
                    isopen = true;
                    diffs_select.resize(mKengDifficulties.size());
                    for (auto& i : diffs_select)
                        i = 0;
                    time = std::chrono::year_month_day(std::chrono::floor<std::chrono::days>(std::chrono::system_clock::now()));
                    ImGui::OpenPopup(S(THPRAC_KENG_PLAY_ADD_POPUP));
                }

                if (GuiModal(S(THPRAC_KENG_PLAY_ADD_POPUP), { LauncherWndGetSize().x * 0.95f, LauncherWndGetSize().y * 0.8f }, &isopen)) {
                    ImGui::PushTextWrapPos(LauncherWndGetSize().x * 0.9f);
                    {
                        ImGui::Columns(2, 0, false);
                        ImGui::SetColumnWidth(0, 300.0f);
                        ImGui::Text(S(THPRAC_KENG_PLAY_NAME));
                        ImGui::NextColumn();
                        ImGui::InputText("##play name", playName, sizeof(playName));
                        ImGui::NextColumn();

                        ImGui::Text(S(THPRAC_KENG_PLAY_DATE));
                        ImGui::NextColumn();
                        GuiDateSelector("##play date", &time);
                        ImGui::NextColumn();

                        ImGui::Text(S(THPRAC_KENG_PLAY_CMT));
                        ImGui::NextColumn();
                        ImGui::InputTextMultiline("##play cmt", playCmt, sizeof(playCmt));

                        ImGui::Columns(1);

                        static int idx_add = -1;
                        int idx_u = -1, idx_d = -1, idx_rem = -1;
                        if (ImGui::BeginTable("##diffaddTable", 2, ImGuiTableFlags_::ImGuiTableFlags_Borders | ImGuiTableFlags_::ImGuiTableFlags_Resizable))
                        {
                            ImGui::TableSetupColumn(S(THPRAC_KENG_DIFF_TABLE_CK), 0, ImGui::GetTextLineHeight() * 10);
                            ImGui::TableSetupColumn(S(THPRAC_KENG_DIFF_TABLE_NAME), 0, ImGui::GetWindowWidth() - ImGui::GetTextLineHeight() * 10);

                            ImGui::TableHeadersRow();
                            
                            if (mKengDifficulties.size() == 0) {
                                ImGui::TableNextRow();
                                ImGui::TableNextColumn();
                                if (ImGui::Button(S(THPRAC_KENG_DIFF_TABLE_ADD)))
                                    idx_add = 0;
                            }
                            
                            for (int idx = 0; idx < mKengDifficulties.size(); idx++) {
                                ImGui::TableNextRow();
                                ImGui::TableNextColumn();
                                if (ImGui::Button(std::format("{}##diff_add_{}", S(THPRAC_KENG_DIFF_TABLE_ADD), idx).c_str()))
                                    idx_add = idx;
                                ImGui::SameLine();
                                if (ImGui::Button(std::format("{}##diff_rem_{}", S(THPRAC_KENG_DIFF_TABLE_REM), idx).c_str()))
                                    idx_rem = idx;
                                ImGui::SameLine();
                                // u
                                if (ImGui::Button(std::format("{}##diff_u_{}", S(THPRAC_KENG_DIFF_TABLE_U), idx).c_str()))
                                    idx_u = idx;
                                ImGui::SameLine();
                                // d
                                if (ImGui::Button(std::format("{}##diff_d_{}", S(THPRAC_KENG_DIFF_TABLE_D), idx).c_str()))
                                    idx_d = idx;
                                ImGui::SameLine();
                                ImGui::Checkbox(std::format("##diff{}", idx).c_str(), (bool*)(&diffs_select[idx]));
                                ImGui::TableNextColumn();
                                ImGui::Text(mKengDifficulties[idx].name);
                            }
                            ImGui::EndTable();
                        }else {
                            idx_add = -1;
                        }
                        GuiSwapDiff(idx_u, idx_d, mKengDifficulties, diffs_select);
                        GuiInsertDiff(idx_add, mKengDifficulties, diffs_select, mDifficulties_idtot);
                        if(idx_rem!=-1)
                            RemoveDiffs(idx_rem);

                        ImGui::SetCursorPosY(std::fmaxf(ImGui::GetCursorPosY(), ImGui::GetWindowHeight() - ImGui::GetTextLineHeightWithSpacing() * 1.5f));
                        auto retnValue = GuiCornerButton(S(THPRAC_APPLY), S(THPRAC_CANCEL), ImVec2(1.5f, 0.0f), true);
                        if (retnValue == 1) {
                            std::vector<int> ids;
                            for (int i = 0; i < mKengDifficulties.size(); i++) {
                                if (diffs_select[i])
                                    ids.push_back(mKengDifficulties[i].id);
                            }
                            mPlays.emplace_back(playName, playCmt, ids, time);
                            KengSave();
                            isopen = false;
                        } else if (retnValue == 2) {
                            isopen = false;
                        }
                        if (!isopen)
                            ImGui::CloseCurrentPopup();
                        ImGui::PopTextWrapPos();
                        ImGui::EndPopup();
                    }
                }

                ImGui::SameLine();
                GuiGetDetails();
                ImGui::SameLine();
                if (ImGui::Button(S(THPRAC_KENG_SAVE_CSV)))
                {
                    OPENFILENAMEW ofn;
                    wchar_t szFile[MAX_PATH] = L".csv";
                    ZeroMemory(&ofn, sizeof(ofn));
                    ofn.lStructSize = sizeof(ofn);
                    ofn.hwndOwner = nullptr;
                    ofn.lpstrFile = szFile;
                    ofn.nMaxFile = MAX_PATH;
                    ofn.lpstrFilter = L"csv File\0*.csv\0";
                    ofn.nFilterIndex = 1;
                    ofn.lpstrFileTitle = nullptr;
                    ofn.nMaxFileTitle = 0;
                    ofn.lpstrInitialDir = L"";
                    ofn.lpstrDefExt = L".csv";
                    ofn.Flags = OFN_OVERWRITEPROMPT;
                    if (GetSaveFileNameW(&ofn)) {
                        try {
                            WriteToCsv(szFile);
                        } catch (std::exception& e) {
                        }
                    }
                }
                
                ImGui::SetCursorPosX(LauncherWndGetSize().x * 0.05f);
                if (ImGui::Button(std::format("{}##2", S(THPRAC_KENG_RETURN)).c_str()))
                    *is_open = false;
                ImGui::SameLine();
                if (ImGui::Button(S(THPRAC_KENG_DEL))) {
                    ImGui::OpenPopup(S(THPRAC_KENG_DEL_POPUP));
                }
                if (GuiModal(S(THPRAC_KENG_DEL_POPUP), { LauncherWndGetSize().x * 0.3f, LauncherWndGetSize().y * 0.3f })) {
                    ImGui::PushTextWrapPos(LauncherWndGetSize().x * 0.9f);
                    {
                        ImGui::Text(S(THPRAC_KENG_DEL_YES_OR_NO));
                        ImGui::SetCursorPosY(std::fmaxf(ImGui::GetCursorPosY(), ImGui::GetWindowHeight() - ImGui::GetTextLineHeightWithSpacing() * 1.5f));
                        auto retnValue = GuiCornerButton(S(THPRAC_APPLY), S(THPRAC_CANCEL), ImVec2(1.0f, 0.0f), true);
                        if (retnValue == 1) {
                            *is_del = true;
                            *is_open = false;
                            ImGui::CloseCurrentPopup();
                        } else if (retnValue == 2) {
                            ImGui::CloseCurrentPopup();
                        }
                        ImGui::PopTextWrapPos();
                        ImGui::EndPopup();
                    }
                }
            }
        }
        friend class KengRecorder;
    };


    class KengRecorder
    {
        std::vector<Keng> mKengs;
        int cur_draw_keng_idx=-1;
    public:
        void ReadRecords(std::istream& is)
        {
            int ver=1,keng_cnt=0;
            is.read((char*)&ver, sizeof(ver));
            is.read((char*)&cur_draw_keng_idx, sizeof(cur_draw_keng_idx));
            is.read((char*)&keng_cnt, sizeof(keng_cnt));
            for (int i = 0; i < keng_cnt; i++) {
                mKengs.emplace_back(is, ver);
            }
        }
        void WriteRecords(std::ostream& os)
        {
            int ver = 1, keng_cnt = mKengs.size();
            os.write((char*)&ver, sizeof(ver));
            os.write((char*)&cur_draw_keng_idx, sizeof(cur_draw_keng_idx));
            os.write((char*)&keng_cnt, sizeof(keng_cnt));
            for (int i = 0; i < keng_cnt; i++) {
                mKengs[i].Write(os);
            }
        }
        void DrawKengs()
        {
            if (cur_draw_keng_idx >= mKengs.size() || cur_draw_keng_idx < 0)
                cur_draw_keng_idx = -1;
            if (cur_draw_keng_idx == -1)
            {
                ImGui::SetCursorPosX(LauncherWndGetSize().x * 0.05f);
                if (ImGui::BeginTable("##kengsTable", 3, ImGuiTableFlags_::ImGuiTableFlags_Borders | ImGuiTableFlags_::ImGuiTableFlags_Resizable, ImVec2(LauncherWndGetSize().x * 0.9f, 0.0f)))
                {
                    ImGui::TableSetupColumn(S(THPRAC_KENG_NAME), 0, LauncherWndGetSize().x * 0.9f * 0.25f);
                    ImGui::TableSetupColumn(S(THPRAC_KENG_DATE), 0, LauncherWndGetSize().x * 0.9f * 0.25f);
                    ImGui::TableSetupColumn(S(THPRAC_KENG_DESC), 0, LauncherWndGetSize().x * 0.9f * 0.5f);
                    ImGui::TableHeadersRow();

                    int idx = 0;
                    for (auto it = mKengs.begin(); it != mKengs.end(); it++) {

                        ImGui::TableNextRow();
                        ImGui::TableNextColumn();
                        if (ImGui::Selectable(std::format("{}##{}", it->mKengName, idx).c_str(), false, ImGuiSelectableFlags_::ImGuiSelectableFlags_SpanAllColumns)) {
                            cur_draw_keng_idx = idx;
                        }
                        ImGui::TableNextColumn();
                        ImGui::Text(it->GetTimeDesc());
                        ImGui::TableNextColumn();
                        ImGui::Text(it->GetDescription_Line());
                        idx++;
                    }
                    ImGui::EndTable();
                }

                // adder
                ImGui::NewLine();
                ImGui::Separator();
                ImGui::SetCursorPosX(LauncherWndGetSize().x * 0.05f);

                static std::chrono::year_month_day time;
                static bool isopen = false;
                if (ImGui::Button(S(THPRAC_KENG_ADD_KENG))) {
                    isopen = true;
                    time = std::chrono::year_month_day(std::chrono::floor<std::chrono::days>(std::chrono::system_clock::now()));
                    ImGui::OpenPopup(S(THPRAC_KENG_ADD_KENG_POPUP));
                }

                if (GuiModal(S(THPRAC_KENG_ADD_KENG_POPUP), { LauncherWndGetSize().x * 0.95f, LauncherWndGetSize().y * 0.8f }, &isopen)) {
                    ImGui::PushTextWrapPos(LauncherWndGetSize().x * 0.9f);
                    {
                        static char kengName[256];
                        static char kengDesc[2048];

                        ImGui::Columns(2, 0, false);
                        ImGui::SetColumnWidth(0, 300.0f);
                        ImGui::Text(S(THPRAC_KENG_NAME));
                        ImGui::NextColumn();
                        ImGui::InputText("##keng name", kengName, sizeof(kengName));
                        ImGui::NextColumn();

                        ImGui::Text(S(THPRAC_KENG_DATE));
                        ImGui::NextColumn();
                        GuiDateSelector("##keng date", &time);
                        ImGui::NextColumn();
                        
                        ImGui::Text(S(THPRAC_KENG_DESC));
                        ImGui::NextColumn();
                        ImGui::InputTextMultiline("##keng desc", kengDesc, sizeof(kengDesc));

                        ImGui::Columns(1);

                        ImGui::SetCursorPosY(std::fmaxf(ImGui::GetCursorPosY(), ImGui::GetWindowHeight() - ImGui::GetTextLineHeightWithSpacing() * 1.5f));
                        auto retnValue = GuiCornerButton(S(THPRAC_APPLY), S(THPRAC_CANCEL), ImVec2(1.0f, 0.0f), true);
                        if (retnValue == 1) {
                            mKengs.emplace_back(kengName, kengDesc,time);
                            KengSave();
                            isopen = false;
                        } else if (retnValue == 2) {
                            isopen = false;
                        }
                        if(!isopen)
                            ImGui::CloseCurrentPopup();
                        ImGui::PopTextWrapPos();
                        ImGui::EndPopup();
                    }
                }

                ImGui::SameLine();
                if (ImGui::Button(S(THPRAC_KENG_READ_CSV)))
                {
                    std::wstring file = LauncherWndFileSelect(nullptr, L"csv(*.csv)\0*.csv\0*.*\0\0");
                    try {
                        mKengs.emplace_back(file);
                        KengSave();
                    } catch (std::exception& e) {

                    }
                }
            }else{
                bool is_del = false,is_open=true;
                mKengs[cur_draw_keng_idx].DrawKeng(&is_del, &is_open);
                if (is_del) {
                    mKengs.erase(mKengs.begin() + cur_draw_keng_idx);
                    cur_draw_keng_idx = -1;
                }
                if (!is_open){
                    cur_draw_keng_idx = -1;
                }
            }
        }
    };


class THKengRecorder {
public:
    KengRecorder mRecorder;

    THKengRecorder() { }
    SINGLETON(THKengRecorder);

private:
    
private:
public: 
    void Gui(){
        mRecorder.DrawKengs();
    }
};

class THOthersGui {
private:
    THOthersGui()
    {

    }
    SINGLETON(THOthersGui);

public:
    void GuiUpdate(){
        GuiMain();
    }

private:
    void GuiMain(){
        THKengRecorder::singleton().Gui();
    }
};

bool LauncherKengGuiUpd()
{
    THOthersGui::singleton().GuiUpdate();
    return true;
}

void LauncherKengInit()
{
    PushCurrentDirectory(L"%appdata%\\thprac");
    auto fs = ::std::fstream("keng.dat", ::std::ios::in | ::std::ios::binary);
    if (fs.is_open()) {
        THKengRecorder::singleton().mRecorder.ReadRecords(fs);
    }
    PopCurrentDirectory();
}

void LauncherKengUpdate()
{
}

void KengSave()
{
    int version = 1;
    PushCurrentDirectory(L"%appdata%\\thprac");
    auto fs = ::std::fstream("keng.dat", ::std::ios::out | ::std::ios::binary);
    if (fs.is_open()) {
        THKengRecorder::singleton().mRecorder.WriteRecords(fs);
    }
    PopCurrentDirectory();
}

void LauncherKengDestroy()
{
    KengSave();
}

}