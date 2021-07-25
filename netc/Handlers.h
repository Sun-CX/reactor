//
// Created by suncx on 2020/8/20.
//

#ifndef REACTOR_HANDLERS_H
#define REACTOR_HANDLERS_H

#include <functional>
#include <memory>
#include "Timestamp.h"

namespace reactor::net {
    using std::function;
    using std::shared_ptr;
    using reactor::core::Timestamp;

    class TcpConnection;

    using ConnectionHandler = function<void(const shared_ptr<TcpConnection> &)>;
    using DataHandler = function<void(const shared_ptr<TcpConnection> &, Timestamp)>;
    using WriteCompleteHandler = function<void(const shared_ptr<TcpConnection> &)>;
    // using HighWaterMarkHandler = function<void(const shared_ptr<TcpConnection> &, size_t)>;
    using CloseHandler = function<void(const shared_ptr<TcpConnection> &)>;
}

#endif //REACTOR_HANDLERS_H
