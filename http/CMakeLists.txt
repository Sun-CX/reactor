cmake_minimum_required(VERSION 3.15)

add_subdirectory(tests)

set(LIBRARY_SOURCE HttpRequest.cpp HttpResponse.cpp HttpServer.cpp HttpContext.cpp)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

include_directories(
        ${PROJECT_SOURCE_DIR}/core
        ${PROJECT_SOURCE_DIR}/netc
)
link_directories(${PROJECT_BINARY_DIR}/lib)

add_library(http SHARED ${LIBRARY_SOURCE})
target_link_libraries(http netc)