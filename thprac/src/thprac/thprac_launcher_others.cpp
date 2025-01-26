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
#include <thread>
#include <numbers>
#include <fstream>
#include <sstream>
#include <format>

namespace THPrac {

bool CheckIfAnyGame2() // = THPrac_main.cpp: CheckIfAnyGame()
{
    static const char* mutexStrA = nullptr;
    static const WCHAR* mutexStrW = nullptr;
    if (mutexStrA) { // less CPU
        auto resultA = OpenMutexA(SYNCHRONIZE, FALSE, mutexStrA);
        if (resultA) {
            CloseHandle(resultA);
            return true;
        }
    }
    if (mutexStrW) {
        auto resultW = OpenMutexW(SYNCHRONIZE, FALSE, mutexStrW);
        if (resultW) {
            CloseHandle(resultW);
            return true;
        }
    }
    for (auto& gamesig : gGameDefs) {
        if (gamesig.mutexStr) {
            auto result = OpenMutexA(SYNCHRONIZE, FALSE, gamesig.mutexStr);
            if (result) {
                mutexStrA = gamesig.mutexStr;
                CloseHandle(result);
                return true;
            }
        } else if (gamesig.mutexWStr) {
            auto result = OpenMutexW(SYNCHRONIZE, FALSE, gamesig.mutexWStr);
            if (result) {
                mutexStrW = gamesig.mutexWStr;
                CloseHandle(result);
                return true;
            }
        }
    }
    mutexStrA = nullptr;
    mutexStrW = nullptr;
    return false;
}



class THDrawLuck {
    THDrawLuck() { memset(name, 0, sizeof(name)); }
    SINGLETON(THDrawLuck);

private:
    int cur_year;
    int cur_month;
    int cur_day;

    int draw_luck_day_count=0;
    int draw_luck_Y = 0;
    int draw_luck_M = 0;
    int draw_luck_D = 0;

    bool is_drawed = 0;
    int luck_value = -1;
    int better1 = -1;
    int better2 = -1;
    int better3 = -1;
    int betternot1 = -1;
    int betternot2 = -1;
    int betternot3 = -1;
    char name[20];
    char name_orig[20];

    void LoadSave()
    {
        if (!LauncherSettingGet("draw_day_count", draw_luck_day_count))
            draw_luck_day_count = 0;
        std::string time_m;
        if (!LauncherSettingGet("draw_day_max", time_m)) {
            draw_luck_Y = draw_luck_M = draw_luck_D = 0;
        } else{
            std::stringstream ss(time_m);
            ss >> draw_luck_Y >> draw_luck_M >> draw_luck_D;
        }
        std::string namestr;
        memset(name_orig, 0, sizeof(name_orig));
        memset(name, 0, sizeof(name));
        if (LauncherSettingGet("luck_name", namestr)) {
            for (int i = 0; i < sizeof(name) - 1 && i < namestr.size(); i++)
            {
                name[i] = namestr[i];
                name_orig[i] = namestr[i];
            }
        }
        std::string luck_desc;
        if (LauncherSettingGet("luck_desc", luck_desc)) {
            std::stringstream ss(luck_desc);
            ss >> better1 >> better2 >> better3 >> betternot1 >> betternot2 >> betternot3;
        }
        LauncherSettingGet("luck_value", luck_value);
    }

    void SaveSave()
    {
        LauncherSettingSet("draw_day_count", draw_luck_day_count);
        std::stringstream ss;
        ss << draw_luck_Y << " " << draw_luck_M << " "<< draw_luck_D;
        LauncherSettingSet("draw_day_max", ss.str());
        ss=std::stringstream();
        ss << better1 << " " << better2 << " " << better3 << " " << betternot1 << " " << betternot2 << " " << betternot3;
        LauncherSettingSet("luck_desc", ss.str());
        LauncherSettingSet("luck_name", std::string(name));
        LauncherSettingSet("luck_value", luck_value);
    }
    
    bool TestIsDrawed(bool update)
    {
        time_t now = time(0);
        tm* currentDate = localtime(&now);
        cur_year = 1900 + currentDate->tm_year;
        cur_month = 1 + currentDate->tm_mon;
        cur_day = currentDate->tm_mday;
        if (cur_year == draw_luck_Y && cur_month == draw_luck_M && cur_day == draw_luck_D) {
            if (luck_value >= 0 && better1 >= 0 && better2 >= 0 && betternot1 >= 0 && betternot2 >= 0 && better3>=0 && betternot3>=0){
                return true;
            }
        }
        if (update) {
            draw_luck_Y = cur_year;
            draw_luck_M = cur_month;
            draw_luck_D = cur_day;
        }
        return false;
    }
private:
public:
    void Init(){
        LoadSave();
    }
    void DrawTextScaled(const char* ch, float relPos, float scale, const ImVec4& col, float space1 = 0.0f, float space2 = 0.0f)
    {
        if (space1 != 0.0f)
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + space1 * ImGui::GetTextLineHeight());
        ImScaleStart();
        ImGui::PushStyleColor(ImGuiCol_Text, col);
        if (relPos != 0.0f) {
            auto wndSize = ImGui::GetWindowSize();
            auto textSz = ImGui::CalcTextSize(ch);
            ImGui::SetCursorPosX(wndSize.x * relPos - textSz.x * 0.5f);
        }
        ImGui::TextUnformatted(ch);
        ImGui::PopStyleColor();
        ImScaleEnd(scale, scale);
        if (space2 != 0.0f)
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + space2 * ImGui::GetTextLineHeight());
    }
    void DrawTextScaled2(const char* ch1, const char* ch2, float relPos, float scale, const ImVec4& col, float space1=0.0f, float space2 = 0.0f)
    {
        if (space1 != 0.0f)
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + space1 * ImGui::GetTextLineHeight());
        ImScaleStart();
        ImGui::PushStyleColor(ImGuiCol_Text, col);
        if (relPos != 0.0f) {
            auto wndSize = ImGui::GetWindowSize();
            auto textSz1 = ImGui::CalcTextSize(ch1);
            auto textSz2 = ImGui::CalcTextSize(ch2);
            auto textSz3 = ImGui::CalcTextSize("  ");
            ImGui::SetCursorPosX(wndSize.x * relPos - textSz1.x * 0.5f - textSz2.x * 0.5f - textSz3.x * 0.5f);
        }
        ImGui::Text("%s: %s", ch1,ch2);
        ImGui::PopStyleColor();
        ImScaleEnd(scale, scale);
        if (space2 != 0.0f)
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + space2 * ImGui::GetTextLineHeight());
    }
    void Gui()
    {
        if (Gui::LocaleGet()==0) {// chinese special
            auto charfilter = [](ImGuiInputTextCallbackData* data) -> int {
                static std::string allowed = "+-=.,!?@:;[]()_/{}|~^#$%&* ";
                if (data->EventFlag == ImGuiInputTextFlags_CallbackCharFilter) {
                    if ((data->EventChar >= '0' && data->EventChar <= '9') || 
                        (data->EventChar >= 'A' && data->EventChar <= 'Z')|| 
                        (data->EventChar >= 'a' && data->EventChar <= 'z')
                    ){
                        return 0;
                    } else {
                        for (auto ch : allowed)
                            if (data->EventChar == ch)
                                return 0;
                        data->EventChar = 0;
                        return 1;
                    }
                }
                return 0;
            };
            is_drawed = TestIsDrawed(false);
            static char chs[255];

            sprintf_s(chs, S(THPRAC_OTHER_TODAY), cur_year, cur_month, cur_day);
            DrawTextScaled(chs, 0.5f, 2.0f, { 1, 1, 1, 1 }, 0.5f, 0.5f);

            if (!is_drawed) {
                ImGui::InputText(S(THPRAC_OTHER_NAME), name, sizeof(name) - 1, ImGuiInputTextFlags_::ImGuiInputTextFlags_CallbackCharFilter, charfilter);
                if (strnlen_s(name,sizeof(name))>0) {
                    auto wndSize = ImGui::GetWindowSize();
                    ImGui::SetCursorPosX(wndSize.x * (0.5f-0.15f));
                    if (ImGui::Button(S(THPRAC_OTHER_CLOCK_IN), { wndSize.x * 0.3f, 150.0f }))
                    {
                            is_drawed = TestIsDrawed(true);
                            int name_s = 0;
                            for (int i = 0; i < sizeof(name); i++) {
                                if (name[i] == 0)
                                    break;
                                name_s *= 114;
                                name_s ^= name[i] * name[i];
                            }
                            std::default_random_engine rand((draw_luck_D * draw_luck_D * 114) ^ (draw_luck_Y * 514) ^ (draw_luck_M * 1919) ^ (name_s * 810));

                            // idk why the dev tool made arr 1 element larger
                            std::uniform_int_distribution<int32_t> rand_luck_value(0, ARRAYSIZE(LUCK_RANGE) - 2);
                            std::vector<int> ld1, ld2;
                            for (int i = 0; i <= ARRAYSIZE(LUCK_DESC_1) - 2; i++)
                                ld1.push_back(i);
                            for (int i = 0; i <= ARRAYSIZE(LUCK_DESC_2) - 2; i++)
                                ld2.push_back(i);
                            std::shuffle(ld1.begin(), ld1.end(), rand);
                            std::shuffle(ld2.begin(), ld2.end(), rand);

                            luck_value = rand_luck_value(rand);
                            better1 = ld1[0];
                            betternot1 = ld1[1];
                            better3 = ld1[2];
                            betternot3 = ld1[3];

                            better2 = ld2[0];
                            betternot2 = ld2[1];
                            if (strcmp(name_orig, name) != 0)
                                draw_luck_day_count = 0;
                            draw_luck_day_count++;
                            SaveSave();
                    }
                }
            }
            if (is_drawed)
            {
                DrawTextScaled(std::format("{}{}",name,S(THPRAC_OTHER_DRAW_LUCK)).c_str(), 0.5f, 1.0f, { 1, 0.7, 0.7, 1 },0.0f,1.0f);
                float r, g, b;
                ImGui::ColorConvertHSVtoRGB(0.0f, 1.0f - (luck_value / (float)(ARRAYSIZE(LUCK_RANGE) - 2)), 1.0f - 0.6f*(luck_value / (float)(ARRAYSIZE(LUCK_RANGE) - 2)), r, g, b);
                DrawTextScaled(S(LUCK_RANGE[luck_value]), 0.5f, 2.0f, { r,g,b, 1 },0.125f);
                ImGui::NewLine();
                if (luck_value == 0)
                {
                    DrawTextScaled(S(THPRAC_OTHER_LUCK_BETTER_ALL), 0.5f, 1.25f, { r, g, b, 1 },0.25f,0.0f);
                }else if (luck_value == ARRAYSIZE(LUCK_RANGE) - 2){
                    DrawTextScaled(S(THPRAC_OTHER_LUCK_BETTER_NOT_ALL), 0.5f, 1.25f, { r, g, b, 1 }, 0.25f,0.0f);
                } else {
                    float r1, g1, b1, r2, g2, b2;
                    ImGui::Columns(2);
                    ImGui::ColorConvertHSVtoRGB(0.0f, 1.0f, 1.0f, r1, g1, b1);
                    ImGui::ColorConvertHSVtoRGB(0.0f, 0.0f, 0.95f, r2, g2, b2);
                    float sz_text1 = 1.25f;
                    float sz_text2 = 1.0f;
                    float space1 = 0.5f;
                    float space2 = 0.1f;
                    DrawTextScaled2(S(THPRAC_OTHER_LUCK_BETTER), S(LUCK_DESC_1[better1]), 0.25f, sz_text1, { r1, g1, b1, 1 }, space1);
                    ImGui::NextColumn();
                    DrawTextScaled2(S(THPRAC_OTHER_LUCK_BETTER_NOT), S(LUCK_DESC_1[betternot1]), 0.75f, sz_text1, { r2, g2, b2, 1 }, space1);
                    ImGui::NextColumn();

                    DrawTextScaled(S(LUCK_DESC_1A[better1]), 0.25f, sz_text2, { r1, g1, b1, 1 },0.0f, space2);
                    ImGui::NextColumn();
                    DrawTextScaled(S(LUCK_DESC_1B[betternot1]), 0.75f, sz_text2, { r2, g2, b2, 1 }, 0.0f, space2);
                    ImGui::NextColumn();

                    DrawTextScaled2(S(THPRAC_OTHER_LUCK_BETTER), S(LUCK_DESC_1[better3]), 0.25f, sz_text1, { r1, g1, b1, 1 }, space1);
                    ImGui::NextColumn();
                    DrawTextScaled2(S(THPRAC_OTHER_LUCK_BETTER_NOT), S(LUCK_DESC_1[betternot3]), 0.75f, sz_text1, { r2, g2, b2, 1 }, space1);
                    ImGui::NextColumn();

                    DrawTextScaled(S(LUCK_DESC_1A[better3]), 0.25f, sz_text2, { r1, g1, b1, 1 }, 0.0f, space2);
                    ImGui::NextColumn();
                    DrawTextScaled(S(LUCK_DESC_1B[betternot3]), 0.75f, sz_text2, { r2, g2, b2, 1 }, 0.0f, space2);
                    ImGui::NextColumn();

                    DrawTextScaled2(S(THPRAC_OTHER_LUCK_BETTER), S(LUCK_DESC_2[better2]), 0.25f, sz_text1, { r1, g1, b1, 1 }, space1);
                    ImGui::NextColumn();
                    DrawTextScaled2(S(THPRAC_OTHER_LUCK_BETTER_NOT), S(LUCK_DESC_2[betternot2]), 0.75f, sz_text1, { r2, g2, b2, 1 }, space1);
                    ImGui::NextColumn();

                    DrawTextScaled(S(LUCK_DESC_2A[better2]), 0.25f, sz_text2, { r1, g1, b1, 1 }, 0.0f, space2);
                    ImGui::NextColumn();
                    DrawTextScaled(S(LUCK_DESC_2B[betternot2]), 0.75f, sz_text2, { r2, g2, b2, 1 }, 0.0f, space2);
                    ImGui::NextColumn();
                    ImGui::Columns(1);
                }
                sprintf_s(chs, S(THPRAC_OTHER_CONTINUE), draw_luck_day_count);
                DrawTextScaled(chs, 0.5f, 1.0f, { 1, 1, 1, 1 }, 1.0f, 0.6f);
            }
            ImGui::Separator();
        } else {

        }
    }
};

class THGameTimeRecorder {
    THGameTimeRecorder() { }
    SINGLETON(THGameTimeRecorder);

private:
    int64_t mGameTime_ns = -1;
    int64_t mGameTimeCur_ns = -1;
    int64_t mGameTimeTestGameOpen_ns = -1;
    int64_t mGameTimeTooLongSE_ns = -1;
    bool mUpdateGameTime = true;

    LARGE_INTEGER mLastTime;
    LARGE_INTEGER mFreq;

    float mTooLongGamePlay_hour = 3.0f;
    float mTooLongGamePlaySE_sec = 10.0f;
    bool mEnableRecordGameTime = false;
    bool mEnableTooLongGamePlaySE = false;
    std::thread mUpdateThread;

private:
public: 
    static void UpdateGameTime(){
        THGameTimeRecorder& thiz = THGameTimeRecorder::singleton();
        while (thiz.mUpdateGameTime) {
            if (thiz.mGameTimeCur_ns >= 1000000000ll * 3600 * (double)thiz.mTooLongGamePlay_hour) {
                if (thiz.mEnableTooLongGamePlaySE && thiz.mGameTimeTooLongSE_ns >= thiz.mTooLongGamePlaySE_sec * 1000000000ll && thiz.mGameTimeTooLongSE_ns >= 5 * 1000000000ll && // mintime: 5sec
                    CheckIfAnyGame2()) {
                    PlaySoundW(L"SE.wav", NULL, SND_FILENAME | SND_ASYNC);
                    thiz.mGameTimeTooLongSE_ns = 0;
                }
            }
            static bool is_game_open = false;
            Sleep(16);
            if (thiz.mGameTimeTestGameOpen_ns > 1000000000) { // test every second
                is_game_open = CheckIfAnyGame2();
                thiz.mGameTimeTestGameOpen_ns = 0;
            }
            LARGE_INTEGER cur_time;
            QueryPerformanceCounter(&cur_time);
            int64_t passed_time = (((double)(cur_time.QuadPart - thiz.mLastTime.QuadPart)) / (double)(thiz.mFreq.QuadPart)) * 1e9;
            if (is_game_open) {
                thiz.mGameTime_ns += passed_time;
                thiz.mGameTimeCur_ns += passed_time;
            }
            thiz.mGameTimeTestGameOpen_ns += passed_time;
            thiz.mGameTimeTooLongSE_ns += passed_time;
            thiz.mLastTime = cur_time;
        }
        LauncherSetGameTime(thiz.mGameTime_ns);
    }
    bool IsEnabled()
    {
        return mEnableRecordGameTime;
    }
    void UpdateGameTimeRecord()
    {
        LauncherSettingGet("gameTimeTooLong_Time", mTooLongGamePlay_hour);
        LauncherSettingGet("gameTimeTooLong_SE", mEnableTooLongGamePlaySE);
        LauncherSettingGet("gameTimeTooLong_SE_repeat", mTooLongGamePlaySE_sec);
    }
    void StartGameTimeRecord()
    {
        
        LauncherSettingGet("recordGameTime", mEnableRecordGameTime);
        if (mEnableRecordGameTime) { // game counter
            mGameTime_ns = LauncherGetGameTime();
            if (mGameTime_ns < 0)
                mGameTime_ns = 0;
            mGameTimeCur_ns = 0;
            mGameTimeTestGameOpen_ns = 0;
            mGameTimeTooLongSE_ns = 0;
            mUpdateGameTime = true;
            QueryPerformanceFrequency(&mFreq);
            QueryPerformanceCounter(&mLastTime);
            mUpdateThread = std::thread(UpdateGameTime);
        }
    }
    void StopGameTimeRecord()
    {
        if (mEnableRecordGameTime) {
            mUpdateGameTime = false;
            mUpdateThread.join();
        }
    }
    
    void Gui()
    {
        if (mEnableRecordGameTime) {
            ImGui::Text(S(THPRAC_GAME_TIME_TOTAL));
            ImGui::TextWrapped(GetTime_HHMMSS(mGameTime_ns > 0 ? mGameTime_ns : 0).c_str());
            ImGui::TextWrapped(GetTime_YYMMDD_HHMMSS(mGameTime_ns > 0 ? mGameTime_ns : 0).c_str());
            ImGui::TextWrapped("(%lld ns)", mGameTime_ns > 0 ? mGameTime_ns : 0);

            ImGui::NewLine();
            ImGui::Separator();

            float r, g, b;
            int gametimecur_hour = (mGameTimeCur_ns / (1000000000ll * 3600ll)) % 24;
            ImGui::ColorConvertHSVtoRGB(gametimecur_hour / 24.0f, 1, 1, r, g, b);
            ImGui::PushStyleColor(ImGuiCol_Text, { r, g, b, 1 });
            ImGui::Text(S(THPRAC_GAME_TIME_CURRENT));
            ImGui::PopStyleColor();

            int gametimecur_minute = (mGameTimeCur_ns / (1000000000ll * 60ll)) % 60;
            ImGui::ColorConvertHSVtoRGB(gametimecur_minute / 60.0f, 1, 1, r, g, b);
            ImGui::PushStyleColor(ImGuiCol_Text, { r, g, b, 1 });
            ImGui::TextWrapped(GetTime_HHMMSS(mGameTimeCur_ns > 0 ? mGameTimeCur_ns : 0).c_str());
            ImGui::PopStyleColor();

            int gametimecur_sec = (mGameTimeCur_ns / (1000000000ll)) % 60;
            ImGui::ColorConvertHSVtoRGB(gametimecur_sec / 60.0f, 1, 1, r, g, b);
            ImGui::PushStyleColor(ImGuiCol_Text, { r, g, b, 1 });
            ImGui::TextWrapped(GetTime_YYMMDD_HHMMSS(mGameTimeCur_ns > 0 ? mGameTimeCur_ns : 0).c_str());
            ImGui::PopStyleColor();

            int gametimecur_frame = (mGameTimeCur_ns / (16666666ll)) % 60;
            ImGui::ColorConvertHSVtoRGB(gametimecur_frame / 60.0f, 1, 1, r, g, b);
            ImGui::PushStyleColor(ImGuiCol_Text, { r, g, b, 1 });
            ImGui::TextWrapped("(%lld ns)", mGameTimeCur_ns);
            ImGui::PopStyleColor();
            if (ImGui::Button(S(THPRAC_GAME_TIME_RESET)))
            {
                mGameTimeCur_ns = 0;
            }
            ImGui::NewLine();
            ImGui::Separator();

            if (mGameTimeCur_ns >= 1000000000ll * 3600 * (double)mTooLongGamePlay_hour) {
                auto y_orig=ImGui::GetCursorPosY();
                ImGui::SetCursorPosY(ImGui::GetWindowHeight()*0.5f);
                static float h = 0.0f, vt = 0.0f, angle = 0.0f;
                h += 0.02f;
                if (h >= 1.0f)
                    h = 0.0f;
                vt += 0.01f;
                angle += 0.1f;
                ImScaleStart();
                ImRotateStart();
                ImGui::NewLine();
                ImGui::ColorConvertHSVtoRGB(h, sinf(vt) * 0.3f + 0.5f, 1.0f, r, g, b);
                ImGui::PushStyleColor(ImGuiCol_Text, { r, g, b, 1 });
                auto wndSize = ImGui::GetWindowSize();
                auto textSz = ImGui::CalcTextSize(S(THPRAC_GAME_TIME_LONG));
                ImGui::SetCursorPosX(wndSize.x * 0.5f - textSz.x * 0.5f);
                ImGui::TextUnformatted(S(THPRAC_GAME_TIME_LONG));
                ImGui::PopStyleColor();
                constexpr auto HALF_PI = static_cast<float>(std::numbers::pi / 2.0);
                ImRotateEnd(HALF_PI - sinf(-1.23f * angle + HALF_PI / 2.0f) * 0.12f);
                ImScaleEnd(2.3f + sinf(angle) * 0.8f, 2.3f + cosf(angle) * 0.8f);
                ImGui::SetCursorPosY(y_orig);
            }
        } else {
            ImGui::Text(S(THPRAC_ENABLE_GAMETIME_RECORD));
        }
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
        THDrawLuck::singleton().Gui();
        THGameTimeRecorder::singleton().Gui();
    }
};
bool THPrac::LauncherOthersGuiUpd()
{
    THOthersGui::singleton().GuiUpdate();
    return true;
}

void LauncherOthersInit()
{
    THGameTimeRecorder::singleton().StartGameTimeRecord();
    THDrawLuck::singleton().Init();
}

void LauncherOthersUpdate()
{
    THGameTimeRecorder::singleton().UpdateGameTimeRecord();
}

void LauncherOthersDestroy()
{
    THGameTimeRecorder::singleton().StopGameTimeRecord();
}

}