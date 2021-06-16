cmake_minimum_required(VERSION 3.15)

add_subdirectory(tests)

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
set_target_properties(reactor-core PROPERTIES
        VERSION 1.0
        SOVERSION 1
        LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib
        )
target_link_libraries(reactor-core PRIVATE pthread)