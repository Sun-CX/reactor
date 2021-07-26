//
// Created by suncx on 2020/9/11.
//

#include "TimerTask.h"
#include "ConsoleStream.h"

using std::chrono::steady_clock;
using std::chrono_literals::operator ""ns;
using reactor::net::TimerTask;

TimerTask::TimerTask(TimerCallback callback, const steady_clock::time_point expire, const nanoseconds interval) :
        expire(expire),
        interval(interval),
        timer_callback(move(callback)),
        index(-1) {
    RC_DEBUG << "---------------------- +TimerTask ----------------------";
}

TimerTask::~TimerTask() {
    RC_DEBUG << "---------------------- -TimerTask ----------------------";
}

void TimerTask::restart(const steady_clock::time_point point) {
    expire = point;
}

void TimerTask::alarm() {
    timer_callback();
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