//
// Created by 孙诚雄 on 2021/7/24.
//

#include "Timestamp.h"

using std::string;
using std::chrono::milliseconds;
using std::chrono::time_point;
using std::chrono::time_point_cast;
using std::chrono_literals::operator ""s;

string reactor::core::to_string(Timestamp ts) {
    char buf[32];

    time_point<system_clock, milliseconds> since = time_point_cast<milliseconds>(ts);

    time_t sec = since.time_since_epoch() / 1s;

    auto ms = since.time_since_epoch() % 1s;

    tm tm_time;

    ::localtime_r(&sec, &tm_time);

    ::snprintf(buf, sizeof(buf), "%4i-%02i-%02i %02i:%02i:%02i.%03li",
               tm_time.tm_year + 1900,
               tm_time.tm_mon + 1,
               tm_time.tm_mday,
               tm_time.tm_hour,
               tm_time.tm_min,
               tm_time.tm_sec,
               ms.count());
    return buf;
}

string reactor::core::to_utc_string(Timestamp ts) {
    char buf[32];

    time_point<system_clock, milliseconds> since = time_point_cast<milliseconds>(ts);

    time_t sec = since.time_since_epoch() / 1s;

    auto ms = since.time_since_epoch() % 1s;

    tm tm_time;

    ::gmtime_r(&sec, &tm_time);

    ::snprintf(buf, sizeof(buf), "%4i-%02i-%02i %02i:%02i:%02i.%03li",
               tm_time.tm_year + 1900,
               tm_time.tm_mon + 1,
               tm_time.tm_mday,
               tm_time.tm_hour,
               tm_time.tm_min,
               tm_time.tm_sec,
               ms.count());
    return buf;
}