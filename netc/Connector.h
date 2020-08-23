//
// Created by suncx on 2020/8/23.
//

#ifndef REACTOR_CONNECTOR_H
#define REACTOR_CONNECTOR_H

#include "NonCopyable.h"
#include "InetAddress.h"
#include <memory>
#include <functional>

using std::unique_ptr;
using std::enable_shared_from_this;
using std::function;

class EventLoop;

class Channel;

/**
 * 对主动连接套接字的封装
 */
class Connector final : public NonCopyable, public enable_shared_from_this<Connector> {
private:
    using NewConnectionCallback = function<void(int)>;
    enum STATUS {
        Disconnected, Connecting, Connected
    };

    EventLoop *loop;
    InetAddress server_addr;
    STATUS status;
    unique_ptr<Channel> channel;
    NewConnectionCallback callback;
    int retry_delay_ms;
public:
    Connector(EventLoop *loop, const InetAddress &svr_addr);

    void set_new_conn_callback(const NewConnectionCallback &cb);

    void start();

    void restart();

    void stop();

    const InetAddress &get_server_addr() const;
};

#endif //REACTOR_CONNECTOR_H
