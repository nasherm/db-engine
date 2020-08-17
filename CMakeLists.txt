cmake_minimum_required(VERSION 3.16)
project(db_engine)

set(CMAKE_CXX_STANDARD 17)

file(GLOB SRC "src/*")
file(GLOB TESTS "tests/*")
add_executable(db_engine main.cpp ${SRC})
add_executable(test ${TESTS} ${SRC})