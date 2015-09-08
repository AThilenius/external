// Copyright 2015 Alec Thilenius
// All rights reserved.

#include "suite.h"

namespace thilenius {
namespace external {
namespace vanguard {
namespace {

Suite* active_suite_ = nullptr;

}  // namespace

Suite::Suite(const std::string& suite_name, const SuiteFunction& suite_function)
    : did_pass(false),
      points_denominator(0),
      points_earned(0),
      points_possible(0),
      suite_function_(suite_function),
      suite_name(suite_name),
      active_test_index_(-1) {
  // Scrape the suite, to collect all test names
  active_suite_ = this;
  active_test_closure_ =
      [this](const std::string& test_name, int points_possible,
             int points_denominator) -> bool {
        TestCase test_case;
        test_case.name = test_name;
        test_case.points_possible = points_possible;
        test_case.points_denominator = points_denominator;
        test_cases.emplace_back(std::move(test_case));
        return false;
      };
  suite_function_();
  active_suite_ = nullptr;
  active_test_closure_ = [](const std::string& test_name, int points_possible,
                            int points_denominator) -> bool {
    return test_name == Suite::ActiveSuite().ActiveTestCase().name;
  };
}

// static
Suite& Suite::ActiveSuite() { return *active_suite_; }

TestCase& Suite::ActiveTestCase() { return test_cases[active_test_index_]; }

bool Suite::ActivateTestPhase(const std::string& test_name, int points_possible,
                              int points_denominator) {
  return active_test_closure_(test_name, points_possible, points_denominator);
}

void Suite::RunOrRerunSuite() {
  active_suite_ = this;
  // If active_test_index_ isn't -1 then this is a recovery call, make sure we
  // call EndRun on the old test case
  if (active_test_index_ > -1) {
    test_cases[active_test_index_].EndRun();
  }
  // Increment first, to skip failed tests if this is a rerun call
  active_test_index_++;
  while (active_test_index_ < (int)test_cases.size()) {
    ActiveTestCase().BeginRun();
    suite_function_();
    ActiveTestCase().EndRun();
    active_test_index_++;
  }
  did_pass = true;
  for (const auto& test_case : test_cases) {
    if (!test_case.did_pass) {
      did_pass = false;
    }
    points_denominator += test_case.points_denominator;
    points_earned += test_case.points_earned;
    points_possible += test_case.points_possible;
  }
  active_suite_ = nullptr;
}

}  // namespace vanguard
}  // namespace external
}  // namespace thilenius
