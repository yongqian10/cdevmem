add_library(
    cdevmem   
    src/cdevmem/cdevmem.h
    src/cdevmem/cdevmem.c
)

target_include_directories(cdevmem PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")