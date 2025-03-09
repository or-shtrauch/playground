cmake_minimum_required(VERSION 3.10)

project(lru_cache)

file(GLOB SRC "src/*.c")

add_executable(lru_cache main.c ${SRC})
target_include_directories(lru_cache PUBLIC include)