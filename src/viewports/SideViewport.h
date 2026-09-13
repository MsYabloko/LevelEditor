
#ifndef LEVELEDITOR_SIDEVIEWPORT_H
#define LEVELEDITOR_SIDEVIEWPORT_H

#include "../ViewPort.h"
#include "../opengl/Framebuffer.h"
#include "../opengl/Shader.h"
#include "../opengl/Mesh.h"
#include "WorldViewerViewport.h"
#include "../opengl/PlanarCamera.h"

class SideViewport : public WorldViewerViewport
{
private:
    static Shader* _shader;
    static Mesh* _rect;
    bool started_moving = false;
    glm::vec2 start_pos, start_mouse_pos;

    bool isResized = false;
    float prev_width = 0;
    float prev_height = 0;
    Framebuffer* _framebuffer;
    glm::vec3 lookAxis = glm::vec3(0, 0, 1);
public:
    PlanarCamera _camera{};
    explicit SideViewport(glm::vec3 lookAxis);
    void Render(float x, float y, float width, float height, int id) override;
    ~SideViewport() override;

};


#endif //LEVELEDITOR_SIDEVIEWPORT_H
