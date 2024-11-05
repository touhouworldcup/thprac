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
            ImGui::NewLine();

            ImGui::Separator();

            if (mGameTimeCur_ns >= 1000000000ll * 3600 * (double)mTooLongGamePlay_hour) {
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