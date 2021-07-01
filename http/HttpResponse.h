//
// Created by suncx on 2020/9/23.
//

#ifndef REACTOR_HTTPRESPONSE_H
#define REACTOR_HTTPRESPONSE_H

#include "HttpConstants.h"

namespace reactor::net {
    using http::Version;
    using http::Headers;
    using std::string;

    class HttpResponse final {
    private:
        friend class HttpContext;

        Version version;
        int response_code;
        string code_desc;
        Headers headers;
        string response_body;
    public:
        HttpResponse();

        void set_version(Version ver);

        void set_response_status(int code, const string &desc);

        void set_header(const string &name, const string &value);

        void set_body(const string &body);
    };
}

#endif //REACTOR_HTTPRESPONSE_H