// 环形队列，一个线程pop一个线程push可以保证线程安全
//
#pragma once

#include <set>
#include <queue>
#include <vector>
#include <memory>
#include <iostream>

namespace ext {

template<typename T>
class ring_queue_base
{
public:
    ring_queue_base(size_t size) : size_(size + 1)
    {
        ring_.resize(size + 1);
    }

    /*
     * purge data from queue to ring
     */
    void purge()
    {
        while (!queue_.empty() && push_to_ring(std::move(queue_.front())))
            queue_.pop();
    }

    void push(const T &t)
    {
        purge(); push_to_ring_or_queue(t);
    }
    void push(T &&t)
    {
        purge(); push_to_ring_or_queue(std::move(t));
    }

    bool pop(T *pt)
    {
        if (head_idx_ == tail_idx_)
            return false;
        if(pt) *pt = std::move(ring_[head_idx_]);
        head_idx_ = (head_idx_ + 1) % size_;
        return true;
    }

private:
    template<typename _T>
    void push_to_ring_or_queue(_T &&t)
    {
        if (!queue_.empty() || !push_to_ring(std::forward<_T>(t)))
            queue_.push(std::forward<_T>(t));
    }

    template<typename _T>
    bool push_to_ring(_T &&t)
    {
        if ((tail_idx_ + 1) % size_ == head_idx_)
            return false;
        ring_[tail_idx_] = std::forward<_T>(t);
        tail_idx_ = (tail_idx_ + 1) % size_;
        return true;
    }

#ifdef _UNIT_TEST
public:
#else
private:
#endif
    const size_t size_;
    std::vector<T> ring_;
    size_t tail_idx_ = 0;
    size_t head_idx_ = 0;
    //
    std::queue<T> queue_;
};

// T*
template<typename T>
class ring_queue : public ring_queue_base<T>
{
public:
    ring_queue(size_t size) : ring_queue_base<T>(size) { }
};

template<typename T>
class ring_queue<T*> : public ring_queue_base<T*>
{
public:
    ring_queue(size_t size) : ring_queue_base<T*>(size) { }

    void push(T *p)
    {
        if(p) ring_queue_base<T*>::push(p);
    }

    T* pop()
    {
        T *p = NULL;
        return ring_queue_base<T*>::pop(&p) ? p : NULL;
    }
};

// std::shared_ptr<T>
template<typename T>
class ring_queue<std::shared_ptr<T> > : public ring_queue_base<std::shared_ptr<T> >
{
public:
    ring_queue(size_t size) : ring_queue_base<std::shared_ptr<T> >(size) { }

    void push(T *p)
    {
        if(p) ring_queue_base<std::shared_ptr<T> >::push(p);
    }

    std::shared_ptr<T> pop()
    {
        std::shared_ptr<T> p;
        ring_queue_base<std::shared_ptr<T> >::pop(&p);
        return p;
    }
};

} // namespace ext

