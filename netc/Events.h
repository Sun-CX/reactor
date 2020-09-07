//
// Created by suncx on 2020/8/20.
//

#ifndef REACTOR_EVENTS_H
#define REACTOR_EVENTS_H

#include <functional>
#include <memory>

using std::function;
using std::shared_ptr;

class TcpConnection;
class Buffer;
class Timestamp;

using ConnectionCallback = function<void(const shared_ptr<TcpConnection> &)>;
using MessageCallback = function<void(const shared_ptr<TcpConnection> &, Buffer *, Timestamp)>;
using WriteCompleteCallback = function<void(const shared_ptr<TcpConnection> &)>;
using HighWaterMarkCallback = function<void(const shared_ptr<TcpConnection> &, size_t)>;
using CloseCallback = function<void(const shared_ptr<TcpConnection> &)>;

#endif //REACTOR_EVENTS_H
