#pragma once
#include "modes.h"
#include "num_threads.h"

#ifdef EIGEN_MODE

#define EIGEN_USE_THREADS
#include "eigen/nonblocking_thread_pool.h"
#include "tracing.h"

#if EIGEN_MODE == EIGEN_RAPID
inline auto EigenPool =
    Eigen::ThreadPool(GetNumThreads() - 1); // 1 for main thread
#else
inline Eigen::ThreadPool& EigenPool() {
  static auto pool = Eigen::ThreadPool(GetNumThreads(), true, true); 
  return pool;
}
#endif

class EigenPoolWrapper {
public:
  template <typename F> void run(F &&f) {
    EigenPool().Schedule(Eigen::MakeTask(std::forward<F>(f)));
  }

  template <typename F> void run_on_thread(F &&f, size_t hint) {
    auto task = Eigen::MakeProxyTask(std::forward<F>(f));
    Eigen::Tracing::TaskShared();
    EigenPool().RunOnThread(task, hint);
    EigenPool().Schedule(task); // might push twice to the same thread, OK for now
  }

  bool join_main_thread() { return EigenPool().JoinMainThread(); }

  bool execute_something_else() {
    return EigenPool().TryExecuteSomething();
  }
};

#endif
