#pragma once

#include <functional>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace svm 
{
namespace window
{
extern thread_local int glfw_errno;
extern thread_local std::string glfw_errmsg;

class glfw_error: public std::runtime_error
{
public:
    glfw_error(int code, const std::string& glfw_info, const std::string& prefix = "")
        : std::runtime_error((prefix.size() ? prefix : "glfw error") + ": #" + std::to_string(code)
            + ", " + glfw_info)
        , m_code(code)
    {}

    glfw_error() = default;

    int code() const { return m_code; }
    bool ok() const { return m_code == 0; }

private:
    int m_code;
};

class Window
{
public:
    bool should_close();
    void notify_should_close();
    void swap_buffers();

    Window(const Window&) = delete;
    Window(Window&&) = delete;
    Window& operator=(const Window&) = delete;
    Window& operator=(Window&&) = delete;

    Window(int width, int height, const char* title);

    ~Window();

    static void poll_events();

private:
    static void key_callback_outer(GLFWwindow*, int, int, int, int);

    GLFWwindow* m_handle;
    // https://www.glfw.org/docs/latest/input_guide.html
    std::function<void(int, int, int, int)> m_key_cb;
    std::function<void(double, double)> m_mouse_cb;
};
}
}