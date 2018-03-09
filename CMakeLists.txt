cmake_minimum_required(VERSION 3.16.5)
project(devmem VERSION 1.0.0)

add_library(
    say-devmem STATIC   
    devmem.h
    devmem.c
)

add_executable(run-devmem main.c)

target_link_libraries(run-devmem PRIVATE say-devmem)