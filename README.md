# CMakeRC - A Standalone CMake-Based C++ Resource Compiler

CMakeRC is a resource compiler provided in a single CMake script that can easily
be included in another project.

## What is a "Resource Compiler"?

For the purpose of this project, a _resource compiler_ is a tool that will
compile arbitrary data into a program. The program can then read this data from
without needing to store that data on disk external to the program.

Examples use cases:

- Storing a web page tree for serving over HTTP to clients. Compiling the web
  page into the executable means that the program is all that is required to run
  the HTTP server, without keeping the site files on disk separately.
- Storing embedded scripts and/or shaders that support the program, rather than
  writing them in the code as string literals.
- Storing images and graphics for GUIs.

These things are all about aiding in the ease of portability and distribution of
the program, as it is no longer required to ship a plethora of support files
with a binary to your users.

## What is Special About CMakeRC?

CMakeRC is implemented as a single CMake module, `CMakeRC.cmake`. No additional
libraries or headers are required.

This project was initially written as a "literate programming" experiment. [The process for the pre-2.0 version can be read about here](https://vector-of-bool.github.io/2017/01/21/cmrc.html).

2.0.0+ is slightly different from what was written in the post, but a lot of it
still applies.

## Installing

Installing CMakeRC is designed to be as simple as possible. The only thing
required is the `CMakeRC.cmake` script. You can copy it into your project
directory (recommended) or install it as a package and get all the features you
need.

For [vcpkg](https://github.com/microsoft/vcpkg) users there is a `cmakerc` [port](https://github.com/microsoft/vcpkg/tree/master/ports/cmakerc) that can be installed via `vcpkg install cmakerc` or by adding it to `dependencies` section of your `vcpkg.json` file.

The following sections will demonstrate how to use CMRC.

A complete example is available at the very end of the documentation if you are
impatient or want to take a sneak peek.

## Usage

The following steps outline how to use CMRC.

1. Once installed, simply import the `CMakeRC.cmake` script. If you placed the
   module in your project directory (recommended), simply use `include(CMakeRC)`
   to import the module. If you installed it as a package, use `find_package(CMakeRC)`.

2. Once included, create a new resource library using `cmrc_add_resource_library`,
   like this:

   ```cmake
   cmrc_add_resource_library(foo-resources ...)
   ```

   Where `...` is simply a list of files that you wish to compile into the
   resource library.

   You can use the `ALIAS` argument to immediately generate an alias target for
   the resource library (recommended):

   ```cmake
   cmrc_add_resource_library(foo-resources ALIAS foo::rc ...)
   ```

   **Note:** If the name of the library target is not a valid C++ `namespace`
   identifier, you will need to provide the `NAMESPACE` argument. Otherwise, the
   name of the library will be used as the resource library's namespace.

   ```cmake
   cmrc_add_resource_library(foo-resources ALIAS foo::rc NAMESPACE foo  ...)
   ```

   When the library is created you can at any time add additional resources:

   ```cmake
   cmrc_add_resources(foo-resources ...)
   ```


3. To use the resource library, link the resource library target into a binary
   using `target_link_libraries()`:

   ```cmake
   add_executable(my-program main.cpp)
   target_link_libraries(my-program PRIVATE foo::rc)
   ```

4. Inside of the source files, any time you wish to use the library, include the
   `cmrc/cmrc.hpp` header, which will automatically become available to any
   target that links to a generated resource library target, as `my-program`
   does above:

   ```c++
   #include <cmrc/cmrc.hpp>

   int main() {
       // ...
   }
   ```

5. At global scope within the `.cpp` file, place the `CMRC_DECLARE(<my-lib-ns>)` macro
   using the namespace that was designated with `cmrc_add_resource_library` (or
   the library name if no namespace was specified):

   ```c++
   #include <cmrc/cmrc.hpp>

   CMRC_DECLARE(foo);

   int main() {
       // ...
   }
   ```

6. Obtain a handle to the embedded resource filesystem by calling the
   `get_filesystem()` function in the generated namespace. It will be
   generated at `cmrc::<my-lib-ns>::get_filesystem()`.

   ```c++
   int main() {
       auto fs = cmrc::foo::get_filesystem();
   }
   ```

   (This function was declared by the `CMRC_DECLARE()` macro from the previous
   step.)

   You're now ready to work with the files in your resource library!
   See the section on `cmrc::embedded_filesystem`.

## The `cmrc::embedded_filesystem` API

All resource libraries have their own `cmrc::embedded_filesystem` that can be
accessed with the `get_filesystem()` function declared by `CMRC_DECLARE()`.

This class is trivially copyable and destructible, and acts as a handle to the
statically allocated resource library data.

### Methods on `cmrc::embedded_filesystem`

Filesystem inspection:

- `is_file(const std::string& path) -> bool` - Returns `true` if the given
  `path` names a regular file, `false` otherwise
- `is_directory(const std::string& path) -> bool` - Returns `true` if the given
  `path` names a directory. `false` otherwise
- `exists(const std::string& path) -> bool` - Returns `true` if the given path
  names an existing file or directory, `false` otherwise
- `iterate_directory(const std::string& path) -> cmrc::directory_iterator` -
  Returns a directory iterator for iterating the contents of a directory, or
  throws if the given `path` does not identify a directory
- `get_size(const std::string& path) -> std::size_t` - Returns the size in bytes
  of the file `path`, or throw `std::system_error()` on error

File access (these will throw `std::system_error()` if used on a directory):

- `open(const std::string& path) -> cmrc::file` - Opens `path` and returns a
  `file` object from where data can be read
- `get_as_string(const std::string& path) -> std::string` - Returns a copy of
  the data for `path` as a `std::string`
- `get_as_string_view(const std::string& path) -> std::string_view` - Returns
  the data for `path` as a `std::string_view` (no deep copy, requires C++17)
- `get_as_raw_ptr(const std::string& path) -> const char*` -
  Returns a raw `const char*` pointer to the the non-directory data at `path`
- `get_as_raw_ptr(const std::string& path, std::size_t& file_size) -> const char*` -
  Returns a raw `const char*` pointer to the non-directory data at `path` and
  updates the supplied `file_size` reference with the data size

If you want to use `get_as_string_view()` you need a C++17 (or newer) compliant
compiler. Example CMake configuration for this:

```cmake
target_compile_features(your_target PRIVATE cxx_std_17)
if(MSVC)
    # MSVC favors their own backwards compatibilty over standards compliance.
    # The below option ensures the __cplusplus pre-processing variable in MSVC
    # is actually standards compliant.
    target_compile_options(your_target PRIVATE "/Zc:__cplusplus")
endif()
```

## Members of `cmrc::file`

- `typename iterator` and `typename const_iterator` - Just `const char*`.
- `begin()/cbegin() -> iterator` - Return an iterator to the beginning of the
  resource.
- `end()/cend() -> iterator` - Return an iterator past the end of the resource.
- `file()` - Default constructor, refers to no resource.

## Members of `cmrc::directory_iterator`

- `typename value_type` - `cmrc::directory_entry`
- `iterator_category` - `std::input_iterator_tag`
- `directory_iterator()` - Default construct.
- `begin() -> directory_iterator` - Returns `*this`.
- `end() -> directory_iterator` - Returns a past-the-end iterator corresponding
  to this iterator.
- `operator*() -> value_type` - Returns the `directory_entry` for which the
  iterator corresponds.
- `operator==`, `operator!=`, and `operator++` - Implement iterator semantics.

## Members of `cmrc::directory_entry`

- `filename() -> std::string` - The filename of the entry.
- `is_file() -> bool` - `true` if the entry is a file.
- `is_directory() -> bool` - `true` if the entry is a directory.

## Additional Options

After calling `cmrc_add_resource_library`, you can add additional resources to
the library using `cmrc_add_resources` with the name of the library and the
paths to any additional resources that you wish to compile in. This way you can
lazily add resources to the library as your configure script runs.

Resources are always stored with an extra trailing `null` byte to facilitate
the direct use of data as C strings if needed. The `null` byte is not a part of
the actual data and does not count towards its size.

Both `cmrc_add_resource_library` and `cmrc_add_resources` take two additional
keyword parameters:

- `WHENCE` tells CMakeRC how to rewrite the filepaths to the resource files.
  The default value for `WHENCE` is the `CMAKE_CURRENT_SOURCE_DIR`, which is
  the source directory where `cmrc_add_resources` or `cmrc_add_resource_library`
  is called. For example, if you say `cmrc_add_resources(foo images/flower.jpg)`,
  the resource will be accessible via `cmrc::open("images/flower.jpg")`, but
  if you say `cmrc_add_resources(foo WHENCE images images/flower.jpg)`, then
  the resource will be accessible only using `cmrc::open("flower.jpg")`, because
  the `images` directory is used as the root where the resource will be compiled
  from.

  Because of the file transformation limitations, `WHENCE` is _required_ when
  adding resources which exist outside of the source directory, since CMakeRC
  will not be able to automatically rewrite the file paths.

- `PREFIX` tells CMakeRC to prepend a directory-style path to the resource
  filepath in the resulting binary. For example,
  `cmrc_add_resources(foo PREFIX resources images/flower.jpg)` will make the
  resource accessible using `cmrc::open("resources/images/flower.jpg")`. This is
  useful to prevent resource libraries from having conflicting filenames. The
  default `PREFIX` is to have no prefix.

The two options can be used together to rewrite the paths to your heart's
content:

```cmake
cmrc_add_resource_library(
    flower-images
    NAMESPACE flower
    WHENCE images
    PREFIX flowers
    images/rose.jpg
    images/tulip.jpg
    images/daisy.jpg
    images/sunflower.jpg
    )
```

This will result in the following files in the resource file system:

```
flowers/rose.jpg
flowers/tulip.jpg
flowers/daisy.jpg
flowers/sunflower.jpg
```

## A complete example

This complete example shows how to use the library and the different ways
resources can be accessed. Only text file resources are used in this
example, but binary data is of course supported using the same interface.

**Files needed:**
```cmake
CMakeLists.txt
example.cpp
CMakeRC.cmake  # Module file copied from the CMRC project
LICENSE.txt    # Added resource
README.txt     # Added resource
```

**CMakeLists.txt**

```cmake
# Require a decent version of CMake
cmake_minimum_required(VERSION 3.6)

# Include the CMRC module
include(./CMakeRC.cmake)

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
```

**example.cpp**

```c++
#include <cmrc/cmrc.hpp>
#include <iostream>

// Declare the resource library. Use the NAMESPACE specified in CMakeFile.txt.
CMRC_DECLARE(rc);

// Small helper to make output tidy
void header(const char *s) {
    std::cout << "\n\n---===[ " << s << " ]===---\n\n";
}

int main() {
    // Get the filesystem object from the specified namespace (under cmrc::)
    auto fs = cmrc::rc::get_filesystem();

    // Access the text file resource in all possible ways and display the contents each time

    header("open() returning iterable cmrc::file object");
    auto license_file = fs.open("LICENSE.txt");
    for(auto i = license_file.begin(); i != license_file.end(); ++i) {
        std::cout << *i;
    }
    std::cout << "\n";

    header("String (std::string), will return a copy of the data");
    auto license_string= fs.get_as_string("LICENSE.txt");
    std::cout << license_string << "\n";

    header("String view (std::string_view), read-only view, avoids a copy of the data");
    auto license_string_view = fs.get_as_string_view("LICENSE.txt");
    std::cout << license_string_view << "\n";

    header("String view (std::string_view) again, accessed using a raw pointer");
    auto license_string_view2 = fs.get_as_string_view("LICENSE.txt");
    auto license_string_view2_data = license_string_view2.data(); // const char*
    auto license_string_view2_size = license_string_view2.size(); // std::size_t
    std::cout << std::string(license_string_view2_data, license_string_view2_size) << "\n";

    header("Raw pointer (const char*), relying on CMRC implicit trailing NULL byte");
    auto license_raw_ptr = fs.get_as_raw_ptr("LICENSE.txt");
    std::cout << std::string(license_raw_ptr) << "\n"; // Trailing NULL byte needed!

    header("Raw pointer (const char*) with size return argument");
    std::size_t license_raw_ptr2_size;
    auto license_raw_ptr2 = fs.get_as_raw_ptr("LICENSE.txt", license_raw_ptr2_size);
    std::cout << std::string(license_raw_ptr2, license_raw_ptr2_size) << "\n";

    header("Raw pointer (const char*) and size (std::size_t)");
    auto license_raw_ptr3 = fs.get_as_raw_ptr("LICENSE.txt");
    auto license_raw_ptr3_size = fs.get_size("LICENSE.txt");
    std::cout << std::string(license_raw_ptr3, license_raw_ptr3_size) << "\n";

    return 0;
}
```
