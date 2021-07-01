//
// Created by ysm on 8/10/20.
//

#ifndef REACTOR_SINGLETON_H
#define REACTOR_SINGLETON_H

#include "GnuExt.h"
#include "ConsoleStream.h"
#include <pthread.h>

namespace reactor::core {
    using std::shared_ptr;
    using std::make_shared;

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
            auto status = pthread_once(&once, Singleton::init);
            if (unlikely(status != 0)) FATAL << "pthread_once invoked error!";
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
}

#endif //REACTOR_SINGLETON_H
