cmake_minimum_required(VERSION 3.15)

add_subdirectory(tests)

set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)
set(LIBRARY_SOURCE
        Thread.cpp
        Exception.cpp
        Timestamp.cpp
        Mutex.cpp
        Condition.cpp
        CountDownLatch.cpp
        FixedThreadPool.cpp
        ConsoleStream.cpp)

add_library(reactor-core SHARED ${LIBRARY_SOURCE})
target_link_libraries(reactor-core INTERFACE pthread)