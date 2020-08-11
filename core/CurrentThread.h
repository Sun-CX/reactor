//
// Created by suncx on 2020/8/8.
//

#ifndef REACTOR_CURRENTTHREAD_H
#define REACTOR_CURRENTTHREAD_H

#include "Thread.h"

class CurrentThread final : public NonCopyable {
public:

    CurrentThread() = delete;

    static const string &get_name();

    static pid_t get_pid();
};


#endif //REACTOR_CURRENTTHREAD_H
