#pragma once
#include "imgui.h"
#include <cstdint>
#include <vector>
namespace THPrac {
struct KeyRectStyle {
    bool separated;
    bool show_aps;
    uint32_t fill_color_press;
    uint32_t fill_color_release;
    uint32_t border_color_press;
    uint32_t border_color_release;
    uint32_t text_color_press;
    uint32_t text_color_release;
    ImVec2 padding;
    ImVec2 size;
    int type;
};
void RecordKey(int ver, uint32_t cur_key);
void KeysHUD(int ver, ImVec2 render_pos_arrow, ImVec2 render_pos_key, const KeyRectStyle& style, bool align_right_arrow = true, bool align_right_key = false); // assume game window's 1280*960
void SaveKeyRecorded();
void ClearKeyRecord();
std::vector<uint8_t>& GetKeyAPS();
}