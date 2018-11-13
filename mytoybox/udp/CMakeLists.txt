project(udp)
cmake_minimum_required(VERSION 3.5)
include (CTest)
set(CMAKE_CXX_STANDARD 14) 
set(CMAKE_BUILD_TYPE Debug)

find_package(Boost REQUIRED COMPONENTS filesystem program_options unit_test_framework regex system)
find_package(Threads)
find_package(wxWidgets COMPONENTS core base REQUIRED)
include(${wxWidgets_USE_FILE})

add_executable(udp  udp.cpp)

target_link_libraries (udp 
	dl
	${CMAKE_THREAD_LIBS_INIT})

add_executable(udp_server  udp_server.cpp)

target_link_libraries (udp_server
	Boost::regex Boost::program_options Boost::filesystem 
	Boost::system 
	${wxWidgets_LIBRARIES}
	dl
	${CMAKE_THREAD_LIBS_INIT})
	
