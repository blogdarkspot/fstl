cmake_minimum_required(VERSION 3.20)

Include(FetchContent)
FetchContent_Declare(
    GoogleBenchmark
    GIT_REPOSITORY https://github.com/google/benchmark.git
    GIT_TAG v1.9.0
)
set(BENCHMARK_ENABLE_GTEST_TESTS OFF)
set(BENCHMARK_ENABLE_TESTING OFF)
FetchContent_MakeAvailable(GoogleBenchmark)

set(BENCH_NAME Benchmark)
add_executable(${BENCH_NAME} hashtable_benchmark.cpp)
target_include_directories(${BENCH_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../include")
target_link_libraries(${BENCH_NAME} PRIVATE benchmark::benchmark benchmark::benchmark_main)
target_compile_features(${BENCH_NAME} PUBLIC cxx_std_17)
