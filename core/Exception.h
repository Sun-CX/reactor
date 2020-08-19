//
// Created by suncx on 2020/8/8.
//

#ifndef REACTOR_EXCEPTION_H
#define REACTOR_EXCEPTION_H

#include <string>

/**
 * 使用 likely()，执行 if 语句的机会更大
 * 使用 unlikely()，执行 else 语句的机会更大
 * 通过这种方式，编译器在编译过程中，会将可能性更大的代码紧跟着前面的代码，从而减少指令跳转带来的性能上的下降
 */

#define likely(x) __builtin_expect((x), true)       //x 很可能为真
#define unlikely(x) __builtin_expect((x), false)    //x 很可能为假

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

    void fill_stack_trace(bool demangle, int max_frames);

    //TODO: demangle function name...
    string demangle_symbol(const char *symbol) const;

public:
    explicit Exception(string message, bool demangle = false, int max_frames = 128);

    const char *what() const noexcept override;

    const char *stack_trace() const noexcept;
};


#endif //REACTOR_EXCEPTION_H
