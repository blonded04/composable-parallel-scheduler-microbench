#include "scan.h"
#include "spmv.h"
#include <benchmark/benchmark.h>

#include "../include/parallel_for.h"

static constexpr size_t SIZE_POW = 20;

static void DoSetup(const benchmark::State &state) {
  InitParallel(GetNumThreads());
}

// cache data for all iterations
static auto data = SPMV::GenVector<double>(1 << SIZE_POW);

static void BM_ScanBench(benchmark::State &state) {
  for (auto _ : state) {
    Scan::Scan(SIZE_POW, data);
  }
}

BENCHMARK(BM_ScanBench)
    ->Name("Scan_" + GetParallelMode())
    ->Setup(DoSetup)
    ->UseRealTime()
    ->Unit(benchmark::kMicrosecond);

BENCHMARK_MAIN();
