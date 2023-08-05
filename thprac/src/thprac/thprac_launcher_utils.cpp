#include "thprac_launcher_utils.h"

namespace THPrac {

void MovWndToTop(HWND m_hWnd) {
    HWND hCurWnd = ::GetForegroundWindow();
    DWORD dwMyID = ::GetCurrentThreadId();
    DWORD dwCurID = ::GetWindowThreadProcessId(hCurWnd, nullptr);
    ::AttachThreadInput(dwCurID, dwMyID, TRUE);
    ::SetWindowPos(m_hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
    ::SetWindowPos(m_hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOMOVE);
    ::SetForegroundWindow(m_hWnd);
    ::SetFocus(m_hWnd);
    ::SetActiveWindow(m_hWnd);
    ::AttachThreadInput(dwCurID, dwMyID, FALSE);
}

void GuiColumnText(const char* text) {
    auto columnX = ImGui::GetColumnWidth();
    auto itemX = ImGui::CalcTextSize(text).x + ImGui::GetStyle().ItemSpacing.x;
    ImGui::TextUnformatted(text);
    if (ImGui::IsItemHovered()) {
        if (itemX > columnX) {
            ImGui::BeginTooltip();
            ImGui::PushTextWrapPos(ImGui::GetIO().DisplaySize.x * 0.9f);
            ImGui::TextUnformatted(text);
            ImGui::PopTextWrapPos();
            ImGui::EndTooltip();
        }
    }
}

inline void GuiCenteredText(const char* text);
inline void GuiSetPosXText(const char* text, float offset);
inline void GuiSetPosYRel(float rel);

void GuiHelpMarker(const char* desc) {
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered()) {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

int GuiCornerButton(const char* text, const char* text2, const ImVec2& offset, bool useCurrentY) {
    int result = 0;
    auto& style = ImGui::GetStyle();
    auto currentCursorY = ImGui::GetCursorPosY();
    ImVec2 cursor = ImGui::GetWindowSize();
    auto textSize = ImGui::CalcTextSize(text);
    cursor.x = cursor.x - textSize.x - offset.x * ImGui::GetFontSize();
    cursor.y = cursor.y - textSize.y - offset.y * ImGui::GetFontSize();
    if (cursor.y < currentCursorY || useCurrentY) {
        cursor.y = currentCursorY;
    }
    if (text2) {
        cursor.x -= style.FramePadding.x * 2 + style.ItemSpacing.x;
        cursor.x -= ImGui::CalcTextSize(text2).x;
    }

    ImGui::SetCursorPos(cursor);
    if (ImGui::Button(text)) {
        result = 1;
    }
    if (text2) {
        ImGui::SameLine();
        if (ImGui::Button(text2)) {
            result = 2;
        }
    }

    return result;
}

inline bool GuiButtonRelCentered(const char* buttonText, float posYRel, const ImVec2& sizeRel);
inline bool GuiButtonTxtCentered(const char* buttonText, float posYRel);
inline bool GuiModal(const char* modalTitle, ImVec2 sizeRel);
inline bool GuiModalFullScreen(const char* modalTitle);
inline bool GuiButtonModal(const char* buttonText, const char* modalTitle);

bool GuiButtonYesNo(const char* buttonText1, const char* buttonText2, float buttonSize) {
    if (buttonSize == -1.0f) {
        buttonSize = ImGui::GetItemRectSize().x / 2.05f;
    } else {
        buttonSize = buttonSize * ImGui::GetFontSize();
    }

    if (ImGui::Button(buttonText1, ImVec2(buttonSize, 0))) {
        ImGui::CloseCurrentPopup();
        return true;
    }
    ImGui::SameLine();
    if (ImGui::Button(buttonText2, ImVec2(buttonSize, 0))) {
        ImGui::CloseCurrentPopup();
    }
    return false;
}

bool GuiButtonAndModalYesNo(
    const char* buttonText,
    const char* modalTitle,
    const char* modalText,
    float buttonSize,
    const char* buttonText1,
    const char* buttonText2
) {
    bool result = false;

    GuiButtonModal(buttonText, modalTitle);
    if (GuiModal(modalTitle)) {
        ImGui::TextUnformatted(modalText);
        if (GuiButtonYesNo(buttonText1, buttonText2, buttonSize)) {
            result = true;
        }
        ImGui::EndPopup();
    }

    return result;
}

}
