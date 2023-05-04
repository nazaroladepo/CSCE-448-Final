#pragma once

#include <memory>

#include "window.h"

namespace svm
{
namespace scene
{
struct Scene
{
    using window_ptr_t = std::shared_ptr<window::Window>;

    virtual void setup(const window_ptr_t& window) = 0;
    virtual void process_input(const window_ptr_t& window, float frame_time) = 0;
    virtual void render(const window_ptr_t& window, float frame_time) = 0;

    virtual ~Scene() {};
};
} // namespace scene
} // namespace svm