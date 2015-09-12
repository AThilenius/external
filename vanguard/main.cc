// Copyright 2015 Alec Thilenius
// All rights reserved.

#include <cstdlib>
#include <iostream>

#include "printer.h"
#include "suite.h"

using ::thilenius::external::vanguard::Suite;
using ::thilenius::external::vanguard::TestCase;

void OverflowStack() { OverflowStack(); }

SUITE(SuiteOne) {
  TEST("True Check", 5, 5) {
    EXPECT_TRUE(true, "Should be True", "Should NOT show up!");
    EXPECT_TRUE(true, "Should also be True", "Should NOT show up!");
  }

  TEST("Seg Fault Check", 5, 5) {
    EXPECT_TRUE(true, "Should fail after this", "Should NOT show up!");
    int dummy = *((int*)0);
    std::cout << "Don't cull dummy!" << dummy;
  }

  TEST("False Check", 5, 5) {
    EXPECT_TRUE(false, "Should be False", "Should Be Shown");
    EXPECT_TRUE(false, "Should also be False", "Should Also Be Shown");
  }

  TEST("Infinite Loops Check", 5, 5) {
    while (true) {
    }
  }

  TEST("Stack Overflow", 5, 5) { OverflowStack(); }

  TEST("Div By Zero", 5, 5) {
    int a = 1;
    int b = 0;
    int c = a / b;
    std::cout << c;
  }

  TEST("Abort", 5, 5) { abort(); }

  TEST("Unknwon Exception", 5, 5) { throw "Unknown Exception Type"; }

  TEST("User output", 10, 10) {
    std::cout << "This, my friend" << std::endl;
    std::cout << "Is some user output, written to std::cout" << std::endl;
    std::cout << "It is captured from the child process" << std::endl;
    std::cout << "Cool shit! AMIRIGHT!" << std::endl;
  }

  // TEST("Memory Leak Check Pass", 5, 5) {
  // Suite::ActiveSuite().ActiveTestCase().leak_check = true;
  // int* foo = new int[1];
  // delete foo;
  //}

  // TEST("Memory Leak Check Fail", 5, 5) {
  // Suite::ActiveSuite().ActiveTestCase().leak_check = true;
  // new int[1];
  // new int[2];
  // new int[3];
  // new int[4];
  // new int[5];
  // new int[100];
  //}

  // TEST("Min Memory Check Fail", 5, 5) {
  // Suite::ActiveSuite().ActiveTestCase().min_memory = 5;
  // int* foo = new int[1];
  // delete foo;
  //}

  // TEST("Min Memory Check Pass", 5, 5) {
  // Suite::ActiveSuite().ActiveTestCase().min_memory = 5;
  // int* foo = new int[10];
  // delete foo;
  //}

  // TEST("Max Memory Check Fail", 5, 5) {
  // Suite::ActiveSuite().ActiveTestCase().max_memory = 5;
  // int* foo = new int[64];
  // delete foo;
  //}

  // TEST("Max Memory Check Pass", 5, 5) {
  // Suite::ActiveSuite().ActiveTestCase().max_memory = 4;
  // int* foo = new int[1];
  // delete foo;
  //}

  // TEST("Many Critital Failutes", 5, 5) {
  // Suite::ActiveSuite().ActiveTestCase().leak_check = true;
  // Suite::ActiveSuite().ActiveTestCase().max_memory = 4;
  // int* foo = new int[64];
  //// Seg-Fault it
  // foo = (int*)0;
  // std::cout << *foo;
  //}

  // TEST("Big Credit", 40, 40) {}

  // TEST("Extra Credit", 5, 0) {}

  // TEST("Partial Credit", 15, 15) {
  // EXPECT_TRUE(true, "Pass", "");
  // EXPECT_TRUE(false, "Fail", "Dun fucked up");
  // EXPECT_TRUE(false, "Fail", "Yup, u dun fucked up!");
  //}
}

using ::thilenius::external::vanguard::Printer;
using ::thilenius::external::vanguard::Suite;

int main(int argc, char* argv[]) {
  Suite suite("Test Suite", SuiteOne);
  suite.RunAllTestCasesProtected();
  Printer printer;
  printer.WriteStdCout(suite, &std::cout);
  printer.WriteSuiteResults(suite, &std::cout);
  printer.WriteSuiteReportCard(suite, &std::cout);
}
