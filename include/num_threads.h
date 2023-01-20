#pragma once

#include "modes.h"
#include <cstddef>
#include <iostream>
#include <string>
#include <thread>

inline int GetNumThreads() {
  int threads = 1;
#if defined(TBB_MODE)
  threads = tbb::info::
      default_concurrency(); // tbb::this_task_arena::max_concurrency();
#elif defined(OMP_MODE)
  threads = omp_get_max_threads();
#elif defined(SERIAL)
  threads = 1;
#elif defined(EIGEN_MODE)
  threads = std::thread::hardware_concurrency();
#else
  static_assert(false, "Unsupported mode");
#endif
  if (const char *envThreads = std::getenv("BENCH_MAX_THREADS")) {
    return std::min(threads, std::stoi(envThreads));
  }
  return threads;
}
