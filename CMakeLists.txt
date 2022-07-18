cmake_minimum_required(VERSION 3.0)
project(server)

set(CMAKE_CXX_STANDARD 11)

if (CMAKE_BUILD_TYPE MATCHES Debug)
    add_definitions(-DDEBUG)
endif()

add_executable(server main.cpp logger/logger.cpp logger/logger.h utils/block_queue.h utils/mutex.h utils/cond.h)
