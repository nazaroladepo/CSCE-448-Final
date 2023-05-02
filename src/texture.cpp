#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include "texture.h"

namespace svm
{
namespace texture
{
void Texture2D::insert_to_unit_spot(GLenum)
{
}

Texture2D::~Texture2D()
{
    //glDeleteTextures(1, &m_handle);
}

Texture2D Texture2D::from_memory(void*, size_t)
{
    return {nullptr, 0, 0, 0};
}

Texture2D Texture2D::from_file(char*)
{
    return {nullptr, 0, 0, 0};
}

Texture2D::Texture2D(void*, GLsizei width, GLsizei height, GLsizei num_channels)
    : m_handle()
    , m_width(width)
    , m_height(height)
    , m_num_chan(num_channels)
{}
} // namespace texture
} // namespace svm
