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

    class EventLoop final : public NonCopyable {
    private:
        using Functor = function<void()>;
        using Functors = vector<Functor>;
        using Channels = vector<Channel *>;

        // for checking one thread has one eventloop object at most.
        static thread_local EventLoop *eventloop_in_current_thread;

        // default timeout setting for poll/epoll system call.
        static const int default_timeout_milliseconds;

        // if eventloop has started.
        bool looping;

        // if eventloop has stopped.
        atomic_bool exited;
        // name of thread that created eventloop.
        const char *thread_name;
        // id of thread that created eventloop.
        const pid_t pid;

        unique_ptr<Poller> poller;
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

        void execute_pending_functors();

    public:
        EventLoop();

        ~EventLoop();

        // start events loop in thread which has created eventloop.
        void loop();

        // check if eventloop is in the thread which has created itself.
        [[nodiscard]]
        bool is_in_created_thread() const;

        // assert eventloop is in the thread which has created itself.
        // program would abort if assert failed.
        void assert_in_created_thread() const;

        void update_channel(Channel *channel);

        void remove_channel(Channel *channel);

        bool has_channel(Channel *channel);

        // stop eventloop, could be called not in thread which has created itself.
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