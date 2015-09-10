cmake_minimum_required (VERSION 2.6)
project (array_list)

set (CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -std=c++11")
set (CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable (
  simple_array_list
  simple_array_list.cc
)

add_executable (
  array_list
  array_list.cc
)
