#pragma once

#include <cstddef>
#include <glad/glad.h>

namespace svm
{
namespace vertex
{
using vertex = float[3];
using indexed_triangle = unsigned int[3];

class VertexArrayBuffer
{
public:
    VertexArrayBuffer
    (
        const vertex* verts,
        size_t num_verts,
        const indexed_triangle* triangles,
        size_t num_triangles
    );

    VertexArrayBuffer(const VertexArrayBuffer&) = delete;
    VertexArrayBuffer(VertexArrayBuffer&&) = delete;
    VertexArrayBuffer& operator=(const VertexArrayBuffer&) = delete;
    VertexArrayBuffer& operator=(VertexArrayBuffer&&) = delete;

    void bind();
    static void unbind();

    ~VertexArrayBuffer();
private:
    GLuint m_vao;
    GLuint m_vbo;
    GLuint m_ebo;
};
} // namespace window
} // namespace svm