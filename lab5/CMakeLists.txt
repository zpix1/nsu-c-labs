cmake_minimum_required(VERSION 3.5.1)
project(lab5 C)

set(SRC src/pq.c src/coder.c src/packer.c src/bitreader.c src/bitwriter.c src/do_tester.c)

add_executable(${PROJECT_NAME} ${SRC})

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/../modules")
include(common_lab)
