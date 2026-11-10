cmake_minimum_required(VERSION 3.16)
project(DbEngine LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
add_library(dbengine_core
    src/disk/disk_manager.cpp src/storage/slotted_page.cpp
    src/buffer/buffer_pool_manager.cpp src/parser/sql_parser.cpp
    src/execution/execution_engine.cpp src/index/b_plus_tree_node.cpp
    src/concurrency/lock_manager.cpp)
target_include_directories(dbengine_core PUBLIC include)
add_executable(DbEngine src/cli/main.cpp)
target_link_libraries(DbEngine PRIVATE dbengine_core)
enable_testing()
add_executable(dbengine_tests tests/dbengine_tests.cpp)
target_link_libraries(dbengine_tests PRIVATE dbengine_core)
add_test(NAME dbengine_tests COMMAND dbengine_tests)

find_package(Python3 QUIET COMPONENTS Interpreter)
if(Python3_Interpreter_FOUND)
    add_test(NAME dbengine_demo
        COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/scripts/demo.py"
                "$<TARGET_FILE:DbEngine>")
endif()
