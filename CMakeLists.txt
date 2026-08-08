cmake_minimum_required(VERSION 3.20)
project(DbEngine)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS OFF)

# Rasta dikhana (Headers kahan hain)
include_directories(${PROJECT_SOURCE_DIR}/include)

# Hamare Modules (Boxes)
add_library(dbengine_disk src/disk/disk_manager.cpp)
add_library(dbengine_storage src/storage/slotted_page.cpp)
add_library(dbengine_buffer src/buffer/buffer_pool_manager.cpp
        include/storage/page.h
        include/parser/sql_parser.h
        include/execution/execution_engine.h
        include/index/b_plus_tree_node.h
        include/concurrency/transaction.h
        include/concurrency/lock_manager.h) # NAYA MODULE

# Asli program (Main CLI) aur sabko aapas me jodna
add_executable(DbEngine src/cli/main.cpp
        src/buffer/buffer_pool_manager.cpp
        src/parser/sql_parser.cpp
        src/execution/execution_engine.cpp
        src/index/b_plus_tree_node.cpp
        include/concurrency/transaction.h
        include/concurrency/lock_manager.h
        src/concurrency/lock_manager.cpp)
target_link_libraries(DbEngine dbengine_disk dbengine_storage dbengine_buffer)