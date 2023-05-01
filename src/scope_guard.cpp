#include "scope_guard.h"

namespace svm
{
namespace tools
{
ScopeGuard::ScopeGuard(ScopeGuard&& other): m_cleanup(std::move(other.m_cleanup))
{
    other.release();
}

ScopeGuard& ScopeGuard::operator=(ScopeGuard&& other)
{
    cleanup();
    m_cleanup = std::move(other.m_cleanup);
    other.release();
    return *this;
}

void ScopeGuard::cleanup()
{
    if (m_cleanup)
    {
        m_cleanup();
        release();
    }
}

void ScopeGuard::release()
{
    m_cleanup = nullptr;
}

ScopeGuard::~ScopeGuard()
{
    cleanup();
}
} // namespace tools
} // namespace svm
