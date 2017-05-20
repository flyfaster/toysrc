cmake_minimum_required (VERSION 3.8)
project (miniqt)
set(CMAKE_CXX_STANDARD 11) 
set(CMAKE_AUTOMOC TRUE)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
find_package(Qt5 REQUIRED COMPONENTS core widgets gui)
include_directories(/usr/local/opt/qt5/include)
add_executable(${CMAKE_PROJECT_NAME} MainWnd.cpp)
target_link_libraries(${CMAKE_PROJECT_NAME} 
	/usr/local/opt/qt5/lib/QtWidgets.framework/Versions/5/QtWidgets 
	/usr/local/opt/qt5/lib/QtGui.framework/Versions/5/QtGui 
	/usr/local/opt/qt5/lib/QtCore.framework/Versions/5/QtCore)