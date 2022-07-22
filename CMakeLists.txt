cmake_minimum_required(VERSION 3.0)
project(server)

set(CMAKE_CXX_STANDARD 11)

include_directories("core")

if (CMAKE_BUILD_TYPE MATCHES Debug)
    add_definitions(-DDEBUG)
endif ()

add_executable(server main.cpp core/logger/Logger.cpp core/logger/Logger.h core/utils/BlockQueue.h core/utils/Mutex.h core/utils/Cond.h core/EventLoop.cpp core/EventLoop.h core/HTTPServer.cpp core/HTTPServer.h core/Callbacks.h core/Channel.cpp core/Channel.h core/HTTPConnection.cpp core/HTTPConnection.h core/Buffer.h core/HTTPRequest.h core/Poller.cpp core/Poller.h core/utils/common.h)
target_link_libraries(server pthread)