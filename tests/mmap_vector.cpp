#include "mmap_vector.hpp"
#include <gtest/gtest.h>

TEST(mmap_vector, vector)
{
    const char *dat = "/tmp/mmap_vector_test.dat";
    unlink(dat);

    auto v = new ext::mmap_vector<int>();
    ASSERT_TRUE(v->attach(dat));
    ASSERT_TRUE(v->empty());
    v->push_back(0);
    v->push_back(1);
    v->push_back(2);
    ASSERT_EQ(v->size(), 3);
    delete(v); v = nullptr;

    v = new ext::mmap_vector<int>();
    ASSERT_TRUE(v->attach(dat));
    ASSERT_EQ(v->size(), 3);
    ASSERT_EQ(v->at(0), 0);
    ASSERT_EQ(v->at(1), 1);
    ASSERT_EQ(v->at(2), 2);
    ASSERT_THROW(v->at(3), std::out_of_range);
    delete(v); v = nullptr;
}

TEST(mmap_vector, iterator)
{
    const char *dat = "/tmp/mmap_vector_test.dat";
    unlink(dat);

    auto v = new ext::mmap_vector<int>();
    ASSERT_TRUE(v->attach(dat));

    v->push_back(0);
    v->push_back(1);
    v->push_back(2);

    auto iter = v->begin();
    ASSERT_EQ(*iter, 0);
    ASSERT_EQ(*(++iter), 1);
    ASSERT_EQ(*(++iter), 2);
    ASSERT_EQ(*(--iter), 1);
    ASSERT_EQ(*(--iter), 0);
    ASSERT_EQ(*(iter+1), 1);
    ASSERT_EQ(*(iter+2), 2);
    ASSERT_TRUE(iter == v->begin());

    *iter = 10;
    ASSERT_EQ(v->front(), 10);

    ext::mmap_vector<int>::const_iterator _iter = iter;
    ASSERT_EQ(*_iter, 10);

    delete(v); v = nullptr;
}
