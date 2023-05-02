#include <iostream>

#include "camera.h"
#include "shader.h"
#include "texture.h"
#include "vertex.h"
#include "window.h"

using namespace svm;
using namespace svm::camera;
using namespace svm::shader;
using namespace svm::texture;
using namespace svm::vertex;
using namespace svm::window;

static constexpr const int DEFAULT_WIDTH = 800;
static constexpr const int DEFAULT_HEIGHT = 600;
static constexpr const char* const DEFAULT_TITLE = "Single View Modeling";

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

vertex3_element vertices[4] = 
{
    // modelx,modely,modelz,  tex_u, tex_v
    { { -0.5f, -0.5f, -1.0f }, { 0.0, 0.0 } },
    { {  0.5f, -0.5f, -1.0f }, { 1.0, 0.0 } },
    { {  0.5f,  0.5f, -1.0f }, { 1.0, 1.0 } },
    { { -0.5f,  0.5f, -1.0f }, { 0.0, 1.0 } }
};

indexed_triangle triangles[2] = {
    // index of vertex data inside "verticies"
    // triangles should be specified in counter-clockwise order
    { 0, 1, 2 },
    { 2, 3, 0 }
};

const char* vshader_src =
    "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec2 aTexCoord;\n"
    "out vec2 TexCoord;\n"
    "uniform mat4 camera;\n"
    "void main()\n"
    "{\n"
    "    gl_Position = camera * vec4(aPos, 1.0);\n"
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
    glEnable(GL_DEPTH_TEST);

    VertexArrayBuffer vao(vertices, ARRAY_SIZE(vertices), triangles, ARRAY_SIZE(triangles));
    ShaderProgram program(vshader_src, fshader_src);
    Texture2D texture = Texture2D::from_file(argv[1]);
    program.setUniformInt("ourTexture", 0);

    Camera camera;
    camera.z = 3;
    camera.set_screen(DEFAULT_WIDTH, DEFAULT_HEIGHT);
    window.set_resize_callback([&camera](int width, int height)
    {
        camera.set_screen(width, height);
    });
    window.set_keyboard_callback([&camera](int key, int, int action, int)
    {
        constexpr float move_size = 0.05;
        if (action != GLFW_PRESS) return;
        switch (key)
        {
        case GLFW_KEY_W:
            camera.move_forward(move_size);
            break;
        case GLFW_KEY_S:
            camera.move_forward(-move_size);
            break;
        case GLFW_KEY_A:
            camera.strafe_left(move_size);
            break;
        case GLFW_KEY_D:
            camera.strafe_left(-move_size);
            break;
        default:
            break;
        };
    });

    std::atexit([](){ glfwTerminate(); });
    while (!window.should_close())
    {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        std::cout << camera.position().x << ' ' << camera.position().y << ' ' << camera.position().z << std::endl;
        std::cout << camera.direction().x << ' ' << camera.direction().y << ' ' << camera.direction().z << std::endl;
        std::cout << camera.aspect_ratio << std::endl;
        program.setUniformMat4("camera", camera.get_view_projection());

        texture.insert_to_unit_spot(GL_TEXTURE0);

        program.use();
        vao.draw_elements();

        window.swap_buffers();
        Window::poll_events();
    }

    return 0;
}
