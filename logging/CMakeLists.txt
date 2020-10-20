cmake_minimum_required(VERSION 3.15)

set(LIBRARY_SOURCE StringPiece.h TimeZone.cpp LogStream.cpp String.cpp FixedBuffer.h Logger.cpp)
include_directories(${PROJECT_SOURCE_DIR}/core)
link_directories(${PROJECT_BINARY_DIR}/lib)

add_subdirectory(tests)

add_library(logging SHARED ${LIBRARY_SOURCE})
target_link_libraries(logging reactor-core)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)