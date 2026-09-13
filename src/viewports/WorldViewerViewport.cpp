#include <sstream>
#include <iomanip>
#include "WorldViewerViewport.h"
#include "../Project.h"
#include "imgui.h"

void WorldViewerViewport::ShowCellSize(float x, float y, float width, float height, float cellSize)
{
    std::ostringstream oss;
    oss << "Grid: " << std::setprecision(8) << std::noshowpoint << cellSize;
    std::string text = oss.str();
    ImVec2 textSize = ImGui::CalcTextSize(text.c_str());
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    ImVec2 start = {x + width - textSize.x - 10, y + height - textSize.y - 10};
    drawList->AddRectFilled(start, {start.x + textSize.x + 5, start.y + textSize.y + 5}, ImColor(45, 45, 45), 5);
    drawList->AddText({start.x + 2.5f, start.y + 2.5f}, ImColor(255, 255, 255), text.c_str());
}
