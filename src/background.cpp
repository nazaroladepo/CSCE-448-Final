#include <utility>

#include "background.h"

namespace
{
constexpr float REAR_H = 20; // arbitrary

const char* vshader_src =
    "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec2 aTexCoord;\n"
    "out vec2 TexCoord;\n"
    "uniform mat4 camera;\n"
    "void main()\n"
    "{\n"
    "    gl_Position = camera * vec4(aPos, 1.0);\n"
    "    TexCoord = aTexCoord;\n"
    "}\n";

const char* fshader_src =
    "#version 330 core\n"
    "in vec2 TexCoord;\n"
    "out vec4 FragColor;\n"
    "uniform sampler2D ourTexture;\n"
    "void main()\n"
    "{\n"
    "    FragColor = texture(ourTexture, TexCoord);\n"
    "}\n";

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
    texture::Texture2D&& bg,
    const glm::vec2& top_left,
    const glm::vec2& bot_right,
    const glm::vec2& vanishing,
    float fovy
)
    : m_prog(vshader_src, fshader_src)
    , m_texture(std::forward<texture::Texture2D>(bg))
    , m_vao()
    , m_start_pos(0, 0, 3)
    , m_start_fov(fovy)
{


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

void Background::render(const glm::mat4& view_proj)
{
    m_prog.use();
    m_prog.setUniformMat4("camera", view_proj);
    m_texture.insert_to_unit_spot(GL_TEXTURE0);
    m_vao.draw_elements();
}

void Background::orient_camera_initially(camera::Camera& camera)
{
    camera.x = m_start_pos.x;
    camera.y = m_start_pos.y;
    camera.z = m_start_pos.z;
    camera.pitch = 0;
    camera.yaw = -90.0;
    camera.fovy = m_start_fov;
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
    const float tex_width = m_texture.width();
    const float tex_height = m_texture.height();
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
}
} // namespace background
} // namespace svm
