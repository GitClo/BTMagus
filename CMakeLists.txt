cmake_minimum_required(VERSION 4.0)
project(dbus)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_BUILD_TYPE Debug)

find_package(sdbus-c++ REQUIRED)
add_executable(dbus main.cpp)

target_link_libraries(dbus PRIVATE SDBusCpp::sdbus-c++)