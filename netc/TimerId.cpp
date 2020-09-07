//
// Created by suncx on 2020/8/18.
//

#include "TimerId.h"

TimerId::TimerId() : timer_task(nullptr), sequence(0) {}

TimerId::TimerId(TimerTask *task, uint32_t seq) : timer_task(task), sequence(seq) {}
