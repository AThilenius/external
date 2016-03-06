// Copyright 2015 Alec Thilenius
// All rights reserved.

#ifndef EXTERNAL_VANGUARD_VANGUARD_H_
#define EXTERNAL_VANGUARD_VANGUARD_H_

#define VANGUARD_VERSION "0.2.0"

#include <ostream>
#include <string>
#include <unordered_map>

#include "printer.h"
#include "suite.h"

namespace thilenius {
namespace external {
namespace vanguard {

class Vanguard {
 public:
  Vanguard(int argc, char* argv[]);

  void RunSuite(const Suite::SuiteFunction& suite_function,
                const std::string& name);

 private:
  Printer printer_;
  bool offline_;
  std::ostream* output_stream_;
  std::string chronicle_path_;
  std::string crucible_path_;
  std::string sentinel_path_;
  std::unordered_map<std::string, std::string> flags_;
};

}  // namespace vanguard
}  // namespace external
}  // namespace thilenius

#endif  // EXTERNAL_VANGUARD_VANGUARD_H_
