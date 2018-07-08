project(wx)
cmake_minimum_required(VERSION 3.5)
set(CMAKE_CXX_STANDARD 11) 

find_package(Boost REQUIRED COMPONENTS filesystem program_options system)
find_package(Threads)

find_package(wxWidgets COMPONENTS core base REQUIRED)
include(${wxWidgets_USE_FILE})
 
add_executable(hello hello.cpp)
target_link_libraries(hello ${wxWidgets_LIBRARIES} 
	${CMAKE_THREAD_LIBS_INIT})

add_executable(miniwx miniwx.cpp)
target_link_libraries(miniwx ${wxWidgets_LIBRARIES} 
	${CMAKE_THREAD_LIBS_INIT})
	
# cmake -DBOOST_ROOT=/usr/local/Cellar/boost/1.65.1 ~/bin/tools/cpp/path-exists