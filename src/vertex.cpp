#include "vertex.h"

namespace svm
{
namespace vertex
{
VertexArrayBuffer::VertexArrayBuffer
(
    const vertex3_element* verts,
    GLsizei num_verts,
    const indexed_triangle* triangles,
    GLsizei num_triangles
)
    : m_vao(0)
    , m_vbo(0)
    , m_ebo(0)
    , m_draw_mode(GL_TRIANGLES)
    , m_num_elements(3 * num_triangles)
{
    // Generate buffer and vertex array buffers
    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    glGenBuffers(1, &m_ebo);

    // 1. bind Vertex Array Object
    glBindVertexArray(m_vao);
    // 2. copy our vertices array in a vertex buffer for OpenGL to use
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex3_element) * num_verts, verts, GL_STATIC_DRAW);
    // 3. copy our index array in a element buffer for OpenGL to use
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexed_triangle) * num_triangles, triangles,
        GL_STATIC_DRAW);
    // 4. then set the vertex attributes pointers for position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex3_element),
        reinterpret_cast<void*>(offsetof(vertex3_element, xyz)));
    glEnableVertexAttribArray(0);
    // 5. and for texture coords
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(vertex3_element),
        reinterpret_cast<void*>(offsetof(vertex3_element, texture_uv)));
    glEnableVertexAttribArray(1);
}

VertexArrayBuffer::VertexArrayBuffer
(
    const vertex3_element* verts,
    GLsizei num_verts,
    const indexed_line* lines,
    GLsizei num_lines
)
    : m_vao(0)
    , m_vbo(0)
    , m_ebo(0)
    , m_draw_mode(GL_LINES)
    , m_num_elements(2 * num_lines)
{
    // Generate buffer and vertex array buffers
    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    glGenBuffers(1, &m_ebo);

    // 1. bind Vertex Array Object
    glBindVertexArray(m_vao);
    // 2. copy our vertices array in a vertex buffer for OpenGL to use
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex3_element) * num_verts, verts, GL_STATIC_DRAW);
    // 3. copy our index array in a element buffer for OpenGL to use
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexed_line) * num_lines, lines,
        GL_STATIC_DRAW);
    // 4. then set the vertex attributes pointers for position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex3_element),
        reinterpret_cast<void*>(offsetof(vertex3_element, xyz)));
    glEnableVertexAttribArray(0);
    // 5. and for texture coords
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(vertex3_element),
        reinterpret_cast<void*>(offsetof(vertex3_element, texture_uv)));
    glEnableVertexAttribArray(1);
}

VertexArrayBuffer::VertexArrayBuffer(VertexArrayBuffer&& other)
{
    m_vao = other.m_vao;
    m_vbo = other.m_vbo;
    m_ebo = other.m_ebo;
    m_draw_mode = other.m_draw_mode;
    m_num_elements = other.m_num_elements;
    other.m_vao = 0;
}

VertexArrayBuffer& VertexArrayBuffer::operator=(VertexArrayBuffer&& other)
{
    destroy();
    m_vao = other.m_vao;
    m_vbo = other.m_vbo;
    m_ebo = other.m_vbo;
    m_draw_mode = other.m_draw_mode;
    m_num_elements = other.m_num_elements;
    other.m_vao = 0;
    return *this;
}

void VertexArrayBuffer::draw_elements()
{
    glBindVertexArray(m_vao);
    glDrawElements(m_draw_mode, m_num_elements, GL_UNSIGNED_INT, NULL);
    glBindVertexArray(0);
}

void VertexArrayBuffer::destroy()
{
    if (m_vao != 0)
    {
        glDeleteVertexArrays(1, &m_vao);
        glDeleteBuffers(1, &m_ebo);
        glDeleteBuffers(1, &m_vbo);
        m_vao = 0;
    }
}

VertexArrayBuffer::~VertexArrayBuffer()
{
    destroy();
}
} // namespace window
} // namespace svm