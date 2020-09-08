//
// Created by suncx on 2020/8/17.
//

#ifndef REACTOR_EVENTLOOP_H
#define REACTOR_EVENTLOOP_H

#include "Mutex.h"
#include "TimerTask.h"
#include <vector>
#include <memory>
#include <atomic>

using std::vector;
using std::unique_ptr;
using std::atomic_bool;

class Channel;

class Poller;

class TimerId;

class Timer;

// 创建 EventLoop 对象的线程是 IO 线程，其主要功能是运行事件循环
class EventLoop final : public NonCopyable {
private:
    using Functor = function<void()>;
    using Functors = vector<Functor>;
    using Channels = vector<Channel *>;

    thread_local static EventLoop *loop_ptr_in_this_thread; // 控制一个线程最多只能有一个 EventLoop
    static const int default_poll_timeout_milliseconds;

    bool looping;
    atomic_bool exited;         // 循环是否退出（跨线程读写，原子保护）
    const pid_t pid;
    unique_ptr<Poller> poller;  // poller 生命周期与 EventLoop 对象相同
    Channels active_channels;

    Mutex mutex;
    bool calling_pending_func;
    Functors pending_functors;  // 挂起的执行任务

    const int event_fd;   // 用来唤醒 poll 调用
    unique_ptr<Channel> wakeup_channel;

    unique_ptr<Timer> timer;

//    bool event_handling;
//    int64_t iteration;
//    Timestamp poll_return_time;
//    unique_ptr<TimerQueue> timer_queue;
    void wakeup() const;

    int create_event_fd() const;

    void handle_readable_event() const;

    // 让 IO 线程也能执行一些计算任务
    void execute_pending_functors();

public:
    EventLoop();

    virtual ~EventLoop();

    // 只能在创建该对象的线程中调用，不能跨线程调用
    void loop();

    // 检查当前对象是否处于创建该对象的线程中
    bool is_in_loop_thread() const;

    void update_channel(Channel *channel);

    void remove_channel(Channel *channel);

    // 检查 channel 是否在当前 EventLoop 对象中
    bool has_channel(Channel *channel);

    // 本函数可以跨线程调用
    void quit();

    /**
     * 在 IO 线程中执行某个回调函数，可跨线程调用
     * @param func 回调函数
     */
    void run_in_loop(const Functor &func);

    void queue_in_loop(const Functor &func);

    TimerId run_at(const TimerTask::TimerCallback &callback, Timestamp timestamp);

    // delay(seconds)
    TimerId run_after(const TimerTask::TimerCallback &callback, double delay);

    // delay(seconds)
    TimerId run_every(const TimerTask::TimerCallback &callback, double delay);

    void cancel(TimerId id);

    static EventLoop *event_loop_of_current_thread();
};

#endif //REACTOR_EVENTLOOP_H
