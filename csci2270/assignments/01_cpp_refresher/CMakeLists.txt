cmake_minimum_required (VERSION 2.6)
project (cpp_refresher)

set (CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -std=c++11")
set (CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Threads)

add_executable (
  cpp_refresher
  cpp_refresher.cc
  unit_tests.cc
)

target_link_libraries (
  cpp_refresher
  ${CMAKE_THREAD_LIBS_INIT}
  ${CMAKE_CURRENT_SOURCE_DIR}/libtesting_framework.a
)
