cmake_minimum_required (VERSION 3.8)

set(CMAKE_CXX_STANDARD 17)
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    add_compile_options(-Wall -Wextra -Wpedantic)
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    add_compile_options(/W4 /D_CRT_SECURE_NO_WARNINGS)
endif()

project ("cpp_experiments")

# SubDirectories
add_subdirectory ("cpp_experiments")
