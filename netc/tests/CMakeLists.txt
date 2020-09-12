cmake_minimum_required(VERSION 3.15)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

include_directories(
        ${PROJECT_SOURCE_DIR}/core
        ${PROJECT_SOURCE_DIR}/logging
        ${PROJECT_SOURCE_DIR}/netc
)
link_directories(${PROJECT_BINARY_DIR}/lib)

add_executable(channel-test Channel-Test.cpp)
target_link_libraries(channel-test netc)

add_executable(eventloop-test EventLoop-Test.cpp)
target_link_libraries(eventloop-test netc)

add_executable(timerfd-test timerfd-Test.cpp)

add_executable(inetaddress-test InetAddress-Test.cpp)
target_link_libraries(inetaddress-test netc)

add_executable(echoserver-test EchoServer-Test.cpp)
target_link_libraries(echoserver-test netc)

add_executable(buffer-test Buffer-Test.cpp)
target_link_libraries(buffer-test netc)

add_executable(acceptor-test Acceptor-Test.cpp)
target_link_libraries(acceptor-test netc)

add_executable(timer-test Timer-Test.cpp)
target_link_libraries(timer-test netc)

add_executable(eventloopthread-test EventLoopThread-Test.cpp)
target_link_libraries(eventloopthread-test netc)

add_executable(minheap-test MinHeap-Test.cpp)
target_link_libraries(minheap-test reactor-core)