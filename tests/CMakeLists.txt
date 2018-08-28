cmrc_add_resource_library(hello hello.txt ALIAS rc::hello)
cmrc_add_resources(hello
    flower.jpg
    subdir_a/subdir_b/file_a.txt
    subdir_a/subdir_b/file_b.txt
    )

add_executable(simple-load simple.cpp)
add_test(simple-load simple-load)
set_property(TEST simple-load PROPERTY PASS_REGULAR_EXPRESSION "^Hello, world!")
target_link_libraries(simple-load PRIVATE rc::hello)

add_executable(flower flower.cpp)
target_link_libraries(flower PRIVATE rc::hello)
add_test(flower flower ${CMAKE_CURRENT_SOURCE_DIR}/flower.jpg)

