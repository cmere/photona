cmake_minimum_required (VERSION 2.6)
project (photona)

set(CMAKE_VERBOSE_MAKEFILE on)

include_directories(${CMAKE_SOURCE_DIR})

add_compile_options(-std=c++11 -Wall -Werror)

file(GLOB SocketServer_SOURCE
  "SocketServer/*.cpp"
)

add_executable (photona ${SocketServer_SOURCE})

add_executable (socketserver_test SocketServer_test/main.cpp SocketServer/TCPSocket.cpp)
