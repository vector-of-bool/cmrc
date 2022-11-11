cmake_minimum_required(VERSION 3.6)
project(CMakeRC VERSION 2.0.0)

list(INSERT CMAKE_MODULE_PATH 0 "${PROJECT_SOURCE_DIR}/cmake")

# Include the main module
include(CMakeRC.cmake)

option(BUILD_TESTS "Build tests" ON)
if(BUILD_TESTS AND (PROJECT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR))
    enable_testing()
    if(NOT MSVC)
        add_compile_options(-Wall -Wextra)
    endif()
    add_subdirectory(tests)
endif()

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

if(${CMAKE_VERSION} VERSION_LESS "3.14")
    write_basic_package_version_file(${CMAKE_CURRENT_BINARY_DIR}/cmakerc-config-version.cmake
        COMPATIBILITY SameMajorVersion
    )
else()
    write_basic_package_version_file(${CMAKE_CURRENT_BINARY_DIR}/cmakerc-config-version.cmake
        COMPATIBILITY SameMajorVersion
        ARCH_INDEPENDENT
    )
endif()

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/cmakerc-config-version.cmake DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/cmake/cmakerc)
install(FILES CMakeRC.cmake DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/cmake/cmakerc RENAME cmakerc-config.cmake)
