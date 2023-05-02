#include <cstddef>
#include <stdexcept>
#include <string>

#include "scope_guard.h"
#include "shader.h"

namespace svm
{
namespace shader
{
ShaderProgram::ShaderProgram(const char* vert_shader_src, const char* frag_shader_src): m_handle(0)
{
    const GLuint vert_shader = glCreateShader(GL_VERTEX_SHADER);
    const GLuint frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
    tools::ScopeGuard shader_free([vert_shader, frag_shader]()
    {
        glDeleteShader(vert_shader);
        glDeleteShader(frag_shader);
    });

    int success;
    char info_log[512];

    // Compile vertex shader and check for compilation errors
    glShaderSource(vert_shader, 1, &vert_shader_src, NULL);
    glCompileShader(vert_shader);
    glGetShaderiv(vert_shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vert_shader, sizeof(info_log), NULL, info_log);
        throw std::runtime_error(std::string("vertex shader compile failed:\n") + std::string(info_log));
    }

    // Compile fragment shader and check for compilation errors
    glShaderSource(frag_shader, 1, &frag_shader_src, NULL);
    glCompileShader(frag_shader);
    glGetShaderiv(frag_shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(frag_shader, sizeof(info_log), NULL, info_log);
        throw std::runtime_error(std::string("vertex shader compile failed:\n") + std::string(info_log));
    }

    // Create and link shader program, checking for linking errors
    m_handle = glCreateProgram();
    tools::ScopeGuard program_free([this]()
    {
        glDeleteProgram(m_handle);
    });
    glAttachShader(m_handle, vert_shader);
    glAttachShader(m_handle, frag_shader);
    glLinkProgram(m_handle);
    glGetProgramiv(m_handle, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(m_handle, sizeof(info_log), NULL, info_log);
        throw std::runtime_error(std::string("failed to link shader program:\n") + std::string(info_log));
    }

    program_free.release();
}

void ShaderProgram::use()
{
    glUseProgram(m_handle);
}

void ShaderProgram::setUniformInt(const char* uniform_name, GLint value)
{
    use();
    glUniform1i(glGetUniformLocation(m_handle, uniform_name), value);
}

void ShaderProgram::setUniformMat4(const char* uniform_name, const glm::mat4& value)
{
    use();
    glUniformMatrix4fv(glGetUniformLocation(m_handle, uniform_name), 1, GL_FALSE, &value[0][0]);
}

ShaderProgram::~ShaderProgram()
{
    glDeleteProgram(m_handle);
}
} // namespace shader
} // namespace svm
