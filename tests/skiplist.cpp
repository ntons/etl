#include "skiplist.hpp"
#include "random.hpp"
#include <gtest/gtest.h>
#include <iostream>

using namespace std;
using namespace ext;

typedef skiplist<int> skiplist_int;

TEST(skiplist, insert_erase)
{
    std::vector<skiplist_int::node_type> v;
    skiplist_int s(&v);
    ASSERT_EQ(v.size(), 2);

    for (int i = 0; i < 10000; i++)
    {
        s.insert(random<int>(1, 1000));
    }
    ASSERT_EQ(s.size(), 10000);
    ASSERT_TRUE(s.check());

    for (int i = 1; i < 1000; i++)
    {
        s.erase(i);
        ASSERT_TRUE(s.check());
    }
    ASSERT_EQ(s.size(), 0);
}

TEST(skiplist, unique_insert)
{
    std::vector<skiplist_int::node_type> v;
    skiplist_int s(&v);
    ASSERT_EQ(v.size(), 2);

    std::pair<skiplist_int::iterator, bool> pair;

    pair = s.unique_insert(100);
    ASSERT_TRUE(pair.second);
    ASSERT_EQ(*pair.first, 100);

    pair = s.unique_insert(100);
    ASSERT_FALSE(pair.second);
    ASSERT_EQ(*pair.first, 100);

    pair = s.unique_insert(50);
    ASSERT_TRUE(pair.second);
    ASSERT_EQ(*pair.first, 50);

    pair = s.unique_insert(50);
    ASSERT_FALSE(pair.second);
    ASSERT_EQ(*pair.first, 50);
}
