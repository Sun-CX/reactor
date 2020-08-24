//
// Created by suncx on 2020/8/18.
//

#ifndef REACTOR_TIMER_H
#define REACTOR_TIMER_H

#include "NonCopyable.h"
#include "Timestamp.h"
#include <functional>
#include <atomic>

using std::function;
using std::atomic_int64_t;

/**
 * 定时器内部实现，不向用户代码暴露
 */
class Timer final : public NonCopyable {
private:
    using TimerCallback = function<void()>;

//    friend class EventLoop;

    const TimerCallback callback;   //定时器回调函数
    Timestamp expiration;       //下一次超时时刻
    const double interval;      //超时间隔，如果是一次性定时器，则设为 0
    const bool repeated;        //是否重复，若为 false，表示一次性定时器
    const int64_t sequence;     //定时器序号
    static atomic_int64_t num_created;  //定时器计数，当前已经创建的定时器个数
public:
    Timer(TimerCallback callback, Timestamp when, double interval);

    void run() const;

    Timestamp expire_time() const;

    bool is_repeated() const;

    int64_t get_sequence() const;

    void restart(Timestamp now);

    static int64_t get_num_created();
};

#endif //REACTOR_TIMER_H
