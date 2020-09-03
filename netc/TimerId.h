//
// Created by suncx on 2020/8/18.
//

#ifndef REACTOR_TIMERID_H
#define REACTOR_TIMERID_H

#include "NonCopyable.h"
#include <cstdint>

class Timer;

/**
 * 不透明的定时器 ID, 用来取消定时器
 */
class TimerId final {
private:
    Timer *timer;
    uint32_t sequence;

    friend class TimerQueue;

public:
    TimerId();

    TimerId(Timer *timer, uint32_t seq);
};

#endif //REACTOR_TIMERID_H
