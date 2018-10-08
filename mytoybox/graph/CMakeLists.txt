cmake_minimum_required(VERSION 3.5)
project(graph)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE "Debug" CACHE
      STRING "Choose the type of build." FORCE)

find_package(Threads)
find_package(Boost REQUIRED COMPONENTS timer thread system program_options )

add_executable(dijkstra dijkstra.cpp)

add_executable(astar astar.cpp)