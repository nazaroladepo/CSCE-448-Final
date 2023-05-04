#pragma once

#include <glm/vec2.hpp>
#include <memory>

#include "shader.h"
#include "texture.h"
#include "vertex.h"
//#include "window.h"

namespace svm
{
namespace window
{
class Window;
}
namespace mesh
{
class Mesh
{
public:
    Mesh(const std::shared_ptr<texture::Texture2D>& tex);

    glm::vec2 top_left;
    glm::vec2 bot_right;
    glm::vec2 vanishing;

    void setup(window::Window& window);

    void process_input(window::Window& window);

    void render(window::Window& window);

    bool should_switch_scenes() const;

private:
    void recalculate_mesh();

    shader::ShaderProgram m_tex_prog;
    shader::ShaderProgram m_mesh_prog;
    shader::ShaderProgram m_dot_prog;
    std::shared_ptr<texture::Texture2D> m_tex; 
    vertex::VertexArrayBuffer m_tex_vao;
    vertex::VertexArrayBuffer m_mesh_vao;
    vertex::VertexArrayBuffer m_dot_vao;
    int m_dragging_edge;
    bool m_done;
};
} // namespace mash
} // namespace svm
