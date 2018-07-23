project(memfunc)
cmake_minimum_required(VERSION 3.5)
include (CTest)
set(CMAKE_CXX_STANDARD 11) 

add_custom_target(
  FastDelegate
  COMMAND curl -O https://raw.githubusercontent.com/dreamcat4/FastDelegate/master/FastDelegate.h
  WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
)

add_custom_target(
  FastDelegateBind
  COMMAND curl -O https://raw.githubusercontent.com/dreamcat4/FastDelegate/master/FastDelegateBind.h
  WORKING_DIRECTORY  "${CMAKE_CURRENT_BINARY_DIR}"
)

include_directories("${CMAKE_CURRENT_BINARY_DIR}")

add_executable(memfunc memfunc.cpp)

add_dependencies(memfunc FastDelegate FastDelegateBind)

add_test(ctest_memfunc memfunc)
