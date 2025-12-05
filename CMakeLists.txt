cmake_minimum_required(VERSION 4.0)
project(BTMagus)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_BUILD_TYPE Debug)

find_package(sdbus-c++ REQUIRED)

add_library(cli INTERFACE)
target_include_directories(cli INTERFACE
        ${CMAKE_SOURCE_DIR}/external/cli/include)

add_executable(btmagus
        flipper_scan.cpp
        flipper_scan.h
        main.cpp
        globals.h
        globals.cpp)

target_link_libraries(btmagus PRIVATE SDBusCpp::sdbus-c++ PRIVATE cli)
