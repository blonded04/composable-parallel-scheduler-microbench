#pragma once

#include <atomic>
#include <cstddef>
#include <cstdio>

#include "util.h"

struct SpinBarrier {
  SpinBarrier(size_t count) : remain_(count) {}

  void Notify(size_t count = 1) { remain_.fetch_sub(count); }

  void Wait() {
    unsigned spins = 0;
    while (remain_.load()) {
      CpuRelax();

      ++spins;
      if (spins == 10000000) {
        std::printf("%lu remaining...\n", remain_.load());
        spins = 0;
      }
    }
  }

  std::atomic<size_t> remain_;
};
