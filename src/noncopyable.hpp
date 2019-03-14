#pragma once


namespace ext {

class noncopyable
{
protected:
    noncopyable() {}
    ~noncopyable() {}

    noncopyable(const noncopyable &) = delete;
    noncopyable& operator= (const noncopyable &) = delete;
};

} // namespace ext

#define NONCOPYABLE private noncopyable
