//
// Created by suncx on 2020/8/15.
//

#ifndef REACTOR_THREADLOCAL_H
#define REACTOR_THREADLOCAL_H

#include "Ext.h"
#include "ConsoleStream.h"
#include <pthread.h>

namespace reactor::core {
    template<class T>
    class ThreadLocal final : public NonCopyable {
    private:
        pthread_key_t key;

        static void destructor(void *arg) {
            delete static_cast<T *>(arg);
        }

    public:
        ThreadLocal() noexcept {
            int status = pthread_key_create(&key, ThreadLocal::destructor);
            if (unlikely(status != 0)) RC_FATAL << "pthread create key error.";
        }

        T &get_value() {
            T *ptr = static_cast<T *>(pthread_getspecific(key));
            if (ptr == nullptr) {
                T *q = new T();
                int status = pthread_setspecific(key, q);
                if (unlikely(status != 0)) RC_FATAL << "pthread set key error.";
                ptr = q;
            }
            return *ptr;
        }

        ~ThreadLocal() {
            int status = pthread_key_delete(key);
            if (unlikely(status != 0)) RC_FATAL << "pthread delete key error.";
        }
    };
}

#endif //REACTOR_THREADLOCAL_H