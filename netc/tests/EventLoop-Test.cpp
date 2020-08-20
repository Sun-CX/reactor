//
// Created by suncx on 2020/8/18.
//

#include "CurrentThread.h"
#include <cstdio>
#include <EventLoop.h>

static void thread_func(){
    printf("%s[%d] started...\n", CurrentThread::name, CurrentThread::pid);
    EventLoop loop;

}

int main(int argc, const char *argv[]) {
    printf("%s[%d] started...\n", CurrentThread::name, CurrentThread::pid);

    EventLoop loop;



    return 0;
}