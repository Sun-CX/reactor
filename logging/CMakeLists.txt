cmake_minimum_required(VERSION 3.15)
project(logging)

set(CMAKE_CXX_STANDARD 14)

set(LIBRARY_SOURCE Logger.cpp LogStream.cpp StringPiece.h TimeZone.cpp)

include_directories(../core)

add_library(logging SHARED ${LIBRARY_SOURCE})
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)