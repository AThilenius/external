// Copyright 2015 Alec Thilenius
// All rights reserved.

#ifndef EXTERNAL_VANGUARD_PRINTER_H_
#define EXTERNAL_VANGUARD_PRINTER_H_

#include <ostream>

#include "suite.h"
#include "test_case.h"

// Used for printing colored output
enum ConsoleColor { White, Red, Yellow, Green, Blue };
std::ostream& operator<<(std::ostream& stream, ConsoleColor color);

namespace thilenius {
namespace external {
namespace vanguard {

class Printer {
 public:
  // Create a default colorized printer
  Printer();

  // Write a suite results out to stream
  void WriteSuiteResults(const Suite& suite, std::ostream* stream);

  // Write a suite report card out to stream
  void WriteSuiteReportCard(const Suite& suite, std::ostream* stream);

  // Use console colors while printing
  bool colorized;

 private:
  static void WriteTestCaseResults(const TestCase& test_case,
                                   std::ostream* stream);

  static void WriteTestResults(const TestCase::Test& test,
                               std::ostream* stream);

  static void PrintRatioColor(int percent, std::ostream* stream);

  static int GetPercent(int nominator, int denominator);
};

}  // namespace vanguard
}  // namespace external
}  // namespace thilenius

#endif  // EXTERNAL_VANGUARD_PRINTER_H_
