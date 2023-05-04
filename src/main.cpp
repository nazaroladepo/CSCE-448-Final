#include <iostream>

#include "background.h"
#include "camera.h"
#include "mesh.h"
#include "shader.h"
#include "texture.h"
#include "vertex.h"
#include "window.h"

using namespace svm;
using namespace svm::background;
using namespace svm::camera;
using namespace svm::mesh;
using namespace svm::scene;
using namespace svm::shader;
using namespace svm::texture;
using namespace svm::vertex;
using namespace svm::window;

static constexpr const int DEFAULT_WIDTH = 800;
static constexpr const int DEFAULT_HEIGHT = 600;
static constexpr const char* const DEFAULT_TITLE = "Single View Modeling";

glm::vec2 gl_coords_to_tex_coords(const glm::vec2& v)
{
    return (v + glm::vec2(1.0f, 1.0f)) / 2.0f;
}

int main(int argc, const char* argv[])
{
    if (argc != 2)
    {
        std::cerr << "Invalid usage: single_view_modeling <IMAGE PATH>" << std::endl;
        return 1;
    }

    std::shared_ptr<Window> window(new Window(DEFAULT_WIDTH, DEFAULT_HEIGHT, DEFAULT_TITLE));
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    std::shared_ptr<Texture2D> texture(Texture2D::from_file(argv[1]));
    Mesh mesh(texture);
    Background bg(texture);/*, glm::vec2(0.25, 0.75), glm::vec2(0.75, 0.25),
        glm::vec2(0.5, 0.5), 54);*/
    //bg.setup(window);

    Scene* scene = &mesh;
    scene->setup(window);

    std::atexit([](){ glfwTerminate(); });
    while (!window->should_close())
    {
        scene->process_input(window, 1);
        if (scene == &mesh && mesh.should_switch_scenes())
        {
            scene = &bg;
            bg.set_user_params
            (
                gl_coords_to_tex_coords(mesh.top_left),
                gl_coords_to_tex_coords(mesh.bot_right),
                gl_coords_to_tex_coords(mesh.vanishing),
                54.0f
            );
            scene->setup(window);
            continue;
        }

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        scene->render(window, 1);

        window->swap_buffers();
        Window::poll_events();
    }

    return 0;
}

