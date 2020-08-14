//
// Created by suncx on 2020/8/13.
//

#ifndef REACTOR_THREADPOOL_H
#define REACTOR_THREADPOOL_H

#include "NonCopyable.h"
#include "Mutex.h"
#include "Condition.h"
#include "Thread.h"
#include <vector>
#include <memory>
#include <deque>
#include <cstring>
#include <functional>

using std::vector;
using std::unique_ptr;
using std::deque;
using std::move;
using std::bind;

/**
 * 线程池本质上也是一个生产者/消费者问题
 */
class ThreadPool : public NonCopyable {
private:
    using Task = function<void()>;

    mutable Mutex mutex;
    Condition not_empty;
    Condition not_full;
    string name;
//    Task task;
    vector<unique_ptr<Thread>> threads;
    deque<Task> task_queue;
    size_t max_queue_size;  // 任务队列的最大长度
    bool running;           // 线程池是否处于运行状态

    /**
     * 必须互斥访问
     * @return 
     */
    bool full() const;

    void run_in_thread();

    Task get_task();

public:
    explicit ThreadPool(string name = "thread-pool");

    /**
     * 必须在 start() 函数之前调用
     * @param max_size
     */
    void set_max_queue_size(size_t max_size);

    void start(int n_threads);

    void stop();

    /**
     * 向线程池中提交一个任务
     * @param t 任务
     */
    void submit(Task t);

};

#endif //REACTOR_THREADPOOL_H
