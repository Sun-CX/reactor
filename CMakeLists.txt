cmake_minimum_required(VERSION 3.15)
project(reactor)

set(CMAKE_CXX_STANDARD 14)

#add_definitions(-Wall -Wextra -Wdeprecated)
#add_compile_definitions(NDEBUG)

add_subdirectory(core)
add_subdirectory(netc)