// Copyright 2015 Alec Thilenius
// All rights reserved.

#include "vanguard.h"

#include <limits.h>
#include <unordered_map>

#include "process.h"

namespace thilenius {
namespace external {
namespace vanguard {

Vanguard::Vanguard(int argc, char* argv[])
    : execution_mode_(ExecutionMode::HARNESSED_PARENT),
      output_stream_(&std::cout),
      anvil_path_("/usr/bin/anvil") {
  for (int i = 0; i < argc; i++) {
    std::string arg = argv[i];
    while (arg.length() > 0 && arg[0] == '-') {
      arg = arg.substr(1, arg.length() - 1);
    }
    // If this arg starts with - and the next doesn't start with -
    if (argv[i][0] == '-' && i + 1 < argc && argv[i + 1][0] != '-') {
      // Its a value arg: --hello world
      flags_[arg] = argv[i + 1];
      i++;
    } else {
      flags_[arg] = "";
    }
  }
  // Exit for help flag
  if (flags_.find("help") != flags_.end()) {
    std::cout << "Vanguard flags:" << std::endl
              << "  --version       Prints the version number of the Vanguard "
                 "executable" << std::endl
              << "  --no_avil       Runs tests in off-line mode, without Anvil"
              << std::endl
              << "  --anvil_path    Uses the Anvil binary at the given path "
                 "instead of "
                 "/usr/bin/anvil" << std::endl
              << "  --output_stream Uses one of [cout, cerr, null] as the "
                 "output stream" << std::endl
              << "  --no_color      Runs tests without colorized output"
              << std::endl;
    exit(EXIT_SUCCESS);
  }
  // Exit for version flag
  if (flags_.find("version") != flags_.end()) {
    std::cout << VANGUARD_VERSION << std::endl;
    exit(EXIT_SUCCESS);
  }
  if (flags_.find("harness") != flags_.end()) {
    execution_mode_ = ExecutionMode::HARNESSED_CHILD;
  }
  // Let forced fall-back override harnessing
  if (flags_.find("offline") != flags_.end()) {
    execution_mode_ = ExecutionMode::FORCED_DIRECT;
  }
  if (flags_.find("no_color") != flags_.end()) {
    printer_.colorized = false;
  }
  if (flags_.find("anvil_path") != flags_.end()) {
    anvil_path_ = flags_["anvil_path"];
  }
  if (flags_.find("output_stream") != flags_.end()) {
    if (flags_["output_stream"] == "cout") {
      output_stream_ = &std::cout;
    } else if (flags_["output_stream"] == "cerr") {
      output_stream_ = &std::cerr;
    } else if (flags_["output_stream"] == "null") {
      output_stream_ = new std::stringstream();
    } else {
      std::cout << Red << "No stream type \"" << flags_["output_stream"] << "\""
                << std::endl;
      exit(EXIT_FAILURE);
    }
  }
  // TODO(athilenius): Warn of unused flags
  // Test for Anvil existence
  Process exec_process = Process::FromExecv(anvil_path_, {"--version"});
  if (execution_mode_ != ExecutionMode::FORCED_DIRECT &&
      (!exec_process.Execute() || !exec_process.Wait(1000) ||
       exec_process.GetOut().length() == 0)) {
    execution_mode_ = ExecutionMode::FALLBACK_DIRECT;
  }
}

void Vanguard::RunSuite(const Suite::SuiteFunction& suite_function,
                        const std::string& name) {
  Suite suite(name, suite_function);
  switch (execution_mode_) {
    case ExecutionMode::FORCED_DIRECT: {
      // Run normally, with a warning at the end
      suite.RunAllTestCasesProtected();
      printer_.WriteStdCout(suite, output_stream_);
      printer_.WriteSuiteResults(suite, output_stream_);
      printer_.WriteSuiteReportCard(suite, output_stream_);
      std::cout
          << Yellow << "Vanguard was run with the --offline flag" << std::endl
          << "Nothing has been submitted for grading to Crucible or Chronicle."
          << White << std::endl;
      break;
    }
    case ExecutionMode::FALLBACK_DIRECT: {
      // Run normally, with a warning at the end
      suite.RunAllTestCasesProtected();
      printer_.WriteStdCout(suite, output_stream_);
      printer_.WriteSuiteResults(suite, output_stream_);
      printer_.WriteSuiteReportCard(suite, output_stream_);
      std::cout
          << Red << "Vanguard failed to find Anvil installed on your system! "
                    "Tests are running off-line!" << std::endl
          << "Nothing has been submitted for grading to Crucible or Chronicle."
          << White << std::endl;
      break;
    }
    case ExecutionMode::HARNESSED_PARENT: {
      // Pass execution off to Anvil
      char buff[PATH_MAX];
      ssize_t len = ::readlink("/proc/self/exe", buff, sizeof(buff) - 1);
      if (len != -1) {
        buff[len] = '\0';
      } else {
        std::cout
            << Red
            << "Something went wrong while trying to find Vanguard's path :("
            << std::endl;
        exit(EXIT_FAILURE);
      }
      Process process = Process::FromExecv(anvil_path_, {"--harness", buff});
      if (!process.Execute()) {
        std::cout << Red << "Something went horribly wrong while trying to "
                            "pass execution off to Anvil :(" << std::endl;
        exit(EXIT_FAILURE);
      }
      int exit_status = process.Wait(60000);
      std::string anvil_cerr = process.GetErr();
      std::string anvil_cout = process.GetOut();
      // TODO(athilenius): This is hacky. Return a JSON object form Anvil
      if (exit_status < 0 || anvil_cerr != "OK") {
        std::cout << Red << "Something went horribly wrong while trying to "
                            "pass execution off to Anvil :(" << std::endl;
        exit(EXIT_FAILURE);
      }
      break;
    }
    case ExecutionMode::HARNESSED_CHILD: {
      // Run the test, and collect JSON. Send it to cerr.
      suite.RunAllTestCasesProtected();
      std::string suite_json = suite.ToJson().dump();
      std::cerr << "<JSON>" << suite_json << "</JSON>" << std::endl;
      break;
    }
  }
}

}  // namespace vanguard
}  // namespace external
}  // namespace thilenius
