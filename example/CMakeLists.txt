# Require a decent version of CMake
cmake_minimum_required(VERSION 3.6)

# Include the CMRC module
include(../CMakeRC.cmake)

# Define the example project
project(cmrc_example)

# Add an executable to the project
add_executable(cmrc_example example.cpp)

# In this case we would like to use get_as_string_view() which requries C++17.
# If this is not needed the following lines can be skipped.
target_compile_features(cmrc_example PRIVATE cxx_std_17)
if(MSVC)
    # MSVC favors their own backwards compatibilty over standards compliance.
    # The below option ensures the __cplusplus pre-processing variable in MSVC
    # is actually standards compliant.
    target_compile_options(cmrc_example PRIVATE "/Zc:__cplusplus")
endif()

# Create a "resource library" called "myrclib" which will hold the resources.
# We also create a CMake ALIAS to the library which can be used when linking.
# Finally we choose to add a text file as a first resource.
cmrc_add_resource_library(
    myrclib
    NAMESPACE rc
    ALIAS cmrc_example::rc

    LICENSE.txt
)

# Add more resource files to the library
cmrc_add_resources(myrclib README.txt)

# Add the resource library using the library ALIAS
target_link_libraries(cmrc_example PRIVATE cmrc_example::rc)

# ... or using the library name if preferred
#target_link_libraries(cmrc_example PRIVATE myrclib)
