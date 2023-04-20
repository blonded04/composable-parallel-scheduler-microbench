#include <benchmark/benchmark.h>

#include "../include/benchmarks/spmv.h"
#include "../include/parallel_for.h"
#include <unordered_map>

using namespace SPMV;

static void DoSetup(const benchmark::State &state) {
  InitParallel(GetNumThreads());
}

static constexpr auto width =
    std::array<size_t, 6>{1 << 10, 1 << 11, 1 << 12, 1 << 13, 1 << 14, 1 << 15};

static auto cachedMatrix = [] {
  std::unordered_map<size_t, SparseMatrixCSR<double>> res;
  for (auto &&w : width) {
    res[w] =
        GenSparseMatrix<double, SparseKind::TRIANGLE>(MATRIX_SIZE, w, DENSITY);
  }
  return res;
}();

static auto cachedVector = GenVector<double>(MATRIX_SIZE);
static std::vector<double> cachedResult(MATRIX_SIZE);

static void BM_SpmvBenchTriangle(benchmark::State &state) {
  // cache matrix and vector for all iterations
  auto &A = cachedMatrix.at(state.range(0));
  for (auto _ : state) {
    MultiplyMatrix(A, cachedVector, cachedResult);
  }
}

BENCHMARK(BM_SpmvBenchTriangle)
    ->Name("SpmvTriangle_" + GetParallelMode())
    ->Setup(DoSetup)
    ->UseRealTime()
    ->MeasureProcessCPUTime()
    ->ArgName("width")
    ->RangeMultiplier(2)
    ->Range(*width.begin(), *std::prev(width.end()))
    ->Unit(benchmark::kMicrosecond);

BENCHMARK_MAIN();
