#include "thprac_th19.h"
#include "thprac_games.h"

namespace THPrac {
namespace TH19 {

    void drawEnemyHP(PCONTEXT pCtx, float SCALE)
    {
        uint32_t side = *(uint32_t*)(pCtx->Ecx + 0x5638);

        if (!GameState_Assert(side <= 1)) {
            return;
        }

        static const ImVec2 side_offsets[] = {
            { 164.0f, 16.0f },
            { 476.0f, 16.0f },
        };

        const ImVec2& offset = side_offsets[side];

        uint32_t flags = *(uint32_t*)(pCtx->Ecx + 0x516c);
        if (flags & 0x21) {
            return;
        }

        float* enm_pos = (float*)(pCtx->Ecx + 0x48);
        uint32_t hp = *(uint32_t*)(pCtx->Ecx + 0x5008);

        std::string hp_str = std::to_string(hp);

        auto* drawList = ImGui::GetOverlayDrawList();
        auto* font = ImGui::GetFont();

        ImVec2 textSize = font->CalcTextSizeA(font->FontSize, ImGui::GetIO().DisplaySize.x, 0.0f, hp_str.c_str(), hp_str.c_str() + hp_str.size());

        ImVec2 upperLeft = ImVec2((enm_pos[0] + offset.x) * SCALE, (enm_pos[1] + offset.y) * SCALE);
        ImVec2 lowerRight = ImVec2(upperLeft.x + textSize.x, upperLeft.y + textSize.y);

        drawList->AddRectFilled(upperLeft, lowerRight, 0xAA555555);
        drawList->AddText(upperLeft, 0xFFFF55FF, hp_str.c_str(), hp_str.c_str() + hp_str.size());
    }

}
}