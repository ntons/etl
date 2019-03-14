// 可追踪对象，用于判断对象是否被析构

#pragma once

#include <memory>

namespace ext {

class trackable
{
private:
    std::shared_ptr<void> tracked_ptr_;

protected:
    trackable(): tracked_ptr_(static_cast<int*>(0)) { }
    trackable(const trackable &): tracked_ptr_(static_cast<int*>(0)) { }
    inline trackable& operator=(const trackable &) { return *this; }
    ~trackable() { }

public:
    inline const std::shared_ptr<void>& get_tracked_ptr() const
    {
        return tracked_ptr_;
    }
};

class tracker : public trackable {};

}
