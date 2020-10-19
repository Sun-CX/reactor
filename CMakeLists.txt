cmake_minimum_required(VERSION 3.15)
project(reactor CXX)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(core)
add_subdirectory(logging)
add_subdirectory(netc)
add_subdirectory(http)

if (NOT ${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    message(FATAL_ERROR "${PROJECT_NAME} only supports Linux OS for now.")
endif ()

message(STATUS "Current build type: ${CMAKE_BUILD_TYPE}")
if (${CMAKE_BUILD_TYPE} STREQUAL "Release")
    add_definitions(-Wall -Wextra -Wdeprecated -O2)
endif ()