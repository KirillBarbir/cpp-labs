cmake_minimum_required(VERSION 3.12)

project(
        labwork10
        VERSION 1.0
        DESCRIPTION "C++ Year 2024 labwork10"
        LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 23)


add_subdirectory(lib)
add_subdirectory(bin)


enable_testing()
add_subdirectory(test)
