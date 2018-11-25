project(wx)
cmake_minimum_required(VERSION 3.5)
set(CMAKE_CXX_STANDARD 14) 

if(APPLE)
# brew install nasm
add_compile_definitions(__WXMAC__)
elseif(UNIX)
add_compile_definitions(__WXGTK__)
elseif(MSVC)
set(CMAKE_BUILD_TYPE RelWithDebInfo)
set(CMAKE_GENERATOR_PLATFORM x64)
add_compile_definitions(WXUSINGDLL  _WIN32 _UNICODE)
# Windows (/SUBSYSTEM:WINDOWS) _MBCS __WXUNIVERSAL__ 
# Don't combine /SUBSYSTEM:WINDOWS with /ENTRY:mainCRTStartup
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS ")
set(BOOST_INCLUDEDIR "${_VCPKG_ROOT_DIR}/installed/x64-windows/include")
set(wxWidgets_LIB_DIR "${_VCPKG_ROOT_DIR}/packages/wxwidgets_x64-windows/lib")
set(wxWidgets_INCLUDE_DIRS 
	"${_VCPKG_ROOT_DIR}/packages/wxwidgets_x64-windows/include/msvc" 
	"${_VCPKG_ROOT_DIR}/packages/wxwidgets_x64-windows/include")

message("_VCPKG_ROOT_DIR = ${_VCPKG_ROOT_DIR}")
endif()

find_package(Boost REQUIRED COMPONENTS filesystem program_options system)
find_package(Threads)

find_package(wxWidgets COMPONENTS core aui adv base REQUIRED)
include(${wxWidgets_USE_FILE})
 
add_executable(hello hello.cpp)
target_link_libraries(hello ${wxWidgets_LIBRARIES} 
	${CMAKE_THREAD_LIBS_INIT})

add_executable(miniwx miniwx.cpp)
target_link_libraries(miniwx ${wxWidgets_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
	
# cmake -DBOOST_ROOT=/usr/local/Cellar/boost/1.65.1 ..
