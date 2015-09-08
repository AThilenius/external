// Copyright 2015 Alec Thilenius
// All rights reserved.

#include <chrono>
#include <iostream>
#include <signal.h>
#include <thread>

#include "printer.h"
#include "suite_runner.h"

namespace thilenius {
namespace external {
namespace vanguard {
namespace {

// Note: a lot of things need to be proper-global for segsegv recovery. The
// stack is fully unwound during a sigsegv catch, so things must be global. It's
// not ideal.
// TODO(athilenius): Explore alternates to all this global crap, it's fuck ugly.
// Maybe run each test in a child process.

Suite* active_suite_ = nullptr;

void SigSegVHandler(int);
void RackSigSegVRegistrar();
void RackThreadedTimeout();
void RackExceptionHandler();
void TestEnd();

// The free SigSegV (Segmentation fault) handler. It will call back into
// vanguard
void SigSegVHandler(int signum) {
  // Re-Register the SIG-SEGV handler
  active_suite_->ActiveTestCase().sig_seg_v_registered = true;
  RackSigSegVRegistrar();
  TestEnd();
  exit(EXIT_FAILURE);
}

// The first middleware in the test rack, the sigsegv registrar
void RackSigSegVRegistrar() {
  signal(SIGSEGV, SigSegVHandler);
  RackThreadedTimeout();
}

// The second middleware in the test rack, the used for catching infinite loops
void RackThreadedTimeout() {
  // No need to syncronize this, there are no possible race conditions that I
  // care about
  bool thread_finished = false;
  std::thread worker_thread([&thread_finished]() {
    RackExceptionHandler();
    thread_finished = true;
  });
  static bool warning_issued = false;
  auto warn_time =
      std::chrono::steady_clock::now() + std::chrono::milliseconds(500);
  auto endTime = std::chrono::steady_clock::now() + std::chrono::seconds(4);
  while (thread_finished == false &&
         std::chrono::steady_clock::now() < endTime) {
    if (std::chrono::steady_clock::now() > warn_time && !warning_issued) {
      std::cout << "One of the tests is taking an unusually long time...";
      warning_issued = true;
      std::cout.flush();
    }
    // Spin wait (can't reply on conditions)
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
  }
  // Check if we timed out
  if (std::chrono::steady_clock::now() >= endTime) {
    worker_thread.detach();
    active_suite_->ActiveTestCase().timeout_registered = true;
    RackThreadedTimeout();
  } else {
    worker_thread.join();
  }
}

// The third and final middleware in the test rack, used for catching all
// exceptions
void RackExceptionHandler() {
  try {
    active_suite_->RunOrRerunSuite();
  } catch (...) {
    active_suite_->ActiveTestCase().exception_registered =
        "A fatal exception was thrown!";
    RackExceptionHandler();
  }
}

// Ends the current suite (printing results)
void TestEnd() {
  Printer printer;
  printer.WriteSuiteResults(*active_suite_, &std::cout);
  printer.WriteSuiteReportCard(*active_suite_, &std::cout);
}

}  // namespace

SuiteRunner::SuiteRunner(int* argc, char*** argv) {}

void SuiteRunner::RunSuite(Suite* suite) {
  active_suite_ = suite;
  RackSigSegVRegistrar();
  TestEnd();
  active_suite_ = nullptr;
}

}  // namespace vanguard
}  // namespace external
}  // namespace thilenius
