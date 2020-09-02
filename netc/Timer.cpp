//
// Created by suncx on 2020/8/18.
//

#include "Timer.h"

atomic_int64_t Timer::num_created;

Timer::Timer(Timer::TimerCallback callback, Timestamp when, double interval) : callback(move(callback)),
                                                                               expiration(when), interval(interval),
                                                                               sequence(++num_created) {}

void Timer::run() const {
    callback();
}

Timestamp Timer::expire_time() const {
    return expiration;
}

bool Timer::is_repeated() const {
    return interval > 0;
}

int64_t Timer::get_sequence() const {
    return sequence;
}

void Timer::restart(Timestamp now) {
    if (interval > 0) {
        expiration = add_time(now, interval);
    } else expiration = Timestamp();
}

int64_t Timer::get_num_created() {
    return num_created;
}