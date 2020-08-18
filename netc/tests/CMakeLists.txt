cmake_minimum_required(VERSION 3.15)

set(CMAKE_CXX_STANDARD 14)

include_directories(../ ../../core)
link_directories(${PROJECT_BINARY_DIR}/lib)

add_executable(eventloopdonothing-test EventLoopDoNothing-Test.cpp)
target_link_libraries(eventloopdonothing-test reactor-core netc)