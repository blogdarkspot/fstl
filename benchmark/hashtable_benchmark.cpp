#include <benchmark/benchmark.h>
#include "hash_table_lock_free.hpp"
#include <unordered_set>
#include <mutex>

static lf::set<int> SetLockFreeTestObj;

static std::unordered_set<int> SetSTLTestObj;
static std::mutex STLMutex;

static void BM_InsertOneValue(benchmark::State &state)
{
    for (auto _ : state)
        SetLockFreeTestObj.insert(1);
}
// Register the function as a benchmark
BENCHMARK(BM_InsertOneValue)->ThreadRange(1, 8);

static void BM_InsertOneValueSTL(benchmark::State &state)
{
    for (auto _ : state)
    {
        std::lock_guard<std::mutex> guard(STLMutex);
        SetSTLTestObj.insert(1);
    }
}
// Register the function as a benchmark
BENCHMARK(BM_InsertOneValueSTL)->ThreadRange(1, 8);

// Define another benchmark
static void BM_StringCopy(benchmark::State &state)
{
    std::string x = "hello";
    for (auto _ : state)
        std::string copy(x);
}
BENCHMARK(BM_StringCopy);

BENCHMARK_MAIN();