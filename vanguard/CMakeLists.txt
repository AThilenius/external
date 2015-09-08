cmake_minimum_required (VERSION 2.6)
project (vanguard)

find_package(Threads)

set (CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -std=c++11")
set (CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library (
  vanguard
  /root/thilenius/external/vanguard/main.cc
  /root/thilenius/external/vanguard/memory_monitor.cc
  /root/thilenius/external/vanguard/printer.cc
  /root/thilenius/external/vanguard/suite.cc
  /root/thilenius/external/vanguard/suite_runner.cc
  /root/thilenius/external/vanguard/test_case.cc
)

target_link_libraries (
  vanguard
  ${CMAKE_THREAD_LIBS_INIT}
)

add_executable (
  vanguard_test
  /root/thilenius/external/vanguard/main.cc
)

target_link_libraries (
  vanguard_test
  vanguard
)
