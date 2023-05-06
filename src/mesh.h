#pragma once

#include <glm/vec2.hpp>
#include <memory>

#include "scene.h"
#include "shader.h"
#include "texture.h"
#include "vertex.h"

namespace svm
{
namespace mesh
{
class Mesh: public scene::Scene
{
public:
    Mesh(const std::shared_ptr<texture::Texture2D>& tex);

    glm::vec2 top_left;
    glm::vec2 bot_right;
    glm::vec2 vanishing;

    void setup(const window_ptr_t& window) override;
    void process_input(const window_ptr_t& window, float frame_time) override;
    void render(const window_ptr_t& window, float frame_time) override;

    bool should_switch_scenes() const;

private:
    void recalculate_mesh(const window_ptr_t& window);

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
