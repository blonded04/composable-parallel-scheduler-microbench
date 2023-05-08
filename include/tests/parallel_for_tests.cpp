
#include "../parallel_for.h"
#include <atomic>
#include <gtest/gtest.h>
#include <random>

TEST(ParallelFor, Basic) {
  std::atomic<int> sum(0);
  ParallelFor(0, 100, [&](int i) { sum += i; });
  EXPECT_EQ(4950, sum);
}

TEST(ParallelFor, BasicNonZero) {
  std::atomic<int> sum(0);
  ParallelFor(10, 20, [&](int i) { sum += i; });
  EXPECT_EQ(145, sum);
}

TEST(ParallelFor, ConcurrentCalls) {
  std::atomic<int64_t> sum(0);
  auto maxThreads = GetNumThreads();
  std::vector<std::thread> threads;
  for (int i = 0; i < maxThreads; ++i) {
    threads.emplace_back(
        [&]() { ParallelFor(0, 1024 * 1024, [&](int i) { sum += 1; }); });
  }
  for (auto &t : threads) {
    t.join();
  }
  EXPECT_EQ(1024 * 1024 * maxThreads, sum);
}

TEST(ParallelFor, MultipleCalls) {
  std::atomic<int> sum(0);
  ParallelFor(0, 100, [&](int i) { sum += i; });
  ParallelFor(0, 100, [&](int i) { sum += i; });
  EXPECT_EQ(9900, sum);
}

TEST(ParallelFor, AllThreadsUsed) {
  std::atomic<int> current(0);
  auto maxThreads = GetNumThreads();
  ParallelFor(0, maxThreads, [&](int i) {
    current++;
    while (current != maxThreads) {
      CpuRelax();
    }
  });
  EXPECT_EQ(maxThreads, current);
}

TEST(ParallelFor, Recursive) {
  std::atomic<int> sum(0);
  auto maxThreads = GetNumThreads();
  ParallelFor(0, maxThreads, [&](int i) {
    ParallelFor(0, maxThreads, [&](int j) { sum++; });
  });
  EXPECT_EQ(maxThreads * maxThreads, sum);
}

TEST(ParallelFor, Unbalanced) {
  std::atomic<int> sum(0);
  auto maxThreads = GetNumThreads();
  ParallelFor(0, maxThreads, [&](int i) {
    for (size_t j = 0; j != 1000 * (i + 1); ++j) {
      sum++;
    }
  });
  EXPECT_EQ((maxThreads + 1) * maxThreads * 1000 / 2, sum);
}

TEST(ParallelFor, StressTest) {
  // make a lot of random concurrent parallel for calls with random tasks delay
  std::atomic<int> sum(0);
  auto maxThreads = GetNumThreads();
  std::vector<std::thread> threads;
  for (int i = 0; i < maxThreads; ++i) {
    threads.emplace_back([&]() {
      std::default_random_engine rnd{std::random_device()()};
      for (int j = 0; j < 100; ++j) {
        ParallelFor(0, 1000, [&](int i) {
          std::this_thread::sleep_for(std::chrono::microseconds(rnd() % 100));
          sum++;
        });
      }
    });
  }
  for (auto &t : threads) {
    t.join();
  }
  EXPECT_EQ(100 * 1000 * maxThreads, sum);
}

#if EIGEN_MODE == EIGEN_SIMPLE
TEST(ParallelFor, Workstealing) {
  // 1 task should be blocked until all other tasks are done
  std::atomic<int> sum(0);
  auto maxThreads = GetNumThreads();
  ParallelFor(0, maxThreads * 16, [&](int i) {
    if (i == 0) {
      while (sum != maxThreads * 16 - 1) {
        CpuRelax();
      }
    } else {
      sum++;
    }
  });
}
#endif

#if defined(EIGEN_MODE)
TEST(ParallelFor, InitialDistribution) {
  // all tasks should be distributed without work stealing
  auto maxThreads = GetNumThreads();
  SpinBarrier barrier(maxThreads);
  ParallelFor(0, maxThreads, [&](int i) {
    EXPECT_EQ(i, GetThreadIndex());
    barrier.Notify();
    barrier.Wait();
  });
  EXPECT_EQ(0, GetThreadIndex());
}
#endif

#if EIGEN_MODE == EIGEN_TIMESPAN || EIGEN_MODE == EIGEN_TIMESPAN_GRAINSIZE
TEST(ParallelFor, InitialDistributionBalanced) {
  auto maxThreads = GetNumThreads();
  SpinBarrier barrier(maxThreads - 1);
  ParallelFor(0, maxThreads * 2 - 1, [&](int i) {
    if (i % 2 == 0) {
      return;
    }
    auto expectedThread = i / 2;
    EXPECT_EQ(expectedThread, GetThreadIndex());
    barrier.Notify();
    barrier.Wait();
  });
  EXPECT_EQ(0, GetThreadIndex());
}
#endif
