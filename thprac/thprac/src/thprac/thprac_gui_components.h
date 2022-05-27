#pragma once
#include <imgui.h>
#include <vector>
#include <functional>
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
            : mStatus(0)
            , mAlpha(0.0f)
            , mWndFlag(STYLE_DEFAULT)
            , mItemWidth(-100)
            , mSizeFlag(true)
            , mPosFlag(true)
            , mFocus(false)
            , mViewport(nullptr)
        {
            ImGuiIO& io = ImGui::GetIO();
            io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

            mLocale = (locale_t)-1;
        }

        static constexpr ImGuiWindowFlags STYLE_DEFAULT = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | 0;

        void Update()
        {
            OnPreUpdate();

            if (mLocale != LocaleGet()) {
                mLocale = LocaleGet();
                OnLocaleChange();
            }

            ImGuiIO& io = ImGui::GetIO();
            size_t styleCount = 0;

            if (mStatus == 1) {
                mAlpha += mAlphaStep;
                if (mAlpha >= mAlphaMax) {
                    mAlpha = mAlphaMax;
                    mStatus = 2;
                }
            } else if (mStatus == 3) {
                io.MouseDrawCursor = false;
                mAlpha -= mAlphaStep;
                if (mAlpha <= 0.0f) {
                    mAlpha = 0.0f;
                    mStatus = 0;
                }
            }

            //io.NavInputs[ImGuiNavInput_Activate] = GetGameKey(0x5A); // Z
            //io.NavInputs[ImGuiNavInput_Cancel] = GetGameKey(0x58); // X
            io.NavInputs[ImGuiNavInput_DpadUp] = InGameInputGet(VK_UP);
            io.NavInputs[ImGuiNavInput_DpadDown] = InGameInputGet(VK_DOWN);
            io.NavInputs[ImGuiNavInput_DpadLeft] = InGameInputGet(VK_LEFT);
            io.NavInputs[ImGuiNavInput_DpadRight] = InGameInputGet(VK_RIGHT);

            if (mStatus) {
                LocalePushFont();
                for (auto& style : mStyle) {
                    ImGui::PushStyleVarAlt(style.style, style.value);
                    styleCount++;
                }
                if (mSizeFlag) {
                    ImGui::SetNextWindowSize(mSize, ImGuiCond_Always); //ImGuiCond_FirstUseEver
                    mSizeFlag = false;
                }
                if (mPosFlag) {

                    ImGui::SetNextWindowPos(mPos, ImGuiCond_Always);
                    mPosFlag = false;
                }
                if (mFocus)
                    ImGui::SetNextWindowFocus();

                ImGui::SetNextWindowBgAlpha(mAlpha);
                ImGui::Begin(mTitle.c_str(), NULL, mWndFlag);
                ImGui::PushItemWidth(mItemWidth);

                if (mStatus == 2) {
                    OnContentUpdate();
                }

                ImGui::PopItemWidth();
                ImGui::End();
                for (size_t i = 0; i < styleCount; i++)
                    ImGui::PopStyleVar();
                LocalePopFont();
            }

            OnPostUpdate();
        }
        void Open()
        {
            if (mStatus != 2)
                mStatus = 1;
        }
        void Close()
        {
            if (mStatus)
                mStatus = 3;
        }

        bool IsOpen()
        {
            return mStatus == 2;
        }
        bool IsClosed()
        {
            return mStatus == 0;
        }

        void SetViewport(void* ptr) 
        {
            mViewport = (Viewport*)ptr;
        }
        void SetSize(float width, float height)
        {
            mSizeFlag = true;
            mSize.x = width;
            mSize.y = height;
        }
        void SetPos(float x, float y)
        {
            mPosFlag = true;
            mPos.x = x;
            mPos.y = y;
        }
        void SetItemWidth(float item_width)
        {
            mItemWidth = item_width;
        }
        void SetSizeRel(float width_prop, float height_prop)
        {
            ImGuiIO& io = ImGui::GetIO();
            mSizeFlag = true;
            if (mViewport) {
                mSize.x = width_prop * (float)mViewport->Width;
                mSize.y = height_prop * (float)mViewport->Height;
            } else {
                mSize.x = width_prop * io.DisplaySize.x;
                mSize.y = height_prop * io.DisplaySize.y;
            }
        }
        void SetPosRel(float x_prop, float y_prop)
        {
            ImGuiIO& io = ImGui::GetIO();
            mPosFlag = true;
            if (mViewport) {
                mPos.x = x_prop * (float)mViewport->Width + (float)mViewport->X;
                mPos.y = y_prop * (float)mViewport->Height + (float)mViewport->Y;
            } else {
                mPos.x = x_prop * io.DisplaySize.x;
                mPos.y = y_prop * io.DisplaySize.y;
            }
        }
        void SetItemWidthRel(float item_width_prop)
        {
            ImGuiIO& io = ImGui::GetIO();
            if (mViewport) {
                mItemWidth = item_width_prop * (float)mViewport->Width;
            } else {
                mItemWidth = item_width_prop * io.DisplaySize.x;
            }
        }
        void SetItemSpacing(float x, float y)
        {
            auto itemSpacing = GetStyleIt(ImGuiStyleVar_ItemSpacing);
            itemSpacing->value = ImVec2(x, y);
        }
        void SetItemSpacingRel(float x_prop, float y_prop)
        {
            ImGuiIO& io = ImGui::GetIO();
            auto itemSpacing = GetStyleIt(ImGuiStyleVar_ItemSpacing);
            if (mViewport) {
                itemSpacing->value = ImVec2(x_prop * (float)mViewport->Width, y_prop * (float)mViewport->Height);
            } else {
                itemSpacing->value = ImVec2(x_prop * io.DisplaySize.x, y_prop * io.DisplaySize.y);
            }
        }
        void SetAutoSpacing(bool toggle)
        {
            if (toggle) {
                ImGuiIO& io = ImGui::GetIO();
                if (mViewport) {
                    auto framePadding = GetStyleIt(ImGuiStyleVar_FramePadding);
                    framePadding->value = ImVec2(
                        0.00625f * (float)mViewport->Width, 0.00625f * (float)mViewport->Height);
                    auto itemSpacing = GetStyleIt(ImGuiStyleVar_ItemSpacing);
                    itemSpacing->value = ImVec2(
                        0.0125f * (float)mViewport->Width, 0.00833f * (float)mViewport->Height);
                } else {
                    auto framePadding = GetStyleIt(ImGuiStyleVar_FramePadding);
                    framePadding->value = ImVec2(
                        0.00625f * io.DisplaySize.x, 0.00625f * io.DisplaySize.y);
                    auto itemSpacing = GetStyleIt(ImGuiStyleVar_ItemSpacing);
                    itemSpacing->value = ImVec2(
                        0.0125f * io.DisplaySize.x, 0.00833f * io.DisplaySize.y);
                }
            } else {
                auto framePadding = GetStyleIt(ImGuiStyleVar_FramePadding);
                mStyle.erase(framePadding);
                auto itemSpacing = GetStyleIt(ImGuiStyleVar_ItemSpacing);
                mStyle.erase(itemSpacing);
            }
        }
        void AutoSize(ImVec2 size,
            const char* content_1 = nullptr, const char* content_2 = nullptr, const char* label_1 = nullptr, const char* label_2 = nullptr,
            float widiget_counts = 0.0f, ImVec2 size_max = { -1.0f, -1.0f }, ImVec2 size_min = { -1.0f, -1.0f })
        {
            auto& io = ImGui::GetIO();
            auto& style = ImGui::GetStyle();

            ImVec2 label_size = { -1.0f, -1.0f };
            if (label_1) {
                auto label_size_1 = ImGui::CalcTextSize(label_1);
                if (label_2) {
                    auto label_size_2 = ImGui::CalcTextSize(label_2);
                    label_size = label_size_1.x > label_size_2.x ? label_size_1 : label_size_2;
                } else
                    label_size = label_size_1;
            }
            bool label_valid = label_size.x >= 0.0f && label_size.y >= 0.0f;

            ImVec2 content_size = { -1.0f, -1.0f };
            if (content_1) {
                auto content_size_1 = ImGui::CalcTextSize(content_1);
                if (content_2) {
                    auto content_size_2 = ImGui::CalcTextSize(content_2);
                    content_size = content_size_1.x > content_size_2.x ? content_size_1 : content_size_2;
                } else
                    content_size = content_size_1;
            }
            bool content_valid = content_size.x >= 0.0f && content_size.y >= 0.0f;

            if (size.x >= 1.0f) {
                mSize.x = size.x;
            } else if (size.x > 0.0f) {
                mSize.x = io.DisplaySize.x * size.x;
            } else if (size.x == 0.0f && content_valid && label_valid) {
                mSize.x = content_size.x + label_size.x + ImGui::GetFrameHeight() * 3;
            }
            if (size_max.x >= 0.0f && mSize.x > size_max.x)
                mSize.x = size_max.x;
            else if (size_min.x >= 0.0f && mSize.x < size_min.x)
                mSize.x = size_min.x;

            if (size.y > 1.0f) {
                mSize.y = size.y;
            } else if (size.y > 0.0f) {
                mSize.y = io.DisplaySize.y * size.y;
            } else if (size.y == 0.0f) {
                float font_y;
                if (label_valid)
                    font_y = label_size.y;
                else
                    ImGui::GetFont()->FontSize;

                mSize.y = (font_y + style.FramePadding.y * 2.0f + style.ItemSpacing.y) * widiget_counts;
            }
            if (size_max.y >= 0.0f && mSize.y > size_max.y)
                mSize.y = size_max.y;
            else if (size_min.y >= 0.0f && mSize.y < size_min.y)
                mSize.y = size_min.y;

            if (label_valid)
                mItemWidth = label_size.x * -1.0f;

            mSizeFlag = true;
        }
        void AutoSize(float x, float y, const char* content = nullptr, const char* label = nullptr, float widigit_counts = 0.0f, float max_y = 0.0f)
        {
            auto& io = ImGui::GetIO();
            auto& style = ImGui::GetStyle();

            if (x >= 1.0001f) {
                mSize.x = x;
            } else if (x > 0.0f) {
                mSize.x = io.DisplaySize.x * x;
            } else if (x == 0.0f && content && label) {
                mSize.x = ImGui::CalcTextSize(content).x + ImGui::CalcTextSize(label).x + ImGui::GetFrameHeight() * 3;
            }

            if (y > 1.0001f) {
                mSize.y = y;
            } else if (y > 0.0f) {
                mSize.y = io.DisplaySize.y * y;
            } else if (y == 0.0f) {
                float font_y;
                if (content || label)
                    font_y = ImGui::CalcTextSize(content ? content : label).y;
                else
                    ImGui::GetFont()->FontSize;

                mSize.y = (font_y + style.FramePadding.y * 2.0f + style.ItemSpacing.y) * widigit_counts;
                if (mSize.y > max_y)
                    mSize.y = max_y;
            }

            if (label)
                mItemWidth = ImGui::CalcTextSize(label).x * -1.0f;

            mSizeFlag = true;
        }
        void AutoPos(float x, float y)
        {
            auto& io = ImGui::GetIO();

            if (x > 1.0001f) {
                mPos.x = x;
            } else if (x >= 0.0f) {
                mPos.x = (io.DisplaySize.x - mSize.x) * x;
            }

            if (y > 1.0001f) {
                mPos.y = y;
            } else if (y >= 0.0f) {
                mPos.y = (io.DisplaySize.y - mSize.y) * y;
            }

            mPosFlag = true;
        }
        void AutoItemWidth(const char* label)
        {
            mItemWidth = ImGui::CalcTextSize(label).x * -1.0f;
        }

        void SetStyle(ImGuiStyleVar style, float param1, float param2 = 0.0f)
        {
            auto s = GetStyleIt(style);
            s->value = ImVec2(param1, param2);
        }
        void SetWndFlag(ImGuiWindowFlags flags)
        {
            mWndFlag = flags;
        }
        void SetFade(float transparency, float step)
        {
            mAlphaMax = transparency;
            mAlphaStep = step;
        }
        void SetTitle(const char* title)
        {
            mTitle = title;
        }
        void SetWndFoucs(bool toggle)
        {
            mFocus = toggle;
        }

        void WndDebugOutput()
        {
            SetWndFlag(ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
            auto pos = ImGui::GetWindowPos();
            auto size = ImGui::GetWindowSize();
            auto& io = ImGui::GetIO();

            ImGui::Text("Size: %f (%f), %f (%f)", size.x, size.x / io.DisplaySize.x, size.y, size.y / io.DisplaySize.y);
            ImGui::Text("Pos X: %f (%f)", pos.x, pos.x / (io.DisplaySize.x - size.x));
            ImGui::Text("Pos Y: %f (%f)", pos.y, pos.y / (io.DisplaySize.y - size.y));
        }

    protected:
        std::vector<StyleValue>::iterator GetStyleIt(ImGuiStyleVar style) {
            for (auto it = mStyle.begin(); it != mStyle.end(); ++it) {
                if (it->style == style) {
                    return it;
                }
            }
            mStyle.emplace_back();
            auto it = std::prev(mStyle.end());
            it->style = style;
            return it;
        }

        virtual void OnLocaleChange() {};
        virtual void OnContentUpdate() {};
        virtual void OnPreUpdate() {};
        virtual void OnPostUpdate() {};

        int mStatus;
        std::vector<StyleValue> mStyle;
        std::string mTitle;
        ImVec2 mSize;
        ImVec2 mPos;
        ImGuiWindowFlags mWndFlag;
        float mItemWidth;
        float mAlpha;
        float mAlphaMax;
        float mAlphaStep;
        bool mSizeFlag;
        bool mPosFlag;
        bool mFocus;
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

        int& operator*()
        {
            return mCurrent;
        }
        bool operator()()
        {
            auto items = XITEMS;
            CheckComboItemNew(mSelector, items, 1);

            char* label;
            if (mLabelRef != A0000ERROR_C)
                label = XSTR(mLabelRef);
            else if (mLabel)
                label = mLabel;
            else
                label = "#ERROR";

            auto hasChanged = ImGui::ComboSections(label, &mCurrent, mSelector, items, "");

            if (ImGui::IsItemFocused()) {
                if (InGameInputGet(VK_LEFT)) {
                    hasChanged = true;
                    --mCurrent;
                    if (mCurrent < 0) {
                        for (mCurrent = 0; mSelector[mCurrent]; ++mCurrent)
                            ;
                        mCurrent--;
                    }
                    CheckComboItemNew(mSelector, items, -1);
                } else if (InGameInputGet(VK_RIGHT)) {
                    hasChanged = true;
                    ++mCurrent;
                    if (mSelector[mCurrent] == 0) {
                        mCurrent = 0;
                    }
                    CheckComboItemNew(mSelector, items, 1);
                }
            }

            return hasChanged;
        }
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

        bool operator()()
        {
            char* label = mLabel ? mLabel : XSTR(mLabelRef);
            bool res = ImGui::Button(label, mSize);
            if (ImGui::IsItemFocused() && InGameInputGet(0x5A))
                res = true;
            return res;
        }
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

        bool operator()()
        {
            char* label = mLabel ? mLabel : XSTR(mLabelRef);
            bool pressed = ImGui::Checkbox(label, &mToggle);
            if (ImGui::IsItemFocused() && (InGameInputGet(VK_LEFT) || InGameInputGet(VK_RIGHT))) {
                mToggle = !mToggle;
                pressed = true;
            }
            return pressed;
        }
    };

    class GuiHotKey {
    private:
        unsigned int mTextRef = 0;
        char* mText = nullptr;
        char* mKeyText = nullptr;
        int mKey;
        bool mStatus = false;
        std::vector<HookCtx*> mPatches;
        float mXOffset1 = 0.0f;
        float mXOffset2 = 0.0f;

    protected:
        virtual void OnWidgetUpdate(bool status, bool has_changed)
        {
            char* text = mText ? mText : XSTR(mTextRef);

            if (mStatus)
                ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "[%s: %s]", mKeyText, text);
            else
                ImGui::Text("%s: %s", mKeyText, text);
            return;

#if 0
				ImGui::Text("(%s): ", mKeyText); ImGui::SameLine();
				ImGui::Text("%s ", text); ImGui::SameLine(mXOffset1);
				if (mStatus)
					ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), XSTR(TH_ON));
				else
					ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), XSTR(TH_OFF));
				return;
#endif

#if 0
				ImGui::Text("%s ", text);
				ImGui::SameLine(mXOffset1);
				ImGui::Text("(%s): ", mKeyText);
				ImGui::SameLine(mXOffset2);
				if (mStatus)
					ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), XSTR(TH_ON));
				else
					ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), XSTR(TH_OFF));
#endif
        }

    public:
        template <class... Args>
        GuiHotKey(unsigned int text_ref, char* key_text, int vkey, Args... rest)
            : mTextRef(text_ref)
            , mKeyText(key_text)
            , mKey(vkey)
        {
            AddPatch(rest...);
        }
        template <class... Args>
        GuiHotKey(const char* text, char* key_text, int vkey, Args... rest)
            : mText(const_cast<char*>(text))
            , mKeyText(key_text)
            , mKey(vkey)
        {
            AddPatch(rest...);
        }
        template <class... Args>
        GuiHotKey(unsigned int text_ref, char* key_text, int vkey,
            float x_offset_1, float x_offset_2, bool use_rel_offset,
            Args... rest)
            : mTextRef(text_ref)
            , mKeyText(key_text)
            , mKey(vkey)
        {
            if (use_rel_offset)
                SetTextOffsetRel(x_offset_1, x_offset_2);
            else
                SetTextOffset(x_offset_1, x_offset_2);

            AddPatch(rest...);
        }

        template <class... Args>
        void AddPatch(void* target, const char* patch, size_t size, Args... rest)
        {
            HookCtx* hookPatch = new HookCtx(target, patch, size);
            hookPatch->Setup();
            mPatches.push_back(hookPatch);
            AddPatch(rest...);
        }
        void AddPatch()
        {
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
                for (auto& patch : mPatches) {
                    patch->Enable();
                }
            } else {
                for (auto& patch : mPatches) {
                    patch->Disable();
                }
            }
        }

        bool& operator*()
        {
            return mStatus;
        }
        bool operator()(bool use_widget = true)
        {
            bool flag = Gui::ImplWin32CheckHotkey(mKey);

            if (flag) {
                mStatus = !mStatus;
                if (mStatus) {
                    for (auto& patch : mPatches) {
                        patch->Enable();
                    }
                } else {
                    for (auto& patch : mPatches) {
                        patch->Disable();
                    }
                }
            }

            if (use_widget)
                OnWidgetUpdate(mStatus, flag);

            return flag;
        }
    };

    class GuiTimer {
    public:
        GuiTimer()
        {
            QueryPerformanceFrequency(&mFreq);
            mElapsed.QuadPart = 0;
        }

        void Start()
        {
            if (!mIsTiming) {
                QueryPerformanceCounter(&mStart);
                mIsTiming = true;
            }
        }
        void End()
        {
            if (mIsTiming) {
                LARGE_INTEGER end;
                QueryPerformanceCounter(&end);
                mElapsed.QuadPart = end.QuadPart - mStart.QuadPart;
                mIsTiming = false;
            }
        }

        long long GetElapsedUs()
        {
            return (mElapsed.QuadPart * 1000000 / mFreq.QuadPart);
        }

    private:
        LARGE_INTEGER mStart;
        LARGE_INTEGER mElapsed;
        LARGE_INTEGER mFreq;
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

        void ForceFocus(int item_id)
        {
            mForceFocusId = item_id;
        }
        static void GlobalDisable(bool disable)
        {
            mGlobalDisable = disable;
        }

        int& operator*()
        {
            return mFocusId;
        }
        void operator()()
        {
            if (mGlobalDisable)
                return;

            bool has_active_popup = false;
            int last_activated_id = 0;

            for (auto id : mNavId) {
                if (ImGui::IsPopupOpen(XSTR(id)))
                    has_active_popup = true;

                if (ImGui::IsItemActiveAlt(XSTR(id)))
                    last_activated_id = id;
                else if (ImGui::IsItemFocusedAlt(XSTR(id)) && !mForceFocusId)
                    mFocusId = id;
            }

            if (!has_active_popup)
                ImGui::SetWindowFocus();

            if (last_activated_id && !mForceFocusId)
                mFocusId = last_activated_id;

            bool locale_changed = false;
            if (mLocale != LocaleGet()) {
                mLocale = LocaleGet();
                locale_changed = true;
            }

            if (mForceFocusId)
                ImGui::SetItemFocusAlt(XSTR(mForceFocusId), true);
            else if (mFocusId)
                ImGui::SetItemFocusAlt(XSTR(mFocusId), locale_changed);
            else
                ImGui::SetItemFocusAlt(XSTR(mNavId[0]), locale_changed);

            if (mForceFocusId) {
                mFocusId = mForceFocusId;
                mForceFocusId = 0;
            }
        }

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