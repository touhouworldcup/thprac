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
#include "..\3rdParties\rapidcsv\rapidcsv.h"
#include <numbers>
#include <format>
#include "thprac_launcher_tools_waifus.h"
namespace THPrac {

void LauncherToolsGuiSwitch(const char* gameStr);

std::wstring NormalizePath(std::wstring path)
{
    std::replace(path.begin(), path.end(), L'/', L'\\');
    std::transform(path.begin(), path.end(), path.begin(), ::towupper);
    return path;
}

std::vector<std::wstring> THClearGame()
{
    std::vector<std::wstring> gameKilled;

    auto paths = GetAllGamePaths();
    std::vector<std::wstring> normalizedList;
    for (const auto& path : paths) {
        normalizedList.push_back(NormalizePath(path));
    }
    if (paths.empty())
        return gameKilled;
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE)
        return gameKilled;
    PROCESSENTRY32W pe;
    pe.dwSize = sizeof(PROCESSENTRY32W);
    if (Process32FirstW(hSnapshot, &pe)) {
        do {
            HANDLE hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION | PROCESS_TERMINATE, FALSE, pe.th32ProcessID);
            if (hProcess) {
                wchar_t processPath[MAX_PATH];
                DWORD dwSize = MAX_PATH;
                if (QueryFullProcessImageNameW(hProcess, 0, processPath, &dwSize)) {
                    std::wstring currentPath = NormalizePath(processPath);
                    auto it = std::find(normalizedList.begin(), normalizedList.end(), currentPath);
                    if (it != normalizedList.end()) {
                        if (TerminateProcess(hProcess, 9)) {
                            gameKilled.push_back(currentPath);
                        }
                    }
                }
                CloseHandle(hProcess);
            }
        } while (Process32NextW(hSnapshot, &pe));
    }
    CloseHandle(hSnapshot);
    return gameKilled;
}

class THRoll
{
public:
    struct THRollSelection {
        std::string name;
        float weight;
        ImVec4 color;
    };
    float GetRandomFloat(float a=0.0f,float b=1.0f)
    {
        static std::random_device mRandDevice;
        static std::default_random_engine mRandEngine(mRandDevice());
        static std::uniform_real_distribution<float> rand_value(a, b);
        return rand_value(mRandEngine);
    }
    float MInterpolation(float t, float a, float b)
    {
        if (t < 0.0f) {
            return a;
        } else if (t < 0.5) {
            float k = (b - a) * 2.0f;
            return k * t * t + a;
        } else if (t < 1.0f) {
            float k = (b - a) * 4.0f;
            t = t - 1.0f;
            return k * t * t * t + b;
        }
        return b;
    };

private:
    std::vector<THRollSelection> selections;
    float weight_sum;
    std::string mRollName;

    bool DrawPie(ImDrawList* p, ImVec2 mid, float radius, float angle1, float angle2, uint32_t col_fill)
    {
        uint32_t col_line = 0xFFFFFFFF;
        bool res = false;
        std::vector<ImVec2> points;
        points.push_back({ cosf(angle1), sinf(angle1) });
        float dangle = std::numbers::pi * 0.005f;
        for (float angle = angle1 + dangle; angle < angle2; angle += dangle)
            points.push_back({ cosf(angle), sinf(angle) });
        points.push_back({ cosf(angle2), sinf(angle2) });

        auto mp = ImGui::GetMousePos();
        if (hypotf(mp.y - mid.y, mp.x - mid.x) < radius) {
            float mangle = atan2(mp.y - mid.y, mp.x - mid.x);
            mangle += (2.0f * std::numbers::pi) * ceilf((angle1 - mangle) / (2.0f * std::numbers::pi));
            res = mangle < angle2;
        }
        uint32_t col_fill2 = 0;
        if (res) {
            radius *= 1.1f;
            ImVec4 color = ImGui::ColorConvertU32ToFloat4(col_fill);
            col_fill2 = ImGui::ColorConvertFloat4ToU32(ImVec4 { color.x * 0.75f,
                                                                color.y * 0.75f,
                                                                color.z * 0.75f,
                                                                color.w });
        }
        
        for (int i = 0; i < points.size() - 1; i++) {
            ImVec2 p2 = { mid.x + points[i].x * radius, mid.y + points[i].y * radius };
            ImVec2 p3 = { mid.x + points[i + 1].x * radius, mid.y + points[i + 1].y * radius };
            p->AddTriangleFilled(mid, p2, p3, res ? col_fill2 : col_fill);
            p->AddTriangle(mid, p2, p3, res ? col_fill2 : col_fill, 1.33f);
            if (res) {
                p->AddLine(p2, p3, col_line, 2.0f);
            }
            
        }
        p->AddLine(mid, { mid.x + points[0].x * radius, mid.y + points[0].y * radius }, col_line, 2.0f);
        if (res) {
            p->AddLine(mid, { mid.x + points[points.size() - 1].x * radius, mid.y + points[points.size() - 1].y * radius }, col_line, 2.0f);
        }
        return res;
    }

    void InitWeight()
    {
        weight_sum = 0.0f;
        for (auto& i : selections)
        {
            weight_sum += i.weight;
        }
    }
    void InitColors(bool rand_color = true)
    {
        if (rand_color) {
            for (int i = 0; i < selections.size(); i++) {
                float r, g, b, h, s, v;
                h = GetRandomFloat();
                s = GetRandomFloat() * 0.4f + 0.6f;
                v = GetRandomFloat() * 0.4f + 0.6f;
                ImGui::ColorConvertHSVtoRGB(h, s, v, r, g, b);
                selections[i].color = { r, g, b, 1.0f };
            }
        } else {
            int hi = 0, si = 255, vi = 255;
            for (int i = 0; i < selections.size(); i++) {
                float r, g, b, h, s, v;
                if (selections.size() > 100)
                    hi += 7;
                else if (selections.size() > 50)
                    hi += 19;
                else
                    hi += 41;
                if (hi >= 256) {
                    hi -= 256;
                    si -= 83;
                    if (si <= 64) {
                        si += 191;
                        vi -= 101;
                        if (vi <= 128)
                            vi += 127;
                    }
                }
                hi %= 256;
                h = (float)hi / 255.0f;
                vi %= 256;
                s = (float)vi / 255.0f;
                si %= 256;
                v = (float)si / 255.0f;
                ImGui::ColorConvertHSVtoRGB(h, s, v, r, g, b);
                selections[i].color = { r, g, b, 1.0f };
            }
        }
    }

public:
    THRollSelection GetSelection(int idx)
    {
        return selections[idx];
    }

    int NumSelection()
    {
        return selections.size();
    }

    void RemoveSelection(int idx)
    {
        if (idx < selections.size()) {
            selections.erase(selections.begin() + idx);
        }
        InitWeight();
    }

    void InitRoll(std::string rollName, std::vector<THRollSelection> sels, bool reset_color = true, bool rand_color = true)
    {
        selections = sels;
        mRollName = rollName;
        if (reset_color) {
            InitColors(rand_color);
        }
        InitWeight();
    }

    void InitRoll(std::wstring csv_filename, bool rand_color = true)
    {
        std::vector<std::string> mNames = {};
        std::vector<float> mWeights = {};

        std::string rollName = utf16_to_utf8(GetNameFromFullPath(csv_filename).c_str());

        rapidcsv::Document doc(utf16_to_mb(csv_filename.c_str(), CP_ACP), rapidcsv::LabelParams(0, -1));
        mNames = doc.GetColumn<std::string>(0);
        mWeights = doc.GetColumn<float>(1);
        int n = std::min(mNames.size(), mWeights.size());
        std::vector<THRollSelection> selections_csv;
        for (int i = 0; i < n; i++) {
            THRollSelection sel;
            sel.name = mNames[i];
            sel.weight = mWeights[i];
            sel.color = { 1.0f, 1.0f, 1.0f, 1.0f };
            selections_csv.push_back(sel);
        }
        InitRoll(rollName, selections_csv, true, rand_color);
    }

    void InitRoll(std::string rollName,std::vector<std::string> names, bool rand_color = true)
    {
        std::vector <THRollSelection> sels = {};
        for (auto& n : names)
        {
            THRollSelection sel;
            sel.name = n;
            sel.weight = 1.0f;
            sel.color = { 1.0f, 1.0f, 1.0f, 1.0f };
            sels.push_back(sel);
        }
        InitRoll(rollName, sels, true, rand_color);
    }

    THRoll(std::string rollName, std::vector<THRollSelection> sels, bool reset_color = true, bool rand_color = true)
    {
        InitRoll(rollName, sels, reset_color, rand_color);
    }
    THRoll()
    {
        weight_sum = 0.0f;
    }
    THRoll(std::wstring csv_filename, bool rand_color = true)
    {
        InitRoll(csv_filename, rand_color);
    }

    int CalcIdx(float angle)
    {
        angle = -angle;
        angle = angle - floorf(angle / (2.0f * std::numbers::pi)) * 2.0f * std::numbers::pi;
        int idx = -1;
        float res = angle / (2.0f * std::numbers::pi);
        float weight_cur = 0;
        for (int i = 0; i < selections.size(); i++) {
            float weight_next = weight_cur + selections[i].weight;
            if (weight_cur / weight_sum <= res && weight_next / weight_sum > res) {
                idx = i;
                break;
            }
            weight_cur = weight_next;
        }
        return idx;
    }

    void GuiDraw(float& angle,int selection_idx,float height_rem = 0.0f)
    {
        angle = angle - floorf(angle / (2.0f * std::numbers::pi)) * 2.0f * std::numbers::pi;
        {
            ImVec2 p0 = ImGui::GetCursorScreenPos();
            ImVec2 csz = ImGui::GetContentRegionAvail();
            csz.y -= height_rem;
            if (csz.y < 0)
                csz.y = 0;
            ImVec2 cmid = { p0.x + csz.x * 0.5f, p0.y + csz.y * 0.5f };
            ImVec2 p1 = { cmid.x + csz.x * 0.5f, cmid.y + csz.y * 0.5f };
            float hheight = std::min(csz.x, csz.y) * 0.95f * 0.5f;

            ImDrawList* p = ImGui::GetWindowDrawList();
            p->AddRectFilled(p0, p1, IM_COL32(50, 50, 50, 100));
            p->AddRect(p0, p1, IM_COL32(255, 255, 255, 100));
            p->PushClipRect(p0, p1);

            ImVec2 cir_cen = { hheight * 1.1f, cmid.y };
            if (selections.size() > 0 && csz.y > 0.0f) {
                {
                    p->AddCircle(cir_cen, hheight, 0xFFFFFFFF);
                    float pie_angle_start = angle;
                    for (int i = 0; i < selections.size(); i++) {
                        float pie_angle_delta = selections[i].weight / weight_sum * 2.0f * std::numbers::pi;

                        auto col1 = ImGui::ColorConvertFloat4ToU32(selections[i].color);
                        bool hovered = false;
                        if (selection_idx==i)
                            hovered = DrawPie(p, cir_cen, hheight * 1.05f, pie_angle_start, pie_angle_start + pie_angle_delta, col1);
                        else
                            hovered  = DrawPie(p, cir_cen, hheight, pie_angle_start, pie_angle_start + pie_angle_delta, col1);
                        if (hovered)
                            ImGui::SetTooltip("%s", selections[i].name.c_str());
                        pie_angle_start += pie_angle_delta;
                    }
                }
                p->PopClipRect();
                ImVec2 tri_pos1 = { cir_cen.x + hheight * 0.95f, cir_cen.y };
                ImVec2 tri_pos2 = { cir_cen.x + hheight * 1.45f, cir_cen.y - hheight * 0.05f };
                ImVec2 tri_pos3 = { cir_cen.x + hheight * 1.45f, cir_cen.y + hheight * 0.05f };
                p->AddTriangleFilled(tri_pos1, tri_pos2, tri_pos3, 0xFFFFCCCC);
                p->AddTriangle(tri_pos1, tri_pos2, tri_pos3, 0xFFFFFFFF, 1.5f);
                if (selection_idx >= 0 && selection_idx < selections.size())
                    p->AddText({ cir_cen.x + hheight * 1.5f, cir_cen.y - ImGui::GetTextLineHeight() * 0.5f }, 0xFFFFFFFF, std::format("{}", selections[selection_idx].name).c_str());
            }
        }
        return;
    }
};


class THGuiRollAll {
private:
    THRoll mRoll;
    bool mRandColor;

    bool is_rolling;
    float angle_fin;
    float angle_last;
    float time;
    float time_tot;

public:
    THGuiRollAll()
    {
        is_rolling = false;
        angle_fin = 0.0f;
        angle_last = 0.0f;
        mRandColor = false;
        time_tot = 240.0f;
        time = 0.0f;
    }
    
    void LoadRoll()
    {
        std::wstring csv_filename = LauncherWndFileSelect(nullptr, L"csv(*.csv)\0*.csv\0*.*\0\0");
        mRoll.InitRoll(csv_filename, true);
    }
    void LoadRollWaifu()
    {
        mRoll.InitRoll("waifu", waifus, mRandColor);
    }
    bool GuiUpdate()
    {
        bool result = true;
        if (ImGui::Button(S(THPRAC_BACK))) {
            result = false;
        }
        ImGui::SameLine();
        GuiCenteredText(S(THPRAC_TOOLS_ROLLF_FROM_FILE));
        ImGui::Separator();
        if (ImGui::Button(S(THPRAC_TOOLS_ROLLF_CSV)))
            LoadRoll();
        ImGui::SameLine();
        GuiHelpMarker(S(THPRAC_TOOLS_ROLLF_CSV_DESC));
        ImGui::SameLine();
        if (ImGui::Button("waifus"))
            LoadRollWaifu();
        ImGui::SameLine();
        ImGui::Checkbox(S(THPRAC_TOOLS_ROLLF_RANDOM_COLOR), &mRandColor);
        ImGui::SameLine();
        if (ImGui::CollapsingHeader(S(THPRAC_TOOLS_ROLLF_CLPS))){
            if (ImGui::BeginTable("__rolls table",2)) {
                for (int i = 0; i < mRoll.NumSelection(); i++)
                {
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("%s", mRoll.GetSelection(i).name.c_str());
                    ImGui::TableNextColumn();
                    ImGui::Text("%.2f", mRoll.GetSelection(i).weight);
                }
                ImGui::EndTable();
            }
        }
        ImGui::Separator();

        if (ImGui::Button(S(THPRAC_TOOLS_ROLLF_START)) && mRoll.NumSelection() > 0)
        {
            is_rolling = true;
            time = 0;
            angle_last = angle_fin - floorf(angle_fin / (2.0f * std::numbers::pi)) * 2.0f * std::numbers::pi;
            angle_fin = (mRoll.GetRandomFloat() * 2.0f * std::numbers::pi) + 30.0f * std::numbers::pi;
        }
        float angle_cur;
        if (is_rolling) {
            angle_cur = mRoll.MInterpolation(time / (float)time_tot, angle_last, angle_fin);
            if (time < time_tot) {
                time++;
            } else {
                is_rolling = false;
            }
        } else {
            angle_cur = angle_fin;
        }
        int roll_result = mRoll.CalcIdx(angle_cur);
        if (!is_rolling) {
            ImGui::SameLine();
            if (ImGui::Button(S(THPRAC_TOOLS_ROLLF_REMOVE_CUR))){
                mRoll.RemoveSelection(roll_result);
            }
            
        }
        if (!is_rolling) {
            ImGui::SameLine();
            ImGui::Text(S(THPRAC_TOOLS_ROLLF_TIME));
            ImGui::SameLine();
            ImGui::SetNextItemWidth(150.0f);
            if (ImGui::DragFloat("##roll time", &time_tot, 1.0f, 1, 1000))
                time_tot = std::clamp(time_tot, 1.0f, 1000.0f);
        }

        mRoll.GuiDraw(angle_cur, roll_result);
        return result;
    }

private:
};

THGuiTestReactionTest::THGuiTestReactionTest()
{
    QueryPerformanceFrequency(&mTimeFreq);
    mTestState = NOT_BEGIN;
    mTestType = PRESS;
    mTestTime = 5;
    mRndSeedGen = GetRndGenerator(1200u, 2800u);
    // mRndSeedGen = GetRndGenerator(0u, 1u);
    mShowProgressBar = false;
    mCurTest = 0;
}
bool THGuiTestReactionTest::GuiUpdate(bool ingame)
{
    LARGE_INTEGER curTime;
    QueryPerformanceCounter(&curTime);
    Gui::KeyboardInputUpdate(VK_DOWN);
    Gui::KeyboardInputUpdate(VK_UP);
    Gui::KeyboardInputUpdate(VK_RIGHT);
    Gui::KeyboardInputUpdate(VK_LEFT);
    Gui::KeyboardInputUpdate('Z');
    Gui::KeyboardInputUpdate(VK_SHIFT);

    ImVec2 colorBtnSz = ImVec2(ImGui::GetWindowWidth()*0.5f, ImGui::GetTextLineHeight() * 10.0f);
    ImVec2 colorBtnPos = ImVec2(ImGui::GetWindowWidth()*0.25f, ImGui::GetTextLineHeight() * 2.0f);
    DWORD colorBtnFlag = ImGuiColorEditFlags_::ImGuiColorEditFlags_NoTooltip;
    ImVec4 color_BeforeTimeReact = ImVec4(1, 0.2, 0.2, 1);
    ImVec4 color_AfterTimeReact = ImVec4(0, 1, 0.2, 1);
    ImVec4 color_WaitPress = ImVec4(1, 1, 0.2, 1);

    bool isKeyPressed = ImGui::IsKeyDown(37) || ImGui::IsKeyDown(38) || ImGui::IsKeyDown(39) || ImGui::IsKeyDown(40)
        || (GetAsyncKeyState(VK_DOWN) & 0x8000) || (GetAsyncKeyState(VK_UP) & 0x8000) || (GetAsyncKeyState(VK_RIGHT) & 0x8000) || (GetAsyncKeyState(VK_LEFT) & 0x8000);
    if (!ingame){
        if (ImGui::Button(S(THPRAC_BACK))) {
            Reset();
            return false;
        }
    }
    switch (mTestState)
    {
    case NOT_BEGIN:
    {
        if (!ingame) {
            ImGui::SameLine();
            GuiCenteredText(S(THPRAC_TOOLS_REACTION_TEST));
            ImGui::Separator();
        }
    
        ImGui::RadioButton(S(THPRAC_TOOLS_REACTION_TEST_DOWN), (int*)&mTestType, PRESS);
        ImGui::SameLine();
        ImGui::RadioButton(S(THPRAC_TOOLS_REACTION_TEST_UP), (int*)&mTestType, RELEASE);
    
        ImGui::Checkbox(S(THPRAC_TOOLS_REACTION_SHOW_PROC_BAR), &mShowProgressBar);
    
        ImGui::DragInt(S(THPRAC_TOOLS_REACTION_TEST_TIME), &mTestTime, 1.0f, 1, 20);
        ImGui::NewLine();
        if (ImGui::Button(S(THPRAC_TOOLS_REACTION_TEST_BEGIN)) || ImGui::IsKeyPressed('Z') || ImGui::IsKeyPressed(VK_SHIFT)) {
            // in win 7, due to the initialize sequence, QueryPerformanceFrequency might get 0 value at the beginning, 
            // so query it every time before test begin to avoid this problem
            QueryPerformanceFrequency(&mTimeFreq);
            mTestState = WAIT_TIME;
            mCurTest = 1;
            mFrameCount = 0;
            if (mTestType == PRESS) {
                mWaitTime.QuadPart = mRndSeedGen() / 1000.0 * (double)mTimeFreq.QuadPart;
                mPressTime.QuadPart = mWaitTime.QuadPart + curTime.QuadPart;
            }
            mResults = {};
        }
        ImGui::InvisibleButton("inv", colorBtnSz);
    } break;
    
    case TOO_EARLY: {
        ImGui::Text(S(THPRAC_TOOLS_REACTION_TEST_TOO_EARLY));
        if (ImGui::Button(S(THPRAC_TOOLS_REACTION_TEST_NEXT_TEST)) || ImGui::IsKeyPressed('Z') || ImGui::IsKeyPressed(VK_SHIFT)) // press z(90)/shift(16)
        {
            mFrameCount = 0;
            mTestState = WAIT_TIME;
            mWaitTime.QuadPart = mRndSeedGen() / 1000.0 * (double)mTimeFreq.QuadPart;
            mPressTime.QuadPart = mWaitTime.QuadPart + curTime.QuadPart;
        }
        ImGui::InvisibleButton("inv", colorBtnSz);
    } break;
    case SHOW_RES: {
        if (mResults.size() != 0 && mFrameCounts.size() != 0)
        {
            ImGui::Text("%s(%d): %.1f ms (%.1f frame)(framecount: %d)", S(THPRAC_TOOLS_REACTION_TEST_RESULT), mCurTest, mResults[mResults.size() - 1], mResults[mResults.size() - 1] / 16.66667f,(int)mFrameCounts[mFrameCounts.size()-1]);
            if (mCurTest < mTestTime)
                ImGui::InvisibleButton("inv", colorBtnSz);
        }else
            ImGui::Text("%s(%d): ?????", S(THPRAC_TOOLS_REACTION_TEST_RESULT), mCurTest);
        if (mCurTest == mTestTime)
        {
            float avg = 0.0f;
            float maxv = 60.0f;
            float minv = 0.0f;
            float avg_frameCnt = 0.0f;
            float maxfcnt = 10.0f;
            float minfcnt = 0.0f;
            for (auto x : mResults) {
                avg += fabsf(x);
                maxv = std::max(x, maxv);
                minv = std::min(x, minv);
            }
            for (auto x : mFrameCounts) {
                avg_frameCnt += fabsf(x);
                maxfcnt = std::max(x, maxfcnt);
                minfcnt = std::min(x, minfcnt);
            }
            avg /= (float)mTestTime;
            avg_frameCnt /= (float)mTestTime;
            ImGui::Separator();
            ImGui::Text("%s: %s, %s", S(THPRAC_TOOLS_REACTION_MODE), mTestType == PRESS ? S(THPRAC_TOOLS_REACTION_MODE_DOWN) : S(THPRAC_TOOLS_REACTION_MODE_UP), 
                mShowProgressBar ? S(THPRAC_TOOLS_REACTION_MODE_PROGRESSBAR) : S(THPRAC_TOOLS_REACTION_MODE_NORMAL)
            );
            ImGui::Text("%s: %.1f ms (%.1f frame)(framecount: %.1f)", S(THPRAC_TOOLS_REACTION_TEST_RESULT_AVG), avg, avg / 16.66667f, avg_frameCnt);
            
            ImGui::PlotHistogram(S(THPRAC_TOOLS_REACTION_TEST_RESULT), &mResults[0], mTestTime, 0, S(THPRAC_TOOLS_REACTION_TEST_RESULT), minv-10.0f, maxv+20.0f, ImVec2(0, 200.0));
            
            ImGui::PlotHistogram(std::format("{}(framecount)", S(THPRAC_TOOLS_REACTION_TEST_RESULT)).c_str(), &mFrameCounts[0], mTestTime, 0, std::format("{}(framecount)", S(THPRAC_TOOLS_REACTION_TEST_RESULT)).c_str(), minfcnt-1.0f, maxfcnt + 10.0f, ImVec2(0, 200.0));
            if (ImGui::Button(S(THPRAC_TOOLS_REACTION_TEST_NEXT_TEST)) || ImGui::IsKeyPressed('Z') || ImGui::IsKeyPressed(VK_SHIFT)) {
                mTestState = NOT_BEGIN;
            }
        } else {
            if (ImGui::Button(S(THPRAC_TOOLS_REACTION_TEST_NEXT_TEST)) || ImGui::IsKeyPressed('Z') || ImGui::IsKeyPressed(VK_SHIFT)) // press z
            {
                mCurTest++;
                mTestState = WAIT_TIME;
                mFrameCount = 0;
                mWaitTime.QuadPart = mRndSeedGen() / 1000.0 * (double)mTimeFreq.QuadPart;
                mPressTime.QuadPart = mWaitTime.QuadPart + curTime.QuadPart;
            }
        }
    } break;
    
    case REACT_TIME:
    case WAIT_TIME:
    case WAIT_TIME_PRESSED:
    {
        if (mTestType == PRESS)
        {
            if (mTestState == WAIT_TIME && curTime.QuadPart >= mPressTime.QuadPart){
                mTestState = REACT_TIME;
                mFrameCount = 0;
            }
            ImGui::Text("test %d", mCurTest);
            mFrameCount++;
            if (mTestState == WAIT_TIME) {
                if (mShowProgressBar) {
                    ImGui::ProgressBar(std::clamp(std::fabsf(((double)(mPressTime.QuadPart - curTime.QuadPart)) / ((double)mWaitTime.QuadPart)), 0.0f, 1.0f), ImVec2(0, 0), "waiting...");
                }
                ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + colorBtnPos.x, ImGui::GetCursorPosY() + colorBtnPos.y));
                ImGui::ColorButton("color", color_BeforeTimeReact, colorBtnFlag, colorBtnSz);
                if (isKeyPressed){
                    if (!mShowProgressBar){
                        mTestState = TOO_EARLY;
                    } else {
                        // allow negative reaction time
                        auto curTestReactionTimeMs = ((double)(curTime.QuadPart - mPressTime.QuadPart)) / ((double)mTimeFreq.QuadPart) * 1000.0;
                        mResults.push_back(curTestReactionTimeMs);
                        mFrameCounts.push_back(ceil(fabsf(curTestReactionTimeMs))/16.6667f);
                        mTestState = SHOW_RES;
                    }
                }
            } else {
                if (mShowProgressBar) {
                    ImGui::ProgressBar(0.0f, ImVec2(0, 0), "press...");
                }
                ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + colorBtnPos.x, ImGui::GetCursorPosY() + colorBtnPos.y));
                ImGui::ColorButton("color", color_AfterTimeReact, colorBtnFlag, colorBtnSz);
                if (isKeyPressed){
                    auto curTestReactionTimeMs = ((double)(curTime.QuadPart - mPressTime.QuadPart)) / ((double)mTimeFreq.QuadPart) * 1000.0;
                    mResults.push_back(curTestReactionTimeMs);
                    mFrameCounts.push_back(mFrameCount);
                    mTestState = SHOW_RES;
                }
            }
        }else{
            if (mTestState == WAIT_TIME_PRESSED && curTime.QuadPart >= mPressTime.QuadPart) {
                mTestState = REACT_TIME;
                mFrameCount = 0;
            }
            mFrameCount++;
            ImGui::Text("test %d", mCurTest);
            if (mTestState == WAIT_TIME) {
                if (mShowProgressBar) {
                    ImGui::ProgressBar(1.0f, ImVec2(0, 0), "waiting...");
                }
                ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + colorBtnPos.x, ImGui::GetCursorPosY() + colorBtnPos.y));
                ImGui::ColorButton("color", color_WaitPress, colorBtnFlag, colorBtnSz);
                if (isKeyPressed) {
                    mWaitTime.QuadPart = mRndSeedGen() / 1000.0 * (double)mTimeFreq.QuadPart;
                    mPressTime.QuadPart = mWaitTime.QuadPart + curTime.QuadPart;
                    mTestState = WAIT_TIME_PRESSED;
                }
            } else if(mTestState == REACT_TIME){
                if (mShowProgressBar) {
                    ImGui::ProgressBar(0.0f, ImVec2(0, 0), "press...");
                }
                ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + colorBtnPos.x, ImGui::GetCursorPosY() + colorBtnPos.y));
                ImGui::ColorButton("color", color_AfterTimeReact, colorBtnFlag, colorBtnSz);
                if (!isKeyPressed) {
                    auto curTestReactionTimeMs = ((double)(curTime.QuadPart - mPressTime.QuadPart)) / ((double)mTimeFreq.QuadPart) * 1000.0;
                    mResults.push_back(curTestReactionTimeMs);
                    mFrameCounts.push_back(mFrameCount);
                    mTestState = SHOW_RES;
                }
            }
            else if (mTestState == WAIT_TIME_PRESSED){
                if (mShowProgressBar) {
                    ImGui::ProgressBar(std::clamp(std::fabsf(((double)(mPressTime.QuadPart - curTime.QuadPart)) / ((double)mWaitTime.QuadPart)), 0.0f, 1.0f), ImVec2(0, 0), "waiting...");
                }
                ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + colorBtnPos.x, ImGui::GetCursorPosY() + colorBtnPos.y));
                ImGui::ColorButton("color", color_BeforeTimeReact, colorBtnFlag, colorBtnSz);
                if (!isKeyPressed) {
                    if (!mShowProgressBar) {
                        mTestState = TOO_EARLY;
                    } else {
                        auto curTestReactionTimeMs = ((double)(curTime.QuadPart - mPressTime.QuadPart)) / ((double)mTimeFreq.QuadPart) * 1000.0;
                        mResults.push_back(curTestReactionTimeMs);
                        mFrameCounts.push_back(ceil(fabsf(curTestReactionTimeMs)) / 16.6667f);
                        mTestState = SHOW_RES;
                    }
                }
            }
        }
    } 
    break;
    }
    return true;
}

void THGuiTestReactionTest::Reset()
{
    QueryPerformanceFrequency(&mTimeFreq);
    mTestState = NOT_BEGIN;
    mTestType = PRESS;
    mTestTime = 5;
    mRndSeedGen = GetRndGenerator(1200u, 3500u);
    mShowProgressBar = false;
    mCurTest = 0;
    mResults = {};
    mFrameCounts = {};
}


class THGuiRollPlayer {
public:
    THRoll mRoll;

    bool first_roll;
    bool is_rolling;
    float angle_fin;
    float angle_last;
    float time;
    float time_tot;


    THGuiRollPlayer()
    {
        is_rolling = false;
        first_roll = true;
        angle_fin = -0.1f;
        angle_last = -0.1f;
        time_tot = 240.0f;
        time = 0.0f;

        mRndTextGen = GetRndGenerator(1u, 20u);

        for (auto& game : gGameRoll) {
            if (game.playerSelect) {
                mGameOption.push_back(game);
            }
        }
        if (mRndTextGen() == 1) {
            mRollText = S(THPRAC_GAMEROLL_EENY_MEENY);
        } else {
            mRollText = S(THPRAC_GAMEROLL_ROLL);
        }
        SetPlayerOpt();
        UpdateRoll();
    }

    void UpdateRoll()
    {
        std::vector<std::string> names;
        for (auto& player : mPlayerOption) {
            if (player.second) {
                names.push_back(player.first);
            }
        }
        mRoll.InitRoll("rndgame", names, false);
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
        UpdateRoll();
    }
    void SetPlayerOpt()
    {
        mPlayerOption.clear();
        mRollText = S(THPRAC_GAMEROLL_ROLL);
        auto playerStr = S(mGameOption[mGameSelected].shottypes);
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
        if (ImGui::Button(S(THPRAC_BACK))) {
            result = false;
        }
        ImGui::SameLine();
        GuiCenteredText(S(THPRAC_TOOLS_RND_PLAYER));
        ImGui::Separator();

        bool changed = false;
        if (ImGui::CollapsingHeader(S(THPRAC_TOOLS_RND_GAME))) {

            if (ImGui::BeginCombo(S(THPRAC_TOOLS_RND_PLAYER_GAME), S(mGameOption[mGameSelected].title), 0)) { // The second parameter is the label previewed before opening the combo.
                changed = true;
                for (size_t n = 0; n < mGameOption.size(); n++) {
                    bool is_selected = (mGameSelected == n);
                    if (ImGui::Selectable(S(mGameOption[n].title), is_selected)) {
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
                    changed |= ImGui::Checkbox(player.first.c_str(), &player.second);
                    ImGui::NextColumn();
                }
                ImGui::Columns(1);
            }
        }

        std::vector<std::string> candidate;
        for (auto& player : mPlayerOption) {
            if (player.second) {
                candidate.push_back(player.first);
            }
        }

        if (changed) {
            UpdateRoll();
        }

        float angle_cur;
        if (is_rolling) {
            angle_cur = mRoll.MInterpolation(time / (float)time_tot, angle_last, angle_fin);
            if (time < time_tot) {
                time++;
            } else {
                is_rolling = false;
            }
        } else {
            angle_cur = angle_fin;
        }
        int roll_result = mRoll.CalcIdx(angle_cur);

        auto wndSize = ImGui::GetWindowSize();
        mRoll.GuiDraw(angle_cur, roll_result, wndSize.y * 0.15f);

        if (!first_roll && !is_rolling) {
            auto result = roll_result;
            if (result != -1 && result < candidate.size()) {
                char outputStr[256];
                sprintf_s(outputStr, S(THPRAC_TOOLS_ROLL_RESULT), candidate[result].c_str());
                mRollText = outputStr;
                mRollGame = S(mGameOption[mGameSelected].title);
            }
        }

        if (GuiButtonRelCentered(mRollText.c_str(), 0.9f, ImVec2(1.0f, 0.08f))) {
            first_roll = false;
            is_rolling = true;
            time = 0;
            angle_last = angle_fin - floorf(angle_fin / (2.0f * std::numbers::pi)) * 2.0f * std::numbers::pi;
            angle_fin = (mRoll.GetRandomFloat() * 2.0f * std::numbers::pi) + 30.0f * std::numbers::pi;
        }

        if (mRollText != S(THPRAC_GAMEROLL_ROLL) && ImGui::BeginPopupContextItem("##roll_player_popup")) {
            if (ImGui::Selectable(S(THPRAC_TOOLS_RND_TURNTO_GAME))) {
                GuiLauncherMainSwitchTab(S(THPRAC_GAMES));
                LauncherGamesGuiSwitch(mRollGame.c_str());
            }
            ImGui::EndPopup();
        }

        if (!result) {
            if (mRndTextGen() == 1) {
                mRollText = S(THPRAC_GAMEROLL_EENY_MEENY);
            } else {
                mRollText = S(THPRAC_GAMEROLL_ROLL);
            }
        }
        return result;
    }

private:
    std::function<unsigned int(void)> mRndTextGen;
    std::vector<GameRoll> mGameOption;
    size_t mGameSelected = 0;
    std::vector<std::pair<std::string, bool>> mPlayerOption;
    std::string mRollText;
    std::string mRollGame;
};

class THGuiRollGame {
public:
    THRoll mRoll;

    bool first_roll;
    bool is_rolling;
    float angle_fin;
    float angle_last;
    float time;
    float time_tot;

    THGuiRollGame()
    {
        is_rolling = false;
        first_roll = true;
        angle_fin = -0.1f;
        angle_last = -0.1f;
        time_tot = 240.0f;
        time = 0.0f;

        mRndTextGen = GetRndGenerator(1u, 20u);
        for (auto& game : gGameRoll) {
            if (game.type == ROLL_MAIN) {
                game.selected = true;
            }
            mGameOption[game.type].push_back(game);
        }
        UpdateRoll();
        if (mRndTextGen() == 1) {
            mRollText = S(THPRAC_GAMEROLL_EENY_MEENY);
        } else {
            mRollText = S(THPRAC_GAMEROLL_ROLL);
        }
    }
    
    bool GuiGameTypeChkBox(const char* text, int idx)
    {
        if (ImGui::Checkbox(text, &(mGameTypeOpt[idx]))) {
            for (auto& game : mGameOption[idx]) {
                game.selected = mGameTypeOpt[idx];
            }
            return true;
        }
        return false;
    }
    void UpdateRoll()
    {
        std::vector<std::string> names;
        for (auto& gameType : mGameOption) {
            for (auto& game : gameType) {
                if (game.selected)
                    names.push_back(S(game.title));
            }
        }
        mRoll.InitRoll("rndgame", names, false);
    }
    bool GuiUpdate()
    {
        bool result = true;
        if (ImGui::Button(S(THPRAC_BACK))) {
            result = false;
        }
        ImGui::SameLine();
        GuiCenteredText(S(THPRAC_TOOLS_RND_GAME));
        ImGui::Separator();

        bool changed = false;
        if (ImGui::CollapsingHeader(S(THPRAC_TOOLS_RND_GAME)))
        {
            int i = 0;
            for (auto& gameType : mGameOption) {
                bool allSelected = true;
                ImGui::Columns(6, 0, false);
                for (auto& game : gameType) {
                    changed |= ImGui::Checkbox(S(game.title), &game.selected);
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
            changed |= GuiGameTypeChkBox(S(THPRAC_TOOLS_RND_GAME_PC98), 0);
            ImGui::SameLine();
            changed |= GuiGameTypeChkBox(S(THPRAC_GAMES_MAIN_SERIES), 1);
            ImGui::SameLine();
            changed |= GuiGameTypeChkBox(S(THPRAC_GAMES_SPINOFF_STG), 2);
            ImGui::SameLine();
            changed |= GuiGameTypeChkBox(S(THPRAC_GAMES_SPINOFF_OTHERS), 3);
        }

        std::vector<GameRoll> candidate;
        for (auto& gameType : mGameOption) {
            for (auto& game : gameType) {
                if (game.selected) {
                    candidate.push_back(game);
                }
            }
        }

        if (changed) {
            UpdateRoll();
        }

        float angle_cur;
        if (is_rolling) {
            angle_cur = mRoll.MInterpolation(time / (float)time_tot, angle_last, angle_fin);
            if (time < time_tot) {
                time++;
            } else {
                is_rolling = false;
            }
        } else {
            angle_cur = angle_fin;
        }
        int roll_result = mRoll.CalcIdx(angle_cur);

        auto wndSize = ImGui::GetWindowSize();
        mRoll.GuiDraw(angle_cur, roll_result, wndSize.y * 0.15f);

        if (!first_roll && !is_rolling) {
            auto result = roll_result;
            if (result != -1 && result < candidate.size()){
                char outputStr[256];
                sprintf_s(outputStr, S(THPRAC_TOOLS_ROLL_RESULT), S(candidate[result].title));
                mRollText = outputStr;
                mRollResult = candidate[result];
            }
        }

        if (GuiButtonRelCentered(mRollText.c_str(), 0.9f, ImVec2(1.0f, 0.08f))) {
            first_roll = false;
            is_rolling = true;
            time = 0;
            angle_last = angle_fin - floorf(angle_fin / (2.0f * std::numbers::pi)) * 2.0f * std::numbers::pi;
            angle_fin = (mRoll.GetRandomFloat() * 2.0f * std::numbers::pi) + 30.0f * std::numbers::pi;
        }
        if (mRollText != S(THPRAC_GAMEROLL_ROLL) && ImGui::BeginPopupContextItem("##roll_game_popup")) {
            if (mRollResult.playerSelect) {
                if (ImGui::Selectable(S(THPRAC_TOOLS_RND_TURNTO_PLAYER))) {
                    mRollText = S(THPRAC_GAMEROLL_ROLL);
                    LauncherToolsGuiSwitch(mRollResult.name);
                }
            }
            if (ImGui::Selectable(S(THPRAC_TOOLS_RND_TURNTO_GAME))) {
                GuiLauncherMainSwitchTab(S(THPRAC_GAMES));
                LauncherGamesGuiSwitch(mRollResult.name);
            }
            ImGui::EndPopup();
        }

        if (!result) {
            if (mRndTextGen() == 1) {
                mRollText = S(THPRAC_GAMEROLL_EENY_MEENY);
            } else {
                mRollText = S(THPRAC_GAMEROLL_ROLL);
            }
        }
        return result;
    }

private:
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
        auto width = GetWidthRel(S(THPRAC_TOOLS_APPLY_THPRAC), 2.0f);
        if (CenteredButton(S(THPRAC_TOOLS_APPLY_THPRAC), 0.2f, width)) {
            FindOngoingGame(true);
        }
        if (CenteredButton(S(THPRAC_TOOLS_RND_GAME), 0.3f, width)) {
            mGuiUpdFunc = [&]() { return mGuiRollGame.GuiUpdate(); };
        }
        if (CenteredButton(S(THPRAC_TOOLS_RND_PLAYER), 0.4f, width)) {
            mGuiUpdFunc = [&]() { return mGuiRollPlayer.GuiUpdate(); };
        }
        if (CenteredButton(S(THPRAC_TOOLS_REACTION_TEST), 0.5f, width)) {
            mGuiUpdFunc = [&]() { return mGuiReactionTest.GuiUpdate(); };
        }
        if (CenteredButton(S(THPRAC_TOOLS_ROLLF_FROM_FILE), 0.6f, width)) {
            mGuiUpdFunc = [&]() { return mGuiRollAll.GuiUpdate(); };
        }
        if (CenteredButton(S(THPRAC_KILL_ALL_GAME), 0.7f, width)) {
            auto game_killed = THClearGame();
            static char chs[256];
            sprintf_s(chs, S(THPRAC_KILL_ALL_GAME_DONE), game_killed.size());
            std::wstringstream ss;
            ss << utf8_to_utf16(chs) << L"\n";
            int n = 0;
            for (const auto& path : game_killed) {
                auto path_a = path;
                if (path_a.length() > 60) {
                    path_a = path_a.substr(0, 57) + L"...";
                }
                ss << L" - " << path_a << L"\n";
                n++;
                if (n > 32) {
                    //???
                    ss << "...";
                    break;
                }
            }
            MessageBoxW(NULL, ss.str().c_str(), L"done", MB_OK);
        }
        ImGui::SameLine();
        HelpMarker(S(THPRAC_KILL_ALL_GAME_ALERT));
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
    THGuiTestReactionTest mGuiReactionTest;
    THGuiRollAll mGuiRollAll;
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
