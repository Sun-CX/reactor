cmake_minimum_required(VERSION 3.15)

set(LIBRARY_SOURCE StringPiece.h TimeZone.cpp LogStream.cpp Logger.cpp)
include_directories(${PROJECT_SOURCE_DIR}/core)

add_subdirectory(tests)

add_library(logging SHARED ${LIBRARY_SOURCE})
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)