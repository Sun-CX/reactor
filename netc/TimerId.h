//
// Created by suncx on 2020/8/18.
//

#ifndef REACTOR_TIMERID_H
#define REACTOR_TIMERID_H

#include "NonCopyable.h"
#include <cstdint>

class TimerTask;

/**
 * 不透明的定时器 ID, 用来取消定时器
 */
class TimerId final {
private:
    friend class Timer;

    TimerTask *timer_task;
    uint32_t sequence;
public:
    TimerId();

    TimerId(TimerTask *task, uint32_t seq);
};

#endif //REACTOR_TIMERID_H
