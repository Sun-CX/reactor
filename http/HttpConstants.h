//
// Created by suncx on 2020/9/24.
//

#ifndef REACTOR_HTTPCONSTANTS_H
#define REACTOR_HTTPCONSTANTS_H

#include <string>
#include <unordered_map>

namespace reactor::net::http {
    using std::string;
    using std::unordered_map;

    enum Version {
        INVALID, HTTP1_1, HTTP2
    };
    enum Method {
        ILLEGAL, OPTIONS, HEAD, GET, POST, PUT, DELETE, TRACE, CONNECT
    };
    using Headers = unordered_map<string, string>;
    using Parameters = unordered_map<string, string>;
}

#endif //REACTOR_HTTPCONSTANTS_H