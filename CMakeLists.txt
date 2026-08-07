cmake_minimum_required(VERSION 4.3)
project(DbEngine)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(DbEngine
        disk_manager.cpp
        disk_manager.h
        main.cpp)
