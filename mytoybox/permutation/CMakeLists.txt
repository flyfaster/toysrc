cmake_minimum_required(VERSION 3.5)
project(permutation)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CXX_STANDARD 11) 
set(CMAKE_BUILD_TYPE "Debug" CACHE
      STRING "Choose the type of build." FORCE)
      
add_executable(${PROJECT_NAME} permutation.cpp )

