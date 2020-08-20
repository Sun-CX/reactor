//
// Created by suncx on 2020/8/19.
//

#include "Buffer.h"
#include <algorithm>
#include <cstring>
#include <sys/uio.h>
#include <bits/types/struct_iovec.h>

using std::search;
using std::copy;

const char Buffer::CRLF[] = "\r\n";
const int Buffer::prepared_size = 8;
const int Buffer::initial_size = 1024;

Buffer::Buffer(size_t init_size) : buf(prepared_size + init_size), read_idx(prepared_size), write_idx(prepared_size) {}

size_t Buffer::readable_bytes() const {
    return write_idx - read_idx;
}

size_t Buffer::writable_bytes() const {
    return buf.size() - write_idx;
}

size_t Buffer::prepared_bytes() const {
    return read_idx;
}

const char *Buffer::peek() const {
    return begin() + read_idx;
}

const char *Buffer::begin() const {
    return &*buf.cbegin();
}

char *Buffer::begin() {
    return &*buf.begin();
}

const char *Buffer::find_CRLF() const {
    auto pos = search(peek(), begin_write(), CRLF, CRLF + 2);
    return pos == begin_write() ? nullptr : pos;
}

const char *Buffer::find_CRLF(const char *start) const {
    auto pos = search(start, begin_write(), CRLF, CRLF + 2);
    return pos == begin_write() ? nullptr : pos;
}

const char *Buffer::begin_write() const {
    return begin() + write_idx;
}

char *Buffer::begin_write() {
    return begin() + write_idx;
}

const char *Buffer::find_EOL() const {
    auto p = memchr(peek(), '\n', readable_bytes());
    return static_cast<const char *>(p);
}

const char *Buffer::find_EOL(const char *start) const {
    auto p = memchr(start, '\n', readable_bytes());
    return static_cast<const char *>(p);
}

void Buffer::retrieve(size_t n) {
    if (n < readable_bytes()) {
        read_idx += n;
    } else retrieve_all();
}

void Buffer::retrieve_all() {
    read_idx = prepared_size;
    write_idx = prepared_size;
}

void Buffer::retrieve_until(const char *end) {
    retrieve(end - peek());
}

void Buffer::retrieve_64() {
    retrieve(sizeof(int64_t));
}

void Buffer::retrieve_32() {
    retrieve(sizeof(int32_t));
}

void Buffer::retrieve_16() {
    retrieve(sizeof(int16_t));
}

void Buffer::retrieve_8() {
    retrieve(sizeof(int8_t));
}

string Buffer::retrieve_string(size_t n) {
    string res(peek(), n);
    retrieve(n);
    return res;
}

string Buffer::retrieve_all_string() {
    return retrieve_string(readable_bytes());
}

void Buffer::append(const char *data, size_t n) {
    if (writable_bytes() < n) enlarge_space(n);
    copy(data, data + n, begin_write());
    write_idx += n;
}

void Buffer::enlarge_space(size_t n) {
    if (writable_bytes() + prepared_bytes() < n + prepared_size) {
        buf.resize(write_idx + n);
    } else {
        auto read = readable_bytes();
        copy(begin() + read_idx, begin() + write_idx, begin() + prepared_size);
        read_idx = prepared_size;
        write_idx = read_idx + read;
    }
}

void Buffer::append(const void *data, size_t n) {
    append(static_cast<const char *>(data), n);
}

ssize_t Buffer::read_from_fd(int fd, int *err_no) {
    char buf[65536];
    iovec vec[2];
    const size_t writable = writable_bytes();
    vec[0].iov_base = begin() + write_idx;
    vec[0].iov_len = writable;

    vec[1].iov_base = buf;
    vec[1].iov_len = sizeof(buf);

    const int iov_cnt = writable < sizeof(buf) ? 2 : 1;
    const ssize_t n = readv(fd, vec, iov_cnt);
    if (n < 0) *err_no = errno;
    else if (n <= writable) write_idx += n;
    else {
        write_idx = this->buf.size();
        append(buf, n - writable);
    }
    return n;
}

