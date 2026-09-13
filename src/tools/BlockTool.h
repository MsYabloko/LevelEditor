
#ifndef LEVELEDITOR_BLOCKTOOL_H
#define LEVELEDITOR_BLOCKTOOL_H


#include "BaseTool.h"
#include "../opengl/Mesh.h"
#include "imgui.h"

class BoxCreationAction : public Action
{
private:
    int meshId = -1;
public:
    glm::vec3 center;
    glm::vec3 size;
    void Apply() override;
    void Revert() override;
    void DrawMenu() override;
    std::string GetName() override;
    ~BoxCreationAction() override = default;

};

class BoxCreationGizmo : public Gizmo
{
private:
    glm::vec3 centerOffset{};
    Mesh* _currentMesh = nullptr;
    glm::vec3 prevMin {};
    glm::vec3 prevMax {};
    int lastHoldId = -1;
    int lastViewportId = -1;
    void Render2DResize(PlanarCamera camera, float x, float y, float width, float height, ImDrawList* drawList,
                        glm::vec2 direction, int id, int viewportId, int mode);
public:
    void Normalize();
    bool IsFinished = false;
    bool IsBusy();
    void Draw3D(Camera camera, float width, float height) override;
    void Draw2D(PlanarCamera camera, float x, float y, float width, float height, ImDrawList* drawList,
                int viewportId) override;
    glm::vec3 minimum{};
    glm::vec3 maximum{};
    ~BoxCreationGizmo() override;

    void DrawBefore2D(PlanarCamera camera, float x, float y, float width, float height, ImDrawList* drawList,
                      int viewportId) override;
};

class BlockTool : public BaseTool
{
private:
    BoxCreationGizmo* _box = new BoxCreationGizmo();
public:
    void RenderToolbox() override;

    void OnGridClick(int mouse, glm::vec3 worldPos, int viewportId) override;
    void OnGridHold(int mouse, glm::vec3 worldPos, int viewportId) override;
    void OnGridUp(int mouse, glm::vec3 worldPos, int viewportId) override;
    void Update() override;
    ~BlockTool() override;
};


#endif //LEVELEDITOR_BLOCKTOOL_H
