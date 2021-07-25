//
// Created by suncx on 2020/8/17.
//

#ifndef REACTOR_EVENTLOOP_H
#define REACTOR_EVENTLOOP_H

#include "Mutex.h"
#include "Timer.h"
#include <atomic>

namespace reactor::net {
    using std::vector;
    using std::unique_ptr;
    using std::atomic_bool;
    using std::chrono::nanoseconds;
    using std::chrono_literals::operator ""ns;
    using reactor::core::NonCopyable;
    using reactor::core::Mutex;
    using Task = shared_ptr<TimerTask>;

    class Channel;

    class Poller;

    class EventLoop final : public NonCopyable {
    private:
        using Functor = function<void()>;
        using Functors = vector<Functor>;
        using Channels = vector<Channel *>;

        // for checking one thread has only one eventloop object at most.
        static thread_local EventLoop *eventloop_in_current_thread;

        // default timeout setting for poll/epoll system call.
        static const int default_timeout_milliseconds;

        // if eventloop has started.
        bool looping;

        // if eventloop has stopped.
        atomic_bool exited;
        // name of thread that created eventloop.
        const char *const thread_name;
        // id of thread that created eventloop.
        const pid_t pid;

        unique_ptr<Poller> poller;
        Channels active_channels;

        // whether `pending_functors` are running.
        bool calling_pending_func;
        // mutex lock for ensure `pending_functors` thread safe.
        Mutex mutex;
        // pending tasks waiting to be run after poll return.
        Functors pending_functors;

        unique_ptr<Channel> wakeup_channel;
        unique_ptr<Timer> timer;

        [[nodiscard]]
        int create_event_fd() const;

        void close_event_fd(int fd) const;

        // wake eventloop up by writing data to eventfd.
        void wakeup() const;

        // read data on eventfd avoiding readable events triggered again.
        void read_wakeup() const;

        void execute_pending_functors();

    public:
        // return a pointer points to eventloop object in current thread.
        // nullptr would be returned if there is no eventloop object in current thread.
        static EventLoop *eventloop_of_current_thread();

        EventLoop();

        ~EventLoop();

        // start events loop in thread which has created eventloop.
        void loop();

        // check if eventloop is in the thread which has created itself.
        // cross-thread calling is allowed.
        [[nodiscard]]
        bool is_in_created_thread() const;

        void update_channel(Channel *channel);

        void remove_channel(Channel *channel);

        bool has_channel(Channel *channel);

        // stop eventloop.
        // cross-thread calling is allowed.
        void quit();

        // cross-thread calling is allowed.
        void run_in_loop(const Functor &func);

        // cross-thread calling is allowed.
        void queue_in_loop(const Functor &func);

        // cross-thread calling is allowed.
        Task schedule(const TimerTask::TimerCallback &callback, const nanoseconds &delay, const nanoseconds &interval = 0ns);

        // cancel a timer task created by calling `schedule`.
        // cross-thread calling is allowed.
        void cancel(const Task &task) const;
    };
}

#endif //REACTOR_EVENTLOOP_H