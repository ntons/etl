#pragma once

#include "noncopyable.hpp"
#include "trackable.hpp"

#include <functional>
#include <map>
#include <list>

namespace ext {

// never break
struct delegate_never_break
{
    template<typename R>
    inline bool operator()(const R&) const { return false; }
};
// break if returned value is true
struct delegate_true_break
{
    template<typename R>
    inline bool operator()(const R &r) const { return r; }
};
// break if returned value is false
struct delegate_false_break
{
    template<typename R>
    inline bool operator()(const R &r) const { return !r; }
};

template<typename R, typename... Args>
class delegate_base
{
private:
    typedef std::function<R(Args...)> Func;

    class callable
    {
    public:
        callable(const Func &f, const trackable *t) : f_(f)
        {
            if (t)
            {
                tracked_ = true;
                tracker_ = t->get_tracked_ptr();
            }
        }

        operator bool() const
        {
            return f_ && (!tracked_ || !tracker_.expired());
        }

        R operator()(Args... args) const
        {
            return f_(args...);
        }

    private:
        Func f_;
        bool tracked_ = false;
        std::weak_ptr<void> tracker_;
    };

    class slot
    {
    public:
        slot(std::list<callable> &list) : list_(list) {}

        void connect(Func f, const trackable *t = nullptr)
        {
            if (f) list_.push_back(callable(f, t));
        }
        void operator+= (Func f)
        {
            connect(f, nullptr);
        }
    private:
        std::list<callable> &list_;
    };

protected:
    delegate_base() { }

public:
    void connect(Func f, const trackable *t)
    {
        priority(0).connect(f, t);
    }
    void operator+= (Func f)
    {
        priority(0).connect(f, nullptr);
    }

    slot priority(int n)
    {
        return slot(map_[n]);
    }
    slot operator[] (int n)
    {
        return slot(map_[n]);
    }

    inline bool empty() const
    {
        for (auto iter = map_.begin(); iter != map_.end(); ++iter)
        {
            auto const &list = iter->second;
            for (auto iter = list.begin(); iter != list.end(); ++iter)
            {
                if (*iter)
                {
                    return false;
                }
            }
        }
        return true;
    }
    inline operator bool () const
    {
        return !empty();
    }

    inline void clear()
    {
        map_.clear();
    }

protected:
    std::map<int, std::list<callable> > map_;
};

template<typename Signature, typename Break = delegate_never_break>
class delegate
{
};

template<typename Break, typename R, typename... Args>
class delegate<R(Args...), Break> : public delegate_base<R, Args...>
{
    static Break break_if;

public:
    R operator()(Args... args)
    {
        R r = R();
        for (auto _iter = this->map_.begin(); _iter != this->map_.end(); )
        {
            auto iter = _iter++;
            auto &list = iter->second;
            for (auto _iter = list.begin(); _iter != list.end(); )
            {
                auto iter = _iter++;
                if (!(*iter))
                {
                    list.erase(iter);
                    continue;
                }
                if (break_if((r = (*iter)(args...))))
                {
                    return r;
                }
            }
            if (list.empty())
            {
                this->map_.erase(iter);
            }
        }
        return r;
    }
};

template<typename Break, typename... Args>
class delegate<void(Args...), Break> : public delegate_base<void, Args...>
{
    static Break break_if;

public:
    void operator()(Args... args)
    {
        for (auto _iter = this->map_.begin(); _iter != this->map_.end(); )
        {
            auto iter = _iter++;
            auto &list = iter->second;
            for(auto _iter = list.begin(); _iter != list.end(); )
            {
                auto iter = _iter++;
                if(!(*iter))
                {
                    list.erase(iter);
                    continue;
                }
                (*iter)(args...);
            }
            if(list.empty())
            {
                this->map_.erase(iter);
            }
        }
    }
};

} // namespace ext

