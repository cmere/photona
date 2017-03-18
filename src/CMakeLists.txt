cmake_minimum_required (VERSION 2.6)
project (photona)

set(CMAKE_VERBOSE_MAKEFILE on)

include_directories(${CMAKE_SOURCE_DIR})

add_compile_options(-std=c++11 -Wall -Werror -O0)


find_library(LibBoostLocale libboost_locale.a)
find_library(LibBoostSystem libboost_system.a)
find_library(LibBoostDateTime libboost_date_time.a)

file(GLOB SocketLib_Source "SocketLib/*.cpp")
#list(REMOVE_ITEM SocketServer_Lib_Source "SocketServer/main.cpp")
add_library(socketlib STATIC ${SocketLib_Source})

file(GLOB UtilLib_Source "Util/*.cpp")
add_library(util STATIC ${UtilLib_Source})

add_executable (server Server/main.cpp)
target_link_libraries(server socketlib util ${LibBoostDateTime})

add_executable (client Client/main.cpp)
target_link_libraries(client socketlib util ${LibBoostDateTime})
