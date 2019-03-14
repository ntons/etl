#include "delegate.hpp"
#include "etl.hpp"
#include <gtest/gtest.h>

int Add(int a, int b)
{
    return a + b;
}

TEST(delegate, with_return)
{
    ext::delegate<int(int, int)> d;
    ASSERT_TRUE(d.empty());
    d += [](int a, int b) -> int { return a + b; };
    ASSERT_FALSE(d.empty());
    ASSERT_EQ(d(1, 2), 3);
}

TEST(delegate, without_return)
{
    int v = 0;
    ext::delegate<void(int)> d;
    ASSERT_TRUE(d.empty());
    d += [&v](int a) { v += a; };
    d += [&v](int a) { v += a; };
    ASSERT_FALSE(d.empty());
    d(1);
    ASSERT_EQ(v, 2);
}

TEST(delegate, priority)
{
    int i = 0;
    int seq[5] = {0, 0, 0, 0, 0};
    ext::delegate<void()> d;
    d[0]  += [&](){ seq[i++] = 0; };
    d[2]  += [&](){ seq[i++] = 2; };
    d[-1] += [&](){ seq[i++] = -1; };
    d[1]  += [&](){ seq[i++] = 1; };
    d[-2] += [&](){ seq[i++] = -2; };
    d();
    ASSERT_EQ(seq[0], -2);
    ASSERT_EQ(seq[1], -1);
    ASSERT_EQ(seq[2], 0);
    ASSERT_EQ(seq[3], 1);
    ASSERT_EQ(seq[4], 2);
}

TEST(delegate, track)
{
    ext::tracker *_tracker = new ext::tracker();
    ext::delegate<void()> d;
    ASSERT_TRUE(d.empty());
    d.connect([](){}, _tracker);
    ASSERT_FALSE(d.empty());
    SAFE_DELETE(_tracker);
    ASSERT_TRUE(d.empty());
}

