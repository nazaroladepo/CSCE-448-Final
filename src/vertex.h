#pragma once

#include <cstddef>
#include <glad/glad.h>
#include <utility>

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

namespace svm
{
namespace vertex
{
struct vertex3_element
{
    GLfloat xyz[3];
    GLfloat texture_uv[2];
};

using indexed_triangle = GLuint[3];
using indexed_line = std::pair<GLuint, GLuint>;
static_assert(sizeof(indexed_line) == 2 * sizeof(GLuint),
    "Pair packing will not work in this configuration");

class VertexArrayBuffer
{
public:
    VertexArrayBuffer
    (
        const vertex3_element* verts,
        GLsizei num_verts,
        const indexed_triangle* triangles,
        GLsizei num_triangles
    );

    VertexArrayBuffer
    (
        const vertex3_element* verts,
        GLsizei num_verts,
        const indexed_line* lines,
        GLsizei num_lines
    );

    VertexArrayBuffer() = default;

    VertexArrayBuffer(const VertexArrayBuffer&) = delete;
    VertexArrayBuffer& operator=(const VertexArrayBuffer&) = delete;

    VertexArrayBuffer(VertexArrayBuffer&&);
    VertexArrayBuffer& operator=(VertexArrayBuffer&&);

    void draw_elements();

    ~VertexArrayBuffer();

private:
    void destroy();

    GLuint m_vao;
    GLuint m_vbo;
    GLuint m_ebo;
    GLint m_draw_mode;
    GLsizei m_num_elements;
};
} // namespace window
} // namespace svm