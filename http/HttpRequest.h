//
// Created by suncx on 2020/9/10.
//

#ifndef REACTOR_HTTPREQUEST_H
#define REACTOR_HTTPREQUEST_H

#include <string>
#include <unordered_map>

using std::string;
using std::unordered_map;

class HttpRequest final {
public:
    enum Method {
        UNKNOWN, GET, HEAD, POST, PUT, DELETE, OPTIONS
    };

    enum Version {
        INVALID, HTTP1_1, HTTP2
    };
private:
    using Headers = unordered_map<string, string>;

    Method method;
    Version version;
    string path;
    Headers headers;
public:
    explicit HttpRequest(Method method = UNKNOWN, Version version = INVALID);

    void set_method(const char *start, const char *end);

    void set_version(const char *start, const char *end);

    void set_path(const char *start, const char *end);
};

#endif //REACTOR_HTTPREQUEST_H
