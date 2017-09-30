cmake_minimum_required(VERSION 2.8)
project(htmltest)

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)

if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
else()
        message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

add_definitions(-DWITH_SHARED_LIB=1)

include_directories("${CMAKE_CURRENT_BINARY_DIR}")
include_directories("${PROJECT_SOURCE_DIR}/gen-cpp")
include(ThriftMacros)

#target_compile_options(htmltestgencpp PUBLIC  -DWITH_SHARED_LIB=1)
#target_compile_options(htmltest PUBLIC  -DWITH_SHARED_LIB=1)

set(htmltest_thrift_SOURCES 
    gen-cpp/CheckSumService.cpp  
    gen-cpp/htmltest_constants.cpp 
    gen-cpp/CheckSumService_server.skeleton.cpp  
    gen-cpp/htmltest_types.cpp
)

add_library(htmltestgencpp STATIC ${htmltest_thrift_SOURCES})
#LINK_AGAINST_THRIFT_LIBRARY(htmltestgencpp thrift)

add_custom_command(OUTPUT gen-cpp/CheckSumService.cpp  gen-cpp/htmltest_constants.cpp gen-cpp/CheckSumService_server.skeleton.cpp  gen-cpp/htmltest_types.cpp
    COMMAND thrift --gen cpp -r ${PROJECT_SOURCE_DIR}/htmltest.thrift
)

add_custom_command(OUTPUT MainWnd_moc.cpp
	COMMAND /usr/lib64/qt4/bin/moc MainWnd.h -o MainWnd_moc.cpp
	)

add_executable(htmltest MainWnd.cpp MainWnd_moc.cpp CheckSumServer.cpp SimpleThriftServer.cpp clock_timer.cpp resource_database.cpp page_parser.cpp url_class.cpp image_size.cpp http_downloader.cpp digestclass.cpp main.cpp)

target_link_libraries (htmltest gumbo pthread boost_system boost_program_options boost_filesystem boost_regex curl crypto sqlite3 jpeg png htmltestgencpp thrift QtCore QtGui)
#LINK_AGAINST_THRIFT_LIBRARY(htmltest thrift)

