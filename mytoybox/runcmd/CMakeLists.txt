project(runcmd)
cmake_minimum_required(VERSION 3.5)
include (CTest)
set(CMAKE_CXX_STANDARD 14) 
set(CMAKE_BUILD_TYPE Debug)

if(MSVC)
set(CMAKE_BUILD_TYPE RelWithDebInfo)
set(CMAKE_GENERATOR_PLATFORM x64)
add_compile_definitions(WXUSINGDLL  _WIN32 _UNICODE)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS")
set(BOOST_INCLUDEDIR "${_VCPKG_ROOT_DIR}/installed/x64-windows/include")
set(wxWidgets_LIB_DIR "${_VCPKG_ROOT_DIR}/packages/wxwidgets_x64-windows/lib")
set(wxWidgets_INCLUDE_DIRS 
	"${_VCPKG_ROOT_DIR}/packages/wxwidgets_x64-windows/include/msvc" 
	"${_VCPKG_ROOT_DIR}/packages/wxwidgets_x64-windows/include")
message("_VCPKG_ROOT_DIR = ${_VCPKG_ROOT_DIR}")
endif()

find_package(Boost REQUIRED COMPONENTS filesystem program_options unit_test_framework regex system)
find_package(Threads)
find_package(wxWidgets COMPONENTS core base REQUIRED)
include(${wxWidgets_USE_FILE})

add_executable(runcmd  runcmd.cpp frame.cpp)

target_link_libraries (runcmd
	Boost::regex Boost::program_options Boost::filesystem 
	Boost::system 
	${wxWidgets_LIBRARIES}
#	dl
	${CMAKE_THREAD_LIBS_INIT})