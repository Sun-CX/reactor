cmake_minimum_required(VERSION 3.15)

include_directories(${PROJECT_SOURCE_DIR}/logging)
link_directories(${PROJECT_BINARY_DIR}/lib)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

add_executable(logstream-test LogStream-Test.cpp)
target_link_libraries(logstream-test logging)

add_executable(timezone-test TimeZone-Test.cpp)
target_link_libraries(timezone-test logging)