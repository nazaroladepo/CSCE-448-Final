#pragma once

#include <glm/glm.hpp>

namespace svm
{
namespace camera
{
struct Camera
{
    glm::mat4 get_view_projection() const;

    glm::vec3 position() const;
    glm::vec3 direction() const;

    void strafe_left(float d);
    void move_forward(float d);
    void pitch_up(float deg);
    void yaw_left(float deg);

    template <typename Int>
    void set_screen(Int width, Int height)
    {
        screen_width = static_cast<float>(width);
        screen_height = static_cast<float>(height);
    }

    float x;
    float y;
    float z;
    float pitch;
    float yaw = -90.0;
    float screen_width = 600.0;
    float screen_height = 400.0;
    float fovy = 54.0;
};
} // namespace camera
} // namespace svm
