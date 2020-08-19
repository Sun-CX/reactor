//
// Created by suncx on 2020/8/13.
//

#ifndef REACTOR_THREADPOOL_H
#define REACTOR_THREADPOOL_H

#include "Thread.h"
#include "Condition.h"
#include <vector>
#include <memory>
#include <deque>

using std::vector;
using std::unique_ptr;
using std::deque;

/**
 * 线程池本质上也是一个生产者/消费者问题
 */
class FixedThreadPool : public NonCopyable {
private:
    using Task = function<void()>;

    mutable Mutex mutex;
    Condition not_empty;
    Condition not_full;
    string name;
    vector<unique_ptr<Thread>> threads;
    deque<Task> task_queue;
    size_t max_queue_size;  // 任务队列的最大大小
    bool running;           // 线程池是否处于运行状态

    bool full() const;

    void thread_routine();

    Task get_task();

public:
    explicit FixedThreadPool(int n_threads, int max_task_size, string name = "thread-pool");

    void start();

    void shutdown();

    /**
     * 向线程池中提交一个任务
     * @param task 任务
     */
    void submit(Task task);

};

#endif //REACTOR_THREADPOOL_H
