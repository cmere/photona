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

file(GLOB ServerApp_Source "Server/*.cpp")
add_executable (photona.server ${ServerApp_Source})
target_link_libraries(photona.server socketlib util ${LibBoostDateTime})

file(GLOB ClientApp_Source "Client/*.cpp")
add_executable (photona.client ${ClientApp_Source})
target_link_libraries(photona.client socketlib util ${LibBoostDateTime})
