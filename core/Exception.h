//
// Created by suncx on 2020/8/8.
//

#ifndef REACTOR_EXCEPTION_H
#define REACTOR_EXCEPTION_H

#include <string>

#define ERROR_EXIT(e)\
do{\
    fprintf(stderr,"[%s: %s: line: %d]: %s\n", __FILE__, __PRETTY_FUNCTION__, __LINE__, (e));\
    exit(errno);\
} while(0)

using std::string;
using std::exception;

class Exception : public exception {
private:
    string message;
    string stacks;

    void fill_stack_trace(int max_frames);

    //TODO: demangle function name...
    //string demangle_symbol(const char *symbol) const;

public:
    explicit Exception(string message, int max_frames = 128);

    [[nodiscard]]
    const char *what() const noexcept override;

    [[nodiscard]]
    const char *stack_trace() const noexcept;
};

#endif //REACTOR_EXCEPTION_H
