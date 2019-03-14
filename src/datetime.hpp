#pragma once

#include <string>
#include <climits>

namespace ext {

class datetime
{
public:
    /*
     * Interval constance
     */
    static const long FOREVER       = LONG_MAX;
    static const long MILLISECOND   = 1L;
    static const long SECOND        = 1000L;
    static const long MINUTE        = 60000L;
    static const long HOUR          = 3600000L;
    static const long DAY           = 86400000L;
    static const long WEEK          = 604800000L;

public:
    /*
     * 1970-1-1 00:00:00 GMT THUR.
     * 1970-1-1 08:00:00 UTC THUR.
     */
    static datetime epoch();

    /*
     * Now time from system
     */
    static datetime now();

public:
    datetime();
    explicit datetime(long timestamp);
    explicit datetime(
        int year, int month, int day,
        int hour = 0, int minute = 0,
        int second = 0, int millisecond = 0);

    /*
     * clone
     */
    datetime(const datetime &another);
    datetime& operator= (const datetime &another);

    /*
     * timestamp by millisecond
     */
    inline long get_timestamp() const { return timestamp_; }

    /*
     * validate
     */
    inline operator bool () const { return timestamp_ > -1; }

    /*
     * compare
     */
    inline bool operator == (const datetime &dt) const {
        return timestamp_ == dt.timestamp_;
    }
    inline bool operator > (const datetime &dt) const {
        return timestamp_ > dt.timestamp_;
    }
    inline bool operator >= (const datetime &dt) const {
        return timestamp_ >= dt.timestamp_;
    }
    inline bool operator < (const datetime &dt) const {
        return timestamp_ < dt.timestamp_;
    }
    inline bool operator <= (const datetime &dt) const {
        return timestamp_ <= dt.timestamp_;
    }

    /*
     * time delta
     */
    inline long operator - (long timestamp) const {
        return timestamp_ - timestamp;
    }
    inline long operator - (const datetime &dt) const {
        return timestamp_ - dt.timestamp_;
    }

    /*
     * time adjust
     */
    inline datetime& operator += (long value) {
        timestamp_ += value; return *this;
    }
    inline datetime& operator -= (long value) {
        timestamp_ += value; return *this;
    }

    /*
     * values of LOCAL TIMEZONE
     */
    bool parse(const std::string &s, const std::string &fmt = "%F %T");
    std::string format(const std::string &fmt = "%F %T") const;

    void get_tm(struct tm *tm) const;

    void get_tm(
        int *year = nullptr,
        int *month = nullptr,
        int *day = nullptr,
        int *hour = nullptr,
        int *minute = nullptr,
        int *second = nullptr,
        int *millisecond = nullptr,
        int *weekday = nullptr) const;

    static bool is_same_year(const datetime &lhs, const datetime &rhs);
    static bool is_same_month(const datetime &lhs, const datetime &rhs);
    static bool is_same_day(const datetime &lhs, const datetime &rhs);
    static bool is_same_hour(const datetime &lhs, const datetime &rhs);
    static bool is_same_minute(const datetime &lhs, const datetime &rhs);
    // offset to adjust week beginning
    static bool is_same_week(const datetime &lhs, const datetime &rhs, long offset = 0);

    inline bool is_same_year(const datetime &dt) const {
        return is_same_year(*this, dt);
    }
    inline bool is_same_month(const datetime &dt) const {
        return is_same_month(*this, dt);
    }
    inline bool is_same_day(const datetime &dt) const {
        return is_same_day(*this, dt);
    }
    inline bool is_same_hour(const datetime &dt) const {
        return is_same_hour(*this, dt);
    }
    inline bool is_same_minute(const datetime &dt) const {
        return is_same_minute(*this, dt);
    }
    inline bool is_same_week(const datetime &dt, long week_start) const {
        return is_same_week(*this, dt, week_start);
    }

    long elapsed_of_month() const;
    long elapsed_of_day() const;
    long elapsed_of_hour() const;

    long remainning_of_month() const;
    long remainning_of_hour() const;
    long remainning_of_day() const;

public:
    // 计算时间戳
    static long make_timestamp(
        int year, int month, int day,
        int hour = 0, int minute = 0,
        int second = 0, int millisecond = 0);

private:
    long timestamp_ = -1;
};

} // namespace ext

