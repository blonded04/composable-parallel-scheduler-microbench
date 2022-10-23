cmake_minimum_required (VERSION 3.15)
project(composable-parallel-scheduler)

set(CMAKE_CXX_STANDARD 17)

# todo: install libraries?
find_package(GTest REQUIRED)
find_package(benchmark REQUIRED)

# build options
option(USE_SANITIZERS "Enable to build with undefined,leak and address sanitizers" OFF)

add_subdirectory(benchmarks)
