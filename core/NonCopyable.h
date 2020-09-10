//
// Created by suncx on 2020/8/8.
//

#ifndef REACTOR_NONCOPYABLE_H
#define REACTOR_NONCOPYABLE_H

class NonCopyable {
public:
    NonCopyable(const NonCopyable &rhs) = delete;

    NonCopyable &operator=(const NonCopyable &rhs) = delete;

protected:
    NonCopyable() = default;

    ~NonCopyable() = default;
};

#endif //REACTOR_NONCOPYABLE_H
