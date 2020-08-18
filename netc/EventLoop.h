//
// Created by suncx on 2020/8/17.
//

#ifndef REACTOR_EVENTLOOP_H
#define REACTOR_EVENTLOOP_H

#include "Timestamp.h"
#include "CurrentThread.h"
#include "Channel.h"
#include <atomic>

using std::unique_ptr;
using std::atomic_bool;

/**
 * 每个 EventLoop 代表一个 IO 线程
 */
class EventLoop final : public NonCopyable {
private:
//    thread_local static EventLoop *loop_in_this_thread;

    bool looping;
    const char *thread_name;
    const pid_t pid;

    /**
     * 控制一个线程最多只能有一个 EventLoop
     */
    thread_local static bool already_existed_in_this_thread;

//    atomic_bool quit;
//    bool event_handling;
//    bool calling_pending_func;
//    int64_t iteration;
//    Timestamp poll_return_time;
//    unique_ptr<Poller> poller;
//    unique_ptr<TimerQueue> timer_queue;
//    int wakeup_fd;
//    unique_ptr<Channel> wakeup_channel;
public:
    EventLoop();

    virtual ~EventLoop();

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
};

#endif //REACTOR_EVENTLOOP_H
