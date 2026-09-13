
#include <sstream>
#include <iomanip>
#include "ViewPort.h"
#include "imgui.h"

void ViewPort::Render(float x, float y, float width, float height)
{
    /*ImGui::SetCursorScreenPos({x, y});
    ImGui::Image(1, {width, height},{0, 0},{1, 1},
                 ImColor(0, 0, 255).Value);*/

    if(_behaviour != nullptr) _behaviour->Render(x, y, width, height, Id);

    float combo_width = ImGui::CalcTextSize(GetAllBehaviours()[current_selected].name.c_str()).x + 25;
    ImGui::SetCursorScreenPos({x + width - combo_width - 5, y + 5});
    ImGui::PushItemWidth(combo_width);
    ImGui::PushID(Id);
    ImGui::PushStyleColor(ImGuiCol_Button, ImColor(127, 127, 127, 255).Value);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor(169, 169, 169, 255).Value);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImColor(169, 169, 169, 255).Value);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImColor(66, 66, 66, 255).Value);
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImColor(117, 117, 117, 255).Value);
    ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImColor(117, 117, 117, 255).Value);
    ImGui::PushStyleColor(ImGuiCol_PopupBg, ImColor(52, 52, 52, 255).Value);
    ImGui::PushStyleColor(ImGuiCol_Border, ImColor(0, 0, 0, 0).Value);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5);
    ImGui::PushStyleVar(ImGuiStyleVar_PopupRounding, 5);


    if(ImGui::BeginCombo("##viewport_selection", GetAllBehaviours()[current_selected].name.c_str()))
    {
        for(int i = 0; i < GetAllBehaviours().size(); i++ )
        {
            if(ImGui::Selectable(GetAllBehaviours()[i].name.c_str()))
            {
                if(_behaviour != nullptr) delete _behaviour;
                _behaviour = GetAllBehaviours()[i].func();
                current_selected = i;
            }
            if(i == current_selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }
    ImGui::PopStyleColor(8);
    ImGui::PopStyleVar(2);
    ImGui::PopID();
    ImGui::PopItemWidth();
}

#include "viewports/Fullbright3DViewport.h"
#include "viewports/SideViewport.h"
#include "viewports/AssetsViewport.h"

const std::vector<ViewPortBehaviourSelector>& ViewPort::GetAllBehaviours()
{
    static std::vector<ViewPortBehaviourSelector> selectors {
            { "3D (FullBright)", [](){ return new Fullbright3DViewport(); } },
            { "Top", [](){ return new SideViewport(glm::vec3(0, 1, 0)); } },
            { "Forward", [](){ return new SideViewport(glm::vec3(0, 0, 1)); } },
            { "Right", [](){ return new SideViewport(glm::vec3(1, 0, 0)); } },
            { "Assets", [](){ return new AssetsViewport(); } },
    };
    return selectors;
}

ViewPort::ViewPort(int id)
{
    Id = id;
    current_selected = id;
    _behaviour = GetAllBehaviours()[current_selected].func();
}

ViewPortBehaviour* ViewPort::GetBehaviour()
{
    return _behaviour;
}
