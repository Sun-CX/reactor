//
// Created by suncx on 2020/8/18.
//

#ifndef REACTOR_TIMERQUEUE_TEST_H
#define REACTOR_TIMERQUEUE_TEST_H

#include "TimerTask.h"
#include "Channel.h"
#include <vector>
#include <set>

using std::vector;
using std::set;
using std::pair;

class EventLoop;

class Timestamp;

class TimerId;

// TODO: 使用最小堆重写
// 定时器内部实现，不向用户代码暴露
class Timer final : public NonCopyable {
private:
    /**
     * 这里用 set<Entry> 结构,而不直接使用 map<Timestamp, Timer *> 的原因在于:
     * 相同的 Timestamp 可能有不同的 Timer, 而 map 不允许有相同的 key
     * 可以考虑使用 multimap
     */
    using Entry = pair<Timestamp, TimerTask *>;
    using Timers = set<Entry>;  // 按照过期时间升序排序（允许有相同的过期时间，若过期时间相同则再按照 Timer 地址升序排序）
    using ActiveTimer = pair<TimerTask *, uint32_t>;
    using ActiveTimerSet = set<ActiveTimer>;

    EventLoop *loop;
    const int timer_fd;
    bool calling_expired_timers;
    Channel timer_channel;

    Timers timers;
    ActiveTimerSet active_timers;
    ActiveTimerSet canceled_timers;

    [[nodiscard]]
    int create_timer_fd() const;

    void reset_timer_fd(int fd, Timestamp timestamp) const;

    // 超时可读事件触发,读取可读数据以消耗掉本次事件(水平触发模式下如果不读取则会一直触发该事件)
    void read_timeout_event(int fd, Timestamp now) const;

    // 计算从当前到 time 的时间,以 timespec 结构返回
    [[nodiscard]]
    timespec time_from_now(Timestamp time) const;

    vector<Entry> get_expired(Timestamp now);

    void read_handler();

    // only invoked in event loop
    void add_timer_in_loop(TimerTask *task);

    // only invoked in event loop
    void cancel_in_loop(TimerId timer_id);

    // only invoked in event loop
    // 返回：当前插入的 timer 的到期时间是否为最早
    bool insert(TimerTask *task);

    void reset(vector<Entry> &expired, Timestamp now);

public:
    explicit Timer(EventLoop *loop);

    virtual ~Timer();

    // 可跨线程调用
    TimerId schedule(TimerTask::TimerCallback callback, Timestamp when, double interval = 0);

    // 可跨线程调用
    void cancel(TimerId timer_id);
};

#endif //REACTOR_TIMERQUEUE_TEST_H
