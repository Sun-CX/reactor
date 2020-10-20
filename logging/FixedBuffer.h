//
// Created by suncx on 2020/10/20.
//

#ifndef REACTOR_FIXEDBUFFER_H
#define REACTOR_FIXEDBUFFER_H

#include "StringPiece.h"
#include "NonCopyable.h"

template<const size_t size>
class FixedBuffer final : public NonCopyable {
private:
    char data[size];
    char *cur;

    void (*cookie)();

    [[nodiscard]]
    const char *end() const {
        return data + size;
    }

    static void cookie_start() {

    }

    static void cookie_end() {

    }

public:
    FixedBuffer() : cur(data), cookie(cookie_start) {}

    virtual ~FixedBuffer() {
        set_cookie(cookie_end);
    }

    void set_cookie(void (*c)()) {
        cookie = c;
    }

    void append(const char *buf, size_t len) {
        if (len < available()) {
            memcpy(cur, buf, len);
            cur += len;
        }
    }

    [[nodiscard]]
    size_t available() const {
        return end() - cur;
    }

    [[nodiscard]]
    const char *get_data() const { return data; }

    [[nodiscard]]
    size_t length() const { return cur - data; }

    [[nodiscard]]
    char *current() const { return cur; }

    void add(size_t len) { cur += len; }

    void reset() { cur = data; }

    void zero() { memset(data, 0, sizeof(data)); }

    [[nodiscard]]
    StringPiece to_string_piece() const { return StringPiece(data, length()); }
};

#endif //REACTOR_FIXEDBUFFER_H
