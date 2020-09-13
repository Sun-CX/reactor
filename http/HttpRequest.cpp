//
// Created by suncx on 2020/9/10.
//

#include "HttpRequest.h"
#include <cassert>

HttpRequest::HttpRequest(HttpRequest::Method method, HttpRequest::Version version) : method(method), version(version) {}

void HttpRequest::set_method(const char *start, const char *end) {
    assert(method == UNKNOWN);
    string me(start, end);
    if (me == "GET") method = GET;
    else if (me == "HEAD") method = HEAD;
    else if (me == "POST") method = POST;
    else if (me == "PUT") method = PUT;
    else if (me == "DELETE") method = DELETE;
    else if (me == "OPTIONS") method = OPTIONS;
}

void HttpRequest::set_version(const char *start, const char *end) {
    assert(version == INVALID);
    string v(start, end);
    if (v == "HTTP/1.1") version = HTTP1_1;
    else if (v == "HTTP/2") version = HTTP2;
}

void HttpRequest::set_path(const char *start, const char *end) {
    path.assign(start, end);
}
