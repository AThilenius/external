// Copyright 2015 Alec Thilenius
// All rights reserved.

#include "memory_monitor.h"

#include <sstream>

// Remove the macro wrappers around new
#undef new
#undef delete

// TODO(athilenius): Make these not global. This shit is gross
const char* __file__ = "unknown";
size_t __line__ = -1;

namespace thilenius {
namespace external {
namespace vanguard {
namespace {

// Static local-scope accessor for pre-main ordered static initialization
MemoryMonitor*& ActiveMemoryMonitor() {
  static MemoryMonitor* active_memory_monitor = nullptr;
  return active_memory_monitor;
}

bool& FirstOrderHanlderActive() {
  static bool first_order_handler_active = false;
  return first_order_handler_active;
}

}  // namespace
}  // namespace vanguard
}  // namespace external
}  // namespace thilenius

using ::thilenius::external::vanguard::ActiveMemoryMonitor;
using ::thilenius::external::vanguard::FirstOrderHanlderActive;
using ::thilenius::external::vanguard::MemoryMonitor;

void* operator new(size_t size) {
  if (ActiveMemoryMonitor() == nullptr || FirstOrderHanlderActive() ||
      __line__ == -1) {
    return malloc(size);
  } else {
    void* ptr = malloc(size);
    FirstOrderHanlderActive() = true;
    try {
      MemoryMonitor::MemoryAllocation memory_allocation{ptr, size, __file__,
                                                        __line__};
      std::vector<std::string> tokens;
      std::istringstream stringStream(__file__);
      std::string token;
      while (std::getline(stringStream, token, '\\')) {
        tokens.push_back(token);
      }
      if (tokens.size() > 1) {
        memory_allocation.file = tokens[tokens.size() - 1];
      } else {
        // Split on /
        std::istringstream string_stream_forward(__file__);
        tokens.clear();
        while (std::getline(string_stream_forward, token, '/')) {
          tokens.push_back(token);
        }
        if (tokens.size() > 1) {
          memory_allocation.file = tokens[tokens.size() - 1];
        }
      }
      ActiveMemoryMonitor()->outstanding_allocations.insert(
          {ptr, std::move(memory_allocation)});
    } catch (...) {
      // noexcept
    }
    FirstOrderHanlderActive() = false;
    __file__ = "unknown";
    __line__ = -1;
    return ptr;
  }
}

void operator delete(void* ptr) noexcept {
  if (ActiveMemoryMonitor() == nullptr || FirstOrderHanlderActive() ||
      __line__ == -1) {
    free(ptr);
  } else {
    FirstOrderHanlderActive() = true;
    free(ptr);
    try {
      auto iter = ActiveMemoryMonitor()->outstanding_allocations.find(ptr);
      if (iter != ActiveMemoryMonitor()->outstanding_allocations.end()) {
        // Close correctly
        ActiveMemoryMonitor()->closed_allocations.push_back(iter->second);
        ActiveMemoryMonitor()->outstanding_allocations.erase(iter);
      } else {
        // delete called without new being called
        ActiveMemoryMonitor()->outstanding_frees.insert(
            {iter->first, iter->second});
      }
    } catch (...) {
      // noexcept
    }
    FirstOrderHanlderActive() = false;
    __file__ = "unknown";
    __line__ = -1;
  }
}

namespace thilenius {
namespace external {
namespace vanguard {

void MemoryMonitor::Attach() { ActiveMemoryMonitor() = this; }

void MemoryMonitor::Detach() { ActiveMemoryMonitor() = nullptr; }

size_t MemoryMonitor::OutstandingAllocationSize() const {
  size_t outstanding_allocation_size = 0;
  for (const auto& allocation : outstanding_allocations) {
    outstanding_allocation_size += allocation.second.size;
  }
  return outstanding_allocation_size;
}

size_t MemoryMonitor::ClosedSize() const {
  size_t closed_allocation_size = 0;
  for (const auto& allocation : closed_allocations) {
    closed_allocation_size += allocation.size;
  }
  return closed_allocation_size;
}

}  // namespace vanguard
}  // namespace external
}  // namespace thilenius
