
#ifndef LEVELEDITOR_CAMERA_H
#define LEVELEDITOR_CAMERA_H


#include <glm/vec3.hpp>
#include <glm/glm.hpp>

class Camera
{
private:
    glm::vec3 forward;
    glm::vec3 up;
    glm::vec3 right;
public:
    float fov = 90;
    void update_directions();
    glm::vec3 Position = {0, 0, 0};
    glm::vec3 Rotation = {0, 0, 0};
    glm::vec3 get_forward();
    glm::vec3 get_right();
    glm::mat4 get_view();
    glm::mat4 get_projection(float width, float height);
};


#endif //LEVELEDITOR_CAMERA_H
