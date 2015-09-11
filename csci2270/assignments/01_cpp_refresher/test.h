// Copyright 2015 Alec Thilenius
// All rights reserved.

#ifndef SCORCH_ANVIL_TESTING_FRAMEWORK_TEST_H_
#define SCORCH_ANVIL_TESTING_FRAMEWORK_TEST_H_

#include <vector>
#include <unordered_map>

// ==  UTCondition  ===========================================================
class UTCondition {
 public:
  UTCondition(bool did_pass, std::string name, std::string asert_message);
  void Print();

 public:
  bool did_pass;
  std::string name;
  std::string assert_message;
};

// ==  UTTestConfiguration  ====================================================
class UTTestConfiguration {
 public:
  UTTestConfiguration();

 public:
  int min_memory;
  int max_memory;
  bool leak_check;
};

// ==  MemoryAllocation  =======================================================
class MemoryAllocation {
 public:
  MemoryAllocation(void* address, size_t size, std::string file, size_t line);

 public:
  void* address;
  size_t size;
  std::string file;
  size_t line;
};

// ==  UTTest  =================================================================
class UTTest {
 public:
  UTTest();
  bool DidPass();
  int PointsEarned();
  void RegisterAllocation(MemoryAllocation allocation);
  void RegisterFree(void* ptr);
  void Print();

 public:
  bool memory_leaked;
  std::string name;
  int points_possible;
  int points_denominator;
  std::string fatal_message;
  UTTestConfiguration configuration;
  std::vector<UTCondition> conditions;
  std::unordered_map<void*, MemoryAllocation> outstanding_allocations;
  std::unordered_map<void*, MemoryAllocation> outstanding_frees;
  std::vector<MemoryAllocation> closed_allocations;
};

#endif  // SCORCH_ANVIL_TESTING_FRAMEWORK_TEST_H_
