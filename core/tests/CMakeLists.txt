cmake_minimum_required(VERSION 3.15)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

include_directories(${PROJECT_SOURCE_DIR}/core)
link_directories(${PROJECT_BINARY_DIR}/lib)

add_executable(thread-test Thread-Test.cpp)
target_link_libraries(thread-test PRIVATE reactor-core)

add_executable(exception-test Exception-Test.cpp)
target_link_libraries(exception-test PRIVATE reactor-core)

add_executable(timestamp-test Timestamp-Test.cpp)
target_link_libraries(timestamp-test PRIVATE reactor-core)

add_executable(mutex-test Mutex-Test.cpp)
target_link_libraries(mutex-test PRIVATE reactor-core)

add_executable(countdownlatch-test-1 CountDownLatch-Test1.cpp)
target_link_libraries(countdownlatch-test-1 PRIVATE reactor-core)

add_executable(countdownlatch-test-2 CountDownLatch-Test2.cpp)
target_link_libraries(countdownlatch-test-2 PRIVATE reactor-core)

add_executable(singleton-test Singleton-Test.cpp)
target_link_libraries(singleton-test PRIVATE reactor-core)

add_executable(blockingqueue-test BlockingQueue-Test.cpp)
target_link_libraries(blockingqueue-test PRIVATE reactor-core)

add_executable(blockingqueue-bench-test BlockingQueue-Bench-Test.cpp)
target_link_libraries(blockingqueue-bench-test PRIVATE reactor-core)

add_executable(circularbuffer-test CircularBuffer-Test.cpp)
target_link_libraries(circularbuffer-test PRIVATE reactor-core)

add_executable(boundedblockingqueue-test BoundedBlockingQueue-Test.cpp)
target_link_libraries(boundedblockingqueue-test PRIVATE reactor-core)

add_executable(fixedthreadpool-test FixedThreadPool-Test.cpp)
target_link_libraries(fixedthreadpool-test PRIVATE reactor-core)

add_executable(threadlocal-test ThreadLocal-Test.cpp)
target_link_libraries(threadlocal-test PRIVATE reactor-core)

add_executable(consolestream-test ConsoleStream-Test.cpp)
target_link_libraries(consolestream-test PRIVATE reactor-core)