#include <iostream>

#include "background.h"
#include "camera.h"
#include "shader.h"
#include "texture.h"
#include "vertex.h"
#include "window.h"

using namespace svm;
using namespace svm::background;
using namespace svm::camera;
using namespace svm::shader;
using namespace svm::texture;
using namespace svm::vertex;
using namespace svm::window;

static constexpr const int DEFAULT_WIDTH = 800;
static constexpr const int DEFAULT_HEIGHT = 600;
static constexpr const char* const DEFAULT_TITLE = "Single View Modeling";

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

void process_keyboard_input(Window& window, Camera& camera);

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

int main(int argc, const char* argv[])
{
    if (argc != 2)
    {
        std::cerr << "Invalid usage: single_view_modeling <IMAGE PATH>" << std::endl;
        return 1;
    }

    Window window(DEFAULT_WIDTH, DEFAULT_HEIGHT, DEFAULT_TITLE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Texture2D texture = Texture2D::from_file(argv[1]);
    Background scene(std::move(texture), glm::vec2(0.25, 0.75), glm::vec2(0.75, 0.25),
        glm::vec2(0.5, 0.5), 54);

    Camera camera;
    camera.set_screen(texture.width(), texture.height());
    scene.orient_camera_initially(camera);
    float last_cursor_x = 0.0;
    float last_cursor_y = 0.0;
    bool first_cursor_input = true;
    window.set_cursor_pos_callback(
        [&camera, &last_cursor_x, &last_cursor_y, &first_cursor_input](double x_pos, double y_pos)
    {
        constexpr float sensitivity = 0.1;

        if (first_cursor_input)
        {
            last_cursor_x = static_cast<float>(x_pos);
            last_cursor_y = static_cast<float>(y_pos);
            first_cursor_input = false;
        }

        const float offset_x = static_cast<float>(x_pos) - last_cursor_x;
        const float offset_y = static_cast<float>(y_pos) - last_cursor_y;

        camera.yaw_left(offset_x * sensitivity);
        camera.pitch_up(-offset_y * sensitivity);

        last_cursor_x = x_pos;
        last_cursor_y = y_pos;
    });
    window.set_resize_callback([&camera](int width, int height)
    {
        camera.set_screen(width, height);
    });

    std::atexit([](){ glfwTerminate(); });
    while (!window.should_close())
    {
        process_keyboard_input(window, camera);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        scene.render(camera.get_view_projection());

        window.swap_buffers();
        Window::poll_events();
    }

    return 0;
}

void process_keyboard_input(Window& window, Camera& camera)
{
    constexpr float move_size = 0.05;
    if (window.key_is_pressed(GLFW_KEY_W))
    {
        camera.move_forward(move_size);
    }
    if (window.key_is_pressed(GLFW_KEY_S))
    {
        camera.move_forward(-move_size);
    }
    if (window.key_is_pressed(GLFW_KEY_A))
    {
        camera.strafe_left(move_size);
    }
    if (window.key_is_pressed(GLFW_KEY_D))
    {
        camera.strafe_left(-move_size);
    }
}
