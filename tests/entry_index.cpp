#include "entry_index.hpp"
#include <gtest/gtest.h>

struct entry
{
    int key, value;
};

struct get_key
{
    int operator() (entry *e) { return e->key; }
};

TEST(entry_index, unordered_unique_index)
{
    ext::unordered_unique_index<entry, int, get_key> index;
    entry *e = new entry{1, 1};
    ASSERT_TRUE(index.insert(e));
    ASSERT_EQ(index.size(), 1);
    ASSERT_FALSE(index.insert(e));
    for (auto iter = index.begin(); iter != index.end(); iter++)
    {
        ASSERT_EQ(iter->value, 1);
    }
    index.erase(e);
    ASSERT_TRUE(index.empty());
    delete e;
}
