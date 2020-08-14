//
// Created by suncx on 2020/8/13.
//

#include "ThreadPool.h"

ThreadPool::ThreadPool(string name)
        : mutex(), not_empty(mutex), not_full(mutex), name(move(name)), max_queue_size(0), running(false) {}

void ThreadPool::set_max_queue_size(size_t max_size) {
    max_queue_size = max_size;
}

void ThreadPool::start(int n_threads) {
    running = true;
    threads.reserve(n_threads);
    char thread_no[16];
    for (int i = 0; i < n_threads; ++i) {
        memset(thread_no, 0, sizeof(thread_no));
        snprintf(thread_no, sizeof(thread_no), "-%d", i + 1);
        threads.emplace_back(new Thread(bind(&ThreadPool::run_in_thread, this), name + thread_no));
        threads[i]->start();
    }
}

void ThreadPool::stop() {
    {
        MutexGuard guard(mutex);
        running = false;
        not_empty.notify();
    }
    for (const auto &th:threads) th->join();
}

bool ThreadPool::full() const {
    return 0 < max_queue_size and max_queue_size <= task_queue.size();
}

void ThreadPool::submit(ThreadPool::Task t) {
    if (threads.empty()) t();
    else {
        MutexGuard guard(mutex);
        while (full()) not_full.wait();
        task_queue.push_back(move(t));
        not_empty.notify();
    }
}

ThreadPool::Task ThreadPool::get_task() {
    MutexGuard guard(mutex);
    while (task_queue.empty() and running) {
        not_empty.wait();
    }
    Task task;
    if (!task_queue.empty()) {
        task = task_queue.front();
        task_queue.pop_front();
        if (0 < max_queue_size) {
            not_full.notify();
        }
    }
    return task;
}

void ThreadPool::run_in_thread() {
    if (task) task();
    while (running) {
        Task task = get_task();
        if (task) task();
    }
}
