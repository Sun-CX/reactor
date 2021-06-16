//
// Created by suncx on 2020/8/9.
//

#ifndef REACTOR_CONDITION_H
#define REACTOR_CONDITION_H

#include "Mutex.h"

class Condition final : public NonCopyable {
private:
    Mutex &mutex;
    pthread_cond_t cond;
public:
    explicit Condition(Mutex &mutex);

    ~Condition();

    /**
     * 注意：此函数调用之前必须加锁，以保护条件
     * 将当前线程加入阻塞队列，等待其它的线程对条件变量发送信号：signal() or broadcast()
     */
    void wait();

    /**
     * 注意：此函数调用之前必须加锁，以保护条件
     *
     * 线程等待一定的时间，如果超时或有信号触发，则线程唤醒
     * @param seconds 秒
     * @param microseconds 微秒
     * @return 是否超时
     */
    bool timed_wait(long seconds, long microseconds = 0);

    /**
     * 唤醒阻塞在条件变量上的一个线程，使其处于就绪态
     */
    void notify();

    /**
     * 唤醒阻塞在条件变量上的所有线程
     */
    void notify_all();
};

#endif //REACTOR_CONDITION_H
