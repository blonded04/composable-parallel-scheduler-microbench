#include "scan.h"
#include "spmv.h"
#include <benchmark/benchmark.h>

#include "../include/parallel_for.h"

static constexpr size_t MIN_SIZE = 1 << 10;
static constexpr size_t MAX_SIZE = 1 << 27;

static void BM_ScanBench(benchmark::State &state) {
  InitParallel(GetNumThreads());
  size_t size = state.range(0);
  size_t size_pow = Scan::GetBlockPow(size);
  // allocate data and result once, reuse it for all iterations
  auto data = SPMV::GenVector<double>(size);
  for (auto _ : state) {
    Scan::Scan(size_pow, data);
  }
}

BENCHMARK(BM_ScanBench)
    ->Name("Scan_" + GetParallelMode())
    ->UseRealTime()
    ->Unit(benchmark::kMicrosecond)
    ->Range(MIN_SIZE, MAX_SIZE);

BENCHMARK_MAIN();
