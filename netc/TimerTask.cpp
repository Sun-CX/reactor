//
// Created by suncx on 2020/8/18.
//

#include "TimerTask.h"

atomic_uint32_t TimerTask::num_created;

TimerTask::TimerTask(TimerCallback callback, Timestamp when, double interval) : callback(move(callback)),
                                                                               expiration(when), interval(interval),
                                                                               sequence(++num_created) {}

void TimerTask::run() const {
    callback();
}

Timestamp TimerTask::expire_time() const {
    return expiration;
}

bool TimerTask::repeated() const {
    return interval > 0;
}

uint32_t TimerTask::get_sequence() const {
    return sequence;
}

void TimerTask::restart(Timestamp now) {
    if (interval > 0) {
        expiration = add_time(now, interval);
    } else expiration = Timestamp();
}