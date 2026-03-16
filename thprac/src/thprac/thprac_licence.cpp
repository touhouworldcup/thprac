#include "thprac_licence.h"
#include <imgui.h>

namespace THPrac {
namespace Gui {
    void ShowLicenceInfo()
    {
        if (ImGui::BeginTabBar("COPYING_TABS", ImGuiTabBarFlags_None)) {
            if (ImGui::BeginTabItem("Freetype")) {
                ImGui::BeginChild("COPYING.Freetype");
                ImGui::TextUnformatted(COPYING_FreeType);
                ImGui::EndChild();
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("imgui")) {
                ImGui::BeginChild("COPYING.imgui");
                ImGui::TextUnformatted(COPYING_imgui);
                ImGui::EndChild();
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("MetroHash")) {
                ImGui::BeginChild("COPYING.MetroHash");
                ImGui::TextUnformatted(COPYING_MetroHash);
                ImGui::EndChild();
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("yyjson")) {
                ImGui::BeginChild("COPYING.yyjson");
                ImGui::TextUnformatted(COPYING_yyjson);
                ImGui::EndChild();
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("thprac")) {
                ImGui::BeginChild("COPYING.thprac");
                ImGui::TextUnformatted(COPYING_thprac);
                ImGui::EndChild();
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }
    }
}
}
