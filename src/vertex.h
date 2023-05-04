#pragma once

#include <cstddef>
#include <glad/glad.h>

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
    GLsizei m_num_triangles;
};
} // namespace window
} // namespace svm