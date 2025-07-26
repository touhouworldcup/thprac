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
class THGuiRollAll {
private:
    char mRollName[256] = { 0 };
    std::vector<std::string> mNames;
    std::vector<float> mProbs;
    std::vector<float> mProbs2;
    std::vector<ImVec4> mColors;
    std::vector<ImVec4> mColors2;
    float mTotWeight;

    std::random_device mRandDevice;
    std::default_random_engine mRandEngine;
    bool mRandColor;

public:
    float GetRandomFloat()
    {
        static std::uniform_real_distribution<float> rand_value(0.0f,1.0f);
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
    THGuiRollAll()
        : mRandEngine(mRandDevice())
        , mRandColor(false)
    {
    }
    void InitRoll(int remove,bool change_color)
    {
        if (mProbs.size() < mNames.size()) {
            int d = mNames.size() - mProbs.size();
            for (int i = 0; i < d; i++)
                mProbs.emplace_back(1);
        }
        if (mNames.size() == 0)
            return;
        if (remove >=0 && remove<mNames.size()){
            mNames.erase(mNames.begin() + remove);
            mProbs.erase(mProbs.begin() + remove);
            mColors.erase(mColors.begin() + remove);
            mColors2.erase(mColors2.begin() + remove);
        }
        if (mNames.size() == 0)
            return;

        float tot = 0.0f;
        for (int i = 0; i < mNames.size(); i++) tot += mProbs[i];

        mTotWeight = tot;
        mProbs2 = {};
        if (change_color) {
            mColors = {};
            mColors2 = {};
        }
        mProbs2.push_back(0.0f);
        tot = 0.0f;

        int hi=0, si=255, vi=255;
        for (int i = 0; i < mNames.size(); i++) {
            tot += mProbs[i];
            mProbs2.push_back(tot / mTotWeight);
            if (change_color)
            {
                
                float r, g, b, h, s, v;
                //256=2^8
                if (!mRandColor)
                {
                    if (mNames.size() > 100)
                        hi += 7;
                    else if (mNames.size() > 50)
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
                }else{
                    h = GetRandomFloat();
                    s = GetRandomFloat() * 0.4f + 0.6f;
                    v = GetRandomFloat() * 0.4f + 0.6f;
                }
                ImGui::ColorConvertHSVtoRGB(h, s, v, r, g, b);
                mColors.push_back({ r, g, b, 1.0f });
                ImGui::ColorConvertHSVtoRGB(h, s, v - 0.3f, r, g, b);
                mColors2.push_back({ r, g, b, 1.0f });
            }
        }
    }
    void LoadRoll()
    {
        
        mNames = {};
        mProbs = {};
        
        std::wstring csv_filename = LauncherWndFileSelect(nullptr, L"csv(*.csv)\0*.csv\0*.*\0\0");
        std::string name = utf16_to_mb(GetNameFromFullPath(csv_filename).c_str(), CP_UTF8);
        strcpy_s(mRollName, name.c_str());
        rapidcsv::Document doc(utf16_to_mb(csv_filename.c_str(), CP_ACP), rapidcsv::LabelParams(0, -1));
        mNames = doc.GetColumn<std::string>(0);
        mProbs = doc.GetColumn<float>(1);
        InitRoll(-1, true);
    }
    void LoadRollWaifu()
    {
        mNames = {};
        mProbs = {};
        mNames = waifus;
        mProbs.resize(mNames.size(),1);
        InitRoll(-1, true);
    }
    bool DrawPie(ImDrawList* p, ImVec2 mid, float radius, float angle1, float angle2, uint32_t col_fill, uint32_t col_fill2, uint32_t col_line)
    {
        bool res = false;
        std::vector<ImVec2> points;
        points.push_back({ cosf(angle1), sinf(angle1) });
        float dangle = std::numbers::pi * 0.005f;
        for (float angle = angle1+dangle; angle < angle2; angle += dangle)
            points.push_back({ cosf(angle), sinf(angle) });
        points.push_back({ cosf(angle2), sinf(angle2) });

        auto mp = ImGui::GetMousePos();
        if (hypotf(mp.y - mid.y, mp.x - mid.x)<radius)
        {
            float mangle = atan2(mp.y - mid.y, mp.x - mid.x);
            mangle += (2.0f * std::numbers::pi) * ceilf((angle1 - mangle) / (2.0f * std::numbers::pi));
            res = mangle < angle2;
        }
        for (int i = 0; i < points.size() - 1; i++) {
            ImVec2 p2 = { mid.x + points[i].x * radius, mid.y + points[i].y * radius };
            ImVec2 p3 = {mid.x + points[i + 1].x * radius, mid.y + points[i + 1].y * radius};
            p->AddTriangleFilled(mid,p2,p3,res?col_fill2:col_fill);
            p->AddTriangle(mid,p2,p3,res?col_fill2:col_fill,1.33f);
        }
        
        p->AddLine(mid, { mid.x + points[0].x * radius, mid.y + points[0].y * radius }, col_line, 2.0f);
        return res;
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
                for (int i = 0; i < mNames.size(); i++)
                {
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("%s", mNames[i].c_str());
                    ImGui::TableNextColumn();
                    ImGui::Text("%.2f", mProbs[i]);
                }
                ImGui::EndTable();
            }
        }
        ImGui::Separator();
        static bool roll = false;
        static bool has_result = false;
        static int roll_time = 0;
        static float roll_rand=0.0f;
        static int roll_result=0;
        static int tot_time = 240;
        static float last_angle = 0.0f;
        if (ImGui::Button(S(THPRAC_TOOLS_ROLLF_START)) && mNames.size() > 0)
        {
            roll = true;
            has_result = false;
            roll_time = 0;
            roll_rand = GetRandomFloat();
        }
        ImGui::SameLine();
        if (ImGui::Button(S(THPRAC_TOOLS_ROLLF_REMOVE_CUR)))
        {
            if (has_result == true){
                InitRoll(roll_result, false);
                roll = false;
                has_result = false;
            }
        }
        
        if (!roll){
            ImGui::SameLine();
            ImGui::Text(S(THPRAC_TOOLS_ROLLF_TIME));
            ImGui::SameLine();
            ImGui::SetNextItemWidth(150.0f);
            if (ImGui::DragInt("##roll time", &tot_time, 1.0f, 1, 1000))
                tot_time = std::clamp(tot_time, 1, 1000);
        }

        static float angle_add, angle_wraped;
        if (roll) {
            angle_add = MInterpolation(roll_time / (float)tot_time, last_angle, roll_rand * 2.0f * std::numbers::pi + 30.0f * std::numbers::pi);
            angle_wraped = angle_add - floorf(angle_add / (2.0f * std::numbers::pi)) * 2.0f * std::numbers::pi;
            if (roll_time < tot_time) {
                roll_time++;
            } else {
                roll = false;
                has_result = true;
                last_angle = angle_wraped;
            }
        }
        roll_result = -1;
        float roll_res_temp;
        if (has_result == false)
            roll_res_temp = angle_wraped / (2.0f * std::numbers::pi);
        else
            roll_res_temp = roll_rand;
        for (int i = 0; i < mNames.size(); i++) {
            if (mProbs2[i] <= roll_res_temp && mProbs2[i + 1] > roll_res_temp) {
                roll_result = i;
                break;
            }
        }
        if (roll_result == -1)
            roll_result = mNames.size() - 1;

        if (mNames.size() > 0 && roll_result >= 0 && roll_result < mNames.size()) {
            ImGui::SameLine();
            ImGui::Text(std::format("{}", mNames[roll_result]).c_str());
        }
        {
            ImVec2 p0 = ImGui::GetCursorScreenPos();
            ImVec2 csz = ImGui::GetContentRegionAvail();
            if (csz.y < 0)
                csz.y = 0;
            ImVec2 cmid = { p0.x + csz.x * 0.5f, p0.y + csz.y * 0.5f };
            ImVec2 p1 = { cmid.x + csz.x * 0.5f, cmid.y + csz.y * 0.5f};
            float hheight = std::min(csz.x,csz.y) * 0.95f * 0.5f;

            ImDrawList* p = ImGui::GetWindowDrawList();
            p->AddRectFilled(p0, p1, IM_COL32(50, 50, 50, 100));
            p->AddRect(p0, p1, IM_COL32(255, 255, 255, 100));
            p->PushClipRect(p0, p1);

           ImVec2 cir_cen = { hheight * 1.1f, cmid.y };
           if (mNames.size() > 0 && csz.y>0.0f)
           {
               {
                   // p->AddCircleFilled(cir_cen, hheight, ImGui::ColorConvertFloat4ToU32(col1));
                   p->AddCircle(cir_cen, hheight, 0xFFFFFFFF);
                   //
                   for (int i = 0; i < mNames.size(); i++) {
                       auto col1 = ImGui::ColorConvertFloat4ToU32(mColors[i]);
                       auto col2 = ImGui::ColorConvertFloat4ToU32(mColors2[i]);
                       auto res = DrawPie(p, cir_cen, hheight, mProbs2[i] * 2.0f * std::numbers::pi - angle_wraped, mProbs2[i + 1] * 2.0f * std::numbers::pi - angle_wraped, col1, col2, 0xFFFFFFFF);
                       if (res)
                           ImGui::SetTooltip("%s", mNames[i].c_str());
                   }
               }
               p->PopClipRect();
               ImVec2 tri_pos1 = { cir_cen.x + hheight * 0.75f, cir_cen.y };
               ImVec2 tri_pos2 = { cir_cen.x + hheight * 1.25f, cir_cen.y - hheight * 0.05f };
               ImVec2 tri_pos3 = { cir_cen.x + hheight * 1.25f, cir_cen.y + hheight * 0.05f };
               p->AddTriangleFilled(tri_pos1, tri_pos2, tri_pos3, 0xFFFFCCCC);
               p->AddTriangle(tri_pos1, tri_pos2, tri_pos3, 0xFFFFFFFF, 1.5f);
               if (roll_result >= 0 && roll_result < mNames.size()) {
                   p->AddText({ cir_cen.x + hheight * 1.3f, cir_cen.y - ImGui::GetTextLineHeight() * 0.5f }, 0xFFFFFFFF, std::format("{}", mNames[roll_result]).c_str());
               }
           }
        }
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

    ImVec2 colorBtnSz = ImVec2(ImGui::GetWindowWidth()*0.5f, ImGui::GetTextLineHeight() * 10.0f);
    ImVec2 colorBtnPos = ImVec2(ImGui::GetWindowWidth()*0.25f, ImGui::GetTextLineHeight() * 2.0f);
    DWORD colorBtnFlag = ImGuiColorEditFlags_::ImGuiColorEditFlags_NoTooltip;
    ImVec4 color_BeforeTimeReact = ImVec4(1, 0.2, 0.2, 1);
    ImVec4 color_AfterTimeReact = ImVec4(0, 1, 0.2, 1);
    ImVec4 color_WaitPress = ImVec4(1, 1, 0.2, 1);

    bool isKeyPressed = ImGui::IsKeyDown(37) || ImGui::IsKeyDown(38) || ImGui::IsKeyDown(39) || ImGui::IsKeyDown(40);
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
        if (ImGui::Button(S(THPRAC_TOOLS_REACTION_TEST_BEGIN))) {
            mTestState = WAIT_TIME;
            mCurTest = 1;
            if (mTestType == PRESS) {
                mWaitTime.QuadPart = mRndSeedGen() / 1000.0 * (double)mTimeFreq.QuadPart;
                mPressTime.QuadPart = mWaitTime.QuadPart + curTime.QuadPart;
            }
            mResults = {};
        }
    } break;
    
    case TOO_EARLY: {
        ImGui::Text(S(THPRAC_TOOLS_REACTION_TEST_TOO_EARLY));
        if (ImGui::Button(S(THPRAC_TOOLS_REACTION_TEST_NEXT_TEST)) || ImGui::IsKeyPressed(90) || ImGui::IsKeyPressed(16)) // press z(90)/shift(16)
        {
            mTestState = WAIT_TIME;
            mWaitTime.QuadPart = mRndSeedGen() / 1000.0 * (double)mTimeFreq.QuadPart;
            mPressTime.QuadPart = mWaitTime.QuadPart + curTime.QuadPart;
        }
    } break;
    case SHOW_RES: {
        if (mResults.size() != 0)
            ImGui::Text("%s(%d): %.1f ms (%.1f frame)", S(THPRAC_TOOLS_REACTION_TEST_RESULT), mCurTest, mResults[mResults.size() - 1], mResults[mResults.size() - 1] / 16.66667f);
        else
            ImGui::Text("%s(%d): ?????", S(THPRAC_TOOLS_REACTION_TEST_RESULT), mCurTest);
        if (mCurTest == mTestTime)
        {
            float avg = 0.0f;
            float maxv = 60.0f;
            float minv = 0.0f;
            for (auto x : mResults) {
                avg += fabsf(x);
                maxv = std::max(x, maxv);
                minv = std::min(x, minv);
            }
            avg /= (float)mTestTime;
            ImGui::Separator();
            ImGui::Text("%s: %s, %s", S(THPRAC_TOOLS_REACTION_MODE), mTestType == PRESS ? S(THPRAC_TOOLS_REACTION_MODE_DOWN) : S(THPRAC_TOOLS_REACTION_MODE_UP), 
                mShowProgressBar ? S(THPRAC_TOOLS_REACTION_MODE_PROGRESSBAR) : S(THPRAC_TOOLS_REACTION_MODE_NORMAL)
            );
            ImGui::Text("%s: %.1f ms (%.1f frame)", S(THPRAC_TOOLS_REACTION_TEST_RESULT_AVG), avg, avg / 16.66667f);
            
            ImGui::PlotHistogram(S(THPRAC_TOOLS_REACTION_TEST_RESULT), &mResults[0], mTestTime, 0, S(THPRAC_TOOLS_REACTION_TEST_RESULT), minv-10.0f, maxv+20.0f, ImVec2(0, 200.0));
    
            if (ImGui::Button(S(THPRAC_TOOLS_REACTION_TEST_NEXT_TEST)) || ImGui::IsKeyPressed(90) || ImGui::IsKeyPressed(16)) {
                mTestState = NOT_BEGIN;
            }
        } else {
            if (ImGui::Button(S(THPRAC_TOOLS_REACTION_TEST_NEXT_TEST)) || ImGui::IsKeyPressed(90) || ImGui::IsKeyPressed(16)) // press z
            {
                mCurTest++;
                mTestState = WAIT_TIME;
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
            }
            ImGui::Text("test %d", mCurTest);

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
                    mTestState = SHOW_RES;
                }
            }
        }else{
            if (mTestState == WAIT_TIME_PRESSED && curTime.QuadPart >= mPressTime.QuadPart) {
                mTestState = REACT_TIME;
            }
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
    mTestState = NOT_BEGIN;
    mTestType = PRESS;
    mTestTime = 5;
    mRndSeedGen = GetRndGenerator(1200u, 3500u);
    mShowProgressBar = false;
    mCurTest = 0;
    mResults = {};
}


class THGuiRollPlayer {
public:
    THGuiRollPlayer()
    {
        mRndSeedGen = GetRndGenerator(0u, UINT_MAX);
        mRndTextGen = GetRndGenerator(1u, 20u);

        if (mRndSeedGen() % 32 == 0 && strcmp(gGameRoll[32].name, "th19") == 0) {
            gGameRoll[32].playerSelect = "Reimu\0Marisa\0Sanae\0Ran\0Aunn\0Nazrin\0Seiran\0Orin\0Tsukasa\0Mamizou\0Yachie\0Saki\0Yuuma\0Suika\0Goku\0Enoko\0Chiyari\0Hisami\0Zanmu\0\0";
        }

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
            sprintf_s(outputStr, S(THPRAC_TOOLS_ROLL_RESULT), candidate[result].c_str());
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
        if (ImGui::Button(S(THPRAC_BACK))) {
            result = false;
        }
        ImGui::SameLine();
        GuiCenteredText(S(THPRAC_TOOLS_RND_PLAYER));
        ImGui::Separator();

        if (ImGui::BeginCombo(S(THPRAC_TOOLS_RND_PLAYER_GAME), mGameOption[mGameSelected].name, 0)) // The second parameter is the label previewed before opening the combo.
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
            if (ImGui::Selectable(S(THPRAC_TOOLS_RND_TURNTO_GAME))) {
                GuiLauncherMainSwitchTab(S(THPRAC_GAMES));
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
            sprintf_s(outputStr, S(THPRAC_TOOLS_ROLL_RESULT), candidate[result].name);
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
        if (ImGui::Button(S(THPRAC_BACK))) {
            result = false;
        }
        ImGui::SameLine();
        GuiCenteredText(S(THPRAC_TOOLS_RND_GAME));
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
        GuiGameTypeChkBox(S(THPRAC_TOOLS_RND_GAME_PC98), 0);
        ImGui::SameLine();
        GuiGameTypeChkBox(S(THPRAC_GAMES_MAIN_SERIES), 1);
        ImGui::SameLine();
        GuiGameTypeChkBox(S(THPRAC_GAMES_SPINOFF_STG), 2);
        ImGui::SameLine();
        GuiGameTypeChkBox(S(THPRAC_GAMES_SPINOFF_OTHERS), 3);

        if (GuiButtonRelCentered(mRollText.c_str(), 0.9f, ImVec2(1.0f, 0.08f))) {
            RollGame();
        }
        if (mRollText != "ROLL!" && ImGui::BeginPopupContextItem("##roll_game_popup")) {
            if (mRollResult.playerSelect) {
                if (ImGui::Selectable(S(THPRAC_TOOLS_RND_TURNTO_PLAYER))) {
                    mRollText = "ROLL!";
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
