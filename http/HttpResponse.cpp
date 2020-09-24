//
// Created by suncx on 2020/9/23.
//

#include "HttpResponse.h"

HttpResponse::HttpResponse() : version(Version::INVALID), response_code(0) {}

void HttpResponse::set_header(const string &name, const string &value) {
    headers[name] = value;
}

void HttpResponse::set_version(Version ver) {
    version = ver;
}

void HttpResponse::set_response_status(int code, const string &desc) {
    response_code = code;
    code_desc = desc;
}

void HttpResponse::set_body(const string &body) {
    response_body = body;
}
