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
    void setUniformMat4(const char* uniform_name, const glm::mat4& value);

    ~ShaderProgram();
private:
    GLuint m_handle;
};
} // namespace shader
} // namespace svm
