cmake_minimum_required(VERSION 3.5)
project(complexity)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CXX_STANDARD 14) 
set(CMAKE_BUILD_TYPE "Debug" CACHE
      STRING "Choose the type of build." FORCE)
      
find_package(Boost REQUIRED COMPONENTS program_options unit_test_framework filesystem regex)
      
add_executable(complexity complexity.cpp )

target_link_libraries (complexity 
	Boost::regex Boost::program_options Boost::filesystem Boost::system Boost::unit_test_framework
	${CMAKE_THREAD_LIBS_INIT})