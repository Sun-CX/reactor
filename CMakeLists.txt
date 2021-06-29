cmake_minimum_required(VERSION 3.15)
project(reactor CXX)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(core)
add_subdirectory(netc)
add_subdirectory(http)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address")

if (NOT ${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    message(FATAL_ERROR "${PROJECT_NAME} only supports Linux OS for now.")
endif ()

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif ()

message(STATUS "Current build type: ${CMAKE_BUILD_TYPE}")
if (${CMAKE_BUILD_TYPE} STREQUAL "Release")
    add_definitions(-Wall -Wextra -Wdeprecated -O2)
endif ()