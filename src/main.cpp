#include <iostream>

#include "shader.h"
#include "vertex.h"
#include "window.h"

using namespace svm;
using namespace svm::shader;
using namespace svm::window;
using namespace svm::vertex;

static constexpr const int DEFAULT_WIDTH = 800;
static constexpr const int DEFAULT_HEIGHT = 600;
static constexpr const char* const DEFAULT_TITLE = "Single View Modeling";

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

vertex3 vertices[3] = 
{
    { -0.5f, -0.5f, 0.0f },
    { 0.5f, -0.5f, 0.0f },
    { 0.0f,  0.5f, 0.0f }
};

indexed_triangle triangles[1] = {
    { 0, 1, 2 }
};

const char* vshader_src =
    "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\n";

const char* fshader_src =
    "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "}\n"; 

int main()
{
    Window window(DEFAULT_WIDTH, DEFAULT_HEIGHT, DEFAULT_TITLE);
    VertexArrayBuffer vao(vertices, ARRAY_SIZE(vertices), triangles, ARRAY_SIZE(triangles));
    ShaderProgram program(vshader_src, fshader_src);

    while (!window.should_close())
    {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        program.use();
        vao.draw();

        window.swap_buffers();
        Window::poll_events();
    }

    std::atexit([](){ glfwTerminate(); });
    return 0;
}
