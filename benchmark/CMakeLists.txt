cmake_minimum_required(VERSION 3.20)

set(BENCH_NAME Benchmark)
add_executable(${BENCH_NAME} hashtable_benchmark.cpp)
target_include_directories(${BENCH_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../include")
target_link_libraries(${BENCH_NAME} PRIVATE benchmark::benchmark benchmark::benchmark_main)
target_compile_features(${BENCH_NAME} PUBLIC cxx_std_17)