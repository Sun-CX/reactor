cmake_minimum_required(VERSION 3.15)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

include_directories(
        ${PROJECT_SOURCE_DIR}/core
        ${PROJECT_SOURCE_DIR}/netc
)
link_directories(${PROJECT_BINARY_DIR}/lib)

add_executable(channel-test Channel-Test.cpp)
target_link_libraries(channel-test PRIVATE netc reactor-core)

add_executable(eventloop-test EventLoop-Test.cpp)
target_link_libraries(eventloop-test PRIVATE asan netc reactor-core)

add_executable(inetaddress-test InetAddress-Test.cpp)
target_link_libraries(inetaddress-test PRIVATE asan netc reactor-core)

add_executable(echoserver-test EchoServer-Test.cpp)
target_link_libraries(echoserver-test asan netc reactor-core)

add_executable(echoserver-benchmark EchoServer-Benchmark.cpp)
target_link_libraries(echoserver-benchmark reactor-core)

add_executable(buffer-test Buffer-Test.cpp)
target_link_libraries(buffer-test PRIVATE asan netc reactor-core)

add_executable(acceptor-test Acceptor-Test.cpp)
target_link_libraries(acceptor-test PRIVATE netc reactor-core)

add_executable(eventloopthread-test EventLoopThread-Test.cpp)
target_link_libraries(eventloopthread-test PRIVATE netc reactor-core)

add_executable(minheap-test MinHeap-Test.cpp)
target_link_libraries(minheap-test reactor-core)

add_executable(socketopt-test SocketOpt-Test.cpp)
target_link_libraries(socketopt-test PRIVATE reactor-core)

add_executable(timerheap-test TimerHeap-Test.cpp)
target_link_libraries(timerheap-test PRIVATE asan netc reactor-core)

add_executable(timer-test Timer-Test.cpp)
target_link_libraries(timer-test PRIVATE asan netc reactor-core)

add_executable(timerfd-test timerfd-Test.cpp)
target_link_libraries(timerfd-test PRIVATE reactor-core)

add_executable(connector-test Connector-Test.cpp)
target_link_libraries(connector-test PRIVATE asan reactor-core netc)

add_executable(echoclient-test EchoClient-Test.cpp)
target_link_libraries(echoclient-test PRIVATE asan reactor-core netc)