// Copyright 2015 Alec Thilenius
// All rights reserved.

#ifndef EXTERNAL_VANGUARD_PROCESS_H_
#define EXTERNAL_VANGUARD_PROCESS_H_

#include <chrono>
#include <functional>
#include <string>
#include <unistd.h>
#include <vector>

namespace thilenius {
namespace external {
namespace vanguard {

// A recreation of parts of ::thilenius::base::process without boost
// TODO(athilenius): Assert that Execute is called before wait
// TODO(athilenius): Get rid of this hacky error code nonsense from Wait
class Process {
 public:
  static Process FromFork(std::function<int()> child_task);

  static Process FromExecv(const std::string& full_path,
                           const std::vector<std::string> args);

  bool Execute();

  int Wait(int timeout_ms);

  bool WriteCin(const std::string& value);

  std::string GetOut();

  std::string GetErr();

 private:
  Process() = default;

  std::function<int()> child_task_;
  pid_t pid_;
  int pipes_[3][2];
  std::chrono::time_point<std::chrono::system_clock> start_time_;
};

}  // namespace vanguard
}  // namespace external
}  // namespace thilenius

#endif  // EXTERNAL_VANGUARD_PROCESS_H_
