#include <iostream>
#include <utility>

#include "background.h"

namespace
{
constexpr float REAR_H = 20; // arbitrary

glm::vec2 comp_mul(const glm::vec2& a, const glm::vec2& b)
{
    return glm::vec2(a.x * b.x, a.y * b.y);
}
} // anonymous namespace

namespace svm
{
namespace background
{
Background::Background
(
    std::shared_ptr<texture::Texture2D> bg,
    const glm::vec2& top_left,
    const glm::vec2& bot_right,
    const glm::vec2& vanishing,
    float fovy
)
    : m_camera()
    , m_prog(shader::ShaderProgram::textured_object())
    , m_texture(bg)
    , m_vao()
    , m_last_cursor_x()
    , m_last_cursor_y()
    , m_first_cursor_input(true)
{
    m_camera.pitch = 0;
    m_camera.yaw = -90.0;
    m_camera.fovy = fovy;
    std::cout << fovy << std::endl;

    vertex::vertex3_element verts[12];
    calculate_tex_2d(top_left, bot_right, vanishing, fovy, verts);
    calculate_box_3d(top_left, bot_right, vanishing, fovy, verts);

    vertex::indexed_triangle triangles[10] =
    {
        // THESE INDEXES ARE 1-based, we readjust below
        // rear wall
        {  1,  8,  7 },
        {  1,  2,  8 },
        // floor
        {  1,  3,  2 },
        {  2,  3,  4 },
        // ceiling
        {  7,  8, 10 },
        {  7, 10,  9 },
        // left
        {  1, 11,  5 },
        {  1,  7, 11 },
        // right
        {  2,  6, 12 },
        {  2, 12,  8 },
    };
    for (int i = 0; i < 10; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            triangles[i][j]--;
        }
    }

    m_vao = vertex::VertexArrayBuffer(verts, 12, triangles, 10);
}

void Background::setup(const window_ptr_t& window)
{
    int win_width, win_height;
    window->get_window_size(win_width, win_height);
    m_camera.set_screen(win_width, win_height);

    window->set_cursor_enabled(false);
    window->set_resize_callback([this](int width, int height)
    {
        m_camera.set_screen(width, height);
    });
    window->set_cursor_pos_callback(
        [this](double x_pos, double y_pos)
    {
        constexpr float sensitivity = 0.1;

        if (m_first_cursor_input)
        {
            m_last_cursor_x = static_cast<float>(x_pos);
            m_last_cursor_y = static_cast<float>(y_pos);
            m_first_cursor_input = false;
        }

        const float offset_x = static_cast<float>(x_pos) - m_last_cursor_x;
        const float offset_y = static_cast<float>(y_pos) - m_last_cursor_y;

        m_camera.yaw_left(offset_x * sensitivity);
        m_camera.pitch_up(-offset_y * sensitivity);

        m_last_cursor_x = x_pos;
        m_last_cursor_y = y_pos;
    });
}

void Background::process_input(const window_ptr_t& window, float frame_time)
{
    constexpr float move_size = 0.05;
    if (window->key_is_pressed(GLFW_KEY_W))
    {
        m_camera.move_forward(move_size);
    }
    if (window->key_is_pressed(GLFW_KEY_S))
    {
        m_camera.move_forward(-move_size);
    }
    if (window->key_is_pressed(GLFW_KEY_A))
    {
        m_camera.strafe_left(move_size);
    }
    if (window->key_is_pressed(GLFW_KEY_D))
    {
        m_camera.strafe_left(-move_size);
    }
}

void Background::render(const window_ptr_t& window, float frame_time)
{
    m_prog.use();
    m_prog.setUniformMat4("camera", m_camera.get_view_projection());
    m_texture->insert_to_unit_spot(GL_TEXTURE0);
    m_vao.draw_elements();
}

void Background::calculate_tex_2d
(
    const glm::vec2& top_left,
    const glm::vec2& bot_right,
    const glm::vec2& vanishing,
    float fovy,
    vertex::vertex3_element tex_uv[12]
)
{
    // 1
    tex_uv[0].texture_uv[0] = top_left.x;
    tex_uv[0].texture_uv[1] = bot_right.y;
    // 2
    tex_uv[1].texture_uv[0] = bot_right.x;
    tex_uv[1].texture_uv[1] = bot_right.y;
    // 3
    tex_uv[2].texture_uv[0] = 0.0;
    tex_uv[2].texture_uv[1] = 0.0;
    // 4
    tex_uv[3].texture_uv[0] = 1.0;
    tex_uv[3].texture_uv[1] = 0.0;
    // 5
    tex_uv[4].texture_uv[0] = 0.0;
    tex_uv[4].texture_uv[1] = 0.0;
    // 6
    tex_uv[5].texture_uv[0] = 1.0;
    tex_uv[5].texture_uv[1] = 0.0;
    // 7
    tex_uv[6].texture_uv[0] = top_left.x;
    tex_uv[6].texture_uv[1] = top_left.y;
    // 8
    tex_uv[7].texture_uv[0] = bot_right.x;
    tex_uv[7].texture_uv[1] = top_left.y;
    // 9
    tex_uv[8].texture_uv[0] = 0.0;
    tex_uv[8].texture_uv[1] = 1.0;
    // 10
    tex_uv[9].texture_uv[0] = 1.0;
    tex_uv[9].texture_uv[1] = 1.0;
    // 11
    tex_uv[10].texture_uv[0] = 0.0;
    tex_uv[10].texture_uv[1] = 1.0;
    // 12
    tex_uv[11].texture_uv[0] = 1.0;
    tex_uv[11].texture_uv[1] = 1.0;
}

void Background::calculate_box_3d
(
    const glm::vec2& top_left,
    const glm::vec2& bot_right,
    const glm::vec2& vanishing,
    float fovy,
    vertex::vertex3_element box_coords[12]
)
{
    const float tex_width = m_texture->width();
    const float tex_height = m_texture->height();
    const glm::vec2 tex_dimensions(tex_width, tex_height);
    const glm::vec2 tl = comp_mul(top_left, tex_dimensions);
    const glm::vec2 br = comp_mul(bot_right, tex_dimensions);
    const glm::vec2 vp_in_rear = comp_mul(vanishing, tex_dimensions);

    const float rear_w = REAR_H * tex_width / tex_height;
    const float box_depth = rear_w; // @TODO: replace

    // 1
    box_coords[0].xyz[0] = 0; //
    box_coords[0].xyz[1] = 0; //
    box_coords[0].xyz[2] = 0; //
    // 2
    box_coords[1].xyz[0] = rear_w;
    box_coords[1].xyz[1] = 0; //
    box_coords[1].xyz[2] = 0; //
    // 3
    box_coords[2].xyz[0] = 0; // 
    box_coords[2].xyz[1] = 0; //
    box_coords[2].xyz[2] = box_depth;
    // 4
    box_coords[3].xyz[0] = rear_w;
    box_coords[3].xyz[1] = 0; //
    box_coords[3].xyz[2] = box_depth;
    // 5
    box_coords[4].xyz[0] = 0; //
    box_coords[4].xyz[1] = 0; //
    box_coords[4].xyz[2] = box_depth;
    // 6
    box_coords[5].xyz[0] = rear_w;
    box_coords[5].xyz[1] = 0; //
    box_coords[5].xyz[2] = box_depth;
    // 7
    box_coords[6].xyz[0] = 0; //
    box_coords[6].xyz[1] = REAR_H;
    box_coords[6].xyz[2] = 0; //
    // 8
    box_coords[7].xyz[0] = rear_w;
    box_coords[7].xyz[1] = REAR_H;
    box_coords[7].xyz[2] = 0; //
    // 9
    box_coords[8].xyz[0] = 0; //
    box_coords[8].xyz[1] = REAR_H;
    box_coords[8].xyz[2] = box_depth;
    // 10
    box_coords[9].xyz[0] = rear_w;
    box_coords[9].xyz[1] = REAR_H;
    box_coords[9].xyz[2] = box_depth;
    // 11
    box_coords[10].xyz[0] = 0; //
    box_coords[10].xyz[1] = REAR_H;
    box_coords[10].xyz[2] = box_depth;
    // 12
    box_coords[11].xyz[0] = rear_w;
    box_coords[11].xyz[1] = REAR_H;
    box_coords[11].xyz[2] = box_depth;

    m_camera.x = 0; // ...
    m_camera.y = 0; // ...
    m_camera.z = 3; // ...
}
} // namespace background
} // namespace svm
