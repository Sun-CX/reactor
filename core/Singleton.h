//
// Created by ysm on 8/10/20.
//

#ifndef REACTOR_SINGLETON_H
#define REACTOR_SINGLETON_H

#include <pthread.h>
#include <memory>
#include <cassert>
#include <functional>
#include "NonCopyable.h"

using std::shared_ptr;
using std::function;
using std::make_shared;
using std::bind;

template<typename T>
class Singleton : public NonCopyable {
private:
    static pthread_once_t once;
    static shared_ptr<T> ptr;

    static void init() {
        ptr = make_shared<T>();
    }

public:
    Singleton() = delete;

    static shared_ptr<T> instance() {
        assert(pthread_once(&once, Singleton::init) == 0);
        return ptr;
    }
};

template<typename T>
pthread_once_t Singleton<T>::once = PTHREAD_ONCE_INIT;

template<typename T>
shared_ptr<T> Singleton<T>::ptr;

template<class T>
shared_ptr<T> make_singleton() {
    return Singleton<T>::instance();
}

#endif //REACTOR_SINGLETON_H
