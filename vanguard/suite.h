// Copyright 2015 Alec Thilenius
// All rights reserved.

#ifndef EXTERNAL_VANGUARD_SUITE_H_
#define EXTERNAL_VANGUARD_SUITE_H_

#include <functional>
#include <string>
#include <vector>

#include "test_case.h"

#define SUITE(suite_name) void suite_name()
#define TEST(test_name, points_possible, points_denominator)                   \
  if (::thilenius::external::vanguard::Suite::ActiveSuite().ActivateTestPhase( \
          test_name, points_possible, points_denominator))
#define EXPECT_TRUE(val, name, assert_message)          \
  ::thilenius::external::vanguard::Suite::ActiveSuite() \
      .ActiveTestCase()                                 \
      .ExpectTrue(val, name, assert_message)

namespace thilenius {
namespace external {
namespace vanguard {

class Suite {
 public:
  // The type of the suite lambda
  typedef std::function<void()> SuiteFunction;

  // Creates a suite from an anonymous lambda and a name
  Suite(const std::string& suite_name, const SuiteFunction& suite_function);

  // Returns the currently running suite (only valid to call from within a suite
  // function)
  static Suite& ActiveSuite();

  // Returns the currently active test case (only valid to call from within a
  // test function)
  TestCase& ActiveTestCase();

  // Used by a suite function to run a single stage at a time, giving the suite
  // control over scope. Because the behaviour of this changes, this function is
  // actually just a pass-through to a closure.
  bool ActivateTestPhase(const std::string& test_name, int points_possible,
                         int points_denominator);

  // Runs the suite, from the last run test case index + 1. This should be
  // called at the end of a protective rack, on a separate thread
  void RunOrRerunSuite();

  // The name of the test suite
  const std::string suite_name;

  // All test cases, in order of execution
  std::vector<TestCase> test_cases;

  // Valid after RunOrRerunSuite has been called and returned
  bool did_pass;
  int points_denominator;
  int points_earned;
  int points_possible;

 private:
  const SuiteFunction suite_function_;
  int active_test_index_;
  std::function<bool(const std::string&, int, int)> active_test_closure_;
};

}  // namespace vanguard
}  // namespace external
}  // namespace thilenius

#endif  // EXTERNAL_VANGUARD_SUITE_H_
