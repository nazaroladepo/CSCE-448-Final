#include "mesh.h"
#include "window.h"

namespace
{
    static constexpr const int UP = 0;
    static constexpr const int DOWN = 0;
    static constexpr const int LEFT = 0;
    static constexpr const int RIGHT = 0;
    static constexpr const int VANISHING = 0;

    static constexpr const double VANISHING_DRAG_RADIUS = 15.0;

    static constexpr const float DOT_Z = -0.01f;
    static constexpr const float MESH_Z = -0.02f;
    static constexpr const float TEX_Z = -0.03f;

    static constexpr const float DOT_RADIUS = 0.005f;
    svm::vertex::vertex3_element dot_verts[4] =
    {
        { { -DOT_RADIUS, -DOT_RADIUS, DOT_Z }, {} },
        { {  DOT_RADIUS, -DOT_RADIUS, DOT_Z }, {} },
        { {  DOT_RADIUS,  DOT_RADIUS, DOT_Z }, {} },
        { { -DOT_RADIUS,  DOT_RADIUS, DOT_Z }, {} }
    };

    svm::vertex::indexed_triangle quad_tris[2] =
    {
        { 0, 1, 2 },
        { 0, 2, 3 }
    };

    svm::vertex::vertex3_element ui_tex_verts[4] =
    {
        { { 0.0, 0.0, TEX_Z }, { 0.0, 0.0 } },
        { { 1.0, 0.0, TEX_Z }, { 1.0, 0.0 } },
        { { 1.0, 1.0, TEX_Z }, { 1.0, 1.0 } },
        { { 0.0, 1.0, TEX_Z }, { 0.0, 1.0 } }
    };

    glm::vec2 screen_2_gl
    (
        const std::shared_ptr<svm::window::Window>& window,
        const glm::vec2& screen)
    {
        float gl_x, gl_y;
        window->screen_2_gl(screen.x, screen.y, gl_x, gl_y);
        return glm::vec2(gl_x, gl_y);
    }
}

namespace svm
{
namespace mesh
{
Mesh::Mesh(const std::shared_ptr<texture::Texture2D>& tex)
    : top_left()
    , bot_right()
    , vanishing()
    , m_tex_prog(shader::ShaderProgram::textured_object()) 
    , m_mesh_prog(shader::ShaderProgram::flat_color(glm::vec4(1.0, 1.0, 1.0, 1.0)))
    , m_dot_prog(shader::ShaderProgram::gui_dot(glm::vec3(1.0, 0, 0), DOT_RADIUS))
    , m_tex(tex)
    , m_tex_vao(ui_tex_verts, 4, quad_tris, 2)
    , m_mesh_vao()
    , m_dot_vao(dot_verts, 4, quad_tris, 2)
    , m_dragging_edge(-1)
    , m_done(false)
{
    recalculate_mesh();
}

void Mesh::setup(const window_ptr_t& window)
{
    const int win_height = 640;
    const int win_width = win_height * m_tex->width() / m_tex->height();
    window->set_cursor_enabled(true);
    window->set_window_size(win_width, win_height);
    window->enforce_aspect_ratio(m_tex->width(), m_tex->height());

    top_left = glm::vec2(win_width * 0.25, win_height * 0.25);
    bot_right = glm::vec2(win_width * 0.75, win_height * 0.75);
    vanishing = glm::vec2(win_width * 0.5, win_height * 0.5);
}

void Mesh::process_input(const window_ptr_t& window, float)
{
    if (window->key_is_pressed(GLFW_KEY_ENTER))
    {
        // Convert screen coords to GL texture coords
        top_left = screen_2_gl(window, top_left);
        bot_right = screen_2_gl(window, bot_right);
        vanishing = screen_2_gl(window, vanishing);
        m_done = true;
        return;
    }

    if (window->is_cursor_pressed())
    {
        double cursor_x, cursor_y;
        window->get_cursor_pos(cursor_x, cursor_y);

        if (m_dragging_edge == VANISHING)
        {
            vanishing = glm::vec2(cursor_x, cursor_y);
        }
        else if (m_dragging_edge == UP)
        {
            top_left.y = cursor_y;
        }
        else if (m_dragging_edge == DOWN)
        {
            bot_right.y = cursor_y;
        }
        else if (m_dragging_edge == LEFT)
        {
            top_left.y = cursor_x;
        }
        else if (m_dragging_edge == RIGHT)
        {
            bot_right.y = cursor_x;
        }
        else if (m_dragging_edge == -1)
        {
            double vanishing_dist2 = (cursor_x - vanishing.x) * (cursor_x - vanishing.x) +
                (cursor_y - vanishing.y) * (cursor_y - vanishing.y);
            if (vanishing_dist2 <= VANISHING_DRAG_RADIUS * VANISHING_DRAG_RADIUS) {
                m_dragging_edge = VANISHING;
            } else if (cursor_y < top_left.y) {
                m_dragging_edge = UP;
            } else if (cursor_y > bot_right.y) {
                m_dragging_edge = DOWN;
            } else if (cursor_x < top_left.x) {
                m_dragging_edge = LEFT;
            } else if (cursor_x > bot_right.x) {
                m_dragging_edge = RIGHT;
            }
        }

        if (bot_right.y < top_left.y)
        {
            std::swap(bot_right.y, top_left.y);
            if (m_dragging_edge == UP) {
                m_dragging_edge = DOWN;
            } else if (m_dragging_edge == DOWN) {
                m_dragging_edge = UP;
            } else 
            {
                throw std::logic_error("top/bottom crossed while not dragging UP/DOWN");
            }
        }
        else if (bot_right.x < top_left.x)
        {
            if (m_dragging_edge == LEFT) {
                m_dragging_edge = RIGHT;
            } else if (m_dragging_edge == RIGHT) {
                m_dragging_edge = LEFT;
            } else 
            {
                throw std::logic_error("left/right crossed while not dragging LEFT/RIGHT");
            }
        }

        recalculate_mesh();
    }
    else
    {
        m_dragging_edge = -1;
    }
}

void Mesh::render(const window_ptr_t& window, float)
{
    m_tex_prog.use();
    m_tex->insert_to_unit_spot(0);
    m_tex_vao.draw_elements();

    //m_mesh_prog.use();
    //m_mesh_vao.draw_elements();

    static constexpr const glm::vec3 REAR_DOTS_COLOR = glm::vec3(1.0f, 0.0f, 0.0f);
    static constexpr const glm::vec3 VANISHING_COLOR = glm::vec3(0.0f, 1.0f, 0.0f);

    const glm::vec2 bot_left = glm::vec2(top_left.x, bot_right.y);
    const glm::vec2 top_right = glm::vec2(bot_right.x, top_left.y);
    m_dot_prog.setUniformVec3("dot_center", glm::vec3(screen_2_gl(window, top_left), 0));
    m_dot_prog.setUniformVec3("flat_color", REAR_DOTS_COLOR);
    m_dot_vao.draw_elements();
    m_dot_prog.setUniformVec3("dot_center", glm::vec3(screen_2_gl(window, top_right), 0));
    m_dot_prog.setUniformVec3("flat_color", REAR_DOTS_COLOR);
    m_dot_vao.draw_elements();
    m_dot_prog.setUniformVec3("dot_center", glm::vec3(screen_2_gl(window, bot_right), 0));
    m_dot_prog.setUniformVec3("flat_color", REAR_DOTS_COLOR);
    m_dot_vao.draw_elements();
    m_dot_prog.setUniformVec3("dot_center", glm::vec3(screen_2_gl(window, bot_left), 0));
    m_dot_prog.setUniformVec3("flat_color", REAR_DOTS_COLOR);
    m_dot_vao.draw_elements();

    m_dot_prog.setUniformVec3("dot_center", glm::vec3(screen_2_gl(window, vanishing), 0));
    m_dot_prog.setUniformVec3("flat_color", VANISHING_COLOR);
    m_dot_vao.draw_elements();
}

bool Mesh::should_switch_scenes() const
{
    return m_done;
}

void Mesh::recalculate_mesh()
{

}
} // namespace mash
} // namespace svm