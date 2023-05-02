#pragma once

#include <glad/glad.h>

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

    ~ShaderProgram();
private:
    GLuint m_handle;
};
} // namespace shader
} // namespace svm
