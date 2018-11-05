project(tree)
cmake_minimum_required(VERSION 3.5)
include (CTest)
set(CMAKE_CXX_STANDARD 14) 
set(CMAKE_BUILD_TYPE Debug)

find_package(Boost REQUIRED COMPONENTS filesystem program_options unit_test_framework regex system)
find_package(Threads)
 
add_executable(embed_v8  embed_v8.cpp)
target_link_libraries (embed_v8 v8 v8_libplatform
	Boost::regex Boost::program_options Boost::filesystem Boost::system Boost::unit_test_framework
	${CMAKE_THREAD_LIBS_INIT})
