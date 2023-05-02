#include <iostream>

#include "shader.h"
#include "texture.h"
#include "vertex.h"
#include "window.h"

using namespace svm;
using namespace svm::shader;
using namespace svm::texture;
using namespace svm::window;
using namespace svm::vertex;

static constexpr const int DEFAULT_WIDTH = 800;
static constexpr const int DEFAULT_HEIGHT = 600;
static constexpr const char* const DEFAULT_TITLE = "Single View Modeling";

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

vertex3_element vertices[4] = 
{
    { { -0.5f, -0.5f, 0.0f }, { 0.0, 0.0 } },
    { {  0.5f, -0.5f, 0.0f }, { 1.0, 0.0 } },
    { {  0.5f,  0.5f, 0.0f }, { 1.0, 1.0 } },
    { { -0.5f,  0.5f, 0.0f }, { 0.0, 1.0 } }
};

indexed_triangle triangles[2] = {
    { 0, 1, 2 },
    { 2, 3, 0 }
};

const char* vshader_src =
    "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec2 aTexCoord;\n"
    "out vec2 TexCoord;\n"
    "void main()\n"
    "{\n"
    "    gl_Position = vec4(aPos, 1.0);\n"
    "    TexCoord = aTexCoord;\n"
    "};\n";

const char* fshader_src =
    "#version 330 core\n"
    "in vec2 TexCoord;\n"
    "out vec4 FragColor;\n"
    "uniform sampler2D ourTexture;\n"
    "void main()\n"
    "{\n"
    "    FragColor = texture(ourTexture, TexCoord);\n"
    "}\n"; 

int main(int argc, const char* argv[])
{
    if (argc != 2)
    {
        std::cerr << "Invalid usage: single_view_modeling <IMAGE PATH>" << std::endl;
        return 1;
    }

    Window window(DEFAULT_WIDTH, DEFAULT_HEIGHT, DEFAULT_TITLE);
    VertexArrayBuffer vao(vertices, ARRAY_SIZE(vertices), triangles, ARRAY_SIZE(triangles));
    ShaderProgram program(vshader_src, fshader_src);
    Texture2D texture = Texture2D::from_file(argv[1]);
    program.setUniformInt("ourTexture", 0);

    while (!window.should_close())
    {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        texture.insert_to_unit_spot(GL_TEXTURE0);

        program.use();
        vao.draw_elements();

        window.swap_buffers();
        Window::poll_events();
    }

    std::atexit([](){ glfwTerminate(); });
    return 0;
}
