// Copyright 2015 Alec Thilenius
// All rights reserved.

#include "test_case.h"

#include <math.h>
#include <sstream>
#include <string>
#include <string>
#include <unordered_map>
#include <vector>

namespace thilenius {
namespace external {
namespace vanguard {

TestCase::TestCase()
    : did_pass(false),
      name("Unknown"),
      points_earned(0),
      points_possible(0),
      points_denominator(0),
      timeout_registered(false),
      sig_seg_v_registered(false),
      min_memory(-1),
      max_memory(-1),
      leak_check(false) {}

void TestCase::BeginRun() { memory_monitor.Attach(); }

void TestCase::EndRun() {
  memory_monitor.Detach();
  did_pass = true;
  bool fatal_failure = false;
  for (const auto& test : tests) {
    if (!test.did_pass) {
      did_pass = false;
    }
  }
  // Min check
  if (min_memory != -1 && min_memory > memory_monitor.ClosedSize()) {
    did_pass = false;
    fatal_failure = true;
  }
  // Max check
  if (max_memory != -1 && max_memory < memory_monitor.ClosedSize()) {
    did_pass = false;
    fatal_failure = true;
  }
  // Leak check
  if (leak_check && memory_monitor.OutstandingAllocationSize() > 0) {
    did_pass = false;
    fatal_failure = true;
  }
  if (sig_seg_v_registered || timeout_registered ||
      exception_registered.length() != 0) {
    did_pass = false;
    fatal_failure = true;
  }
  // Points earned
  if (fatal_failure) {
    // No partial credit awarded in the presense of a fatal failure
    return;
  }
  int passed_conditions = 0;
  for (const auto& test : tests) {
    if (test.did_pass) {
      passed_conditions += 1;
    }
  }
  if (tests.size() == 0) {
    // No tests and no fatal failures means full credit
    points_earned = points_possible;
  } else {
    points_earned = round(static_cast<float>(points_possible) *
                          static_cast<float>(passed_conditions) /
                          static_cast<float>(tests.size()));
  }
}

void TestCase::ExpectTrue(bool value, const std::string& name,
                          const std::string& assert_message) {
  Test test{value, name, assert_message};
  tests.emplace_back(std::move(test));
}

}  // namespace vanguard
}  // namespace external
}  // namespace thilenius
