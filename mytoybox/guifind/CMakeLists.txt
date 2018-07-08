cmake_minimum_required(VERSION 3.5)
project(guifind)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CXX_STANDARD 11) 
set(CMAKE_BUILD_TYPE "Debug" CACHE
      STRING "Choose the type of build." FORCE)
      
find_package(wxWidgets REQUIRED core base)
include(${wxWidgets_USE_FILE})
find_package(Threads)
 
# https://github.com/Microsoft/cpprestsdk/issues/536
find_package(Boost REQUIRED COMPONENTS regex thread system program_options filesystem)
             
add_executable(${PROJECT_NAME} CustomEvent.cpp  FindThread.cpp  GuiFindApp.cpp  GuiFindDlg.cpp  VerboseDialog.cpp)

target_link_libraries (${PROJECT_NAME} 
	${wxWidgets_LIBRARIES}
	Boost::regex Boost::thread Boost::program_options Boost::filesystem Boost::system
	${CMAKE_THREAD_LIBS_INIT})

