cmake_minimum_required(VERSION 3.15)

add_subdirectory(tests)

add_library(procodec SHARED Codec.cpp Dispatcher.h DispatcherLite.h)
set_target_properties(procodec PROPERTIES
        VERSION 1.0
        SOVERSION 1
        LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
target_include_directories(procodec PRIVATE
        ${CMAKE_SOURCE_DIR}/core
        ${CMAKE_SOURCE_DIR}/netc)
target_link_directories(procodec PRIVATE ${CMAKE_BINARY_DIR}/lib)
target_link_libraries(procodec PRIVATE reactor-core netc protobuf)