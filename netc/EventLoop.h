//
// Created by suncx on 2020/8/17.
//

#ifndef REACTOR_EVENTLOOP_H
#define REACTOR_EVENTLOOP_H

#include "Mutex.h"
#include <vector>
#include <memory>
#include <functional>

using std::vector;
using std::unique_ptr;
using std::function;

class Channel;

class Poller;

/**
 * 创建 EventLoop 对象的线程是 IO 线程，其主要功能是运行事件循环
 */
class EventLoop final : public NonCopyable {
private:
    using Channels = vector<Channel *>;
    using Functor = function<void()>;
    using Functors = vector<Functor>;
    /**
     * 控制一个线程最多只能有一个 EventLoop
     */
    thread_local static EventLoop *loop_in_this_thread;

    static int default_timeout_milliseconds;

    bool looping;
    bool exited;        // 循环是否退出
    const pid_t pid;
    unique_ptr<Poller> poller;  // 其生命期与 EventLoop 对象相等
    Channels active_channels;

    mutable Mutex mutex;
    bool calling_pending_func;
    Functors pending_functors;  // 挂起的执行任务

    int wakeup_fd;
    unique_ptr<Channel> wakeup_channel;

//    bool event_handling;
//    int64_t iteration;
//    Timestamp poll_return_time;
//    unique_ptr<TimerQueue> timer_queue;
    void wakeup() const;

    int create_event_fd() const;

    void read_handler();

    /**
     * 让 IO 线程也能执行一些计算任务
     */
    void execute_pending_functors();

public:
    EventLoop();

    virtual ~EventLoop();

    // 只能在创建该对象的线程中调用，不能跨线程调用
    void loop();

    /**
     * 检查当前对象是否处于创建该对象的线程中
     * @return
     */
    bool is_in_loop_thread() const;

    void update_channel(Channel *channel);

    void remove_channel(Channel *channel);

    /**
     * 检查 channel 是否在当前 EventLoop 中
     * @param channel
     * @return
     */
    bool has_channel(Channel *channel);

    /**
     * 本函数可以跨线程调用
     */
    void quit();

    /**
     * 在 IO 线程中执行某个回调函数，可跨线程调用
     * @param func 回调函数
     */
    void run_in_loop(const Functor &func);

    void queue_in_loop(const Functor &func);
//    TimerId run_at(const Timer::TimerCallback &callback, Timestamp timestamp);

    static EventLoop *event_loop_of_current_thread();
};

#endif //REACTOR_EVENTLOOP_H
