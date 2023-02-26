#include <benchmark/benchmark.h>

#include "../include/parallel_for.h"
#include "spmv.h"
#include <iostream>

static void DoSetup(const benchmark::State &state) {
  InitParallel(GetNumThreads());
}

static void BM_Noop(benchmark::State &state) {
  for (auto _ : state) {
    ParallelFor(0, state.range(0), [](size_t i) {
      // TODO: don't optimize?
    });
  }
}

BENCHMARK(BM_Noop)
    ->Name(std::string("Noop_") + GetParallelMode())
    ->Setup(DoSetup)
    ->UseRealTime()
    ->MeasureProcessCPUTime()
    ->ArgName("tasks")
    ->Range(GetNumThreads(), 1 << 16)
    ->Unit(benchmark::kMicrosecond);

BENCHMARK_MAIN();
