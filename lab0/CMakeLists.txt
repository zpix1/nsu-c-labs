cmake_minimum_required(VERSION 3.5.1)
project(lab0 C)

set(CMAKE_C_STANDARD 99)

set(SRC src/main.c)

add_executable(lab0 ${SRC})

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/../modules")
include(enable_warnings)
include(valgrind)
include(test_runner)
