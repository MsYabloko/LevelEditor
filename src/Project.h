
#ifndef LEVELEDITOR_PROJECT_H
#define LEVELEDITOR_PROJECT_H


#include "tools/BaseTool.h"
#include "opengl/Mesh.h"

class Project
{
private:
    std::vector<Action*> _actions;
    int _currentAction = 0;
    static Project* _current;
public:
    std::vector<Gizmo*> _gizmos{};
    ViewPort _viewports[4]{ ViewPort{0}, ViewPort{1}, ViewPort{2}, ViewPort{3} };
    std::vector<EditableMesh> meshes{};
    BaseTool* CurrentTool = nullptr;
    static Project* GetCurrent();
    static void CreateNew();
    int cell_multiplier = 0;
    float GetCellSize();
    glm::vec3 SnapToGrid(glm::vec3);

    void ApplyAction(Action* action);
    void RevertAction();
    void RenderAction();
    std::string GetActionName();
};


#endif //LEVELEDITOR_PROJECT_H
