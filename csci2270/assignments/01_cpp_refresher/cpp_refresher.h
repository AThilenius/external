// Copyright 2015 Alec Thilenius
// All rights reserved.

#ifndef CSCI_DATA_STRUCTURES_CPP_REFRESHER_CPP_REFRESHER_H_
#define CSCI_DATA_STRUCTURES_CPP_REFRESHER_CPP_REFRESHER_H_

#include <string>
#include <typeinfo>

#include "test_runner.h"

class UberClass {
 public:
  UberClass() : uber_member(-1), method_called(false) {}

  void UberMethod() { method_called = true; }

  int uber_member;

 private:
  bool method_called;
  friend void CPlusPlusRefresher(UTTestRunner*);
};

void PointersTestAddCall(const std::string& type_name);
int PrintHelloWorldHarness();
void BaseMethodCalled();

template <typename T>
void UberFunction(T val) {
  PointersTestAddCall(typeid(T).name());
}

void DoesAwesomeLoopyThings();

void PrintHelloWorld();

void CopyArrayOnHeap(const char* array);

void CallUberFunctions();

void Loops(int number_of_times);

void UseingObjects(UberClass& uber_class_1, UberClass* uber_class_2,
                   UberClass** uber_class_3);

void InPlaceReverse(char* str);

namespace uber_namespace {
void OtherUberFunction();
}

#endif  // CSCI_DATA_STRUCTURES_CPP_REFRESHER_CPP_REFRESHER_H_
