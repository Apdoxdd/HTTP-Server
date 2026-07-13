cmake_minimum_required(VERSION 3.10)

project(httpServer)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(include)

set(SOURCES
    src/main.cpp
    src/httpRequest.cpp
    src/httpServer.cpp
    src/methods.cpp
)

add_executable(httpServer ${SOURCES})
target_link_libraries(httpServer ws2_32 mswsock)
