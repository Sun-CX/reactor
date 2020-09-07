//
// Created by suncx on 2020/8/18.
//

#ifndef REACTOR_TIMERTASK_H
#define REACTOR_TIMERTASK_H

#include "NonCopyable.h"
#include "Timestamp.h"
#include <functional>
#include <atomic>

using std::function;
using std::atomic_uint32_t;

/**
 * 定时器内部实现，不向用户代码暴露
 */
class TimerTask final : public NonCopyable {
private:
    friend class EventLoop;

    friend class Timer;

    using TimerCallback = function<void()>;

    const TimerCallback callback;   //定时器回调函数
    Timestamp expiration;           //下一次超时时刻
    const double interval;          //超时间隔，如果是一次性定时器，则设为 0
    const uint32_t sequence;        //定时器序号，从 1 开始
    static atomic_uint32_t num_created;  //定时器计数，当前已经创建的定时器个数
public:
    /**
     *
     * @param callback 超时回调
     * @param when 超时时刻
     * @param interval 为 0 表示一次性定时器
     */
    TimerTask(TimerCallback callback, Timestamp when, double interval);

    void run() const;

    Timestamp expire_time() const;

    bool repeated() const;

    uint32_t get_sequence() const;

    void restart(Timestamp now);
};

#endif //REACTOR_TIMERTASK_H