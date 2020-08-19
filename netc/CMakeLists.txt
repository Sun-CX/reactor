cmake_minimum_required(VERSION 3.15)
project(netc)

set(CMAKE_CXX_STANDARD 14)

add_subdirectory(tests)

set(LIBRARY_SOURCE
        EventLoop.cpp
        Channel.cpp
        Poller.cpp
        EpollPoller.cpp
        InetAddress.cpp
        #        Acceptor.cpp
        Socket.cpp
        #        Timer.cpp TimerId.cpp TimerQueue.cpp
        )

add_library(netc SHARED ${LIBRARY_SOURCE})
target_include_directories(netc PRIVATE ../core)
#target_link_libraries()
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)
