#include <cmrc/cmrc.hpp>

#include <iostream>

CMRC_DECLARE(simple_raw_ptr);

int main() {
    auto fs = cmrc::simple_raw_ptr::get_filesystem();
    const char* data = fs.get_as_raw_ptr("hello.txt");
    // This kind of use relies on the implicit NULL byte added by
    // CMRC as well as that the file has no NULL bytes inside it.
    std::cout << std::string(data) << '\n';
}
