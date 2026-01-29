set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
project(LocCounter VERSION 1.0.0 LANGUAGES CXX)
cmake_minimum_required(VERSION 4.2)

file(GLOB_RECURSE SOURCES "src/*.cpp")

add_executable(LocCounter ${SOURCES})

