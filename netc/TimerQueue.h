//
// Created by suncx on 2020/8/18.
//

#ifndef REACTOR_TIMERQUEUE_H
#define REACTOR_TIMERQUEUE_H

#include "NonCopyable.h"
#include "Channel.h"
#include <vector>
#include <set>

using std::vector;
using std::set;
using std::pair;

class EventLoop;

class Timestamp;

class Timer;

class TimerId;

/**
 * 定时器内部实现，不向用户代码暴露
 */
class TimerQueue final : public NonCopyable {
private:
    using TimerCallback = function<void()>;

    /**
     * 这里用 set<Entry> 结构,而不直接使用 map<Timestamp, Timer *> 的原因在于:
     * 相同的 Timestamp 可能有不同的 Timer, 而 map 不允许有相同的 key
     * 可以考虑使用 multimap
     */
    using Entry = pair<Timestamp, Timer *>;
    using Timers = set<Entry>;
    using ActiveTimer = pair<Timer *, uint32_t>;
    using ActiveTimerSet = set<ActiveTimer>;

    EventLoop *loop;
    const int timer_fd;
    bool calling_expired_timers;
    Channel timer_channel;

    Timers timers;
    ActiveTimerSet active_timers;
    ActiveTimerSet canceled_timers;

    int create_timer_fd() const;

    void reset_timer_fd(int fd, Timestamp timestamp) const;

    /**
     * 超时可读事件触发,读取可读数据以消耗掉本次事件(水平触发模式下如果不读取则会一直触发该事件)
     * @param fd 定时器 fd
     */
    void read_timeout_event(int fd, Timestamp now) const;

    /**
     * 计算从当前到 time 的时间,以 timespec 结构返回
     * @param time 未来的某个时刻
     * @return
     */
    timespec time_from_now(Timestamp time) const;

    vector<Entry> get_expired(Timestamp now);

    void read_handler();

    void add_timer_in_loop(Timer *timer);

    void cancel_in_loop(TimerId timer_id);

    bool insert(Timer *timer);

    void reset(const vector<Entry> &expired, Timestamp now);

public:
    explicit TimerQueue(EventLoop *loop);

    /**
     * 必须是线程安全的,通常被其它线程调用(即可以跨线程调用)
     * @param callback
     * @param when
     * @param interval
     * @return
     */
    TimerId add_timer(TimerCallback callback, Timestamp when, double interval);

    /**
     * 可被其它线程调用(即可以跨线程调用)
     * @param timer_id
     */
    void cancel(TimerId timer_id);
};

#endif //REACTOR_TIMERQUEUE_H
