cmake_minimum_required(VERSION 3.12)
project(libffm CXX)

set(CMAKE_C_STANDARD 11)
SET(CMAKE_CXX_COMPILER g++)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_FLAGS "-Wall -O3 -std=c++0x -march=native -DUSESSE")

add_library(ffm SHARED ffm.cpp timer.cpp)
add_executable(ffm-train ffm-train.cpp ffm.cpp timer.cpp)
add_executable(ffm-predict ffm-predict.cpp ffm.cpp timer.cpp)
