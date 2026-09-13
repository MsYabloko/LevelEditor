
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include "PlanarCamera.h"

glm::mat4 PlanarCamera::GetProjection(float width, float height) const
{
    float aspect = width / height;
    return glm::ortho(0.f, aspect * Scale, 1.f * Scale, 0.f, 0.25f, 100.f);
}

glm::mat4 PlanarCamera::GetView() const
{
    return glm::translate(glm::mat4(1.f), up * Position.y + right * Position.x);
}

glm::vec2 PlanarCamera::WorldToUv(glm::vec3 world, float width, float height)
{
    return PlanarWorldToUv(glm::vec2(glm::dot(world, right), glm::dot(world, up)), width, height);
}

glm::vec3 PlanarCamera::UvToWorld(glm::vec2 uv, float width, float height)
{
    uv = UvToPlanarWorld(uv, width, height);
    glm::vec3 result = right * uv.x + up * uv.y;
    return result;
}

glm::vec2 PlanarCamera::PlanarWorldToUv(glm::vec2 world, float width, float height)
{
    float aspect = width / height;
    float worldRight = world.x - Position.x;
    float worldUp = world.y - Position.y;
    worldRight /= Scale;
    worldRight /= aspect;
    worldUp /= Scale;
    return glm::vec2(worldRight * 2.f + 0.5f, worldUp * 2.f + 0.5f);
}

glm::vec2 PlanarCamera::UvToPlanarWorld(glm::vec2 uv, float width, float height)
{
    float aspect = width / height;
    uv -= glm::vec2(0.5f, 0.5f);
    uv.x *= aspect;
    uv /= 2;
    uv *= Scale;
    uv += Position;
    return uv;
}
