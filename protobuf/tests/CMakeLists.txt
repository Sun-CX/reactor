cmake_minimum_required(VERSION 3.15)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_custom_command(OUTPUT query.pb.h query.pb.cc
        COMMAND protoc ARGS -I=${CMAKE_CURRENT_SOURCE_DIR} --cpp_out=. query.proto
        DEPENDS query.proto
        COMMENT "Compile Google Protocol Buffer Source Files")
add_executable(protobuf-assert Protobuf-Assert.cpp query.pb.h query.pb.cc)
target_link_libraries(protobuf-assert PRIVATE asan protobuf pthread)