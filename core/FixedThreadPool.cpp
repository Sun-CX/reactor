//
// Created by suncx on 2020/8/13.
//

#include "FixedThreadPool.h"
#include "Thread.h"

using std::bind;
using reactor::core::FixedThreadPool;

FixedThreadPool::FixedThreadPool(int n_threads, int max_task_size, string name)
        : mutex(), not_empty(mutex), not_full(mutex), name(move(name)), max_queue_size(max_task_size), running(false) {
    threads.reserve(n_threads);
}

void FixedThreadPool::start() {
    running = true;
    char thread_no[16];
    for (size_t i = 0; i < threads.capacity(); ++i) {
        snprintf(thread_no, sizeof(thread_no), "-%lu", i + 1);
        threads.emplace_back(new Thread(bind(&FixedThreadPool::thread_routine, this), name + thread_no));
        threads[i]->start();
    }
}

void FixedThreadPool::shutdown() {
    {
        MutexGuard guard(mutex);
        running = false;
        not_empty.notify_all();
    }
    for (const auto &th:threads) th->join();
}

bool FixedThreadPool::full() const {
    return task_queue.size() >= max_queue_size;
}

void FixedThreadPool::submit(FixedThreadPool::Task task) {
    if (threads.empty()) task();
    else {
        MutexGuard guard(mutex);
        while (full()) not_full.wait();
        task_queue.push_back(move(task));
        not_empty.notify();
    }
}

FixedThreadPool::Task FixedThreadPool::get_task() {
    MutexGuard guard(mutex);
    while (task_queue.empty() and running) {
        not_empty.wait();
    }
    Task task;
    if (!task_queue.empty()) {
        task = task_queue.front();
        task_queue.pop_front();
        not_full.notify();
    }
    return task;
}

void FixedThreadPool::thread_routine() {
    while (running) {
        Task task = get_task();
        if (task) task();
    }
}