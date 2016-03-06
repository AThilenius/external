# Vanguard - Testing Framework
*"An open source testing framework for C++ with a strong focus on education and beauty"*

### Beautiful Output:
Vanguard puts strong emphasis on beautiful console output. Because it is so robust, the student always gets pretty output, not filled with garbage spit out by the shell / OS when a test segfaults. Wonderful and robust formatting with nice colors make it easy to read at first glance. Students get a little 'report card' at the end showing their current grade on the assignment.

This is **sample output** from a Binary Search Tree homework assignment at CU Boulder as a [ScorchForge Fiber Cord](https://scorchforge.com/#/fiber/f964f136-1463-4f61-aa23-919455258d46). Source code for the assignment can be found in as a Crucible CL link at the top of the Fiber Cord.

Results can also be output as JSON (useful for automated grading of assignments)

### Highly Robust
Vanguard also puts strong emphasis on being as robust as possible without 'dirtying' the output. **Each test is run in a child process** allowing each test to individually hard-fail or timeout without affecting the student.

### Easy to write tests for - Seriously...
```c
#include "vanguard.h"
#include "binary_tree.h"

SUITE(BinaryTreeTestSuite) {
  TEST("Set Head to nullptr", 5, 5) {
    EXPECT_TRUE(binary_tree.head == nullptr,
                "Head should be initialized to nullptr",
                "You didn't set head to nullptr in the constructor");
  }

  TEST("False Check And Leak Memory", 5, 5) {
    ACTIVE_TEST_CASE.leak_check = true;
    int leak_memory_shows_up_here[] = new int[10];
    EXPECT_TRUE(false, "Should be False", "Will be shown in red");
  }
}

using ::thilenius::external::vanguard::Vanguard;

int main(int argc, char* argv[]) {
  Vanguard vanguard (argc, argv);
  vanguard.RunSuite(BinaryTreeTestSuite, "Binary Search Tree");
}
```

### Memory Tracking
Vanguard can track memory allocations for tests cases where it's enabled. If a leak is found, it will report the file and line number where the memory was allocated and how many bytes were lost.
