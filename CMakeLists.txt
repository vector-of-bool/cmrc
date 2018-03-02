cmake_minimum_required(VERSION 3.6)
project(CMRCExample)

include(CMakeRC.cmake)

cmrc_add_resource_library(hello hello.txt)
add_executable(main main.cpp)
target_link_libraries(main PRIVATE hello)

include(CTest)
enable_testing()
add_test(main main)
set_property(TEST main PROPERTY PASS_REGULAR_EXPRESSION "Hello, world!")

add_executable(flower flower.cpp)
target_link_libraries(flower PRIVATE hello)
add_test(flower flower ${CMAKE_CURRENT_SOURCE_DIR}/flower.jpg)
cmrc_add_resources(hello flower.jpg)
