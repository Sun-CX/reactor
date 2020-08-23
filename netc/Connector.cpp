//
// Created by suncx on 2020/8/23.
//

#include "Connector.h"

Connector::Connector(EventLoop *loop, const InetAddress &svr_addr) : loop(loop), server_addr(svr_addr),
                                                                     status(Disconnected), retry_delay_ms(500) {}

void Connector::set_new_conn_callback(const Connector::NewConnectionCallback &cb) {
    callback = cb;
}

const InetAddress &Connector::get_server_addr() const {
    return server_addr;
}
