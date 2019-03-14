
#include "preprocessor.hpp"
#include <gtest/gtest.h>

TEST(preprocessor, PP_BOOL) {
    ASSERT_EQ(PP_BOOL(0), 0);
    ASSERT_EQ(PP_BOOL(1), 1);
    ASSERT_EQ(PP_BOOL(2), 1);
    ASSERT_EQ(PP_BOOL(15), 1);
}

TEST(preprocessor, PP_IF_I) {
    ASSERT_FALSE(PP_IF_I(0, true, false));
    ASSERT_TRUE(PP_IF_I(1, true, false));
}

TEST(preprocessor, PP_IF) {
    ASSERT_FALSE(PP_IF(0, true, false));
    ASSERT_TRUE(PP_IF(1, true, false));
    ASSERT_TRUE(PP_IF(2, true, false));
    ASSERT_TRUE(PP_IF(15, true, false));
}
