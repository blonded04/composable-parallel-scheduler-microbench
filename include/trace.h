#pragma once
#include "util.h"

namespace Tracing {
struct TaskTrace {
  TaskTrace() = default;

  TaskTrace(Timestamp created) : Created(created), ExecutionStart(Now()) {}

  void OnExecuted() { ExecutionEnd = Now(); }

  void OnWrite() { TraceWrite = Now(); }

  Timestamp Created{};
  Timestamp ExecutionStart{};
  Timestamp ExecutionEnd{};
  Timestamp TraceWrite{};
};

struct TaskInfo {
  TaskInfo() = default;

  TaskInfo(size_t taskIdx, Timestamp created) : Trace(created) {
    TaskIdx = taskIdx;
    ThreadIdx = GetThreadIndex();
    SchedCpu = sched_getcpu();
  }

  bool operator<(const TaskInfo &other) { return TaskIdx < other.TaskIdx; }

  size_t TaskIdx;
  int ThreadIdx;
  int SchedCpu;
  TaskTrace Trace;
};

} // namespace Tracing
