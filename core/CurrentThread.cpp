//
// Created by suncx on 2020/8/8.
//

#include "CurrentThread.h"

thread_local char CurrentThread::name[16];

thread_local pid_t CurrentThread::pid;


