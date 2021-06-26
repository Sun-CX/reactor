//
// Created by suncx on 2020/9/10.
//

#ifndef REACTOR_HTTPREQUEST_H
#define REACTOR_HTTPREQUEST_H

#include "HttpConstants.h"

using http::Method;
using http::Version;
using http::Headers;
using http::Parameters;

class HttpRequest final {
private:
    friend class HttpContext;

    Method method;
    Version version;
    string path;
    Parameters parameters;
    Headers headers;

    explicit HttpRequest(Method method = Method::ILLEGAL, Version version = Version::INVALID);

    bool set_method(const char *start, const char *end);

    bool set_version(const char *start, const char *end);

    void set_path(const char *start, const char *end);

    void set_header(const char *name_start, const char *name_end,
                    const char *value_start, const char *value_end);

public:
    const string &get_header(const string &name) const;

    const string &get_parameter(const string &name) const;

    const Parameters &get_parameters() const;

    const Headers &get_headers() const;

    Version get_version() const;
};

#endif //REACTOR_HTTPREQUEST_H
