cmake_minimum_required(VERSION 3.15)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

include_directories(
        ${PROJECT_SOURCE_DIR}/core
        ${PROJECT_SOURCE_DIR}/netc
        ${PROJECT_SOURCE_DIR}/http
)
link_directories(${PROJECT_BINARY_DIR}/lib)

add_executable(httpserver-test HttpServer-Test.cpp)
target_link_libraries(httpserver-test PRIVATE http netc reactor-core)