// Copyright 2015 Alec Thilenius
// All rights reserved.

#include "suite.h"

namespace thilenius {
namespace external {
namespace vanguard {

Suite::Suite(const std::string& suite_name, const SuiteFunction& suite_function)
    : did_pass(false),
      points_denominator(0),
      points_earned(0),
      points_possible(0),
      suite_function_(suite_function),
      suite_name(suite_name) {
  // Scrape the suite, to collect all test names
  suite_function_(this, nullptr);
}
bool RegisterTest(const std::string& test_name, int points_possible,
                  int points_denominator);

bool Suite::RegisterTest(const std::string& test_name, int points_possible,
                         int points_denominator) {
  TestCase test_case(suite_function_);
  test_case.name = test_name;
  test_case.points_possible = points_possible;
  test_case.points_denominator = points_denominator;
  test_cases.push_back(std::move(test_case));
  return false;
}

void Suite::RunAllTestCasesProtected() {
  std::vector<TestCase> run_test_cases;
  for (auto& test_case : test_cases) {
    run_test_cases.emplace_back(test_case.RunProtected());
  }
  test_cases.clear();
  for (auto&& test_case : run_test_cases) {
    test_cases.emplace_back(std::move(test_case));
  }
  did_pass = true;
  for (const auto& test_case : test_cases) {
    if (!test_case.did_pass) {
      did_pass = false;
    }
    points_earned += test_case.points_earned;
    points_possible += test_case.points_possible;
    points_denominator += test_case.points_denominator;
  }
}

::nlohmann::json Suite::ToJson() const {
  ::nlohmann::json json = {{"suite_name", suite_name},
                           {"points_earned", points_earned},
                           {"points_possible", points_possible},
                           {"points_denominator", points_denominator},
                           {"did_pass", did_pass},
                           {"test_cases", ::nlohmann::json::array()}};
  for (const auto& test_case : test_cases) {
    json["test_cases"].push_back(test_case.ToJson());
  }
  return std::move(json);
}

Suite Suite::FromJson(const ::nlohmann::json& json) {
  Suite suite("deserialize_error", [](Suite*, TestCase*) {});
  try {
    suite.suite_name = json["suite_name"].get<std::string>();
    suite.points_earned = json["points_earned"].get<int>();
    suite.points_possible = json["points_possible"].get<int>();
    suite.points_denominator = json["points_denominator"].get<int>();
    suite.did_pass = json["did_pass"].get<bool>();
    for (const auto& test_case_json : json["test_cases"]) {
      suite.test_cases.emplace_back(TestCase::FromJson(test_case_json));
    }
  } catch (...) {
  }
  return std::move(suite);
}

}  // namespace vanguard
}  // namespace external
}  // namespace thilenius
