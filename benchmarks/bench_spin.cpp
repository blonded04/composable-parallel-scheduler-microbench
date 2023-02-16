#include <benchmark/benchmark.h>

#include "../include/parallel_for.h"
#include "spmv.h"
#include <iostream>

static void DoSetup(const benchmark::State &state) {
  InitParallel(GetNumThreads());
}

static void BM_Spin(benchmark::State &state) {
  for (auto _ : state) {
    ParallelFor(0, state.range(0), [&](size_t i) {
      for (size_t i = 0; i != state.range(1); ++i) {
        CpuRelax();
      }
    });
  }
}

BENCHMARK(BM_Spin)
    ->Name("Spin_" + GetParallelMode())
    ->Setup(DoSetup)
    ->UseRealTime()
    ->Args({1 << 10, 1 << 10})         // few small tasks
    ->Args({GetNumThreads(), 1 << 20}) // few big tasks
    ->Args({1 << 13, 1 << 13})         // something in between
    ->Args({1 << 16, 1 << 10})         // many small tasks
    ->Unit(benchmark::kMicrosecond);

BENCHMARK_MAIN();
