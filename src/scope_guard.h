#pragma once

#include <functional>

namespace svm
{
namespace tools
{
class ScopeGuard
{
public:
    template <class CleanupFunc>
    ScopeGuard(CleanupFunc&& cleanup): m_cleanup(cleanup)
    {}

    // copy construction/assignment is deleted
    ScopeGuard(const ScopeGuard&) = delete;
    ScopeGuard& operator=(const ScopeGuard&) = delete;

    // move construction/assignment will put moved ScopeGuard in a state as if release() was called
    ScopeGuard(ScopeGuard&&);
    ScopeGuard& operator=(ScopeGuard&&);

    void cleanup();
    void release();
    ~ScopeGuard();

private:
    std::function<void()> m_cleanup;
};
} // namespace tools
} // namespace svm
