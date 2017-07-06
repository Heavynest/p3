cmake_minimum_required(VERSION 3.8)
project(p3)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp simulation.cpp simulation.h)
add_executable(p3 ${SOURCE_FILES})