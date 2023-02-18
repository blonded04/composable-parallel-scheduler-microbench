#include <benchmark/benchmark.h>

#include "../include/parallel_for.h"
#include "spmv.h"
#include <iostream>

using namespace SPMV;

static void DoSetup(const benchmark::State &state) {
  InitParallel(GetNumThreads());
}

// cache matrix and vector for all iterations
static auto A = GenSparseMatrix<double, SparseKind::BALANCED>(
    MATRIX_SIZE, MATRIX_SIZE, DENSITY);
static auto x = GenVector<double>(MATRIX_SIZE);
static std::vector<double> y(A.Dimensions.Rows);

static void BM_SpmvBenchBalanced(benchmark::State &state) {
  for (auto _ : state) {
    MultiplyMatrix(A, x, y, 128 * 128);
  }
}

BENCHMARK(BM_SpmvBenchBalanced)
    ->Name("SpmvBalanced_" + GetParallelMode())
    ->Setup(DoSetup)
    ->UseRealTime()
    ->Unit(benchmark::kMicrosecond);

BENCHMARK_MAIN();
