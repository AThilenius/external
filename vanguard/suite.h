// Copyright 2015 Alec Thilenius
// All rights reserved.

#ifndef EXTERNAL_VANGUARD_SUITE_H_
#define EXTERNAL_VANGUARD_SUITE_H_

#include <functional>
#include <string>
#include <vector>

#include "json.h"
#include "test_case.h"

#define SUITE(suite_name)                                        \
  void suite_name(::thilenius::external::vanguard::Suite* suite, \
                  ::thilenius::external::vanguard::TestCase* test_case)
#define TEST(test_name, points_possible, points_denominator)                   \
  if ((suite &&                                                                \
       suite->RegisterTest(test_name, points_possible, points_denominator)) || \
      (test_case && test_case->name == test_name))
#define EXPECT_TRUE(val, name, assert_message) \
  test_case->ExpectTrue(val, name, assert_message)

namespace thilenius {
namespace external {
namespace vanguard {

class Suite {
 public:
  // The type of the suite lambda
  typedef std::function<void(Suite*, TestCase*)> SuiteFunction;

  // Creates a suite from an anonymous lambda and a name
  Suite(const std::string& suite_name, const SuiteFunction& suite_function);

  // Registers a new tests with the suite, always returns false
  bool RegisterTest(const std::string& test_name, int points_possible,
                    int points_denominator);

  // Runs all test cases in their own children processes
  void RunAllTestCasesProtected();

  // Serialize a Suite to Json
  ::nlohmann::json ToJson() const;

  // Deserialize a suite from Json. Returns a blank suite on failure
  static Suite FromJson(const ::nlohmann::json& json);

  std::string suite_name;
  std::vector<TestCase> test_cases;
  bool did_pass;
  int points_denominator;
  int points_earned;
  int points_possible;

 private:
  const SuiteFunction suite_function_;
};

}  // namespace vanguard
}  // namespace external
}  // namespace thilenius

#endif  // EXTERNAL_VANGUARD_SUITE_H_
