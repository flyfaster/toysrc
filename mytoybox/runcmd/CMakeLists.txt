project(runcmd)
cmake_minimum_required(VERSION 3.5)
include (CTest)
set(CMAKE_CXX_STANDARD 14) 
set(CMAKE_INCLUDE_CURRENT_DIR ON)
#set(CMAKE_BUILD_TYPE Debug)

if(MSVC)
set(CMAKE_BUILD_TYPE RelWithDebInfo)
set(CMAKE_GENERATOR_PLATFORM x64)
add_compile_definitions(WXUSINGDLL  _WIN32 _UNICODE _CRT_SECURE_NO_WARNINGS)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS")
set(BOOST_INCLUDEDIR "${_VCPKG_ROOT_DIR}/installed/x64-windows/include")
set(wxWidgets_LIB_DIR "${_VCPKG_ROOT_DIR}/packages/wxwidgets_x64-windows/lib")
set(wxWidgets_INCLUDE_DIRS 
	"${_VCPKG_ROOT_DIR}/packages/wxwidgets_x64-windows/include/msvc" 
	"${_VCPKG_ROOT_DIR}/packages/wxwidgets_x64-windows/include")
message("_VCPKG_ROOT_DIR = ${_VCPKG_ROOT_DIR}")

set(PROTO_FILES
  "${CMAKE_CURRENT_BINARY_DIR}/runcmd.pb.cc"
)

add_custom_command(OUTPUT ${PROTO_FILES}
  COMMAND ${_VCPKG_ROOT_DIR}/packages/protobuf_x64-windows/tools/protobuf/protoc.exe --cpp_out . -I .. runcmd.proto
  WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
  DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/runcmd.proto"
  COMMENT "Compile protocol buffers file"
  VERBATIM
)

endif()

find_package(Boost REQUIRED COMPONENTS filesystem program_options unit_test_framework regex system)
find_package(Threads)
find_package(wxWidgets COMPONENTS core base REQUIRED)
include(${wxWidgets_USE_FILE})
find_package(protobuf CONFIG REQUIRED)
#    target_link_libraries(main PRIVATE protobuf::libprotoc protobuf::libprotobuf)
    
add_executable(runcmd  runcmd.cpp frame.cpp ${PROTO_FILES})

target_link_libraries (runcmd PRIVATE
	Boost::regex Boost::program_options Boost::filesystem 
	Boost::system 
	${wxWidgets_LIBRARIES}
	protobuf::libprotoc protobuf::libprotobuf
#	dl
	${CMAKE_THREAD_LIBS_INIT})