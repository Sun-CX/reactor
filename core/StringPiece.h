// From PCRE pcre_stringpiece.h
// ========================================================================
// Copyright (c) 2005, Google Inc.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//     * Neither the name of Google Inc. nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Author: Sanjay Ghemawat
//
// A string like object that points into another piece of memory.
// Useful for providing an interface that allows clients to easily
// pass in either a "const char*" or a "string".
//
// Arghh!  I wish C++ literals were automatically of type "string".

#ifndef REACTOR_STRINGPIECE_H
#define REACTOR_STRINGPIECE_H

#include <string>
#include <cstring>
#include <iosfwd>

using std::string;
using std::ostream;

// ====================== Not From PCRE pcre_stringpiece.h ======================
class StringArg final {
private:
    const char *str;
public:
    explicit StringArg(const char *str) : str(str) {}

    explicit StringArg(const string &str) : str(str.c_str()) {}

    const char *c_str() const { return str; }
};

// ====================== From PCRE pcre_stringpiece.h ======================
class StringPiece final {
private:
    const char *ptr;
    int length;
public:
    StringPiece() : ptr(nullptr), length(0) {}

    explicit StringPiece(const char *str) : ptr(str), length(static_cast<int>(strlen(str))) {}

    explicit StringPiece(const unsigned char *str) : ptr(reinterpret_cast<const char *>(str)),
                                                     length(static_cast<int>(strlen(ptr))) {}

    explicit StringPiece(const string &str) : ptr(str.data()), length(static_cast<int>(str.size())) {}

    StringPiece(const char *offset, int len) : ptr(offset), length(len) {}

    const char *data() const { return ptr; }

    int size() const { return length; }

    bool empty() const { return length == 0; }

    void clear() { ptr = nullptr, length = 0; }

    void set(const char *buffer, int len) { ptr = buffer, length = len; }

    void set(const char *str) { ptr = str, length = static_cast<int>(strlen(str)); }

    void set(const void *buffer, int len) { ptr = reinterpret_cast<const char *>(buffer), length = len; }

    char operator[](int i) const { return ptr[i]; }

    void remove_prefix(int n) { ptr += n, length -= n; }

    void remove_suffix(int n) { length -= n; }

    bool operator==(const StringPiece &piece) const {
        return length == piece.length and memcmp(ptr, piece.ptr, length) == 0;
    }

    bool operator!=(const StringPiece &piece) const {
        return !(*this == piece);
    }

#define STRINGPIECE_BINARY_PREDICATE(cmp, auxcmp)\
    bool operator cmp (const StringPiece &piece) const {\
        int r = memcmp(ptr, piece.ptr, length < piece.length ? length : piece.length);\
        return ((r auxcmp 0) or ((r == 0) and (length cmp piece.length)));\
    }

    STRINGPIECE_BINARY_PREDICATE(<, <);
    STRINGPIECE_BINARY_PREDICATE(<=, <);
    STRINGPIECE_BINARY_PREDICATE(>=, >);
    STRINGPIECE_BINARY_PREDICATE(>, >);
#undef STRINGPIECE_BINARY_PREDICATE

    int compare(const StringPiece &piece) const {
        int r = memcmp(ptr, piece.ptr, length < piece.length ? length : piece.length);
        if (r == 0) {
            if (length < piece.length) r = -1;
            else if (length > piece.length) r = 1;
        }
        return r;
    }

    string as_string() const { return string(data(), size()); }

    void copy_to_string(string *target) const { target->assign(ptr, length); }

    bool starts_with(const StringPiece &piece) const {
        return length >= piece.length and memcmp(ptr, piece.ptr, piece.length) == 0;
    }
};

// allow StringPiece to be logged
ostream &operator<<(ostream &os, const StringPiece &piece);

#endif //REACTOR_STRINGPIECE_H
