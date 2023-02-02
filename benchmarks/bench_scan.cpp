#include "scan.h"
#include "spmv.h"
#include <benchmark/benchmark.h>

#include "../include/parallel_for.h"

static constexpr size_t SIZE_POW = 20;

static void BM_ScanBench(benchmark::State &state) {
  InitParallel(GetNumThreads());
  // allocate data and result once, reuse it for all iterations
  auto data = SPMV::GenVector<double>(1 << SIZE_POW);
  for (auto _ : state) {
    Scan::Scan(SIZE_POW, data);
  }
}

BENCHMARK(BM_ScanBench)
    ->Name("Scan_" + GetParallelMode())
    ->UseRealTime()
    ->Unit(benchmark::kMicrosecond);

BENCHMARK_MAIN();
