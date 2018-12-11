#pragma once

class NonCopyable
{
protected:
    NonCopyable() {}
    ~NonCopyable() {}

private:
    NonCopyable(NonCopyable&&) = delete;
    NonCopyable& operator=(NonCopyable&&) = delete;

    NonCopyable(NonCopyable const&) = delete;
    NonCopyable const& operator=(NonCopyable const&) = delete;
};