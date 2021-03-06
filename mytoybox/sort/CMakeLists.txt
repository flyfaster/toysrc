cmake_minimum_required(VERSION 3.5)
project(sort)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_BUILD_TYPE "Debug" CACHE
      STRING "Choose the type of build." FORCE)

find_package(Threads)
find_package(Boost REQUIRED COMPONENTS timer thread system program_options unit_test_framework)

add_executable(heapsort heapsort.cpp)
add_executable(mergesort mergesort.cpp)
target_link_libraries (mergesort 
	Boost::unit_test_framework Boost::system 
	${CMAKE_THREAD_LIBS_INIT})
	
add_executable(quicksort quicksort.cpp)

target_link_libraries (mergesort
         Boost::timer)
target_link_libraries (quicksort
         Boost::timer  Boost::system
        ${CMAKE_THREAD_LIBS_INIT})
        
add_executable(insertion_sort insertion_sort.cpp)
target_link_libraries (insertion_sort 
	Boost::unit_test_framework Boost::system 
	${CMAKE_THREAD_LIBS_INIT})
	
add_executable(linear_sort linear_sort.cpp)
target_link_libraries (linear_sort 
	Boost::unit_test_framework Boost::system 
	${CMAKE_THREAD_LIBS_INIT})
	
add_executable(binary_search binary_search.cpp)
target_link_libraries (binary_search 
	Boost::unit_test_framework Boost::system 
	${CMAKE_THREAD_LIBS_INIT})