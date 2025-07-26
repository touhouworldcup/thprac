#pragma once
#define NOMINMAX
#include <Windows.h>
#include <functional>
#include <vector>

namespace THPrac {
bool LauncherToolsGuiUpd();

class THGuiTestReactionTest {
    enum TestState {
        NOT_BEGIN,
        WAIT_TIME,
        WAIT_TIME_PRESSED,
        REACT_TIME,
        TOO_EARLY,
        SHOW_RES,
    };
    enum TestType {
        PRESS,
        RELEASE
    };

public:
    THGuiTestReactionTest();
    bool GuiUpdate(bool ingame = false);
    void Reset();

private:
    LARGE_INTEGER mTimeFreq;
    LARGE_INTEGER mPressTime;
    LARGE_INTEGER mWaitTime;

    TestState mTestState;
    TestType mTestType;
    int mTestTime;
    bool mShowProgressBar;

    int mCurTest;
    // float mCurTestWaitTimeMs;
    // float mCurTestWaitTimeTotMs;

    // float mCurTestReactionTimeMs;
    std::function<unsigned int(void)> mRndSeedGen;
    std::vector<float> mResults;
};

}
