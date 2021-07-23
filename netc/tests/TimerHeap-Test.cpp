//
// Created by 孙诚雄 on 2021/7/22.
//

#include "TimerHeap.h"
#include "TimerTask.h"
#include "ConsoleStream.h"

using reactor::net::TimerTask;
using reactor::net::TimerHeap;
using std::chrono::steady_clock;
using std::chrono_literals::operator ""ns;

int main(int argc, const char *argv[]) {

    TimerHeap heap;

    steady_clock::time_point point(0ns);

    heap.push(new TimerTask(nullptr, point + 10ns, 0ns));
    heap.print_all();

    heap.push(new TimerTask(nullptr, point + 3ns, 0ns));
    heap.print_all();

    heap.push(new TimerTask(nullptr, point + 7ns, 0ns));
    heap.print_all();

    heap.push(new TimerTask(nullptr, point + 6ns, 0ns));
    heap.print_all();

    heap.push(new TimerTask(nullptr, point + 24ns, 0ns));
    heap.print_all();

    heap.push(new TimerTask(nullptr, point + 8ns, 0ns));
    heap.print_all();

    heap.push(new TimerTask(nullptr, point + 1ns, 0ns));
    heap.print_all();

    RC_DEBUG << "-----------------------";

    TimerTask *task;

    while (!heap.empty()) {
        task = heap.pop();
        RC_DEBUG << task->get_expire().time_since_epoch().count();
    }

    return 0;
}