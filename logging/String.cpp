//
// Created by suncx on 2020/10/20.
//

#include "String.h"
#include <cstring>
#include <cassert>

String::String(const char *str, size_t len) : data(str), l(len) {
    assert(strlen(data) == l);
}

const char *String::get_data() const {
    return data;
}

size_t String::len() const {
    return l;
}

