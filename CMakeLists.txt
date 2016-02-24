cmake_minimum_required(VERSION 3.3)
project(machelper)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.c Includes/help.h Includes/constants.h)
add_executable(machelper ${SOURCE_FILES})