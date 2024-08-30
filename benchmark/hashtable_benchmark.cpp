#include <benchmark/benchmark.h>
#include "hash_table_lock_free.hpp"
#include <unordered_set>
#include <unordered_map>
#include <mutex>

struct dummyHash
{
    int operator()(const int& value)
    {
        return value;
    }

    size_t operator()(const int &value) const
    {
        return value;
    }
};

static lf::set<int> SetLockFreeTestObj;
static std::unordered_set<int> SetSTLTestObj;
static std::unordered_map<int, int, dummyHash> HashTable;
static lf::HashTable<int, int, dummyHash> HashTableLF;
static std::mutex STLMutex;

static void BM_InsertOneValue(benchmark::State &state)
{
    for (auto _ : state)
        SetLockFreeTestObj.insert(1);
}
// Register the function as a benchmark
//BENCHMARK(BM_InsertOneValue)->ThreadRange(1, 8);

static void BM_InsertOneValueSTL(benchmark::State &state)
{
    for (auto _ : state)
    {
        std::lock_guard<std::mutex> guard(STLMutex);
        SetSTLTestObj.insert(1);
    }
}
// Register the function as a benchmark
//BENCHMARK(BM_InsertOneValueSTL)->ThreadRange(1, 8);

static void BM_InsertValuesHashTable(benchmark::State &state)
{
    size_t i = 0;
    for (auto _ : state)
    {
        HashTableLF.insert({++i, 0});
    }
}
// Register the function as a benchmark
BENCHMARK(BM_InsertValuesHashTable)->ThreadRange(1, 8);

static void BM_InsertValuesSTLHashTable(benchmark::State &state)
{
    size_t i = 0;
    for (auto _ : state)
    {
        std::lock_guard<std::mutex> guard(STLMutex);
        HashTable.insert({++i, 0});
    }
}
// Register the function as a benchmark
BENCHMARK(BM_InsertValuesSTLHashTable)->ThreadRange(1, 8);


BENCHMARK_MAIN();