// Copyright 2015 Alec Thilenius
// All rights reserved.

#ifndef EXTERNAL_VANGUARD_TEST_CASE_H_
#define EXTERNAL_VANGUARD_TEST_CASE_H_

#include <string>
#include <unordered_map>
#include <vector>

#include "memory_monitor.h"

namespace thilenius {
namespace external {
namespace vanguard {

// Mirrors /anvil/anvil.thrift::TestCase
class TestCase {
 public:
  // Mirrors /anvil/anvil.thrift::Test
  // Represents a single assertion
  struct Test {
    bool did_pass;
    std::string name;
    std::string assert_message;
  };

  // Default initialization
  TestCase();

  // Called just before the test code is run, allowing things like memory
  // monitors to be attached on a per-test bases
  void BeginRun();

  // Called after a full test run. Allows things like memory monitors to be
  // detached and resolves the TestCase values from children tests
  void EndRun();

  // Asset a single test (called from a test in a suite)
  void ExpectTrue(bool value, const std::string& name,
                  const std::string& assert_message);

  // Mirros fields /anvil/anvil.thrift::TestCase
  bool did_pass;
  std::string name;
  int points_earned;
  int points_possible;
  int points_denominator;
  std::vector<Test> tests;

  // Used for per-test configuration
  int min_memory;
  int max_memory;
  bool leak_check;
  bool sig_seg_v_registered;
  bool timeout_registered;
  std::string exception_registered;
  MemoryMonitor memory_monitor;
};

}  // namespace vanguard
}  // namespace external
}  // namespace thilenius

#endif  // EXTERNAL_VANGUARD_TEST_CASE_H_
