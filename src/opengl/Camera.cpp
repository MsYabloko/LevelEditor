#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include "Camera.h"

glm::mat4 Camera::get_view()
{
    update_directions();
    return glm::lookAt(Position, Position + forward, up);
}

glm::mat4 Camera::get_projection(float width, float height)
{
    update_directions();
    return glm::perspective(glm::radians(fov), width / height, 0.1f, 100.f);
}

void Camera::update_directions()
{
    glm::vec3 newFront;
    newFront.x = cosf(glm::radians(Rotation.y)) * cosf(glm::radians(Rotation.x));
    newFront.y = sinf(glm::radians(Rotation.x));
    newFront.z = sinf(glm::radians(Rotation.y)) * cosf(glm::radians(Rotation.x));
    forward = glm::normalize(newFront);
    right = glm::normalize(glm::cross(forward, glm::vec3(0, 1, 0)));
    up = glm::normalize(glm::cross(right, forward));
}

glm::vec3 Camera::get_right()
{
    update_directions();
    return right;
}

glm::vec3 Camera::get_forward()
{
    return forward;
}
