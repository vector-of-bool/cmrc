function(cmrc_add_test)
    set(options)
    set(args NAME PASS_REGEX WHENCE PREFIX)
    set(list_args RESOURCES TEST_ARGV)
    cmake_parse_arguments(PARSE_ARGV 0 ARG "${options}" "${args}" "${list_args}")

    if(ARG_WHENCE)
        set(whence_arg WHENCE "${ARG_WHENCE}")
    endif()
    if(ARG_PREFIX)
        set(prefix_arg PREFIX "${ARG_PREFIX}")
    endif()

    add_executable("${ARG_NAME}" "${ARG_NAME}.cpp")
    cmrc_add_resource_library(rc_${ARG_NAME} NAMESPACE "${ARG_NAME}" ${whence_arg} ${prefix_arg} ${ARG_RESOURCES})
    target_link_libraries("${ARG_NAME}" PRIVATE rc_${ARG_NAME})
    add_test("${ARG_NAME}" "${ARG_NAME}" ${ARG_TEST_ARGV})
    if(ARG_PASS_REGEX)
        set_property(
            TEST "${ARG_NAME}"
            PROPERTY PASS_REGULAR_EXPRESSION "${ARG_PASS_REGEX}"
            )
    endif()
    if(ARG_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR "Invalid test arguments: ${ARG_UNPARSED_ARGUMENTS}")
    endif()
endfunction()

cmrc_add_test(
    NAME simple
    PASS_REGEX "^Hello, world!"
    RESOURCES
        hello.txt
        # Resources not used by the test, but just make sure we get no compilation
        # errors with the more complex paths:
        subdir_a/subdir_b/file_a.txt
        subdir_a/subdir_b/file_b.txt
    )

cmrc_add_test(
    NAME flower
    RESOURCES flower.jpg
    TEST_ARGV "${CMAKE_CURRENT_SOURCE_DIR}/flower.jpg"
    )

cmrc_add_test(
    NAME prefix
    PASS_REGEX "^Hello, world!"
    RESOURCES hello.txt
    PREFIX some-prefix
    )

cmrc_add_test(
    NAME whence
    PASS_REGEX "^I am a file!"
    RESOURCES subdir_a/subdir_b/file_a.txt
    WHENCE subdir_a
    )

cmrc_add_test(
    NAME whence_prefix
    PASS_REGEX "^I am a file!"
    RESOURCES subdir_a/subdir_b/file_b.txt
    WHENCE subdir_a
    PREFIX imaginary-prefix/
    )

cmrc_add_test(
    NAME iterate
    PASS_REGEX "^file_a.txt\nfile_b.txt\n$"
    RESOURCES
        subdir_a/subdir_b/file_a.txt
        subdir_a/subdir_b/file_b.txt
    )
