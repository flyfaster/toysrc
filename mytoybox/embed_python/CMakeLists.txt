project(tree)
cmake_minimum_required(VERSION 3.5)
include (CTest)
set(CMAKE_CXX_STANDARD 14) 
set(CMAKE_BUILD_TYPE Debug)

#set(Python_ADDITIONAL_VERSIONS 3.7)
#FIND_PACKAGE(PythonInterp REQUIRED)
#FIND_PACKAGE(PythonLibs 3.7 REQUIRED)

#find_package(Boost REQUIRED COMPONENTS filesystem program_options unit_test_framework regex system)
find_package(Threads)
#find_library(PY_LIBRARY python HINTS /usr/local/Cellar/python)
find_package(PythonLibs REQUIRED)
include_directories(${PYTHON_INCLUDE_DIRS})

# wx-config --cppflags
find_package(wxWidgets COMPONENTS core base REQUIRED)
include(${wxWidgets_USE_FILE})

add_executable(embed_python  embed_python.cpp)
target_link_libraries(embed_python ${PYTHON_LIBRARIES} dl
	${CMAKE_THREAD_LIBS_INIT})
	
add_executable(wxembed_python  wxembed_python.cpp)
target_link_libraries(wxembed_python 
	${wxWidgets_LIBRARIES}
	${PYTHON_LIBRARIES} dl
	${CMAKE_THREAD_LIBS_INIT})
	
set(SOURCE_FILES sub_interpreter.cpp)

add_executable(sub_interpreter ${SOURCE_FILES})
target_link_libraries(sub_interpreter ${PYTHON_LIBRARIES} dl
	${CMAKE_THREAD_LIBS_INIT})
	
add_executable(embed_python_mt embed_python_mt.cpp)
target_link_libraries(embed_python_mt ${PYTHON_LIBRARIES} dl
	${CMAKE_THREAD_LIBS_INIT})