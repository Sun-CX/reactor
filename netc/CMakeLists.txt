cmake_minimum_required(VERSION 3.15)

add_subdirectory(tests)

set(LIBRARY_SOURCE
        EventLoop.cpp
        Channel.cpp
        Poller.cpp
        PollPoller.cpp
        EpollPoller.cpp
        InetAddress.cpp
        ServerSocket.cpp
        Socket.cpp
        Acceptor.cpp
        Buffer.cpp
        TcpConnection.cpp
        TcpServer.cpp
        EventLoopThread.cpp
        EventLoopThreadPool.cpp
        TimerTask.cpp
        Timer.cpp
        MinHeap.h
        #        Connector.cpp
        #        TcpClient.cpp
        )

add_library(netc SHARED ${LIBRARY_SOURCE})
set_target_properties(netc PROPERTIES
        VERSION 1.0
        SOVERSION 1
        LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib
        )
target_include_directories(netc PRIVATE ${PROJECT_SOURCE_DIR}/core)
target_link_directories(netc PRIVATE ${PROJECT_BINARY_DIR}/lib)
target_link_libraries(netc PRIVATE reactor-core)
