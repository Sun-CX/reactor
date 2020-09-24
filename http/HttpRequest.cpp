//
// Created by suncx on 2020/9/10.
//

#include "HttpRequest.h"
#include <cassert>

HttpRequest::HttpRequest(Method method, Version version) : method(method), version(version) {}

bool HttpRequest::set_method(const char *start, const char *end) {
    assert(method == Method::ILLEGAL);
    string me(start, end);
    if (me == "GET") method = Method::GET;
    else if (me == "HEAD") method = Method::HEAD;
    else if (me == "POST") method = Method::POST;
    else if (me == "PUT") method = Method::PUT;
    else if (me == "DELETE") method = Method::DELETE;
    else if (me == "OPTIONS") method = Method::OPTIONS;
    return method != Method::ILLEGAL;
}

bool HttpRequest::set_version(const char *start, const char *end) {
    assert(version == Version::INVALID);
    string v(start, end);
    if (v == "HTTP/1.1") version = Version::HTTP1_1;
    else if (v == "HTTP/2") version = Version::HTTP2;
    return version != Version::INVALID;
}

void HttpRequest::set_path(const char *start, const char *end) {
    path.assign(start, end);
}

void
HttpRequest::set_header(const char *name_start, const char *name_end, const char *value_start, const char *value_end) {
    string name(name_start, name_end);
    string value(value_start, value_end);
    headers[name] = value;
}

const string &HttpRequest::get_header(const string &name) const {
    return headers.at(name);
}

const string &HttpRequest::get_parameter(const string &name) const {
    return parameters.at(name);
}

Version HttpRequest::get_version() const {
    return version;
}
