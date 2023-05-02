#include <mutex>
#include <string>

#include "window.h"

namespace
{
void glfw_set_errno(int code, const char* description)
{
    svm::window::glfw_errno = code;
    svm::window::glfw_errmsg = description;
}

svm::window::glfw_error make_current_error(const std::string& prefix = "")
{
    return svm::window::glfw_error(svm::window::glfw_errno, svm::window::glfw_errmsg, prefix);
}

void initialize_glfw_idempotent()
{
    static std::once_flag flag_init_glfw;;

    std::call_once(flag_init_glfw, []()
    {
        glfwSetErrorCallback(glfw_set_errno);

        if (GLFW_TRUE != glfwInit())
        {
            throw make_current_error("failed to initialize GLFW");
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    });
}
} // anonymous namespace

namespace svm
{
namespace window
{
thread_local int glfw_errno = 0;
thread_local std::string glfw_errmsg = "";

Window::Window(int width, int height, const char* title)
    : m_handle(nullptr)
    , m_key_cb()
    , m_mouse_cb()
    , m_resize_cb()
{
    initialize_glfw_idempotent();

    m_handle = glfwCreateWindow(width, height, title, NULL, NULL);
    if (m_handle == NULL)
    {
        throw make_current_error("could not create window");
    }
    glfwMakeContextCurrent(m_handle);

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
    {
        throw std::runtime_error("Failed to initialize GLAD");
    }

    glViewport(0, 0, width, height);

    glfwSetFramebufferSizeCallback(m_handle, framebuffer_size_callback);
    glfwSetKeyCallback(m_handle, key_callback_outer);
    glfwSetWindowUserPointer(m_handle, this);
}

Window::~Window()
{
    glfwDestroyWindow(m_handle);
}

bool Window::should_close()
{
    return glfwWindowShouldClose(m_handle);
}

void Window::notify_should_close()
{
    glfwSetWindowShouldClose(m_handle, true);
}

void Window::swap_buffers()
{
    glfwSwapBuffers(m_handle);
}

void Window::get_window_size(int& width, int& height)
{
    glfwGetWindowSize(m_handle, &width, &height);
}

void Window::poll_events()
{
    glfwPollEvents();
}

void Window::key_callback_outer(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    Window* window_outer = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        window_outer->notify_should_close();
    }
    else if (window_outer->m_key_cb)
    {
        window_outer->m_key_cb(key, scancode, action, mods);
    }
}

void Window::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);

    Window* window_outer = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if (window_outer->m_resize_cb)
    {
        window_outer->m_resize_cb(width, height);
    }
}
} // namespace window
} // namespace svm
