//
// Created by suncx on 2020/8/8.
//

#ifndef REACTOR_CURRENTTHREAD_H
#define REACTOR_CURRENTTHREAD_H

#include "NonCopyable.h"
#include <unistd.h>

class CurrentThread final : public NonCopyable {
public:
    CurrentThread() = delete;

    // main 线程名最长为 16 字节（包括末尾 '\0' 符）
    thread_local static char name[32];
    thread_local static pid_t pid;
};

#endif //REACTOR_CURRENTTHREAD_H
