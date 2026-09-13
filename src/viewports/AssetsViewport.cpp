
#include "AssetsViewport.h"
#include "imgui.h"

void AssetsViewport::Render(float x, float y, float width, float height, int id)
{
    float button_width = 100;
    const char* items[] = {"Materials", "Models", "Prefabs"};
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    drawList->AddRectFilled({x, y}, {x + width, y + height}, ImColor(40, 40, 40));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5);
    for(int i = 0; i < IM_ARRAYSIZE(items);i++)
    {
        ImGui::SetCursorScreenPos({x + (button_width + 5) * i + 5, y + 5});
        int r = current_tab == i ? 90 : 61;
        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, current_tab == i ? 1 : 0);
        ImGui::PushStyleColor(ImGuiCol_Border, ImColor(150, 150, 150).Value);
        ImGui::PushStyleColor(ImGuiCol_Button, ImColor(r, r, r).Value);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor(r + 20, r + 20, r + 20).Value);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImColor(r + 40, r + 40, r + 40).Value);
        if(ImGui::Button((items[i] + ("##" + std::to_string(id))).c_str(), {button_width,0}))
            current_tab = i;
        ImGui::PopStyleColor(4);
        ImGui::PopStyleVar();
    }
    ImGui::PopStyleVar();
    ImGui::SetCursorScreenPos({x + 5, y + 25 + 5});
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImColor(30, 30, 30, 255).Value);
    if(ImGui::BeginChildFrame(6 + id, {width - 5 - 5, height - 5 - 5 - 25}))
    {
        ImGui::PopStyleVar(2);
        ImGui::PopStyleColor();
        float size = 128;
        int inrow = 0;
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5);
        for(int i = 0; i < 5 * (3 - current_tab);i++)
        {
            ImGui::PushID(i);
            ImGui::ColorButton("aaa", ImColor(128 * (current_tab), 128 * (current_tab > 0 ? 0 : 1), 128 * (current_tab > 1 ? 0 : 1)), ImGuiColorEditFlags_NoDragDrop, {size, size});
            ImGui::PopID();
            inrow++;
            if((size + 10) * inrow < width - size) ImGui::SameLine();
            else inrow = 0;
        }
        ImGui::PopStyleVar();
        ImGui::EndChild();
    }
}
