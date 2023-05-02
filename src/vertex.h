#pragma once

#include <cstddef>
#include <glad/glad.h>

namespace svm
{
namespace vertex
{
using vertex3 = GLfloat[3];
using indexed_triangle = GLuint[3];

class VertexArrayBuffer
{
public:
    VertexArrayBuffer
    (
        const vertex3* verts,
        GLsizei num_verts,
        const indexed_triangle* triangles,
        GLsizei num_triangles
    );

    VertexArrayBuffer(const VertexArrayBuffer&) = delete;
    VertexArrayBuffer(VertexArrayBuffer&&) = delete;
    VertexArrayBuffer& operator=(const VertexArrayBuffer&) = delete;
    VertexArrayBuffer& operator=(VertexArrayBuffer&&) = delete;

    void draw_elements();

    ~VertexArrayBuffer();
private:
    GLuint m_vao;
    GLuint m_vbo;
    GLuint m_ebo;
    GLsizei m_num_triangles;
};
} // namespace window
} // namespace svm