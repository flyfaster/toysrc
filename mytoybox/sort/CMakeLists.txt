cmake_minimum_required(VERSION 3.5)
project(sort)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_BUILD_TYPE "Debug" CACHE
      STRING "Choose the type of build." FORCE)

find_package(Threads)
find_package(Boost REQUIRED COMPONENTS timer thread system program_options )

add_executable(heapsort heapsort.cpp)
add_executable(mergesort mergesort.cpp)
add_executable(quicksort quicksort.cpp)

target_link_libraries (quicksort
         Boost::timer  Boost::system
        ${CMAKE_THREAD_LIBS_INIT})