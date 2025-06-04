cmake_minimum_required(VERSION 3.10)

project(CONN_POOL)

file(GLOB SRCS src/*.c)

add_executable(conn_pool ${SRCS} main.c)

target_include_directories(conn_pool PUBLIC include)