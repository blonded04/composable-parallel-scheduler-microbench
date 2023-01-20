#pragma once
#include "modes.h"
#include "num_threads.h"

#ifdef EIGEN_MODE

#define EIGEN_USE_THREADS
#include "../contrib/eigen/unsupported/Eigen/CXX11/Tensor"
#include "../contrib/eigen/unsupported/Eigen/CXX11/TensorSymmetry"
#include "../contrib/eigen/unsupported/Eigen/CXX11/ThreadPool"

inline auto EigenPool =
    Eigen::ThreadPool(GetNumThreads() - 1); // 1 for main thread

class EigenPoolWrapper {
public:
  template <typename F> void run(F &&f) {
    EigenPool.Schedule(std::forward<F>(f));
  }

  void wait() {
    // TODO: implement
  }
};

#endif
