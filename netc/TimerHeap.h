//
// Created by 孙诚雄 on 2021/7/22.
//

#ifndef REACTOR_TIMERHEAP_H
#define REACTOR_TIMERHEAP_H

#include "NonCopyable.h"
#include <vector>
#include <memory>

namespace reactor::net {
    using reactor::core::NonCopyable;
    using std::vector;
    using std::shared_ptr;

    class TimerTask;

    class TimerHeap final : public NonCopyable {
    private:
        vector<shared_ptr<TimerTask>> heap;

        void sift_up(int end);

        void sift_down(int begin, int end);

        void swap(shared_ptr<TimerTask> &x, shared_ptr<TimerTask> &y) const;

    public:
        void push(const shared_ptr<TimerTask> &task);

        [[nodiscard]]
        shared_ptr<TimerTask> peek() const;

        shared_ptr<TimerTask> pop();

        shared_ptr<TimerTask> remove(int i);

        [[nodiscard]]
        bool empty() const;

        [[nodiscard]]
        size_t size() const;

        void print_all() const;
    };
}

#endif //REACTOR_TIMERHEAP_H
