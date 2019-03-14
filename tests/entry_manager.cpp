#include "entry_manager.hpp"
#include <gtest/gtest.h>

struct entry
{
    int key, value;
};

struct get_key
{
    int operator() (entry *e) { return e->key; }
};

struct get_value
{
    int operator() (entry *e) { return e->value; }
};

TEST(entry_manager, test)
{
    ext::entry_manager<
        ext::unordered_unique_index<entry, int, get_key>,
        ext::ordered_multi_index<entry, int, get_value> > m;
    entry *e1 = new entry{1, 1};
    entry *e2 = new entry{2, 1};
    ASSERT_TRUE(m.add(e1));
    ASSERT_EQ(m.size(), 1);
    ASSERT_FALSE(m.add(e1));
    ASSERT_TRUE(m.add(e2));
    ASSERT_EQ(m.size(), 2);
    for (auto iter = m.begin(); iter != m.end(); iter++)
    {
        ASSERT_EQ(iter->value, 1);
    }
    m.remove(e1);
    m.remove(e2);
    ASSERT_TRUE(m.empty());
    delete e1;
    delete e2;
}
