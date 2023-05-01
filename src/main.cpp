#include <iostream>

#include "window.h"

using namespace svm;
using namespace svm::window;

static constexpr const int DEFAULT_WIDTH = 800;
static constexpr const int DEFAULT_HEIGHT = 600;
static constexpr const char* const DEFAULT_TITLE = "Single View Modeling";

int main()
{
    Window window(DEFAULT_WIDTH, DEFAULT_HEIGHT, DEFAULT_TITLE);

    while (!window.should_close())
    {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        window.swap_buffers();
        Window::poll_events();
    }

    glfwTerminate();
    return 0;
}
