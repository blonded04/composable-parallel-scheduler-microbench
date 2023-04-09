#include "../include/benchmarks/spmv.h"
#include <benchmark/benchmark.h>

#include "../include/parallel_for.h"

static constexpr size_t MATRIX_SIZE = 1 << 10; // TODO: tune

static void DoSetup(const benchmark::State &state) {
  InitParallel(GetNumThreads());
}

// cache data for all iterations
static auto left = SPMV::GenDenseMatrix<double>(MATRIX_SIZE, MATRIX_SIZE);
static auto right = SPMV::GenDenseMatrix<double>(MATRIX_SIZE, MATRIX_SIZE);
static auto out = SPMV::DenseMatrix<double>(MATRIX_SIZE, MATRIX_SIZE);

static void BM_MatrixMul(benchmark::State &state) {
  for (auto _ : state) {
    SPMV::MultiplyMatrix(left, right, out);
  }
}

BENCHMARK(BM_MatrixMul)
    ->Name("MatrixMul_" + GetParallelMode())
    ->Setup(DoSetup)
    ->UseRealTime()
    ->MeasureProcessCPUTime()
    ->Unit(benchmark::kMicrosecond);

BENCHMARK_MAIN();
