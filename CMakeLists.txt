cmake_minimum_required(VERSION 3.0)
project(server)

set(CMAKE_CXX_STANDARD 11)

include_directories("core")

if (CMAKE_BUILD_TYPE MATCHES Debug)
    add_definitions(-DDEBUG)
endif ()

add_executable(server main.cpp core/logger/Logger.cpp core/logger/Logger.h core/utils/BlockQueue.h core/utils/Mutex.h core/utils/Cond.h core/EventLoop.cpp core/EventLoop.h core/TcpServer.cpp core/TcpServer.h core/Callbacks.h core/Channel.cpp core/Channel.h core/TcpConnection.cpp core/TcpConnection.h core/Buffer.cpp core/Buffer.h)
