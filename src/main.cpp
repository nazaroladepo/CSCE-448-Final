#include <iostream>

#include "window.h"

using namespace svm;
using namespace svm::window;

int main()
{
    Window& window = Window::instance;

    while (!window.should_close())
    {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        window.swap_buffers();
        Window::poll_events();
    }

    return 0;
}
