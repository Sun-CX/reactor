//
// Created by suncx on 2020/9/3.
//

#include "Timer.h"
#include "EventLoop.h"

static int count = 0;

int main(int argc, const char *argv[]) {

    EventLoop loop;
    Timer timer(&loop);

    printf("now: %s\n", Timestamp::now().to_string(true).c_str());
    timer.schedule([&loop] {
        printf("hello1, now at: %s\n", Timestamp::now().to_string().c_str());
        if (++count == 5) loop.quit();
    }, 2_s, 1_s);

    loop.loop();

    return 0;
}