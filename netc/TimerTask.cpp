//
// Created by suncx on 2020/9/11.
//

#include "TimerTask.h"

using std::chrono::steady_clock;
using std::chrono_literals::operator ""ns;
using reactor::net::TimerTask;

TimerTask::TimerTask(TimerCallback callback, const steady_clock::time_point &expire, const nanoseconds &interval) :
        expire(expire),
        interval(interval),
        callback(move(callback)),
        index(-1) {}

void TimerTask::set_index(int i) {
    index = i;
}

void TimerTask::restart(const steady_clock::time_point &point) {
    expire = point;
}

void TimerTask::alarm() {
    callback();
}

bool TimerTask::is_repeated() const {
    return interval > 0ns;
}

steady_clock::time_point TimerTask::get_expire() const {
    return expire;
}

int TimerTask::get_index() const {
    return index;
}

bool TimerTask::operator<(const TimerTask &rhs) const {
    return expire < rhs.expire;
}