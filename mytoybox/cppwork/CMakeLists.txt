cmake_minimum_required (VERSION 3.5)
project (cppwork)
set(CMAKE_CXX_STANDARD 14) 
set(CMAKE_CXX_STANDARD_REQUIRED ON) #...is required...
set(CMAKE_CXX_EXTENSIONS OFF) #...without compiler extensions like gnu++11
set(CMAKE_BUILD_TYPE Debug)
#set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address -fno-omit-frame-pointer")
#SET (CMAKE_CXX_COMPILER  "clang++")
#SET( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -fno-omit-frame-pointer" )

find_package(Threads)
find_package(Boost REQUIRED COMPONENTS timer thread system program_options unit_test_framework)

add_executable(a0.out invalidfree.cpp)
target_compile_options(a0.out PRIVATE -O0 )

add_executable(a1.out invalidfree.cpp)
target_compile_options(a1.out PRIVATE -O1 )

add_executable(a2.out invalidfree.cpp)
target_compile_options(a2.out PRIVATE -O2 )

# make VERBOSE=1
add_executable(container container.cpp)
target_link_libraries (container 
	Boost::unit_test_framework Boost::system 
	${CMAKE_THREAD_LIBS_INIT})