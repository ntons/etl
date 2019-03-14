#include "datetime.hpp"

#include <cassert>
#include <sys/time.h>

namespace ext {

static long LOCAL_TIMEZOFFSET = []() -> long {
    tzset();
    return timezone * datetime::SECOND;
}();

datetime datetime::epoch()
{
    return datetime(0);
}

datetime datetime::now()
{
    struct timespec ts;
    ::clock_gettime(CLOCK_REALTIME, &ts);
    return datetime(ts.tv_sec * 1000L + ts.tv_nsec / 1000000L);
}

datetime::datetime()
{
}

datetime::datetime(long timestamp) : timestamp_(timestamp)
{
}

datetime::datetime(
    int year, int month, int day,
    int hour, int minute,
    int second, int millisecond) :
    timestamp_(make_timestamp(year, month, day, hour, minute,
                              second, millisecond))
{
}

datetime::datetime(const datetime &another) :
    timestamp_(another.timestamp_)
{
}

datetime& datetime::operator= (const datetime &another)
{
    timestamp_ = another.timestamp_;
    return *this;
}

bool datetime::parse(const std::string &s, const std::string &fmt)
{
    struct tm tm;
    if(!::strptime(s.c_str(), fmt.c_str(), &tm))
        return false;
    timestamp_ = ::timelocal(&tm) * 1000L;
    return true;
}

std::string datetime::format(const std::string &fmt) const
{
    struct tm tm;
    get_tm(&tm);
    char buffer[256] = { 0 };
    ::strftime(buffer, sizeof(buffer), fmt.c_str(), &tm);
    return std::string(buffer);
}

void datetime::get_tm(struct tm *tm) const {
    const time_t t = timestamp_ / SECOND;
    localtime_r(&t, tm);
}

void datetime::get_tm(
    int *year,
    int *month,
    int *day,
    int *hour,
    int *minute,
    int *second,
    int *millisecond,
    int *weekday) const {
    struct tm tm;
    get_tm(&tm);
    if (year) *year = tm.tm_year + 1900;
    if (month) *month = tm.tm_mon + 1;
    if (day) *day = tm.tm_mday;
    if (hour) *hour = tm.tm_hour;
    if (minute) *minute = tm.tm_min;
    if (second) *second = tm.tm_sec;
    if (millisecond) *millisecond = timestamp_ % SECOND;
    if (weekday) *weekday = tm.tm_wday;
}

bool datetime::is_same_year(const datetime &lhs, const datetime &rhs)
{
    if (!lhs || !rhs)
        return false;
    struct tm tm1, tm2;
    lhs.get_tm(&tm1);
    rhs.get_tm(&tm2);
    return tm1.tm_year == tm2.tm_year;
}

bool datetime::is_same_month(const datetime &lhs, const datetime &rhs)
{
    if (!lhs || !rhs)
        return false;
    struct tm tm1, tm2;
    lhs.get_tm(&tm1);
    rhs.get_tm(&tm2);
    return tm1.tm_year == tm2.tm_year && tm1.tm_mon == tm2.tm_mon;
}

bool datetime::is_same_day(const datetime &lhs, const datetime &rhs)
{
    if (!lhs || !rhs)
        return false;
    return (lhs.timestamp_ - LOCAL_TIMEZOFFSET + DAY) / DAY
        == (rhs.timestamp_ - LOCAL_TIMEZOFFSET + DAY) / DAY;
}

bool datetime::is_same_hour(const datetime &lhs, const datetime &rhs)
{
    if (!lhs || !rhs)
        return false;
    return lhs.timestamp_ / HOUR == rhs.timestamp_ / HOUR;
}

bool datetime::is_same_minute(const datetime &lhs, const datetime &rhs)
{
    if (!lhs || !rhs)
        return false;
    return lhs.timestamp_ / MINUTE == rhs.timestamp_ / MINUTE;
}

bool datetime::is_same_week(const datetime &lhs, const datetime &rhs, long offset)
{
    if (!lhs || !rhs)
        return false;
    // epoch is thursday, align to sunday
    return (lhs.timestamp_ - LOCAL_TIMEZOFFSET + 4 * DAY - offset + WEEK) / WEEK
        == (rhs.timestamp_ - LOCAL_TIMEZOFFSET + 4 * DAY - offset + WEEK) / WEEK;
}

long datetime::elapsed_of_hour() const
{
    return timestamp_ % HOUR;
}

long datetime::elapsed_of_day() const
{
    return timestamp_ % DAY;
}

long datetime::elapsed_of_month() const
{
    int y, m;
    get_tm(&y, &m);
    return timestamp_ - make_timestamp(y, m, 1);
}

long datetime::remainning_of_hour() const
{
    return HOUR - elapsed_of_hour();
}

long datetime::remainning_of_day() const
{
    return DAY - elapsed_of_day();
}

long datetime::remainning_of_month() const
{
    int y, m;
    get_tm(&y, &m);
    if(m < 12)
        return make_timestamp(y, m + 1, 1) - timestamp_;
    else
        return make_timestamp(y + 1, 1, 1) - timestamp_;
}

long datetime::make_timestamp(
    int year, int month, int day,
    int hour, int minute,
    int second, int millisecond)
{
    struct tm tm;
    tm.tm_year = year - 1900;
    tm.tm_mon = month - 1;
    tm.tm_mday = day;
    tm.tm_hour = hour;
    tm.tm_min = minute;
    tm.tm_sec = second;
    time_t t = ::mktime(&tm);
    if (t == -1) {
        return -1;
    } else {
        return t * 1000L + millisecond;
    }
}

} // namespace ext

