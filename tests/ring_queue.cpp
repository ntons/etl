#include "ring_queue.hpp"
#include <gtest/gtest.h>
#include <string>

using namespace ext;

TEST(ring_queue, push) {
    ring_queue<int> rq(1);

    rq.push(10);
    ASSERT_EQ(rq.ring_.size(), 2);
    ASSERT_EQ(rq.queue_.size(), 0);
}
