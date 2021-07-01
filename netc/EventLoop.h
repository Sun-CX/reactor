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

namespace reactor::net {
    using std::vector;
    using std::unique_ptr;
    using std::atomic_bool;
    using reactor::core::NonCopyable;
    using reactor::core::Mutex;

    class Timer;

    class Channel;

    class Poller;

    // 每个 EventLoop 对象所在的线程是 IO 线程，其功能是运行事件循环
    class EventLoop final : public NonCopyable {
    private:
        using Functor = function<void()>;
        using Functors = vector<Functor>;
        using Channels = vector<Channel *>;

        static thread_local EventLoop *current_thread_loop; // 控制一个线程中最多只能有一个 EventLoop 对象
        static const int default_poll_timeout_milliseconds;

        bool looping;
        atomic_bool exited;         // 循环是否退出（跨线程读写，原子保护）
        const char *thread_name;    // EventLoop 对象创建时所在线程的线程名称
        const pid_t pid;            // EventLoop 对象创建时所在的线程 ID
        unique_ptr<Poller> poller;  // poller 生命周期与 EventLoop 对象相同
        Channels active_channels;

        Mutex mutex;                // 互斥锁，用来保证 pending_functors
        bool calling_pending_func;  // 是否正在执行 pending_functors
        Functors pending_functors;  // 挂起的执行任务

        unique_ptr<Channel> wakeup_channel; // 用于唤醒 poll 调用，使其立即返回
        unique_ptr<Timer> timer;            // 定时器

        void wakeup() const;

        [[nodiscard]]
        int create_event_fd() const;

        void read_wakeup_event() const;

        // 让 IO 线程执行一些计算任务
        void execute_pending_functors();

    public:
        EventLoop();

        ~EventLoop();

        // 只能在创建该对象的线程中调用，不能跨线程调用
        void loop();

        // 检查当前对象是否处于创建该对象的线程中
        [[nodiscard]]
        bool is_in_loop_thread() const;

        void update_channel(Channel *channel);

        void remove_channel(Channel *channel);

        // 检查 channel 是否在当前 EventLoop 对象中
        bool has_channel(Channel *channel);

        // 本函数可以跨线程调用
        void quit();

        // 如果当前线程处于 loop 线程，那么直接执行回调函数
        // 如果当前线程不在 loop 线程，则将回调函数加入队列
        void run_in_loop(const Functor &func);

        void queue_in_loop(const Functor &func);

        void schedule(const TimerTask::TimerCallback &callback, const Timestamp &after, const Timestamp &interval = Timestamp());

        static EventLoop *event_loop_of_current_thread();
    };
}

#endif //REACTOR_EVENTLOOP_H