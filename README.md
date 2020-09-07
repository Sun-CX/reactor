# reactor
reactor is a network communicating library written in C++11.

## Features
enlightened by muduo from chenshuo, reactor has all advantages muduo has and also has
more improvements:
1. purely written in C++ 11, more modernized code style, don't require any third dependencies(boost).
2. fix some bugs in muduo.
3. redesign Socket, implement CircularBuffer instead of `boost::circular_buffer`.
4. redesign timer by `QuadHeap`, instead of `std::set`.