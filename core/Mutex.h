//
// Created by suncx on 2020/8/9.
//

#ifndef REACTOR_MUTEX_H
#define REACTOR_MUTEX_H

#include "NonCopyable.h"
#include <pthread.h>

namespace reactor::core {
    class Mutex final : public NonCopyable {
    private:

        friend class Condition;

        pthread_mutex_t mutex;
        pid_t pid;              // 当前持有锁的线程 ID，为 0 表示没有线程持有锁

        class ConditionWaitGuard final : public NonCopyable {
        private:
            Mutex &mut;
        public:
            explicit ConditionWaitGuard(Mutex &mut);

            ~ConditionWaitGuard();
        };

        pthread_mutex_t *get_mutex();

    public:
        Mutex() noexcept;

        ~Mutex();

        void lock();

        void unlock();

        [[nodiscard]]
        bool is_locked_by_cur_thread() const;
    };

    class MutexGuard final : public NonCopyable {
    private:
        Mutex &mutex;
    public:
        explicit MutexGuard(Mutex &mutex);

        ~MutexGuard();
    };
}

#endif //REACTOR_MUTEX_H
