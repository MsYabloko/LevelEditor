#ifndef LEVELEDITOR_BASETOOL_H
#define LEVELEDITOR_BASETOOL_H


#include <glm/vec3.hpp>
#include "../viewports/WorldViewerViewport.h"
#include "../opengl/Camera.h"
#include "../opengl/PlanarCamera.h"
#include "imgui.h"

class Action
{
public:
    virtual void Apply() = 0;
    virtual void Revert() = 0;
    virtual void DrawMenu() = 0;
    virtual ~Action() = default;
    virtual std::string GetName() = 0;
};

class Gizmo
{
public:
    virtual void Draw3D(Camera camera, float width, float height) = 0;
    virtual void Draw2D(PlanarCamera camera, float x, float y, float width, float height, ImDrawList* drawList,
                        int viewportId) = 0;
    virtual void DrawBefore2D(PlanarCamera camera, float x, float y, float width, float height, ImDrawList* drawList,
                              int viewportId) = 0;
    virtual ~Gizmo() = default;
};

class BaseTool
{
public:
    virtual void OnGridClick(int mouse, glm::vec3 worldPos, int viewportId) {};
    virtual void OnGridUp(int mouse, glm::vec3 worldPos, int viewportId) {};
    virtual void OnGridHold(int mouse, glm::vec3 worldPos, int viewportId) {};
    virtual void Update() {}
    virtual void RenderToolbox() = 0;
    virtual ~BaseTool() = default;
};


#endif //LEVELEDITOR_BASETOOL_H
