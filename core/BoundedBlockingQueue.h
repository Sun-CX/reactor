//
// Created by suncx on 2020/8/11.
//

#ifndef REACTOR_BOUNDEDBLOCKINGQUEUE_H
#define REACTOR_BOUNDEDBLOCKINGQUEUE_H

#include "Mutex.h"
#include "Condition.h"

template<class T>
class BoundedBlockingQueue {
private:
    mutable Mutex mutex;
    Condition not_empty;
    Condition not_full;

};

#endif //REACTOR_BOUNDEDBLOCKINGQUEUE_H
