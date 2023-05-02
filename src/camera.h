#pragma once

#include <glm/glm.hpp>

namespace svm
{
namespace camera
{
struct Camera
{
    glm::mat4 get_view_proj() const;

    glm::vec3 position() const;
    glm::vec3 direction() const;

    void strafe_left(float d);
    void move_forward(float d);
    void pitch_up(float deg);
    void yaw_left(float deg);

    template <typename Int>
    void set_screen(Int width, Int height)
    {
        aspect_ratio = static_cast<float>(width) / static_cast<float>(height);
    }

    float x;
    float y;
    float z;
    float pitch;
    float yaw;
    float aspect_ratio = 1.0;
    float fovy_deg = 45.0;
};
} // namespace camera
} // namespace svm
