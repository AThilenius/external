// Copyright 2015 Alec Thilenius
// All rights reserved.

#include "printer.h"

#include <iomanip>
#include <iostream>
#include <math.h>
#include <signal.h>

std::ostream& operator<<(std::ostream& stream, ConsoleColor color) {
  switch (color) {
    case White: {
      std::cout << "\033[0m";
      break;
    }
    case Red: {
      std::cout << "\033[31m";
      break;
    }
    case Yellow: {
      std::cout << "\033[33m";
      break;
    }
    case Green: {
      std::cout << "\033[32m";
      break;
    }
    case Blue: {
      std::cout << "\033[36m";
      break;
    }
  }
  return stream;
}

namespace thilenius {
namespace external {
namespace vanguard {
namespace {

void WriteError(const std::string name, std::vector<std::string> lines,
                std::ostream* stream) {
  *stream << Blue << "|   |   " << Red << "- " << name << std::endl;
  for (const auto& line : lines) {
    *stream << Blue << "|   |   |   " << Yellow << line << std::endl;
  }
}

bool use_colors_ = true;

}  // namespace

Printer::Printer() : colorized(true) {}

void Printer::WriteStdCout(const Suite& suite, std::ostream* stream) {
  use_colors_ = colorized;
  *stream << std::endl;
  bool has_any_output = false;
  for (const auto& test_case : suite.test_cases) {
    if (test_case.stdcout.length() != 0) {
      has_any_output = true;
      break;
    }
  }
  if (!has_any_output) {
    *stream << Blue << "Standard Out: None" << std::endl;
  } else {
    int line_count_total = 0;
    int test_count = 0;
    *stream << Blue << "Standard Out:" << std::endl;
    *stream << Blue << "|" << std::endl;
    for (const auto& test_case : suite.test_cases) {
      if (test_case.stdcout.length() != 0) {
        test_count++;
        *stream << Blue << "|   Test: " << test_case.name << White << std::endl;
        std::stringstream sstream(test_case.stdcout);
        std::string line;
        int line_count = 0;
        while (std::getline(sstream, line)) {
          *stream << Blue << "|   |   " << White << line << std::endl;
          line_count++;
          line_count_total++;
        }
        *stream << Blue << "|   Displaying " << line_count << " lines" << White
                << std::endl;
        *stream << Blue << "|" << White << std::endl;
      }
    }
    *stream << Blue << "Displaying " << line_count_total << " total lines from "
            << test_count << " tests" << White << std::endl;
  }
}

void Printer::WriteSuiteResults(const Suite& suite, std::ostream* stream) {
  use_colors_ = colorized;
  *stream << std::endl;
  *stream << Blue << "Suite: " << suite.suite_name << std::endl;
  *stream << Blue << "|" << std::endl;
  for (const auto& test : suite.test_cases) {
    WriteTestCaseResults(test, stream);
  }
  if (suite.did_pass) {
    *stream << Green << "Passed!" << White << std::endl;
  } else {
    *stream << Red << "Failed!" << White << std::endl;
  }
}

void Printer::WriteSuiteReportCard(const Suite& suite, std::ostream* stream) {
  if (suite.points_possible > 0) {
    *stream << White << std::endl;
    *stream << Blue << "Points: ";
    int earned_percent_total =
        GetPercent(suite.points_earned, suite.points_denominator);
    PrintRatioColor(earned_percent_total, stream);
    *stream << suite.points_earned << "/" << suite.points_denominator << ", "
            << earned_percent_total << "%" << std::endl;
    *stream << Blue << "|" << std::endl;
    for (const auto& test_case : suite.test_cases) {
      *stream << Blue << "|   ";
      int earned_percent =
          GetPercent(test_case.points_earned, test_case.points_denominator);
      PrintRatioColor(earned_percent, stream);
      *stream << std::setw(3) << std::right << test_case.points_earned
              << std::setw(3) << std::left << " / " << std::setw(3) << std::left
              << test_case.points_denominator << std::setw(0) << std::left
              << Blue << " - " << test_case.name << std::endl;
    }
    *stream << Blue << "|" << std::endl;
    *stream << Blue << "[";
    PrintRatioColor(earned_percent_total, stream);
    for (int i = 0; i < 50; i++) {
      if (round(earned_percent_total / 2.0f) > i) {
        *stream << "=";
      } else {
        *stream << " ";
      }
    }
    if (suite.points_possible > suite.points_denominator) {
      *stream << Blue << "|" << Green;
      int extra_credit_points =
          suite.points_possible - suite.points_denominator;
      int extra_credit_percent =
          GetPercent(extra_credit_points, suite.points_denominator);
      int earned_extra_credit_percent = earned_percent_total - 100;
      for (int i = 0; i < extra_credit_percent; i++) {
        if (earned_extra_credit_percent > i) {
          *stream << "=";
        } else {
          *stream << " ";
        }
      }
    }
    *stream << Blue << "]" << White << std::endl;
    *stream << std::endl;
  }
  std::cout.flush();
}

void Printer::WriteTestCaseResults(const TestCase& test_case,
                                   std::ostream* stream) {
  *stream << Blue << "|   Test: " << test_case.name << std::endl;
  for (const auto& test : test_case.tests) {
    WriteTestResults(test, stream);
  }
  if (test_case.did_pass) {
    *stream << Blue << "|   " << Green << "Passed!" << std::endl;
  } else {
    if (test_case.min_memory != -1 &&
        test_case.min_memory > test_case.memory_monitor.ClosedSize()) {
      WriteError("Not enough head memory allocated!",
                 {"Minimum required heap allocations: " +
                      std::to_string(test_case.min_memory) + " Bytes",
                  "Total Heap Allocations: " +
                      std::to_string(test_case.memory_monitor.ClosedSize()) +
                      " Bytes"},
                 stream);
    }
    // Max check
    if (test_case.max_memory != -1 &&
        test_case.max_memory < test_case.memory_monitor.ClosedSize()) {
      WriteError("Too much heap memory allocated!",
                 {"Maximum allowed heap allocations: " +
                      std::to_string(test_case.max_memory) + " Bytes",
                  "Total heap allocations: " +
                      std::to_string(test_case.memory_monitor.ClosedSize()) +
                      " Bytes"},
                 stream);
    }
    // Leak check
    if (test_case.leak_check &&
        test_case.memory_monitor.OutstandingAllocationSize() > 0) {
      *stream << Blue << "|   |   " << Red
              << "- Memory Leak! \"new\" was called without a \"delete\" from:"
              << std::endl;
      for (const auto& allocation :
           test_case.memory_monitor.outstanding_allocations) {
        *stream << Blue << "|   |   |   " << Yellow << allocation.second.size
                << " Bytes from: " << allocation.second.file
                << ". line: " << allocation.second.line << std::endl;
      }
    }
    switch (test_case.termination_signal) {
      case SIGINT:
        WriteError("SIGINT Interrupt!",
                   {"Did you press ctrl-c? Don't do that if you did."}, stream);
        break;
      case SIGQUIT:
        WriteError("SIGQUIT Quit!", {"Not sure why, sorry."}, stream);
        break;
      case SIGILL:
        WriteError("SIGILL Illegal Instruction!",
                   {"The test executed an illegal CPU instruction."}, stream);
        break;
      case SIGABRT:
        WriteError("SIGABRT Abort!", {"Abort was called, not good news."},
                   stream);
        break;
      case SIGSEGV:
        WriteError("SIGSEGV Segmentation Fault!",
                   {"You are probably trying to dereference a null or invalid "
                    "pointer."},
                   stream);
        break;
      case SIGFPE:
        WriteError("SIGFPE Floating-Point Exception!",
                   {"You are probably trying to divide something by zero."},
                   stream);
        break;
      case SIGBUS:
        WriteError("SIGBUS Bus Error!", {"Not sure why, sorry."}, stream);
        break;
      case EXIT_FAILURE:
        WriteError(
            "Hard Termination",
            {"The test crashed spectacularly. Could be a stack overflow?"},
            stream);
    }
    if (test_case.timeout_registered) {
      WriteError("Timeout!", {"You probably have an infinite loop."}, stream);
    }
    if (test_case.std_exception_present) {
      WriteError("Exception!", {"A fatal exception was throw"}, stream);
    }
    *stream << Blue << "|   " << Red << "Failed!" << White << std::endl;
  }
  *stream << Blue << "|" << std::endl;
}

void Printer::WriteTestResults(const TestCase::Test& test,
                               std::ostream* stream) {
  if (test.did_pass) {
    *stream << Blue << "|   |   " << Green << "-" << test.name << std::endl;
  } else {
    *stream << Blue << "|   |   " << Red << "-" << test.name << std::endl;
    *stream << Blue << "|   |   |   " << Yellow << test.assert_message
            << std::endl;
  }
}

void Printer::PrintRatioColor(int percent, std::ostream* stream) {
  if (percent < 70) {
    *stream << Red;
  } else if (percent < 90) {
    *stream << Yellow;
  } else {
    *stream << Green;
  }
}

int Printer::GetPercent(int nominator, int denominator) {
  if (denominator == 0) {
    return 100;
  }
  return static_cast<int>(100.0f * static_cast<float>(nominator) /
                          static_cast<float>(denominator));
}

}  // namespace vanguard
}  // namespace external
}  // namespace thilenius
