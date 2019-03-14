// 第一个索引是主索引，"主"是什么概念，请直接看代码
// C++标准禁止类里面的模板成员函数，不错编译器一般都有支持。
// 但是这支持程度确实让人蛋疼的不行，我列举几个遇到的问题，
// 环境g++ 4.4.3。仅供参考：
//
// 模板类的模板成员函数不能特化或者偏特化
// 模板类继承模板基类，不能调用模板基类中的模板成员函数


#pragma once

#include "entry_index.hpp"

#include <tuple>

namespace ext
{

template<typename T0, typename... Args>
class entry_manager
{
public:
    // 索引元组
    typedef std::tuple<T0, Args...>     tuple_type;
    typedef typename T0::value_type     value_type;
    typedef typename T0::pointer        pointer;
    typedef typename T0::iterator       iterator;
    typedef typename T0::const_iterator const_iterator;

    // 索引类型推导
    template<int N>
    struct index_type
    {
        typedef typename std::tuple_element<N, tuple_type>::type type;
    };

public:
    bool add(pointer p)
    {
        return recursive_insert<0, std::tuple_size<tuple_type>::value>(p);
    }

    void remove(pointer p)
    {
        recursive_erase<0, std::tuple_size<tuple_type>::value>(p);
    }

    void clear()
    {
        recursive_clear<0, std::tuple_size<tuple_type>::value>();
    }

    bool empty() const
    {
        return index<0>().empty();
    }

    size_t size() const
    {
        return index<0>().size();
    }

    iterator begin()
    {
        return index<0>().begin();
    }
    const_iterator begin() const
    {
        return index<0>().begin();
    }

    iterator end()
    {
        return index<0>().end();
    }
    const_iterator end() const
    {
        return index<0>().end();
    }

    template<int N>
    inline typename index_type<N>::type& index()
    {
        return std::get<N>(index_tuple_);
    }
    template<size_t N>
    inline const typename index_type<N>::type& index() const
    {
        return std::get<N>(index_tuple_);
    }

    template<int N>
    typename index_type<N>::type::iterator begin()
    {
        return index<N>().begin();
    }
    template<int N>
    typename index_type<N>::type::const_iterator begin() const
    {
        return index<N>().begin();
    }

    template<int N>
    typename index_type<N>::type::iterator end()
    {
        return index<N>().end();
    }
    template<int N>
    typename index_type<N>::type::const_iterator end() const
    {
        return index<N>().end();
    }

private:
    template<size_t X, size_t N, typename std::enable_if<X<N>::type* =nullptr>
    void recursive_clear()
    {
        index<X>().clear();
        recursive_clear<X+1, N>();
    }
    template<size_t X, size_t N, typename std::enable_if<X==N>::type* =nullptr>
    void recursive_clear()
    {
    }

    template<size_t X, size_t N, typename std::enable_if<X<N>::type* =nullptr>
    void recursive_erase(pointer p)
    {
        index<X>().erase(p);
        recursive_erase<X+1, N>(p);
    }
    template<size_t X, size_t N, typename std::enable_if<X==N>::type* =nullptr >
    void recursive_erase(pointer p)
    {
    }

    template<size_t X, size_t N, typename std::enable_if<X<N>::type* =nullptr >
    bool recursive_insert(pointer p)
    {
        if (!index<X>().insert(p))
        {
            recursive_erase<0, X>(p);
            return false;

        }
        return recursive_insert<X+1, N>(p);
    }
    template<size_t X, size_t N, typename std::enable_if<X==N>::type* =nullptr >
    bool recursive_insert(pointer p)
    {
        return true;
    }
private:
    tuple_type index_tuple_;
};

} // namespace ext

