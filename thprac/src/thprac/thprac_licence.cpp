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
            if (ImGui::BeginTabItem("rapidjson")) {
                ImGui::BeginChild("COPYING.rapidjson");
                ImGui::TextUnformatted(COPYING_rapidjson);
                ImGui::EndChild();
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("fmt")) {
                ImGui::BeginChild("COPYING.fmt");
                ImGui::TextUnformatted(COPYING_fmt);
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
