cmake_minimum_required (VERSION 3.5)
project (readjar)
set(CMAKE_CXX_STANDARD 11) 
set(CMAKE_INCLUDE_CURRENT_DIR ON)

#find_package(Boost REQUIRED COMPONENTS asio)
find_package(Boost)
find_package(wxWidgets REQUIRED core base)
include(${wxWidgets_USE_FILE})
find_package(Threads)
find_package( ZLIB REQUIRED )

add_executable(${CMAKE_PROJECT_NAME} readjar.cpp
	${ZLIB_ROOT}/contrib/minizip/unzip.c
	${ZLIB_ROOT}/contrib/minizip/ioapi.c
)

target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC 
	${ZLIB_INCLUDE_DIRS} 
	"${ZLIB_ROOT}/contrib/minizip"
	${Boost_INCLUDE_DIR})

target_link_libraries (${CMAKE_PROJECT_NAME} 
	${wxWidgets_LIBRARIES}
	${ZLIB_LIBRARIES}
	${CMAKE_THREAD_LIBS_INIT})