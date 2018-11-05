project(tree)
cmake_minimum_required(VERSION 3.5)
include (CTest)
set(CMAKE_CXX_STANDARD 14) 
set(CMAKE_BUILD_TYPE Debug)

set(Python_ADDITIONAL_VERSIONS 3.7)
FIND_PACKAGE(PythonInterp REQUIRED)
FIND_PACKAGE(PythonLibs 3.7 REQUIRED)

find_package(Boost REQUIRED COMPONENTS filesystem program_options unit_test_framework regex system)
find_package(Threads)
find_library(PY_LIBRARY python HINTS /usr/local/Cellar/python)

add_executable(embed_python  embed_python.cpp)

target_include_directories(embed_python PRIVATE ${PYTHON_INCLUDE_DIRS})
link_directories(/usr/local/Cellar/python/3.7.0/Frameworks/Python.framework/Versions/3.7/lib)
target_link_libraries (embed_python 
	${PY_LIBRARY} 
	/usr/local/Cellar/python/3.7.0/Frameworks/Python.framework/Versions/3.7/lib/libpython3.7m.dylib 
	dl
	${CMAKE_THREAD_LIBS_INIT})