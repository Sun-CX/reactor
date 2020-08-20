cmake_minimum_required(VERSION 3.15)

set(CMAKE_CXX_STANDARD 14)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

include_directories(${PROJECT_SOURCE_DIR}/core ${PROJECT_SOURCE_DIR}/netc)
link_directories(${PROJECT_BINARY_DIR}/lib)

add_executable(eventloop-test EventLoop-Test.cpp)
target_link_libraries(eventloop-test netc)

add_executable(timerfd-test timerfd-Test.cpp)

add_executable(inetaddress-test InetAddress-Test.cpp)
target_link_libraries(inetaddress-test netc)

add_executable(echoserver-test EchoServer-Test.cpp)
target_link_libraries(echoserver-test netc)

add_executable(buffer-test Buffer-Test.cpp)
target_link_libraries(buffer-test netc)
