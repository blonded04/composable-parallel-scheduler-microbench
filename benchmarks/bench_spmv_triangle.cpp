#include <benchmark/benchmark.h>

#include "../include/benchmarks/spmv.h"
#include "../include/parallel_for.h"

using namespace SPMV;

static void DoSetup(const benchmark::State &state) {
  InitParallel(GetNumThreads());
}

static void BM_SpmvBenchTriangle(benchmark::State &state) {
  // cache matrix and vector for all iterations
  auto A = GenSparseMatrix<double, SparseKind::TRIANGLE>(MATRIX_SIZE,
                                                         MATRIX_SIZE, DENSITY);
  auto x = GenVector<double>(MATRIX_SIZE);
  std::vector<double> y(A.Dimensions.Rows);
  for (auto _ : state) {
    MultiplyMatrix(A, x, y);
  }
}

BENCHMARK(BM_SpmvBenchTriangle)
    ->Name("SpmvTriangle_" + GetParallelMode())
    ->Setup(DoSetup)
    ->UseRealTime()
    ->MeasureProcessCPUTime()
    // ->Args({1 << 10, ScaleIterations(1 << 10), 1})         // few small tasks
    // ->Args({GetNumThreads(), ScaleIterations(1 << 20), 1}) // few big tasks
    // ->Args({1 << 13, ScaleIterations(1 << 13), 1}) // something in between
    // ->Args({1 << 16, ScaleIterations(1 << 10), 1}) // many small tasks
    // ->Args({GetNumThreads(), 1, 1024}) // multiple small parallel for runs
    // ->Args({1 << 20, 1, 1})            // as in the scan benchmarks
    ->Unit(benchmark::kMicrosecond);

BENCHMARK_MAIN();
