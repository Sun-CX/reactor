//
// Created by suncx on 2020/8/8.
//

#include "Timestamp.h"

Timestamp::Timestamp(int64_t msSinceEpoch) : microsecond_since_epoch(msSinceEpoch) {}

string Timestamp::to_string() const {
    char buf[32];
    memset(buf, 0, sizeof(buf));
    int64_t sec = microsecond_since_epoch / microseconds_per_second;

    int64_t micro_seconds = microsecond_since_epoch % microseconds_per_second;

    snprintf(buf, sizeof(buf), "%" PRId64 ".%06" PRId64, sec, micro_seconds);
    return buf;
}

void Timestamp::swap(Timestamp &timestamp) {
    std::swap(microsecond_since_epoch, timestamp.microsecond_since_epoch);
}

bool Timestamp::valid() const {
    return microsecond_since_epoch > 0;
}

bool Timestamp::operator<(const Timestamp &rhs) const {
    return microsecond_since_epoch < rhs.microsecond_since_epoch;
}

bool Timestamp::operator>(const Timestamp &rhs) const {
    return rhs < *this;
}

bool Timestamp::operator<=(const Timestamp &rhs) const {
    return !(rhs < *this);
}

bool Timestamp::operator>=(const Timestamp &rhs) const {
    return !(*this < rhs);
}

bool Timestamp::operator==(const Timestamp &rhs) const {
    return microsecond_since_epoch == rhs.microsecond_since_epoch;
}

bool Timestamp::operator!=(const Timestamp &rhs) const {
    return !(rhs == *this);
}

double time_diff(const Timestamp &high, const Timestamp &low) {
    if (unlikely(low > high)) ERROR_EXIT("low > high...");
    int64_t diff = high.microsecond_since_epoch - low.microsecond_since_epoch;
    return static_cast<double>(diff) / Timestamp::microseconds_per_second;
}

Timestamp add_time(const Timestamp timestamp, double seconds_offset) {
    int64_t delta = seconds_offset * Timestamp::microseconds_per_second;
    return Timestamp(timestamp.microsecond_since_epoch + delta);
}

Timestamp Timestamp::now() {
    timespec ts;
    auto status = clock_gettime(CLOCK_REALTIME, &ts);
    if (unlikely(status != 0)) ERROR_EXIT("error occurred.");
    return Timestamp(ts.tv_sec * Timestamp::microseconds_per_second + ts.tv_nsec / 1000);
}

string Timestamp::to_fmt_string(bool show_microsecond) const {
    char buf[32];
    memset(buf, 0, sizeof(buf));
    time_t seconds = microsecond_since_epoch / microseconds_per_second;
    tm tm_time;
    localtime_r(&seconds, &tm_time);

    if (show_microsecond) {
        auto ms = microsecond_since_epoch % microseconds_per_second;
        snprintf(buf, sizeof(buf), "%04d-%02d-%02d %02d:%02d:%02d.%06ld",
                 tm_time.tm_year + 1900,
                 tm_time.tm_mon + 1,
                 tm_time.tm_mday,
                 tm_time.tm_hour,
                 tm_time.tm_min,
                 tm_time.tm_sec,
                 ms);
        return buf;
    }

    snprintf(buf, sizeof(buf), "%04d-%02d-%02d %02d:%02d:%02d",
             tm_time.tm_year + 1900,
             tm_time.tm_mon + 1,
             tm_time.tm_mday,
             tm_time.tm_hour,
             tm_time.tm_min,
             tm_time.tm_sec);
    return buf;
}
