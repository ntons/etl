#pragma once

#include "random.hpp"
#include "iterator_adapter.hpp"

#include <vector>
#include <functional>

#include <stddef.h>
#include <string.h>

namespace ext
{

template<typename T, int kMaxHeight = 12>
struct skiplist_node
{
    skiplist_node() : value_() {}
    skiplist_node(const T& value) : value_(value) {}
    skiplist_node(T&& value) : value_(value) {}
    T value_;
    int height_ = 0;
    int next_[kMaxHeight] = { 0 };
    int prev_[kMaxHeight] = { 0 };
};

template<typename T,
    typename Less = std::less<T>,
    int kMaxHeight = 12,
    typename Vec = std::vector<skiplist_node<T, kMaxHeight> > >
class skiplist
{
    static const int kHeadPos   = 0;
    static const int kTailPos   = 1;
    static const int kBranching = 2;

public:
    typedef skiplist_node<T, kMaxHeight> node_type;

    typedef T           value_type;
    typedef T&          reference;
    typedef const T&    const_reference;
    typedef T*          pointer;
    typedef const T*    const_pointer;

    struct iterator_facade
    {
        iterator_facade() {}
        iterator_facade(Vec *vec, int pos) : vec_(vec), pos_(pos) {}
        inline reference dereference() const
        {
            return (*vec_)[pos_].value_;
        }
        inline void increment()
        {
            pos_ = (*vec_)[pos_].next_[0];
        }
        inline void decrement()
        {
            pos_ = (*vec_)[pos_].prev_[0];
        }
        inline bool equal(const iterator_facade &x) const
        {
            return vec_ == x.vec_ && pos_ == x.pos_;
        }

        Vec *vec_ = nullptr; 
        int pos_ = 0;
    };

    typedef iterator_adapter<
        value_type, iterator_facade,
        std::bidirectional_iterator_tag>            iterator;
    typedef iterator_adapter<
        const value_type, iterator_facade,
        std::bidirectional_iterator_tag>            const_iterator;

    typedef std::reverse_iterator<iterator>         reverse_iterator;
    typedef std::reverse_iterator<const_iterator>   const_reverse_iterator;

#ifdef _UNIT_TEST
public:
    void Graph() const
    {
        static constexpr int ROW = kMaxHeight + 1;
        static constexpr int COL = 120;
        static constexpr int GAP = 8;
        char buf[ROW][COL];
        memset(buf, 0, ROW * COL);

        int i = 0;
        for (auto it = begin(); it != end(); ++it, ++i)
        {
            snprintf(&buf[0][i*GAP], GAP, "%d,%d",
                     *it, at(it.facade_.pos_).height_);
            for (int j = 0; j < at(it.facade_.pos_).height_; j++)
            {
                snprintf(&buf[j+1][i*GAP], GAP, "=");
            }
        }
        for (int i = ROW-1; i >= 0; i--)
        {
            for (int j = 0; j < COL; j++)
            {
                if (buf[i][j])
                    printf("%c", buf[i][j]);
                else
                    printf(" ");
            }
            printf("\n");
        }
    }

    void GraphVec() const
    {
        static char buf[6+4+10*kMaxHeight];
        for (int i = 0; i < vec_->size(); ++i)
        {
            memset(buf, 0, sizeof(buf));

            node_type &n = vec_->at(i);
            snprintf(buf+0, 6, "%d", n.value_);
            snprintf(buf+6, 4, "%d", n.height_);
            for (int j = 0; j < kMaxHeight; j++)
            {
                snprintf(buf+10+10*j, 10, "%d,%d", n.prev_[j], n.next_[j]);
            }
            for (int k = 0; k < sizeof(buf); k++)
            {
                if (buf[k])
                    printf("%c", buf[k]);
                else
                    printf(" ");
            }
            printf("\n");
        }
    }

    bool check()
    {
        for(int i = 2; i < vec_->size(); ++i)
        {
            const node_type &node = (*vec_)[i];
            for (int j = 0; j < node.height_; ++j)
            {
                if (node.prev_[j] >= vec_->size())
                {
                    printf("check1: %d,%d %d,%d\n", i, j,
                           node.prev_[j], int(vec_->size()));
                    return false;
                }
                if (node.next_[j] >= vec_->size())
                {
                    printf("check2: %d,%d\n", i, j);
                    return false;
                }
                if ((*vec_)[node.prev_[j]].next_[j] != i)
                {
                    printf("check3: %d,%d\n", i, j);
                    return false;
                }
                if ((*vec_)[node.next_[j]].prev_[j] != i)
                {
                    printf("check4: %d,%d\n", i, j);
                    return false;
                }
            }
        }
        return true;
    }
#endif

public:
    explicit skiplist(Vec *vec, int branching = kBranching) :
        vec_(vec), branching_(branching)
    {
        if (vec_->size() < 2) clear(); // initialize
    }

    // Iterators
    iterator begin() noexcept
    {
        return iterator(vec_, at(kHeadPos).next_[0]);
    }
    const_iterator begin() const noexcept
    {
        return const_iterator(vec_, at(kHeadPos).next_[0]);
    }

    iterator end() noexcept
    {
        return iterator(vec_, kTailPos);
    }
    const_iterator end() const noexcept
    {
        return const_iterator(vec_, kTailPos);
    }

    reverse_iterator rbegin() noexcept
    {
        return reverse_iterator(end());
    }
    const_reverse_iterator rbegin() const noexcept
    {
        return const_reverse_iterator(end());
    }

    reverse_iterator rend() noexcept
    {
        return reverse_iterator(begin());
    }
    const_reverse_iterator rend() const noexcept
    {
        return const_reverse_iterator(begin());
    }

    // Capacity
    bool empty() const
    {
        return size() == 0;
    }
    size_t size() const
    {
        return vec_->size() - 2;
    }

    // Modifiers
    void clear()
    {
        vec_->clear();
        vec_->resize(2);
        at(kHeadPos).height_ = 0;
        at(kTailPos).height_ = 0;
        for (int i = 0; i < kMaxHeight; i++)
        {
            at(kHeadPos).next_[i] = kTailPos;
            at(kHeadPos).prev_[i] = kHeadPos;
            at(kTailPos).next_[i] = kTailPos;
            at(kTailPos).prev_[i] = kHeadPos;
        }
    }

    iterator insert(value_type&& value)
    {
        const int pos = vec_->size();
        const int height = random_height();
        // push back new node
        vec_->resize(vec_->size()+1);
        node_type &node = vec_->back();
        node.height_ = height;
        upper_bound_pos(value, node.prev_);
        for (int i = 0; i < height; ++i)
        {
            node.next_[i] = at(node.prev_[i]).next_[i];
            at(node.prev_[i]).next_[i] = pos;
            at(node.next_[i]).prev_[i] = pos;
        }
        node.value_ = std::move(value);
        // update max height
        if (at(kHeadPos).height_ < height)
            at(kHeadPos).height_ = height;
        return iterator(vec_, pos);
    }
    iterator insert(const value_type& value)
    {
        return insert(value_type(value));
    }

    std::pair<iterator, bool> replace(value_type&& value)
    {
        const int pos = vec_->size();
        const int height = random_height();
        // push back new node
        vec_->resize(vec_->size()+1);
        node_type &node = vec_->back();
        node.height_ = height;
        upper_bound_pos(value, node.prev_);
        if (!less(at(node.prev_[0]).value_, value) &&
            !less(value, at(node.prev_[0]).value_))
        {
            iterator it = iterator(vec_, node.prev_[0]);
            vec_->pop_back();
            return std::make_pair(it, false);
        }
        for (int i = 0; i < height; ++i)
        {
            node.next_[i] = at(node.prev_[i]).next_[i];
            at(node.prev_[i]).next_[i] = pos;
            at(node.next_[i]).prev_[i] = pos;
        }
        node.value_ = std::move(value);
        // update max height
        if (at(kHeadPos).height_ < height)
            at(kHeadPos).height_ = height;
        return std::make_pair(iterator(vec_, pos), true);
    }
    std::pair<iterator, bool> replace(const value_type& value)
    {
        return replace(value_type(value));
    }

    iterator erase(const_iterator first, const_iterator last)
    {
        std::vector<int> v;
        for (const_iterator iter = first; iter != last; ++iter)
            v.push_back(iter.facade_.pos_);
        if (v.empty()) return last;
        std::sort(v.begin(), v.end());
        return iterator(vec_, erase_pos(v, last.facade_.pos_));
    }
    iterator erase(const_iterator iter)
    {
        return erase(iter, ++const_iterator(iter));
    }
    template<typename _T>
    iterator erase(const _T& v)
    {
        std::pair<iterator, iterator> range = equal_range(v);
        return erase(range.first, range.second);
    }

    // Lookup
    template<typename _T>
    iterator find(const _T& v)
    {
        int pos = lower_bound_pos(v);
        if (less(v, at(pos).value_) || less(at(pos).value_, v))
            return end();
        return iterator(vec_, pos);
    }
    template<typename _T>
    const_iterator find(const _T& v) const
    {
        int pos = lower_bound_pos(v);
        if (less(v, at(pos).value_) || less(at(pos).value_, v))
            return end();
        return const_iterator(vec_, pos);
    }

    template<typename _T>
    std::pair<iterator, iterator> equal_range(const _T& v)
    {
        std::pair<int, int> range = equal_range_pos(v);
        return std::make_pair(
            iterator(vec_, range.first),
            iterator(vec_, range.second));
    }
    template<typename _T>
    std::pair<const_iterator, const_iterator> equal_range(const _T& v) const
    {
        std::pair<int, int> range = equal_range_pos(v);
        return std::make_pair(
            const_iterator(vec_, range.first),
            const_iterator(vec_, range.second));
    }

    template<typename _T>
    iterator lower_bound(const _T& v)
    {
        return iterator(vec_, lower_bound_pos(v));
    }
    template<typename _T>
    const_iterator lower_bound(const _T& v) const
    {
        return const_iterator(vec_, lower_bound_pos(v));
    }

    template<typename _T>
    iterator upper_bound(const _T& v)
    {
        return iterator(vec_, upper_bound_pos(v));
    }
    template<typename _T>
    const_iterator upper_bound(const _T& v) const
    {
        return const_iterator(vec_, upper_bound_pos(v));
    }

private:
    int random_height() const
    {
        int h = 1;
        while (h < kMaxHeight && random(0, branching_) == 0) ++h;
        return h;
    }

    // make sure the last won't lost
    int erase_pos(const std::vector<int> v, int last)
    {
#ifdef _UNIT_TEST
        if (!std::is_sorted(v.begin(), v.end()))
            throw std::logic_error("not sorted");
        if (v.size() > size())
            throw std::logic_error("bad size");
        for (int i = 0; i < v.size(); ++i) 
        {
            if (v[i] < 2 || v[i] >=vec_->size())
                throw std::logic_error("bad pos");
            if (i > 0 && v[i-1] == v[i])
                throw std::logic_error("not unique");
        }
#endif
        for (int i = v.size() - 1; i >= 0; i--)
        {
            const int pos = v[i];
            // unlink pos
            for (int i = 0, h = at(pos).height_; i < h; i++)
            {
                at(at(pos).prev_[i]).next_[i] = at(pos).next_[i];
                at(at(pos).next_[i]).prev_[i] = at(pos).prev_[i];
            }
            int back = vec_->size() - 1;
            if (back != pos)
            {
                // move back to pos
                for (int i = 0, h = at(back).height_; i < h; i++)
                {
                    at(at(back).prev_[i]).next_[i] = pos;
                    at(at(back).next_[i]).prev_[i] = pos;
                }
                at(pos) = std::move(at(back));
            }
            vec_->pop_back();
            // if back == last, set to where it is
            if (last == back) last = pos;
        }
        return last;
    }

    template<typename _T>
    int lower_bound_pos(const _T& v, int prev[] = nullptr) const
    {
        int xpos = kHeadPos;
        for (int i = at(kHeadPos).height_ - 1; i >= 0; )
        {
            int npos = at(xpos).next_[i];
            if (npos == kTailPos || !less(at(npos).value_, v))
            {
                if (prev) prev[i] = xpos;
                i--;
            }
            else
            {
                xpos = npos;
            }
        }
        return at(xpos).next_[0];
    }

    template<typename _T>
    int upper_bound_pos(const _T& v, int prev[] = nullptr) const
    {
        int xpos = kHeadPos;
        for (int i = at(kHeadPos).height_ - 1; i >= 0; )
        {
            int npos = at(xpos).next_[i];
            if (npos == kTailPos || less(v, at(npos).value_))
            {
                if (prev) prev[i] = xpos;
                i--;
            }
            else
            {
                xpos = npos;
            }
        }
        return at(xpos).next_[0];
    }

    template<typename _T>
    std::pair<int, int> equal_range_pos(const _T& v) const
    {
        return std::make_pair(lower_bound_pos(v), upper_bound_pos(v));
    }

private:
    template<typename _T1, typename _T2>
    static inline bool less(const _T1 &a, const _T2 &b)
    {
        static Less _less; return _less(a, b);
    }

    inline node_type& at(int pos)
    {
        return (*vec_)[pos];
    }
    inline const node_type& at(int pos) const
    {
        return (*vec_)[pos];
    }

private:
    Vec *vec_ = nullptr;
    const int branching_;
};

}

