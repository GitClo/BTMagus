cmake_minimum_required(VERSION 4.0)
project(dbus)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_BUILD_TYPE Debug)

find_package(sdbus-c++ REQUIRED)

include(FetchContent)
FetchContent_Declare(
        cli
        GIT_REPOSITORY https://github.com/daniele77/cli.git
        GIT_TAG v2.2.0
)
FetchContent_MakeAvailable(cli)

add_executable(dbus flipper_scan.cpp
        flipper_scan.h
        main.cpp
        globals.h
        globals.cpp)

target_link_libraries(dbus PRIVATE SDBusCpp::sdbus-c++ PRIVATE cli)