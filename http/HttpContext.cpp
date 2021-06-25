//
// Created by suncx on 2020/9/23.
//

#include "HttpContext.h"
#include "Buffer.h"
#include <algorithm>

using std::find;
using std::to_string;

HttpContext::HttpContext() : parse_status(ExpectRequestLine) {}

bool HttpContext::parse_request_line(const char *begin, const char *end) {
    decltype(begin) start = begin;
    auto space = find(start, end, ' ');
    if (space == end) {
        fprintf(stderr, "bad request line!\n");
        return false;
    }
    auto r = request.set_method(start, space);
    if (!r) {
        fprintf(stderr, "bad request method!\n");
        return false;
    }

    start = space + 1;
    space = find(start, end, ' ');
    if (space == end) {
        fprintf(stderr, "bad request url!\n");
        return false;
    }
    //TODO:parse parameters.
    request.set_path(start, space);

    start = space + 1;
    r = request.set_version(start, end);
    if (!r) {
        fprintf(stderr, "unsupported http version!\n");
        return false;
    }
    return true;
}

bool HttpContext::parse_header(const char *begin, const char *end) {
    auto colo = find(begin, end, ':');
    if (colo == end) {
        fprintf(stderr, "bad http request header!\n");
        return false;
    }
    request.set_header(begin, colo, colo + 1, end);
    return true;
}

bool HttpContext::parse_request(Buffer &buf) {
    bool has_more = true;
    bool ok = true;
    while (has_more) {
        if (parse_status == ExpectRequestLine) { // 解析请求行
            auto pos = buf.find_crlf();
            if (pos) {
                ok = parse_request_line(reinterpret_cast<const char *>(buf.peek()),
                                        reinterpret_cast<const char *>(pos));
                if (ok) {
                    buf.retrieve_until(pos + 2);
                    parse_status = ExpectHeaders;
                }
            } else has_more = false;
        } else if (parse_status == ExpectHeaders) { // 解析请求头
            auto pos = buf.find_crlf();
            if (pos) {
                auto colon = find(buf.peek(), pos, ':');
                if (colon != pos) {
                    parse_header(reinterpret_cast<const char *>(buf.peek()), reinterpret_cast<const char *>(pos));
                } else {
                    parse_status = ParseFinished;
                    has_more = false;
                }
                buf.retrieve_until(pos + 2);
            } else {
                has_more = false;
            }
        } else if (parse_status == ExpectBody) {
            printf("parse request body...\n");
        }
    }
    return ok;
}

bool HttpContext::parse_success() const {
    return parse_status == ParseFinished;
}

HttpRequest &HttpContext::get_request() {
    return request;
}

HttpResponse &HttpContext::get_response() {
    return response;
}

void HttpContext::parse_response(Buffer &buf) {
    string version = response.version == Version::HTTP1_1 ? "HTTP/1.1" : "HTTP/2";
    buf.append((version + ' ').c_str(), version.size() + 1);
    string code = to_string(response.response_code);
    buf.append((code + ' ').c_str(), code.size() + 1);
    buf.append((response.code_desc + "\r\n").c_str(), response.code_desc.size() + 2);

    for (const auto &header : response.headers) {
        string line = header.first + ": " + header.second + "\r\n";
        buf.append(line.c_str(), line.size());
    }
    buf.append("\r\n", 2);
    buf.append(response.response_body.c_str(), response.response_body.size());
}