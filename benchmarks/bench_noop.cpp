#include <benchmark/benchmark.h>

static void BM_EmptyLoop(benchmark::State &state) {
  std::string s = "hello";
  for (auto _ : state) {
    auto doubled = s + s;
    benchmark::DoNotOptimize(doubled);
  }
}

BENCHMARK(BM_EmptyLoop);

BENCHMARK_MAIN();
