#include "../include/benchmarks/spmv.h"
#include <benchmark/benchmark.h>

#include "../include/parallel_for.h"

static const size_t MAX_SIZE = GetNumThreads() * (1 << 19);
// static constexpr size_t BLOCK_SIZE = 1 << 14;
// static constexpr size_t blocks = (MAX_SIZE + BLOCK_SIZE - 1) / BLOCK_SIZE;

static void DoSetup(const benchmark::State &state) {
  InitParallel(GetNumThreads());
}

// cache data for all iterations
static auto data = SPMV::GenVector<double>(MAX_SIZE);

static void BM_ReduceBench(benchmark::State &state) {
  auto blockSize = state.range(0);
  auto blocks = (MAX_SIZE + blockSize - 1) / blockSize;
  for (auto _ : state) {
    ParallelFor(0, blocks, [&](size_t i) {
      static thread_local double res = 0;
      double sum = 0;
      auto start = i * blockSize;
      auto end = std::min(start + blockSize, MAX_SIZE);
      for (size_t j = start; j < end; ++j) {
        sum += data[j];
      }
      res += sum;
    });
  }
}

BENCHMARK(BM_ReduceBench)
    ->Name("Reduce_" + GetParallelMode())
    ->Setup(DoSetup)
    ->UseRealTime()
    ->MeasureProcessCPUTime()
    ->ArgName("blocksize")
    ->RangeMultiplier(2)
    ->Range(1 << 10, 1 << 19)
    ->Unit(benchmark::kMicrosecond);

BENCHMARK_MAIN();
