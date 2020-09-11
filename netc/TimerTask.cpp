//
// Created by suncx on 2020/9/11.
//

#include "TimerTask.h"

TimerTask::TimerTask(TimerCallback callback, const Timestamp &expire_time, const Timestamp &interval)
        : expire_time(expire_time), interval(interval), callback(move(callback)) {}

void TimerTask::alarm() {
    callback();
}

void TimerTask::restart(const Timestamp &abs_time) {
    expire_time = abs_time;
}

bool TimerTask::repeated() const {
    return interval.valid();
}

Timestamp TimerTask::get_expired_time() const {
    return expire_time;
}

bool TimerTask::operator<(const TimerTask &rhs) const {
    return expire_time < rhs.expire_time;
}

bool TimerTask::operator==(const TimerTask &rhs) const {
    return expire_time == rhs.expire_time;
}