// Copyright 2015 Alec Thilenius
// All rights reserved.

#include <iostream>

#include "printer.h"
#include "suite.h"
#include "suite_runner.h"

using ::thilenius::external::vanguard::Suite;
using ::thilenius::external::vanguard::TestCase;

SUITE(SuiteOne) {
  TEST("True Check", 5, 5) {
    EXPECT_TRUE(true, "Should be True", "Should NOT show up!");
    EXPECT_TRUE(true, "Should also be True", "Should NOT show up!");
  }

  // TEST("Seg Fault Check", 5, 5) {
  // EXPECT_TRUE(true, "Should fail after this", "Should NOT show up!");
  // int foo = *((int*)0);
  // std::cout << foo;
  //}

  TEST("False Check", 5, 5) {
    EXPECT_TRUE(false, "Should be False", "Should Be Shown");
    EXPECT_TRUE(false, "Should also be False", "Should Also Be Shown");
  }

  TEST("Infinite Loops Check", 5, 5) {
    while (true) {
    }
  }

  TEST("Unknwon Exception", 5, 5) {
    throw "Unknown Exception Type";
  }

  TEST("Memory Leak Check Pass", 5, 5) {
    Suite::ActiveSuite().ActiveTestCase().leak_check = true;
    int* foo = new int[1];
    delete foo;
  }

  TEST("Memory Leak Check Fail", 5, 5) {
    Suite::ActiveSuite().ActiveTestCase().leak_check = true;
    new int[1];
    new int[2];
    new int[3];
    new int[4];
    new int[5];
    new int[100];
  }

  TEST("Min Memory Check Fail", 5, 5) {
    Suite::ActiveSuite().ActiveTestCase().min_memory = 5;
    int* foo = new int[1];
    delete foo;
  }

  TEST("Min Memory Check Pass", 5, 5) {
    Suite::ActiveSuite().ActiveTestCase().min_memory = 5;
    int* foo = new int[10];
    delete foo;
  }

  TEST("Max Memory Check Fail", 5, 5) {
    Suite::ActiveSuite().ActiveTestCase().max_memory = 5;
    int* foo = new int[64];
    delete foo;
  }

  TEST("Max Memory Check Pass", 5, 5) {
    Suite::ActiveSuite().ActiveTestCase().max_memory = 4;
    int* foo = new int[1];
    delete foo;
  }

  TEST("Many Critital Failutes", 5, 5) {
    Suite::ActiveSuite().ActiveTestCase().leak_check = true;
    Suite::ActiveSuite().ActiveTestCase().max_memory = 4;
    int* foo = new int[64];
    // Seg-Fault it
    foo = (int*)0;
    std::cout << *foo;
  }

  TEST("Big Credit", 40, 40) {}

  TEST("Extra Credit", 5, 0) {}

  TEST("Partial Credit", 15, 15) {
    EXPECT_TRUE(true, "Pass", "");
    EXPECT_TRUE(false, "Fail", "Dun fucked up");
    EXPECT_TRUE(false, "Fail", "Yup, u dun fucked up!");
  }
}

using ::thilenius::external::vanguard::Printer;
using ::thilenius::external::vanguard::Suite;
using ::thilenius::external::vanguard::SuiteRunner;

int main(int argc, char* argv[]) {
  Suite* suite = new Suite("Test Suite", SuiteOne);
  // suite.RunOrRerunSuite();

  // Printer printer;
  // printer.WriteSuiteResults(suite, &std::cout);
  SuiteRunner* runner = new SuiteRunner(&argc, &argv);
  runner->RunSuite(suite);
}
