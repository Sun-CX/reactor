cmake_minimum_required(VERSION 3.15)

add_subdirectory(tests)

set(LIBRARY_SOURCE
        HttpRequest.cpp
        HttpResponse.cpp
        HttpServer.cpp
        HttpContext.cpp
        URLEncoder.h)

add_library(http SHARED ${LIBRARY_SOURCE})
set_target_properties(http PROPERTIES
        VERSION 1.0
        SOVERSION 1
        LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
target_include_directories(http PRIVATE
        ${PROJECT_SOURCE_DIR}/core
        ${PROJECT_SOURCE_DIR}/netc)

target_link_directories(http PRIVATE ${PROJECT_BINARY_DIR}/lib)
target_link_libraries(http PRIVATE netc)