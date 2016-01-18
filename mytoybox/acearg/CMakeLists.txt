cmake_minimum_required (VERSION 2.8)

set(CMAKE_VERBOSE_MAKEFILE ON)
project(acearg)
set(FAKE_TARGET fakeTarget)
add_custom_target(fakeTarget
#cd ${PROJECT_SOURCE_DIR} && objcopy --input binary --output elf32-i386 --binary-architecture i386 main.cpp ${CMAKE_CURRENT_BINARY_DIR}/mydata.o
cd ${PROJECT_SOURCE_DIR} && objcopy --input binary --output elf64-x86-64 --binary-architecture i386 main.cpp ${CMAKE_CURRENT_BINARY_DIR}/mydata.o
)


SET_SOURCE_FILES_PROPERTIES(mydata.o 
  PROPERTIES 
  EXTERNAL_OBJECT true
  GENERATED true
  )

set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES ${CMAKE_CURRENT_BINARY_DIR}/mydata.o)

set(ACE_ROOT /home/onega/src/ACE_wrappers)
include_directories(${ACE_ROOT})
link_directories (${ACE_ROOT}/lib  ${CMAKE_CURRENT_BINARY_DIR}) 
add_executable(acearg main.cpp)
target_link_libraries (acearg ${CMAKE_CURRENT_BINARY_DIR}/mydata.o ACE pthread dl )

add_dependencies(acearg ${FAKE_TARGET})

get_directory_property(OUT_VAR LINK_DIRECTORIES)
message(STATUS "DIR: ${OUT_VAR}")
message(STATUS "PROJECT_BINARY_DIR: ${PROJECT_BINARY_DIR}")
message(STATUS "CMAKE_CURRENT_BINARY_DIR: ${CMAKE_CURRENT_BINARY_DIR}")
message(STATUS "CMAKE_FILES_DIRECTORY: ${CMAKE_FILES_DIRECTORY}")
message(STATUS "CMAKE_BINARY_DIR: ${CMAKE_BINARY_DIR}")
