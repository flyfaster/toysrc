cmake_minimum_required(VERSION 2.8)
project(taosimple)

set(ACE_ROOT /home/onega/3rdlib/ACE_wrappers)
include_directories(${ACE_ROOT} ${ACE_ROOT}/TAO ${ACE_ROOT}/TAO/orbsvcs ${CMAKE_CURRENT_SOURCE_DIR}/build)
link_directories (${ACE_ROOT}/lib) 

list(APPEND IDL_COMPILED_FILES taosimpleC.h taosimpleC.cpp taosimpleS.h taosimpleS.cpp taosimpleC.inl)
add_custom_command(OUTPUT ${IDL_COMPILED_FILES}
		  COMMAND LD_LIBRARY_PATH=${ACE_ROOT}/lib ${ACE_ROOT}/bin/tao_idl -GC ${CMAKE_CURRENT_SOURCE_DIR}/taosimple.idl
		  COMMENT "Compiling taosimple.idl")

set_source_files_properties(${IDL_COMPILED_FILES} PROPERTIES GENERATED TRUE)		  
	  
add_executable(taosimple main.cpp ${IDL_COMPILED_FILES})
target_link_libraries (taosimple  TAO_Messaging TAO_Valuetype TAO_PI TAO_CodecFactory TAO_PortableServer TAO_IORTable TAO_CosNaming TAO_AnyTypeCode TAO_Utils TAO ACE pthread dl)	

add_executable(client client.cpp ${IDL_COMPILED_FILES})
target_link_libraries (client TAO_Messaging TAO_Valuetype TAO_PI TAO_CodecFactory TAO_PortableServer TAO_IORTable TAO_CosNaming TAO_AnyTypeCode TAO_Utils TAO ACE pthread dl)

add_executable(clientdebug clientdebug.cpp ${IDL_COMPILED_FILES})
target_link_libraries (clientdebug TAO_Messaging TAO_Valuetype TAO_PI TAO_CodecFactory TAO_PortableServer TAO_IORTable TAO_CosNaming TAO_AnyTypeCode TAO_Utils TAO ACE pthread dl)	

add_executable(amiclient amiclient.cpp ${IDL_COMPILED_FILES})
target_link_libraries (amiclient TAO_Messaging TAO_PI TAO_CodecFactory TAO_Valuetype TAO_PortableServer TAO_CosNaming TAO_AnyTypeCode TAO ACE  dl rt)	
