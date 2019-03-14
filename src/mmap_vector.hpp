#pragma once

#include "iterator_adapter.hpp"
#include <string>
#include <vector>
#include <stdexcept>

namespace ext {

template<typename T>
class mmap_vector
{

public:
    typedef T          value_type;
    typedef T&         reference;
    typedef const T&   const_reference;
    typedef T*         pointer;
    typedef const T*   const_pointer;
    typedef long       difference_type;
    typedef size_t     size_type;

public:
    struct iterator_facade
    {
        iterator_facade() {}
        iterator_facade(T* p) : p_(p) {}
        inline reference dereference() const { return *p_; }
        inline void increment() { p_++; }
        inline void decrement() { p_--; }
        inline void advance(size_t n) { p_ += n; }
        inline bool equal(const iterator_facade &x) const { return p_ == x.p_; }
        T* p_ = nullptr;
    };

    typedef iterator_adapter<T, iterator_facade,
            std::random_access_iterator_tag>        iterator;
    typedef iterator_adapter<const T, iterator_facade,
            std::random_access_iterator_tag>        const_iterator;
    typedef std::reverse_iterator<iterator>         reverse_iterator;
    typedef std::reverse_iterator<const_iterator>   const_reverse_iterator;

public:
    // DONOT DO ANYTHING BEFORE ATTACH
    mmap_vector() { }
    ~mmap_vector();

    bool attach(const std::string &path, const size_t capacity = 0);
    bool sync() const;
    void reserve(size_type n);

public:
    // move is logical mess, unsupport
    mmap_vector& operator= (const mmap_vector& v)
    {
        assign(v.begin(), v.end()); return *this;
    }
    mmap_vector& operator= (const std::vector<T>& v)
    {
        assign(v.begin(), v.end()); return *this;
    }

    template <class InputIterator>
    void assign (InputIterator first, InputIterator last)
    {
        clear();
        for (InputIterator iter = first; iter != last; ++iter)
            push_back(*iter);
    }
    void assign (size_type n, const value_type& val)
    {
        clear();
        reserve(n);
        for (size_type i = 0; i < n; ++i)
            push_back(val);
    }

    // Element access
    reference at (size_type n) throw(std::out_of_range)
    {
        if (n < size()) return *(buff_ptr_ + n);
        throw std::out_of_range("mmap_vector::_M_range_check");
    }
    const_reference at (size_type n) const throw(std::out_of_range)
    {
        if (n < size()) return *(buff_ptr_ + n);
        throw std::out_of_range("mmap_vector::_M_range_check");
    }

    reference operator[] (size_type n)
    {
        return *(buff_ptr_ + n);
    }
    const_reference operator[] (size_type n) const
    {
        return *(buff_ptr_ + n);
    }

    reference front()
    {
        return *buff_ptr_;
    }
    const_reference front() const
    {
        return *buff_ptr_;
    }

    reference back()
    {
        return *(buff_ptr_ + size() - 1);
    }
    const_reference back() const
    {
        return *(buff_ptr_ + size() - 1);
    }

    value_type* data()
    {
        return buff_ptr_;
    }
    const value_type* data() const
    {
        return buff_ptr_;
    }

    // Iterators
    iterator begin()
    {
        return iterator(buff_ptr_);
    }
    const_iterator begin() const
    {
        return const_iterator(buff_ptr_);
    }

    iterator end()
    {
        return iterator(buff_ptr_ + size());
    }
    const_iterator end() const
    {
        return const_iterator(buff_ptr_ + size());
    }

    reverse_iterator rbegin()
    {
        return reverse_iterator(end());
    }
    const_reverse_iterator rbegin() const
    {
        return const_reverse_iterator(end());
    }
    reverse_iterator rend()
    {
        return reverse_iterator(begin());
    }
    const_reverse_iterator rend() const
    {
        return const_reverse_iterator(begin());
    }

    // Capacity
    bool empty() const
    {
        return 0 == size();
    }
    size_type size() const
    {
        return *size_ptr_;
    }
    size_type capacity() const
    {
        return capacity_;
    }

    // Modifiers
    void clear()
    {
        *size_ptr_ = 0;
    }

    const_iterator erase(const_iterator iter)
    {
        return erase(iter, iter + 1);
    }
    iterator erase(const_iterator first, const_iterator last)
    {
        memmove(first, last, (end() - last) * sizeof(value_type));
        *size_ptr_ -= last - first;
        return first;
    }

    void push_back(const value_type& val)
    {
        resize(size() + 1, val);
    }
    void pop_back()
    {
        --(*size_ptr_);
    }

    void resize(size_type n, value_type val = value_type())
    {
        if (n > capacity()) reserve(n * 2);
        for (size_t i = size(); i < n; *(buff_ptr_ + i++) = val);
        *size_ptr_ = n;
    }

private:
    // 文件句柄
    int     fd_ = -1;
    // mmap句柄
    void*   mmap_buff_ = nullptr;
    size_t  mmap_size_ = 0;
    // 内存(逻辑)句柄
    T*      buff_ptr_ = nullptr;
    size_t* size_ptr_ = nullptr;
    size_t  capacity_ = 0;
};

} // namespace ext

#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <cassert>

namespace ext
{
template<typename T>
mmap_vector<T>::~mmap_vector()
{
    if(mmap_buff_)
    {
        assert(msync(mmap_buff_, mmap_size_, MS_ASYNC) == 0);
        assert(munmap(mmap_buff_, mmap_size_) == 0);
        mmap_buff_ = nullptr;
    }
    if(fd_ != -1)
    {
        close(fd_);
        fd_ = -1;
    }
}

template<typename T>
bool mmap_vector<T>::attach(const std::string &path, const size_t capacity)
{
    fd_ = open(path.c_str(), O_RDWR | O_CREAT, 0777);
    if(fd_ == -1)
    {
        return false;
    }

    mmap_size_ = lseek(fd_, 0, SEEK_END);
    if(mmap_size_ < sizeof(size_t))
    {
        if(ftruncate(fd_, sizeof(size_t)) == -1)
        {
            close(fd_);
            fd_ = -1;
            return false;
        }
        mmap_size_ = sizeof(size_t);
    }

    mmap_buff_ = mmap(nullptr, mmap_size_, PROT_READ | PROT_WRITE, MAP_SHARED, fd_, 0);
    if(mmap_buff_ == MAP_FAILED)
    {
        close(fd_);
        fd_ = -1;
        return false;
    }

    size_ptr_ = reinterpret_cast<size_t*>(mmap_buff_);
    buff_ptr_ = reinterpret_cast<T*>(reinterpret_cast<char *>(mmap_buff_) + sizeof(size_t));
    capacity_ = (mmap_size_ - sizeof(size_t)) / sizeof(T);

    if(capacity_ < capacity)
        reserve(capacity);

    return true;
}

template<typename T>
bool mmap_vector<T>::sync() const
{
    return msync(mmap_buff_, mmap_size_, MS_ASYNC) == 0;
}

template<typename T>
void mmap_vector<T>::reserve(size_type n)
{
    if(fd_ == -1 || capacity_ >= n)
        return;

    if(mmap_buff_)
    {
        assert(msync(mmap_buff_, mmap_size_, MS_ASYNC) == 0);
        assert(munmap(mmap_buff_, mmap_size_) == 0);
        mmap_buff_ = nullptr;
    }

    mmap_size_ = sizeof(size_t) + sizeof(T) * n;
    assert(ftruncate(fd_, mmap_size_) == 0);
    mmap_buff_ = mmap(nullptr, mmap_size_, PROT_READ | PROT_WRITE, MAP_SHARED, fd_, 0);
    assert(mmap_buff_ != MAP_FAILED);

    size_ptr_ = reinterpret_cast<size_t*>(mmap_buff_);
    buff_ptr_ = reinterpret_cast<T*>(reinterpret_cast<char *>(mmap_buff_) + sizeof(size_t));
    capacity_ = (mmap_size_ - sizeof(size_t)) / sizeof(T);
}

} // namespace ext

