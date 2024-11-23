#include "thprac_igi_key_render.h"
#include <cmath>

namespace THPrac {
void static KeysRect(ImDrawList* p, ImVec2 pos, ImVec2 size, float border_size, const KeyRectStyle& style, bool is_pressed, const char* text, bool disabled = false)
{
    uint32_t fill_color = is_pressed ? style.fill_color_press : style.fill_color_release;
    uint32_t border_color = is_pressed ? style.border_color_press : style.border_color_release;
    uint32_t text_color = is_pressed ? style.text_color_press : style.text_color_release;
    if (disabled){
        fill_color = 0xFFAAAAAA;
        border_color = style.border_color_release;
        text_color = 0xFF555555;
    }
    ImVec2 real_pos1 = { pos.x + style.padding.x * size.x, pos.y + style.padding.y * size.y };
    ImVec2 real_pos2 = { real_pos1.x + size.x * (1.0f - 2.0f * style.padding.x), real_pos1.y + +size.y * (1.0f - 2.0f * style.padding.y) };
    switch (style.type) {
    case 3: {
        ImVec2 real_pos_m = { (real_pos1.x + real_pos2.x) * 0.5f, (real_pos1.y + real_pos2.y) * 0.5f };
        ImVec2 sz_ratio = { size.x * (1.0f - 2.0f * style.padding.x), size.y * (1.0f - 2.0f * style.padding.y) };
        constexpr int n_heart = 15;
        static ImVec2 points_heart_u[n_heart + 1] = { { -1, -1 } };
        static ImVec2 points_heart_d[n_heart + 1] = { { -1, -1 } };
        static ImVec2 points_heart_buffer[n_heart][8];
        static ImVec2 points_heart_buffer2[n_heart * 2];
        if (points_heart_u[0].x == -1) { // not init
            for (int i = 0; i <= n_heart; i++) {
                float t = -1.57079f + (float)i / n_heart * 3.1415926f;
                points_heart_u[i] = { 16.0f * powf(sinf(t), 3.0f), 13.0f * cosf(t) - 5.0f * cos(2.0f * t) - 2.0f * cos(3.0f * t) - cosf(4.0f * t) };
                t = 1.57079f + (float)i / n_heart * 3.1415926f;
                points_heart_d[i] = { 16.0f * powf(sinf(t), 3.0f), 13.0f * cosf(t) - 5.0f * cos(2.0f * t) - 2.0f * cos(3.0f * t) - cosf(4.0f * t) };
                points_heart_u[i].x /= 28.0f;
                points_heart_d[i].x /= 28.0f;
                points_heart_u[i].y /= -28.0f;
                points_heart_d[i].y /= -28.0f;
            }
        }
        for (int i = 0; i < n_heart; i++) {
            points_heart_buffer[i][0] = points_heart_u[i];
            points_heart_buffer[i][1] = points_heart_u[i + 1];
            points_heart_buffer[i][2] = { points_heart_u[i + 1].x, points_heart_u[0].y };
            points_heart_buffer[i][3] = { points_heart_u[i].x, points_heart_u[0].y };
            points_heart_buffer[i][4] = points_heart_d[i];
            points_heart_buffer[i][5] = points_heart_d[i + 1];
            points_heart_buffer[i][6] = { points_heart_d[i + 1].x, points_heart_d[0].y };
            points_heart_buffer[i][7] = { points_heart_d[i].x, points_heart_d[0].y };
            for (int j = 0; j < 8; j++) {
                points_heart_buffer[i][j] = { points_heart_buffer[i][j].x * sz_ratio.x + real_pos_m.x,
                    points_heart_buffer[i][j].y * sz_ratio.y + real_pos_m.y };
            }
            points_heart_buffer2[i] = points_heart_buffer[i][0];
            points_heart_buffer2[i + n_heart] = points_heart_buffer[i][4];
        }
        for (int i = 0; i < n_heart; i++) {
            auto pts = points_heart_buffer[i];
            p->AddQuadFilled(pts[0], pts[1], pts[2], pts[3], fill_color);
            p->AddQuadFilled(pts[4], pts[5], pts[6], pts[7], fill_color);
            p->AddLine(pts[1], pts[2], fill_color, 3.0f);
            p->AddLine(pts[5], pts[6], fill_color, 3.0f);
        }
        p->AddLine(points_heart_buffer[0][0], points_heart_buffer[n_heart - 1][1], fill_color, 2.0f);
        p->AddPolyline(points_heart_buffer2, n_heart * 2, border_color, ImDrawFlags_Closed, border_size);
        break;
    }

    case 2: {
        float corner = (1.0f - 2.0f * style.padding.y) * size.y * 0.25f;
        ImVec2 points[] = {
            real_pos1,
            { real_pos2.x - corner, real_pos1.y },
            { real_pos2.x, real_pos1.y + corner },
            real_pos2,
            { real_pos1.x + corner, real_pos2.y },
            { real_pos1.x, real_pos2.y - corner },
        };
        p->AddTriangleFilled(points[0], points[4], points[5], fill_color);
        p->AddTriangleFilled(points[1], points[2], points[3], fill_color);
        p->AddQuadFilled(points[0], points[1], points[3], points[4], fill_color);
        p->AddLine(points[0], points[4], fill_color, 2.0f);
        p->AddLine(points[1], points[3], fill_color, 2.0f);
        p->AddPolyline(points, 6, border_color, ImDrawFlags_Closed, border_size);
        break;
    }
    case 1:
        p->AddRectFilled(real_pos1, real_pos2, fill_color, 0.15f * size.y);
        p->AddRect(real_pos1, real_pos2, border_color, 0.15f * size.y, 0, border_size);
        break;
    default:
    case 0:
        p->AddRectFilled(real_pos1, real_pos2, fill_color);
        p->AddRect(real_pos1, real_pos2, border_color, 0.0f, 0, border_size);
        break;
    }
    ImVec2 textSz = ImGui::CalcTextSize(text);
    ImVec2 textPos = {
        (real_pos1.x + real_pos2.x) * 0.5f - textSz.x * 0.5f,
        (real_pos1.y + real_pos2.y) * 0.5f - textSz.y * 0.5f
    };
    p->AddText(textPos, text_color, text);
}

void KeysHUD(int ver, uint32_t cur_key, ImVec2 render_pos_arrow, ImVec2 render_pos_key, const KeyRectStyle& style, bool align_right_arrow, bool align_right_key)
{
    ImGuiIO& io = ImGui::GetIO();
    float iratio_x = io.DisplaySize.x / 1280.0f;
    float iratio_y = io.DisplaySize.y / 960.0f;
    ImVec2 size = { style.size.x * iratio_x,
        style.size.y * iratio_y };
    render_pos_arrow.x *= iratio_x;
    render_pos_arrow.y *= iratio_y;
    render_pos_key.x *= iratio_y;
    render_pos_key.y *= iratio_y;
    float border_size = ceilf(style.size.x * iratio_x / 32.0f*3.0f);
    enum THKey { Key_Up,
        Key_Down,
        Key_Left,
        Key_Right,
        Key_Z,
        Key_X,
        Key_C,
        Key_D,
        Key_Ctrl,
        Key_Shift,
        END };
    bool keys_down[END] = { 0 };
    int key_mask[END] = { 0 };
    switch (ver) {
    default:
    case 6:
    case 7:
    case 8:
    case 10:
        key_mask[Key_Shift] = 0x4;
        key_mask[Key_Z] = 0x1;
        key_mask[Key_X] = 0x2;
        key_mask[Key_Ctrl] = 0x100;
        key_mask[Key_Up] = 0x10;
        key_mask[Key_Down] = 0x20;
        key_mask[Key_Left] = 0x40;
        key_mask[Key_Right] = 0x80;
        break;
    case 11:
    case 12:
        key_mask[Key_Shift] = 0x8;
        key_mask[Key_Z] = 0x1;
        key_mask[Key_X] = 0x2;
        key_mask[Key_Ctrl] = 0x200;
        key_mask[Key_Up] = 0x10;
        key_mask[Key_Down] = 0x20;
        key_mask[Key_Left] = 0x40;
        key_mask[Key_Right] = 0x80;
        break;
    case 128:
        key_mask[Key_Shift] = 0x8;
        key_mask[Key_Z] = 0x1;
        key_mask[Key_X] = 0x2;
        key_mask[Key_C] = 0x200;
        key_mask[Key_Up] = 0x10;
        key_mask[Key_Down] = 0x20;
        key_mask[Key_Left] = 0x40;
        key_mask[Key_Right] = 0x80;
        break;
    case 13:
    case 16:
        key_mask[Key_Shift] = 0x8;
        key_mask[Key_Z] = 0x1;
        key_mask[Key_X] = 0x2;
        key_mask[Key_C] = 0xA00;
        key_mask[Key_Ctrl] = 0x200;

        key_mask[Key_Up] = 0x10;
        key_mask[Key_Down] = 0x20;
        key_mask[Key_Left] = 0x40;
        key_mask[Key_Right] = 0x80;
        break;
    case 14:
    case 15:
    case 17:
        key_mask[Key_Shift] = 0x8;
        key_mask[Key_Z] = 0x1;
        key_mask[Key_X] = 0x2;
        key_mask[Key_Ctrl] = 0x200;

        key_mask[Key_Up] = 0x10;
        key_mask[Key_Down] = 0x20;
        key_mask[Key_Left] = 0x40;
        key_mask[Key_Right] = 0x80;
        break;
    case 18:
        key_mask[Key_Shift] = 0x8;
        key_mask[Key_Z] = 0x1;
        key_mask[Key_X] = 0x2;
        key_mask[Key_C] = 0x400;
        key_mask[Key_D] = 0x800;

        key_mask[Key_Up] = 0x10;
        key_mask[Key_Down] = 0x20;
        key_mask[Key_Left] = 0x40;
        key_mask[Key_Right] = 0x80;
        break;
    }
    for (int i = 0; i < END; i++) {
        keys_down[i] = ((cur_key & key_mask[i]) == key_mask[i]);
    }
    auto p = ImGui::GetOverlayDrawList();

    if (align_right_arrow){
         render_pos_arrow.x -= size.x * 3.5f;
    }
    if (!style.separated) {
        render_pos_key.x = render_pos_arrow.x - size.x * 4.0f;
    } else {
        if (align_right_key)
            render_pos_key.x -= size.x * 4.0f;
    }
    p->AddRectFilled({ render_pos_arrow.x, render_pos_arrow.y }, { render_pos_arrow.x + size.x * 3.5f, render_pos_arrow.y + size.y * 2.5f }, 0xCC000000);
    p->AddRectFilled({ render_pos_key.x, render_pos_key.y }, { render_pos_key.x + size.x * 4.0f, render_pos_key.y + size.y * 2.5f }, 0xCC000000);
    render_pos_arrow.x += size.x * 0.25f;
    render_pos_arrow.y += size.y * 0.25f;
    render_pos_key.x += size.x * 0.25f;
    render_pos_key.y += size.y * 0.25f;
    render_pos_key.x = roundf(render_pos_key.x);//align to pixel
    render_pos_arrow.x = roundf(render_pos_arrow.x);
    { // arrows
        ImVec2 render_pos_orig = render_pos_arrow;
        render_pos_arrow.x += size.x;
        KeysRect(p, render_pos_arrow, size, border_size, style, keys_down[Key_Up], "↑");
        render_pos_arrow.y += size.y;
        render_pos_arrow.x = render_pos_orig.x;
        KeysRect(p, render_pos_arrow, size, border_size, style, keys_down[Key_Left], "←");
        render_pos_arrow.x += size.x;
        KeysRect(p, render_pos_arrow, size, border_size, style, keys_down[Key_Down], "↓");
        render_pos_arrow.x += size.x;
        KeysRect(p, render_pos_arrow, size, border_size, style, keys_down[Key_Right], "→");
    }
    { // keys
        ImVec2 render_pos_orig = render_pos_key;
        KeysRect(p, render_pos_key, { size.x * 1.5f, size.y }, border_size, style, keys_down[Key_Shift], "Δ");
        render_pos_key.x += size.x * 1.5f;
        KeysRect(p, render_pos_key, size, border_size, style, keys_down[Key_Z], "Z");
        render_pos_key.x += size.x;
        KeysRect(p, render_pos_key, size, border_size, style, keys_down[Key_X], "X");
        render_pos_key.y += size.y;
        render_pos_key.x = render_pos_orig.x;
        KeysRect(p, render_pos_key, size, border_size, style, keys_down[Key_Ctrl], "Σ", key_mask[Key_Ctrl] == 0);
        render_pos_key.x += size.x;
        KeysRect(p, render_pos_key, size, border_size, style, keys_down[Key_C], "C", key_mask[Key_C] == 0);
        render_pos_key.x += size.x;
        KeysRect(p, render_pos_key, size, border_size, style, keys_down[Key_D], "D", key_mask[Key_D] == 0);
    }
}

}