cmake_minimum_required(VERSION 4.0)
project(BTMagus LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

include(FetchContent)
FetchContent_Declare(
    sdbus-cpp
    GIT_REPOSITORY https://github.com/Kistler-Group/sdbus-cpp.git
    GIT_TAG        v2.2.1
)
set(BUILD_SHARED_LIBS OFF)
set(SDBUSCPP_BUILD_TESTS OFF)
set(SDBUSCPP_BUILD_LIBSYSTEMD OFF)
FetchContent_MakeAvailable(sdbus-cpp)

add_library(cli INTERFACE)
target_include_directories(cli INTERFACE ${CMAKE_SOURCE_DIR}/external/cli/include)

add_executable(btmagus
    main.cpp
    core/globals.cpp
    flipper_scan/flipper_scan.cpp
    ble_spam/ble_spam.cpp
)

string(TOLOWER ${CMAKE_BUILD_TYPE} BUILD_TYPE_LOWER)
set_target_properties(btmagus PROPERTIES OUTPUT_NAME "btmagus-${BUILD_TYPE_LOWER}")

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_link_options(btmagus PRIVATE -s)  # strip debug symbols
endif()

target_link_libraries(btmagus PRIVATE
    sdbus-c++
    cli
)
