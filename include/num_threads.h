#pragma once

#include "modes.h"
#include <cstddef>
#include <string>
#include <thread>

inline int GetNumThreads() {
  // TODO(blonded04): actually you need a way to programmatically find number of cores on 1 NUMA node
  return 24;
}
