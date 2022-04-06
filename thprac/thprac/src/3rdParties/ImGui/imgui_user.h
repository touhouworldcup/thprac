#pragma once

namespace ImGui
{
	bool ArrowButtonEx(const char* str_id, ImGuiDir dir, ImVec2 size, ImGuiButtonFlags flags);
	void MinimizeButton(bool* p_min, float max_x, float min_y);
	bool IsWindowResizing();
	void BeginDisabled(bool disabled = true);
	void EndDisabled(bool disabled = true);

	void PushStyleVarAlt(ImGuiStyleVar idx, const ImVec2& val);
	bool BeginComboAlt(const char* label, const char* preview_value, ImGuiComboFlags flags = 0);

	template<typename T>
	bool ComboSections(const char* label, int* current_item, T* selector, char** items, const char* skip)
	{
		if (!selector[0])
		{
			if (!(*current_item)) return false;
			*current_item = 0;
			return true;
		}

		ImGuiContext& g = *ImGui::GetCurrentContext();

		// Call the getter to obtain the preview string which is a parameter to BeginCombo()
		const char* preview_value = items[selector[*current_item]];
		//const char* preview_value = NULL;
		//if (*current_item >= 0 && *current_item < items_count)
		//	items_getter(data, *current_item, &preview_value);

		// The old Combo() API exposed "popup_max_height_in_items". The new more general BeginCombo() API doesn't have/need it, but we emulate it here.
		//if (popup_max_height_in_items != -1 && !g.NextWindowData.SizeConstraintCond)
		//	SetNextWindowSizeConstraints(ImVec2(0, 0), ImVec2(FLT_MAX, CalcMaxPopupHeightFromItemCount(popup_max_height_in_items)));

		if (!BeginComboAlt(label, preview_value, ImGuiComboFlags_None))
			return false;

		// Display items
		// FIXME-OPT: Use clipper (but we need to disable it on the appearing frame to make sure our call to SetItemDefaultFocus() is processed)
		bool value_changed = false;
		for (int i = 0; selector[i]; i++)
		{
			const bool item_selected = (i == *current_item);
			const char* item_text = items[selector[i]];

			if (item_text != skip)
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

	template<typename T>
	bool ComboSectionsDefault(const char* label, int* current_item, T* selector, char** items, const char* skip)
	{
		if (!selector[0])
		{
			if (!(*current_item)) return false;
			*current_item = 0;
			return true;
		}

		ImGuiContext& g = *ImGui::GetCurrentContext();

		// Call the getter to obtain the preview string which is a parameter to BeginCombo()
		const char* preview_value = items[selector[*current_item]];
		//const char* preview_value = NULL;
		//if (*current_item >= 0 && *current_item < items_count)
		//	items_getter(data, *current_item, &preview_value);

		// The old Combo() API exposed "popup_max_height_in_items". The new more general BeginCombo() API doesn't have/need it, but we emulate it here.
		//if (popup_max_height_in_items != -1 && !g.NextWindowData.SizeConstraintCond)
		//	SetNextWindowSizeConstraints(ImVec2(0, 0), ImVec2(FLT_MAX, CalcMaxPopupHeightFromItemCount(popup_max_height_in_items)));

		if (!BeginCombo(label, preview_value, ImGuiComboFlags_None))
			return false;

		// Display items
		// FIXME-OPT: Use clipper (but we need to disable it on the appearing frame to make sure our call to SetItemDefaultFocus() is processed)
		bool value_changed = false;
		for (int i = 0; selector[i]; i++)
		{
			const bool item_selected = (i == *current_item);
			const char* item_text = items[selector[i]];

			if (item_text != skip)
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
}