//
// Created by suncx on 2020/8/9.
//

#ifndef REACTOR_MUTEX_H
#define REACTOR_MUTEX_H

#include <pthread.h>
#include "NonCopyable.h"
#include "CurrentThread.h"

class Mutex final : public NonCopyable {
private:

    friend class Condition;

    pthread_mutex_t mutex;
    pid_t pid;              // 持有锁的线程 ID，为 0 表示没有线程持有锁

    class ConditionWaitGuard final : public NonCopyable {
    private:
        Mutex &mut;
    public:
        explicit ConditionWaitGuard(Mutex &mut);

        virtual ~ConditionWaitGuard();
    };

public:
    Mutex();

    virtual ~Mutex();

    void lock();

    void unlock();

    bool is_locked_by_cur_thread() const;

    void assert_locked_by_cur_thread() const;

    pthread_mutex_t *get_mutex();
};

class MutexGuard final : public NonCopyable {
private:
    Mutex &mutex;
public:
    explicit MutexGuard(Mutex &mutex);

    virtual ~MutexGuard();
};

//#define MutexGuard(x) error "anonymous MutexGuard object is not allowed."

#endif //REACTOR_MUTEX_H
