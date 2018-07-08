cmake_minimum_required(VERSION 3.5)
project(testgperf)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CXX_STANDARD 11) 
set(CMAKE_BUILD_TYPE "Debug" CACHE
      STRING "Choose the type of build." FORCE)
      

find_package(Threads)
find_package(Boost REQUIRED COMPONENTS regex thread system program_options filesystem)
include_directories(/usr/local/Cellar/gperftools/2.7/include)
link_directories(/usr/local/Cellar/gperftools/2.7/lib)
             
add_executable(${PROJECT_NAME} testgperf.cpp profiler.cpp)

target_link_libraries (${PROJECT_NAME} 
	Boost::regex Boost::thread Boost::program_options Boost::filesystem Boost::system
	profiler tcmalloc
	${CMAKE_THREAD_LIBS_INIT})