cmake_minimum_required (VERSION 2.6)
project (photona)

set(CMAKE_VERBOSE_MAKEFILE on)

include_directories(${CMAKE_SOURCE_DIR})

add_compile_options(-std=c++11 -Wall -Werror -O0)

file(GLOB SocketServer_Lib_Source "SocketServer/*.cpp")
list(REMOVE_ITEM SocketServer_Lib_Source "SocketServer/main.cpp")

find_library(LibBoostLocale libboost_locale.a)
find_library(LibBoostSystem libboost_system.a)
find_library(LibBoostDateTime libboost_date_time.a)

add_library(libsocketserver STATIC ${SocketServer_Lib_Source})
add_executable (socketserver SocketServer/main.cpp)
target_link_libraries(socketserver libsocketserver ${LibBoostDateTime})

add_executable (socketserver_test SocketServer_test/main.cpp)
target_link_libraries(socketserver_test libsocketserver ${LibBoostDateTime})
