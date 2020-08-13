cmake_minimum_required(VERSION 3.15)

set(CMAKE_CXX_STANDARD 14)

add_executable(noncopyable-test NonCopyable-Test.cpp)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

link_directories(${PROJECT_BINARY_DIR}/lib)

add_executable(thread-test Thread-Test.cpp)
target_link_libraries(thread-test reactor-core)

add_executable(exception-test Exception-Test.cpp)
target_link_libraries(exception-test reactor-core)

add_executable(timestamp-test Timestamp-Test.cpp)
target_link_libraries(timestamp-test reactor-core)

add_executable(mutex-test Mutex-Test.cpp)
target_link_libraries(mutex-test reactor-core)

add_executable(countdownlatch-test-1 CountDownLatch-Test1.cpp)
target_link_libraries(countdownlatch-test-1 reactor-core)

add_executable(countdownlatch-test-2 CountDownLatch-Test2.cpp)
target_link_libraries(countdownlatch-test-2 reactor-core)

add_executable(singleton-test Singleton-Test.cpp)
target_link_libraries(singleton-test reactor-core)

add_executable(blockingqueue-test BlockingQueue-Test.cpp)
target_link_libraries(blockingqueue-test reactor-core)

add_executable(blockingqueue-bench-test BlockingQueue-Bench-Test.cpp)
target_link_libraries(blockingqueue-bench-test reactor-core)

#add_executable(circuit-test CircuitBuffer-test.cpp)
#target_link_libraries(circuit-test reactor-core)

add_executable(circularbuffer-test CircularBuffer-Test.cpp)
target_link_libraries(circularbuffer-test reactor-core)



