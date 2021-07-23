//
// Created by 孙诚雄 on 2021/7/22.
//

#include "TimerHeap.h"
#include "TimerTask.h"
#include "ConsoleStream.h"
#include <cassert>

using reactor::net::TimerHeap;
using reactor::net::TimerTask;

TimerHeap::TimerHeap() : heap() {}

TimerHeap::~TimerHeap() {
    for (auto e : heap)
        delete e;
}

void TimerHeap::push(TimerTask *task) {
    heap.push_back(task);
    int index = static_cast<int>(size() - 1);
    task->index = index;
    sift_up(index);
}

const TimerTask *TimerHeap::peek() const {
    assert(!empty());
    return heap.front();
}

TimerTask *TimerHeap::pop() {
    assert(!empty());
    TimerTask *ret = heap.front();
    if (size() == 1) {
        heap.pop_back();
    } else {
        swap(heap.front(), heap.back());
        heap.pop_back();
        sift_down(0, static_cast<int>(size() - 1));
    }
    ret->index = -1;
    return ret;
}

bool TimerHeap::empty() const {
    return heap.empty();
}

size_t TimerHeap::size() const {
    return heap.size();
}

void TimerHeap::sift_up(int end) {
    int cur = end, parent = (cur - 1) / 2;

    while (cur > 0) {
        if (*heap[cur] < *heap[parent]) {
            swap(heap[cur], heap[parent]);
            cur = parent;
            parent = (cur - 1) / 2;
        } else break;
    }
}

void TimerHeap::sift_down(int begin, int end) {
    int cur = begin, child = cur * 2 + 1;

    while (child <= end) {

        if (child < end and *heap[child + 1] < *heap[child])
            ++child;

        if (*heap[child] < *heap[cur]) {
            swap(heap[child], heap[cur]);
            cur = child;
            child = cur * 2 + 1;
        } else break;
    }
}

void TimerHeap::swap(TimerTask *&x, TimerTask *&y) const {
    std::swap(x, y);
    std::swap(x->index, y->index);
}

void TimerHeap::print_all() const {
    for (const auto e: heap)
        RC_DEBUG << e->index << ',' << e->expire.time_since_epoch().count();
}
