#include <cstddef>
#include <stdexcept>
#include <string>

#include "scope_guard.h"
#include "shader.h"

namespace
{
const char* textured_obj_vshader_src =
    "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec2 aTexCoord;\n"
    "out vec2 TexCoord;\n"
    "uniform mat4 camera;\n"
    "void main()\n"
    "{\n"
    "    gl_Position = camera * vec4(aPos, 1.0);\n"
    "    TexCoord = aTexCoord;\n"
    "}\n";

const char* textured_obj_fshader_src =
    "#version 330 core\n"
    "in vec2 TexCoord;\n"
    "out vec4 FragColor;\n"
    "uniform sampler2D tex;\n"
    "void main()\n"
    "{\n"
    "    FragColor = texture(tex, TexCoord);\n"
    "}\n";

const char* flat_color_vshader_src =
    "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "    gl_Position = vec4(aPos, 1.0);\n"
    "}\n";

const char* flat_color_fshader_src =
    "#version 330 core\n"
    "out vec4 FragColor;\n"
    "uniform vec4 flat_color;\n"
    "void main()\n"
    "{\n"
    "    FragColor = flat_color;\n"
    "}\n";

const char* dot_vshader_src =
    "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "out vec3 dot_pos_rel;\n"
    "uniform vec3 dot_center;\n"
    "void main()\n"
    "{\n"
    "    dot_pos_rel = aPos;\n"
    "    gl_Position = vec4(aPos + dot_center, 1.0);\n"
    "}\n";

const char* dot_fshader_src =
    "#version 330 core\n"
    "in vec3 dot_pos_rel;\n"
    "out vec4 FragColor;\n"
    "uniform vec3 flat_color;\n"
    "uniform float dot_rad;\n"
    "void main()\n"
    "{\n"
    "    float dist = sqrt(dot(dot_pos_rel,dot_pos_rel));\n"
    "    if (dist >= dot_rad) {\n"
    "        discard;\n"
    "    }\n"
    "    float sm = smoothstep(0, dot_rad, dist);\n"
    "    sm = sm * sm;\n"
    "    gl_FragColor = vec4(flat_color, 1 - sm);\n"
    "}\n";
} // anonymous namespace

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

void ShaderProgram::setUniformFloat(const char* uniform_name, GLfloat value)
{
    glUniform1f(glGetUniformLocation(m_handle, uniform_name), value);
}

void ShaderProgram::setUniformVec3(const char* uniform_name, const glm::vec3& value)
{
    use();
    glUniform3fv(glGetUniformLocation(m_handle, uniform_name), 1, &value[0]);
}

void ShaderProgram::setUniformVec4(const char* uniform_name, const glm::vec4& value)
{
    use();
    glUniform4fv(glGetUniformLocation(m_handle, uniform_name), 1, &value[0]);
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

ShaderProgram ShaderProgram::textured_object()
{
    ShaderProgram prog(textured_obj_vshader_src, textured_obj_fshader_src);
    prog.setUniformMat4("camera", glm::mat4(1.0f));
    prog.setUniformInt("tex", 0);
    return prog;
}

ShaderProgram ShaderProgram::flat_color(const glm::vec4& rgba)
{
    ShaderProgram prog(flat_color_vshader_src, flat_color_fshader_src);
    prog.setUniformVec4("flat_color", rgba);
    return prog;
}

ShaderProgram ShaderProgram::gui_dot(const glm::vec3& rgb, float radius)
{
    ShaderProgram prog(dot_vshader_src, dot_fshader_src);
    prog.setUniformVec3("flat_color", rgb);
    prog.setUniformFloat("dot_rad", radius);
    return prog;
}
} // namespace shader
} // namespace svm
