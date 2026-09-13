#ifndef LEVELEDITOR_PLANARCAMERA_H
#define LEVELEDITOR_PLANARCAMERA_H


#include <glm/vec2.hpp>
#include <glm/ext/matrix_float4x4.hpp>

class PlanarCamera
{
public:
    glm::vec3 right {1, 0, 0};
    glm::vec3 up {0, 1, 0};
    float Scale = 25.f;
    glm::vec2 Position;
    glm::mat4 GetView() const;
    glm::mat4 GetProjection(float width, float height) const;
    glm::vec3 UvToWorld(glm::vec2 uv, float width, float height);
    glm::vec2 UvToPlanarWorld(glm::vec2 uv, float width, float height);
    glm::vec2 WorldToUv(glm::vec3 world, float width, float height);
    glm::vec2 PlanarWorldToUv(glm::vec2 world, float width, float height);
};


#endif //LEVELEDITOR_PLANARCAMERA_H
