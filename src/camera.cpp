#include <glm/gtc/matrix_transform.hpp>

#include "camera.h"

namespace
{
    static glm::vec3 global_up(0.0, 1.0, 0.0);
}

namespace svm
{
namespace camera
{
glm::mat4 Camera::get_view_proj() const
{
    const glm::vec3 pos = position();
    return glm::lookAt(pos, pos + direction(), global_up);
}

glm::vec3 Camera::position() const
{
    return glm::vec3(x, y, z);
}

glm::vec3 Camera::direction() const
{
    glm::vec3 res;
    res.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    res.y = sin(glm::radians(pitch));
    res.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    res = glm::normalize(res);
    return res;
}

void Camera::strafe_left(float d)
{
    const glm::vec3 left = glm::cross(global_up, direction());
    const glm::vec3 movement = d * glm::normalize(left);
    x += movement.x;
    y += movement.y;
    z += movement.z;
}

void Camera::move_forward(float d)
{
    const glm::vec3 movement = direction() * d;
    x += movement.x;
    y += movement.y;
    z += movement.z;
}

void Camera::pitch_up(float deg)
{
    pitch += deg;
}

void Camera::yaw_left(float deg)
{
    yaw += deg;
}
} // namespace camera
} // namespace svm
