project(tree)
cmake_minimum_required(VERSION 3.5)
include (CTest)
set(CMAKE_CXX_STANDARD 14) 
set(CMAKE_BUILD_TYPE Debug)

find_package(Boost REQUIRED COMPONENTS filesystem program_options unit_test_framework regex system)
find_package(Threads)
 
add_executable(edit_distance edit_distance.cpp)
target_link_libraries (edit_distance 
	Boost::regex Boost::program_options Boost::filesystem Boost::system Boost::unit_test_framework
	${CMAKE_THREAD_LIBS_INIT})

add_executable(cut_rod cut_rod.cpp)
target_link_libraries (cut_rod 
	Boost::regex Boost::program_options Boost::filesystem Boost::system Boost::unit_test_framework
	${CMAKE_THREAD_LIBS_INIT})
