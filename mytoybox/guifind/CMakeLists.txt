cmake_minimum_required(VERSION 2.8)
project(guifind)

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
        message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()
add_definitions(-DWXUSINGDLL -D__WXGTK__)

link_directories(~/src/wxWidgets-3.1.0/lib /usr/local/lib)

find_package( Threads )
 
include_directories(~/src/wxWidgets-3.1.0/include ~/src/wxWidgets-3.1.0/lib/wx/include/gtk3-unicode-3.1 )

add_executable(${PROJECT_NAME} CustomEvent.cpp  FindThread.cpp  GuiFindApp.cpp  GuiFindDlg.cpp  VerboseDialog.cpp)

target_link_libraries (${PROJECT_NAME} boost_regex boost_thread boost_system boost_program_options boost_filesystem wx_gtk3u_adv-3.1 wx_gtk3u_aui-3.1 wx_gtk3u_core-3.1 wx_baseu-3.1 ${CMAKE_THREAD_LIBS_INIT})
