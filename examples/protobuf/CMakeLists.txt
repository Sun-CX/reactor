cmake_minimum_required(VERSION 3.15)

add_executable(proto-test ProtoCodec.cpp)
target_include_directories(proto-test PRIVATE
        ${CMAKE_SOURCE_DIR}/core
        ${CMAKE_SOURCE_DIR}/netc)
target_link_directories(proto-test PRIVATE ${CMAKE_BINARY_DIR}/lib)
target_link_libraries(proto-test PRIVATE reactor-core netc protobuf)