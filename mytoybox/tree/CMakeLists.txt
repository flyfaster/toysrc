project(tree)
cmake_minimum_required(VERSION 3.5)
include (CTest)
set(CMAKE_CXX_STANDARD 17) 
set(CMAKE_BUILD_TYPE Debug)

find_package(Boost REQUIRED COMPONENTS filesystem program_options unit_test_framework regex system)
find_package(Threads)

find_package(wxWidgets COMPONENTS core base REQUIRED)
include(${wxWidgets_USE_FILE})
 
add_executable(bst bst.cpp basictreenode.cpp)
target_link_libraries (bst 
	Boost::regex Boost::program_options Boost::filesystem Boost::system Boost::unit_test_framework
	${CMAKE_THREAD_LIBS_INIT})
#add_executable(avl_test avl_test.cpp avl_tree.cpp basictreenode.cpp)

add_executable(rb_tree rb_tree.cpp basictreenode.cpp)

add_test(bst_test bst)
add_test(rb_tree_test rb_tree)

add_executable(rbtree2 rbtree2.cpp)

add_executable(rbtree3 rbtree3.cpp)

add_executable(suffix_array suffix_array.cpp)
target_link_libraries (suffix_array 
	Boost::regex Boost::program_options Boost::filesystem Boost::system Boost::unit_test_framework
	${CMAKE_THREAD_LIBS_INIT})
	
add_executable(segment_tree segment_tree.cpp)
target_link_libraries (segment_tree 
	Boost::regex Boost::program_options Boost::filesystem Boost::system Boost::unit_test_framework
	${CMAKE_THREAD_LIBS_INIT})
	
add_executable(fenwick_tree fenwick_tree.cpp)
target_link_libraries (fenwick_tree 
	Boost::regex Boost::program_options Boost::filesystem Boost::system Boost::unit_test_framework
	${CMAKE_THREAD_LIBS_INIT})

add_executable(cartesian_tree cartesian_tree.cpp)
target_link_libraries (cartesian_tree 
	Boost::regex Boost::program_options Boost::filesystem Boost::system Boost::unit_test_framework
	${CMAKE_THREAD_LIBS_INIT})
	
add_executable(construct_bst_from_preorder construct_bst_from_preorder.cpp)
target_link_libraries (construct_bst_from_preorder 
	Boost::regex Boost::program_options Boost::filesystem Boost::system Boost::unit_test_framework
	${CMAKE_THREAD_LIBS_INIT})