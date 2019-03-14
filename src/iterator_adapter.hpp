#pragma once

#include <iterator>
#include <type_traits>

namespace ext
{

template<typename T, typename Facade, typename Category>
struct iterator_adapter
{
    typedef Category iterator_category;

    typedef typename std::remove_const<T>::type         value_type;
    typedef typename std::add_lvalue_reference<T>::type reference;
    typedef typename std::add_pointer<T>::type          pointer;

    template<typename... Args>
    iterator_adapter(const Args&... args) : facade_(args...) {}

    template<typename _T>
    iterator_adapter(
        const iterator_adapter<_T, Facade, Category> &it,
        std::enable_if<std::is_same<
        typename std::remove_cv<T>::type,
        typename std::remove_cv<_T>::type
        >::value>* = nullptr) : facade_(it.facade_) {}

    reference operator* () const
    {
        return facade_.dereference();
    }
    pointer   operator-> () const
    {
        return &facade_.dereference();
    }

#define ASSERT_CATEGORY(cat) \
    static_assert(std::is_base_of<std::cat, iterator_category>::value, "");

    // forward_iterator_tag
    iterator_adapter& operator++ ()
    {
        ASSERT_CATEGORY(forward_iterator_tag);
        facade_.increment();
        return *this;
    }
    iterator_adapter  operator++ (int)
    {
        ASSERT_CATEGORY(forward_iterator_tag);
        Facade tmp = facade_;
        facade_.increment();
        return iterator_adapter(tmp);
    }

    // bidirectional_iterator_tag
    iterator_adapter& operator-- ()
    {
        ASSERT_CATEGORY(bidirectional_iterator_tag);
        facade_.decrement();
        return *this;
    }
    iterator_adapter  operator-- (int)
    {
        ASSERT_CATEGORY(bidirectional_iterator_tag);
        Facade tmp = facade_;
        facade_.decrement();
        return iterator_adapter(tmp);
    }

    // random_access_iterator_tag
    iterator_adapter  operator+ (size_t n)
    {
        ASSERT_CATEGORY(random_access_iterator_tag);
        Facade tmp(facade_);
        tmp.advance(n);
        return iterator_adapter(tmp);
    }
    iterator_adapter  operator- (size_t n)
    {
        ASSERT_CATEGORY(random_access_iterator_tag);
        Facade tmp(facade_);
        tmp.advance(-n);
        return iterator_adapter(tmp);
    }

    iterator_adapter& operator+= (size_t n)
    {
        ASSERT_CATEGORY(random_access_iterator_tag);
        facade_.advance(n);
        return *this;
    }
    iterator_adapter& operator-= (size_t n)
    {
        ASSERT_CATEGORY(random_access_iterator_tag);
        facade_.advance(-n);
        return *this;
    }

#undef ASSERT_CATEGORY

    Facade facade_;
};

template<typename T, typename Facade, typename Tag>
bool operator == (
    const iterator_adapter<T, Facade, Tag> &a,
    const iterator_adapter<T, Facade, Tag> &b)
{
    return a.facade_.equal(b.facade_);
}
template<typename T, typename Facade, typename Tag>
bool operator == (
    const iterator_adapter<T, Facade, Tag> &a,
    const iterator_adapter<const T, Facade, Tag> &b)
{
    return a.facade_.equal(b.facade_);
}
template<typename T, typename Facade, typename Tag>
bool operator == (
    const iterator_adapter<const T, Facade, Tag> &a,
    const iterator_adapter<T, Facade, Tag> &b)
{
    return a.facade_.equal(b.facade_);
}

template<typename T, typename Facade, typename Tag>
bool operator != (
    const iterator_adapter<T, Facade, Tag> &a,
    const iterator_adapter<T, Facade, Tag> &b)
{
    return !a.facade_.equal(b.facade_);
}
template<typename T, typename Facade, typename Tag>
bool operator != (
    const iterator_adapter<T, Facade, Tag> &a,
    const iterator_adapter<const T, Facade, Tag> &b)
{
    return !a.facade_.equal(b.facade_);
}
template<typename T, typename Facade, typename Tag>
bool operator != (
    const iterator_adapter<const T, Facade, Tag> &a,
    const iterator_adapter<T, Facade, Tag> &b)
{
    return !a.facade_.equal(b.facade_);
}

} // namespace ext

