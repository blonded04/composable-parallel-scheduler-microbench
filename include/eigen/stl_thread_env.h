#pragma once

#include <functional>
#include <thread>

struct StlThreadEnvironment {
  struct Task {
    std::function<void()> f;
  };

  // EnvThread constructor must start the thread,
  // destructor must join the thread.
  class EnvThread {
  public:
    EnvThread(std::function<void()> f) : thr_(std::move(f)) {}
    ~EnvThread() { thr_.join(); }
    // This function is called when the threadpool is cancelled.
    void OnCancel() {}

  private:
    std::thread thr_;
  };

  EnvThread *CreateThread(std::function<void()> f) {
    return new EnvThread(std::move(f));
  }
  Task CreateTask(std::function<void()> f) { return Task{std::move(f)}; }
  void ExecuteTask(const Task &t) { t.f(); }
};
