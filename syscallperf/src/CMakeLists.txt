cmake_minimum_required(VERSION 3.5)
project(syscallperf)

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
else()
        message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

find_package(Threads)
 
# https://github.com/Microsoft/cpprestsdk/issues/536
find_package(Boost REQUIRED COMPONENTS timer)
             
add_executable(${PROJECT_NAME} syscallperf.cpp)

target_link_libraries (${PROJECT_NAME} 
						Boost::timer 
						${CMAKE_THREAD_LIBS_INIT})

