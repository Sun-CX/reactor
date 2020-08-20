//
// Created by suncx on 2020/8/18.
//

#include "TimerId.h"

TimerId::TimerId() : timer(nullptr), sequence(0) {}

TimerId::TimerId(Timer *timer, int64_t seq) : timer(timer), sequence(seq) {}
