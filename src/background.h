#pragma once

#include "camera.h"
#include "shader.h"
#include "texture.h"
#include "vertex.h"

namespace svm
{
namespace background
{
class Background
{
public:
    Background
    (
        texture::Texture2D&& bg,
        const glm::vec2& top_left,
        const glm::vec2& bot_right,
        const glm::vec2& vanishing,
        float fovy
    );

    void orient_camera_initially(camera::Camera& camera);

    void render(const glm::mat4& view_proj);

private:
    void calculate_tex_2d
    (
        const glm::vec2& view_top_left,
        const glm::vec2& bot_right,
        const glm::vec2& vanishing,
        float fovy,
        vertex::vertex3_element tex_uv[12]
    );

    void calculate_box_3d
    (
        const glm::vec2& view_top_left,
        const glm::vec2& bot_right,
        const glm::vec2& vanishing,
        float fovy,
        vertex::vertex3_element box_coords[12]
    );

    shader::ShaderProgram m_prog;
    texture::Texture2D m_texture;
    vertex::VertexArrayBuffer m_vao;
    glm::vec3 m_start_pos;
    float m_start_fov;
};
} // namespace background
} // namespace svm
