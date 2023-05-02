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
void Texture2D::insert_to_unit_spot(GLenum spot)
{
    glActiveTexture(spot);
    glBindTexture(GL_TEXTURE_2D, m_handle);
}

Texture2D::~Texture2D()
{
    glDeleteTextures(1, &m_handle);
}

Texture2D Texture2D::from_memory(void*, size_t)
{
    throw std::logic_error("not implemented");
}

Texture2D Texture2D::from_file(const char* image_path)
{
    int width, height, num_channels;
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
    return Texture2D(rgb_data, width, height, num_channels);
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
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    const GLenum pix_type = (num_channels == 4) ? GL_RGBA : GL_RGB;
    glTexImage2D(GL_TEXTURE_2D, 0, pix_type, width, height, 0, pix_type, GL_UNSIGNED_BYTE, rgb_data);
    glGenerateMipmap(GL_TEXTURE_2D);
}
} // namespace texture
} // namespace svm
