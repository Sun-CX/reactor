//
// Created by suncx on 2020/9/24.
//

#ifndef REACTOR_HTTPCONSTANTS_H
#define REACTOR_HTTPCONSTANTS_H

#include <string>
#include <unordered_map>

using std::string;
using std::unordered_map;

namespace http {
    enum Version {
        INVALID, HTTP1_1, HTTP2
    };
    enum Method {
        ILLEGAL, GET, HEAD, POST, PUT, DELETE, OPTIONS
    };
    using Headers = unordered_map<string, string>;
    using Parameters = unordered_map<string, string>;
}
#endif //REACTOR_HTTPCONSTANTS_H
