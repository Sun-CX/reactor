//
// Created by suncx on 2020/10/21.
//

#include "ConsoleStream.h"
#include "Thread.h"

using reactor::core::Thread;

static void v1() {
    RC_TRACE << "hello" << 1 << 6.9 << 'x' << "world" << 34 << " bye " << false;
    RC_DEBUG << "hello" << 1 << 6.9 << 'x' << "world" << 34 << " bye " << (1 == 2);
    RC_INFO << "hello" << 1 << 6.9 << 'x' << "world" << 34 << " bye ";
    RC_WARN << "hello" << 1 << 6.9 << 'x' << "world" << 34 << " bye ";
    RC_ERROR << "hello" << 1 << 6.9 << 'x' << "world" << 34 << " bye ";
//    FATAL << "hello" << 1 << 6.9 << 'x' << "world" << 34 << " bye";
}

int main(int argc, const char *argv[]) {

    Thread t1(v1);
    Thread t2(v1);

    t1.start();
    t2.start();

    t1.join();
    t2.join();
    return 0;
}
