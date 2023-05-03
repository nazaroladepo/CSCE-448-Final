#pragma once

#include <glad/glad.h>

namespace svm
{
namespace texture
{
class Texture2D
{
public:
    Texture2D(const Texture2D&) = delete;
    Texture2D& operator=(const Texture2D&) = delete;

    Texture2D(Texture2D&&);
    Texture2D& operator=(Texture2D&&);

    void insert_to_unit_spot(GLenum texture_unit);

    ~Texture2D();

    static Texture2D from_memory(void* image_buf, size_t image_len);
    static Texture2D from_file(const char* image_path);

private:
    Texture2D(unsigned char* rgb_data, GLsizei width, GLsizei height, GLsizei num_channels);

    GLuint m_handle;
    GLsizei m_width;
    GLsizei m_height;
    GLsizei m_num_chan;
};
} // namespace texture
} // namespace svm
