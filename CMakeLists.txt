cmake_minimum_required(VERSION 3.15)
project(reactor CXX)

set(CMAKE_CXX_STANDARD 14)

add_subdirectory(core)
add_subdirectory(netc)

if (NOT ${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    message(FATAL_ERROR "${PROJECT_NAME} library only supports Linux OS for now.")
endif ()

#sudo apt-get install libxcb-xinerama0

message(STATUS "Current build type: ${CMAKE_BUILD_TYPE}")
if (${CMAKE_BUILD_TYPE} STREQUAL "Release")
    add_compile_definitions(NDEBUG)
    add_definitions(-Wall -Wextra -Wdeprecated -O2)
endif ()
