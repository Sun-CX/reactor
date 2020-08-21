cmake_minimum_required(VERSION 3.15)

set(CMAKE_CXX_STANDARD 14)

add_subdirectory(tests)

set(LIBRARY_SOURCE
        EventLoop.cpp
        Channel.cpp
        Poller.cpp
        PollPoller.cpp
        #        InetAddress.cpp
        #        Acceptor.cpp
        #        Socket.cpp
        #        TcpServer.cpp
        #        TcpConnection.cpp
        #        Buffer.cpp
        #        Events.h
        #        EventLoopThread.cpp
        #        EventLoopThreadPool.cpp
        #        Timer.cpp TimerId.cpp TimerQueue.cpp
        )

set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

include_directories(${PROJECT_SOURCE_DIR}/core)
link_directories(${PROJECT_BINARY_DIR}/lib)

add_library(netc SHARED ${LIBRARY_SOURCE})
target_link_libraries(netc reactor-core)
