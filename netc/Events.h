//
// Created by suncx on 2020/8/20.
//

#ifndef REACTOR_EVENTS_H
#define REACTOR_EVENTS_H

#include <functional>
#include <memory>
#include "Timestamp.h"

namespace reactor::net {
    using std::function;
    using std::shared_ptr;
    using reactor::core::Timestamp;

    class TcpConnection;

    class Buffer;

    using ConnectionCallback = function<void(const shared_ptr<TcpConnection> &)>;
    using MessageCallback = function<void(const shared_ptr<TcpConnection> &, Timestamp)>;
    using WriteCompleteCallback = function<void(const shared_ptr<TcpConnection> &)>;
    using HighWaterMarkCallback = function<void(const shared_ptr<TcpConnection> &, size_t)>;
    using CloseCallback = function<void(const shared_ptr<TcpConnection> &)>;
}

#endif //REACTOR_EVENTS_H
