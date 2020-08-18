cmake_minimum_required(VERSION 3.15)
project(netc)

set(CMAKE_CXX_STANDARD 14)

add_subdirectory(tests)

set(LIBRARY_SOURCE
        EventLoop.cpp
#        Channel.cpp
#        Poller.cpp
#        Epoller.cpp
#        InetAddress.cpp
#        Acceptor.cpp
#        Socket.cpp
        )

add_library(netc SHARED ${LIBRARY_SOURCE})
target_include_directories(netc PRIVATE ../core)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)
