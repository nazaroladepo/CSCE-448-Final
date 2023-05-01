#include "vertex.h"

namespace svm
{
namespace vertex
{
VertexArrayBuffer::VertexArrayBuffer
(
    const vertex3* verts,
    GLsizei num_verts,
    const indexed_triangle* triangles,
    GLsizei num_triangles
)
    : m_vao(0)
    , m_vbo(0)
    , m_ebo(0)
    , m_num_triangles(num_triangles)
{
    // Generate buffer and vertex array buffers
    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    glGenBuffers(1, &m_ebo);

    // 1. bind Vertex Array Object
    glBindVertexArray(m_vao);
    // 2. copy our vertices array in a vertex buffer for OpenGL to use
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex3) * num_verts, verts, GL_STATIC_DRAW);
    // 3. copy our index array in a element buffer for OpenGL to use
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexed_triangle) * num_triangles, triangles,
        GL_STATIC_DRAW);
    // 4. then set the vertex attributes pointers
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex3), NULL);
    glEnableVertexAttribArray(0);
}

void VertexArrayBuffer::draw()
{
    glBindVertexArray(m_vao);
    glDrawElements(GL_TRIANGLES, 3 * m_num_triangles, GL_UNSIGNED_INT, NULL);
    glBindVertexArray(0);
}

VertexArrayBuffer::~VertexArrayBuffer()
{
    glDeleteVertexArrays(1, &m_vao);
    glDeleteBuffers(1, &m_ebo);
    glDeleteBuffers(1, &m_vbo);
}
} // namespace window
} // namespace svm