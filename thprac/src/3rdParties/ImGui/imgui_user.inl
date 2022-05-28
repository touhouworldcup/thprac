// Changes required to implement the minimize button:
// 1. imgui.h: Change Begin() declaration: Add p_min.
// 2. imgui.cpp: Change RenderWindowTitleBarContents() declaration: Add p_min.
// 3. imgui.cpp: Change RenderWindowTitleBarContents() definition: Add MinimizeButton().
// 4. imgui.cpp: Change Begin() definition: Change RenderWindowTitleBarContents()'s parameters.
// 5. imgui.h: Change Columns() declaration: Add resize, default_offset.
// 6. imgui_internal.h: Change BeginColumns() declaration: Add default_offset.
// 7. imgui_widgets.cpp: Change Columns() and BeginColumns() definition.
// 8. imgui.h: Change Selectable() declaration: Add text_wrap.
// 9. imgui_widgets.cpp: Change Selectable() definition: Add text_wrap.
void ImGui::MinimizeButton(bool* p_min, float max_x, float min_y)
{
	if (p_min == nullptr) return;

	ImGuiContext& g = *GImGui;
	ImGuiWindow* window = g.CurrentWindow;

	ImGuiStyle& style = g.Style;
	ImGuiID id = window->GetID("#MINIMIZE");
	ImVec2 pos(max_x - style.FramePadding.x * 3 - g.FontSize * 2, min_y);

	// We intentionally allow interaction when clipped so that a mechanical Alt,Right,Validate sequence close a window.
	// (this isn't the regular behavior of buttons, but it doesn't affect the user much because navigation tends to keep items visible).
	const ImRect bb(pos, pos + ImVec2(g.FontSize, g.FontSize) + g.Style.FramePadding * 2.0f);
	bool is_clipped = !ItemAdd(bb, id);

	bool hovered, held;
	bool pressed = ButtonBehavior(bb, id, &hovered, &held);
	if (!is_clipped)
	{
		// Render
		ImU32 col = GetColorU32(held ? ImGuiCol_ButtonActive : ImGuiCol_ButtonHovered);
		ImVec2 center = bb.GetCenter();
		if (hovered)
			window->DrawList->AddCircleFilled(center, ImMax(2.0f, g.FontSize * 0.5f + 1.0f), col, 12);

		float cross_extent = g.FontSize * 0.5f * 0.7071f - 1.0f;
		ImU32 cross_col = GetColorU32(ImGuiCol_Text);
		center -= ImVec2(0.5f, 0.5f);
		window->DrawList->AddLine(center + ImVec2(+cross_extent, 0), center + ImVec2(-cross_extent, 0), cross_col, 1.5f);
		//window->DrawList->AddLine(center + ImVec2(+cross_extent, -cross_extent), center + ImVec2(-cross_extent, +cross_extent), cross_col, 1.0f);
	}

	if (pressed)
		*p_min = true;
}

bool ImGui::IsWindowResizing()
{
	ImGuiContext& g = *GImGui;
	ImGuiWindow* window = g.CurrentWindow;

	bool isResizing = false;
	const int resize_grip_count = g.IO.ConfigWindowsResizeFromEdges ? 2 : 1;
	const int resize_border_count = g.IO.ConfigWindowsResizeFromEdges ? 4 : 0;

	PushID("#RESIZE");
	for (int resize_grip_n = 0; resize_grip_n < resize_grip_count; resize_grip_n++)
		isResizing |= (g.ActiveId == window->GetID(resize_grip_n));
	for (int border_n = 0; border_n < resize_border_count; border_n++)
		isResizing |= (g.ActiveId == window->GetID(border_n + 4));
	PopID();

	return isResizing;
}

void ImGui::BeginDisabled(bool disabled)
{
    if (disabled) {
        ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
    }
}

void ImGui::EndDisabled(bool disabled)
{
    if (disabled) {
        ImGui::PopItemFlag();
        ImGui::PopStyleVar();
    }
}

bool ImGui::ComboAlt(const char* data_ex, const char* label, int* current_item, bool(*items_getter)(void*, int, const char**), void* data, int items_count, int popup_max_height_in_items)
{
	ImGuiContext& g = *GImGui;

	// Call the getter to obtain the preview string which is a parameter to BeginCombo()
	const char* preview_value = NULL;
	if (*current_item >= 0 && *current_item < items_count)
		items_getter(data, *current_item, &preview_value);

	// The old Combo() API exposed "popup_max_height_in_items". The new more general BeginCombo() API doesn't have/need it, but we emulate it here.
	//if (popup_max_height_in_items != -1 && !g.NextWindowData.SizeConstraintCond)
	//	SetNextWindowSizeConstraints(ImVec2(0, 0), ImVec2(FLT_MAX, CalcMaxPopupHeightFromItemCount(popup_max_height_in_items)));

	if (!BeginCombo(label, preview_value, ImGuiComboFlags_None))
		return false;

	// Display items
	// FIXME-OPT: Use clipper (but we need to disable it on the appearing frame to make sure our call to SetItemDefaultFocus() is processed)
	bool value_changed = false;
	for (int i = 0; i < items_count; i++)
	{
		const bool item_selected = (i == *current_item);
		const char* item_text;
		if (!items_getter(data, i, &item_text))
			item_text = "*Unknown item*";
		
		if (item_text != data_ex)
		{
			PushID((void*)(intptr_t)i);
			if (Selectable(item_text, item_selected))
			{
				value_changed = true;
				*current_item = i;
			}
			if (item_selected)
				SetItemDefaultFocus();
			PopID();
		}
	}

	EndCombo();
	return value_changed;
}


void ImGui::SetItemDefaultFocusAlt()
{
	if (ImGui::IsWindowFocused() && !ImGui::IsAnyItemFocused())
	{
		GImGui->NavId = GImGui->CurrentWindow->DC.LastItemId;
		GImGui->NavDisableHighlight = false;
	}
}


bool ImGui::IsItemFocusedAlt(const char* item_id)
{
	ImGuiContext& g = *GImGui;
	ImGuiWindow* window = g.CurrentWindow;

	if (g.NavId == 0 || g.NavDisableHighlight || g.NavId != window->GetID(item_id))
		return false;
	return true;
}


void ImGui::SetItemFocusAlt(const char* item_id, bool force_focus)
{
	if (ImGui::IsWindowFocused() && !ImGui::IsAnyItemActive())
	{
		if (force_focus || !ImGui::IsAnyItemFocused())
		{
			GImGui->NavId = GImGui->CurrentWindow->GetID(item_id);
			GImGui->NavDisableHighlight = false;
		}
	}
}


bool ImGui::IsItemActiveAlt(const char* item_id)
{
	ImGuiContext& g = *GImGui;
	if (g.ActiveId)
	{
		ImGuiWindow* window = g.CurrentWindow;
		return g.ActiveId == window->GetID(item_id);
	}
	return false;
}


void ImGui::PushStyleVarAlt(ImGuiStyleVar idx, const ImVec2& val)
{
	if (GStyleVarInfo[idx].Count == 1)
		ImGui::PushStyleVar(idx, val.x);
	else
		ImGui::PushStyleVar(idx, val);
}


static float CalcMaxPopupHeightFromItemCount(int items_count)
{
	ImGuiContext& g = *GImGui;
	if (items_count <= 0)
		return FLT_MAX;
	return (g.FontSize + g.Style.ItemSpacing.y) * items_count - g.Style.ItemSpacing.y + (g.Style.WindowPadding.y * 2);
}
bool ImGui::BeginComboAlt(const char* label, const char* preview_value, ImGuiComboFlags flags)
{
	// Always consume the SetNextWindowSizeConstraint() call in our early return paths
	ImGuiContext& g = *GImGui;
	bool has_window_size_constraint = (g.NextWindowData.Flags & ImGuiNextWindowDataFlags_HasSizeConstraint) != 0;
	g.NextWindowData.Flags &= ~ImGuiNextWindowDataFlags_HasSizeConstraint;

	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return false;

	IM_ASSERT((flags & (ImGuiComboFlags_NoArrowButton | ImGuiComboFlags_NoPreview)) != (ImGuiComboFlags_NoArrowButton | ImGuiComboFlags_NoPreview)); // Can't use both flags together

	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID(label);

	const float arrow_size = (flags & ImGuiComboFlags_NoArrowButton) ? 0.0f : GetFrameHeight();
	const ImVec2 label_size = ImMax(CalcTextSize(label, NULL, true), CalcTextSize(preview_value, NULL, true));
	const float expected_w = CalcItemWidth();
	const float w = (flags & ImGuiComboFlags_NoPreview) ? arrow_size : expected_w;
	const ImRect frame_bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(w, label_size.y + style.FramePadding.y * 2.0f));
	const ImRect total_bb(frame_bb.Min, frame_bb.Max + ImVec2(label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f, 0.0f));
	ItemSize(total_bb, style.FramePadding.y);
	if (!ItemAdd(total_bb, id, &frame_bb))
		return false;

	bool hovered, held;
	bool pressed = ButtonBehavior(frame_bb, id, &hovered, &held);
	bool popup_open = IsPopupOpen(id, 0);

	const ImU32 frame_col = GetColorU32(hovered ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg);
	const float value_x2 = ImMax(frame_bb.Min.x, frame_bb.Max.x - arrow_size);
	RenderNavHighlight(frame_bb, id);
	if (!(flags & ImGuiComboFlags_NoPreview))
		window->DrawList->AddRectFilled(frame_bb.Min, ImVec2(value_x2, frame_bb.Max.y), frame_col, style.FrameRounding, (flags & ImGuiComboFlags_NoArrowButton) ? ImDrawCornerFlags_All : ImDrawCornerFlags_Left);
	if (!(flags & ImGuiComboFlags_NoArrowButton))
	{
		ImU32 bg_col = GetColorU32((popup_open || hovered) ? ImGuiCol_ButtonHovered : ImGuiCol_Button);
		ImU32 text_col = GetColorU32(ImGuiCol_Text);
		window->DrawList->AddRectFilled(ImVec2(value_x2, frame_bb.Min.y), frame_bb.Max, bg_col, style.FrameRounding, (w <= arrow_size) ? ImDrawCornerFlags_All : ImDrawCornerFlags_Right);
		if (value_x2 + arrow_size - style.FramePadding.x <= frame_bb.Max.x)
			RenderArrow(window->DrawList, ImVec2(value_x2 + style.FramePadding.y, frame_bb.Min.y + style.FramePadding.y), text_col, ImGuiDir_Down, 1.0f);
	}
	RenderFrameBorder(frame_bb.Min, frame_bb.Max, style.FrameRounding);
	if (preview_value != NULL && !(flags & ImGuiComboFlags_NoPreview))
		RenderTextClipped(frame_bb.Min + style.FramePadding, ImVec2(value_x2, frame_bb.Max.y), preview_value, NULL, NULL, ImVec2(0.0f, 0.0f));
	if (label_size.x > 0)
		RenderText(ImVec2(frame_bb.Max.x + style.ItemInnerSpacing.x, frame_bb.Min.y + style.FramePadding.y), label);

	if ((pressed || g.NavActivateId == id) && !popup_open)
	{
		if (window->DC.NavLayerCurrent == 0)
			window->NavLastIds[0] = id;
		OpenPopupEx(id);
		popup_open = true;
	}

	if (!popup_open)
		return false;

	if (has_window_size_constraint)
	{
		g.NextWindowData.Flags |= ImGuiNextWindowDataFlags_HasSizeConstraint;
		g.NextWindowData.SizeConstraintRect.Min.x = ImMax(g.NextWindowData.SizeConstraintRect.Min.x, w);
	}
	else
	{
		if ((flags & ImGuiComboFlags_HeightMask_) == 0)
			flags |= ImGuiComboFlags_HeightRegular;
		IM_ASSERT(ImIsPowerOfTwo(flags & ImGuiComboFlags_HeightMask_));    // Only one
		int popup_max_height_in_items = -1;
		if (flags & ImGuiComboFlags_HeightRegular)     popup_max_height_in_items = 8;
		else if (flags & ImGuiComboFlags_HeightSmall)  popup_max_height_in_items = 4;
		else if (flags & ImGuiComboFlags_HeightLarge)  popup_max_height_in_items = 20;
		SetNextWindowSizeConstraints(ImVec2(w, 0.0f), ImVec2(FLT_MAX, CalcMaxPopupHeightFromItemCount(popup_max_height_in_items)));
	}

	char name[16];
	ImFormatString(name, IM_ARRAYSIZE(name), "##Combo_%02d", g.BeginPopupStack.Size); // Recycle windows based on depth

	// Peak into expected window size so we can position it
	if (ImGuiWindow * popup_window = FindWindowByName(name))
		if (popup_window->WasActive)
		{
			// Always override 'AutoPosLastDirection' to not leave a chance for a past value to affect us.
            ImVec2 size_expected = CalcWindowNextAutoFitSize(popup_window);
            if (flags & ImGuiComboFlags_PopupAlignLeft)
                popup_window->AutoPosLastDirection = ImGuiDir_Left; // "Below, Toward Left"
            else
                popup_window->AutoPosLastDirection = ImGuiDir_Down; // "Below, Toward Right (default)"
			ImRect r_outer = GetWindowAllowedExtentRect(popup_window);
			ImVec2 pos = FindBestWindowPosForPopupEx(frame_bb.GetBL(), size_expected, &popup_window->AutoPosLastDirection, r_outer, frame_bb, ImGuiPopupPositionPolicy_ComboBox);
			SetNextWindowPos(pos);
		}

	// Horizontally align ourselves with the framed text
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_Popup | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings;
	PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(style.FramePadding.x, style.WindowPadding.y));
	bool ret = Begin(name, NULL, window_flags);
	PopStyleVar();
	if (!ret)
	{
		EndPopup();
		IM_ASSERT(0);   // This should never happen as we tested for IsPopupOpen() above
		return false;
	}
	return true;
}

void ImGui::EraseColumn(const char* item_id, int columns_count)
{
    auto id = ImGui::GetColumnsID(item_id, columns_count);
    ImGuiWindow* window = ImGui::GetCurrentWindow();

	for (int n = 0; n < window->ColumnsStorage.Size; n++) {
		if (window->ColumnsStorage[n].ID == id) {
            window->ColumnsStorage.erase(&(window->ColumnsStorage[n]));
		}
	}
}


#ifdef IMGUI_USER_USE_TLS_FOR_GIMGUI
// TLS Related
#define WIN32_LEAN_AND_MEAN 
#define NOMINMAX
#include <Windows.h>
namespace ImGui
{
	static int __ctxTlsIndex = -1;
	bool TlsInit()
	{
		if (__ctxTlsIndex == -1)
			__ctxTlsIndex = TlsAlloc();

		if (__ctxTlsIndex == -1) return false;
		return true;
	}
	bool SetGImGui(void* ptr)
	{
		return TlsSetValue(__ctxTlsIndex, ptr);
	}
	void* GetGImGui()
	{
		return TlsGetValue(__ctxTlsIndex);
	}
}
#endif