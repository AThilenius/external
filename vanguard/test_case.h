// Copyright 2015 Alec Thilenius
// All rights reserved.

#ifndef EXTERNAL_VANGUARD_TEST_CASE_H_
#define EXTERNAL_VANGUARD_TEST_CASE_H_

#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

#include "json.h"
#include "memory_monitor.h"

namespace thilenius {
namespace external {
namespace vanguard {

class Suite;

// Mirrors /anvil/anvil.thrift::TestCase
class TestCase {
 public:
  // Mirrors /anvil/anvil.thrift::Test
  // Represents a single assertion
  struct Test {
    bool did_pass;
    std::string name;
    std::string assert_message;

    // Serializes a test to Json
    ::nlohmann::json ToJson() const;

    // Deserializes a test from Json. Returns a blank test case on error.
    static Test FromJson(const ::nlohmann::json& json);
  };

  // Default initialization
  explicit TestCase(std::function<void(Suite*, TestCase*)> suite_function);

  // Asset a single test (called from a test in a suite)
  void ExpectTrue(bool value, const std::string& name,
                  const std::string& assert_message);

  // Serializes a test case to Json
  ::nlohmann::json ToJson() const;

  // Deserializes a test case from Json. Returns a blank test case on error.
  static TestCase FromJson(const ::nlohmann::json& json);

  // Runs the test case in a separate process space, using JSON over stderr for
  // communication. Returns a new test case object is successful or the same
  // object if a run failed.
  TestCase RunProtected();

  // Crunches the numbers for things like points_earned and did_pass
  void ProcessData();

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
  bool std_exception_present;
  bool timeout_registered;
  int termination_signal;
  MemoryMonitor memory_monitor;
  std::string stdcout;

  // Not captured during serialization
  int timeout_ms;

 private:
  // Runs the test case, writes results to stderr (in json) and exits the
  // current assembly
  void RunTestCaseAndExit();

  const std::function<void(Suite*, TestCase*)> suite_function_;
};

}  // namespace vanguard
}  // namespace external
}  // namespace thilenius

#endif  // EXTERNAL_VANGUARD_TEST_CASE_H_
