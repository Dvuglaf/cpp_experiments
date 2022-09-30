cmake_minimum_required(VERSION 3.8)

add_library(utils_shared SHARED vector.h)

target_include_directories(utils_shared PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} )