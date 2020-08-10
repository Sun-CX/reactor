//
// Created by ysm on 8/10/20.
//

#ifndef REACTOR_SINGLETON_H
#define REACTOR_SINGLETON_H

#include <memory>
#include <pthread.h>

template <typename T>
class Singleton {
public:
    static std::shared_ptr<T> instance()
    {
        pthread_once(&once_control, &Singleton::init);
        return obj;
    }
private:
    static void init()
    {
        obj = std::make_shared<T>();
    }
private:
    static pthread_once_t once_control;
    static std::shared_ptr<T> obj;
};

template <typename T>
pthread_once_t Singleton<T>::once_control = PTHREAD_ONCE_INIT;

template <typename T>
std::shared_ptr<T> Singleton<T>::obj = nullptr;


#endif //REACTOR_SINGLETON_H
