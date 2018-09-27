cmake_minimum_required(VERSION 3.5)
project(hashtable)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CXX_STANDARD 14) 
set(CMAKE_BUILD_TYPE "Debug" CACHE
      STRING "Choose the type of build." FORCE)
      
find_package(Threads)
find_package(Boost REQUIRED COMPONENTS regex thread system program_options 
			filesystem unit_test_framework)

add_executable(${PROJECT_NAME} hashtable.cpp)

target_link_libraries (${PROJECT_NAME} 
	Boost::regex Boost::thread Boost::program_options Boost::filesystem Boost::system
	${CMAKE_THREAD_LIBS_INIT})
	
add_executable(hashtable2 hashtable2.cpp)

add_executable(rabin_karp rabin_karp.cpp)
target_link_libraries (rabin_karp
	Boost::regex Boost::thread Boost::program_options Boost::filesystem 
	Boost::unit_test_framework Boost::system
	${CMAKE_THREAD_LIBS_INIT})