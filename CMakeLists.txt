cmake_minimum_required(VERSION 3.28)
project(NetworkTest)

enable_testing()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        # 为所有目标添加DEBUG宏
        add_compile_definitions(DEBUG)
endif()

include_directories(Common Service)

find_package(PkgConfig)
# find_package(spdlog)

pkg_check_modules(MyLib IMPORTED_TARGET gtest spdlog jsoncpp cpp-httplib)

add_executable(Main main.cpp
        Common/util.hpp
)

target_link_libraries(Main 
        PkgConfig::MyLib
        )
