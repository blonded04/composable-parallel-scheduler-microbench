#include <benchmark/benchmark.h>

#include "../include/parallel_for.h"
#include "spmv.h"
#include <iostream>

#define RELAX 1
#define ATOMIC 2
#define DISTRIBUTED_READ 3

static void DoSetup(const benchmark::State &state) {
  InitParallel(GetNumThreads());
}

namespace {
struct AlignedAtomic {
  alignas(64) std::atomic<int> value;
};
} // namespace

static void BM_Spin(benchmark::State &state) {
#if SPIN_PAYLOAD == RELAX
  for (auto _ : state) {
    ParallelFor(0, state.range(0), [cnt = state.range(1)](size_t i) {
      for (size_t i = 0; i != cnt; ++i) {
        CpuRelax();
      }
    });
  }
#elif SPIN_PAYLOAD == ATOMIC
  auto atomicPtr = std::make_unique<AlignedAtomic>();
  for (auto _ : state) {
    ParallelFor(0, state.range(0),
                [p = atomicPtr.get(), cnt = state.range(1)](size_t i) {
                  volatile int y = 0;
                  for (size_t i = 0; i != cnt; ++i) {
                    y = p->value.load();
                  }
                });
  }
#elif SPIN_PAYLOAD == DISTRIBUTED_READ
  std::vector<std::unique_ptr<int>> pointers(state.range(0));
  for (auto &p : pointers) {
    p = std::make_unique<int>(0);
  }
  for (auto _ : state) {
    ParallelFor(0, state.range(0), [&pointers, cnt = state.range(1)](size_t i) {
      volatile size_t y = 0;
      auto p = pointers[i].get();
      for (size_t i = 0; i != cnt; ++i) {
        y = *p;
      }
    });
  }
#else
  static_assert(false, "Unsupported mode");
#endif
}

inline std::string GetSpinPayload() {
#if SPIN_PAYLOAD == RELAX
  return "RELAX";
#elif SPIN_PAYLOAD == ATOMIC
  return "ATOMIC";
#elif SPIN_PAYLOAD == DISTRIBUTED_READ
  return "DISTRIBUTED_READ";
#else
  static_assert(false, "Unsupported mode");
#endif
}

BENCHMARK(BM_Spin)
    ->Name(std::string("Spin_") + GetSpinPayload() + "_" + GetParallelMode())
    ->Setup(DoSetup)
    ->UseRealTime()
    ->MeasureProcessCPUTime()
    ->ArgNames({"tasks", "iters"})
    ->Args({1 << 10, 1 << 10})         // few small tasks
    ->Args({GetNumThreads(), 1 << 20}) // few big tasks
    ->Args({1 << 13, 1 << 13})         // something in between
    ->Args({1 << 16, 1 << 10})         // many small tasks
    ->Unit(benchmark::kMicrosecond);

BENCHMARK_MAIN();
