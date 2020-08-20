//
// Created by suncx on 2020/8/18.
//

#ifndef REACTOR_TIMER_H
#define REACTOR_TIMER_H

#include "NonCopyable.h"
#include "Timestamp.h"
#include <functional>
#include <atomic>

using std::function;
using std::atomic_int64_t;

class Timer final : public NonCopyable {
private:
    using TimerCallback = function<void()>;

    friend class EventLoop;

    const TimerCallback callback;
    Timestamp expiration;
    const double interval;
    const bool repeated;
    const int64_t sequence;

    static atomic_int64_t num_created;
public:
    Timer(TimerCallback callback, Timestamp when, double interval);

    void run() const;

    Timestamp expire_time() const;

    bool is_repeated() const;

    int64_t get_sequence() const;

    void restart(Timestamp now);

    static int64_t get_num_created();
};


#endif //REACTOR_TIMER_H
