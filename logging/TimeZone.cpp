//
// Created by suncx on 2020/8/16.
//

#include "TimeZone.h"

/**
 * 将秒数结构化成时分秒
 * @param utc
 * @param seconds
 */
inline void fill_hms(struct tm *utc, int seconds) {
    utc->tm_sec = seconds % 60;
    auto minutes = seconds / 60;
    utc->tm_min = minutes % 60;
    utc->tm_hour = minutes / 60;
}

TimeZone::TimeZone(const char *zone_file) : data(new Data()) {
    if (read_timezone_file(zone_file, data.get())) data.reset();
}

TimeZone::TimeZone(int east_of_utc, const char *tz_name) : data(new Data()) {
    data->local_times.emplace_back(east_of_utc, false, 0);
    data->abbreviation = tz_name;
}

bool TimeZone::valid() const {
    return data != nullptr;
}

tm TimeZone::to_localtime(time_t seconds_since_epoch) const {
    tm localtime;
    memset(&localtime, 0, sizeof(localtime));
    const Data &d(*data);

    Transition sentry(seconds_since_epoch, 0, 0);
//    LocalTime *local =


    return tm();
}
