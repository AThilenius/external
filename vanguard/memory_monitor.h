// Copyright 2015 Alec Thilenius
// All rights reserved.

#ifndef EXTERNAL_VANGUARD_MEMORY_MONITOR_H_
#define EXTERNAL_VANGUARD_MEMORY_MONITOR_H_

#include <cstdlib>
#include <string>
#include <unordered_map>
#include <vector>

#include "json.h"

// Overriding the new operator
void* operator new(size_t size);

// Overriding the delete operator
void operator delete(void* ptr) noexcept;

// Used to track file and line numbers on new and delete calls
extern const char* __file__;
extern size_t __line__;
#define new (__file__ = __FILE__, __line__ = __LINE__) && 0 ? NULL : new
#define delete (__file__ = __FILE__, __line__ = __LINE__), delete

namespace thilenius {
namespace external {
namespace vanguard {

class MemoryMonitor {
 public:
  // Represents a single, outstanding allocation or free
  struct MemoryAllocation {
    void* address;
    size_t size;
    std::string file;
    size_t line;

    // Serializes a Memory Allocation to json
    ::nlohmann::json ToJson() const;

    // Deserializes a Memory Allocation from json. Returns an empty allocation
    // on error.
    static MemoryAllocation FromJson(const ::nlohmann::json& json);
  };

  // Clears and activates this memory monitor (deactivating any other active
  // monitors)
  void Attach();

  // Deactivates the monitor (using default new/delete mechanics)
  void Detach();

  // The size of all memory allocated but never freed
  size_t OutstandingAllocationSize() const;

  // The size of all memory allocated and freed
  size_t ClosedSize() const;

  // Serializes a Memory Monitor to json
  ::nlohmann::json ToJson() const;

  // Deserializes a Memory Monitor from json. Returns an empty allocation
  // on error.
  static MemoryMonitor FromJson(const ::nlohmann::json& json);

  // Outstanding allocation, new called without a delete
  std::unordered_map<void*, MemoryAllocation> outstanding_allocations;

  // Outstanding frees, free called without a delete
  std::unordered_map<void*, MemoryAllocation> outstanding_frees;

  // Closed memory, new called with a delete
  std::vector<MemoryAllocation> closed_allocations;
};

}  // namespace vanguard
}  // namespace external
}  // namespace thilenius

#endif  // EXTERNAL_VANGUARD_MEMORY_MONITOR_H_
