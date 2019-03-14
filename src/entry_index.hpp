// 管理器要求所有索引必须提供如下类型定义：
// -----------------------------------------------------------------------------
// EntryT                 | 被管理对象类型
// EntryPtrT              | 被管理对象指针类型
// iterator               | 索引迭代
// const_iterator         | 索引const迭代
// -----------------------------------------------------------------------------
// 管理器要求所有索引必须提供如下方法定义：
// -----------------------------------------------------------------------------
// bool insert(EntryPtrT) | 向索引插入对象
// void erase(EntryPtrT)  | 从索引移除对象
// void clear()           | 清空索引对象
// -----------------------------------------------------------------------------

#pragma once

//#include "random.h"

#include <type_traits>
#include <map>
#include <unordered_map>
#include <vector>

// REQUIREMENT
//
// member types:
//  value_type
//  pointer
//  iterator
//  const_iterator
//
// member functions:
//  bool insert(pointer)
//  void erase(const pointer)
//  void clear()

namespace ext {

template<typename Map, typename GetKey>
class map_based_index
{
    template<typename T>
    struct iterator_adapter
    {
        struct enabler{};

        typedef typename std::remove_pointer<
            typename T::value_type::second_type>::type value_type;
        typedef typename std::conditional<
            std::is_const<typename T::reference>::value,
            const value_type &, value_type &>::type reference;
        typedef typename std::conditional<
            std::is_const<typename T::pointer>::value,
            const value_type *, value_type *>::type pointer;

        typedef typename T::iterator_category iterator_category;
        typedef typename T::difference_type difference_type;

        explicit iterator_adapter(T iter): iter_(iter) {}

        template<typename _T>
            iterator_adapter(
                const iterator_adapter<_T> &iter,
                typename std::enable_if<std::is_convertible<_T, T>::value,
                enabler>::type = enabler()) : iter_(iter) {}

        reference operator*() const
        {
            return *iter_->second;
        }
        pointer operator->() const
        {
            return iter_->second;
        }

        iterator_adapter<T>& operator ++()
        {
            ++iter_; return *this;
        }
        iterator_adapter<T> operator ++(int)
        {
            return iterator_adapter<T>(iter_++);
        }

        iterator_adapter<T>& operator --()
        {
            --iter_; return *this;
        }
        iterator_adapter<T> operator --(int)
        {
            return iterator_adapter<T>(iter_--);
        }

        bool operator == (const iterator_adapter<T> &x) const
        {
            return iter_ == x.iter_;
        }
        bool operator != (const iterator_adapter<T> &x) const
        {
            return iter_ != x.iter_;
        }

        T iter_;
    };

public:
    typedef typename Map::key_type                      key_type;
    typedef typename Map::mapped_type                   pointer;
    typedef typename std::remove_pointer<pointer>::type value_type;

    typedef iterator_adapter<typename Map::iterator>        iterator;
    typedef iterator_adapter<typename Map::const_iterator>  const_iterator;

public:
    static bool insert_ret(const typename Map::const_iterator &)
    {
        return true;
    }
    static bool insert_ret(const std::pair<typename Map::const_iterator, bool> &r)
    {
        return r.second;
    }
    bool insert(pointer p)
    {
        if(!p) return false;
        return insert_ret(index_map_.insert(std::make_pair(get_key(p), p)));
    }

    void erase(const pointer p)
    {
        if(!p) return;
        auto range = index_map_.equal_range(get_key(p));
        for(auto iter = range.first; iter != range.second; ++iter)
        {
            if(iter->second == p)
            {
                index_map_.erase(iter);
                break;
            }
        }
    }

    void clear()
    {
        index_map_.clear();
    }

    bool empty() const
    {
        return index_map_.empty();
    }

    size_t size() const
    {
        return index_map_.size();
    }

    size_t count(const key_type &key) const
    {
        return index_map_.count(key);
    }

    pointer find(const key_type &key) const
    {
        auto it = index_map_.find(key);
        if(it == index_map_.end())
            return NULL;
        return it->second;
    }

    void find(const key_type &key, std::vector<pointer> *res) const
    {
        if(res)
        {
            auto range = index_map_.equal_range(key);
            for(auto iter = range.first; iter != range.second; ++iter)
            {
                res->push_back(iter->second);
            }
        }
    }

    iterator begin()
    {
        return iterator(index_map_.begin());
    }
    const_iterator begin() const
    {
        return const_iterator(index_map_.begin());
    }

    iterator end()
    {
        return iterator(index_map_.end());
    }
    const_iterator end() const
    {
        return const_iterator(index_map_.end());
    }

    std::pair<iterator, iterator>
        equal_range(const key_type &key)
        {
            auto pair = index_map_.equal_range(key);
            return std::make_pair(iterator(pair.first), iterator(pair.second));
        }
    std::pair<const_iterator, const_iterator>
        equal_range (const key_type &key) const
        {
            auto pair = index_map_.equal_range(key);
            return std::make_pair(const_iterator(pair.first), const_iterator(pair.second));
        }

protected:
    static key_type get_key(const pointer e)
    {
        static GetKey _get_key;
        return _get_key(e);
    }

protected:
    Map index_map_;
};

template<typename T, typename Key, typename GetKey,
typename KeyEqual = std::equal_to<Key> >
class unordered_unique_index :
    public map_based_index<std::unordered_map<
    Key, T*, std::hash<Key>, KeyEqual>, GetKey>
{
};

template<typename T, typename Key, typename GetKey,
typename KeyEqual = std::equal_to<Key> >
class unordered_multi_index :
    public map_based_index<std::unordered_multimap<
    Key, T*, std::hash<Key>, KeyEqual>, GetKey>
{
};

template<typename Map, typename GetKey>
class ordered_index_base : public map_based_index<Map, GetKey>
{
    using map_based_index<Map, GetKey>::index_map_;

public:
    typedef typename map_based_index<Map, GetKey>::key_type       key_type;
    typedef typename map_based_index<Map, GetKey>::iterator       iterator;
    typedef typename map_based_index<Map, GetKey>::const_iterator const_iterator;

public:
    inline iterator lower_bound(const key_type &key)
    {
        return iterator(index_map_.lower_bound(key));
    }

    inline const_iterator lower_bound(const key_type &key) const
    {
        return const_iterator(index_map_.lower_bound(key));
    }

    inline iterator upper_bound(const key_type &key)
    {
        return iterator(index_map_.upper_bound(key));
    }

    inline const_iterator upper_bound(const key_type &key) const
    {
        return const_iterator(index_map_.upper_bound(key));
    }
};

template<typename T, typename Key, typename GetKey,
typename KeyLess = std::less<Key> >
class ordered_unique_index :
    public ordered_index_base<std::map<Key, T*, KeyLess>, GetKey>
{
};

template<typename T, typename Key, typename GetKey,
typename KeyLess = std::less<Key> >
class ordered_multi_index :
    public ordered_index_base<std::multimap<Key, T*, KeyLess>, GetKey>
{
};

} // namespace ext

