cmake_minimum_required(VERSION 3.15)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

include_directories(${CMAKE_SOURCE_DIR}/protobuf)
include_directories(${CMAKE_SOURCE_DIR}/core)
include_directories(${CMAKE_SOURCE_DIR}/netc)

add_custom_command(OUTPUT query.pb.h query.pb.cc
        COMMAND protoc ARGS -I=${CMAKE_CURRENT_SOURCE_DIR} --cpp_out=. query.proto
        DEPENDS query.proto
        COMMENT "Compile Google Protocol Buffer Source Files")

add_executable(protobuf-assert Protobuf-Assert.cpp query.pb.h query.pb.cc)
target_link_libraries(protobuf-assert PRIVATE asan protobuf pthread)

add_executable(dispatcherlite-test DispatcherLite-Test.cpp query.pb.h query.pb.cc)
target_link_libraries(dispatcherlite-test PRIVATE asan protobuf reactor-core pthread)

add_executable(dispatcher-test Dispatcher-Test.cpp query.pb.h query.pb.cc)
target_link_libraries(dispatcher-test PRIVATE asan protobuf reactor-core pthread)

add_executable(queryserver-test QueryServer-Test.cpp query.pb.h query.pb.cc)
target_link_libraries(queryserver-test PRIVATE asan protobuf reactor-core netc procodec)