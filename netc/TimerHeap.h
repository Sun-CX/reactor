//
// Created by 孙诚雄 on 2021/7/22.
//

#ifndef REACTOR_TIMERHEAP_H
#define REACTOR_TIMERHEAP_H

#include "NonCopyable.h"
#include <vector>

namespace reactor::net {
    using reactor::core::NonCopyable;
    using std::vector;
    
    class TimerTask;

    class TimerHeap final : public NonCopyable {
    private:
        vector<TimerTask *> heap;

        void sift_up(int end);

        void sift_down(int begin, int end);

        void swap(TimerTask *&x, TimerTask *&y) const;

    public:
        TimerHeap();

        ~TimerHeap();

        void push(TimerTask *task);

        [[nodiscard]]
        const TimerTask *peek() const;

        TimerTask *pop();

        [[nodiscard]]
        bool empty() const;

        [[nodiscard]]
        size_t size() const;

        void print_all() const;
    };
}

#endif //REACTOR_TIMERHEAP_H
