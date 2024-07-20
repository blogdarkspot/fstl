cmake_minimum_required(VERSION 3.20)

project(HashTables VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(ENABLE_TESTS "Build Tests" ON)
option(ENABLE_BENCHMARK "Benchmark" ON)

if(ENABLE_TESTS)
	enable_testing()
	add_subdirectory(tests)
endif()

if(ENABLE_BENCHMARK)
	add_subdirectory(submodules/benchmark)
	add_subdirectory(benchmark)
endif()


