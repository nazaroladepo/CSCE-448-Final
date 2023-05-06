#pragma once

#include "camera.h"
#include "scene.h"
#include "shader.h"
#include "texture.h"
#include "vertex.h"
#include "window.h"

namespace svm
{
namespace background
{
class Background: public scene::Scene
{
public:
    Background(std::shared_ptr<texture::Texture2D> bg);

    void set_user_params
    (
        const glm::vec2& top_left,
        const glm::vec2& bot_right,
        const glm::vec2& vanishing,
        float fovy
    );

    void setup(const window_ptr_t& window) override;
    void process_input(const window_ptr_t& window, float frame_time) override;
    void render(const window_ptr_t& window, float frame_time) override;

private:
    void calculate_tex_2d
    (
        const glm::vec2& view_top_left,
        const glm::vec2& bot_right,
        const glm::vec2& vanishing,
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

    camera::Camera m_camera;
    shader::ShaderProgram m_prog;
    std::shared_ptr<texture::Texture2D> m_texture;
    vertex::VertexArrayBuffer m_vao;
    float m_last_cursor_x;
    float m_last_cursor_y;
    bool m_first_cursor_input;
};
} // namespace background
} // namespace svm
