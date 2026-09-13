
#ifndef LEVELEDITOR_FULLBRIGHT3DVIEWPORT_H
#define LEVELEDITOR_FULLBRIGHT3DVIEWPORT_H

#include "../ViewPort.h"
#include "../opengl/Framebuffer.h"
#include "../opengl/Shader.h"
#include "../opengl/Mesh.h"
#include "../opengl/Camera.h"
#include "WorldViewerViewport.h"

class Fullbright3DViewport : public WorldViewerViewport
{
private:
    double prev_time = 0;
    bool started_moving = false;
    glm::vec2 start_mouse{};
    glm::vec3 start_rotation{};
    static Shader* _shader;
    static Shader* _worldShader;
    static Mesh* _mesh;
    bool isResized = false;
    float prev_width = 0;
    float prev_height = 0;
    Framebuffer* _framebuffer{};

    unsigned int VBO{};
public:
    Camera camera{};
    Fullbright3DViewport();
    ~Fullbright3DViewport() override;
    void Render(float x, float y, float width, float height, int id) override;

};


#endif //LEVELEDITOR_FULLBRIGHT3DVIEWPORT_H
