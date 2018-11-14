project(wxpinyin)
cmake_minimum_required(VERSION 3.5)
include (CTest)
set(CMAKE_CXX_STANDARD 14) 
set(CMAKE_BUILD_TYPE Debug)

find_package(Boost REQUIRED COMPONENTS filesystem program_options unit_test_framework regex system)
find_package(Threads)
find_package(wxWidgets COMPONENTS core base REQUIRED)
include(${wxWidgets_USE_FILE})

if(APPLE)
add_compile_definitions(__WXMAC__)
elseif(UNIX)
add_compile_definitions(__WXGTK__)
endif()

SET(CUSTOM_OBJECT
  ${CMAKE_CURRENT_BINARY_DIR}/dict${CMAKE_C_OUTPUT_EXTENSION})
  
SET_SOURCE_FILES_PROPERTIES(
  ${CUSTOM_OBJECT}
  PROPERTIES
  EXTERNAL_OBJECT true
  GENERATED true
)

message(STATUS "${CUSTOM_OBJECT}")
message(STATUS "${CMAKE_SOURCE_DIR}")
message(STATUS "${CMAKE_CURRENT_BINARY_DIR}")

add_custom_command(
      OUTPUT ${CUSTOM_OBJECT}
      COMMAND /usr/local/bin/nasm -fmacho64 ${CMAKE_SOURCE_DIR}/dict.asm -o ${CUSTOM_OBJECT}
      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
      DEPENDS ${CMAKE_SOURCE_DIR}/dict.asm ${CMAKE_SOURCE_DIR}/dict.txt
      COMMENT "Generate binary file for dictionary: nasm -fmacho64 dict.asm -o bld/dict.o"
      )
      
add_executable(wxpinyin main.cpp wxfront.cpp ${CUSTOM_OBJECT})

target_link_libraries (wxpinyin 
	Boost::regex Boost::program_options Boost::filesystem 
	Boost::system 
	${wxWidgets_LIBRARIES}
	dl
	${CMAKE_THREAD_LIBS_INIT})