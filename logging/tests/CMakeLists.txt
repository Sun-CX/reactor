cmake_minimum_required(VERSION 3.15)

include_directories(../)
link_directories(${PROJECT_BINARY_DIR}/lib)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

add_executable(sundry-test Sundry-Test.cpp)
target_link_libraries(sundry-test logging)

add_executable(timezone-test TimeZone-Test.cpp)
target_link_libraries(timezone-test logging)