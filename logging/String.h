//
// Created by suncx on 2020/10/20.
//

#ifndef REACTOR_STRING_H
#define REACTOR_STRING_H

#include <cstddef>

class String final {
private:
    const char *data;
    size_t l;
public:
    String(const char *str, size_t len);

    [[nodiscard]]
    const char *get_data() const;

    [[nodiscard]]
    size_t len() const;
};

#endif //REACTOR_STRING_H
