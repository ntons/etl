
#include "random.hpp"
#include <gtest/gtest.h>
#include <stdlib.h>

using namespace ext;

TEST(random, random_pick) {
    srand(time(nullptr));

    for (int i = 0; i < 100; ++i) {
        char ch = random_pick(
            {
            {'0', '9'},
            {'A', 'Z'},
            {'a', 'z'},
            });
        //std::cout << ch << std::endl;
        ASSERT_TRUE(
            ('0' <= ch && ch <= '9') ||
            ('A' <= ch && ch <= 'Z') ||
            ('a' <= ch && ch <= 'z'));
    }
}

