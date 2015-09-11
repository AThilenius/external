// Copyright 2015 Alec Thilenius
// All rights reserved.

#include <algorithm>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <typeinfo>
#include <unordered_set>

#include "cpp_refresher.h"
#include "test_runner.h"

namespace {

bool other_uber_function_called = false;
int does_loopy_things_count = 0;
int last_new_size_ = 0;
std::unordered_set<std::string> type_call_set_;
void* last_delete_ptr_ = nullptr;
void* last_new_ptr_ = nullptr;
bool base_method_called = false;

}  // namespace

void PointersTestAddCall(const std::string& type_name) {
  type_call_set_.insert(type_name);
}

void DoesAwesomeLoopyThings() { does_loopy_things_count++; }

void BaseMethodCalled() { base_method_called = true; }

namespace uber_namespace {

void OtherUberFunction() { other_uber_function_called = true; }
}

int PrintHelloWorldHarness() {
  std::stringstream buffer;
  std::streambuf* sbuf = std::cout.rdbuf();
  std::cout.rdbuf(buffer.rdbuf());
  PrintHelloWorld();
  int count = buffer.str().length();
  std::cout.rdbuf(sbuf);
  return count;
}

std::string GetRandomString() {
  std::string random_string;
  srand(time(NULL));
  int size = 10 + (rand() % 100);
  for (int i = 0; i < size; i++) {
    random_string += static_cast<char>(65 + (std::rand() % 24));
  }
  return std::move(random_string);
}

void LoopsTestHelper(UTTestRunner* runner, int count) {
  does_loopy_things_count = 0;
  Loops(count);
  runner->IsTrue(
      does_loopy_things_count == count,
      "DoesAwesomeLoopyThings called " + std::to_string(count) + " times.",
      "Your loop should have run " + std::to_string(count) + " times, not " +
          std::to_string(does_loopy_things_count) + " times");
}

SUITE(CPlusPlusRefresher) {
  TEST("Print Hello, world", 10, 10) {
    int number_of_chars_couted = PrintHelloWorldHarness();
    runner->IsTrue(number_of_chars_couted > 0, "Was something printed",
                   "You need to print something to cout");
  }

  TEST("Call the UberFunction with different types", 25, 25) {
    CallUberFunctions();
    runner->IsTrue(
        type_call_set_.find(typeid(int).name()) != type_call_set_.end(),
        "Was uber function called with int",
        "You need to call the "
        "function \"UberFunction\" "
        "with an int argument");
    runner->IsTrue(
        type_call_set_.find(typeid(int*).name()) != type_call_set_.end(),
        "Was uber function called with int*",
        "You need to call the "
        "function \"UberFunction\" "
        "with an int* argument");
    runner->IsTrue(
        type_call_set_.find(typeid(int**).name()) != type_call_set_.end(),
        "Was uber function called with int**",
        "You need to call the "
        "function \"UberFunction\" "
        "with an int** argument");
    runner->IsTrue(
        type_call_set_.find(typeid(char*).name()) != type_call_set_.end(),
        "Was uber function called with char*",
        "You need to call the "
        "function \"UberFunction\" "
        "with a char* argument");
    runner->IsTrue(other_uber_function_called,
                   "Called OtherUberFunction in the uber_namespace namespace",
                   "You need to call the function OtherUberFunction in the "
                   "uber_namespace namespace");
  }

  TEST("Call DoesAwesomeLoopyThings N times", 20, 20) {
    srand(time(NULL));
    LoopsTestHelper(runner, 1);
    LoopsTestHelper(runner, rand() % 100);
  }

  static std::string array = GetRandomString();
  runner->GetConfig()->min_memory = (array.length() + 1) * sizeof(char);
  runner->GetConfig()->max_memory = (array.length() + 1) * sizeof(char);
  runner->GetConfig()->leak_check = true;
  TEST("Copy an array to the heap", 25, 25) {
    CopyArrayOnHeap(array.c_str());
    std::string alloc_size =
        std::to_string((array.length() + 1) * sizeof(char));
    runner->IsTrue(
        true, "Allocate and free " + alloc_size + " bytes using new / delete",
        "");
  }

  TEST("Create a class method called UberMethod", 20, 20) {
    base_method_called = false;
    UberClass uber_c1;
    UberClass uber_c2;
    UberClass uber_c3;
    UberClass* uber_c3_ptr = &uber_c3;
    UseingObjects(uber_c1, &uber_c2, &uber_c3_ptr);
    runner->IsTrue(uber_c1.method_called, "Call UberMethod on uber_class_1",
                   "You need to call UberMethod on the uber_class_1 instance");
    runner->IsTrue(uber_c1.uber_member == 42,
                   "Set the uber_member on uber_class_1 to 42",
                   "You need to set the member uber_member to 42");
    runner->IsTrue(uber_c2.method_called, "Call UberMethod on uber_class_2",
                   "You need to call UberMethod on the uber_class_2 instance");
    runner->IsTrue(uber_c2.uber_member == 42,
                   "Set the uber_member on uber_class_2 to 42",
                   "You need to set the member uber_member to 42");
    runner->IsTrue(uber_c3.method_called, "Call UberMethod on uber_class_3",
                   "You need to call UberMethod on the uber_class_3 instance");
    runner->IsTrue(uber_c3.uber_member == 42,
                   "Set the uber_member on uber_class_3 to 42",
                   "You need to set the member uber_member to 42");
  }

  runner->GetConfig()->max_memory = 0;
  TEST("Extra Credit: In place string reversal", 5, 0) {
    std::string forward = GetRandomString();
    std::string backward = forward;
    char* c_buffer =
        static_cast<char*>(malloc((forward.length() + 1) * sizeof(char)));
    backward = forward;
    std::reverse(backward.begin(), backward.end());
    forward.copy(c_buffer, forward.length(), 0);
    c_buffer[forward.length()] = 0;
    InPlaceReverse(c_buffer);
    runner->IsTrue(
        std::string(c_buffer) == backward,
        "Reverse the order of the string without allocating new memory",
        "You need to reverse the order of the string");
  }
}

int main(int argc, const char* argv[]) {
  UTTestRunner runner;
  runner.RunSuite("Test Name", &CPlusPlusRefresher);
}
