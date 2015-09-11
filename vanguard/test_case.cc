// Copyright 2015 Alec Thilenius
// All rights reserved.

#include "test_case.h"

#include "process.h"

#include <math.h>
#include <signal.h>
#include <sstream>
#include <string>
#include <string>
#include <unordered_map>
#include <vector>

namespace thilenius {
namespace external {
namespace vanguard {
namespace {

void EndRun(TestCase* test_case) {
  test_case->memory_monitor.Detach();
  test_case->ProcessData();
  std::cerr << "<JSON>" << test_case->ToJson().dump() << "</JSON>" << std::endl;
  exit(EXIT_SUCCESS);
}

// Signal handling
TestCase* active_test_case_ = nullptr;

void SignalTrap(int signum) {
  if (active_test_case_ == nullptr) {
    std::cout << "Null active test, recovery impossible." << std::endl;
    exit(EXIT_SUCCESS);
  } else {
    active_test_case_->termination_signal = signum;
    EndRun(active_test_case_);
  }
}

}  // namespace

::nlohmann::json TestCase::Test::ToJson() const {
  return {{"did_pass", did_pass},
          {"name", name},
          {"assert_message", assert_message}};
}

TestCase::Test TestCase::Test::FromJson(const ::nlohmann::json& json) {
  TestCase::Test test;
  try {
    test.did_pass = json["did_pass"].get<bool>();
    test.name = json["name"].get<std::string>();
    test.assert_message = json["assert_message"].get<std::string>();
  } catch (...) {
  }
  return std::move(test);
}

TestCase::TestCase(std::function<void(Suite*, TestCase*)> suite_function)
    : did_pass(false),
      name("Unknown"),
      points_earned(0),
      points_possible(0),
      points_denominator(0),
      std_exception_present(false),
      timeout_registered(false),
      termination_signal(-1),
      min_memory(-1),
      max_memory(-1),
      leak_check(false),
      suite_function_(suite_function),
      timeout_ms(4000) {}

void TestCase::RunTestCaseAndExit() {
  active_test_case_ = this;
  // Trap any termainl signals
  signal(SIGINT, SignalTrap);
  signal(SIGQUIT, SignalTrap);
  signal(SIGILL, SignalTrap);
  signal(SIGABRT, SignalTrap);
  signal(SIGSEGV, SignalTrap);
  signal(SIGFPE, SignalTrap);
  signal(SIGBUS, SignalTrap);
  // Monitor memory
  memory_monitor.Attach();
  try {
    // Run the test case
    suite_function_(nullptr, this);
  } catch (...) {
    std_exception_present = true;
  }
  EndRun(this);
}

void TestCase::ExpectTrue(bool value, const std::string& name,
                          const std::string& assert_message) {
  Test test{value, name, assert_message};
  tests.emplace_back(std::move(test));
}

::nlohmann::json TestCase::ToJson() const {
  // Ensure we eneded the test
  ::nlohmann::json json = {{"did_pass", did_pass},
                           {"name", name},
                           {"points_earned", points_earned},
                           {"points_possible", points_possible},
                           {"points_denominator", points_denominator},
                           {"tests", ::nlohmann::json::array()},
                           {"min_memory", min_memory},
                           {"max_memory", max_memory},
                           {"leak_check", leak_check},
                           {"std_exception_present", std_exception_present},
                           {"timeout_registered", timeout_registered},
                           {"termination_signal", termination_signal},
                           {"memory_monitor", memory_monitor.ToJson()}};
  for (const auto& test : tests) {
    json["tests"].push_back(test.ToJson());
  }
  return std::move(json);
}

TestCase TestCase::FromJson(const ::nlohmann::json& json) {
  TestCase test_case([](Suite*, TestCase*) {});
  try {
    test_case.did_pass = json["did_pass"].get<bool>();
    test_case.name = json["name"].get<std::string>();
    test_case.points_earned = json["points_earned"].get<int>();
    test_case.points_possible = json["points_possible"].get<int>();
    test_case.points_denominator = json["points_denominator"].get<int>();
    for (const auto& test_json : json["tests"]) {
      test_case.tests.emplace_back(TestCase::Test::FromJson(test_json));
    }
    test_case.min_memory = json["min_memory"].get<int>();
    test_case.max_memory = json["max_memory"].get<int>();
    test_case.leak_check = json["leak_check"].get<bool>();
    test_case.std_exception_present = json["std_exception_present"].get<bool>();
    test_case.timeout_registered = json["timeout_registered"].get<bool>();
    test_case.termination_signal = json["termination_signal"].get<int>();
    test_case.memory_monitor = MemoryMonitor::FromJson(json["memory_monitor"]);
  } catch (...) {
  }
  return std::move(test_case);
}

void TestCase::ProcessData() {
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
  if (timeout_registered || std_exception_present || termination_signal != -1) {
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

TestCase TestCase::RunProtected() {
  did_pass = false;
  try {
    Process child = Process::FromFork([this]() -> int {
      RunTestCaseAndExit();
      return EXIT_FAILURE;
    });
    child.Execute();
    int process_return_code = child.Wait(timeout_ms);
    if (process_return_code == -1) {
      // Timed out, use this test
      timeout_registered = true;
      ProcessData();
    } else if (process_return_code == -2) {
      termination_signal = EXIT_FAILURE;
      ProcessData();
    } else {
      // Try to deserialize from JSON on stderr
      std::string stderr_output = child.GetErr();
      int first = stderr_output.find("<JSON>");
      int last = stderr_output.find("</JSON>");
      std::string json_text = stderr_output.substr(first, last - first);
      json_text = json_text.substr(first + 6);
      ::nlohmann::json json = ::nlohmann::json::parse(json_text);
      // Quick sanity check (with all the insane shit going on around here)
      if (json["name"].get<std::string>() != name) {
        throw;
      }
      // Looks like it worked
      return std::move(TestCase::FromJson(json));
    }
  } catch (...) {
    // Fatal, unknown crash
    termination_signal = EXIT_FAILURE;
    ProcessData();
  }
  return *this;
}

}  // namespace vanguard
}  // namespace external
}  // namespace thilenius
