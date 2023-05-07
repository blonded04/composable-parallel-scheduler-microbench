#include "../include/benchmarks/scan.h"
#include "../include/benchmarks/spmv.h"
#include <benchmark/benchmark.h>

#include "../include/parallel_for.h"

static constexpr size_t SIZE_POW = 24;

static void DoSetup(const benchmark::State &state) {
  InitParallel(GetNumThreads());
}

// cache data for all iterations
static auto data = SPMV::GenVector<double>(1 << SIZE_POW);

static void BM_ScanBench(benchmark::State &state) {
  for (auto _ : state) {
    Scan::Scan(state.range(0), data);
  }
}

BENCHMARK(BM_ScanBench)
    ->Name("Scan_" + GetParallelMode())
    ->Setup(DoSetup)
    ->UseRealTime()
    ->MeasureProcessCPUTime()
    ->ArgName("SizePow")
    ->DenseRange(10, SIZE_POW)
    ->Unit(benchmark::kMicrosecond);

BENCHMARK_MAIN();
