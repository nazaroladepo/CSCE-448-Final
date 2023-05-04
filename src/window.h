#pragma once

#include <stdexcept>
#include <string>
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
        : std::runtime_error((prefix.size() ? prefix : "glfw error") + std::string(": #") + std::to_string(code)
            + ", " + glfw_info)
        , m_code(code)
    {}

    int code() const { return m_code; }
    bool ok() const { return m_code == 0; }

private:
    int m_code;
};

class Window
{
public:
    Window(int width, int height, const char* title);

    bool should_close();
    void notify_should_close();

    void swap_buffers();

    void get_window_size(int& width, int& height);
    void set_window_size(int width, int height);
    void enforce_aspect_ratio(int num, int denom);
    void screen_2_gl(double screen_x, double screen_y, float& gl_x, float& gl_y);

    bool key_is_pressed(int key);

    void set_cursor_enabled(bool enabled);
    bool is_cursor_pressed(bool left = true);
    void get_cursor_pos(double& xpos, double& ypos);

    template <class CbFunc>
    void set_keyboard_callback(CbFunc&& cb)
    {
        m_key_cb = cb;
    }

    template <class CbFunc>
    void set_cursor_pos_callback(CbFunc&& cb)
    {
        m_mouse_cb = cb;
    }

    template <class CbFunc>
    void set_resize_callback(CbFunc&& cb)
    {
        m_resize_cb = cb;
    }

    Window(const Window&) = delete;
    Window(Window&&) = delete;
    Window& operator=(const Window&) = delete;
    Window& operator=(Window&&) = delete;

    ~Window();

    static void poll_events();

private:
    static void key_callback_outer(GLFWwindow*, int, int, int, int);
    static void cursor_pos_callback(GLFWwindow*, double, double);
    static void framebuffer_size_callback(GLFWwindow*, int, int);

    GLFWwindow* m_handle;
    // https://www.glfw.org/docs/latest/input_guide.html
    std::function<void(int, int, int, int)> m_key_cb;
    std::function<void(double, double)> m_mouse_cb;
    std::function<void(int, int)> m_resize_cb;
};
}
}