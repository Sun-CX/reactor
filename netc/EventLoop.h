//
// Created by suncx on 2020/8/17.
//

#ifndef REACTOR_EVENTLOOP_H
#define REACTOR_EVENTLOOP_H

#include "NonCopyable.h"
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
    /**
     * 控制一个线程最多只能有一个 EventLoop
     */
    thread_local static EventLoop *loop_in_this_thread;

    static int default_timeout_milliseconds;

    bool looping;
    bool exited;        // 循环是否退出
    const pid_t pid;
    unique_ptr<Poller> poller;  // 其生命期与 EventLoop 对象相等
    const char *thread_name;
    Channels active_channels;

//    bool event_handling;
//    bool calling_pending_func;
//    int64_t iteration;
//    Timestamp poll_return_time;
//    unique_ptr<TimerQueue> timer_queue;
//    int wakeup_fd;
//    unique_ptr<Channel> wakeup_channel;
public:
    EventLoop();

    ~EventLoop() override;

    // 只能在创建该对象的线程中调用，不能跨线程调用
    void loop();

    /**
     * 检查当前对象是否处于创建该对象的线程中
     * @return
     */
    bool is_in_created_thread() const;

    void assert_in_created_thread();

    void update_channel(Channel *channel);

    void remove_channel(Channel *channel);

    bool has_channel(Channel *channel);

    void quit();

    void run_in_loop(const function<void()> &func);
//    TimerId run_at(const Timer::TimerCallback &callback, Timestamp timestamp);

    static EventLoop *event_loop_of_current_thread();
};

#endif //REACTOR_EVENTLOOP_H
