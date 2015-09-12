// Copyright 2015 Alec Thilenius
// All rights reserved.

#include <cstdlib>
#include <iostream>

#include "printer.h"
#include "suite.h"

SUITE(LinkedListTestSuite) {
  TEST("Stub Test Case", 5, 5) {
    EXPECT_TRUE(true, "A stub test", "Should NOT show up!");
  }
}

int main(int argc, char* argv[]) {
  ::thilenius::external::vanguard::Suite suite("Linked List",
                                               LinkedListTestSuite);
  suite.RunAllTestCasesProtected();
  ::thilenius::external::vanguard::Printer printer;
  printer.WriteStdCout(suite, &std::cout);
  printer.WriteSuiteResults(suite, &std::cout);
  printer.WriteSuiteReportCard(suite, &std::cout);
}
