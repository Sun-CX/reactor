# reactor
reactor is a network communicating library written in C++ 17.

## Features
enlightened by muduo from chenshuo, reactor has all advantages muduo has and also has
more improvements:
1. purely written in C++ 17, more modernized code style, doesn't require any third part dependencies(boost).
2. fix some vulnerabilities in muduo.
3. redesign Socket, distinguish ServerSocket for listening and Socket for communicating.
4. implement CircularBuffer instead of `boost::circular_buffer`.
5. redesign timer by `QuadHeap` instead of `std::set`.