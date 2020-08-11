//
// Created by suncx on 2020/8/8.
//

#include "Exception.h"

Exception::Exception(string message, bool demangle, int max_frames) : message(move(message)) {
    fill_stack_trace(demangle, max_frames);
}

void Exception::fill_stack_trace(bool demangle, int max_frames) {
    void *frames[max_frames];
    int n = backtrace(frames, max_frames);
    char **msgs = backtrace_symbols(frames, n);
    if (msgs != nullptr) {
        for (int i = 0; i < n; ++i) {
            stacks.append(msgs[i]);
            stacks.push_back('\n');
        }
        free(msgs);
    }
}

const char *Exception::what() const noexcept {
    return message.c_str();
}

const char *Exception::stack_trace() const noexcept {
    return stacks.c_str();
}
