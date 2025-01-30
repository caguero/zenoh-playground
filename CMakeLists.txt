cmake_minimum_required(VERSION 3.16)
project(zenoh-playground LANGUAGES CXX)

if(CMAKE_BUILD_TYPE STREQUAL "")
  set(CMAKE_BUILD_TYPE Release)
endif()

# Find zenoh-cpp and its backend.
find_package(zenohc REQUIRED)
find_package(zenohcxx REQUIRED)

# Generate examples
add_executable(z_pub z_pub.cc)
target_link_libraries(z_pub PRIVATE zenohcxx::zenohc)
set_property(TARGET z_pub PROPERTY LANGUAGE CXX)
set_property(TARGET z_pub PROPERTY CXX_STANDARD 17)

add_executable(z_sub z_sub.cc)
target_link_libraries(z_sub PRIVATE zenohcxx::zenohc)
set_property(TARGET z_sub PROPERTY LANGUAGE CXX)
set_property(TARGET z_sub PROPERTY CXX_STANDARD 17)

add_executable(z_queriable z_queriable.cc)
target_link_libraries(z_queriable PRIVATE zenohcxx::zenohc)
set_property(TARGET z_queriable PROPERTY LANGUAGE CXX)
set_property(TARGET z_queriable PROPERTY CXX_STANDARD 17)

add_executable(z_get z_get.cc)
target_link_libraries(z_get PRIVATE zenohcxx::zenohc)
set_property(TARGET z_get PROPERTY LANGUAGE CXX)
set_property(TARGET z_get PROPERTY CXX_STANDARD 17)
