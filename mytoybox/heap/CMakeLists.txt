cmake_minimum_required (VERSION 2.6)
project (queue-with-min)
SET(ENV{LD_LIBRARY_PATH} "~/gcc-6.3.0/lib64:$ENV{LD_LIBRARY_PATH}")

set(CMAKE_CXX_STANDARD 11) # C++11...
set(CMAKE_CXX_STANDARD_REQUIRED ON) #...is required...
set(CMAKE_CXX_EXTENSIONS OFF) #...without compiler extensions like gnu++11
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address -fno-omit-frame-pointer")
SET (CMAKE_CXX_COMPILER  "/home/onega/bin/clang+llvm-3.9.0-x86_64-fedora23/bin/clang++")

set(CMAKE_INSTALL_RPATH "~/gcc-6.3.0/lib64")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

add_executable(queue-with-min queue-with-min.cpp)
