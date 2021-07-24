//
// Created by 孙诚雄 on 2021/7/22.
//

#include "TimerHeap.h"
#include "TimerTask.h"
#include "ConsoleStream.h"
#include <cassert>

using std::make_shared;
using std::shared_ptr;
using reactor::net::TimerTask;
using reactor::net::TimerHeap;
using std::chrono::steady_clock;
using std::chrono_literals::operator ""ns;

void test_in_heap() {

    TimerHeap heap;

    steady_clock::time_point point(0ns);

    const auto f = [] {};

    heap.push(make_shared<TimerTask>(f, point + 10ns, 0ns));
    heap.print_all();

    heap.push(make_shared<TimerTask>(f, point + 3ns, 0ns));
    heap.print_all();

    heap.push(make_shared<TimerTask>(f, point + 7ns, 0ns));
    heap.print_all();

    heap.push(make_shared<TimerTask>(f, point + 6ns, 0ns));
    heap.print_all();

    heap.push(make_shared<TimerTask>(f, point + 24ns, 0ns));
    heap.print_all();

    heap.push(make_shared<TimerTask>(f, point + 8ns, 0ns));
    heap.print_all();

    heap.push(make_shared<TimerTask>(f, point + 1ns, 0ns));
    heap.print_all();

    RC_DEBUG << "-----------------------";

    shared_ptr<TimerTask> task;

    while (!heap.empty()) {
        task = heap.pop();
        assert(task->get_index() == -1);
        RC_DEBUG << task->get_expire().time_since_epoch().count();
    }
}

void test_remove() {
    TimerHeap heap;

    steady_clock::time_point point(0ns);

    const auto f = [] {};

    heap.push(make_shared<TimerTask>(f, point + 10ns, 0ns));
    heap.print_all();

    heap.push(make_shared<TimerTask>(f, point + 3ns, 0ns));
    heap.print_all();

    heap.push(make_shared<TimerTask>(f, point + 7ns, 0ns));
    heap.print_all();

    heap.push(make_shared<TimerTask>(f, point + 6ns, 0ns));
    heap.print_all();

    heap.push(make_shared<TimerTask>(f, point + 24ns, 0ns));
    heap.print_all();

    heap.push(make_shared<TimerTask>(f, point + 8ns, 0ns));
    heap.print_all();

    heap.push(make_shared<TimerTask>(f, point + 1ns, 0ns));
    heap.print_all();

    RC_DEBUG << "-----------------------";

    heap.remove(6);
    heap.print_all();

}

int main(int argc, const char *argv[]) {

    // test_in_heap();

    test_remove();
    return 0;
}