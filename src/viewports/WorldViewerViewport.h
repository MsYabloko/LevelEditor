#ifndef LEVELEDITOR_WORLDVIEWERVIEWPORT_H
#define LEVELEDITOR_WORLDVIEWERVIEWPORT_H

#include <glm/vec3.hpp>
#include "../ViewPort.h"

class WorldViewerViewport : public ViewPortBehaviour
{
protected:
    void ShowCellSize(float x, float y, float width, float height, float cellSize);
};

#endif //LEVELEDITOR_WORLDVIEWERVIEWPORT_H
