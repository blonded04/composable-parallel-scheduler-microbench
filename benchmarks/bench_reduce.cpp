#include "spmv.h"
#include <benchmark/benchmark.h>

#include "../include/parallel_for.h"

static constexpr size_t MAX_SIZE = 1 << 24;
static constexpr size_t BLOCK_SIZE = 1 << 10;

static void BM_ReduceBench(benchmark::State &state) {
  InitParallel(GetNumThreads());
  auto blocks = (MAX_SIZE + BLOCK_SIZE - 1) / BLOCK_SIZE;
  // allocate data and result once, reuse it for all iterations
  auto data = SPMV::GenVector<double>(MAX_SIZE);
  std::vector<double> result(blocks);

  for (auto _ : state) {
    ParallelFor(0, blocks, [&](size_t i) {
      double sum = 0;
      auto start = i * BLOCK_SIZE;
      auto end = std::min(start, MAX_SIZE - BLOCK_SIZE) + BLOCK_SIZE;
      for (size_t j = start; j < end; ++j) {
        sum += data[j];
      }
      result[i] = sum;
    });
  }
}

BENCHMARK(BM_ReduceBench)
    ->Name("Reduce_" + GetParallelMode())
    ->UseRealTime()
    ->Unit(benchmark::kMicrosecond);

BENCHMARK_MAIN();
