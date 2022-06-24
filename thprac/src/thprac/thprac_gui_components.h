#pragma once
#include <imgui.h>
#include <vector>
#include <functional>
#include <initializer_list>
#include <utility>
#define NOMINMAX
#include <Windows.h>

#include "thprac_gui_impl_win32.h"
#include "thprac_gui_input.h"
#include "thprac_gui_locale.h"
#include "thprac_hook.h"
#include "utils/utils.h"

namespace THPrac {
namespace Gui {
    class GameGuiWnd {
    public:
        struct Viewport {
            DWORD X;
            DWORD Y;
            DWORD Width;
            DWORD Height;
        };
        struct StyleValue {
            ImGuiStyleVar style;
            ImVec2 value;
        };
        GameGuiWnd() noexcept
        {
            ImGuiIO& io = ImGui::GetIO();
            io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

            mLocale = (locale_t)-1;
        }

        static constexpr ImGuiWindowFlags STYLE_DEFAULT = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | 0;

        void Update();
       
        void Open();
        void Close();

        bool IsOpen();
        bool IsClosed();

        void SetViewport(void* ptr);
        void SetSize(float width, float height);
        void SetPos(float x, float y);
        void SetItemWidth(float item_width);
        void SetSizeRel(float width_prop, float height_prop);
        void SetPosRel(float x_prop, float y_prop);
        void SetItemWidthRel(float item_width_prop);
        void SetItemSpacing(float x, float y);
        void SetItemSpacingRel(float x_prop, float y_prop);
        void SetAutoSpacing(bool toggle);
        void AutoSize(ImVec2 size,
            const char* content_1 = nullptr, const char* content_2 = nullptr, const char* label_1 = nullptr, const char* label_2 = nullptr,
            float widiget_counts = 0.0f, ImVec2 size_max = { -1.0f, -1.0f }, ImVec2 size_min = { -1.0f, -1.0f });
        void AutoSize(float x, float y, const char* content = nullptr, const char* label = nullptr, float widigit_counts = 0.0f, float max_y = 0.0f);
        void AutoPos(float x, float y);
        void AutoItemWidth(const char* label);

        void SetStyle(ImGuiStyleVar style, float param1, float param2 = 0.0f);
        void SetWndFlag(ImGuiWindowFlags flags);
        void SetFade(float transparency, float step);
        void SetTitle(const char* title);
        void SetWndFoucs(bool toggle);

        void WndDebugOutput();

    protected:
        std::vector<StyleValue>::iterator GetStyleIt(ImGuiStyleVar style);

        virtual void OnLocaleChange() {};
        virtual void OnContentUpdate() {};
        virtual void OnPreUpdate() {};
        virtual void OnPostUpdate() {};

        int mStatus = 0;
        std::vector<StyleValue> mStyle;
        std::string mTitle;
        ImVec2 mSize;
        ImVec2 mPos;
        ImGuiWindowFlags mWndFlag = STYLE_DEFAULT;
        float mItemWidth = -100.0f;
        float mAlpha = 0.0f;
        float mAlphaMax = 0.0f;
        float mAlphaStep = 0.0f;
        bool mSizeFlag = true;
        bool mPosFlag = true;
        bool mFocus = false;
        Viewport* mViewport = nullptr;

    private:
        locale_t mLocale;
    };

    class PPGuiWnd : public GameGuiWnd {
    protected:
        void InitUpdFunc(
            std::function<void(void)>&& contentUpdFunc,
            std::function<void(void)>&& localeUpdFunc,
            std::function<void(void)>&& preUpdFunc,
            std::function<void(void)>&& postUpdFunc)
        {
            mContentUpdFuncTmp = mContentUpdFunc = contentUpdFunc;
            mLocaleUpdFuncTmp = mLocaleUpdFunc = localeUpdFunc;
            mPreUpdFuncTmp = mPreUpdFunc = preUpdFunc;
            mPostUpdFuncTmp = mPostUpdFunc = postUpdFunc;
        }
        void SetContentUpdFunc(std::function<void(void)>&& contentUpdFunc)
        {
            mContentUpdFuncTmp = contentUpdFunc;
        }
        void SetPreUpdFunc(std::function<void(void)>&& preUpdFunc)
        {
            mPreUpdFuncTmp = preUpdFunc;
        }
        void SetPostUpdFunc(std::function<void(void)>&& postUpdFunc)
        {
            mPostUpdFuncTmp = postUpdFunc;
        }
        void SetLocaleUpdFunc(std::function<void(void)>&& localeUpdFunc)
        {
            mLocaleUpdFuncTmp = localeUpdFunc;
        }
                
        void OnLocaleChange() override final
        {
            mLocaleUpdFunc();
        }
        void OnContentUpdate() override final
        {
            mContentUpdFunc();
        }
        void OnPreUpdate() override final
        {
            mContentUpdFunc = mContentUpdFuncTmp;
            mLocaleUpdFunc = mLocaleUpdFuncTmp;
            mPreUpdFunc = mPreUpdFuncTmp;
            mPostUpdFunc = mPostUpdFuncTmp;

            mPreUpdFunc();
        }
        void OnPostUpdate()
        {
            mPostUpdFunc();
        }

        int mIndicator = 0;

    private:
        std::function<void(void)> mContentUpdFunc = []() {};
        std::function<void(void)> mLocaleUpdFunc = []() {};
        std::function<void(void)> mPreUpdFunc = []() {};
        std::function<void(void)> mPostUpdFunc = []() {};

        std::function<void(void)> mContentUpdFuncTmp = []() {};
        std::function<void(void)> mLocaleUpdFuncTmp = []() {};
        std::function<void(void)> mPreUpdFuncTmp = []() {};
        std::function<void(void)> mPostUpdFuncTmp = []() {};
    };

    template <typename T, ImGuiDataType type>
    class GuiSlider {
    private:
        unsigned int mLabelRef = 0;
        char* mLabel = nullptr;
        T mValue = 0;
        T mValueMin = 0;
        T mValueMax = 0;
        T mStep = 1;
        T mStepMin = 1;
        T mStepMax = 1;
        T mStepX = 1;

    public:
        GuiSlider(const char* label, const T&& minimum, const T&& maximum,
            T step_min = 1, T step_max = 1, T step_x = 10)
            : mLabel(const_cast<char*>(label))
            , mValueMin(minimum)
            , mValueMax(maximum)
            , mStep(step_min)
            , mStepMin(step_min)
            , mStepMax(step_max)
            , mStepX(step_x)
        {
        }

        GuiSlider(unsigned int label_ref, const T&& minimum, const T&& maximum,
            T step_min = 1, T step_max = 1, T step_x = 10)
            : mLabelRef(label_ref)
            , mValueMin(minimum)
            , mValueMax(maximum)
            , mStep(step_min)
            , mStepMin(step_min)
            , mStepMax(step_max)
            , mStepX(step_x)
        {
        }

        inline void SetValue(const T&& value)
        {
            mValue = value;
        }
        inline void SetLabel(unsigned int ref)
        {
            mLabelRef = ref;
            mLabel = nullptr;
        }
        inline void SetLabel(const char* label)
        {
            mLabelRef = 0;
            mLabel = label;
        }
        inline void SetBound(const T minimum, const T maximum)
        {
            mValueMin = minimum;
            mValueMax = maximum;
            if (mValue < mValueMin)
                mValue = mValueMin;
            else if (mValue > mValueMax)
                mValue = mValueMax;
        }
        inline void SetStep(const T&& step_min, const T&& step_max, const T&& step_x = 10)
        {
            mStep = step_min;
            mStepMin = step_min;
            mStepMax = step_max;
            mStepX = step_x;
        }
        inline void SetCurrentStep(const T&& step)
        {
            mStep = step;
            if (mStep % mStepX)
                mStep -= (mStep % mStepX);
        }

        inline T GetValue()
        {
            return mValue;
        }
        inline T& operator*()
        {
            return mValue;
        }

        inline void RoundDown(T x)
        {
            if (mValue % x)
                mValue -= mValue % x;
        }

        bool operator()(const char* format = nullptr)
        {
            bool isFocused;
            char* label = mLabel ? mLabel : XSTR(mLabelRef);

            auto hasChanged = ImGui::SliderScalar(label, type, &mValue, &mValueMin, &mValueMax, format);
            isFocused = ImGui::IsItemFocused();

            if (isFocused) {
                if (InGameInputGet(VK_LSHIFT)) {
                    if (mStep == mStepMax)
                        mStep = mStepMin;
                    else {
                        mStep *= mStepX;
                        if (mStep > mStepMax)
                            mStep = mStepMax;
                    }
                }

                if (InGameInputGet(VK_LEFT)) {
                    T oldValue = mValue;
                    mValue -= mStep;
                    if (mValue < mValueMin)
                        mValue = mValueMin;
                    hasChanged |= (mValue != oldValue);
                } else if (InGameInputGet(VK_RIGHT)) {
                    T oldValue = mValue;
                    mValue += mStep;
                    if (mValue > mValueMax)
                        mValue = mValueMax;
                    hasChanged |= (mValue != oldValue);
                }
            }

            return hasChanged;
        }
    };

    template <typename T, ImGuiDataType type>
    class GuiDrag {
    private:
        unsigned int mLabelRef = 0;
        char* mLabel = nullptr;
        T mValue = 0;
        T mValueMin = 0;
        T mValueMax = 0;
        T mStep = 1;
        T mStepMin = 1;
        T mStepMax = 1;
        T mStepX = 1;

    public:
        GuiDrag(const char* label, const T&& minimum, const T&& maximum,
            T step_min = 1, T step_max = 1, T step_x = 10)
            : mLabel(const_cast<char*>(label))
            , mValueMin(minimum)
            , mValueMax(maximum)
            , mStep(step_min)
            , mStepMin(step_min)
            , mStepMax(step_max)
            , mStepX(step_x)
        {
        }

        GuiDrag(unsigned int label_ref, const T&& minimum, const T&& maximum,
            T step_min = 1, T step_max = 1, T step_x = 10)
            : mLabelRef(label_ref)
            , mValueMin(minimum)
            , mValueMax(maximum)
            , mStep(step_min)
            , mStepMin(step_min)
            , mStepMax(step_max)
            , mStepX(step_x)
        {
        }

        inline void SetValue(const T&& value)
        {
            mValue = value;
        }
        inline void SetLabel(unsigned int ref)
        {
            mLabelRef = ref;
            mLabel = nullptr;
        }
        inline void SetLabel(const char* label)
        {
            mLabelRef = 0;
            mLabel = label;
        }
        inline void SetBound(const T&& minimum, const T&& maximum)
        {
            mValueMin = minimum;
            mValueMax = maximum;
            if (mValue < mValueMin)
                mValue = mValueMin;
            else if (mValue > mValueMax)
                mValue = mValueMax;
        }
        inline void SetStep(const T&& step_min, const T&& step_max, const T&& step_x = 10)
        {
            mStep = step_min;
            mStepMin = step_min;
            mStepMax = step_max;
            mStepX = step_x;
        }

        inline T GetValue()
        {
            return mValue;
        }
        inline T& operator*()
        {
            return mValue;
        }

        inline void RoundDown(T x)
        {
            if (mValue % x)
                mValue -= mValue % x;
        }

        void operator()(const char* format = nullptr)
        {
            bool isFocused;
            char* label = mLabel ? mLabel : XSTR(mLabelRef);

            ImGui::DragScalar(label, type, &mValue, (float)(mStep * 2), &mValueMin, &mValueMax, format);
            isFocused = ImGui::IsItemFocused();

            if (mValue > mValueMax)
                mValue = mValueMax;
            if (mValue < mValueMin)
                mValue = mValueMin;

            if (isFocused) {
                if (InGameInputGet(VK_LSHIFT)) {
                    mStep *= mStepX;
                    if (mStep > mStepMax)
                        mStep = mStepMin;
                }

                if (InGameInputGet(VK_LEFT)) {
                    mValue -= mStep;
                    if (mValue < mValueMin)
                        mValue = mValueMin;
                } else if (InGameInputGet(VK_RIGHT)) {
                    mValue += mStep;
                    if (mValue > mValueMax)
                        mValue = mValueMax;
                }
            }
        }
    };

    class GuiCombo {
    public:
        GuiCombo(const char* label, th_glossary_t* selector = nullptr)
            : mLabel(const_cast<char*>(label))
            , mSelector(selector)
        {
        }
        GuiCombo(unsigned int label_ref, th_glossary_t* selector = nullptr)
            : mLabelRef(label_ref)
            , mSelector(selector)
        {
        }

        inline void SetLabel(unsigned int ref)
        {
            mLabelRef = ref;
            mLabel = nullptr;
        }
        inline void SetLabel(const char* label)
        {
            mLabelRef = 0;
            mLabel = const_cast<char*>(label);
        }

        inline int& operator*()
        {
            return mCurrent;
        }
        bool operator()();
        template <typename T>
        bool operator()(th_glossary_t new_label_ref, T* selector, char** items = XITEMS)
        {
            if (selector == nullptr || items == nullptr)
                return false;

            CheckComboItemNew(selector, items, 1);

            char* label;
            if (new_label_ref != A0000ERROR_C)
                label = XSTR(new_label_ref);
            else if (mLabelRef != A0000ERROR_C)
                label = XSTR(mLabelRef);
            else if (mLabel)
                label = mLabel;
            else
                label = "#ERROR";

            auto hasChanged = ImGui::ComboSections(label, &mCurrent, selector, items, "");

            if (ImGui::IsItemFocused()) {
                if (InGameInputGet(VK_LEFT)) {
                    hasChanged = true;
                    --mCurrent;
                    if (mCurrent < 0) {
                        for (mCurrent = 0; selector[mCurrent]; ++mCurrent)
                            ;
                        mCurrent--;
                    }
                    CheckComboItemNew(selector, items, -1);
                } else if (InGameInputGet(VK_RIGHT)) {
                    hasChanged = true;
                    ++mCurrent;
                    if (selector[mCurrent] == 0) {
                        mCurrent = 0;
                    }
                    CheckComboItemNew(selector, items, 1);
                }
            }

            return hasChanged;
        }

    private:
        unsigned int mLabelRef = 0;
        char* mLabel = nullptr;

        th_glossary_t* mSelector;

        int mCurrent = 0;

        template <typename T>
        inline void CheckComboItemNew(T* selector, char** items, int nav)
        {
            const int origin = mCurrent;
            while (true) {
                if (*items[selector[mCurrent]])
                    return;
                mCurrent += nav;
                if (mCurrent < 0) {
                    for (;selector[++mCurrent];);
                    mCurrent--;
                } else if (selector[mCurrent] == 0) {
                    mCurrent = 0;
                }
                if (mCurrent == origin)
                    return;
            }
        }
    };

    class GuiButton {
    private:
        ImVec2 mSize;
        unsigned int mLabelRef = 0;
        char* mLabel = nullptr;

    public:
        GuiButton(unsigned int label_ref, float width, float height)
            : mLabelRef(label_ref)
            , mSize { width, height }
        {
        }

        GuiButton(const char* label, float width, float height)
            : mLabel(const_cast<char*>(label))
            , mSize { width, height }
        {
        }

        inline void SetSize(float width, float height)
        {
            mSize.x = width;
            mSize.y = height;
        }
        inline void SetLabel(unsigned int ref)
        {
            mLabelRef = ref;
            mLabel = nullptr;
        }
        inline void SetLabel(const char* label)
        {
            mLabelRef = 0;
            mLabel = const_cast<char*>(label);
        }

        bool operator()();
    };

    class GuiCheckBox {
    private:
        unsigned int mLabelRef = 0;
        char* mLabel = nullptr;
        bool mToggle = false;

    public:
        GuiCheckBox(unsigned int label_ref)
            : mLabelRef(label_ref)
        {
        }

        GuiCheckBox(const char* label)
            : mLabel(const_cast<char*>(label))
        {
        }

        inline void SetLabel(unsigned int ref)
        {
            mLabelRef = ref;
            mLabel = nullptr;
        }
        inline void SetLabel(const char* label)
        {
            mLabelRef = 0;
            mLabel = const_cast<char*>(label);
        }

        inline bool& operator*()
        {
            return mToggle;
        }

        bool operator()();
    };

    class GuiHotKey {
    private:
        unsigned int mTextRef = 0;
        char* mText = nullptr;
        char* mKeyText = nullptr;
        int mKey;
        bool mStatus = false;
        std::vector<HookCtx*> mHooks;
        float mXOffset1 = 0.0f;
        float mXOffset2 = 0.0f;

    protected:
        virtual void OnWidgetUpdate(bool status, bool has_changed);
    public:
        GuiHotKey(unsigned int text_ref, char* key_text, int vkey, std::initializer_list<HookCtx*> hooks = {})
            : mTextRef(text_ref)
            , mKeyText(key_text)
            , mKey(vkey)
        {
            for (auto& hook : hooks) {
                hook->Setup();
                mHooks.push_back(hook);
            }
        }

        GuiHotKey(const char* text, char* key_text, int vkey, std::initializer_list<HookCtx*> hooks = {})
            : mText(const_cast<char*>(text))
            , mKeyText(key_text)
            , mKey(vkey)
        {
            for (auto& hook : hooks) {
                hook->Setup();
                mHooks.push_back(hook);
            }
        }

        GuiHotKey(unsigned int text_ref, char* key_text, int vkey,
            float x_offset_1, float x_offset_2, bool use_rel_offset,
            std::initializer_list<HookCtx*> hooks = {})
            : mTextRef(text_ref)
            , mKeyText(key_text)
            , mKey(vkey)
        {
            if (use_rel_offset)
                SetTextOffsetRel(x_offset_1, x_offset_2);
            else
                SetTextOffset(x_offset_1, x_offset_2);

            for (auto& hook : hooks) {
                hook->Setup();
                mHooks.push_back(hook);
            }
        }

        inline void SetText(unsigned int ref)
        {
            mTextRef = ref;
            mText = nullptr;
        }
        inline void SetText(const char* label)
        {
            mTextRef = 0;
            mText = const_cast<char*>(label);
        }
        inline void SetKey(const char* key_text, int vkey)
        {
            mKeyText = const_cast<char*>(key_text);
            mKey = vkey;
        }
        inline void SetTextOffset(float x_offset_1, float x_offset_2)
        {
            mXOffset1 = x_offset_1;
            mXOffset2 = x_offset_2;
        }
        inline void SetTextOffsetRel(float x_offset_prop_1, float x_offset_prop_2)
        {
            auto disp_x = ImGui::GetIO().DisplaySize.x;
            mXOffset1 = x_offset_prop_1 * disp_x;
            mXOffset2 = x_offset_prop_2 * disp_x;
        }
        inline void Toggle(bool status)
        {
            mStatus = status;
            if (status) {
                for (auto& hook : mHooks) {
                    hook->Enable();
                }
            } else {
                for (auto& hook : mHooks) {
                    hook->Disable();
                }
            }
        }
        inline bool& operator*()
        {
            return mStatus;
        }
        bool operator()(bool use_widget = true);
    };

    class GuiTimer {
    public:
        GuiTimer()
        {
            QueryPerformanceFrequency(&mFreq);
            mElapsed.QuadPart = 0;
        }

        void Start();
        void End();
        long long GetElapsedUs();

    private:
        LARGE_INTEGER mStart = {};
        LARGE_INTEGER mElapsed = {};
        LARGE_INTEGER mFreq = {};
        bool mIsTiming = false;
    };

    class GuiNavFocus {
    public:
        GuiNavFocus(std::initializer_list<int> arr_id)
        {
            for (auto it = std::begin(arr_id); it != std::end(arr_id); ++it) {
                mNavId.push_back(*it);
            }
            mLocale = LocaleGet();
        }

        inline void ForceFocus(int item_id)
        {
            mForceFocusId = item_id;
        }
        static void GlobalDisable(bool disable)
        {
            mGlobalDisable = disable;
        }
        inline int& operator*()
        {
            return mFocusId;
        }

        void operator()();

    private:
        static bool mGlobalDisable;
        std::vector<int> mNavId;
        locale_t mLocale;
        int mFocusId = 0;
        int mForceFocusId = 0;
    };

    template <class... Args>
    inline bool CheckPopups(int id, Args... rest)
    {
        if (ImGui::IsPopupOpen(XSTR(id)))
            return true;
        return CheckPopups(rest...);
    }
    inline bool CheckPopups(int id)
    {
        if (ImGui::IsPopupOpen(XSTR(id)))
            return true;
        return false;
    }
}
}
