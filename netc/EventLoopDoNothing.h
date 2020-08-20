//
// Created by suncx on 2020/8/18.
//

#ifndef REACTOR_EVENTLOOPDONOTHING_H
#define REACTOR_EVENTLOOPDONOTHING_H

#include <cstdio>
#include <cstdlib>
#include "NonCopyable.h"
#include "CurrentThread.h"
#include "Exception.h"

class EventLoopDoNothing final : public NonCopyable {
private:
    bool looping;
    const char *thread_name;
    const pid_t pid;

    /**
     * 控制一个线程最多只能有一个 EventLoopDoNothing
     */
    thread_local static bool already_existed_in_this_thread;

    /**
     * 检查当前对象是否处于创建该对象的线程中
     * @return 当前对象是否处于创建该对象的线程中
     */
    bool is_in_created_thread() const;

public:
    EventLoopDoNothing();

    virtual ~EventLoopDoNothing();

    /**
     * 只能在创建该对象的线程中调用本函数，禁止跨线程调用
     */
    void loop();
};


#endif //REACTOR_EVENTLOOPDONOTHING_H
