cmake_minimum_required(VERSION 3.15)

set(CMAKE_CXX_STANDARD 14)

add_subdirectory(tests)

set(LIBRARY_SOURCE
        #        ============== reactor ==============
        EventLoop.cpp
        Channel.cpp
        Poller.cpp
        PollPoller.cpp
        #        =============== tcp server ===============
        InetAddress.cpp
        Socket.cpp
        Acceptor.cpp

        Buffer.cpp
        TcpConnection.cpp
        TcpServer.cpp
        EventLoopThread.cpp
        EventLoopThreadPool.cpp
        Timer.cpp
        TimerId.cpp
        TimerQueue.cpp
        #        Connector.cpp
        )

set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

include_directories(${PROJECT_SOURCE_DIR}/core)
link_directories(${PROJECT_BINARY_DIR}/lib)

add_library(netc SHARED ${LIBRARY_SOURCE})
target_link_libraries(netc reactor-core)
