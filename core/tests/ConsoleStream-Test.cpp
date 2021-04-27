//
// Created by suncx on 2020/10/21.
//

#include "ConsoleStream.h"
#include "Thread.h"
#include "Mutex.h"

//static Mutex mutex;

static void v1() {
//    MutexGuard guard(mutex);
    INFO << "hello" << 1 << 6.9 << 'x' << "world" << 34 << " bye";
}

int main(int argc, const char *argv[]) {

    LOG << "log world.";
    INFO << "info world.";
    ERROR << "error world.";
//    FATAL << "fatal world.";

//    Thread t1(v1);
//    Thread t2(v1);
//
//    t2.start();
//    t1.start();
//
//    t1.join();
//    t2.join();

    return 0;
}
