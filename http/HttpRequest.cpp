//
// Created by suncx on 2020/9/10.
//

#include "HttpRequest.h"
#include <cassert>

HttpRequest::HttpRequest(HttpRequest::Method method, HttpRequest::Version version) : method(method), version(version) {}

bool HttpRequest::set_method(const char *start, const char *end) {
    assert(method == UNKNOWN);
    string me(start, end);
    if (me == "GET") method = GET;
    else if (me == "HEAD") method = HEAD;
    else if (me == "POST") method = POST;
    else if (me == "PUT") method = PUT;
    else if (me == "DELETE") method = DELETE;
    else if (me == "OPTIONS") method = OPTIONS;
    return method != UNKNOWN;
}

bool HttpRequest::set_version(const char *start, const char *end) {
    assert(version == INVALID);
    string v(start, end);
    if (v == "HTTP/1.1") version = HTTP1_1;
    else if (v == "HTTP/2") version = HTTP2;
    return version != INVALID;
}

void HttpRequest::set_path(const char *start, const char *end) {
    path.assign(start, end);
}

void
HttpRequest::add_header(const char *name_start, const char *name_end, const char *value_start, const char *value_end) {
    string name(name_start, name_end);
    string value(value_start, value_end);
    headers[name] = value;
}

void HttpRequest::reset() {
    method = UNKNOWN;
    version = INVALID;
    path = "";
    parameters.clear();
    headers.clear();
}

const string &HttpRequest::get_header(const string &name) const {
    return headers.at(name);
}
