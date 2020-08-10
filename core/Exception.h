//
// Created by suncx on 2020/8/8.
//

#ifndef REACTOR_EXCEPTION_H
#define REACTOR_EXCEPTION_H

#include <exception>
#include <string>
#include <execinfo.h>

using std::string;
using std::exception;
using std::move;

class Exception : public exception {
private:
    string message;
    string stacks;

    void fill_stack_trace(bool demangle, int max_frames);

    string demangle_symbol(const char *symbol) const;

public:
    explicit Exception(string message, bool demangle = false, int max_frames = 128);

    const char *what() const noexcept override;

    const char *stack_trace() const noexcept;
};


#endif //REACTOR_EXCEPTION_H
