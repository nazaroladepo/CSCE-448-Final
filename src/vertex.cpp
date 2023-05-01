#include "vertex.h"

namespace svm
{
namespace vertex
{
VertexArrayBuffer::VertexArrayBuffer
(
    const vertex* verts,
    size_t num_verts,
    const indexed_triangle* triangles,
    size_t num_triangles
)
    : m_vao(0)
    , m_vbo(0)
    , m_ebo(0)
{
    // Generate buffer and vertex array buffers
    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    glGenBuffers(1, &m_ebo);

    // 1. bind Vertex Array Object
    glBindVertexArray(m_vao);
    // 2. copy our vertices array in a vertex buffer for OpenGL to use
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex) * num_verts, verts, GL_STATIC_DRAW);
    // 3. copy our index array in a element buffer for OpenGL to use
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexed_triangle) * num_triangles, triangles,
        GL_STATIC_DRAW);
    // 4. then set the vertex attributes pointers
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), NULL);
    glEnableVertexAttribArray(0);
}

void VertexArrayBuffer::bind()
{
    glBindVertexArray(m_vao);
}

void VertexArrayBuffer::unbind()
{
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