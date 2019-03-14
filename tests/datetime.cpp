#include "datetime.hpp"
#include <gtest/gtest.h>
#include <iostream>

using namespace ext;

TEST(datetime, is_same_week) {
    datetime d20190316 = datetime(2019, 3, 16); // Sat.
    datetime d20190317 = datetime(2019, 3, 17); // Sun.
    datetime d20190318 = datetime(2019, 3, 18); // Mon.
    datetime d20190319 = datetime(2019, 3, 19); // Tues.
    datetime d20190320 = datetime(2019, 3, 20); // Wed.
    datetime d20190321 = datetime(2019, 3, 21); // Thur.
    datetime d20190322 = datetime(2019, 3, 22); // Fri.
    datetime d20190323 = datetime(2019, 3, 23); // Sat.
    datetime d20190324 = datetime(2019, 3, 24); // Sun.
    // align by sun.
    ASSERT_FALSE(datetime::is_same_week(d20190316, d20190317));
    ASSERT_TRUE(datetime::is_same_week(d20190317, d20190318));
    ASSERT_TRUE(datetime::is_same_week(d20190318, d20190319));
    ASSERT_TRUE(datetime::is_same_week(d20190319, d20190320));
    ASSERT_TRUE(datetime::is_same_week(d20190320, d20190321));
    ASSERT_TRUE(datetime::is_same_week(d20190321, d20190322));
    ASSERT_TRUE(datetime::is_same_week(d20190322, d20190323));
    ASSERT_FALSE(datetime::is_same_week(d20190323, d20190324));
    // align by mon.
    long offset = datetime::DAY;
    ASSERT_TRUE(datetime::is_same_week(d20190316, d20190317, offset));
    ASSERT_FALSE(datetime::is_same_week(d20190317, d20190318, offset));
    ASSERT_TRUE(datetime::is_same_week(d20190318, d20190319, offset));
    ASSERT_TRUE(datetime::is_same_week(d20190319, d20190320, offset));
    ASSERT_TRUE(datetime::is_same_week(d20190320, d20190321, offset));
    ASSERT_TRUE(datetime::is_same_week(d20190321, d20190322, offset));
    ASSERT_TRUE(datetime::is_same_week(d20190322, d20190323, offset));
    ASSERT_TRUE(datetime::is_same_week(d20190323, d20190324, offset));
}

TEST(datetime, is_same_day) {
    datetime d20190318000000 = datetime(2019, 3, 18, 0, 0, 0);
    datetime d20190318235959 = datetime(2019, 3, 18, 23, 59, 59);
    ASSERT_TRUE(datetime::is_same_day(d20190318000000, d20190318235959));
}

TEST(datetime, format) {
    datetime d20190318152930 = datetime(2019, 3, 18, 15, 29, 30);
    ASSERT_EQ(d20190318152930.format(), "2019-03-18 15:29:30");
}

TEST(datetime, parse) {
    std::string s("2019-03-18 15:29:30");
    datetime d;
    ASSERT_TRUE(d.parse(s));
    int year, month, day, hour, minute, second;
    d.get_tm(&year, &month, &day, &hour, &minute, &second);
    ASSERT_EQ(year, 2019);
    ASSERT_EQ(month, 3);
    ASSERT_EQ(day, 18);
    ASSERT_EQ(hour, 15);
    ASSERT_EQ(minute, 29);
    ASSERT_EQ(second, 30);
}
