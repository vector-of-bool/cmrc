# CMakeRC - A CMake-Based Resource Compiler

This projects provides a single CMake module, `CMakeRC.cmake` which can be used
to add compile arbitrary files into binaries, and then load and read from them
at runtime. [Learn more here.](https://vector-of-bool.github.io/2017/01/21/cmrc.html)

## Installing

Installing CMakeRC is meant to be as simple as possible. The only thing required
is the `CMakeRC.cmake` script. You can copy it into your project directory or
install it as a package and get all the features you need.

## Usage

1. Once installed, simply import the `CMakeRC.cmake` script. If you installed it
   as a package, use `find_package(CMakeRC)`. If you placed the module in your
   project directory, simply use `include(CMakeRC.cmake)` to import the module.

2. Once included, create a new resource library using `cmrc_add_resource_library`,
   like this:

   ```cmake
   cmrc_add_resource_library(my-resources ...)
   ```

   Where `...` is simply a list of files that you wish to compile into the
   resource library.

3. To use the resource library, link the resource library target into a binary
   using `target_link_libraries()`:

   ```cmake
   add_executable(my-program main.cpp)
   target_link_libraries(my-program PRIVATE my-resources)
   ```

4. Inside of the source files, any time you wish to use the library, include the
   `cmrc/cmrc.hpp` header, which will automatically become available to any
   target that linkes to a generated resource library target, as `my-program`
   does above:

   ```c++
   #include <cmrc/cmrc.hpp>

   int main() {
       // ...
   }
   ```

4. Before reading any of the compiled-in resources, call `CMRC_INIT()` with the
   identifier-ified name of the resource library. (The identifier-ified name is
   the name with all non-alphanumeric characters convered to underscores):

   ```c++
   int main() {
       CMRC_INIT(my_resources);
   }
   ```

5. Call `cmrc::open()` with the path to any compiled-in resources, and you will
   get an instance of `cmrc::resource` which exposes `begin()` and `end()`
   functions for accessing the data using simple `char` pointers.

6. Profit!

## Additional Options

After calling `cmrc_add_resource_library`, you can add additional resources to
the library using `cmrc_add_resources` with the name of the library and the
paths to any additional resources that you wish to compile in. This way you can
lazily add resources to the library as your configure script runs.

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
  WHENCE images
  PREFIX flowers
  images/rose.jpg
  images/tulip.jpg
  images/daisy.jpg
  images/sunflower.jpg
)
```

```c++
int foo() {
  auto rose = cmrc::open("flowers/rose.jpg");
}
```