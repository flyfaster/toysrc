project(udp)
cmake_minimum_required(VERSION 3.5)
include (CTest)
set(CMAKE_CXX_STANDARD 14) 
#set(CMAKE_BUILD_TYPE Debug)

if(APPLE)
# brew install nasm
add_compile_definitions(__WXMAC__)
elseif(UNIX)
add_compile_definitions(__WXGTK__)
elseif(MSVC)
set(CMAKE_BUILD_TYPE RelWithDebInfo)
set(CMAKE_GENERATOR_PLATFORM x64)
add_compile_definitions(WXUSINGDLL  _WIN32 _UNICODE)

set(BOOST_INCLUDEDIR "${_VCPKG_ROOT_DIR}/installed/x64-windows/include")
set(wxWidgets_LIB_DIR "${_VCPKG_ROOT_DIR}/packages/wxwidgets_x64-windows/lib")
set(wxWidgets_INCLUDE_DIRS 
	"${_VCPKG_ROOT_DIR}/packages/wxwidgets_x64-windows/include/msvc" 
	"${_VCPKG_ROOT_DIR}/packages/wxwidgets_x64-windows/include")
message("_VCPKG_ROOT_DIR = ${_VCPKG_ROOT_DIR}")
endif()

function(add_SUBSYSTEM_ldflags _target)
    if(MSVC)
        set(new_link_flags "/SUBSYSTEM:WINDOWS")
        get_target_property(existing_link_flags ${_target} LINK_FLAGS)
        if(existing_link_flags)
            set(new_link_flags "${existing_link_flags} ${new_link_flags}")
        endif()
        set_target_properties(${_target} PROPERTIES LINK_FLAGS ${new_link_flags})
    endif()
endfunction()

find_package(Boost REQUIRED COMPONENTS filesystem program_options unit_test_framework regex system)
find_package(Threads)
find_package(wxWidgets COMPONENTS core base REQUIRED)
include(${wxWidgets_USE_FILE})

if(CYGWIN)
add_compile_definitions(__CYGWIN__)
endif()

add_executable(udp  udp.cpp)

target_link_libraries (udp 
#	dl
	${CMAKE_THREAD_LIBS_INIT})

add_executable(udp_server  udp_server.cpp)

target_link_libraries (udp_server
	Boost::regex Boost::program_options Boost::filesystem 
	Boost::system 
	${wxWidgets_LIBRARIES}
#	dl
	${CMAKE_THREAD_LIBS_INIT})
	
add_SUBSYSTEM_ldflags(udp_server)

