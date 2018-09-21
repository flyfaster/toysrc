cmake_minimum_required(VERSION 3.5)
project(math)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CXX_STANDARD 17) 
set(CMAKE_BUILD_TYPE "Debug" CACHE
      STRING "Choose the type of build." FORCE)
      
find_package(Boost REQUIRED COMPONENTS program_options unit_test_framework filesystem regex)
      
add_executable(math math.cpp )

target_link_libraries (math 
	Boost::regex Boost::program_options Boost::filesystem Boost::system Boost::unit_test_framework
	${CMAKE_THREAD_LIBS_INIT})
	
add_executable(bitcount bitcount.cpp)
add_executable(sampling sampling.cpp)
add_executable(combinatorics combinatorics.cpp)

target_link_libraries (combinatorics 
	Boost::regex Boost::program_options Boost::filesystem Boost::system Boost::unit_test_framework
	${CMAKE_THREAD_LIBS_INIT})
	
