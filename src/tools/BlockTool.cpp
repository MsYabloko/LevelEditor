
#include <iostream>
#include "BlockTool.h"
#include "imgui.h"
#include "../Project.h"

static void addFace(std::vector<glm::vec3>& poses, unsigned int v1, unsigned int v2, unsigned int v3,
                    unsigned int v4, std::vector<Vertex>& vertices, glm::vec3 normal, std::vector<unsigned int>& indices)
{
    size_t start = vertices.size();
    vertices.push_back({poses[v1], normal, {0, 0}});
    vertices.push_back({poses[v2], normal, {1, 0}});
    vertices.push_back({poses[v3], normal, {1, 1}});
    vertices.push_back({poses[v4], normal, {0, 1}});
    indices.push_back(start + 0);
    indices.push_back(start + 1);
    indices.push_back(start + 2);
    indices.push_back(start + 2);
    indices.push_back(start + 3);
    indices.push_back(start + 0);
}

static void generateCube(const glm::vec3& minVert, const glm::vec3& maxVert,
                         std::vector<Vertex>& vertices,
                         std::vector<unsigned int>& indices) {



    std::vector<glm::vec3> poses = {
            {minVert.x, minVert.y, minVert.z}, // Bottom left
            { maxVert.x,  minVert.y, minVert.z}, // Bottom right
            { maxVert.x,  minVert.y, maxVert.z}, // Top right
            { minVert.x,  minVert.y, maxVert.z},  // Top left

            {minVert.x, maxVert.y, minVert.z}, // Bottom left
            { maxVert.x,  maxVert.y, minVert.z}, // Bottom right
            { maxVert.x,  maxVert.y, maxVert.z}, // Top right
            { minVert.x,  maxVert.y, maxVert.z}  // Top left
    };

    addFace(poses, 0, 1, 2, 3, vertices, glm::vec3(0, 1, 0), indices);
    addFace(poses, 4, 5, 6, 7, vertices, glm::vec3(0, -1, 0), indices);
    addFace(poses, 3, 0, 4, 7, vertices, glm::vec3(-1, 0, 0), indices);
    addFace(poses, 2, 1, 5, 6, vertices, glm::vec3(1, 0, 0), indices);
    addFace(poses, 0, 1, 5, 4, vertices, glm::vec3(0, 0, -1), indices);
    addFace(poses, 3, 2, 6, 7, vertices, glm::vec3(0, 0, 1), indices);

    /*
    indices = {
            0, 1, 2,
            2, 3, 0,

            4, 5, 6,
            6, 7, 4
    };*/
}

static Mesh* generateCubeMesh(const glm::vec3& minVert, const glm::vec3& maxVert)
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    generateCube(minVert, maxVert, vertices, indices);
    return new Mesh(vertices, indices, GL_STATIC_DRAW);
}

void BlockTool::RenderToolbox()
{
    ImGui::Button("Block Tool ");
}

void BlockTool::OnGridClick(int mouse, glm::vec3 worldPos, int viewportId)
{
    if(_box->IsBusy()) return;
    _box->IsFinished = false;
    _box->minimum = Project::GetCurrent()->SnapToGrid(worldPos);
}

void BlockTool::OnGridUp(int mouse, glm::vec3 worldPos, int viewportId)
{
    if(_box->IsBusy() || _box->IsFinished) return;
    _box->maximum = Project::GetCurrent()->SnapToGrid(worldPos);
    _box->Normalize();
    _box->IsFinished = true;
}

void BlockTool::Update()
{
    Project::GetCurrent()->_gizmos.push_back(_box);
}

void BlockTool::OnGridHold(int mouse, glm::vec3 worldPos, int viewportId)
{
    if(_box->IsBusy()) return;
    _box->maximum = Project::GetCurrent()->SnapToGrid(worldPos);
}

BlockTool::~BlockTool()
{
    delete _box;
}

void BoxCreationGizmo::Normalize()
{
    glm::vec3 min = glm::min(minimum, maximum);
    glm::vec3 max = glm::max(minimum, maximum);
    minimum = min;
    maximum = max;
    if(maximum.x == minimum.x) maximum.x++;
    if(maximum.y == minimum.y) maximum.y++;
    if(maximum.z == minimum.z) maximum.z++;
}

void BoxCreationGizmo::Draw3D(Camera camera, float width, float height)
{
    glm::vec3 min = glm::min(minimum, maximum);
    glm::vec3 max = glm::max(minimum, maximum);
    if(_currentMesh == nullptr || prevMin != min || prevMax != max)
    {
        _currentMesh = generateCubeMesh(min, max);
    }
    prevMin = min;
    prevMax = max;
    _currentMesh->draw();
}

static void Mask(glm::vec3& min, glm::vec3& max, glm::vec3 to, glm::vec3 mask)
{
    if(mask.x > 0) max.x = to.x;
    if(mask.y > 0) max.y = to.y;
    if(mask.z > 0) max.z = to.z;
    if(mask.x < 0) min.x = to.x;
    if(mask.y < 0) min.y = to.y;
    if(mask.z < 0) min.z = to.z;
}

void BoxCreationGizmo::Render2DResize(PlanarCamera camera, float x, float y, float width, float height, ImDrawList* drawList,
                                      glm::vec2 direction, int id, int viewportId, int mode)
{
    glm::vec3 min = glm::min(minimum, maximum);
    glm::vec3 max = glm::max(minimum, maximum);

    const float rectSize = 100.f / camera.Scale;
    const float invisibleSize = 500.f / camera.Scale;

    glm::vec3 middle = (min + max) / 2.f;
    glm::vec3 dirToCorn = max - middle;
    glm::vec3 worldDirection = camera.right * direction.x + camera.up * direction.y;
    glm::vec2 uv = camera.WorldToUv(middle + dirToCorn * worldDirection, width, height);
    uv.x *= width;
    uv.y *= height;
    uv.x += x;
    uv.y += y;

    if(mode == 0) drawList->AddRectFilled({uv.x - rectSize, uv.y - rectSize}, {uv.x + rectSize, uv.y + rectSize}, ImColor(255, 255, 255));

    if(mode == 0) return;
    ImGui::SetCursorScreenPos({uv.x - invisibleSize, uv.y - invisibleSize});
    ImGui::InvisibleButton(("BlockTool_" + std::to_string(id) + "_" + std::to_string(viewportId)).c_str(), {invisibleSize * 2.f, invisibleSize * 2.f});
    if(ImGui::IsItemHovered())
    {
        if(lastHoldId == -1 && ImGui::IsMouseClicked(0))
        {
            lastHoldId = id;
            lastViewportId = viewportId;
        }

        ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
        if(direction.y != 0) ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNS);
        if(direction.x > 0 && direction.y > 0 || direction.x < 0 && direction.y < 0) ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNWSE);
        if(direction.x > 0 && direction.y < 0 || direction.x < 0 && direction.y > 0) ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNESW);
    }
    if(lastHoldId == id && lastViewportId == viewportId && ImGui::IsMouseDown(0))
    {
        glm::vec2 mouseUv = glm::vec2((ImGui::GetMousePos().x - x) / width, (ImGui::GetMousePos().y - y) / height);
        glm::vec3 world = camera.UvToWorld(mouseUv, width, height);
        world = Project::GetCurrent()->SnapToGrid(world);
        Mask(minimum, maximum, world, worldDirection);
    }
    if(lastHoldId == id && lastViewportId == viewportId && ImGui::IsMouseReleased(0))
    {
        lastHoldId = -1;
        lastViewportId = -1;
        Normalize();
    }

    //if(ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal))
}

void BoxCreationGizmo::Draw2D(PlanarCamera camera, float x, float y, float width, float height, ImDrawList* drawList,
                              int viewportId)
{
    glm::vec3 min = glm::min(minimum, maximum);
    glm::vec3 max = glm::max(minimum, maximum);
    glm::vec2 uvMin = camera.WorldToUv(min, width, height) * glm::vec2(width, height) + glm::vec2(x, y);
    glm::vec2 uvMax = camera.WorldToUv(max, width, height) * glm::vec2(width, height) + glm::vec2(x, y);
    if(uvMin == uvMax) return;
    drawList->AddRect({uvMin.x, uvMin.y}, {uvMax.x, uvMax.y}, ImColor(255, 255, 255), 0.f, 0, 2.f);
    drawList->AddRectFilled({uvMin.x, uvMin.y}, {uvMax.x, uvMax.y}, ImColor(0, 0, 64, 64));

    Render2DResize(camera, x, y, width, height, drawList, glm::vec2(1, 0), 0, viewportId, 0);
    Render2DResize(camera, x, y, width, height, drawList, glm::vec2(-1, 0), 1, viewportId, 0);
    Render2DResize(camera, x, y, width, height, drawList, glm::vec2(0, 1), 2, viewportId, 0);
    Render2DResize(camera, x, y, width, height, drawList, glm::vec2(0, -1), 3, viewportId, 0);

    Render2DResize(camera, x, y, width, height, drawList, glm::vec2(1, 1), 4, viewportId, 0);
    Render2DResize(camera, x, y, width, height, drawList, glm::vec2(-1, 1), 5, viewportId, 0);
    Render2DResize(camera, x, y, width, height, drawList, glm::vec2(-1, -1), 6, viewportId, 0);
    Render2DResize(camera, x, y, width, height, drawList, glm::vec2(1, -1), 7, viewportId, 0);

}

BoxCreationGizmo::~BoxCreationGizmo()
{
    delete _currentMesh;
}

bool BoxCreationGizmo::IsBusy()
{
    return lastHoldId != -1;
}

void BoxCreationGizmo::DrawBefore2D(PlanarCamera camera, float x, float y, float width, float height, ImDrawList* drawList,
                               int viewportId)
{
    if(!IsFinished) return;

    if(ImGui::IsKeyPressed(ImGuiKey_Enter))
    {
        glm::vec3 center = (minimum + maximum) / 2.f;
        glm::vec3 size = (maximum - center) * 2.f;
        BoxCreationAction* _action = new BoxCreationAction();
        _action->center = center;
        _action->size = size;
        Project::GetCurrent()->ApplyAction(_action);
        minimum = {0, 0, 0};
        maximum = {0, 0, 0};
        lastHoldId = -1;
        lastViewportId = -1;
        IsFinished = false;
    }

    glm::vec3 min = glm::min(minimum, maximum);
    glm::vec3 max = glm::max(minimum, maximum);
    glm::vec3 maxOffset = max - min;
    glm::vec3 middle = (min + max) / 2.f;
    glm::vec3 minToMiddle = middle - min;
    glm::vec3 worldDirection = camera.right + camera.up;
    glm::vec2 minUv = camera.WorldToUv(min, width, height) * glm::vec2(width, height) + glm::vec2(x, y);
    glm::vec2 maxUv = camera.WorldToUv(max, width, height) * glm::vec2(width, height) + glm::vec2(x, y);
    ImGui::SetCursorScreenPos({minUv.x + 25, minUv.y + 25});
    ImGui::InvisibleButton(("BlockTool_center_" + std::to_string(viewportId)).c_str(), {maxUv.x - minUv.x - 50, maxUv.y - minUv.y - 50});
    if(ImGui::IsItemHovered())
    {
        ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeAll);
        if(lastHoldId == -1 && ImGui::IsMouseClicked(0))
        {
            glm::vec2 mouseUv = glm::vec2((ImGui::GetMousePos().x - x) / width, (ImGui::GetMousePos().y - y) / height);
            glm::vec3 world = camera.UvToWorld(mouseUv, width, height);
            centerOffset = world - middle;
            lastHoldId = 10;
            lastViewportId = viewportId;
        }
    }
    if(lastHoldId == 10 && lastViewportId == viewportId)
    {
        glm::vec2 mouseUv = glm::vec2((ImGui::GetMousePos().x - x) / width, (ImGui::GetMousePos().y - y) / height);
        glm::vec3 world = camera.UvToWorld(mouseUv, width, height);
        Mask(middle, middle, world, worldDirection);
        min = middle - minToMiddle - centerOffset * worldDirection;
        min = Project::GetCurrent()->SnapToGrid(min);
        max = min + maxOffset;
        minimum = min;
        maximum = max;
    }
    if(lastHoldId == 10 && lastViewportId == viewportId && ImGui::IsMouseReleased(0))
    {
        lastHoldId = -1;
        lastViewportId = -1;
        Normalize();
    }

    Render2DResize(camera, x, y, width, height, drawList, glm::vec2(1, 0), 0, viewportId, 1);
    Render2DResize(camera, x, y, width, height, drawList, glm::vec2(-1, 0), 1, viewportId, 1);
    Render2DResize(camera, x, y, width, height, drawList, glm::vec2(0, 1), 2, viewportId, 1);
    Render2DResize(camera, x, y, width, height, drawList, glm::vec2(0, -1), 3, viewportId, 1);

    Render2DResize(camera, x, y, width, height, drawList, glm::vec2(1, 1), 4, viewportId, 1);
    Render2DResize(camera, x, y, width, height, drawList, glm::vec2(-1, 1), 5, viewportId, 1);
    Render2DResize(camera, x, y, width, height, drawList, glm::vec2(-1, -1), 6, viewportId, 1);
    Render2DResize(camera, x, y, width, height, drawList, glm::vec2(1, -1), 7, viewportId, 1);
}

void BoxCreationAction::Apply()
{
    glm::vec3 min = center - size / 2.f;
    glm::vec3 max = center + size / 2.f;
    Mesh* mesh = generateCubeMesh(min, max);

    if(meshId == -1)
    {
        EditableMesh _editable{};
        _editable._mesh = mesh;
        Project::GetCurrent()->meshes.push_back(_editable);
        meshId = Project::GetCurrent()->meshes.size() - 1;
    }
    else
    {
        EditableMesh& _editable = Project::GetCurrent()->meshes[meshId];
        delete _editable._mesh;
        _editable._mesh = mesh;
    }
}

void BoxCreationAction::Revert()
{
    Project::GetCurrent()->meshes.erase(Project::GetCurrent()->meshes.begin() + meshId);
    meshId = -1;
}

static glm::vec3 RenderFloat3(const std::string& name, glm::vec3 vec, float speed)
{
    float vecf[3] = {vec.x, vec.y, vec.z};
    //ImGui::DragFloat3(name.c_str(), &vecf[0], speed);
    //ImGui::SetNextItemWidth(20);
    ImVec2 csp = ImGui::GetCursorScreenPos();
    ImDrawList* list = ImGui::GetWindowDrawList();
    list->AddText({csp.x, csp.y + ImGui::GetTextLineHeight() / 4.f}, ImColor(255, 255, 255), name.c_str());
    ImGui::SetCursorScreenPos({csp.x + 45, csp.y});
    ImGui::SetNextItemWidth(30);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, {3,3});
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3);
    int comp = 185; int nonComp = 55;
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImColor(comp, nonComp, nonComp).Value);
    ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImColor(comp, nonComp, nonComp).Value);
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImColor(comp, nonComp, nonComp).Value);
    ImGui::DragFloat(("##" + name + "_x").c_str(), &vecf[0], speed);
    ImGui::PopStyleColor(3);
    ImGui::SameLine();
    ImGui::SetNextItemWidth(30);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImColor(nonComp, comp, nonComp).Value);
    ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImColor(nonComp, comp, nonComp).Value);
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImColor(nonComp, comp, nonComp).Value);
    ImGui::DragFloat(("##" + name + "_y").c_str(), &vecf[1], speed);
    ImGui::PopStyleColor(3);
    ImGui::SameLine();
    ImGui::SetNextItemWidth(30);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImColor(nonComp, nonComp, comp).Value);
    ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImColor(nonComp, nonComp, comp).Value);
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImColor(nonComp, nonComp, comp).Value);
    ImGui::DragFloat(("##" + name + "_z").c_str(), &vecf[2], speed);
    ImGui::PopStyleColor(3);
    ImGui::PopStyleVar(2);
    return {vecf[0], vecf[1], vecf[2]};
}

void BoxCreationAction::DrawMenu()
{
    float speed = Project::GetCurrent()->GetCellSize();
    glm::vec3 newCenter = RenderFloat3("Center", center, speed);
    glm::vec3 newSize = RenderFloat3("Size", size, speed);
    if(newCenter != center || newSize != size)
    {
        center = newCenter;
        newSize = newSize;

        /*
        glm::vec3 min = center - size / 2.f;
        glm::vec3 max = center + size / 2.f;
        min = Project::GetCurrent()->SnapToGrid(min);
        max = Project::GetCurrent()->SnapToGrid(max);

        center = (min + max) / 2.f;
        size = (max - center) * 2.f;*/

        Apply();
    }
}

std::string BoxCreationAction::GetName()
{
    return "Create Box";
}
