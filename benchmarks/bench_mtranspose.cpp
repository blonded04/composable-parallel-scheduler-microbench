#include "spmv.h"
#include <benchmark/benchmark.h>

#include "../include/parallel_for.h"

static constexpr size_t MATRIX_SIZE = 1 << 10;

static void DoSetup(const benchmark::State &state) {
  InitParallel(GetNumThreads());
}

// cache data for all iterations
static auto matrix = SPMV::GenDenseMatrix<double>(MATRIX_SIZE, MATRIX_SIZE);
static auto out = SPMV::DenseMatrix<double>(MATRIX_SIZE, MATRIX_SIZE);

static void BM_MatrixTranspose(benchmark::State &state) {
  for (auto _ : state) {
    SPMV::TransposeMatrix(matrix, out);
  }
}

BENCHMARK(BM_MatrixTranspose)
    ->Name("MatrixTranspose_" + GetParallelMode())
    ->Setup(DoSetup)
    ->UseRealTime()
    ->MeasureProcessCPUTime()
    ->Unit(benchmark::kMicrosecond);

BENCHMARK_MAIN();
