// Copyright 2015 Alec Thilenius
// All rights reserved.

#include "printer.h"

#include <iomanip>
#include <iostream>
#include <math.h>

namespace thilenius {
namespace external {
namespace vanguard {
namespace {

bool use_colors_ = true;

enum ConsoleColor { White, Red, Yellow, Green, Blue };

std::ostream& operator<<(std::ostream& stream, ConsoleColor color) {
  if (!use_colors_) {
    return stream;
  }
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

}  // namespace

Printer::Printer() : colorized(true) {}

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
      *stream << Blue << "|   |   " << Red
              << "- Not enough heap memory allocated!" << std::endl;
      *stream << Blue << "|   |   |   " << Yellow
              << "Minimum required heap allocations: " << test_case.min_memory
              << " Bytes" << std::endl;
      *stream << Blue << "|   |   |   " << Yellow << "Total Heap Allocations: "
              << test_case.memory_monitor.ClosedSize() << " Bytes" << std::endl;
    }
    // Max check
    if (test_case.max_memory != -1 &&
        test_case.max_memory < test_case.memory_monitor.ClosedSize()) {
      *stream << Blue << "|   |   " << Red
              << "- Too much heap memory allocated!" << std::endl;
      *stream << Blue << "|   |   |   " << Yellow
              << "Maximum allowed heap allocations: " << test_case.max_memory
              << " Bytes" << std::endl;
      *stream << Blue << "|   |   |   " << Yellow << "Total heap allocations: "
              << test_case.memory_monitor.ClosedSize() << " Bytes" << std::endl;
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
    if (test_case.sig_seg_v_registered) {
      *stream << Blue << "|   |   " << Red << "- Segmentation Fault!"
              << std::endl;
      *stream << Blue << "|   |   |   " << Yellow
              << "You are probably trying to dereference a null pointer."
              << std::endl;
    }
    if (test_case.timeout_registered) {
      *stream << Blue << "|   |   " << Red << "- Timeout!" << std::endl;
      *stream << Blue << "|   |   |   " << Yellow
              << "You probably have an infinite loop." << std::endl;
    }
    if (test_case.exception_registered.length() != 0) {
      *stream << Blue << "|   |   " << Red << "- Exception" << std::endl;
      *stream << Blue << "|   |   |   " << Yellow
              << test_case.exception_registered << std::endl;
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
