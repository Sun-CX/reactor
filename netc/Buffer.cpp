//
// Created by suncx on 2020/8/19.
//

#include "Buffer.h"
#include <algorithm>
#include <cstring>
#include <sys/uio.h>
#include <cassert>
#include <netinet/in.h>

using std::search;
using std::copy;
using reactor::net::Buffer;
using reactor::net::byte;
using std::string;

const char Buffer::CRLF[] = "\r\n";
const int Buffer::RESERVED_SIZE = 8;
const int Buffer::INITIAL_SIZE = 1024;

Buffer::Buffer(size_t init_size) :
        buf(static_cast<byte *>(::malloc(sizeof(byte) * (RESERVED_SIZE + init_size)))),
        read_idx(RESERVED_SIZE),
        write_idx(RESERVED_SIZE),
        capacity(RESERVED_SIZE + init_size) {
    assert(init_size > 0);
    assert(buf != nullptr);
    assert(readable_bytes() == 0);
    assert(writable_bytes() == init_size);
    assert(reserved_bytes() == RESERVED_SIZE);
}

Buffer::~Buffer() {
    assert(buf != nullptr);
    ::free(buf);
    read_idx = write_idx = capacity = 0;
}

size_t Buffer::readable_bytes() const {
    return write_idx - read_idx;
}

size_t Buffer::writable_bytes() const {
    return capacity - write_idx;
}

size_t Buffer::reserved_bytes() const {
    return read_idx;
}

const byte *Buffer::peek() const {
    return begin() + read_idx;
}

uint16_t Buffer::peek_uint16() const {
    assert(readable_bytes() >= sizeof(uint16_t));
    uint16_t val;
    memcpy(&val, peek(), sizeof(val));
    return ntohs(val);
}

uint32_t Buffer::peek_uint32() const {
    assert(readable_bytes() >= sizeof(uint32_t));
    uint32_t val;
    memcpy(&val, peek(), sizeof(val));
    return ntohl(val);
}

const byte *Buffer::begin() const {
    return buf;
}

byte *Buffer::begin() {
    return buf;
}

const byte *Buffer::find_crlf() const {
    auto pos = search(peek(), begin_write(), CRLF, CRLF + 2);
    return pos == begin_write() ? nullptr : pos;
}

const byte *Buffer::find_crlf(const byte *start) const {
    auto pos = search(start, begin_write(), CRLF, CRLF + 2);
    return pos == begin_write() ? nullptr : pos;
}

const byte *Buffer::begin_write() const {
    return begin() + write_idx;
}

byte *Buffer::begin_write() {
    return begin() + write_idx;
}

const byte *Buffer::find_eol() const {
    auto p = memchr(peek(), '\n', readable_bytes());
    return static_cast<const byte *>(p);
}

const byte *Buffer::find_eol(const byte *start) const {
    auto p = memchr(start, '\n', readable_bytes());
    return static_cast<const byte *>(p);
}

void Buffer::retrieve(size_t n) {
    if (n < readable_bytes()) {
        read_idx += n;
    } else retrieve_all();
}

void Buffer::retrieve_all() {
    read_idx = RESERVED_SIZE;
    write_idx = RESERVED_SIZE;
}

void Buffer::retrieve_until(const byte *end) {
    retrieve(end - peek());
}

string Buffer::retrieve_string(size_t n) {
    string res(reinterpret_cast<const char *>(peek()), n);
    retrieve(n);
    return res;
}

string Buffer::retrieve_all_string() {
    return retrieve_string(readable_bytes());
}

void Buffer::ensure_writable(size_t n) {
    if (reserved_bytes() + writable_bytes() < n + RESERVED_SIZE) {
        size_t need = n + RESERVED_SIZE - (reserved_bytes() + writable_bytes());
        buf = static_cast<byte *>(::realloc(buf, capacity + need));
        assert(buf != nullptr);
        capacity += need;
    }
    auto readable = readable_bytes();
    copy(begin() + read_idx, begin() + write_idx, begin() + RESERVED_SIZE);
    read_idx = RESERVED_SIZE;
    write_idx = read_idx + readable;
}

void Buffer::append(const byte *data, size_t n) {
    if (writable_bytes() < n)
        ensure_writable(n);
    copy(data, data + n, begin_write());
    write_idx += n;
}

void Buffer::append(const void *data, size_t n) {
    append(static_cast<const byte *>(data), n);
}

void Buffer::append(const string &msg) {
    append(msg.c_str(), msg.size());
}

ssize_t Buffer::read_from_fd(int fd, int *err_no) {
    byte data[65536];
    iovec vec[2];
    const size_t writable = writable_bytes();
    vec[0].iov_base = begin() + write_idx;
    vec[0].iov_len = writable;

    vec[1].iov_base = data;
    vec[1].iov_len = sizeof(data);

    const int iov_cnt = writable < sizeof(data) ? 2 : 1;
    const ssize_t n = readv(fd, vec, iov_cnt);
    if (n < 0) *err_no = errno;
    else if (n <= writable) write_idx += n;
    else {
        write_idx = capacity;
        append(data, n - writable);
    }
    return n;
}