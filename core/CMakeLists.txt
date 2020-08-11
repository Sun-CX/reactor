cmake_minimum_required(VERSION 3.15)
project(reactor-core)

set(CMAKE_CXX_STANDARD 14)

add_subdirectory(tests)

set(LIBRARY_SOURCE Thread.cpp CurrentThread.cpp Exception.cpp Timestamp.cpp
        Mutex.cpp Condition.cpp CountDownLatch.cpp Singleton.h
        BlockingQueue.h

        CircuitBuffer.h)

add_library(reactor-core SHARED ${LIBRARY_SOURCE})
target_link_libraries(reactor-core pthread)

set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)