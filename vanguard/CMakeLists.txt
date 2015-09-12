cmake_minimum_required (VERSION 2.6)
project (vanguard)

find_package(Threads)

set (CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -std=c++11")
set (CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library (
  vanguard
  memory_monitor.cc
  printer.cc
  process.cc
  suite.cc
  test_case.cc
)

target_link_libraries (
  vanguard
  ${CMAKE_THREAD_LIBS_INIT}
)

add_executable (
  vanguard_test
  vanguard_test.cc
)

target_link_libraries (
  vanguard_test
  vanguard
)
