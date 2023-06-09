#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#include <stdexcept>
#include <string>

#include "scope_guard.h"
#include "texture.h"

namespace svm
{
namespace texture
{
Texture2D::Texture2D(Texture2D&& other)
    : m_handle(other.m_handle)
    , m_width(other.m_width)
    , m_height(other.m_height)
    , m_num_chan(other.m_num_chan)
{
    other.m_handle = 0;
}

Texture2D& Texture2D::operator=(Texture2D&& other)
{
    if (m_handle != 0)
    {
        glDeleteTextures(1, &m_handle);
    }

    m_handle = other.m_handle;
    m_width = other.m_width;
    m_height = other.m_height;
    m_num_chan = other.m_num_chan;
    other.m_handle = 0;
    return *this;
}

int Texture2D::width() const
{
    return static_cast<int>(m_width);
}

int Texture2D::height() const
{
    return static_cast<int>(m_height);
}

void Texture2D::insert_to_unit_spot(GLenum spot)
{
    glActiveTexture(spot);
    glBindTexture(GL_TEXTURE_2D, m_handle);
}

Texture2D::~Texture2D()
{
    if (m_handle != 0)
    {
        glDeleteTextures(1, &m_handle);
    }
}

Texture2D* Texture2D::from_memory(void*, size_t)
{
    throw std::logic_error("not implemented");
}

Texture2D* Texture2D::from_file(const char* image_path)
{
    int width, height, num_channels;
    stbi_set_flip_vertically_on_load(true); //flip loaded texture's on the y-axis.
    unsigned char *rgb_data = stbi_load(image_path, &width, &height, &num_channels, 0);
    if (!rgb_data)
    {
        throw std::runtime_error(std::string("could not open file: ") + image_path);
    }
    else if (num_channels < 3 || num_channels > 4)
    {
        stbi_image_free(rgb_data);   
        throw std::runtime_error("unexpected number of channels: " + std::to_string(num_channels));
    }
    return new Texture2D(rgb_data, width, height, num_channels);
}

Texture2D::Texture2D(unsigned char* rgb_data, GLsizei width, GLsizei height, GLsizei num_channels)
    : m_handle()
    , m_width(width)
    , m_height(height)
    , m_num_chan(num_channels)
{
    tools::ScopeGuard rgb_free([rgb_data](){
        stbi_image_free(rgb_data);
    });

    glGenTextures(1, &m_handle);
    glBindTexture(GL_TEXTURE_2D, m_handle);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);  // set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    const GLenum pix_type = (num_channels == 4) ? GL_RGBA : GL_RGB;
    glTexImage2D(GL_TEXTURE_2D, 0, pix_type, width, height, 0, pix_type, GL_UNSIGNED_BYTE, rgb_data);
    glGenerateMipmap(GL_TEXTURE_2D);
}
} // namespace texture
} // namespace svm
