#include "thprac_gui_components.h"
#include <Shlwapi.h>

namespace THPrac
{
	namespace Gui
	{
		bool GuiNavFocus::mGlobalDisable = false;
        void GameGuiWnd::Update()
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

                io.NavInputs[ImGuiNavInput_DpadUp] = InGameInputGet(VK_UP);
                io.NavInputs[ImGuiNavInput_DpadDown] = InGameInputGet(VK_DOWN);
                io.NavInputs[ImGuiNavInput_DpadLeft] = InGameInputGet(VK_LEFT);
                io.NavInputs[ImGuiNavInput_DpadRight] = InGameInputGet(VK_RIGHT);

                if (mStatus) {
                    for (auto& style : mStyle) {
                        ImGui::PushStyleVarAlt(style.style, style.value);
                        styleCount++;
                    }
                    if (mSizeFlag) {
                        ImGui::SetNextWindowSize(mSize, ImGuiCond_Always); // ImGuiCond_FirstUseEver
                        mSizeFlag = false;
                    }
                    if (mPosFlag) {

                        ImGui::SetNextWindowPos(mPos, ImGuiCond_Always);
                        mPosFlag = false;
                    }
                    if (mFocus)
                        ImGui::SetNextWindowFocus();

                    ImGui::SetNextWindowBgAlpha(mAlpha);
                    ImGui::Begin(mTitle.c_str(), nullptr, mWndFlag);
                    ImGui::PushItemWidth(mItemWidth);

                    if (mStatus == 2) {
                        OnContentUpdate();
                    }

                    ImGui::PopItemWidth();
                    ImGui::End();
                    for (size_t i = 0; i < styleCount; i++)
                        ImGui::PopStyleVar();
                }

                OnPostUpdate();
            }
        void GameGuiWnd::Open()
        {
            if (mStatus != 2)
                mStatus = 1;
        }
        void GameGuiWnd::Close()
        {
            if (mStatus)
                mStatus = 3;
        }

        bool GameGuiWnd::IsOpen()
        {
            return mStatus == 2;
        }
        bool GameGuiWnd::IsClosed()
        {
            return mStatus == 0;
        }

        void GameGuiWnd::SetViewport(void* ptr)
        {
            mViewport = (Viewport*)ptr;
        }
        void GameGuiWnd::SetSize(float width, float height)
        {
            mSizeFlag = true;
            mSize.x = width;
            mSize.y = height;
        }
        void GameGuiWnd::SetPos(float x, float y)
        {
            mPosFlag = true;
            mPos.x = x;
            mPos.y = y;
        }
        void GameGuiWnd::SetItemWidth(float item_width)
        {
            mItemWidth = item_width;
        }
        void GameGuiWnd::SetSizeRel(float width_prop, float height_prop)
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
        void GameGuiWnd::SetPosRel(float x_prop, float y_prop)
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
        void GameGuiWnd::SetItemWidthRel(float item_width_prop)
        {
            ImGuiIO& io = ImGui::GetIO();
            if (mViewport) {
                mItemWidth = item_width_prop * (float)mViewport->Width;
            } else {
                mItemWidth = item_width_prop * io.DisplaySize.x;
            }
        }
        void GameGuiWnd::SetItemSpacing(float x, float y)
        {
            auto itemSpacing = GetStyleIt(ImGuiStyleVar_ItemSpacing);
            itemSpacing->value = ImVec2(x, y);
        }
        void GameGuiWnd::SetItemSpacingRel(float x_prop, float y_prop)
        {
            ImGuiIO& io = ImGui::GetIO();
            auto itemSpacing = GetStyleIt(ImGuiStyleVar_ItemSpacing);
            if (mViewport) {
                itemSpacing->value = ImVec2(x_prop * (float)mViewport->Width, y_prop * (float)mViewport->Height);
            } else {
                itemSpacing->value = ImVec2(x_prop * io.DisplaySize.x, y_prop * io.DisplaySize.y);
            }
        }
        void GameGuiWnd::SetAutoSpacing(bool toggle)
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
        void GameGuiWnd::AutoSize(ImVec2 size,
            const char* content_1, const char* content_2, const char* label_1, const char* label_2,
            float widiget_counts, ImVec2 size_max, ImVec2 size_min)
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
                float font_y = 0.0f;
                if (label_valid)
                    font_y = label_size.y;
                else
                    font_y = ImGui::GetFont()->FontSize;

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
        void GameGuiWnd::AutoSize(float x, float y, const char* content, const char* label, float widigit_counts, float max_y)
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
                float font_y = 0.0f;
                if (content || label)
                    font_y = ImGui::CalcTextSize(content ? content : label).y;
                else
                    font_y = ImGui::GetFont()->FontSize;

                mSize.y = (font_y + style.FramePadding.y * 2.0f + style.ItemSpacing.y) * widigit_counts;
                if (mSize.y > max_y)
                    mSize.y = max_y;
            }

            if (label)
                mItemWidth = ImGui::CalcTextSize(label).x * -1.0f;

            mSizeFlag = true;
        }
        void GameGuiWnd::AutoPos(float x, float y)
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
        void GameGuiWnd::AutoItemWidth(const char* label)
        {
            mItemWidth = ImGui::CalcTextSize(label).x * -1.0f;
        }

        void GameGuiWnd::SetStyle(ImGuiStyleVar style, float param1, float param2)
        {
            auto s = GetStyleIt(style);
            s->value = ImVec2(param1, param2);
        }
        void GameGuiWnd::SetWndFlag(ImGuiWindowFlags flags)
        {
            mWndFlag = flags;
        }
        void GameGuiWnd::SetFade(float transparency, float step)
        {
            mAlphaMax = transparency;
            mAlphaStep = step;
        }
        void GameGuiWnd::SetTitle(const char* title)
        {
            mTitle = title;
        }
        void GameGuiWnd::SetWndFoucs(bool toggle)
        {
            mFocus = toggle;
        }

        void GameGuiWnd::WndDebugOutput()
        {
            SetWndFlag(ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
            auto pos = ImGui::GetWindowPos();
            auto size = ImGui::GetWindowSize();
            auto& io = ImGui::GetIO();

            ImGui::Text("Size: %f (%f), %f (%f)", size.x, size.x / io.DisplaySize.x, size.y, size.y / io.DisplaySize.y);
            ImGui::Text("Pos X: %f (%f)", pos.x, pos.x / (io.DisplaySize.x - size.x));
            ImGui::Text("Pos Y: %f (%f)", pos.y, pos.y / (io.DisplaySize.y - size.y));
        }
        std::vector<GameGuiWnd::StyleValue>::iterator GameGuiWnd::GetStyleIt(ImGuiStyleVar style)
        {
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

        bool GuiCombo::operator()(int skip)
        {
            auto items = LocaleGetCurrentGlossary();
            CheckComboItemNew(mSelector, items, 1);

            const char* label;
            if (mLabelRef != A0000ERROR_C)
                label = LocaleGetStr(mLabelRef);
            else if (mLabel)
                label = mLabel;
            else
                label = "#ERROR";

            const char* _skip = skip == -1 ? "" : LocaleGetStr(mSelector[skip]);
            auto hasChanged = ImGui::ComboSections(label, &mCurrent, mSelector, items, _skip);

            if (ImGui::IsItemFocused()) {
                if (InGameInputGet(VK_LEFT)) {
                    hasChanged = true;
                    --mCurrent;
                    if (skip != -1 && mCurrent == skip)
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
                    if (skip != -1 && mCurrent == skip)
                        ++mCurrent;
                    if (mSelector[mCurrent] == 0) {
                        mCurrent = 0;
                    }
                    CheckComboItemNew(mSelector, items, 1);
                }
            }

            return hasChanged;
        }

        bool GuiButton::operator()()
        {
            const char* label = mLabel ? mLabel : LocaleGetStr(mLabelRef);
            bool res = ImGui::Button(label, mSize);
            if (ImGui::IsItemFocused() && InGameInputGetConfirm())
                res = true;
            return res;
        }

        bool GuiCheckBox::operator()()
        {
            const char* label = mLabel ? mLabel : LocaleGetStr(mLabelRef);
            bool pressed = ImGui::Checkbox(label, &mToggle);
            if (ImGui::IsItemFocused() && (InGameInputGet(VK_LEFT) || InGameInputGet(VK_RIGHT))) {
                mToggle = !mToggle;
                pressed = true;
            }
            return pressed;
        }



        void GuiHotKey::OnWidgetUpdate(bool status, bool has_changed)
        {
            const char* text = mText ? mText : LocaleGetStr(mTextRef);

            if (mStatus)
                ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "[%s: %s]", mKeyText, text);
            else
                ImGui::Text("%s: %s", mKeyText, text);
            return;
        }
        bool GuiHotKey::operator()(bool use_widget)
        {
            bool flag = Gui::ImplWin32CheckHotkey(mKey);

            if (flag) {
                mStatus = !mStatus;
                if (mStatus) {
                    for (auto& hook : mHooks) {
                        hook->Enable();
                    }
                } else {
                    for (auto& hook : mHooks) {
                        hook->Disable();
                    }
                }
            }

            if (use_widget)
                OnWidgetUpdate(mStatus, flag);

            return flag;
        }

        void GuiTimer::Start()
        {
            if (!mIsTiming) {
                QueryPerformanceCounter(&mStart);
                mIsTiming = true;
            }
        }
        void GuiTimer::End()
        {
            if (mIsTiming) {
                LARGE_INTEGER end;
                QueryPerformanceCounter(&end);
                mElapsed.QuadPart = end.QuadPart - mStart.QuadPart;
                mIsTiming = false;
            }
        }
        long long GuiTimer::GetElapsedUs()
        {
            return (mElapsed.QuadPart * 1000000 / mFreq.QuadPart);
        }

        void GuiNavFocus::operator()()
        {
            if (mGlobalDisable)
                return;

            bool has_active_popup = false;
            int last_activated_id = 0;

            for (auto id : mNavId) {
                // TODO: Does `id` really need to be an int?
                auto idAsGlossaryRef = static_cast<th_glossary_t>(id);
                if (ImGui::IsPopupOpen(LocaleGetStr(idAsGlossaryRef)))
                    has_active_popup = true;

                if (ImGui::IsItemActiveAlt(LocaleGetStr(idAsGlossaryRef)))
                    last_activated_id = id;
                else if (ImGui::IsItemFocusedAlt(LocaleGetStr(idAsGlossaryRef)) && !mForceFocusId)
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

            // TODO: Do `mForceFocusId`, `mFocusId`, and `mNavId` need to be ints?
            if (mForceFocusId)
                ImGui::SetItemFocusAlt(LocaleGetStr(static_cast<th_glossary_t>(mForceFocusId)), true);
            else if (mFocusId)
                ImGui::SetItemFocusAlt(LocaleGetStr(static_cast<th_glossary_t>(mFocusId)), locale_changed);
            else
                ImGui::SetItemFocusAlt(LocaleGetStr(static_cast<th_glossary_t>(mNavId[0])), locale_changed);

            if (mForceFocusId) {
                mFocusId = mForceFocusId;
                mForceFocusId = 0;
            }
        }

        void MultiComboSelect(std::vector<size_t>& out, const std::vector<const char*> choices, const char* format, size_t level)
        {
            if (out.size() <= level)
                out.resize(level + 1);

            size_t bufSize = snprintf(nullptr, 0, format, level);
            auto labelStr = new char[bufSize + 2];
            labelStr[bufSize + 1] = 0;
            snprintf(labelStr, bufSize + 1, format, level);

            if (ImGui::BeginCombo(labelStr, choices[out[level]])) {
                for (size_t i = 0; i < choices.size(); i++) {
                    ImGui::PushID(i);

                    bool item_selected = (i == out[level]);

                    if (ImGui::Selectable(choices[i], &item_selected))
                        out[level] = i;

                    if (item_selected)
                        ImGui::SetItemDefaultFocus();

                    ImGui::PopID();
                }
                ImGui::EndCombo();
            }

            if (ImGui::IsItemFocused()) {
                if (Gui::InGameInputGet(VK_LEFT) && out[level] > 0) {
                    out[level]--;
                }
                if (Gui::InGameInputGet(VK_RIGHT) && out[level] + 1 < choices.size()) {
                    out[level]++;
                }
            }

            if (out[level]) {
                ImGui::PushID(++level);
                MultiComboSelect(out, choices, format, level);
                ImGui::PopID();
            }

            delete[] labelStr;
        }
    }
}
