#include <benchmark/benchmark.h>

#include "../include/benchmarks/spmv.h"
#include "../include/parallel_for.h"

using namespace SPMV;

static void DoSetup(const benchmark::State &state) {
  InitParallel(GetNumThreads());
}

static auto cachedMatrix = std::array{
    GenSparseMatrix<double, SparseKind::TRIANGLE>(MATRIX_SIZE, 1 << 10,
                                                  DENSITY),
    GenSparseMatrix<double, SparseKind::TRIANGLE>(MATRIX_SIZE, 1 << 12,
                                                  DENSITY),
    GenSparseMatrix<double, SparseKind::TRIANGLE>(MATRIX_SIZE, 1 << 14,
                                                  DENSITY),
    GenSparseMatrix<double, SparseKind::TRIANGLE>(MATRIX_SIZE, 1 << 16,
                                                  DENSITY),
    GenSparseMatrix<double, SparseKind::TRIANGLE>(MATRIX_SIZE, 1 << 17,
                                                  DENSITY),
};

static SPMV::SparseMatrixCSR<double> &GetCachedMatrix(size_t width) {
  for (auto &&m : cachedMatrix) {
    if (m.Dimensions.Columns == width) {
      return m;
    }
  }
  __builtin_unreachable();
}

static auto cachedVector = GenVector<double>(MATRIX_SIZE);
static std::vector<double> cachedResult(MATRIX_SIZE);

static void BM_SpmvBenchTriangle(benchmark::State &state) {
  // cache matrix and vector for all iterations
  auto &A = GetCachedMatrix(state.range(0));
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
    ->Arg(1 << 10)
    ->Arg(1 << 12)
    ->Arg(1 << 14)
    // ->Arg(1 << 16)
    // ->Arg(1 << 17)
    ->Unit(benchmark::kMicrosecond);

BENCHMARK_MAIN();
