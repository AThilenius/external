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

// Does nothing more than parse command line flags and invoke Vanguard suites in
// the correct mode
class Vanguard {
 public:
  Vanguard(int argc, char* argv[]);

  void RunSuite(const Suite::SuiteFunction& suite_function,
                const std::string& name);

 private:
  enum class ExecutionMode {
    // The flag --no_anvil was passed, harnessing will be skipped
    FORCED_DIRECT,
    // Anvil was not found
    FALLBACK_DIRECT,
    // Parent of Anvil process
    HARNESSED_PARENT,
    // Child of Anvil process
    HARNESSED_CHILD
  };

  std::unordered_map<std::string, std::string> flags_;
  // Parsed as flags
  ExecutionMode execution_mode_;
  Printer printer_;
  std::ostream* output_stream_;
  std::string anvil_path_;
};

}  // namespace vanguard
}  // namespace external
}  // namespace thilenius

#endif  // EXTERNAL_VANGUARD_VANGUARD_H_
