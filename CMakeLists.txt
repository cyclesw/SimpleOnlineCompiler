cmake_minimum_required(VERSION 3.28)
project(Compiler)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR})

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        # 为所有目标添加DEBUG宏
        add_compile_definitions(DEBUG)
endif()

include_directories(Common Service)

find_package(PkgConfig)

pkg_check_modules(MyLib IMPORTED_TARGET spdlog jsoncpp cpp-httplib)

add_executable(Main main.cpp
)

add_executable(test test.cpp)

target_link_libraries(test
        PkgConfig::MyLib
        )

target_link_libraries(Main
        PkgConfig::MyLib
        )

