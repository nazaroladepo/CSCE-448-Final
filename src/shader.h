#pragma once

#include <glad/glad.h>
#include <glm/mat4x4.hpp>

namespace svm
{
namespace shader
{
class ShaderProgram
{
public:
    ShaderProgram(const char* vert_shader_src, const char* frag_shader_src);

    void use();

    void setUniformInt(const char* uniform_name, GLint value);
    void setUniformFloat(const char* uniform_name, GLfloat value);
    void setUniformVec3(const char* uniform_name, const glm::vec3& value);
    void setUniformVec4(const char* uniform_name, const glm::vec4& value);
    void setUniformMat4(const char* uniform_name, const glm::mat4& value);

    ~ShaderProgram();

    static ShaderProgram textured_object();
    static ShaderProgram flat_color(const glm::vec4& rgba);
    static ShaderProgram gui_dot(const glm::vec3& rgb, float radius);
private:
    GLuint m_handle;
};
} // namespace shader
} // namespace svm
