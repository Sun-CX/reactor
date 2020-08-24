//
// Created by suncx on 2020/8/18.
//

#ifndef REACTOR_TIMERID_H
#define REACTOR_TIMERID_H


#include "NonCopyable.h"
#include "Timer.h"

class TimerId final {
private:
    Timer *timer;
    int64_t sequence;

//    friend class EventLoop;
    friend class TimerQueue;

public:
    TimerId();

    TimerId(Timer *timer, int64_t seq);
};


#endif //REACTOR_TIMERID_H
