//
// Created by 孙诚雄 on 2021/7/24.
//

#include "Thread.h"
#include "EventLoop.h"
#include "ConsoleStream.h"

using reactor::core::Thread;
using reactor::net::Task;
using reactor::net::EventLoop;
using std::chrono_literals::operator ""s;

void timer_test1() {

    EventLoop loop;

    int cnt = 0;

    // shrink the scope of a task, so memory of task will be free instantly.
    {
        Task t1 = loop.schedule([&loop, &cnt, &t1] {
            RC_DEBUG << "task-1";
            ++cnt;

            if (cnt == 5) {
                loop.cancel(t1);
                loop.quit();
            }

        }, 1s, 1s);
    }

    {
        Task t2 = loop.schedule([&loop] {
            RC_DEBUG << "task-2";
        }, 2s, 0s);
    }

    {
        Task t3 = loop.schedule([&loop] {
            RC_DEBUG << "task-3";
        }, 3s, 0s);
    }

    loop.loop();
}

void timer_test2() {

    EventLoop loop;

    Task task = loop.schedule([&loop] {
        RC_DEBUG << "hello";
    }, 0s, 1s);

    Thread t([&loop, &task] {
        EventLoop loop1;

        loop1.schedule([&loop, &loop1, &task] {
            loop.cancel(task);
            loop.quit();
            loop1.quit();
        }, 3s, 0s);

        loop1.loop();
    });

    t.start();

    loop.loop();
    t.join();
}

int main(int argc, const char *argv[]) {

    // timer_test1();

    timer_test2();

    return 0;
}