// Copyright 2015 Alec Thilenius
// All rights reserved.

#include <cstdlib>
#include <iostream>

#include "vanguard.h"

void OverflowStack() { OverflowStack(); }

SUITE(SuiteOne) {
  TEST("True Check", 5, 5) {
    EXPECT_TRUE(true, "Should be True", "Should NOT show up!");
    EXPECT_TRUE(true, "Should also be True", "Should NOT show up!");
  }

  TEST("False Check", 5, 5) {
    EXPECT_TRUE(false, "Should be False", "Should Be Shown");
    EXPECT_TRUE(false, "Should also be False", "Should Also Be Shown");
  }

  TEST("Seg Fault Check", 5, 5) {
    EXPECT_TRUE(true, "Should fail after this", "Should NOT show up!");
    volatile int* foo = nullptr;
    volatile int bar = *foo;
  }

  TEST("Infinite Loops Check", 5, 5) {
    while (true) {
    }
  }

  TEST("Stack Overflow", 5, 5) { OverflowStack(); }

  TEST("Div By Zero", 5, 5) {
    int a = 1;
    int b = 0;
    volatile int c = a / b;
  }

  TEST("Abort", 5, 5) { abort(); }

  TEST("Unknwon Exception", 5, 5) { throw "Unknown Exception Type"; }

  TEST("User output", 10, 10) {
    std::cout << "This, my friend" << std::endl;
    std::cout << "Is some user output, written to std::cout" << std::endl;
    std::cout << "It is captured from the child process" << std::endl;
    std::cout << "Cool shit! AMIRIGHT!" << std::endl;
  }

  TEST("Memory Leak Check Pass", 5, 5) {
    ACTIVE_TEST_CASE.leak_check = true;
    int* foo = new int[1];
    delete foo;
  }

  TEST("Memory Leak Check Fail", 5, 5) {
    ACTIVE_TEST_CASE.leak_check = true;
    new int[1];
    new int[2];
    new int[3];
    new int[4];
    new int[5];
    new int[100];
  }

  TEST("Min Memory Check Fail", 5, 5) {
    ACTIVE_TEST_CASE.min_memory = 5;
    int* foo = new int[1];
    delete foo;
  }

  TEST("Min Memory Check Pass", 5, 5) {
    ACTIVE_TEST_CASE.min_memory = 5;
    int* foo = new int[10];
    delete foo;
  }

  TEST("Default No Leak Check", 5, 5) {
    EXPECT_TRUE(true, "Should not show any leaks", "");
    int* foo = new int[10];
  }

  TEST("Max Memory Check Fail", 5, 5) {
    ACTIVE_TEST_CASE.max_memory = 5;
    int* foo = new int[64];
    delete foo;
  }

  TEST("Max Memory Check Pass", 5, 5) {
    ACTIVE_TEST_CASE.max_memory = 5;
    int* foo = new int[1];
    delete foo;
  }

  TEST("Many Critital Failutes", 5, 5) {
    ACTIVE_TEST_CASE.leak_check = true;
    ACTIVE_TEST_CASE.max_memory = 4;
    int* foo = new int[64];
    // Seg-Fault it
    volatile int* bar = nullptr;
    volatile int baz = *bar;
  }

  TEST("Big Credit", 40, 40) {}

  TEST("Extra Credit", 5, 0) {}

  TEST("Partial Credit", 15, 15) {
    EXPECT_TRUE(true, "Pass", "");
    EXPECT_TRUE(false, "Fail", "Should show up");
    EXPECT_TRUE(false, "Fail", "Should also show up");
  }
}

using ::thilenius::external::vanguard::Vanguard;

int main(int argc, char* argv[]) {
  Vanguard vanguard (argc, argv);
  vanguard.RunSuite(SuiteOne, "Test Suite");
}
