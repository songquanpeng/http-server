cmake_minimum_required(VERSION 3.0)
project(server)

set(CMAKE_CXX_STANDARD 11)

if (CMAKE_BUILD_TYPE MATCHES Debug)
    add_definitions(-DDEBUG)
endif ()

include_directories("core" , "core/utils" , "core/logger")

file(GLOB SRC_CORE "core/*.cpp")
file(GLOB SRC_LOGGER "core/logger/*.cpp")
file(GLOB SRC_UTILS "core/utils/*.cpp")
file(GLOB SRC_ROOT "*.cpp")
set(SRCS ${SRC_CORE} ${SRC_LOGGER} ${SRC_UTILS} ${SRC_ROOT})

add_executable(server ${SRCS})
target_link_libraries(server pthread)