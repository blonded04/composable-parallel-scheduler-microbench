#include <benchmark/benchmark.h>

#include "../include/parallel_for.h"
#include "../include/benchmarks/spmv.h"

using namespace SPMV;

static void DoSetup(const benchmark::State &state) {
  InitParallel(GetNumThreads());
}

// cache matrix and vector for all iterations
static auto A = GenSparseMatrix<double, SparseKind::HYPERBOLIC>(
    MATRIX_SIZE, MATRIX_SIZE, DENSITY);
static auto x = GenVector<double>(MATRIX_SIZE);
static std::vector<double> y(A.Dimensions.Rows);

static void BM_SpmvBenchHyperbolic(benchmark::State &state) {
  for (auto _ : state) {
    MultiplyMatrix(A, x, y);
  }
}

BENCHMARK(BM_SpmvBenchHyperbolic)
    ->Name("SpmvHyperbolic_" + GetParallelMode())
    ->Setup(DoSetup)
    ->UseRealTime()
    ->MeasureProcessCPUTime()
    ->Unit(benchmark::kMicrosecond);

BENCHMARK_MAIN();
