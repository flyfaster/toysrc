project(tree)
cmake_minimum_required(VERSION 3.5)
include (CTest)
set(CMAKE_CXX_STANDARD 11) 

find_package(Boost REQUIRED COMPONENTS filesystem program_options system)
find_package(Threads)

find_package(wxWidgets COMPONENTS core base REQUIRED)
include(${wxWidgets_USE_FILE})
 
add_executable(bst bst.cpp basictreenode.cpp)

#add_executable(avl_test avl_test.cpp avl_tree.cpp basictreenode.cpp)

add_executable(rb_tree rb_tree.cpp basictreenode.cpp)

add_test(bst_test bst)
add_test(rb_tree_test rb_tree)

add_executable(rbtree2 rbtree2.cpp)