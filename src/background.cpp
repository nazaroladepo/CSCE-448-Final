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

glm::vec2 x_intersect_at_y(float y, const glm::vec2& p1, const glm::vec2& p2)
{
    return {(p1.x - p2.x)/(p1.y - p2.y) * (y - p1.y) + p1.x, y};
}

glm::vec2 y_intersect_at_x(float x, const glm::vec2& p1, const glm::vec2& p2)
{
    return {x, (p1.y - p2.y)/(p1.x - p2.x) * (x - p1.x) + p1.y};
}
} // anonymous namespace

namespace svm
{
namespace background
{
Background::Background(std::shared_ptr<texture::Texture2D> bg)
    : m_camera()
    , m_prog(shader::ShaderProgram::textured_object())
    , m_texture(bg)
    , m_vao()
    , m_last_cursor_x()
    , m_last_cursor_y()
    , m_first_cursor_input(true)
{}

void Background::set_user_params
(
    const glm::vec2& top_left,
    const glm::vec2& bot_right,
    const glm::vec2& vanishing,
    float fovy
)
{
    m_camera.pitch = 0;
    m_camera.yaw = -90.0;
    m_camera.fovy = fovy;

    vertex::vertex3_element verts[12];
    calculate_tex_2d(top_left, bot_right, vanishing, verts);
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

    m_vao = vertex::VertexArrayBuffer(verts, ARRAY_SIZE(verts), triangles, ARRAY_SIZE(triangles));
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

void Background::process_input(const window_ptr_t& window, float)
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

void Background::render(const window_ptr_t&, float)
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
    vertex::vertex3_element tex_uv[12]
)
{
    const glm::vec2 bot_left(top_left.x, bot_right.y);
    const glm::vec2 top_right(bot_right.x, top_left.y);

    const auto vp_intersect_at_x = [vanishing](float x, const glm::vec2& corner)
        -> glm::vec2
    {
        return y_intersect_at_x(x, corner, vanishing);
    };

    const auto vp_intersect_at_y = [vanishing](float y, const glm::vec2& corner)
        -> glm::vec2
    {
        return x_intersect_at_y(y, corner, vanishing);
    };

    const auto assign_nth_point = [&tex_uv](int n, const glm::vec2& val)
    {
        tex_uv[n - 1].texture_uv[0] = val.x;
        tex_uv[n - 1].texture_uv[1] = val.y;
    };

    assign_nth_point(1, bot_left);
    assign_nth_point(2, bot_right);
    assign_nth_point(3, vp_intersect_at_y(0, bot_left));
    assign_nth_point(4, vp_intersect_at_y(0, bot_right));
    assign_nth_point(5, vp_intersect_at_x(0, bot_left));
    assign_nth_point(6, vp_intersect_at_x(1, bot_right));
    assign_nth_point(7, top_left);
    assign_nth_point(8, top_right);
    assign_nth_point(9, vp_intersect_at_y(1, top_left));
    assign_nth_point(10, vp_intersect_at_y(1, top_right));
    assign_nth_point(11, vp_intersect_at_x(0, top_left));
    assign_nth_point(12, vp_intersect_at_x(1, top_right));
}

void Background::calculate_box_3d
(
    const glm::vec2& top_left_,
    const glm::vec2& bot_right_,
    const glm::vec2& vanishing_,
    float fovy,
    vertex::vertex3_element box_coords[12]
)
{
    const float tex_width = m_texture->width();
    const float tex_height = m_texture->height();
    const glm::vec2 tex_dimensions(tex_width / tex_height, 1.0f);

    const glm::vec2 top_left_view = comp_mul(top_left_, tex_dimensions);
    const glm::vec2 bot_right_view = comp_mul(bot_right_, tex_dimensions);
    const glm::vec2 vanishing_view = comp_mul(vanishing_, tex_dimensions);

    const float rear_width_view = bot_right_view.x - top_left_view.x;
    const float rear_height_view = top_left_view.y - bot_right_view.y;
    const float rear_aspect = rear_width_view / rear_height_view;

    const float rear_w = REAR_H * rear_aspect;
    const float box_depth = rear_w; // @TODO: replace

    const float degrees_from_floor = (vanishing_view.y - bot_right_view.y) / rear_height_view * fovy;

    m_camera.x = (vanishing_view.x - top_left_view.x) / rear_width_view * rear_w; // ...
    m_camera.y = (vanishing_view.y - bot_right_view.y) / rear_height_view * REAR_H; // ...
    m_camera.z = m_camera.y / glm::tan(glm::radians(degrees_from_floor)); // ...
    //std::cout << "camera: " << m_camera.x << ' ' << m_camera.y << ' ' << m_camera.z << std::endl;
    //std::cout << fovy << std::endl;
    //std::cout << rear_w << ' ' << REAR_H <<  ' ' << box_depth << std::endl;

    const auto calc_box_z = [&](int n) -> float
    {
        //const glm::vec2 tex_uv = glm::vec2(box_coords[n].texture_uv[0], box_coords[n].texture_uv[0])
        //    * tex_dimensions;
        //const float dist_from_vp_view = glm::length(tex_uv - vanishing_view);
        //const float degrees_from_vp = dist_from_vp_view * fovy;
        //return dist_from_vp_view * rear_w;
        return box_depth;
    };

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
    box_coords[2].xyz[2] = calc_box_z(2);
    // 4
    box_coords[3].xyz[0] = rear_w;
    box_coords[3].xyz[1] = 0; //
    box_coords[3].xyz[2] = calc_box_z(3);
    // 5
    box_coords[4].xyz[0] = 0; //
    box_coords[4].xyz[1] = 0; //
    box_coords[4].xyz[2] = calc_box_z(4);
    // 6
    box_coords[5].xyz[0] = rear_w;
    box_coords[5].xyz[1] = 0; //
    box_coords[5].xyz[2] = calc_box_z(5);
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
    box_coords[8].xyz[2] = calc_box_z(8);
    // 10
    box_coords[9].xyz[0] = rear_w;
    box_coords[9].xyz[1] = REAR_H;
    box_coords[9].xyz[2] = calc_box_z(9);
    // 11
    box_coords[10].xyz[0] = 0; //
    box_coords[10].xyz[1] = REAR_H;
    box_coords[10].xyz[2] = calc_box_z(10);
    // 12
    box_coords[11].xyz[0] = rear_w;
    box_coords[11].xyz[1] = REAR_H;
    box_coords[11].xyz[2] = calc_box_z(11);
}
} // namespace background
} // namespace svm
