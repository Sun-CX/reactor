//
// Created by suncx on 2020/8/15.
//

#ifndef REACTOR_THREADLOCAL_H
#define REACTOR_THREADLOCAL_H

#include <pthread.h>
#include "NonCopyable.h"
#include "Exception.h"

template<class T>
class ThreadLocal final : public NonCopyable {
private:
    pthread_key_t key;

    static void destructor(void *arg) {
        delete static_cast<T *>(arg);
    }

public:
    ThreadLocal() {
        int status = pthread_key_create(&key, ThreadLocal::destructor);
        if (unlikely(status != 0)) ERROR_EXIT("error occurred.");
    }

    T &get_value() {
        T *ptr = static_cast<T *>(pthread_getspecific(key));
        if (ptr == nullptr) {
            T *q = new T();
            int status = pthread_setspecific(key, q);
            if (unlikely(status != 0)) ERROR_EXIT("error occurred.");
            ptr = q;
        }
        return *ptr;
    }

    virtual ~ThreadLocal() {
        int status = pthread_key_delete(key);
        if (unlikely(status != 0)) ERROR_EXIT("error occurred.");
    }
};

#endif //REACTOR_THREADLOCAL_H
