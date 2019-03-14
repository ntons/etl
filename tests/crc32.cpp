
#include "crc32.hpp"
#include <gtest/gtest.h>

using namespace ext;

TEST(crc32, crc32) {
    const char *s = "Hello World";
    EXPECT_EQ(crc32(0, s, s+11), 0x4A17B156);
}
