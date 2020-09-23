//
// Created by suncx on 2020/9/23.
//

#ifndef REACTOR_HTTPCONTEXT_H
#define REACTOR_HTTPCONTEXT_H

#include "HttpRequest.h"

class Buffer;

class HttpContext final {
private:
    // 协议解析状态机
    enum HttpParseStatus {
        ExpectRequestLine,
        ExpectHeaders,
        ExpectBody,
        ParseFinished
    };
    HttpRequest request;
    HttpParseStatus parse_status;

    // 解析请求行
    bool parse_request_line(const char *begin, const char *end);

    bool parse_header(const char *begin, const char *end);

public:
    HttpContext();

    bool parse_request(Buffer *buf);

    bool parse_success() const;

    HttpRequest &get_request();

    void reset();
};

#endif //REACTOR_HTTPCONTEXT_H
