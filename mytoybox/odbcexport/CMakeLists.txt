cmake_minimum_required(VERSION 2.8)
PROJECT(odbcexport)

set(wxWidgets_USE_DEBUG, FALSE)
set(wxWidgets_USE_UNICODE, TRUE)

#add_definitions(-D__WXGTK__ -DSQL_WCHART_CONVERT -DUNICODE -D_UNICODE)
add_definitions(-D__WXGTK__  -DUNICODE -D_UNICODE -DwxUSE_UNICODE=1)

SET(wxWidgets_ROOT_DIR "/usr/include/wx-2.8")
SET(wxWidgets_CONFIGURATION gtk2-unicode-release-2.8) 
SET(WXWIDGETS_LIBRARIES        "/usr/lib")

find_package(wxWidgets COMPONENTS core base REQUIRED)


set(wxWidget_DEFINITIONS "${wxWidgets_DEFINITIONS} -DLINUX=1")
#add_definitions(__WXGTK__=1)

#CXX_DEFINES = -D_FILE_OFFSET_BITS=64 -D_LARGE_FILES -D__WXGTK__ -DNDEBUG  __WXGTK__=1

ADD_EXECUTABLE(odbcexport src/strtypes.cpp src/mainwnd.cpp src/odbcexportapp.cpp src/otable_archive.cpp src/itable_archive.cpp src/column.cpp src/odbc_helper.cpp main.cpp)

target_link_libraries(odbcexport odbc  boost_date_time  boost_program_options boost_serialization wx_baseu-2.8 wx_gtk2u_core-2.8)

## Compiler flags
if(CMAKE_COMPILER_IS_GNUCXX)   
    list(APPEND CMAKE_EXE_LINKER_FLAGS -v)
    list(APPEND CMAKE_CXX_FLAGS -v)
endif()
INCLUDE_DIRECTORIES(/usr/include/wx-2.8   /usr/lib/wx/include/gtk2-unicode-release-2.8)
