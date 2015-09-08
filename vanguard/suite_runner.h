// Copyright 2015 Alec Thilenius
// All rights reserved.

#ifndef EXTERNAL_VANGUARD_SUITE_RUNNER_H_
#define EXTERNAL_VANGUARD_SUITE_RUNNER_H_

#include "suite.h"
#include "test_case.h"

namespace thilenius {
namespace external {
namespace vanguard {

// A highly protected and isolated runner, capable of catching infinite loops,
// exceptions, and segmentation faults
class SuiteRunner {
 public:
  SuiteRunner(int* argc, char*** argv);
  void RunSuite(Suite* suite);
};

}  // namespace vanguard
}  // namespace external
}  // namespace thilenius

#endif  // EXTERNAL_VANGUARD_SUITE_RUNNER_H_
