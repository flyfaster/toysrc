project(storesym)

set(CMAKE_BUILD_TYPE Debug)

add_executable(storesym main.cpp)

target_link_libraries (storesym boost_filesystem boost_system)