#include "../include/benchmarks/spmv.h"
#include <benchmark/benchmark.h>

#include "../include/parallel_for.h"

static const size_t MATRIX_SIZE = GetNumThreads() * 32;

static void DoSetup(const benchmark::State &state) {
  InitParallel(GetNumThreads());
}

static void BM_MatrixTranspose(benchmark::State &state) {
  static auto matrix = SPMV::GenDenseMatrix<double>(MATRIX_SIZE, MATRIX_SIZE);
  static auto out = SPMV::DenseMatrix<double>(MATRIX_SIZE, MATRIX_SIZE);
  benchmark::DoNotOptimize(matrix);
  benchmark::DoNotOptimize(out);
  for (auto _ : state) {
    SPMV::TransposeMatrix(matrix, out);
    benchmark::ClobberMemory();
  }
}


#ifndef TASKFLOW_MODE
BENCHMARK(BM_MatrixTranspose)
    ->Name("MatrixTranspose_Latency_" + GetParallelMode())
    ->Setup(DoSetup)
    ->UseRealTime()
    ->MeasureProcessCPUTime()
    ->Unit(benchmark::kMicrosecond)
    ->MinTime(2);

BENCHMARK(BM_MatrixTranspose)
    ->Name("MatrixTranspose_Throughput_" + GetParallelMode())
    ->Setup(DoSetup)
    ->UseRealTime()
    ->MeasureProcessCPUTime()
    ->Unit(benchmark::kMicrosecond)
    ->MinTime(2);


BENCHMARK_MAIN();
#else
int main() {}
#endif

