cmake_minimum_required(VERSION 3.5)
project(permutation)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CXX_STANDARD 11) 
set(CMAKE_BUILD_TYPE "Debug" CACHE
      STRING "Choose the type of build." FORCE)
      
find_package(Threads)
find_package(Boost REQUIRED COMPONENTS program_options unit_test_framework filesystem regex)

add_executable(permutation permutation.cpp )

add_executable(median median.cpp )

add_executable(dominotiling dominotiling.cpp )

target_link_libraries (median
	Boost::program_options 
	)

target_link_libraries (dominotiling
	Boost::program_options 
	)
	
add_executable(backtrack backtrack.cpp)
target_link_libraries (backtrack 
	Boost::regex Boost::program_options Boost::filesystem Boost::system Boost::unit_test_framework
	${CMAKE_THREAD_LIBS_INIT})