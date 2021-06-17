//
// Created by suncx on 2020/9/11.
//

#ifndef REACTOR_TIMER_H
#define REACTOR_TIMER_H

#include "TimerTask.h"
#include "MinHeap.h"
#include "Channel.h"

class EventLoop;

class Timer final : public NonCopyable {
private:
    EventLoop *loop;
    QuadHeap<TimerTask *> tasks;
    Channel timer_channel;
    const Timestamp base_time;

    [[nodiscard]]
    int create_timer_fd() const;

    void read_handler();

    void read_timeout_event() const;

    void reset_timer_fd() const;

    void add_timer_task_in_loop(TimerTask *task);

    bool insert(TimerTask *task);

public:
    explicit Timer(EventLoop *loop);

    ~Timer();

    // 可跨线程调用
    void schedule(const TimerTask::TimerCallback &callback, const Timestamp &after,
                  const Timestamp &interval = Timestamp());
};

#endif //REACTOR_TIMER_H
