cmake_minimum_required(VERSION 3.5)
project(guifind)

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
else()
        message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

find_package(wxWidgets REQUIRED core base)
include(${wxWidgets_USE_FILE})
find_package(Threads)
 
# https://github.com/Microsoft/cpprestsdk/issues/536
find_package(Boost REQUIRED COMPONENTS regex thread system program_options filesystem)
             
add_executable(${PROJECT_NAME} CustomEvent.cpp  FindThread.cpp  GuiFindApp.cpp  GuiFindDlg.cpp  VerboseDialog.cpp)

target_link_libraries (${PROJECT_NAME} 
						Boost::regex Boost::thread Boost::program_options Boost::filesystem
                         Boost::system
						${wxWidgets_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})

